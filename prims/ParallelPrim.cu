#include<stdio.h>
#include<math.h>
#include <fstream>
#include <iostream>

using namespace std;

#define THREADS_PER_BLOCK 1024
#define NUMBER_OF_BLOCKS 1024

#define DEBUG 0

//initially, 6 & 9

// number of threads_per_block*blocks should be atleast V-1.
// threads per block should be greater than or equal to number_of_blocks and both should be power of 2.
// only bcz of the restriction of cuda kernel two.


// if the vertices begin with 1 not 0, be careful, got to map it accordingly.
//destinationVertexArray decrease the value of indices by 1, and when result is out, increase the indices by 1.
//R2 array will also begin with 2 in that case, as the R1 initialization would be 1.

void initializeGraph(int *indexArray , int *destinationVertexArray , int *weightArray, int edges, int vertices)
{

	// to be read from file or generators for now make static.

	
	int u = 1,v,w;
	int count = 0;
	int prev = 1;
	for(int i = 0 ; i < edges ; i++) 
	{
		prev = u;
       	cin >>u>>v>>w;
	   	if(prev == u)
	   	{
			count++;
		}	
	   	else
	   	{
		   indexArray[prev] = count + indexArray[prev-1];
		   count = 1;
	   	}	
	   	destinationVertexArray[i] = v-1;
	   	weightArray[i] = w;
    }

}




// R1 source, R2 destination,R3 weight
void initMSTEdgeList(int *R1 , int *R2 , int *R3, int *indexArray , int *destinationVertexArray, int *weightArray, int vertices)
{
	for(int v = indexArray[0] ; v < indexArray[1] ; v++)
	{
		// update weight of adjacent vertices of vertex 0. -1 bcz counting at r2 starts from 1 i.e at index 0 vertex 1 is placed.
		// So correspondingly for R3.
		R3[destinationVertexArray[v]-1] = weightArray[v];
	}	

	for(int i = 0;i< vertices-1;i++)
	{
		R2[i] = i+1;
		R1[i] = 0;
	}
}


void printArray(int *a  , int size)
{
    for(int i = 0; i < size ; i++)
        printf("%d\t",a[i]);
    printf("\n");
}


__global__ void updateMSTList(int startIndex,int endIndex, int *d_nearestVertex, int *destinationVertexArray,int *weightArray,int *d_R1,int *d_R2,int *d_R3)
{
	int threadId = blockIdx.x * THREADS_PER_BLOCK + threadIdx.x;
	int offset = startIndex;
	
	if((threadId+offset) >= startIndex && (threadId+offset) <= endIndex)
	{
		int destVertex = destinationVertexArray[threadId+offset];
		if(destVertex != 0)
		{
			if(d_R2[destVertex-1] != 0)
			{
				if(d_R3[destVertex-1] > weightArray[threadId+offset] )
				{
					d_R3[destVertex-1] = weightArray[threadId+offset];
					d_R1[destVertex-1] = *d_nearestVertex;
				}
			}
		}
	}
}

__global__ void findNearestVertexKernelOne( int *d_R2 , int *d_R3, int *T1, int *T2, int *d_nearestVertex,int secondKernelRequired,int vertices)
{
	int blockNo = blockIdx.x;
	int threadId =  blockNo * THREADS_PER_BLOCK + threadIdx.x;

	__shared__ int	r2[THREADS_PER_BLOCK],r3[THREADS_PER_BLOCK];	
  
	r2[threadIdx.x] = 0;	
	r3[threadIdx.x] = 10000;	

	if(threadId < vertices-1)
	{
		r2[threadIdx.x] = d_R2[threadId];
		r3[threadIdx.x] = d_R3[threadId];
	
	__syncthreads();
	
	
	int i = 0;
	for(i = THREADS_PER_BLOCK/2; i > 0 ; i = i>>1)
	{
		if(threadIdx.x < i)
		{
			if(r2[threadIdx.x] != 0 && r2[i + threadIdx.x]!=0)
			{
				// we also want to store the indices of the minimum location, so that we can get the desired vertex.
				if(r3[threadIdx.x] > r3[i+threadIdx.x])
				{
					//update index.
					r2[threadIdx.x] = r2[i+threadIdx.x];
					//update weight.
					r3[threadIdx.x] = r3[i+threadIdx.x];
				}	
				//else its the same.
			}
			else if(r2[threadIdx.x] == 0 && r2[i+threadIdx.x] == 0)
			{
				// do nothing
			}
			else if(r2[threadIdx.x] == 0 && r2[i+threadIdx.x] != 0)
			{
				r2[threadIdx.x] = r2[i+threadIdx.x];
				r3[threadIdx.x] = r3[i+threadIdx.x];
			}
			// else same present value needs to be considered in the 4th case.
		}
		__syncthreads();
				
	}	
		// only one value needed, first thread will write.
		if(threadIdx.x == 0)
		{
			if(!secondKernelRequired)
			{
				// update the nearest_Vertex
				*d_nearestVertex = r2[0];
			    // mark vertex as used..
			 	 d_R2[r2[0]-1] = 0;
			}
			// for multiple blocks, because they are in the shared memory, set T1 T2 here which is index and weight for second kernel..		
			else
			{
				T1[blockIdx.x] = r2[0];
				T2[blockIdx.x] = r3[0];
			}	
		}
			
	}


}


__global__ void findNearestVertexKernelTwo(int *d_R1 , int *d_R2 , int *d_R3 , int *d_T1, int *d_T2, int *d_nearestVertex)
{

	// no need to use the shared memory, only one block.	
	
    for(int i = NUMBER_OF_BLOCKS/2; i > 0 ; i = i>>1)
    {
        if(threadIdx.x < i)
        {
            if(d_T1[threadIdx.x] != 0 && d_T1[i + threadIdx.x]!=0)
            {
                // we also want to store the indices of the minimum location, so that we can get the desired vertex.
                if(d_T2[threadIdx.x] > d_T2[i+threadIdx.x])
                {
                    //update index.
                    d_T1[threadIdx.x] = d_T1[i+threadIdx.x];
                    //update weight.
                    d_T2[threadIdx.x] = d_T2[i+threadIdx.x];
                }
                //else its the same.
            }
            else if(d_T1[threadIdx.x] == 0 && d_T1[i+threadIdx.x] == 0)
            {
            	// do nothing
			}
            else if(d_T1[threadIdx.x] == 0 && d_T1[i+threadIdx.x] != 0)
            {
                d_T1[threadIdx.x] = d_T1[i+threadIdx.x];
                d_T2[threadIdx.x] = d_T2[i+threadIdx.x];
            }
            // else same present value needs to be considered in the 4th case.
        }
        __syncthreads();

    }

	if(threadIdx.x == 0)
    {
		*d_nearestVertex = d_T1[0];
         d_R2[d_T1[0]-1] = 0;
    }

}


int main()
{
	// graph representatioon arrays
	int *h_indexArray;
	int *h_weightArray;
	int *h_destinationVertexArray;

	int secondKernelRequired = 0;

	// device Graph	
	int *d_indexArray;
	int *d_weightArray;
	int *d_destinationVertexArray;


	// MST Edge list
	int *h_R1;
	int *h_R2;
	int *h_R3;
	int *h_nearestVertex;
   	int *h_T1;
	int *h_T2;
 
	// device MST list	
	int *d_R1;
	int *d_R2;
	int *d_R3;
	int *d_nearestVertex;
   	int *d_T1;
	int *d_T2;


	int edges,vertices;
    cin>>vertices>>edges;	

	cout << vertices << edges<< endl;
	edges = edges/2;		
	// iteration indexes for updation
	int startIndex, endIndex;
	
	// allocate memory to compact adjacency list

	h_indexArray = (int *)malloc(vertices * sizeof(int));
    h_weightArray = (int *)malloc(2 * edges * sizeof(int));
    h_destinationVertexArray = (int *)malloc(2 * edges * sizeof(int));
	h_nearestVertex = (int *)malloc(sizeof(int));
	
	memset(h_indexArray,0,vertices * sizeof(int));
	memset(h_weightArray,0,2 * edges * sizeof(int));
	memset(h_destinationVertexArray,0,2 * edges * sizeof(int));
	*h_nearestVertex = 0;
	
	h_R1 = (int *)malloc((vertices-1) * sizeof(int));
	h_R2 = (int *)malloc((vertices-1) * sizeof(int));
	h_R3 = (int *)malloc((vertices-1) * sizeof(int));

	h_T1 = (int *)malloc(NUMBER_OF_BLOCKS * sizeof(int));
	h_T2 = (int *)malloc(NUMBER_OF_BLOCKS * sizeof(int));
	
	memset(h_R3,1,(vertices-1)*sizeof(int));
	memset(h_T1,0,NUMBER_OF_BLOCKS * sizeof(int));
	memset(h_T2,0,NUMBER_OF_BLOCKS * sizeof(int));

	initializeGraph(h_indexArray,h_destinationVertexArray,h_weightArray,2*edges,vertices);


	
	initMSTEdgeList(h_R1,h_R2,h_R3,h_indexArray,h_destinationVertexArray,h_weightArray,vertices);	


	// cuda memory allocation of graph representation
	cudaMalloc(&d_indexArray,(vertices) * sizeof(int));
	cudaMalloc(&d_weightArray,2 * edges * sizeof(int));
	cudaMalloc(&d_destinationVertexArray,2 * edges * sizeof(int));
	cudaMalloc(&d_nearestVertex,sizeof(int));

	// cuda memory allocation of MST list
	cudaMalloc(&d_R1,(vertices-1) * sizeof(int));
	cudaMalloc(&d_R2,(vertices-1) * sizeof(int));
	cudaMalloc(&d_R3,(vertices-1) * sizeof(int));
	cudaMalloc(&d_T1, NUMBER_OF_BLOCKS * sizeof(int));
	cudaMalloc(&d_T2, NUMBER_OF_BLOCKS * sizeof(int));
	
	// start time
	 cudaEvent_t start, stop;
	 float time;
     cudaEventCreate(&start);
     cudaEventCreate(&stop);
     cudaEventRecord( start, 0 );

	// copy host to device graph and initial MST list
		
	cudaMemcpy(d_indexArray,h_indexArray, vertices* sizeof(int),cudaMemcpyHostToDevice);
	cudaMemcpy(d_destinationVertexArray,h_destinationVertexArray, 2 * edges * sizeof(int),cudaMemcpyHostToDevice);
	cudaMemcpy(d_weightArray,h_weightArray,2 * edges * sizeof(int),cudaMemcpyHostToDevice);
	cudaMemcpy(d_nearestVertex,h_nearestVertex,sizeof(int),cudaMemcpyHostToDevice);
		
	cudaMemcpy(d_R1,h_R1, (vertices - 1)* sizeof(int),cudaMemcpyHostToDevice);
	cudaMemcpy(d_R2,h_R2, (vertices - 1)* sizeof(int),cudaMemcpyHostToDevice);
	cudaMemcpy(d_R3,h_R3, (vertices - 1)* sizeof(int),cudaMemcpyHostToDevice);


	// MEMSET T1 AND T2
	cudaMemset(d_T1,0,NUMBER_OF_BLOCKS * sizeof(int));
	cudaMemset(d_T2,0,NUMBER_OF_BLOCKS * sizeof(int));

	if(DEBUG)
	{
		printArray(h_R1,vertices-1);
		printArray(h_R2,vertices-1);
		printArray(h_R3,vertices-1);
	}
	
	
	if( vertices-1 > THREADS_PER_BLOCK)
	{
		secondKernelRequired = 1;
	}
	
	
	

	// kernel launch
	for(int v = 0 ; v < vertices-2; v++)
	{
		findNearestVertexKernelOne<<<NUMBER_OF_BLOCKS,THREADS_PER_BLOCK>>>(d_R2, d_R3, d_T1, d_T2, d_nearestVertex,secondKernelRequired,vertices);

		if(secondKernelRequired)
		{
			if(DEBUG)
			{
				printf("\n Yes Requird \n");
				cudaMemcpy(h_T1,d_T1, NUMBER_OF_BLOCKS* sizeof(int),cudaMemcpyDeviceToHost);
				cudaMemcpy(h_T2,d_T2, NUMBER_OF_BLOCKS * sizeof(int),cudaMemcpyDeviceToHost);
				printArray(h_T1,NUMBER_OF_BLOCKS);
				printArray(h_T2,NUMBER_OF_BLOCKS);
			}
			findNearestVertexKernelTwo<<<1,THREADS_PER_BLOCK>>>(d_R1, d_R2, d_R3, d_T1, d_T2, d_nearestVertex);
		}	
		
		if(DEBUG)
		{	
			cudaMemcpy(h_R1,d_R1, (vertices - 1)* sizeof(int),cudaMemcpyDeviceToHost);
			cudaMemcpy(h_R2,d_R2, (vertices - 1)* sizeof(int),cudaMemcpyDeviceToHost);
			cudaMemcpy(h_R3,d_R3, (vertices - 1)* sizeof(int),cudaMemcpyDeviceToHost);
			printf("\nAfter Minimum\n");
			printArray(h_R1,vertices-1);
			printArray(h_R2,vertices-1);
			printArray(h_R3,vertices-1);
			printf("%d\n",*h_nearestVertex);
		}
		cudaMemcpy(h_nearestVertex,d_nearestVertex,  sizeof(int),cudaMemcpyDeviceToHost);
		startIndex = h_indexArray[*h_nearestVertex];
		if(*(h_nearestVertex) == vertices-1)
        		endIndex = 2*edges-1;
	    	else
        		endIndex = h_indexArray[*(h_nearestVertex)+1]-1;
		if(DEBUG)
		{
			printf("\nstart : %d  end : %d\n",startIndex,endIndex);
		}
		
		// update the list
		updateMSTList<<<NUMBER_OF_BLOCKS,THREADS_PER_BLOCK>>>(startIndex, endIndex , d_nearestVertex, d_destinationVertexArray, d_weightArray, d_R1, d_R2, d_R3);
		if(DEBUG)
		{
			cudaMemcpy(h_R1,d_R1, (vertices - 1)* sizeof(int),cudaMemcpyDeviceToHost);
			cudaMemcpy(h_R2,d_R2, (vertices - 1)* sizeof(int),cudaMemcpyDeviceToHost);
			cudaMemcpy(h_R3,d_R3, (vertices - 1)* sizeof(int),cudaMemcpyDeviceToHost);
			cudaMemcpy(h_nearestVertex,d_nearestVertex,  sizeof(int),cudaMemcpyDeviceToHost);
			printf("\nAfter Update\n");
			printArray(h_R1,vertices-1);
			printArray(h_R2,vertices-1);
			printArray(h_R3,vertices-1);
			printf("%d\n",*h_nearestVertex);
		}
	}

	// for generating the output...
	
	cudaMemcpy(h_R1,d_R1, (vertices - 1)* sizeof(int),cudaMemcpyDeviceToHost);
	cudaMemcpy(h_R2,d_R2, (vertices - 1)* sizeof(int),cudaMemcpyDeviceToHost);
	cudaMemcpy(h_R3,d_R3, (vertices - 1)* sizeof(int),cudaMemcpyDeviceToHost);
	
	// end time
	cudaEventRecord( stop, 0 );
   	cudaEventSynchronize( stop );
   	cudaEventElapsedTime( &time, start, stop );
   	cudaEventDestroy( start );
   	cudaEventDestroy( stop );


	printf("u\tv\tw\n");
	long cost = 0;
	for(int i = 0; i < vertices-1;i++)
	{
		printf("%d\t%d\t%d\t\n",h_R1[i]+1,i+2,h_R3[i]);
		cost = cost + h_R3[i];
	}
	cout<<"\ncost:"<<cost;
		
	printf("\nTime: %fms\n",time);


}
        

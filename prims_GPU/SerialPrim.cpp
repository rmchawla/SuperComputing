#include<stdio.h>
#include<math.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <cstring> 
using namespace std;


#define THREADS_PER_BLOCK 64
#define NUMBER_OF_BLOCKS 64

#define VERTICES 262144
#define EDGES 10000000

//initially, 6 & 9

// number of threads_per_block*blocks should be atleast V-1.
// threads per block should be greater than or equal to number_of_blocks and both should be power of 2.
// only bcz of the restriction of cuda kernel two.


// if the vertices begin with 1 not 0, be careful, got to map it accordingly.
//destinationVertexArray decrease the value of indices by 1, and when result is out, increase the indices by 1.
//R2 array will also begin with 2 in that case, as the R1 initialization would be 1.

void initializeGraph(int *indexArray , int *destinationVertexArray , int *weightArray)
{

	// to be read from file or generators for now make static.
/*

	indexArray[0] = 0;
	indexArray[1] = 2;
	indexArray[2] = 6;
	indexArray[3] = 9;
	indexArray[4] = 11;
	indexArray[5] = 15;
	
	
	destinationVertexArray[0] = 1;
	weightArray[0] = 4;
	destinationVertexArray[1] = 5;
	weightArray[1] = 2;

	destinationVertexArray[2] = 2;
	weightArray[2] = 7;
	destinationVertexArray[3] = 4;
	weightArray[3] = 8;
	destinationVertexArray[4] = 5;
	weightArray[4] = 3;
	destinationVertexArray[5] = 0;
	weightArray[5] = 4;

	
	destinationVertexArray[6] = 3;
	weightArray[6] = 5;
	destinationVertexArray[7] = 4;
	weightArray[7] = 9;
	destinationVertexArray[8] = 1;
	weightArray[8] = 7;
	

	destinationVertexArray[9] = 2;
	weightArray[9] = 5;
	destinationVertexArray[10] = 4;
	weightArray[10] = 1;

	
	destinationVertexArray[11] = 3;
	weightArray[11] = 1;
	destinationVertexArray[12] = 2;
	weightArray[12] = 9;
	destinationVertexArray[13] = 1;
	weightArray[13] = 8;
	destinationVertexArray[14] = 5;
	weightArray[14] = 6;


	destinationVertexArray[15] = 0;
	weightArray[15] = 2;
	destinationVertexArray[16] = 1;
	weightArray[16] = 3;
	destinationVertexArray[17] = 4;
	weightArray[17] = 6;

*/

/*
	indexArray[0] = 0;
	indexArray[1] = 3;
	indexArray[2] = 6;
	indexArray[3] = 10;
	indexArray[4] = 12;
	indexArray[5] = 15;

	destinationVertexArray[0] = 1;
	weightArray[0] = 6;
	
	destinationVertexArray[1] = 2;
	weightArray[1] = 1;
	
	
	destinationVertexArray[2] = 3;
	weightArray[2] = 5;

	destinationVertexArray[3] = 4;
	weightArray[3] = 3;

	destinationVertexArray[4] = 2;
	weightArray[4] = 5;

	destinationVertexArray[5] = 0;
	weightArray[5] = 6;

	destinationVertexArray[6] = 0;
	weightArray[6] = 1;

	destinationVertexArray[7] = 1;
	weightArray[7] = 5;

	destinationVertexArray[8] = 4; 
	weightArray[8] = 5;

	destinationVertexArray[9] = 5;
	weightArray[9] = 4;

	destinationVertexArray[10] = 0;
	weightArray[10] = 5;

	destinationVertexArray[11] = 5;
	weightArray[11] = 2;

	destinationVertexArray[12] = 1;
	weightArray[12] = 3;

	destinationVertexArray[13] = 2;
	weightArray[13] = 5;

	destinationVertexArray[14] = 5;
	weightArray[14] = 6;

	destinationVertexArray[15] = 3;
	weightArray[15] = 2;

	destinationVertexArray[16] = 2;
	weightArray[16] = 4;

	destinationVertexArray[17] = 4;
	weightArray[17] = 6;

*/
/*	
	indexArray[0] = 0;
	indexArray[1] = 5;
	indexArray[2] = 9;
	indexArray[3] = 13;
	indexArray[4] = 15;
	indexArray[5] = 18;
	indexArray[6] = 24;
	indexArray[7] = 27;
	indexArray[8] = 29;

	destinationVertexArray[0] = 2;
	weightArray[0] = 9;
	destinationVertexArray[1] = 4;
	weightArray[1] = 7;
	destinationVertexArray[2] = 6;
	weightArray[2] = 4;
	destinationVertexArray[3] = 1;
	weightArray[3] = 10;
	destinationVertexArray[4] = 5;
	weightArray[4] = 3;

	destinationVertexArray[5] = 7;
	weightArray[5] = 2;
	destinationVertexArray[6] = 5;
	weightArray[6] = 5;
	destinationVertexArray[7] = 0;
	weightArray[7] = 10;
	destinationVertexArray[8] = 6; 
	weightArray[8] = 3;
	
	destinationVertexArray[9] = 0;
	weightArray[9] = 9;
	destinationVertexArray[10] = 5;
	weightArray[10] = 9;
	destinationVertexArray[11] = 3;
	weightArray[11] = 11;
	destinationVertexArray[12] = 4; 
	weightArray[12] = 12;

	destinationVertexArray[13] = 5;
	weightArray[13] = 13;
	destinationVertexArray[14] = 2;
	weightArray[14] = 11;

	destinationVertexArray[15] = 6;
	weightArray[15] = 8;
	destinationVertexArray[16] = 0;
	weightArray[16] = 7;
	destinationVertexArray[17] = 2;
	weightArray[17] = 12;

	destinationVertexArray[18] = 7;
	weightArray[18] = 6;
	destinationVertexArray[19] = 8;
	weightArray[19] = 15;
	destinationVertexArray[20] = 3;
	weightArray[20] = 13;
	destinationVertexArray[21] = 2;
	weightArray[21] = 9;
	destinationVertexArray[22] = 0;
	weightArray[22] = 3;
	destinationVertexArray[23] = 1;
	weightArray[23] = 5;

	destinationVertexArray[24] = 1;
	weightArray[24] = 3;
	destinationVertexArray[25] = 0;
	weightArray[25] = 4;
	destinationVertexArray[26] = 4;
	weightArray[26] = 8;

	destinationVertexArray[27] = 1;
	weightArray[27] = 2;
	destinationVertexArray[28] = 5;
	weightArray[28] = 6;

	destinationVertexArray[29] = 5;
	weightArray[29] = 15;
*/
	
	int u = 1,v,w;
	int count = 0;
	int prev = 1;
	int edges,vertices;
	cin>>vertices>>edges;
//	cout<<edges<<"  "<<vertices<<endl;
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
	//	   cout<<indexArray[prev]<<endl;
	   }	
	   destinationVertexArray[i] = v-1;
	   weightArray[i] = w;
	 //  cout<<u<<" "<<destinationVertexArray[i]<<" "<<weightArray[i]<<endl;		
    }

/*
	for(int i = 0; i<vertices-1;i++)
	{
		for(int j=indexArray[i]; j<indexArray[i+1];j++)
		{
			cout<<i<<" "<<destinationVertexArray[j]<<" "<<weightArray[j]<<endl;		
		}
	}

	for(int i=indexArray[vertices-1];i<100;i++)
	{
		cout<<"19"<<" "<<destinationVertexArray[i]<<" "<<weightArray[i]<<endl;		
	}
*/	
}


// R1 source, R2 destination,R3 weight
void initMSTEdgeList(int *R1 , int *R2 , int *R3, int *indexArray , int *destinationVertexArray, int *weightArray)
{
	for(int v = indexArray[0] ; v < indexArray[1] ; v++)
	{
		// update weight of adjacent vertices of vertex 0. -1 bcz counting at r2 starts from 1 i.e at index 0 vertex 1 is placed.
		// So correspondingly for R3.
		R3[destinationVertexArray[v]-1] = weightArray[v];
	}	

	for(int i = 0;i< VERTICES-1;i++)
	{
		R2[i] = i+1;
		R1[i] = 0;
	}
}


void printArray(char name,int *a  , int size)
{
    printf("%c: ",name);
    for(int i = 0; i < size ; i++)
        printf("%d\t",a[i]);
    printf("\n");
}


void updateMSTList(int startIndex,int endIndex, int *nearestVertex, int *destinationVertexArray,int *weightArray,int *R1,int *R2,int *R3)
{
	
	for(int i = startIndex; i <= endIndex;i++)
	{	
		if(destinationVertexArray[i] != 0)
		{	
			if(R2[destinationVertexArray[i]-1] != 0)
			{	// -1 bcz the r2 & r3 counting begins from 1.// update the edge and the source vertex.
				if(R3[destinationVertexArray[i]-1] > weightArray[i] )
				{		
					R3[destinationVertexArray[i]-1] = weightArray[i];
					R1[destinationVertexArray[i]-1] = *nearestVertex;
				}
			}
		}
	}
}

void findNearestVertex(int *R1, int *R2 , int *R3, int *nearest_vertex)
{

	int *min = (int *)malloc(sizeof(int));
	memset(min,1,sizeof(int));	

	for(int i = 0; i < VERTICES-1 ; i++)
	{
		if(R3[i] < (*min) && R2[i] != 0)
		{
			 *min = R3[i];
			*nearest_vertex = i+1;			
		}		
	}
	R2[(*nearest_vertex)-1] = 0;
}



int main()
{
	// graph representatioon arrays
	int *indexArray;
	int *weightArray;
	int *destinationVertexArray;


	// MST Edge list
	int *R1;
	int *R2;
	int *R3;
	int *nearestVertex;
   	
		
	// iteration indexes for updation
	int startIndex, endIndex;
	
	// allocate memory to compact adjacency list

	indexArray = (int *)malloc(VERTICES * sizeof(int));
 	weightArray = (int *)malloc(2 * EDGES * sizeof(int));
    	destinationVertexArray = (int *)malloc(2 * EDGES * sizeof(int));
	nearestVertex = (int *)malloc(sizeof(int));
	
	memset(indexArray,0,VERTICES * sizeof(int));
	memset(weightArray,0,2 * EDGES * sizeof(int));
	memset(destinationVertexArray,0,2 * EDGES * sizeof(int));
	*nearestVertex = 0;
	
	R1 = (int *)malloc((VERTICES-1) * sizeof(int));
	R2 = (int *)malloc((VERTICES-1) * sizeof(int));
	R3 = (int *)malloc((VERTICES-1) * sizeof(int));
	memset(R3,1,(VERTICES-1)*sizeof(int));
	
	initializeGraph(indexArray,destinationVertexArray,weightArray);
	
	initMSTEdgeList(R1,R2,R3,indexArray,destinationVertexArray,weightArray);	


//	printArray('u',R1,VERTICES-1);
//	printArray('v',R2,VERTICES-1);
//	printArray('w',R3,VERTICES-1);
	
	cudaEvent_t start, stop;
        float time;
        cudaEventCreate(&start);
        cudaEventCreate(&stop);
        cudaEventRecord( start, 0 );
	
	// kernel launch
	for(int v = 0 ; v < VERTICES-2; v++)
	{
		findNearestVertex(R1, R2, R3, nearestVertex);

		startIndex = indexArray[*nearestVertex];
		if(*(nearestVertex) == VERTICES-1)
        		endIndex = 2*EDGES-1;
	    	else
        		endIndex = indexArray[*nearestVertex+1]-1;
		//printf("\nstart : %d  end : %d\n",startIndex,endIndex);
		// update the list
		//printf("\nAfter minimum::\n");
		//printArray('u',R1,VERTICES-1);
		//printArray('v',R2,VERTICES-1);
		//printArray('w',R3,VERTICES-1);
		//printf("nearestVertex: %d\n",*nearestVertex);

		updateMSTList(startIndex, endIndex , nearestVertex, destinationVertexArray, weightArray, R1, R2, R3);
		
		//printf("\nAfter Update\n");
		//printArray('u',R1,VERTICES-1);
		//printArray('v',R2,VERTICES-1);
		//printArray('w',R3,VERTICES-1);
		//printf("%d\n",*nearestVertex);
	}

		//printf("\nFINAL RESULT\n");
		// update R2
		//for(int i = 0; i< VERTICES-1 ;i++)
		//{
		//	R2[i] = i+1;
		//}	
		//printArray('u',R1,VERTICES-1);
		//printArray('v',R2,VERTICES-1);
		//printArray('w',R3,VERTICES-1);
		 // for generating the output...
        printf("u\tv\tw\n");
	long cost = 0;
	cudaEventRecord( stop, 0 );
        cudaEventSynchronize( stop );
        cudaEventElapsedTime( &time, start, stop );
        cudaEventDestroy( start );
        cudaEventDestroy( stop );

        for(int i = 0; i < VERTICES-1;i++)
        {
       		 printf("%d\t%d\t%d\t\n",R1[i]+1,i+2,R3[i]);
			 cost = cost + R3[i];
        }
	cout<<"\ncost:"<<cost;
 	printf("\nTime: %fms\n",time);		
}
   

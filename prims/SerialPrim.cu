#include<stdio.h>
#include<math.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <cstring> 
using namespace std;



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

void initializeGraph(int *indexArray , int *destinationVertexArray , int *weightArray, int edges)
{

	
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

void findNearestVertex(int *R1, int *R2 , int *R3, int *nearest_vertex,int vertices)
{

	int *min = (int *)malloc(sizeof(int));
	memset(min,1,sizeof(int));	

	for(int i = 0; i < vertices-1 ; i++)
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
   	

	
	int edges,vertices;
	cin>>vertices>>edges;
	edges = edges/2;	
	
	// iteration indexes for updation
	int startIndex, endIndex;
	
	// allocate memory to compact adjacency list

	indexArray = (int *)malloc(vertices * sizeof(int));
 	weightArray = (int *)malloc(2 * edges * sizeof(int));
    destinationVertexArray = (int *)malloc(2 * edges * sizeof(int));
	nearestVertex = (int *)malloc(sizeof(int));
	
	memset(indexArray,0,vertices * sizeof(int));
	memset(weightArray,0,2 * edges * sizeof(int));
	memset(destinationVertexArray,0,2 * edges * sizeof(int));
	*nearestVertex = 0;
	
	R1 = (int *)malloc((vertices-1) * sizeof(int));
	R2 = (int *)malloc((vertices-1) * sizeof(int));
	R3 = (int *)malloc((vertices-1) * sizeof(int));
	memset(R3,1,(vertices-1)*sizeof(int));
	
	initializeGraph(indexArray,destinationVertexArray,weightArray,2*edges);
	
	initMSTEdgeList(R1,R2,R3,indexArray,destinationVertexArray,weightArray,vertices);	


	
		cudaEvent_t start, stop;
        float time;
        cudaEventCreate(&start);
        cudaEventCreate(&stop);
        cudaEventRecord( start, 0 );
	
	// kernel launch
	for(int v = 0 ; v < vertices-2; v++)
	{
		findNearestVertex(R1, R2, R3, nearestVertex,vertices);

		startIndex = indexArray[*nearestVertex];
		if(*(nearestVertex) == vertices-1)
        		endIndex = 2*edges-1;
	    	else
        		endIndex = indexArray[*nearestVertex+1]-1;

		updateMSTList(startIndex, endIndex , nearestVertex, destinationVertexArray, weightArray, R1, R2, R3);
		
	}

        printf("u\tv\tw\n");
		long cost = 0;
		cudaEventRecord( stop, 0 );
        cudaEventSynchronize( stop );
        cudaEventElapsedTime( &time, start, stop );
        cudaEventDestroy( start );
        cudaEventDestroy( stop );

        for(int i = 0; i < vertices-1;i++)
        {
       		 printf("%d\t%d\t%d\t\n",R1[i]+1,i+2,R3[i]);
		 cost = cost + R3[i];
        }
		cout<<"\ncost:"<<cost;
 		printf("\nTime: %fms\n",time);		
}
   

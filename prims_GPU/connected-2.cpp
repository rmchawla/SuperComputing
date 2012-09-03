/*

Given n and m ( >= 2( n - 1 ) ), generates a random connected 
undirected graph with n vertices and m edges. The edges are 
weighted with random integers in [ 1, MAX_W ] ( default value 
of MAX_W is 1000 ).

Method: We first generate a random spanning tree by repeatedly
connecting a vertex chosen uniformly at random from the current 
tree with a vertex chosen uniformly at random from outside the
tree. The remaining m - 2( n - 1 ) edges are added by repeatedly
choosing two vertices u and v from the entire graph ( again 
uniformly at random ) such that edges ( u, v ) and ( v, u )
do not already exist, and adding a pair of edges between them.

*/

#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include<stdio.h>
using namespace std;

// maximum edge weight
#define MAX_W 1000

typedef struct
	{
	 int y;                   // other end of the edge
         int w;                   // edge weight
	} EDGE_TYPE;

typedef struct
	{
	 int pos;                 // temporary storage
         int ne;                  // number of edges
         vector < EDGE_TYPE > E;  // list of edges
	} VERTEX_TYPE;

vector < VERTEX_TYPE > V;         // list of vertices

int n, m;                         // n = #vertices, m = #edges



// generate a random connected undirected graph with n vertices 
// and m edges. The edges are weighted with random integers in 
// [ 1, MAX_W ].

void gen_graph( void )
{
   int x, y, i, j, k, l;
   EDGE_TYPE e;

   V.resize( n + 1 );

   for ( k = 1; k <= n; k++ )
     {
      V[ k ].pos = k;
      V[ k ].ne = 0;
     }

   srand48( time( NULL ) );

   for ( k = 1; k < n; k++ )                     // create a random spanning tree
     {      
      i = 1 + ( lrand48( ) % k );                
      x = V[ i ].pos;                            // choose a random vertex x from the current tree

      j = k + 1 + ( lrand48( ) % ( n - k ) );
      y = V[ j ].pos;                            // choose a random vertex y from outside the tree

      V[ j ].pos = V[ k + 1 ].pos;
      V[ k + 1 ].pos = y;                        // add vertex y to the spanning tree

      e.w = 1 + ( lrand48( ) % MAX_W );          // choose a random edge weight

      e.y = y;
      V[ x ].E.push_back( e );                   // add edge ( x, y )
      V[ x ].ne++;

      e.y = x;
      V[ y ].E.push_back( e );                   // add edge ( y, x )
      V[ y ].ne++;
     }

   k = ( n - 1 ) << 1;                           // 2( n - 1 ) edges has already been added

   while ( k < m )                               // add the remaining edges
     {
      x = 1 + ( lrand48( ) % n );                // choose a random vertex x
      y = 1 + ( lrand48( ) % n );                // choose a random vertex y

      if ( x != y )                              // will not create a self loop
        {
         for ( l = 0; l < V[ x ].ne; l++ )       // check if ( x, y ) already exists
            if ( V[ x ].E[ l ].y == y ) break;

         if ( l == V[ x ].ne )                   // ( x, y ) is a new edge
            {
             e.w = 1 + ( lrand48( ) % MAX_W );   // choose a random edge weight

             e.y = y;
             V[ x ].E.push_back( e );            // add edge ( x, y )
             V[ x ].ne++;

	     e.y = x;
             V[ y ].E.push_back( e );            // add edge ( y, x )
             V[ y ].ne++;

             k += 2;                             // two edges have been added
            }
        }
     }
}


// print the graph in standard DIMACS Challenge 9 format

void print_graph( void )
{
   int i, j;

//   printf( "c an undirected weighted connected graph with %d vertices and %d edges\n", n, m );
   //printf( "p sp %d %d\n", n, m );

   printf("%d %d\n",n,m);
   for ( i = 1; i <= n; i++ )
      for ( j = 0; j < V[ i ].ne; j++ )
         printf( "%d %d %d\n", i, V[ i ].E[ j ].y, (((V[ i ].E[ j ].w )% 100)+1) );
}


int main( int argc, char *argv[ ] )
{
   double mm;

   if ( ( argc < 3 ) || ( sscanf( argv[ 1 ], "%d", &n ) < 1 ) || ( sscanf( argv[ 2 ], "%d", &m ) < 1 ) )
      {
       printf( "\nUsage: %s #vertices #edges\n\n", argv[ 0 ] );
       return 1;
      }

   if ( ( n < 1 ) || ( n > 1000000000 ) || ( m < 1 ) || ( m > 1000000000 ) )
      {
       printf( "\nError: Both #vertices and #edges must be integers in [ 1, 10^9 ].\n\n", argv[ 0 ] );
       return 1;
      }

   if ( m & 1 )                     // each undirected edge counts twice ( as two directed edges )
      {
       printf( "\nError: #edges must be even.\n\n", argv[ 0 ] );
       return 1;
      }

   if ( m < ( ( n - 1 ) << 1 ) )    // at least 2( n - 1 ) edges are needed to generate the connected graph
      {
       printf( "\nError: Too few edges!\n\n", argv[ 0 ] );
       return 1;
      }

   mm = ( 1.0 * n ) * ( n - 1 );

   if ( m > mm )                    // an undirected graph on n vertices cannot contain more than n * ( n - 1 ) edges
      {
       printf( "\nError: Too many edges!\n\n", argv[ 0 ] );
       return 1;
      }

   gen_graph( );                    // generate the graph
   print_graph( );                  // print the graph

   return 0;
}

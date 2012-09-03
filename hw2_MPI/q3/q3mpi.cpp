#include "mpi.h"
#include <stdio.h>
#include <iostream>
#include <iomanip>
using namespace std;
#define MAXWORKER 3 /* maximum number of worker tasks */
#define MINWORKER 3 /* minimum number of worker tasks */
#define TIME_STEPS 5 /* number of time steps */
#define K 3


void printMatrix(double *s, int size)
{

	int i = 0;
	int j = 0;
	int k = 0;
  std::cout<<"\n\n\n\n\n\n";
  for(i = 3; i<7;i++)
   {
     for(j=0;j<size;j++)
        {
           std::cout<<"\n";
           for(k =0; k<size ;k++)
            {
              // std::cin>>s[i][j][k];
               std::cout<<s[i*size*size+j*size+k]<<"\t";
            }
        }
   }
}



int main(int argc,char *argv[])
{
  
  int myRank;
  int numProcessors;// = 3;
  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&numProcessors);
  MPI_Comm_rank(MPI_COMM_WORLD,&myRank);
  int destination = 0;
  int source = 0;
  int tfinal;
  int n;
  double c[4];
  MPI_Status status;
  int slaveProcessors = numProcessors - 1;
  int min_number_planes, number_planes, extra_planes;
  //int numworkers = MAXWORKER-1;
  int i = 0, j = 0, k = 0;
	

  
  if (myRank == 0)
    {


	std::cin>>n;
        //std::cout<<n;
        std::cin>>tfinal;
        //std::cout<<tfinal;

	

                for(i =0; i<4;i++)
                {
                        std::cin>>c[i];
                }

	double *a;
	a = (double *)malloc(n*n*n*sizeof(double));

	
	for(i = 0; i<n;i++)
   	{
     	   for(j=0;j<n;j++)
           {
           //	 std::cout<<"\n";
           	for( k =0; k<n ;k++)
            	{
               		std::cin>>a[i*n*n + j*n + k];
	  //     		std::cout<<a[i*n*n + j*n +k]<<"\t";
            	}
           }
   	}
	printf("\n\n\n\n\n");
	
      min_number_planes = n/slaveProcessors;
      extra_planes = n%slaveProcessors;
      int offset = 0;
      int x = 0, y = 0, z = 0;
	  		for (i = 1; i <= slaveProcessors; i++)
	  		{
				if(min_number_planes >= 3)
                                        number_planes = (i <= extra_planes) ? min_number_planes+1 : min_number_planes;
                                else
                                {
                                        if(offset <= n-3)
                                                number_planes = 3;
                                        else
                                             number_planes = n-offset;
				}
				//number_planes = (i <= extra_planes) ? min_number_planes+1 : min_number_planes;
	   			destination  = i;
				MPI_Send(&n, 1, MPI_INT, destination, 1, MPI_COMM_WORLD);
				MPI_Send(c,4,MPI_DOUBLE,destination,1,MPI_COMM_WORLD);
				MPI_Send(&offset, 1, MPI_INT, destination, 1, MPI_COMM_WORLD);
				MPI_Send(&number_planes, 1, MPI_INT, destination, 1, MPI_COMM_WORLD);
				MPI_Send(&tfinal, 1, MPI_INT, destination, 1, MPI_COMM_WORLD);
				if(number_planes != 0)
				MPI_Send(&a[offset*n*n], n*n*number_planes, MPI_DOUBLE, destination, 1, MPI_COMM_WORLD);
				offset += number_planes;
	//			printf("offset :%diiiiiiiiiiii:::%d\n",offset,i);
			} 
			offset =0;
			for (i=1; i<=slaveProcessors; i++)
			{
	   			source = i;
				MPI_Recv(&offset, 1 , MPI_INT, source, 1, MPI_COMM_WORLD,&status);
				MPI_Recv(&number_planes, 1 , MPI_INT, source, 1, MPI_COMM_WORLD,&status);
		//		printf("O:%d N:%d\n",offset,number_planes);
				if(number_planes!=0)
				MPI_Recv(&a[offset*n*n], n*n*number_planes , MPI_DOUBLE, source, 1, MPI_COMM_WORLD,&status);
	 		        offset += number_planes;
    			}
	//	printf("Inside Main after computation\n");	
   		for(i = 0; i<n;i++)
  		 {
     		for(j=0;j<n;j++)
        	{
           			std::cout<<"\n";
           			for(k =0; k<n ;k++)
            			{
              				// std::cin>>a[i][j][k];
		std::cout<< std::right<<setiosflags(std::ios::fixed)<<std::setprecision(6)<<std::setw(15)<<a[i*n*n +j*n +k];
            			}
        		}
   		}
 
	}
  

	if (myRank != 0)
  	{
		MPI_Status status;
		//int n  = 4;
		int x = 0 , y = 0, z = 0;
		double *d;
		double *final;
		int numberOfPlanes;
		int offset;
		int t = 0;
		double temp = 0;
		double ksum = 0;
		double *swap;
	
			
		 		
		 
		MPI_Recv(&n, 1,MPI_INT, 0 , 1, MPI_COMM_WORLD,&status);
		MPI_Recv(c, 4 ,MPI_DOUBLE, 0 , 1, MPI_COMM_WORLD,&status);
		MPI_Recv(&offset, 1,MPI_INT, 0 , 1, MPI_COMM_WORLD,&status);
		MPI_Recv(&numberOfPlanes, 1, MPI_INT, 0, 1, MPI_COMM_WORLD,&status);	
		MPI_Recv(&tfinal, 1, MPI_INT, 0, 1, MPI_COMM_WORLD,&status);

		d = (double *)malloc((numberOfPlanes+6)*n*n*sizeof(double));
		final = (double *)malloc((numberOfPlanes+6)*n*n*sizeof(double));

		for(x =0; x < numberOfPlanes+6; x++)
                   {
                        for(y = 0; y < n ; y++)
                             for(z = 0; z < n; z++)
                                        d[x*n*n+y*n+z] = 0.0;
                   }

	
		//MPI_Recv(&d[3*n*n], n*n*numberOfPlanes , MPI_DOUBLE, 0, 1, MPI_COMM_WORLD,&status);
	
		if(numberOfPlanes != 0)
		{
			
		MPI_Recv(&d[3*n*n], n*n*numberOfPlanes , MPI_DOUBLE, 0, 1, MPI_COMM_WORLD,&status);
		for(t = 1;t <= tfinal ; t++)
		{
		//MPI_Recv(&d[3*n*n], n*n*numberOfPlanes , MPI_DOUBLE, 0, 1, MPI_COMM_WORLD,&status);
	//	if(myRank == 4)
	//	{
	//		cout<<"numberOfPlanes:\n"<<numberOfPlanes;
	//		cout<<"offset:\n"<<offset;
	//		cout<<"n:\n"<<n<<"\n";
	//		cout<<"tfinal: "<<tfinal<<"\n";
	//	}
//		if(myRank == 2)
//		printMatrix(d,n);	
   		
			
//		printf("\n Inside Slave MyRank: %d\n",myRank);	
	

			//	if(myRank == 4)
	//				printf("Number of pLANE:S%d", numberOfPlanes);
	if( myRank < slaveProcessors)
                {
			//	if(myRank == 4)
	//				printf("Number of pLANE:S%d", numberOfPlanes);

                                if(numberOfPlanes >= 3)
                                {
                                MPI_Send(&d[(numberOfPlanes+0)*n*n],n*n,MPI_DOUBLE,myRank+1,1, MPI_COMM_WORLD);
                                MPI_Send(&d[(numberOfPlanes+1)*n*n],n*n , MPI_DOUBLE,myRank+1,1,MPI_COMM_WORLD);
                                MPI_Send(&d[(numberOfPlanes+2)*n*n],n*n , MPI_DOUBLE,myRank+1,1,MPI_COMM_WORLD);

                                MPI_Recv(&d[(numberOfPlanes+3)*n*n],n*n,MPI_DOUBLE,myRank+1,1, MPI_COMM_WORLD,&status);
                                MPI_Recv(&d[(numberOfPlanes+4)*n*n],n*n,MPI_DOUBLE,myRank+1,1, MPI_COMM_WORLD,&status);
                                MPI_Recv(&d[(numberOfPlanes+5)*n*n],n*n,MPI_DOUBLE,myRank+1,1, MPI_COMM_WORLD,&status);
                                }
                }



		if(myRank > 1)
	  	{
		
//			printf("\nInside > 0\n");	
		
				MPI_Recv(&d[0*n*n], n*n , MPI_DOUBLE, myRank-1, 1, MPI_COMM_WORLD,&status);
				MPI_Recv(&d[1*n*n], n*n , MPI_DOUBLE, myRank-1, 1, MPI_COMM_WORLD,&status);
				MPI_Recv(&d[2*n*n], n*n, MPI_DOUBLE, myRank-1, 1 ,MPI_COMM_WORLD,&status);
				//if(numberOfPlanes >= 3)
				//{
					MPI_Send(&d[3*n*n], n*n, MPI_DOUBLE, myRank-1, 1, MPI_COMM_WORLD);
					MPI_Send(&d[4*n*n], n*n, MPI_DOUBLE, myRank-1, 1, MPI_COMM_WORLD);
					MPI_Send(&d[5*n*n], n*n, MPI_DOUBLE, myRank-1, 1, MPI_COMM_WORLD);
				//}
 	  	}
/*
		if( myRank < slaveProcessors)
		{
			
				if(numberOfPlanes >= 3)
				{
				MPI_Send(&d[(numberOfPlanes+0)*n*n],n*n,MPI_DOUBLE,myRank+1,1, MPI_COMM_WORLD);
				MPI_Send(&d[(numberOfPlanes+1)*n*n],n*n , MPI_DOUBLE,myRank+1,1,MPI_COMM_WORLD);
				MPI_Send(&d[(numberOfPlanes+2)*n*n],n*n , MPI_DOUBLE,myRank+1,1,MPI_COMM_WORLD);
				
				MPI_Recv(&d[(numberOfPlanes+3)*n*n],n*n,MPI_DOUBLE,myRank+1,1, MPI_COMM_WORLD,&status);
				MPI_Recv(&d[(numberOfPlanes+4)*n*n],n*n,MPI_DOUBLE,myRank+1,1, MPI_COMM_WORLD,&status);
				MPI_Recv(&d[(numberOfPlanes+5)*n*n],n*n,MPI_DOUBLE,myRank+1,1, MPI_COMM_WORLD,&status);
				}
		}
		//update logic
*/		

	for(x =3; x< numberOfPlanes+3 ; x++)
    	{
        	for(y =0 ; y<n ;y++)
        	{
           		for(z =0 ; z<n; z++)
           		{
              			ksum = 0;
              			temp = 0;
              			for(i = 1;i <= K ; i++)
              			{
                			ksum = 0 ;
                 			ksum += d[(x-i)*n*n + y*n + z] + d[(x+i)*n*n + y*n + z]; 
               				if((y+i) < n)
                   				ksum+=d[x*n*n + (y+i)*n +z];
                			if((y-i) >= 0)
                   				ksum+=d[x*n*n + (y-i)*n + z];
                			if((z-i) >= 0)
                   				ksum+=d[x*n*n + y*n + z-i];
                			if((z+i) < n)
                   				ksum+=d[x*n*n + y*n + z+i];
                			temp += (c[i]*ksum);
              			}
               			final[x*n*n + y*n + z] = c[0]*d[x*n*n + y*n + z] + temp;
           		}
        	}
    	}
	swap  = d;
   	d = final;
   	final = swap;

  }  //tfinal

	}
	
	// sending master back
		MPI_Send(&offset, 1 , MPI_INT, 0, 1, MPI_COMM_WORLD);
		MPI_Send(&numberOfPlanes, 1 , MPI_INT, 0, 1, MPI_COMM_WORLD);
		if(numberOfPlanes!=0)
		MPI_Send(&d[3*n*n],numberOfPlanes * n * n,MPI_DOUBLE,0,1,MPI_COMM_WORLD);
	}	
 /*gracefully exit MPI */
  MPI_Finalize();
}



		


#include <iostream>
#include <stdlib.h>
#include "mpi.h"

#define TESTS 	10			// define number of tests
#define MS 	1024*1024		// define max. message size
using namespace std;

int main(int argc, char** argv)
{
    double 	 *signal;
    int          rank, size, iterations;
    double       starttime, endtime, t;
   
    MPI_Init( &argc, &argv );
	
    MPI_Status   status;
    MPI_Request  req;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(size < 2) {
	cout << "Minimum of 2 Processes required! Aborting programm!";
	MPI_Abort(MPI_COMM_WORLD, 1);
    }

    for (int k = 1; k <= MS; k *= 2) {
	
	iterations  = 1000/k;
	
        if (iterations < 1) { 
		iterations = 1;
	}

        signal = (double *) malloc( 1024 * k * sizeof(double) );

        for (int n = 0; n < TESTS; n++) {
            if (rank == 0) {
                /* Synchronize both processes */
		
		MPI_Barrier(MPI_COMM_WORLD); 
                starttime = MPI_Wtime();
                
		for (int j = 0; j < iterations; j++) {
		  
                    MPI_Isend( signal, k, MPI_DOUBLE, 1, n, MPI_COMM_WORLD, &req );
                    MPI_Wait( &req, &status );
		    
                    MPI_Irecv( signal, k, MPI_DOUBLE, 1, n, MPI_COMM_WORLD, &req );
                    MPI_Wait( &req, &status );
		    
                }
                endtime = MPI_Wtime();
		t = (endtime - starttime) / iterations;
                
            }
            else {
                /* Synchronize both processes */
		
		MPI_Barrier(MPI_COMM_WORLD);
                
		for (int j = 0; j < iterations; j++) {
		  
                    MPI_Irecv( signal, k, MPI_DOUBLE, 0, n, MPI_COMM_WORLD, &req );
                    MPI_Wait( &req, &status );
		    
                    MPI_Isend( signal, k, MPI_DOUBLE, 0, n, MPI_COMM_WORLD, &req );
                    MPI_Wait( &req, &status );
		    
                }
            }
        }
        

        if (rank == 0) {
	    /* Full round-trip time */
            cout << "Full round-trip:\t" << k << "\t" << t << endl;
	    
	    /* Half round-trip time */
	    cout << "Half round-trip:\t" << k << "\t" << t/2.0 << endl;
	    
        }
    }

    MPI_Finalize( );
    return 0;
}
/*********************************************************************************
 * FILENAME         barrier_synch.c
 * 
 * DESCRIPTION      These functions are part of the submission to exercises of 
 *                  the "Introduction to High Percformance Computing" (Intro HPC) 
 *                  lecture of the University of Heidelberg.
 * 
 *                  Exercise 2 - Implementation of centralized barrier in MPI.
 * 
 * AUTHORS          Klaus Naumann
 *                  Christoph Klein
 *                  Günther Schindler
 *
 * LAST CHANGE      02. NOV 2014
 * 
 ********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

void cBarrier( int *rank, int *size, int *counter ) {
	int i, signal = 1;
	int numprocs = *(size) - 1;
	
	if( *rank != numprocs ){
		MPI_Send( &signal, 1, MPI_INT, numprocs, 1 , MPI_COMM_WORLD );
	} else if( *rank == numprocs ){
		for( i=0 ; i < numprocs; i++ ) {
			MPI_Recv( &signal, 1 ,MPI_INT, i, 1, MPI_COMM_WORLD, NULL );
		}
		*counter = !*counter;
	}

	MPI_Bcast(counter, 1, MPI_INT, numprocs, MPI_COMM_WORLD);

	while(!*counter);
	*counter = !*counter;
}

int main(int argc, char **argv) {
	int rank, size, i;
	double t, t1, t2;
	int iterations = atoi(argv[1]);
	int counter = 0;
	
	MPI_Init(&argc, &argv);

	MPI_Comm_size( MPI_COMM_WORLD, &size );
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );

	//printf("Start test for built-in barrier:\n");
	for( i=0; i<iterations; i++ ){
		t1 = MPI_Wtime();
		MPI_Barrier(MPI_COMM_WORLD);
		t2 = MPI_Wtime();
		t += (t2 - t1);
		//printf("Process No. %d reached barrier No.: %d\n", rank, i+1);
	}
	printf("Average built-in barrier latency for process No. %d is %f (%d Process total)\n", rank, t/iterations, size);
	
	t = 0;

	//printf("Start test for cBarrier:\n");
	for( i=0; i<iterations; i++ ){
		t1 = MPI_Wtime();
		//MPI_Barrier(MPI_COMM_WORLD);
		cBarrier(&rank, &size, &counter);
		t2 = MPI_Wtime();
		t += (t2 - t1);
		//printf("Process No. %d reached barrier No.: %d\n", rank, i+1);
	}

	printf("Average cBarrier latency for process No. %d is %f (%d Process total)\n", rank, t/iterations, size);

	MPI_Finalize();
	return 0;
}


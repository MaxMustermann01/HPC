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
#include "mpi.h"
#include <stdlib.h>

void centralized_barrier( int *rank, int *size, int *counter ) {
	int signal = 1, i;
	
	if( *rank != *(size)-1 ){
		MPI_Send( &signal, 1, MPI_INT, *(size)-1, 1 , MPI_COMM_WORLD );
	}

	if( *rank == *(size)-1 ){
		for( i=0 ; i<*(size)-1; i++ ) {
			MPI_Recv( &signal, 1 ,MPI_INT, i, 1, MPI_COMM_WORLD, NULL );
		}
	}

	if( *rank == *(size)-1 ){
		*counter = !*counter;
	}

	MPI_Bcast(counter, 1, MPI_INT, *(size)-1, MPI_COMM_WORLD);

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

	for( i=0; i<iterations; i++ ){
		t1 = MPI_Wtime();
		//MPI_Barrier(MPI_COMM_WORLD);
		centralized_barrier(&rank, &size, &counter);
		t2 = MPI_Wtime();
		t += (t2 - t1);
		printf("Proc %d reached barrier %d\n", rank, i);
	}

	printf("Time spent in barrier by process %d is %f\n", rank, t/iterations);

	MPI_Finalize();
	return 0;
}


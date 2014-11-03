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
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void barrier(int *rank, int *size) {
	int i;
	int signal = 1;
	MPI_Status status;

		if (*(rank) == 0) {

			for(i = 1; i < *(size); i++) {
				MPI_Send(&signal, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
				printf("%d: sent message to Process No. : %d\n", *(rank), i);
			}

			for(i = 1; i < *(size); i++) {
				MPI_Recv(&signal, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
				printf("%d: received message from Process No. : %d\n", *(rank), i);
			}
		
		} 
		else {
			MPI_Recv(&signal, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
			printf("%d: received message from Process No. : %d\n", *(rank), status.MPI_SOURCE);			

			MPI_Send(&signal, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
			printf("%d: sent message to Process No. : %d\n", *(rank), status.MPI_SOURCE);			
		}
}

int main(int argc, char **argv) {
	int i, size, rank;
	double t, starttime, endtime;
	int iterations = atoi(argv[1]);

	MPI_Init(&argc,&argv);		// Initialisation of MPI
	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	starttime = MPI_Wtime();
	for(i = 0; i < iterations; i++) {
		barrier(&rank, &size);
		//MPI_Barrier();
	}

	endtime = MPI_Wtime();
	t = endtime - starttime;
	printf("Time elapsed: %f\n", t/iterations);

	MPI_Finalize();			// Deinitialisation of MPI
	
	return 0;
}

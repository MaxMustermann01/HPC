#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>

int main(int argc, char **argv) {

	int i, rank, size;
	int iterations = atoi(argv(1)];
	double t1, t2, t;

	
	MPI_Init(&argc, &argv);

	MPI_Comm_size( MPI_COMM_WORLD, &size );
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );

	for( i=0; i<iterations; i++ ){
		t1 = MPI_Wtime();
		MPI_Barrier(MPI_COMM_WORLD);
		t2 = MPI_Wtime();
		t += (time2 - time1);
		printf("Process No. %d reached barrier.", rank);
	}
	printf("Average barrier latency: %f\n", time/NUM_BARRIERS);
	MPI_Finalize();
	return 0;
}

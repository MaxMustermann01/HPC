#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include "cBarrier.h"

int main(int argc, char **argv) {

	int i, rank, size;
	int iterations = atoi(argv[1]);
	double t1, t2, t;
	int do_ = 0;
	
	MPI_Init(&argc, &argv);

	MPI_Comm_size( MPI_COMM_WORLD, &size );
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );

	for( i=0; i<iterations; i++ ){
		t1 = MPI_Wtime();
		printf("do_ : %d\n", do_);
		cBarrier(&rank, &size, &do_);
		printf("do_ : %d\n", do_);
		t2 = MPI_Wtime();
		t += (t2 - t1);
		printf("Process No. %d reached barrier. Time elapsed: %f\n", rank, t/iterations);
	}
	printf("Average barrier latency of process %d :  %f\n", rank, t/iterations);
	MPI_Finalize();
	return 0;
}


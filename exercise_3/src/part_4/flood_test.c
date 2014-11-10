/*********************************************************************************
 * FILENAME         flood_test.c
 * 
 * DESCRIPTION      These functions are part of the submission to exercises of 
 *                  the "Introduction to High Percformance Computing" (Intro HPC) 
 *                  lecture of the University of Heidelberg.
 * 
 *                  Exercise 3 - Bandwidth between processes
 * 
 * AUTHORS          Klaus Naumann
 *                  Christoph Klein
 *
 * LAST CHANGE      9. NOV 2014
 * 
 ********************************************************************************/

#include <mpi.h>
#include "flood_test.h" 
#include <stdlib.h>

double flood(int rank, double packetSize, int blocking) {

    /* data for transfer and time measurements */
    double dt, start;
    int *data;
    data = (int *) malloc(packetSize);
    int nrInts = packetSize/sizeof(int);
   
    MPI_Status status;
    MPI_Request *request;

    /* if sending process */
    if (rank == 0) {
        
        /* time measurement variables */
        start = MPI_Wtime(); 
        
        if (blocking)
            MPI_Send(data, nrInts, MPI_INT, 1, 0, MPI_COMM_WORLD);
        else
            MPI_Isend(data, nrInts, MPI_INT, 1, 0, MPI_COMM_WORLD, request); 

        /* stop timer */
        dt = MPI_Wtime() - start;
    }

    /* if recieving process */
    else if (rank == 1) {
        if (blocking)
            MPI_Recv(data, nrInts, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        else
            MPI_Irecv(data, nrInts, MPI_INT, 0, 0, MPI_COMM_WORLD, request);
        dt = 0;
    }

    /* if rank not equal to 0 or 1 exit program */
    else
        exit(1);

    return dt;
}

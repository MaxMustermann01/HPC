/*********************************************************************************
 * FILENAME         main.c
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
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int main(int argc, char **argv) {

    /* 
    * DECLARATION
    * 
    * blocking:     1=blocking, 0=non-blocking transfers
    * size:         number of processes
    * i:            loop variable
    * M:            Number of measurements
    * startsize:    size of first transferred package
    * endsize:      size of list transferred package
    * steps:        number of different package sizes between
    *               startsize and endsize
    * pTimes:       output array with average transfer times
    * n:            multiplication factor for logarithmic
    *               scaling of packet sizes
    * pPacketSizes: size of packages in Bytes
    *        
    */
    int size, rank, i, M, blocking;
    int startsize, endsize, steps;
    double *pTimes, n;
    int *pPacketSizes;
    char hostname[50];

    /* definition of variables */
    blocking  = 1;
    M         = 10;
    startsize = 200*sizeof(int);
    endsize   = 2e8*sizeof(int);
    steps     = 200;
    n         = exp(log((double) endsize / (double) startsize) / (double) steps);
    pPacketSizes = (int *) malloc(steps*sizeof(int));
    pTimes = (double *) malloc(steps*sizeof(double));  
    
    for (i=0; i<steps; i++)
        pTimes[i] = 0;

   /* exponentially increasing packet sizes */
    for (i=0; i<steps; i++) {
        pPacketSizes[i] = startsize * pow(n,i);
    }
    
    /* mpi startup */
    MPI_Init (&argc, &argv);
    MPI_Status status;
    MPI_Comm_rank( MPI_COMM_WORLD, &rank);
    MPI_Comm_size( MPI_COMM_WORLD, &size);
    gethostname( hostname, 50);

    /* execution */
    for (i=0; i<steps; i++) {
        pTimes[i] += flood(rank, pPacketSizes[i], blocking);
    }

    /* calculate averages */
    for (i=0; i<steps; i++)
        pTimes[i] /= (double) M;

    /* output only for main sending process */
    if (rank == 0) {
        printf("#PacketSizes[Bytes]\t\tTimes[s]\n");
        for (i=0; i<steps; i++) {
            printf("%i\t\t%.10f\n", pPacketSizes[i], pTimes[i]);
        }
    }

    /* finish program */
    free(pTimes);
    free(pPacketSizes);
    MPI_Finalize();
    return 0;
}

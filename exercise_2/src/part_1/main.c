/*********************************************************************************
 * FILENAME         main.c
 * 
 * DESCRIPTION      These functions are part of the submission to exercises of 
 *                  the "Introduction to High Percformance Computing" (Intro HPC) 
 *                  lecture of the University of Heidelberg.
 * 
 *                  Exercise 1 - MPI Ring Communication
 *                               
 * 
 * AUTHORS          Klaus Naumann
 *                  Christoph Klein
 *                  Günther Schindler
 *
 * LAST CHANGE      2. NOV 2014
 * 
 ********************************************************************************/

#include <mpi.h>
#include <stdio.h>
#include <unistd.h>

int main (int argc, char **argv) {

    int size, rank, m = 3000, signal = 666,i;
    double starttime, endtime;
    char hostname[50];
    MPI_Status status;

    MPI_Init (&argc, &argv);

    MPI_Comm_rank( MPI_COMM_WORLD, &rank);
    MPI_Comm_size( MPI_COMM_WORLD, &size);

    gethostname( hostname, 50);


    if (rank == 0 && size > 1) { // at least two processes
        starttime = MPI_Wtime();
        for (i=0; i<m; i++) { // main process sends the first m messages
            MPI_Send( &signal, 1, MPI_INT, 1, 0, MPI_COMM_WORLD); 
        }
        for (i=0; i<m; i++) { // main process waits for the last m messages
            MPI_Recv ( &signal, 1, MPI_INT, size-1, 0, MPI_COMM_WORLD, &status);
        }
        endtime = MPI_Wtime();
        double dt = endtime - starttime;
        double N = m*size;
        double average = dt/N;
        printf( "%.10f %.10f\n", dt, average);
    }
    else if (size > 1) { // at least two processes
        int source = rank - 1;
        for (i=0; i<m; i++) { // side processes wait for m messages
            MPI_Recv ( &signal, 1, MPI_INT, source, 0, MPI_COMM_WORLD, &status);
        }
   

        int destination = (rank + 1) % size;
        for (i=0; i<m; i++) { // side processes send m messages
            MPI_Send( &signal, 1, MPI_INT, destination, 0, MPI_COMM_WORLD);
        }
    }
    else printf ("No communication possible as the number of running processes is equal to one.");

    MPI_Finalize();
    return 0;

}

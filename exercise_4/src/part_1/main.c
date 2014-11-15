/*********************************************************************************
 * FILENAME         main.c
 * 
 * DESCRIPTION      These functions are part of the submission to exercises of 
 *                  the "Introduction to High Percformance Computing" (Intro HPC) lecture of the 
 *                  University of Heidelberg.
 * 
 *                  Exercise 4 - Main function for matrix multiplication (parallel)
 * 
 * AUTHORS          Klaus Naumann
 *                  Christoph Klein
 *                  Günther Schindler
 *
 * LAST CHANGE      15. Nov 2014
 * 
 ********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "matrix_multiply.h"
#include "time_measurement.h"
#include "mpi.h"

int main(int argc, char* argv[]){
  int 		i;
		iSize;					// Matrix size 
		prows;					// rows per process
		mrows;					
		rows;
		dest;					// destination
		matrixpart = 0;
		rank; 
		size;
  sMatrix 	sMa, sMb, sMRes;
  double 	dStartTime=0.0, dElapsedTime=0.0;
 
  iSize = atoi(argv[1]);
  
  /* Allocate memory for matrices */
  if(vAllocMatrix(&sMa, iSize, iSize))
    exit(1);
  if(vAllocMatrix(&sMb, iSize, iSize))
    exit(1);
  if(vAllocMatrix(&sMRes, iSize, iSize))
    exit(1);
  
  MPI_Status	status;
  
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  
  if(size < 2) 
  {
    printf("\nMinimum amount of processes: 3\nAborting program ...\n");
    MPI_Abort(MPI_COMM_WORLD, 1);
    return EXIT_FAILURE;
  }

/* MASTER PROCESS ***************************************************/
  if(rank == 0)
  {
    printf("Master initializing matrices ...\n");
    vInitMatrixA(&sMa);
    vInitMatrixB(&sMb);
    
    prow = iSize / (size - 1);
    mrows = iSize % (size - 1);
    
    for(dest = 1; dest <= (size - 1); dest++)
    {
      if(dest <= mrows)
      {
	rows = prow + 1;
      }
      else
      {
	rows = prow;
      }
    
      MPI_Send(&matrixpart, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
      MPI_Send(&rows, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
      MPI_Send(&sMa[matrixpart][0], rows*iSize, MPI_DOUBLE, dest, 1, MPI_COMM_WORLD);
      MPI_Send(&sMb, iSize*iSize, MPI_DOUBLE, dest, 1, MPI_COMM_WORLD);
      matrixpart += rows;
    }
    
    for(i = 1; i <= size; i++)
    {
      MPI_Recv(&matrixpart, 1, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
      MPI_Recv(&rows, 1, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
      MPI_Recv(&sMRes[matrixpart][0], rows*iSize, MPI_DOUBLE, i, 2, MPI_COMM_WORLD, &status);
    }
    
    /* Print result */
    vPrintMatrix(&sMRes);
  }
  
/* WORKER PROCESS ***************************************************/
  if(rank != 0)
  {
    MPI_Recv(&matrixpart, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
    MPI_Recv(&rows, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
    MPI_Recv(&sMa, rows*iSize, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &status);
    MPI_Recv(&sMb, iSize*iSize, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &status);
  
    iMatrixMultiply(&sMa, &sMb, &sMRes)
    
    MPI_Send(&matrixpart, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
    MPI_Send(&iSize, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
    MPI_Send(&sMRes, iSize*iSize, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);
  }
  
  MPI_Finalize();
  
  vFreeMatrix(&sMa);
  vFreeMatrix(&sMb);
  vFreeMatrix(&sMRes);
  
  return EXIT_SUCCESS;
}
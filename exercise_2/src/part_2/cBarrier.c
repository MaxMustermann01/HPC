/*********************************************************************************
 * FILENAME         matrix_multiply.c
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
#include "cBarrier.h"

void cBarrier(int *rank, int *size, int *do_)
{
  	int i;
	
	if(*rank == 0) {
		MPI_Send(1, 1, MPI_INT, *size - 1, 1, MPI_COMM_WORLD); 
	}
	else if(*rank == *size - 1) {
		for(i = 0; i < *size - 1; i++)
			MPI_Recv(1, 1, MPI_INIT, i, root, MPI_COMM_WORLD, NULL);
		*do_ = !*do_;
	}
	
	MPI_Bcast(do_, 1, MPI_INIT, *size - 1, MPI_COMM_WORLD);

	while(!*do_);
	*do_ = !*do_
}

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
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include "cBarrier.h"

void cBarrier(int *rank, int *size, int *do_)
{
  	int i, finish = 1;
	printf("do_ : %d\n", *do_);

	if(*rank != *(size) - 1) {
		MPI_Send(&finish, 1, MPI_INT, *(size) - 1, 1, MPI_COMM_WORLD); 
	}
	
	if(*rank == *(size) - 1) {
		for(i = 0; i < *(size) - 1; i++)
			MPI_Recv(&finish, 1, MPI_INT, i, 1, MPI_COMM_WORLD, NULL);
	}
	
	if(*rank == *(size) - 1) {
		*do_ = !*do_;
	}

	MPI_Bcast(do_, 1, MPI_INT, *(size) - 1, MPI_COMM_WORLD);
	printf("do_ : %d\n", *do_);
	while(!*do_);
	printf("do_ : %d\n", *do_);
	*do_ = !*do_;
}

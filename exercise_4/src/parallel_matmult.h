/*********************************************************************************
 * FILENAME         parallel_matmult.h
 *
 * DESCRIPTION      These functions are part of the submission to exercises of 
 *                  the "Introduction to High Percformance Computing" (Intro HPC) 
 *                  lecture of the University of Heidelberg.
 * 
 *                  Exercise 4 - Parallel Matrix Multiplication
 * 
 * AUTHORS          Klaus Naumann
 *                  Christoph Klein
 *                  Günther Schindler
 *
 * LAST CHANGE      16. NOV 2014
 *
 ********************************************************************************/

#ifndef PARALLEL_MATMULT_H
#define PARALLEL_MATMULT_H

#include "matrix_multiply.h"
#include "distribution.h"

/*
* DESCRIPTION - Sends a Job to another process. The both matrices
*               are the total matrices which should be multiplied (A*B).
*               The sending process can be blocking or non blocking.
* PARAMETER   - sMatrix*: Matrix A
*             - sMatrix*: Matrix B
*             - sJob*: the job element
*             - integer: send to this process number
*             - boolean: true -> blocking, false -> non-blocking
* RETURN      - void
*/
void vSendJobToProc(sMatrix*, sMatrix*, sJob* , int, bool);

/*
* DESCRIPTION - Recieves a Job sent by function vSendJobToProc.
*               the correct amount of memory is allocated too.
*               Do not allocate memory for matrices A and B
*               by yourself. The function will do that.
* USE         - sMatrix A, B;
*               vRecvJobFromProc(&A, &B, 0, true); //e.g.
* PARAMETER   - sMatrix*: Matrix A
*             - sMatrix*: Matrix B
*             - integer: expect message from this process number
*             - boolean: true -> blocking, false -> non-blocking
* RETURN      - void
*/
void vRecvJobFromProc(sMatrix*, sMatrix*, int, bool);

/*
* DESCRIPTION - Sends the Result of a matrix multiplication to
*               a certain process.
* PARAMETER   - sMatrix*: the resulting matrix
*             - integer: send to this process
* RETURN      - void
*/
void vSendResToProc(sMatrix*, int);

/*
* DESCRIPTION - Recieves a result from a process and
*               writes it to the final output matrix
* PARAMETER   - sMatrix*: the total resulting matrix
*             - integer: recieve from this process
*             - sJob*: the job sent to that process
* RETURN      - void
*/
void vRecvResFromProc(sMatrix*, int, sJob*);
#endif

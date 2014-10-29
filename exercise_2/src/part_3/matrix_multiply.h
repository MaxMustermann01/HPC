/*********************************************************************************
 * FILENAME         matrix_multiply.h
 * 
 * DESCRIPTION      These functions are part of the submission to exercises of 
 *                  the "Introduction to High Percformance Computing" (Intro HPC) 
 *                  lecture of the University of Heidelberg.
 * 
 *                  Exercise 2 - Implementation of the functions for the matrix
 *                               multiplication.
 * 
 * AUTHORS          Klaus Naumann
 *                  Christoph Klein
 *                  Günther Schindler
 *
 * LAST CHANGE      29. OKT 2014
 * 
 ********************************************************************************/
#ifndef MATRIX_MULTIPLY_H
#define MATRIX_MULTIPLY_H

typedef struct sMatrix
{
  int iRow;          // for rows
  int iCol;          // for columns
  double** ppaMat;   // double[][]
} sMatrix;

/* 
 * DESCRIPTION - Multiply a matrix by a vector. Matrix A must have the same amount of
 *               rows that the B has columns.
 * PARAMETER   - sMatrix *: pointer to matrix A which should be multiplied
 *               sMatrix *: pointer to matrix B which should be multiplied
 *               sMatrix *: pointer to matrix in which the result will be stored
 * RETURN      - void
 */
void vMatrixMultiply(sMatrix *, sMatrix *, sMatrix *);

/*
 * DESCRIPTION - Allocate memory for a double-precision floating point matrix
 * PARAMETER   - sMatrix: pointer to a sMatrix type for which the memory will be allocated
 *             - Integer: matrix rows
 *             - Integer: matrix columns
 * RETURN      - Integer: 0 if alloc was ok, 1 if alloc failed 
 */
int vAllocMatrix(sMatrix *,int, int);

/*
 * DESCRIPTION - will free the allocated memory.
 * PARAMETER   - pointer to type sMatrix, which should be destroid
 * RETURN      - void
 */
void vFreeMatrix(sMatrix *);

/*
 * DESCRIPTION - prints the matrix-elements (for testing)
 * PARAMETER   - pointer to matrix, which should be printed
 * RETURN      - void
 */
void vPrintMatrix(sMatrix *);

/*
 * DESCRIPTION - Initialize a matrix with random numbers.
 * PARAMETER   - sMatrix: pointer to matrix
 *               integer: for seed
 * RETRUN      - void
 */
void vInitMatrix(sMatrix *, int);

#endif
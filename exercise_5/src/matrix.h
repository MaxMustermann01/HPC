/*********************************************************************************
 * FILENAME         matrix.h
 *
 * DESCRIPTION      These functions are part of the submission to exercises of 
 *                  the "Introduction to High Percformance Computing" (Intro HPC) 
 *                  lecture of the University of Heidelberg.
 * 
 *                  Exercise 5 - Heat relaxation
 * 
 * AUTHORS          Klaus Naumann
 *                  Christoph Klein
 *                  Günther Schindler
 *
 * LAST CHANGE      20. NOV 2014
 *
 ********************************************************************************/
#ifndef MATRIX_H
#define MATRIX_H

#define FI 0.24

typedef struct sMatrixDouble
{
  int iRow;       // for rows
  int iCol;       // for columns
  double** ppaMat;   // int[][]
} sMatrixDouble;

/*
 * DESCRIPTION - does the relaxion
 * PARAMETER   - sMatrixInt: pointer to a sMatrix type which should be iterated
 *
 * RETURN       - void 
 */
void vRelaxMatrix(sMatrixDouble *);

/*
 * DESCRIPTION - Inject Matrix with elements 
 * PARAMETER   - sMatrixInt: pointer to a sMatrix type in which should be injected
 * RETURN      - void 
 */
void vInjectMatrix(sMatrixDouble *);

/*
 * DESCRIPTION - Allocate memory for the dynamic matrix;
 * PARAMETER   - sMatrixInt: pointer to a sMatrix type for which the memory will be allocated
 *             - Integer: matrix rows
 *             - Integer: matrix columns
 * RETURN       - Integer: 0 if alloc was ok, 1 if alloc failed 
 */
int iAllocMatrixDouble(sMatrixDouble *,int, int);

/*
 * DESCRIPTION - will free the allocated memory;
 * PARAMETER   - pointer to type sMatrix, which should be destroid
 * RETURN      - void
 */
void vFreeMatrixDouble(sMatrixDouble *);
/*
 * DESCRIPTION - prints the matrix-elements;
 * PARAMETER   - pointer to matrix, which should be printed
 * RETURN      - void
 */
void vPrintMatrixDouble(sMatrixDouble *);

#endif

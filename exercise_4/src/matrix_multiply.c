/*********************************************************************************
 * FILENAME         matrix_multiply.c
 * 
 * DESCRIPTION      These functions are part of the submission to exercises of 
 *                  the "Introduction to High Percformance Computing" (Intro HPC) 
 *                  lecture of the University of Heidelberg.
 * 
 *                  Exercise 4 - Implementation of the functions for the matrix
 *                               multiplication.
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

int iMatrixMultiply(sMatrix *pMa, sMatrix *pMb, sMatrix *pMRes)
{
  int i,j,k;
  /* 
   * In order to multiply 2 matrices, one must have the same amount of rows that the 
   * other has columns.
   */
  if(pMa->iRow == pMb->iCol && pMa->iRow == pMRes->iRow && pMb->iRow == pMRes->iCol)
  {
    for(i=0; i<pMa->iRow; i++)
    {
      for(j=0; j<pMb->iCol; j++)
      {
        for(k=0; k<pMa->iCol; k++)
	  pMRes->ppaMat[i][j] += pMa->ppaMat[i][k] * pMb->ppaMat[k][j];
      }
    }
    return 0;
  }
  else
  {
    printf("DEBUG: Problem with the size of a matrix");
    return 1;
  }
}

int iTilledMatrixMultiply(sMatrix *pMa, sMatrix *pMb, sMatrix *pMRes, int iBlockSize)
{
  int i,j,k, ii, jj, kk;
  /* 
   * In order to multiply 2 matrices, one must have the same amount of rows that the 
   * other has columns.
   */
  if(pMa->iRow == pMb->iCol && pMa->iRow == pMRes->iRow && pMb->iRow == pMRes->iCol)
  {
    for(ii=0; ii<pMa->iRow; ii+=iBlockSize)
    {
      for(jj=0; jj<pMb->iCol; jj+=iBlockSize)
      {
        for(kk=0; kk<pMa->iCol; kk+=iBlockSize)
	{
	  for(i=ii; i<ii+iBlockSize; i++)
          {
            for(j=jj; j<jj+iBlockSize; j++)
            {
              for(k=kk; k<kk+iBlockSize; k++)
	        pMRes->ppaMat[i][j] += pMa->ppaMat[i][k] * pMb->ppaMat[k][j];
            }
          } 
	}
      }
    }
    return 0;
  }
  else
  {
    printf("DEBUG: Problem with the size of a matrix");
    return 1;
  }
}

int vAllocMatrix(sMatrix *pM, int iRow, int iCol)
{ 
  int i=0, iRet=0;
  /* Init rows and cols in matrix structure */
  pM->iCol=iCol;
  pM->iRow=iRow;
  /* Alloc Mem for Rows-Pointer */
  pM->ppaMat = malloc(iRow * sizeof(double *));
  if(NULL == pM->ppaMat)
    iRet=1;
  /* Allocate Memory for Rows */
  for(i=0; i < iRow; i++)
  {
    pM->ppaMat[i] = malloc(iCol * sizeof(double *));
    if(NULL == pM->ppaMat[i])
      iRet=1;
  }
  return iRet;
}

void vFreeMatrix(sMatrix *pM)
{
  int i;
  /* free the Rows */
  for(i=0; i<pM->iRow; i++)
    free(pM->ppaMat[i]);
  /* free cols */
  free(pM->ppaMat);
}

void vInitMatrix(sMatrix *pM, int iSeed)
{
  int i,j;
  /* Initializes random number generator */
  srand((unsigned) iSeed);
  /* Fill the matrix-elements with random numbers */
  /* matrix[zeile][spalte] */
  for(i=0; i<pM->iRow; i++)
  {
    for(j=0; j<pM->iCol; j++)
      /* Generate numbers fronm 0 to 50 */
      pM->ppaMat[i][j]=(double)rand();
  }
}

void vInitMatrixA(sMatrix *pM) 
{
  int i, j;
  
  /* Fill the matrix-elements according to: A[i,j] = i + j */
  for(i = 0; i < pM->iRow; i++)
  {
    for(j = 0; j < pM->iCol; j++)
    {
      pM->ppaMat[i][j] = i+j;
    }
  }
}

void vInitMatrixB(sMatrix *pM) 
{
  int i, j;
  
  /* Fill the matrix-elements according to: B[i,j] = i * j */
  for(i = 0; i < pM->iRow; i++)
  {
    for(j = 0; j < pM->iCol; j++)
    {
      pM->ppaMat[i][j] = i*j;
    }
  }
}

void vPrintMatrix(sMatrix *pM)
{
  int i,j;
  /* matrix[zeile][spalte] */
  for(i=0; i<pM->iRow; i++)
  {
    printf("\n");
    for(j=0; j<pM->iCol; j++)
      printf("%5.2lf", pM->ppaMat[i][j]);
  }
  printf("\n");
}

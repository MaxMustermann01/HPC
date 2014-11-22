/*********************************************************************************
 * FILENAME         matrix.c
 *
 * DESCRIPTION      These functions are part of the submission to exercises of 
 *                  the "Introduction to High Percformance Computing" (Intro HPC) 
 *                  lecture of the University of Heidelberg.
 * 
 *                  Exercise 6 - Heat relaxation, parallel
 * 
 * AUTHORS          Klaus Naumann
 *                  Christoph Klein
 *                  Günther Schindler
 *
 * LAST CHANGE      21. NOV 2014
 *
 ********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"

int iAllocMatrixDouble(sMatrixDouble *pM, int iRow, int iCol)
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

void vRelaxInteriorElement(sMatrixDouble *pM)
{
  
}

void vRelaxBorderElement(sMatrixDouble *pM, sMatrixDouble *pMleft, sMatrixDouble *pMright)
{

}

void vInjectMatrix(sMatrixDouble *pM)
{
  int i,j;
  
  for(i=0; i<pM->iRow; i++)
  {
    for(j=0; j<pM->iCol; j++)
    {
      if(i == 0 && j >= (pM->iCol/4) && j <= (3*(pM->iCol)/4))
	pM->ppaMat[i][j]=127.0;
      else
	pM->ppaMat[i][j]=0.0;
    }
  }
}

void vFreeMatrixDouble(sMatrixDouble *pM)
{
  int i;
  /* free the Rows */
  for(i=0; i<pM->iRow; i++)
    free(pM->ppaMat[i]);
  /* free cols */
  free(pM->ppaMat);
}

void vPrintMatrixDouble(sMatrixDouble *pM)
{
  int i,j;
  /* matrix[zeile][spalte] */
  for(i=0; i<pM->iRow; i++)
  { 
    printf("\n");
    for(j=0; j<pM->iCol; j++)
      printf("%6.1lf", pM->ppaMat[i][j]);
  }
  printf("\n");
}

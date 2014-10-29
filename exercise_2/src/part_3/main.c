/*********************************************************************************
 * FILENAME         main.c
 * 
 * DESCRIPTION      These functions are part of the submission to exercises of 
 *                  the "Introduction to High Percformance Computing" (Intro HPC) lecture of the 
 *                  University of Heidelberg.
 * 
 *                  Exercise 2 - Main function for matrix multiplication
 * 
 * AUTHORS          Klaus Naumann
 *                  Christoph Klein
 *                  Günther Schindler
 *
 * LAST CHANGE      29. OKT 2014
 * 
 ********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "matrix_multiply.h"
#include "time_measurement.h"

int main(int argc, char* argv[]){
  int iSize=2048;
  sMatrix sMa, sMb, sMRes;
  double dStartTime=0.0, dElapsedTime=0.0;
  
  /* Allocate memory for matrizen */
  if(vAllocMatrix(&sMa, iSize, iSize))
    exit(1);
  if(vAllocMatrix(&sMb, iSize, iSize))
    exit(1);
  if(vAllocMatrix(&sMRes, iSize, iSize))
    exit(1);
  
  /* Initialize matrizen with random numbers */
  vInitMatrix(&sMa, 8);
  vInitMatrix(&sMb, 19);
  
  /* Start time measurement */
  dStartTime = dstartMeasurement();
  /* Start multiplication */
  vMatrixMultiply(&sMa, &sMb, &sMRes);
  /* Stop time-measurement*/
  dElapsedTime = dstopMeasurement(dStartTime);
  
  printf("\n Time for calculation: %lfs\n", dElapsedTime);
  
  vFreeMatrix(&sMa);
  vFreeMatrix(&sMb);
  vFreeMatrix(&sMRes);
  
  return EXIT_SUCCESS;
}
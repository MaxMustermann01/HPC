/*********************************************************************************
 * FILENAME         main.c
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
#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include "time_measurement.h"

int main(int argc, char* argv[])
{
  int iSize, iIterations, iOpt, i;
  double dValue;
  sMatrixDouble sMgrid;
  double dStartTime=0.0, dElapsedTime=0.0;
  double dGFlops;
  unsigned long long ullFlops;
  
  /* Check integrity of arguments */
  if(argc!=4)
  {
    printf("\nWrong number of arguments. Check usage!"
           " \nrelaxation <MATRIX_SIZE> <ITERATIONS> <OPTION>\n\n"
	   "<OPTION> : (1) Print Matrix, (0) Don't print Matrix\n");
    exit(1);
  }
  /* Convert C-String to int */
  iSize = atoi(argv[1]);
  iIterations = atoi(argv[2]);
  iOpt = atoi(argv[3]);
  
  /* Allocate memory for grid */
  if(iAllocMatrixDouble(&sMgrid, iSize, iSize))
  {
    printf("ERROR: Allocation failure!");
    exit(1);
  }
  
  /* Inject Matrix before relaxation */
  /*
   * 0  127 127 127 127 127  0
   * 0   0   0   0   0   0   0
   * 0   0   0   0   0   0   0
   * 0   0   0   0   0   0   0
   * 0   0   0   0   0   0   0
   * 0   0   0   0   0   0   0
   * 0   0   0   0   0   0   0
   */
  vInjectMatrix(&sMgrid);
  /* Start time-measurement */
  dStartTime = dstartMesGTOD();
  /* Make relaxation */
  for(i = 0; i<iIterations; i++)
    vRelaxMatrix(&sMgrid);
  /* Stop time-measurement, integer */
  dElapsedTime = dstopMesGTOD(dStartTime) / iIterations;
  
  /* Calculate flops: (NxN - 4xN + 4)x7 */
  ullFlops = (iSize*iSize - 4*iSize + 4)*7;
  
  /* Calculate GLFOP/s */
  dGFlops = (ullFlops / dElapsedTime)/1e9;
  
  /* Print the Results */
  printf(" Matrix size    : %d \n", iSize);
  printf(" Elapsed Time   : %lfs \n", dElapsedTime);
  printf(" Flops          : %llu \n", ullFlops);
  printf(" GFLOPS/s       : %lf \n", dGFlops);
  
  /* Print matrix after relaxation */
  if(iOpt == 1)
    vPrintMatrixDouble(&sMgrid);
  
  /* Free Allocated Memory */
  vFreeMatrixDouble(&sMgrid);
  
  return EXIT_SUCCESS;
}
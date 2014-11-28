/*********************************************************************************
 * FILENAME         main.c
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
#include "time_measurement.h"
#include "mpi.h"

#define FI 0.24
#define MASTER 0
#define BEGIN 1
#define LEFTTAG 2
#define RIGHTTAG 3
#define DONE 4

int main(int argc, char* argv[])
{
  int iSize, iIterations, iOpt, i, j, k;
  /* MPI variables */
  int iErrorcode, iNumTasks, iTaskID, iNumWorker;
  int iFrom, iTo, iOffset, iRows, iCols;
  int iLeftNeighbor, iRightNeighbor;
  MPI_Status mpiStatus;
  
  double dValue;
  sMatrixDouble sMgrid, sMleftTmp, sMrightTmp, sMgridTmp;
  
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &iNumTasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &iTaskID);
  iNumWorker = iNumTasks - 1;
  
  /********************************** Master process **********************************/
  if(iTaskID == MASTER)
  {
    /* Check integrity of arguments */
    if(argc!=4)
    {
      printf("\nWrong number of arguments. Check usage!"
             " \nrelaxation <MATRIX_SIZE> <ITERATIONS> <OPTION>\n\n"
             "<OPTION> : (1) Print Matrix, (0) Don't print Matrix\n");
      MPI_Abort(MPI_COMM_WORLD, iErrorcode);
      exit(1);
    }
    /* Convert C-String to int */
    iSize = atoi(argv[1]);
    iIterations = atoi(argv[2]);
    iOpt = atoi(argv[3]);
    
    /* Allocate memory for grid and inject heat */
    iAllocMatrixDouble(&sMgrid, iSize, iSize);
    vInjectMatrix(&sMgrid);
    
    /* Distribute work to tasks */
    for(i=1; i<=iNumWorker; i++)
    {
      /* Figure out how many rows to send */
      iFrom = ((i-1) * sMgrid.iRow) / iNumWorker;
      iTo = (i * sMgrid.iRow) / iNumWorker;
      iRows = iTo - iFrom;
      iCols = iSize;
      
      /* Figure out each task neighbor */
      if(i==1) iLeftNeighbor = 0;
      else iLeftNeighbor = i - 1;
      if(i==iNumWorker) iRightNeighbor = 0;
      else iRightNeighbor = i + 1;
      
      /* Send offset, number of coloumns, iterations and number of rows */
      MPI_Send(&iFrom, 1, MPI_INT, i, BEGIN, MPI_COMM_WORLD);
      MPI_Send(&iRows, 1, MPI_INT, i, BEGIN, MPI_COMM_WORLD);
      MPI_Send(&iCols, 1, MPI_INT, i, BEGIN, MPI_COMM_WORLD);
      MPI_Send(&iIterations, 1, MPI_INT, i, BEGIN, MPI_COMM_WORLD);
      /* Tell each slave task who its neighbors are */
      MPI_Send(&iLeftNeighbor, 1, MPI_INT, i, BEGIN, MPI_COMM_WORLD);
      MPI_Send(&iRightNeighbor, 1, MPI_INT, i, BEGIN, MPI_COMM_WORLD);
      /* Send Data */
      MPI_Send(&(sMgrid.ppaMat[iFrom][0]), iRows*iCols, MPI_DOUBLE, i, BEGIN, MPI_COMM_WORLD);
    }
    /* Collect results from tasks */
    for(i=1; i<=iNumWorker; i++)
    {
      MPI_Recv(&iFrom, 1, MPI_INT, i, DONE, MPI_COMM_WORLD, &mpiStatus);
      MPI_Recv(&iRows, 1, MPI_INT, i, DONE, MPI_COMM_WORLD, &mpiStatus);
      MPI_Recv(&(sMgrid.ppaMat[iFrom][0]), iRows*sMgrid.iCol, MPI_DOUBLE, i, DONE, MPI_COMM_WORLD, &mpiStatus);
    }
    /* Print grid to std-output */
    if(iOpt == 1)
      vPrintMatrixDouble(&sMgrid);
    /* Free allocated memory */
    vFreeMatrixDouble(&sMgrid);
  }
  /************************************ Slave process *********************************/
  else
  {
    /* Recieve offset, number of coloums, iterations and number of rows */
    MPI_Recv(&iFrom, 1, MPI_INT, MASTER, BEGIN, MPI_COMM_WORLD, &mpiStatus);
    MPI_Recv(&iRows, 1, MPI_INT, MASTER, BEGIN, MPI_COMM_WORLD, &mpiStatus);
    MPI_Recv(&iCols, 1, MPI_INT, MASTER, BEGIN, MPI_COMM_WORLD, &mpiStatus);
    MPI_Recv(&iIterations, 1, MPI_INT, MASTER, BEGIN, MPI_COMM_WORLD, &mpiStatus);
    /* Who is left and right neighbor of this task */
    MPI_Recv(&iLeftNeighbor, 1, MPI_INT, MASTER, BEGIN, MPI_COMM_WORLD, &mpiStatus);
    MPI_Recv(&iRightNeighbor, 1, MPI_INT, MASTER, BEGIN, MPI_COMM_WORLD, &mpiStatus);
    /* Allocate memory for grid-slice and for temporary grid */
    iAllocMatrixDouble(&sMgridTmp, iRows+2, iCols);
    iAllocMatrixDouble(&sMgrid, iRows, iCols);
    /* Recieve data from master */
    MPI_Recv(&(sMgrid.ppaMat[0][0]), iRows*iCols, MPI_DOUBLE, MASTER, BEGIN, MPI_COMM_WORLD, &mpiStatus);
    for(i=0; i<iIterations; i++)
    {
      if(iLeftNeighbor != 0)
      { 
        MPI_Recv(&(sMgridTmp.ppaMat[0][0]), iCols, MPI_DOUBLE, iLeftNeighbor, LEFTTAG, MPI_COMM_WORLD, &mpiStatus);
        MPI_Send(&(sMgrid.ppaMat[0][0]), iCols, MPI_DOUBLE, iLeftNeighbor, RIGHTTAG, MPI_COMM_WORLD);
      }
      if(iRightNeighbor != 0)
      {
        MPI_Send(&(sMgrid.ppaMat[iRows-1][0]), iCols, MPI_DOUBLE, iRightNeighbor, LEFTTAG, MPI_COMM_WORLD);
        MPI_Recv(&(sMgridTmp.ppaMat[iRows+1][0]), iCols, MPI_DOUBLE, iRightNeighbor, RIGHTTAG, MPI_COMM_WORLD, &mpiStatus);
      }
      /* Calculate new grid points and let border elements static */
      if(iTaskID == 1){
      vPrintMatrixDouble(&sMgrid);
      vPrintMatrixDouble(&sMgridTmp);}
      for(j=0; j<sMgrid.iRow; j++)
        for(k=0; k<sMgrid.iCol; k++)
	{
          /* Update temporary grid */
          sMgridTmp.ppaMat[j+1][k]=sMgrid.ppaMat[j][k];
          if(!(j==0  || j==sMgrid.iRow-1 || k==0 || k==sMgrid.iCol-1))
            sMgrid.ppaMat[j][k]+=FI*((-4)*sMgridTmp.ppaMat[j+1][k]+sMgridTmp.ppaMat[j+2][k]+sMgridTmp.ppaMat[j][k]+sMgridTmp.ppaMat[j+1][k+1]+sMgridTmp.ppaMat[j+1][k-1]);
        //pM->ppaMat[i][j]+=FI*((-4)*sMTmp.ppaMat[i][j]+sMTmp.ppaMat[i+1][j]+sMTmp.ppaMat[i-1][j]+sMTmp.ppaMat[i][j+1]+sMTmp.ppaMat[i][j-1]);
        }
      if(iTaskID == 1){
      vPrintMatrixDouble(&sMgrid);
      vPrintMatrixDouble(&sMgridTmp);}
      if(iLeftNeighbor != 0)
      {
        j=0;
        for(k=1; k<sMgrid.iCol-1; k++)
          sMgrid.ppaMat[j][k]+=FI*((-4)*sMgridTmp.ppaMat[j+1][k]+sMgridTmp.ppaMat[j+2][k]+sMgridTmp.ppaMat[j][k]+sMgridTmp.ppaMat[j+1][k+1]+sMgridTmp.ppaMat[j+1][k-1]);
      }
      if(iRightNeighbor != 0)
      {
        j=iRows-1;
        for(k=1; k<sMgrid.iCol-1; k++)
        {
          //sMgrid.ppaMat[j][k]+=FI*((-4)*sMgridTmp.ppaMat[j+1][k]+sMgridTmp.ppaMat[j+2][k]+sMgridTmp.ppaMat[j][k]+sMgridTmp.ppaMat[j+1][k+1]+sMgridTmp.ppaMat[j+1][k-1]);
        }
      }
    }
    /* Send results back to master */
    MPI_Send(&iFrom, 1, MPI_INT, MASTER, DONE, MPI_COMM_WORLD);
    MPI_Send(&iRows, 1, MPI_INT, MASTER, DONE, MPI_COMM_WORLD);
    MPI_Send(&(sMgrid.ppaMat[0][0]), iRows*iCols, MPI_DOUBLE, MASTER, DONE, MPI_COMM_WORLD);
    /* Free allocated memory */
    vFreeMatrixDouble(&sMgrid);
    vFreeMatrixDouble(&sMgridTmp);
  }
  MPI_Finalize();
  return EXIT_SUCCESS;
}

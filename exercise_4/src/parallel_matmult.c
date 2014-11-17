/*********************************************************************************
 * FILENAME         parallel_matmult.c
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
 * LAST CHANGE      17. NOV 2014
 *
 ********************************************************************************/

#include "matrix_multiply.h"
#include "distribution.h"
#include <mpi.h>
#define _DEBUG_ true

void vSendJobToProc(sMatrix *pMatA, sMatrix *pMatB, sJob *pJob, int iDest, bool bBlock) {
    
    if (_DEBUG_) {
        printf("vSendJobToProc: got the following Job Nr %d\n",pJob->iJob);
        printf("\tpJob->iRowBegin = %d, pJob->iRowEnd = %d, pJob->iColBegin = %d, pJob->iColEnd = %d\n", pJob->iRowBegin, pJob->iRowEnd, pJob->iColBegin, pJob->iColEnd);
    }

    sJob JobA = {
        .iJob      = 0,
        .iRowBegin = pJob->iRowBegin,
        .iRowEnd   = pJob->iRowEnd,
        .iColBegin = 0,
        .iColEnd   = pMatA->iCol};
    sJob JobB = {
        .iJob = 0,
        .iRowBegin = 0,
        .iRowEnd = pMatB->iRow,
        .iColBegin = pJob->iColBegin,
        .iColEnd = pJob->iColEnd};

    int iRowA = JobA.iRowEnd - JobA.iRowBegin;
    int iColA =  JobA.iColEnd - JobA.iColBegin;
    int iRowB = JobB.iRowEnd - JobB.iRowBegin;
    int iColB = JobB.iColEnd - JobB.iColBegin;
    int mpiTag = 4;
    MPI_Request request;

    if (_DEBUG_) {
        printf("vSendJobToProc: going to send the following matrix sizes:\n");
        printf("\tiRowA = %d, iColA = %d, iRowB = %d, iColB = %d\n", iRowA, iColA, iRowB, iColB);
        printf("\tpMatA->iCol = %d, pMatA->iRow = %d, pMatB->iCol = %d, pMatB->iRow = %d\n", pMatA->iCol, pMatA->iRow, pMatB->iCol, pMatB->iRow);
    }
    /* use blocking send commands */
    if (bBlock) {
        /* send row count for Job of Mat A */
        MPI_Send(&iRowA, 1, MPI_INT, iDest, 0, MPI_COMM_WORLD); 
        /* send col count for Job of Mat A */
        MPI_Send(&iColA, 1, MPI_INT, iDest, 1, MPI_COMM_WORLD); 
        /* send row count for Job of Mat B */
        MPI_Send(&iRowB, 1, MPI_INT, iDest, 2, MPI_COMM_WORLD); 
        /* send col count for Job of Mat B */
        MPI_Send(&iColB, 1, MPI_INT, iDest, 3, MPI_COMM_WORLD); 

        /* Data for Matrix A */
        for (int i = JobA.iRowBegin; i < JobA.iRowEnd; i++) {
            MPI_Send(pMatA->ppaMat[i], iColA, MPI_INT, iDest, mpiTag++, MPI_COMM_WORLD);
        }

        /* Data for Matrix B */
        for (int i = JobB.iRowBegin; i < JobB.iRowEnd; i++) {
            MPI_Send(pMatB->ppaMat[i], iColB, MPI_INT, iDest, mpiTag++, MPI_COMM_WORLD);
        }
    }

    /* use non-blocking send commands */
    else {
        /* send row count for Job of Mat A */
        MPI_Isend(&iRowA, 1, MPI_INT, iDest, 0, MPI_COMM_WORLD, &request); 
        /* send col count for Job of Mat A */
        MPI_Isend(&iColA, 1, MPI_INT, iDest, 1, MPI_COMM_WORLD, &request); 
        /* send row count for Job of Mat B */
        MPI_Isend(&iRowB, 1, MPI_INT, iDest, 2, MPI_COMM_WORLD, &request); 
        /* send col count for Job of Mat B */
        MPI_Isend(&iColB, 1, MPI_INT, iDest, 3, MPI_COMM_WORLD, &request); 

        /* Data for Matrix A */
        for (int i = JobA.iRowBegin; i < JobA.iRowEnd; i++) {
            MPI_Isend(pMatA->ppaMat[i], iColA, MPI_INT, iDest, mpiTag++, MPI_COMM_WORLD, &request);
        }

        /* Data for Matrix B */
        for (int i = JobB.iRowBegin; i < JobB.iRowEnd; i++) {
            MPI_Isend(pMatB->ppaMat[i], iColB, MPI_INT, iDest, mpiTag++, MPI_COMM_WORLD, &request);
        }
    }
}

void vRecvJobFromProc(sMatrix *pMatA, sMatrix *pMatB, int mpiSource, bool mpiBlock) {

    int mpiTag = 4;
    MPI_Request mpiRequest0, mpiRequest1, mpiRequest2, mpiRequest3;
    MPI_Status mpiStatus;

    // blocking recieve
    if (mpiBlock) {
        /* row number mat A */
        MPI_Recv(&(pMatA->iRow), 1, MPI_INT, mpiSource, 0, MPI_COMM_WORLD, &mpiStatus);
        /* col number mat A */
        MPI_Recv(&(pMatA->iCol), 1, MPI_INT, mpiSource, 1, MPI_COMM_WORLD, &mpiStatus);
        /* row number mat B */
        MPI_Recv(&(pMatB->iRow), 1, MPI_INT, mpiSource, 2, MPI_COMM_WORLD, &mpiStatus);
        /* col number mat B */
        MPI_Recv(&(pMatB->iCol), 1, MPI_INT, mpiSource, 3, MPI_COMM_WORLD, &mpiStatus);

        if (_DEBUG_) {
            printf("vRecvJobFromProc: got the following matrix sizes (blocking)\n");
            printf("\tpMatA->iRow = %d, pMatA->iCol = %d\n", pMatA->iRow, pMatA->iCol);
            printf("\tpMatB->iRow = %d, pMatB->iCol = %d\n", pMatB->iRow, pMatB->iCol);
        }
        /* allocate memory for the matrices */
        vAllocMatrix(pMatA, pMatA->iRow, pMatA->iCol);
        vAllocMatrix(pMatB, pMatB->iRow, pMatB->iCol);

        /* Data for Matrix A */
        for (int i = 0; i<pMatA->iRow; i++) {
            MPI_Recv(pMatA->ppaMat[i], pMatA->iCol, MPI_INT, mpiSource, mpiTag++, MPI_COMM_WORLD, &mpiStatus);
        }
        /* Data for Matrix B */
        for (int i = 0; i<pMatB->iRow; i++) {
            MPI_Recv(pMatB->ppaMat[i], pMatB->iCol, MPI_INT, mpiSource, mpiTag++, MPI_COMM_WORLD, &mpiStatus);
        }
    }

    /* non-blocking recieve */
    else {
        /* row number mat A */
        MPI_Irecv(&(pMatA->iRow), 1, MPI_INT, mpiSource, 0, MPI_COMM_WORLD, &mpiRequest0);
        /* col number mat A */
        MPI_Irecv(&(pMatA->iCol), 1, MPI_INT, mpiSource, 1, MPI_COMM_WORLD, &mpiRequest1);
        /* row number mat B */
        MPI_Irecv(&(pMatB->iRow), 1, MPI_INT, mpiSource, 2, MPI_COMM_WORLD, &mpiRequest2);
        /* col number mat B */
        MPI_Irecv(&(pMatB->iCol), 1, MPI_INT, mpiSource, 3, MPI_COMM_WORLD, &mpiRequest3);

        /*
        * MPI_Wait is needed, otherwise the iRow
        * and iCol values for allocation are
        * random values out of the memory.
        */
        MPI_Wait(&mpiRequest0, &mpiStatus);
        MPI_Wait(&mpiRequest1, &mpiStatus);
        MPI_Wait(&mpiRequest2, &mpiStatus);
        MPI_Wait(&mpiRequest3, &mpiStatus);

        if (_DEBUG_) {
            printf("vRecvJobFromProc: got the following matrix sizes (non-blocking)\n");
            printf("\tpMatA->iRow = %d, pMatA->iCol = %d\n", pMatA->iRow, pMatA->iCol);
            printf("\tpMatB->iRow = %d, pMatB->iCol = %d\n", pMatB->iRow, pMatB->iCol);
        }
        /* allocate memory for the matrices */
        vAllocMatrix(pMatA, pMatA->iRow, pMatA->iCol);
        vAllocMatrix(pMatB, pMatB->iRow, pMatB->iCol);

        /* Data for Matrix A */
        for (int i = 0; i<pMatA->iRow; i++) {
            MPI_Irecv(pMatA->ppaMat[i], pMatA->iCol, MPI_INT, mpiSource, mpiTag++, MPI_COMM_WORLD, &mpiRequest0);
            MPI_Wait(&mpiRequest0, &mpiStatus);
        }
        /* Data for Matrix B */
        for (int i = 0; i < pMatB->iRow; i++) {
            MPI_Irecv(pMatB->ppaMat[i], pMatB->iCol, MPI_INT, mpiSource, mpiTag++, MPI_COMM_WORLD, &mpiRequest0);
            MPI_Wait(&mpiRequest0, &mpiStatus);
        }
    }
}

void vSendResToProc(sMatrix *pMatC, int mpiDest) {

    int mpiTag = 0;
    for (int i = 0; i < pMatC->iRow; i++) {
        MPI_Send(pMatC->ppaMat[i], pMatC->iCol, MPI_INT, mpiDest, mpiTag++, MPI_COMM_WORLD);
    }
}

void vRecvResFromProc(sMatrix *pMatC, int mpiSource, sJob *pJob) {
  
    MPI_Status mpiStatus;
    int mpiTag = 0;
    int iColJ = pJob->iColEnd - pJob->iColBegin; // columns of job
    for (int i = pJob->iRowBegin; i < pJob->iRowEnd; i++) {
        MPI_Recv(pMatC->ppaMat[i], iColJ, MPI_INT, mpiSource, mpiTag++, MPI_COMM_WORLD, &mpiStatus);
    }
}

void vDoJob(sMatrix *pMa, sMatrix *pMb, sMatrix *pMc, sJob *pJob) {
    
    for (int i = pJob->iRowBegin; i < pJob->iRowEnd; i++) {
        for (int j = pJob->iColBegin; j < pJob->iColEnd; j++) {
            int sum = 0;
            for (int k = 0; k < pMa->iCol; k++) {
                sum += pMa->ppaMat[i][k] * pMb->ppaMat[k][j];
            }
            pMc->ppaMat[i][j] = sum;
        }
    }
}

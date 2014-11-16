/*********************************************************************************
 * FILENAME         main.c
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

#include "parallel_matmult.h"
#include "matrix_multiply.h"
#include "tests.h"
#include "distribution.h"
#include "chCommandLine.h"
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


const static char DEFAULT_DISTRIBUTION = 't'; // 't' = tree or 'r' = row
const static int DEFAULT_MAT_SIZE = 2000;
const static bool DEFAULT_SENDING_MAIN = false;
const static bool DEFAULT_VERIFY_RESULT = false;
const static char DEFAULT_SENDING = 'n'; // 'n' = non-blocking or 'b' = blocking

int main (int argc, char *argv[]) {

    int mpiSize;
    int mpiRank;
    MPI_Status mpiStatus;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpiSize);
 
/******** GET PROGRAM OPTIONS ********/

    /* blocking or non blocking send */
    bool optBlockingSend = (DEFAULT_SENDING == 'n') ? false : true;
    optBlockingSend = chCommandLineGetBool("b", argc, argv);

    bool optRunTests = chCommandLineGetBool("t", argc, argv);
    if (optRunTests) {
        printf("***RUNNING TESTS\n");
        test_vDistributeOutputMatrix();
        return 0;
    }

    /* optSendingMain = True -> Main process does not do calculations */
    bool optSendingMain = DEFAULT_SENDING_MAIN;
    optSendingMain = chCommandLineGetBool("sm", argc, argv);

    /* get the distribution method */
    bool optTreeDistribution = chCommandLineGetBool("td", argc, argv); 
    bool optRowDistribution = chCommandLineGetBool("rd", argc, argv);
    bool bTreeDistribution;
    if (optTreeDistribution & optRowDistribution)
        bTreeDistribution = (DEFAULT_DISTRIBUTION == 't') ? true : false;
    else if (optTreeDistribution)
        bTreeDistribution = true;
    else if (optRowDistribution)
        bTreeDistribution = false;
    else 
        bTreeDistribution = (DEFAULT_DISTRIBUTION == 't') ? true : false;

    /* verify with sequential results */
    bool optVerifyResult = DEFAULT_VERIFY_RESULT;
    optVerifyResult = chCommandLineGetBool("vr", argc, argv);

    /* get matrix size */
    int optMatSize = 0;
    chCommandLineGet<int>(&optMatSize, "s", argc, argv);
    optMatSize = (optMatSize != 0) ? optMatSize : DEFAULT_MAT_SIZE;

    

/******** START CALCULATIONS ********/
    
    if (mpiRank == 0) {

        sMatrix sMa, sMb, sMc;
        int iSize = optMatSize;
        int iWorkers; // working processes 
        sJobList jobList;
        double dStartTime, dEndTime;
    
        /* allocate memory for matrices */
        if(vAllocMatrix(&sMa, iSize, iSize))
            exit(1);
        if(vAllocMatrix(&sMb, iSize, iSize))
            exit(1);
        if(vAllocMatrix(&sMc, iSize, iSize))
            exit(1);
    
        /* initialize matrix A & B */
        vInitMatrixA(&sMa);
        vInitMatrixB(&sMb);
    
        /* determine the number of working processes */
        if (optSendingMain)
            iWorkers = mpiSize - 1;
        else 
            iWorkers = mpiSize;
   
        /* initialize job list */
        vAllocateJobList(iWorkers, &jobList);

       /* distribute output matrix with wanted sheme */
        if (bTreeDistribution) { 
            vDistributeOutputMatrix(iSize, iSize, &jobList);
        }
        else {
            printf("***\n***ERROR: row sheme not available yet\n***\n");
            exit(1);
        }

        /* start time measurement */
        dStartTime = MPI_Wtime();

        /* 
        * send jobs to other processes 
        * if main calculating: job nr 0 is reserved for main process.
        */
        for (int i = 1; i < mpiSize; i++) {
            int j = (iWorkers == mpiSize) ? i : (i-1);
            vSendJobToProc(&sMa, &sMb, jobList.ppJob[j], i, optBlockingSend);
        }

        /* if not sending main calculate the right part */
        if (!optSendingMain) {
            vDoJob(&sMa, &sMb, &sMc, jobList.ppJob[0]);
        }

        /* get the calculation of other processes */
        for (int i = 1; i < mpiSize; i++) {
            int j = (iWorkers == mpiSize) ? i : (i-1);
            vRecvResFromProc(&sMc, i, jobList.ppJob[j]);
        }

        /* stop time measurement */
        dEndTime = MPI_Wtime();
        
        /* free allocated memory */
        vFreeJobList(&jobList);
        vFreeMatrix(&sMa);
        vFreeMatrix(&sMb);
        vFreeMatrix(&sMc);
    }
    
    /* if not main process */
    else {
        sMatrix sMa, sMb, sMc;

        /* get data from main process and allocate memory */
        vRecvJobFromProc(&sMa, &sMb, 0, optBlockingSend);
        
        /* allocate memory for result matrix */
        if (vAllocMatrix(&sMc, sMa.iRow, sMb.iCol))
            exit(1);
        
        /* execute the multiplication */
        iMatrixMultiply(&sMa, &sMb, &sMc);
    
        /* send result back to main process */        
        vSendResToProc(&sMc, 0);

        /* free allocated memory */
        vFreeMatrix(&sMa);
        vFreeMatrix(&sMb);
        vFreeMatrix(&sMc);
    }

    return 0;
}

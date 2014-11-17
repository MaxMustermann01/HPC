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
 * LAST CHANGE      17. NOV 2014
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

#define _DEBUG_ false

const static char DEFAULT_DISTRIBUTION = 't'; // 't' = tree or 'r' = row
const static int DEFAULT_MAT_SIZE = 10;
const static bool DEFAULT_SENDING_MAIN = false;
const static bool DEFAULT_VERIFY_RESULT = false;
const static char DEFAULT_SENDING = 'n'; // 'n' = non-blocking or 'b' = blocking
const static bool DEFAULT_PRINT_OUTPUT = false;

void printHelp();

int main (int argc, char *argv[]) {

    bool optHelp = false;
    optHelp = chCommandLineGetBool("h", argc, argv);
    if (optHelp) {
        printHelp();    
        return 0;
    }
        
    
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

    /* get output parameter */
    bool optPrintMat = DEFAULT_PRINT_OUTPUT;
    optPrintMat = chCommandLineGetBool("p", argc, argv);

    /* compare with sequential time */
    bool optCompareSeq = false;
    optCompareSeq = chCommandLineGetBool("c", argc, argv);

/******** START CALCULATIONS ********/
    
    if (mpiRank == 0) {

        sMatrix sMa, sMb, sMc;
        int iSize = optMatSize;
        int iWorkers; // working processes 
        sJobList jobList;
        double dStartTime, dEndTime, dTime;
        
        if (_DEBUG_) 
            printf("%d:main: going to allocate memory for matrices\n",mpiRank);

        /* allocate memory for matrices */
        if(vAllocMatrix(&sMa, iSize, iSize))
            exit(1);
        if(vAllocMatrix(&sMb, iSize, iSize))
            exit(1);
        if(vAllocMatrix(&sMc, iSize, iSize))
            exit(1);
   
        if (_DEBUG_)
            printf("0:main: going to initialize matrix A and B\n");

        /* initialize matrix A & B */
        vInitMatrixA(&sMa);
        vInitMatrixB(&sMb);
    
        /* determine the number of working processes */
        if (optSendingMain)
            iWorkers = mpiSize - 1;
        else 
            iWorkers = mpiSize;
  
        /* initialize job list */
        if (_DEBUG_)
            printf("0:main: going to allocate memory for job list\n");
        vAllocateJobList(iWorkers, &jobList);

       /* distribute output matrix with wanted sheme */
        if (_DEBUG_)
            printf("0:main: going to calculate the job distribution\n");

        if (bTreeDistribution) { 
            vDistributeOutputMatrix(iSize, iSize, &jobList);
        }
        else {
            printf("***\n***ERROR: row sheme not available yet\n***\n");
            exit(1);
        }
        
        if (_DEBUG_) {
            printf("0:main: got the following jobList\n");
            vPrintJobList(&jobList);
        }
        /* start time measurement */
        if (_DEBUG_)
            printf("0:main: going to start time measurement\n");
        dStartTime = MPI_Wtime();

        /* 
        * send jobs to other processes 
        * if main calculating: job nr 0 is reserved for main process.
        */
        if (_DEBUG_)
            printf("0:main: going to send jobs to other processes\n");
        
        int j;
        for (int i = 1; i < mpiSize; i++) {
            j = (iWorkers == mpiSize) ? i : (i-1);
            vSendJobToProc(&sMa, &sMb, jobList.ppJob[j], i, optBlockingSend);
        }

        /* if not sending main calculate the right part */
        if (!optSendingMain) {
            vDoJob(&sMa, &sMb, &sMc, jobList.ppJob[0]);
        }

        /* get the calculation of other processes */
        if (_DEBUG_)
            printf("0:main: going to recieve the matrix data from other processes\n");

        for (int i = 1; i < mpiSize; i++) {
            j = (iWorkers == mpiSize) ? i : (i-1);
            vRecvResFromProc(&sMc, i, jobList.ppJob[j]);
        }

        /* stop time measurement */
        dEndTime = MPI_Wtime();
        dTime = dEndTime - dStartTime;

        /* print output if wanted */
        if (optPrintMat) {
            printf("The Resulting Matrix:\n");
            vPrintMatrix(&sMc);
            printf("\n\n");
        }
        
        /* 
        * Verification with sequential result if wanted 
        * or measure speedup
        */
        if (optVerifyResult || optCompareSeq ) {

            sMatrix sMseq;
            double dStartSeq, dEndSeq, dSeqTime; 

            if (vAllocMatrix(&sMseq, iSize, iSize))
                exit(1);

            dStartSeq = MPI_Wtime(); 

            iTilledMatrixMultiply(&sMa, &sMb, &sMseq, 32);

            dEndSeq = MPI_Wtime();
            dSeqTime = dEndSeq - dStartSeq;

            bool equal = bMatrixEqual(&sMseq, &sMc);
            
            /* output for verification */
            if (equal && optVerifyResult)
                printf("Verification with sequential result [OK]\n\n");
            else if (optVerifyResult)
                printf("Verification with sequential result [FAILED]\n\n");

            /* output for comparison */
            if (optCompareSeq) {
                printf("Comparison with sequential calculation time\n");
                printf("\tSequential time %.10f s\n", dSeqTime);
                printf("\tParallel (%d workers) time %.10f s\n", iWorkers, dTime);
                printf("\t->Speedup of %.5f\n",dSeqTime/dTime);
                printf("\t->Efficiency %.5f\n\n",dSeqTime/(dTime * ((double) iWorkers)));
            }

        }
       
        /* standard output */
        
        /*
        * per element in output matrix we have iSize
        * multiplications and iSize-1 additions
	* NOTE: Günther: In this implementation we actually have iSize additions as well.
	*                Look at the algorithm!
        */
        //double dGFLOPS = 1e-9*((double) (iSize*iSize*(iSize+iSize-1)))/dTime;
        double dGFLOPS = 1e-9*((double) (iSize*iSize*(2*iSize)))/dTime;
        printf("Parallel matrix multiplication done with\n");
        printf("\tWorkers %d\n",iWorkers); 
        printf("\tProcesses %d\n",mpiSize);
        printf("\tMatrix height and width %d\n",iSize);
        printf("\tTime nedded %.10f s\n",dTime);
        printf("\tGFLOP/s %e\n\n", dGFLOPS);

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
        if (_DEBUG_)
            printf("%d:main: going to recieve jobs\n",mpiRank);

        vRecvJobFromProc(&sMa, &sMb, 0, optBlockingSend);
        
        /* allocate memory for result matrix */
        if (_DEBUG_) {
            printf("%d:main: going to allocate memory for resulting matrix\n\tsMa.iRow = %d, sMb.iCol =%d\n",mpiRank, sMa.iRow, sMb.iCol);
        }
        if (vAllocMatrix(&sMc, sMa.iRow, sMb.iCol))
            exit(1);
        
        /* execute the multiplication */
        if (_DEBUG_)
            printf("%d:main: going to execute matrix multiplication with\n\tsMa.iRow = %d, sMa.iCol = %d, sMb.iRow = %d, sMb.iCol = %d\n", mpiRank, sMa.iRow, sMa.iCol, sMb.iRow, sMb.iCol);

        iMatrixMultiply(&sMa, &sMb, &sMc);
    
        /* send result back to main process */        
        if (_DEBUG_) {
            printf("%d:main: going to send result back to main process\n",mpiRank);
            printf("%d:main: Matrix looks like\n",mpiRank); 
            vPrintMatrix(&sMc);
        }
        vSendResToProc(&sMc, 0);

        /* free allocated memory */
        vFreeMatrix(&sMa);
        vFreeMatrix(&sMb);
        vFreeMatrix(&sMc);
    }
    MPI_Finalize();
    return 0;
}

void printHelp() {
    printf("Usage:\n");
    printf("\tmpirun [-np <number of processes>] [-host <hosts,>] <programName>\n");
    printf("\t\t[-s <matrix size>]\n");
    printf("\t-p\n\t  print output matrix\n");
    printf("\t-sm\n\t  main process does not do matrix calculations\n");
    printf("\t-vr\n\t  verify result with a sequential execution\n");
    printf("\t-c\n\t  compare parallel with sequential execution time\n");
    printf("\t-b\n\t  use a blocking mpi send\n");
    printf("\t-t\n\t  run only tests in tests.c\n");
    printf("\t-td\n\t  use the tree sheme to distribute the jobs (default)\n");
    printf("\t-rd\n\t  use the row sheme to distribute the jobs (not implmented yet)\n");
}

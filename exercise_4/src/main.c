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
 * LAST CHANGE      14. NOV 2014
 *
 ********************************************************************************/

#include "tests.h"
#include "distribution.h"
#include "chCommandLine.h"
#include <stdio.h>
#include <mpi.h>
#include "parallel_matmult.h"

const static char DEFAULT_DISTRIBUTION = 't'; // 't' = tree or 'r' = row
const static int DEFAULT_MAT_SIZE = 2000;
const static bool DEFAULT_SENDING_MAIN = false;
const static bool DEFAULT_VERIFY_RESULT = false;

int main (int argc, char *argv[]) {

    int mpiSize;
    int mpiRank;
    MPI_Status mpiStatus;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpiSize);
 
/******** GET PROGRAM OPTIONS ********/
    if (mpiRank == 0) {

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
    }

/******** START CALCULATIONS ********/

    return 0;
}

/*********************************************************************************
 * FILENAME         tests.c
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
#include <stdio.h>
#include <stdlib.h>

void test_vDistributeOutputMatrix() {

    char strFunc[] = "DistributeOutputMatrix";
    char strAllocErr[] = "Allocation Error";
    char strNULL[] = " ";
    int failed = 0;
/*
 *  1x1 Matrix
 *  1 Job
 */
    sJobList jobList;
    
    if (vAllocateJobList(1, &jobList)) {
        vPrintError(strFunc, strAllocErr);
        failed = 1;
        exit(EXIT_FAILURE);
    }

    printf("Try to access jobList->iTotalJobs\n");
    printf("TESTS.C: jobList.iTotalJobs = %d\n",jobList.iTotalJobs);
    printf("sizeof(sJobList) = %u\n",sizeof(sJobList));
    printf("sizeof(sJobList*) = %u\n",sizeof(sJobList*));
    vDistributeOutputMatrix(1,1,&jobList);

    if (jobList.iTotalJobs != 1) {
        failed = 1;
        vPrintError(strFunc, strNULL);
    }
    if (!failed) vPrintChecked(strFunc);
}


void vPrintError(char *strFuncName, char *strMsg) {
   printf("***\n***ERROR: function '%s'\n ***\t%s\n ***\n", strFuncName, strMsg); 
}

void vPrintChecked(char *strFuncName) {
    printf("*** TEST '%s' [OK]\n", strFuncName);
}

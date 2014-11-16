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

    vDistributeOutputMatrix(1,1,&jobList);

    if (jobList.iTotalJobs != 1) {
        failed = 1;
        char report1[] = "1x1 Matrix with 1 Job failed (total jobs)";
        vPrintError(strFunc, report1);
    }
    if (jobList.ppJob[0]->iRowBegin != 0) {
        failed = 1;
        char report1[] = "1x1 Matrix with 1 Job failed (row begin)";
        vPrintError(strFunc, report1);
    }
    if (jobList.ppJob[0]->iRowEnd != 1) {
        failed = 1;
        char report1[] = "1x1 Matrix with 1 Job failed (row end)";
        vPrintError(strFunc, report1);
    }
    if (jobList.ppJob[0]->iColBegin != 0) {
        failed = 1;
        char report1[] = "1x1 Matrix with 1 Job failed (col begin)";
        vPrintError(strFunc, report1);
    }
    if (jobList.ppJob[0]->iColEnd != 1) {
        failed = 1;
        char report1[] = "1x1 Matrix with 1 Job failed (col end)";
        vPrintError(strFunc, report1);
    }
    vFreeJobList(&jobList);

    /*
     *  3x3 Matrix
     *  2 Jobs
     */
    sJobList jobList2;

    if (vAllocateJobList(2, &jobList2)) {
        vPrintError(strFunc, strAllocErr);
        failed = 1;
        exit(EXIT_FAILURE);
    }

    vDistributeOutputMatrix(3,3,&jobList2);

    /* first job */
    if (jobList2.iTotalJobs != 2) {
        failed = 1;
        char report1[] = "3x3 Matrix with 2 Jobs failed (total job count)";
        vPrintError(strFunc, report1);
    }
    if (jobList2.ppJob[0]->iRowBegin != 0) {
        failed = 1;
        char report1[] = "3x3 Matrix with 2 Jobs failed (row begin)";
        vPrintError(strFunc, report1);
    }
    if (jobList2.ppJob[0]->iRowEnd != 1) {
        failed = 1;
        char report1[] = "3x3 Matrix with 2 Jobs failed (row end job0)";
        vPrintError(strFunc, report1);
    }
    if (jobList2.ppJob[0]->iColBegin != 0) {
        failed = 1;
        char report1[] = "3x3 Matrix with 2 Jobs failed (col begin)";
        vPrintError(strFunc, report1);
    }
    if (jobList2.ppJob[0]->iColEnd != 3) {
        failed = 1;
        char report1[] = "3x3 Matrix with 2 Jobs failed (col end)";
        vPrintError(strFunc, report1);
    }

    /* scond job */

    if (jobList2.ppJob[1]->iRowBegin != 1) {
        failed = 1;
        char report1[] = "3x3 Matrix with 2 Jobs failed (row begin)";
        vPrintError(strFunc, report1);
    }
    if (jobList2.ppJob[1]->iRowEnd != 3) {
        failed = 1;
        char report1[] = "3x3 Matrix with 2 Jobs failed (row end job1)";
        vPrintError(strFunc, report1);
    }
    if (jobList2.ppJob[1]->iColBegin != 0) {
        failed = 1;
        char report1[] = "3x3 Matrix with 2 Jobs failed (col begin)";
        vPrintError(strFunc, report1);
    }
    if (jobList2.ppJob[1]->iColEnd != 3) {
        failed = 1;
        char report1[] = "3x3 Matrix with 2 Jobs failed (col end)";
        vPrintError(strFunc, report1);
    }

    vFreeJobList(&jobList2);

    /* if everything worked */
    if (!failed) vPrintChecked(strFunc);
}


void vPrintError(char *strFuncName, char *strMsg) {
   printf("***\n***ERROR: function '%s'\n***\t%s\n***\n", strFuncName, strMsg); 
}

void vPrintChecked(char *strFuncName) {
    printf("*** TEST '%s' [OK]\n", strFuncName);
}

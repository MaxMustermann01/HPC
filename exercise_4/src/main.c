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

int main (int argc, char *argv[]) {

    printf("WERDE AUSGEFuert\n");
    int optRunTests = chCommandLineGetBool("t", argc, argv);
    if (optRunTests) {
        printf("***RUNNING TESTS\n");
        test_vDistributeOutputMatrix();
    }

    return 0;
}

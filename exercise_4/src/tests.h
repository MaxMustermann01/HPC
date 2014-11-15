/*********************************************************************************
 * FILENAME         tests.h
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

#ifndef TESTS_H
#define TESTS_H

/* 
*  Tests for distribution.c
*/
void test_vDistributeOutputMatrix ();

/*
* DESCRIPTION - Prints an Error of a function with
*               a Error message
* PARAMETER   - character*: name of the function
*             - character*: error message
* RETURN      - void
*/
void vPrintError(char*, char*);

/*
* DESCRIPTION - Prints if a function checked the test
* PARAMETER   - character*: function's name
* RETURN      - void
*/
void vPrintChecked(char*);

#endif

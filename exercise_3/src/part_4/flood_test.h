/*********************************************************************************
 * FILENAME         flood_test.h
 * 
 * DESCRIPTION      These functions are part of the submission to exercises of 
 *                  the "Introduction to High Percformance Computing" (Intro HPC) 
 *                  lecture of the University of Heidelberg.
 * 
 *                  Exercise 3 - Bandwidth between processes
 * 
 * AUTHORS          Klaus Naumann
 *                  Christoph Klein
 *
 * LAST CHANGE      9. NOV 2014
 * 
 ********************************************************************************/

#ifndef FLOOD_TEST_H
#define FLOOD_TEST_H

/*
* DESCRIPTION - This function returns the time that was needed for
*               sending a packet of a specific size to another process.
*               In total only two processes are allowed to execute this
*               program.
* PARAMETER   - integer: rank of the process (0 or 1)
*             - double: packet size
*             - integer: if non zero blocking mpi send is used. Otherwise
*               non blocking.
* RETURN      - the time which was needed to send the package.
*/
double flood(int, double, int);

#endif

/*********************************************************************************
 * FILENAME         time_measurement.c
 *
 * DESCRIPTION      These functions are part of the submission to exercises of 
 *                  the "Introduction to High Percformance Computing" (Intro HPC) 
 *                  lecture of the University of Heidelberg.
 * 
 *                  Exercise 5 - Heat relaxation
 * 
 * AUTHORS          Klaus Naumann
 *                  Christoph Klein
 *                  Günther Schindler
 *
 * LAST CHANGE      20. NOV 2014
 *
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "time_measurement.h"

/* Start time-measurement */
double dstartMesGTOD(void)
{
  struct timeval tim;
  gettimeofday(&tim, NULL);
  return tim.tv_sec+(tim.tv_usec/1000000.0);
}

/* Stop time-measurement */
double dstopMesGTOD(double dStartTime)
{
  struct timeval tim;
  gettimeofday(&tim, NULL);
  return (tim.tv_sec+(tim.tv_usec/1000000.0)) - dStartTime;
}
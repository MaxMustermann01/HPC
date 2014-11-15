/*********************************************************************************
* FILENAME         time_measurement.c
*
* DESCRIPTION      These functions are part of the submission to exercises of 
*                  the "Introduction to High Percformance Computing" (Intro HPC) 
*                  lecture of the University of Heidelberg.
* 
*                  Exercise 2 - Implementation of the time measurement
* 
* AUTHORS          Klaus Naumann
*                  Christoph Klein
*                  Günther Schindler
*
* LAST CHANGE      29. OKT 2014
*
********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "time_measurement.h"

/* Start time-measurement */
double dstartMeasurement(void)
{
  struct timeval tim;
  gettimeofday(&tim, NULL);
  return tim.tv_sec+(tim.tv_usec/1000000.0);
}

/* Stop time-measurement */
double dstopMeasurement(double dStartTime)
{
  struct timeval tim;
  gettimeofday(&tim, NULL);
  return (tim.tv_sec+(tim.tv_usec/1000000.0)) - dStartTime;
}
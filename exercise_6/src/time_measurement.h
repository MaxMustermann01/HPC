/*********************************************************************************
 * FILENAME         time_measurement.h
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

#ifndef TIME_MEASUREMENT_H
#define TIME_MEASUREMENT_H

/*
 * DESCRIPTION - Starts a time-measurement, based on the gettimeofday() functions
 *               It has a resolution up to one microsecond.
 * PARAMETER   - void
 * RETURN      - double: elapsed seconds this day (is the parameter for dstopMesGTOD())
 */
double dstartMesGTOD(void);

/*
 * DESCRIPTION - Stops the time-measurement, based on the gettimeofday() functions.
 *               It has a resolution up to one microsecond.
 * PARAMETER   - double: return-value of dstartMesGTOD()
 * RETURN      - double: elapsed seconds since dstartMesGTOD()
 */
double dstopMesGTOD(double);

#endif
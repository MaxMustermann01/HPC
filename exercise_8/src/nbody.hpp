/*************************************************************************************************
 *
 * Heidelberg University - IntroHPC Exercise 07
 *
 * Group :          IntroHPC03
 * Participant :    Klaus Naumann
 *				    Christoph Klein
 *                  Günther Schindler
 *
 * File :           nbody.hpp
 *
 * Purpose :        N-Body Simulation (3D)
 *
 *************************************************************************************************/
#ifndef NBODY_HPP
#define NBODY_HPP

#define G 0.00000000006673

typedef struct sBody {
  double dM;		// mass
  double dPx;		// x-position
  double dPy;		// y-position
  double dPz;       // z-position
  double dVx;		// velocity in x-direction
  double dVy;		// velocity in y-direction
  double dVz;       // velocity in z-direction
} sBody;

/*************************************************************************************************
* Function description: 	allocates memory for body and assign random values to mass, position;
*                           assign 0.0 to velocity values
*
* Parameter:			    Integer:	amount of bodies
*                           Integer:    max value for random-function
*
* Return:		            sBody*:     initiated body
*************************************************************************************************/
sBody* initBody(int, int);

/*************************************************************************************************
* Function description: 	Calculates distance between two bodies (not used here)
*
* Parameter:			    sBody*:	    body 1
*                           sBody*:     body 2
*                           Double:
*
* Return:		            Double:     distance
*************************************************************************************************/
//double distance(double, double);

/*************************************************************************************************
* Function description: 	Calculates new velocity for body
*
* Parameter:			    sBody*:	    body 1
*                           sBody*:     body 2
*                           Double:     dt - time step
*                           Double:     softening for graviational potential
*
* Return:		            Void
*************************************************************************************************/
void applyForce(sBody *, sBody *, double, double);

/*************************************************************************************************
* Function description: 	Calculates new position for body
*
* Parameter:			    sBody*:	    body 1
*                           Double:     dt - time step
*
* Return:		            Void
*************************************************************************************************/
void newPos(sBody *, double);

/*************************************************************************************************
* Function description: 	Prints out all values of body
*
* Parameter:			    sBody*:	    body 1
*
* Return:		            Void
*************************************************************************************************/
void outBody(sBody *);

#endif

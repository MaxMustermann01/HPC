/*************************************************************************************************
 *
 * Heidelberg University - IntroHPC Exercise 08
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

//#define G 0.00000000006673
#define G 1.0

// if you change this structure you have to
// adjust the macro in communication.hpp for the
// mpi datatype
typedef struct sBody {
  double dM;		// mass
  double dPx;		// x-position
  double dPy;		// y-position
  double dPz;       // z-position
  double dVx;		// velocity in x-direction
  double dVy;		// velocity in y-direction
  double dVz;       // velocity in z-direction
} sBody;

typedef struct sList {
    int size;
    sBody *pB;
} sList;

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
* Function description: 	Calculates new velocity for body 1
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

/*************************************************************************************************
* Function description: 	Calculates all the new positions for
*                           particle[jobOffset] to particle[jobOffset + jobSize - 1]
*
* Parameter:			    sList:      body list
*                           integer:    jobOffset
*                           integer:    jobSize
*                           double:     softening
*                           double:     stepsize dt
*
* Return:		            Void
*************************************************************************************************/
void calcPositions(sList, int, int, double, double);

/*************************************************************************************************
* Function description: 	synchronizes the positions between list1 and
*                           list2. In list1 every positions will be replaced
*                           by the positions from list2 except the 
*                           positions from particle[jobOffset] to
*                           particle[jobOffset + jobSize - 1].
*
* Parameter:			    sList:      list1
*                           sList:      list2
*                           integer:    jobOffset
*                           integer:    jobSize
*
* Return:		            Void
*************************************************************************************************/
void syncPositions(sList, sList, int, int);
#endif

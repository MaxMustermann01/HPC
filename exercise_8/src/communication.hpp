/*************************************************************************************************
 *
 * Heidelberg University - IntroHPC Exercise 08
 *
 * Group :          IntroHPC03
 * Participant :    Klaus Naumann
 *				    Christoph Klein
 *                  Günther Schindler
 *
 * File :          communication.hpp
 *
 * Purpose :        N-Body Simulation (3D)
 *
 *************************************************************************************************/

 #ifndef COMMUNICATION_HPP
 #define COMMUNICATION_HPP

#include "nbody.hpp"
#include "mpi.h"

// the mpi type definition must be done after a MPI_Init is called
// therefore a macro is used here
#define DEF_MPI_PARTICLE_TYPE(X) MPI_Datatype X, oldtypes[1]; int blockcounts[1]; MPI_Aint offsets[1]; offsets[0] = 0; oldtypes[0] = MPI_DOUBLE; blockcounts[0] = 7; MPI_Type_struct(1, blockcounts, offsets, oldtypes, &X); MPI_Type_commit(&X);


/*************************************************************************************************
* Function description:     sends the initial conditions to the next
*                           process. Before calling this function
*                           a MPI_Init is mandatory. The last
*                           process is not allowed to call this function.
*
* Parameter:			    sList:      body list
*                           integer:    process rank
*                           integer:    amount of processes
*
* Return:		            void
*************************************************************************************************/
void sendInitialConditions(sList, int, int);

/*************************************************************************************************
* Function description:     Gets the initial conditions for the n-body problem.
*
* Parameter:			    integer:    number of particles
*                           integer:    rank
*
* Return:		            void
*************************************************************************************************/
sList getInitialConditions(int, int);

/*************************************************************************************************
* Function description:     Recieves and synchronizes the own new positions
*                           with the positions from the neighbor process
*
* Parameter:			    sList:      body list
*                           integer:    jobOffset
*                           integer:    jobSize
*                           integer:    rank
*                           integer:    amount of processes
*
* Return:		            void
*************************************************************************************************/
void recvAndSyncPositions(sList, int, int, int, int);

/*************************************************************************************************
* Function description:     Sends the whole list of particles to the 
*                           neighbor process.
*
* Parameter:			    sList:      body list
*                           integer:    rank
*                           integer:    amount of processes
*
* Return:		            void
*************************************************************************************************/
void sendPositions(sList, int, int);

 #endif

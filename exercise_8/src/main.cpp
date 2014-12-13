/*************************************************************************************************
 *
 * Heidelberg University - IntroHPC Exercise 08
 *
 * Group :          IntroHPC03
 * Participant :    Klaus Naumann
 *				    Christoph Klein
 *                  Günther Schindler
 *
 * File :           main.cpp
 *
 * Purpose :        N-Body Simulation (3D)
 *
 *************************************************************************************************/
#include <iostream>
#include <cstdlib>
#include <cmath>

#include "nbody.hpp" // body calculations
#include "communication.hpp" // ring communication

using namespace std;

const double dt		= 0.01;			// step-size
const double iter	= 100;			// number of iterations
const double softening = 0.001;     // plummer softening


int main(int argc, char **argv) {

    int nbodies;
    int rank, mpiSize;

    /* get commandline parameters */
    if (argc != 2) {
        cout << "Wrong number of arguments!" << endl;
        cout << "Usage: " << endl;
        cout << "\t" << argv[0] << endl;
        cout << "\t<N number of bodies>" << endl;
        return EXIT_FAILURE;
    }
    else {
        nbodies = atoi(argv[1]);
    }
   
   /***************************
    * GET RANK, MPISIZE AND JOB
    ***************************/

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpiSize);
    
    // job size //
    int jobSize = nbodies / mpiSize;
    if (rank < nbodies % mpiSize)
        jobSize++;

    // job offset //        
    int jobOffset = rank * (nbodies / mpiSize) + nbodies % mpiSize;
    if (rank < nbodies % mpiSize)
        jobOffset -= (nbodies % mpiSize - rank);
    
   /*****************************
    * INITIALIZATION OF PARTICLES
    *****************************/
    sList list;
    list.size = nbodies;
    
    // master process generates initial conditions //
    if (rank == 0){
        sBody *bodys = initBody(nbodies, 10);
        list.pB = bodys;
    }
    // slaves just allocate memory //
    else
        list.pB = new sBody[nbodies];

   /*******************************
    * DISTRIBUTE INITIAL CONDITIONS
    *******************************/

    if (rank != 0)
        list = getInitialConditions(nbodies, rank);
    
    if (rank < mpiSize - 1)
        sendInitialConditions(list, rank, mpiSize);
   

   /*********************
    * N-BODY CALCULATIONS
    *********************/

    for (int i = 0; i < iter; i++) {
        calcPositions(list, jobOffset, jobSize, softening, dt); 
        
        // ring communication //
        if (rank != 0)
            recvAndSyncPositions(list, jobOffset, jobSize, rank, mpiSize);
        if (mpiSize > 1) 
            sendPositions(list, rank, mpiSize); 
        if (rank < mpiSize - 1)
            recvAndSyncPositions(list, jobOffset, jobSize, rank, mpiSize);
        if (rank < mpiSize - 2)
            sendPositions(list, rank, mpiSize); 
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}



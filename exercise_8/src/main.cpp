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

/***********************************************************************
 * POSSIBLE OPTIMIZATIONS
 *      - Replace macro in communication.hpp by adding
 *        an additional function parameter, which defines
 *        the used mpi datatype
 *      - modify the ring communication that only necessary 
 *        data is sent. At the moment every process sends
 *        its whole particle list to the neighbor process
 *      - add a function parameter to sendPositions, which
 *        allows to use a blocking or non-blocking send. This
 *        non-blocking send can be used for the last 
 *        send process in the for-loop in the topic N-BODY-CALCULATIONS.
 ***********************************************************************/

#include <iostream>
#include <cstdlib>
#include <cmath>

#include "nbody.hpp" // body calculations
#include "communication.hpp" // ring communication
#include "time_measurement.h"

// for output to binary file //
#include "coordinate.h"
#define XLENGTH 10
#define YLENGTH 10
#define ZLENGTH 10

using namespace std;

const double dt		= 0.01;			// step-size
const double iter	= 10000;			// number of iterations
// plummer softening - avoids large forces if particles are very close //
const double softening = 0.001;
const bool output = true;
const char filename[] = "./output.bin";
const double maxMass = 100;
const double minMass = 10;
const double maxCoord = 100;

int main(int argc, char **argv) {

    int nbodies;
    int rank, mpiSize;
    double dStartTime, dElapsedTime;
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

        sBody *bodys = initBody(nbodies, maxCoord, minMass, maxMass);
        list.pB = bodys;
    }
    // slaves just allocate memory //
    else
        list.pB = new sBody[nbodies];

   /*******************
    * INITIALIZE OUTPUT
    *******************/
    COORDINATE_SIZE cSize = {XLENGTH, YLENGTH, ZLENGTH};
    // file will be opened if output == true and rank == 0 //
    bool openFile = rank == 0 && output;
    coordinate data(filename, nbodies, iter, maxMass, minMass, cSize, openFile);
    for (int i = 0; i < nbodies; i++) {
        data.initMass(i, list.pB[i].dM);
    }

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
    dStartTime = dstartMesGTOD();
    for (int i = 0; i < iter; i++) {
        if (output && rank == 0)
            data.writeTimestep(list);
        calcPositions(list, jobOffset, jobSize, softening, dt); 
        
        // ring communication //
        if (rank != 0)
            recvAndSyncPositions(list, jobOffset, jobSize, rank, mpiSize);
        if (mpiSize > 1) 
            sendPositions(list, rank, mpiSize); 
        if (rank < mpiSize - 1)
            recvAndSyncPositions(list, jobOffset, jobSize, rank, mpiSize);
        if (rank < mpiSize - 2)
            sendPositions(list, rank, mpiSize); // here a non-blocking send is possible 
    }
    dElapsedTime = dstopMesGTOD(dStartTime);

   /****************
    * PROGRAM REPORT
    ****************/
    if (rank == 0) {
        cout << "***" << endl
             << "*** Report" << endl
             << "***" << endl
             << "***\tcalculation-time " << dElapsedTime << " s" << endl
             << "***\tamount-of-particles " << nbodies << endl
             << "***" << endl;
    }         
    MPI_Finalize();
    return EXIT_SUCCESS;
}



/*************************************************************************************************
 *
 * Heidelberg University - IntroHPC Exercise 08
 *
 * Group :          IntroHPC03
 * Participant :    Klaus Naumann
 *				    Christoph Klein
 *                  Günther Schindler
 *
 * File :          communication.cpp
 *
 * Purpose :        N-Body Simulation (3D)
 *
 *************************************************************************************************/

#include <iostream>

#include "communication.hpp"
#include "nbody.hpp"

using namespace std;

void sendInitialConditions(sList list, int rank, int mpiSize) {
    if (rank == mpiSize - 1) {
        cout << "***ERROR: sendInitialConditions: last process must not send" << endl;
        return;
    }
    
    // macro for definition of mpi particle
    DEF_MPI_PARTICLE_TYPE(mpi_particle) 
    
    int dest = rank + 1;
    int tag = 0;
    MPI_Send(list.pB, list.size, mpi_particle, dest, tag, MPI_COMM_WORLD);
}

sList getInitialConditions(int N, int rank) {
    if (rank == 0) {
        cout << "***ERROR: getInitialConditions: main process must not recieve" << endl;
    }

    // macro for definition of mpi particle //
    DEF_MPI_PARTICLE_TYPE(mpi_particle) 
    MPI_Status stat;

    int source = rank - 1;
    int tag = 0;
    sBody *pB = nullptr;
    pB = new sBody[N];
    MPI_Recv(pB, N, mpi_particle, source, tag, MPI_COMM_WORLD, &stat);
    
    // creating the return list //
    sList list;
    list.size = N;
    list.pB = pB;

    return list;
}

void recvAndSyncPositions(sList list, int jobOffset, int jobSize, int rank, int mpiSize) {

    // macro for definition of mpi particle //
    DEF_MPI_PARTICLE_TYPE(mpi_particle) 
    MPI_Status stat;

    int source = (rank - 1 + mpiSize) % mpiSize;
    int tag = 1;
    
    // create a particle list for recieving //
    sList recList;
    recList.size = list.size;
    recList.pB = new sBody[list.size];

    MPI_Recv(recList.pB, list.size, mpi_particle, source, tag, MPI_COMM_WORLD, &stat);   
    
    
    // synchonize the positions //
    syncPositions(list, recList, jobOffset, jobSize);
    
    // free memory //
    delete recList.pB;
}

void sendPositions(sList list, int rank, int mpiSize) {

    // macro for definition of mpi particle //
    DEF_MPI_PARTICLE_TYPE(mpi_particle) 

    int dest = (rank + 1 + mpiSize) % mpiSize;
    int tag = 1;   
    
    MPI_Send(list.pB, list.size, mpi_particle, dest, tag, MPI_COMM_WORLD);
}

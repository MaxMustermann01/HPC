/*************************************************************************************************
 *
 * Heidelberg University - IntroHPC Exercise 07
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

#include "nbody.hpp"
using namespace std;

const double dt		= 0.01;			// step-size
const double iter	= 1000;			// number of iterations
const double softening = 0.001;     // plummer softening

int main(int argc, char **argv) {

  int nbodies;

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

  /* Initialize bodies */
  sBody *bodys = initBody(nbodies, 10);

  for(int i = 0; i < iter; i++) {
    for(int m = 0; m < nbodies - 1; m++) {
      for(int n = m + 1; n < nbodies; n++) {
        applyForce(&bodys[m], &bodys[n], dt, softening);
      }
    }

    for(int x = 0; x < nbodies; x++) {
      newPos(&bodys[x], dt);
      if(i%100 == 0) {
        cout << "\n" << x << ". Body" << endl;
        outBody(&bodys[x]);
      }
    }

  }

  return EXIT_SUCCESS;
}



/*************************************************************************************************
 *
 * Heidelberg University - IntroHPC Exercise 08
 *
 * Group :          IntroHPC03
 * Participant :    Klaus Naumann
 *				    Christoph Klein
 *                  Günther Schindler
 *
 * File :           nbody.cpp
 *
 * Purpose :        N-Body Simulation (3D)
 *
 *************************************************************************************************/
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <random>
#include "stdio.h"
#include "nbody.hpp"
using namespace std;


/* create new bodies */
sBody* initBody(int nbodies, double maxCoord, double minMass, double maxMass) {
  
  // define the random number generator //
  double dMax = maxCoord;
  double dMin = 0;
  uniform_real_distribution<double> unif(dMin, dMax);
  default_random_engine re; 
  uniform_real_distribution<double> unif1(minMass, maxMass);
  default_random_engine re1;

  sBody *bodys = new sBody[nbodies];
    
  for(int i = 0; i < nbodies; i++) {
    bodys[i].dM =  unif1(re1);
    bodys[i].dPx = unif(re);
    bodys[i].dPy = unif(re);
    bodys[i].dPz = unif(re);
    bodys[i].dVx = 0.0;
    bodys[i].dVy = 0.0;
    bodys[i].dVz = 0.0;
  }

  return bodys;
}


/* applyForce */
void applyForce(sBody *m1, sBody *m2, double dt, double softening) {

  double distX = m2->dPx - m1->dPx;
  double distY = m2->dPy - m1->dPy;
  double distZ = m2->dPz - m1->dPz;
  double distSquare = abs(distX*distX + distY*distY + distZ*distZ + softening);

  double Fx = (G * m1->dM * m2->dM) / distSquare * distX;
  double Fy = (G * m1->dM * m2->dM) / distSquare * distY;
  double Fz = (G * m1->dM * m2->dM) / distSquare * distZ;

  double ax1 = Fx / m1->dM;
  double ay1 = Fy / m1->dM;
  double az1 = Fz / m1->dM;

  m1->dVx += ax1 * dt;
  m1->dVy += ay1 * dt;
  m1->dVz += az1 * dt;
}


/* newPos */
void newPos(sBody *body, double dt) {

  body->dPx += dt * body->dVx;
  body->dPy += dt * body->dVy;
  body->dPz += dt * body->dVz;
}


/* outBody */
void outBody(sBody *body) {
  printf("====================================\n");
  printf("Mass: \t\t %.3f \n", body->dM);
  printf("Position X: \t %.10f \n", body->dPx);
  printf("Position Y: \t %.10f \n", body->dPy);
  printf("Position Z: \t %.10f \n", body->dPz);
  printf("Velocity X: \t %.10f \n", body->dVx);
  printf("Velocity Y: \t %.10f \n", body->dVy);
  printf("Velocity Z: \t %.10f \n", body->dVz);
   printf("====================================\n");
}

void calcPositions(sList list, int jobOffset, int jobSize, double softening, double dt) {

    for (int i = jobOffset; i < jobOffset + jobSize; i++) {
        for (int j = 0; j < list.size; j++) {
            if (j != i)
                applyForce(&list.pB[i], &list.pB[j], dt, softening); 
        }
    }

    for (int i = jobOffset; i < jobOffset + jobSize; i++)
        newPos(&list.pB[i], dt); 
}

void syncPositions(sList list1, sList list2, int jobOffset, int jobSize) {

    if (list1.size != list2.size) {
        cout << "***ERROR: syncPositions: lists have uneqal length" << endl;
        return;
    }

    for (int i = 0; i < list1.size; i++) {
        if (i < jobOffset || i >= jobOffset + jobSize) {
            list1.pB[i].dPx = list2.pB[i].dPx;
            list1.pB[i].dPy = list2.pB[i].dPy;
            list1.pB[i].dPz = list2.pB[i].dPz;
            list1.pB[i].dVx = list2.pB[i].dVx;
            list1.pB[i].dVy = list2.pB[i].dVy;
            list1.pB[i].dVz = list2.pB[i].dVz;
        }
    }
}

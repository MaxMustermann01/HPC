/*************************************************************************************************
 *
 * Heidelberg University - IntroHPC Exercise 07
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
#include "stdio.h"
#include "nbody.hpp"
using namespace std;


/* create new bodies */
sBody* initBody(int nbodies, int max) {

  sBody *bodys = new sBody[nbodies];

  for(int i = 0; i < nbodies; i++) {
    bodys[i].dM = (double)(rand()%100);
    bodys[i].dPx = (double)(rand()%max + 1);
    bodys[i].dPy = (double)(rand()%max + 1);
    bodys[i].dPz = (double)(rand()%max + 1);
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

  double ax2 = - Fx / m2->dM;
  double ay2 = - Fy / m2->dM;
  double az2 = - Fz / m2->dM;

  m1->dVx += ax1 * dt;
  m1->dVy += ay1 * dt;
  m1->dVz += az1 * dt;

  m2->dVx += ax2 * dt;
  m2->dVy += ay2 * dt;
  m2->dVz += az2 * dt;
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


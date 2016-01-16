
#include <stdio.h>
#include <iostream>
#include <Halo.h>

int main(int argc, char** argv) {
  
  //typedef Grid<2> grid;
  
  Grid<int, 2, 1, 1> gd(argc, argv, 100, 100, 1);
  
  // Init grid
  
  // Init neighboors
  
  gd.scatter():
  
  for(...) {
    docalc();
    gd.exchange();
    gd.wait();
  }
  
  gd.gather();
  
  return 0;
}
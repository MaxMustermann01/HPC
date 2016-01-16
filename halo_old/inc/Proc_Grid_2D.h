#ifndef _PROCESS_GRID_2D_H_
#define _PROCESS_GRID_2D_H_

#include <iostream>
#include <mpi.h>

struct Proc_Grid_2D_t {
  static const int ndims = 2;	// Number of dimensions
  MPI_Comm m_communicator;
  int R, C;
  int r, c;
  int pid;
  
  /* Constructor that takes an MPI_Cart communicator, already configured and use it
   * to set up the process grid.
   */
  Proc_Grid_2D_t(MPI_Comm& _communicator) : m_communicator(_communicator) {
    create();
  }
  
  /* Default constructor */
  Proc_Grid_2D_t() { }
  
  /* Returns communicator */
  MPI_Comm communicator() const {
    return m_communicator;
  }

  /* Function to create the grid, internally */
  void create(void) {
    int dims[2] = {0, 0}, periods[2] = {1, 1}, coords[2] = {0, 0};
    MPI_Cart_get( m_communicator, 2, dims, periods, coords );
    MPI_Comm_rank( m_communicator, &pid );
    R = dims[0];
    C = dims[1];
    r = coords[0];
    c = coords[1];
  }
  
  void dims(int &t_R, int &t_C) const {
    t_R = R;
    t_C = C;
  }
  
  void coords(int &t_R, int &t_C) const {
    t_R = r;
    t_C = c;
  }
  
  int size(void) const {
    return R*C;
  }
  
  int getPID(void) const {
    return pid;
  }
  
  /* Returns the process ID of the process with relative coordinates (X, Y) with
   * respect to the caller process.
   */
  template <int X, int Y>
  int proc() const {
    return( proc(X, Y) );
  }
  
  int proc(int X, int Y) const {
    int coords[2], res;
    
    coords[0] = r + X;
    if( coords[0] < 0 || coords[0] >= R )
      return -1;
    
    coords[1] = c + Y;
    if( coords[1] < 0 || coords[1] >= C )
      return -1;
    
    MPI_Cart_rank( m_communicator, coords, &res );
    return res;
  }
};

#endif
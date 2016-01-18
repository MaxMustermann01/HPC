#ifndef _PROCESS_GRID_1D_H_
#define _PROCESS_GRID_1D_H_

#include <iostream>
#include <mpi.h>

struct Proc_Grid_1D_t {
  static const int ndims = 1;	// Number of dimensions
  MPI_Comm m_communicator;
  int R;
  int r;
  int pid;
  
  /* Constructor that takes an MPI_Cart communicator, already configured and use it
   * to set up the process grid.
   */
  Proc_Grid_1D_t(MPI_Comm& _communicator) : m_communicator(_communicator) {
    create();
  }
  
  /* Default constructor */
  Proc_Grid_1D_t() { }
  
  /* Returns communicator */
  MPI_Comm communicator() const {
    return m_communicator;
  }

  /* Function to create the grid, internally */
  void create(void) {
    int dims[1] = {0}, periods[1] = {1}, coords[1] = {0};
    MPI_Cart_get( m_communicator, 1, dims, periods, coords );
    MPI_Comm_rank( m_communicator, &pid );
    R = dims[0];
    r = coords[0];
  }
  
  void dims(int &t_R) const {
    t_R = R;
  }
  
  void coords(int &t_R) const {
    t_R = r;
  }
  
  int size(void) const {
    return R;
  }
  
  int getPID(void) const {
    return pid;
  }
  
  /* Returns the process ID of the process with relative coordinates (X, Y) with
   * respect to the caller process.
   */
  /*template <int Y>
  int proc(int Y) const {
    return( proc(Y) );
  }*/
  
  int proc(int Y) const {
    int coords[1], res;
    
    coords[0] = r + Y;
    if( coords[0] < 0 || coords[0] >= R )
      return -1;
    
    MPI_Cart_rank( m_communicator, coords, &res );
    return res;
  }
};

#endif
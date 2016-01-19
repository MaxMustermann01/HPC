#ifndef _PROCESS_GRID_H_
#define _PROCESS_GRID_H_

#include <iostream>
#include <mpi.h>

template <int DIMS>
class Proc_Grid {
private:
  MPI_Comm m_communicator;
  int R, C, Z;
  int r, c, z;
  int pid;
  
  /* Function to create the grid, internally */
  void create() {
    int dims[DIMS], periods[DIMS], coords[DIMS];
    std::fill(&dims[0], &dims[DIMS], 0);
    std::fill(&periods[0], &periods[DIMS], 1);
    std::fill(&coords[0], &coords[DIMS], 0);
    
    MPI_Cart_get( m_communicator, DIMS, dims, periods, coords );
    MPI_Comm_rank( m_communicator, &pid );
    
    R = dims[0];
    r = coords[0];
    if (DIMS == 2 || DIMS == 3) { 
      C = dims[1];
      c = coords[1];
    }
    if (DIMS == 3) {
      Z = dims[2];
      z = coords[2];
    }
}
  
public:
  /* Constructor that takes an MPI_Cart communicator, already configured and use it
   * to set up the process grid.
   */
  Proc_Grid(MPI_Comm& _communicator) : m_communicator(_communicator) {
    create();
  }
  
  /* Default constructor */
  Proc_Grid() { }
  
  /* Returns communicator */
  MPI_Comm communicator() const {
    return m_communicator;
  }
  
  void dims(int &t_R) const {
    t_R = R;
  }

  void dims(int &t_R, int &t_C) const {
    t_R = R;
    t_C = C;
  }

  void dims(int &t_R, int &t_C, int &t_Z) const {
    t_R = R;
    t_C = C;
    t_Z = Z;
  }
  
  void coords(int &t_R) const {
    t_R = r;
  }

  void coords(int &t_R, int &t_C) const {
    t_R = r;
    t_C = c;
  }

  void coords(int &t_R, int &t_C, int &t_Z) const {
    t_R = r;
    t_C = c;
    t_Z = z;
  }
  
  /* Returns the process ID of the process with relative coordinates (X, Y) with
   * respect to the caller process.
   */
  int proc(int Y) const {
    int coords[1], res;
    
    coords[0] = r + Y;
    if( coords[0] < 0 || coords[0] >= R )
      return -1;
    
    MPI_Cart_rank( m_communicator, coords, &res );
    return res;
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
  
  int proc(int X, int Y, int Z) const {
    int coords[3], res;
   
    coords[0] = r + X;
    if( coords[0] < 0 || coords[0] >= R )
      return -1;
    
    coords[1] = c + Y;
    if( coords[1] < 0 || coords[1] >= C )
      return -1;
    coords[2] = z + Z;
    if(coords[2] < 0 || coords[2] >= Z )
      return -1;
    
    MPI_Cart_rank( m_communicator, coords, &res );
    return res;
  }
};

#endif
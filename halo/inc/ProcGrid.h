#ifndef _PROCGRID_H_
#define _PROCGRID_H_

#include <iostream>
#include <mpi.h>

/**
 * The Process Communication Grid. 
 * Does the mapping between the relative coordinates and the absolute memory address of a MPI-process communicating with its neighbours
 * 
 * @tparam DIMS the integer number of dimensions the processes are structured in; can be only 2 or 3.
 */
template <int DIMS>
struct Proc_Grid_t {  
  MPI_Comm m_communicator; /**< A MPI comminicator **/
  
  /* This currently only supported for maximum 3 dimensions */
  int R, C, Z; /**< an integer for each dimension storing the size of the grid in the associated dimension**/
  int r, c, z; /**< ?**/
  
  /**
   * Constructor for the ProcGrid. 
   * Takes an MPI_Comm communicator, already configured and uses it
   * to set up the process grid.
   * 
   * @param _communicator an object of type MPI_Comm
   */
  Proc_Grid_t(MPI_Comm _communicator) : m_communicator(_communicator) {
    create();
  }
  
  /**
   *Returns the communicator given initially
   *
   *@return m_communicator an object of type MPI_Comm 
   */
  MPI_Comm communicator() const {
    return m_communicator;
  }

  /** 
   * Internal function for initialization: Extracts the information about size, perdiodicity of the entire grid and the coordinates of the associated process from the given communicator.
   */
  void create(void) {
    
    int dims[DIMS], periods[DIMS], coords[DIMS];
    std::fill(&dims[0], &dims[DIMS], 0);
    std::fill(&periods[0], &periods[DIMS], 1);
    std::fill(&coords[0], &coords[DIMS], 0);
    
//     int dims[3] = {0, 0, 0}, periods[3] = {1, 1, 1}, coords[3] = {0, 0, 0};
    MPI_Cart_get( m_communicator, DIMS, &dims[0], &periods[0], &coords[0] );
    R = dims[0];
    C = dims[1];
    if (DIMS == 3) Z = dims[2];
    r = coords[0];
    c = coords[1];
    if (DIMS == 3) z = coords[2];
  }
  
  /**
   * Stores the inforamtion about the grid size for two dimensions at the given integer addresses
   * 
   * @param &t_R of type int 
   * @param &t_C of type int 
   */
void dims(int &t_R, int &t_C) const {
    t_R = R;
    t_C = C;
  }
  
  /**
   * Stores the inforamtion about the grid size for three dimensions at the given integer addresses
   * 
   * @param &t_R of type int 
   * @param &t_C of type int 
   * @param &t_Z of type int
   */
  void dims(int &t_R, int &t_C, int&t_Z) const {
    t_R = R;
    t_C = C;
    t_Z = Z;
  }
  
  /**
   * Stores the inforamtion about the process' absolute coordinates in the given integer addresses for the two-dimensional case
   * 
   * @param &t_R of type int
   * @param &t_C of type int 
   */
  void coords(int &t_R, int &t_C) const {
    t_R = r;
    t_C = c;
  }
  
  /**
   * Stores the inforamtion about the process' absolute coordinates in the given integer addresses for the three-dimensional case
   * 
   * @param &t_R of type int
   * @param &t_C of type int 
   * @param &t_Z of type int
   */  
  void coords(int &t_R, int &t_C, int &t_Z) const {
    t_R = r;
    t_C = c;
    t_Z = z;
  }
  

  template <int X, int Y>
  int proc() const {
    return( proc(X, Y) );
  }
  
  /*
   * Template for the 3-dimensional address mapping
   */
  template <int X, int Y, int Z>
  int proc() const {
    return( proc(X, Y, Z) );
  }
  
  /**
   * Returns the process ID, i.e. the absolute address, of the process with relative coordinates (X, Y) with
   * respect to the caller process for the 2-dimensional case.
   * 
   * @return The process ID
   * @returns -1 if there is no neighbouring process for the given relative coordinates
   */  
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
  
  /**
   * Returns the process ID, i.e. the absolute address, of the process with relative coordinates (X, Y, Z) with
   * respect to the caller process for the 3-dimensional case.
   * 
   * @return The process ID
   * @returns -1 if there is no neighbouring process for the given relative coordinates
   */  
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
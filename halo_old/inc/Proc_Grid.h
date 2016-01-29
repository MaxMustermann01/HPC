#ifndef _PROCESS_GRID_H_
#define _PROCESS_GRID_H_

#include <iostream>
#include <mpi.h>

/**
 * The mapping between relative coordinates and MPI process ids
 * An object of type Proc_Grid stores a mapping to process a given topological relative address and return the associated MPI process id.
 * 
 * @tparam DIMS an integer value representing the dimension of the grid (can only be 1, 2 or 3)
 */
template <int DIMS>
class Proc_Grid {
private:
  MPI_Comm m_communicator;		/**< MPI_Comm object containing the topology of the MPI proesses */
  int R, C, Z;				/**< Integer values storing the size of the grid in potentially 3 dimensions */
  int r, c, z;				/**< Integer values storing the position of one process in the grid in potentially 3 dimensions */
  int pid;				/**< Integer value: the MPI process id */
  
  /**
   * Creates the grid internally: identifies the size, periodicity of the grid and the position of the process and stores the information in the associated fields
   */
  void create() {
    int dims[DIMS], periods[DIMS], coords[DIMS];
    std::fill(&dims[0], &dims[DIMS], 0);
    std::fill(&periods[0], &periods[DIMS], 1);
    std::fill(&coords[0], &coords[DIMS], 0);
    
    MPI_Cart_get( m_communicator, DIMS, dims, periods, coords );
    MPI_Comm_rank( m_communicator, &pid );
    
    R = dims[0];
    r = coords[0];
    if (DIMS >= 2) {
      C = dims[1];
      c = coords[1];
    }
    if (DIMS == 3) {
      Z = dims[2];
      z = coords[2];
    }
}
  
public:
  /**
   * Constructor to initiliaze all fields
   * 
   * @param _communicator an object of type MPI_Comm, represents the mapping to the process ids
   */
  Proc_Grid(MPI_Comm& _communicator) : m_communicator(_communicator) {
    create();
  }
  
  /**
   * Default constructor 
   */
  Proc_Grid() { }
  
  /**
   * Returns communicator the stored MPI_Comm object
   *
  * @return the MPI_Comm object 
   */
  MPI_Comm communicator() const {
    return m_communicator;
  }
  
  /**
   * Retrieves the one-dimensional grid size and stores the value in a given fields
   * 
   * @param &t_R field where to store the information
   */
  void dims(int &t_R) const {
    t_R = R;
  }  
  
  /**
   * Retrieves the two-dimensional grid size and stores the value in a given fields
   * 
   * @param &t_R field where to store the information for the first dimension
   * @param &t_C field where to store the information for the second dimension
   */
  void dims(int &t_R, int &t_C) const {
    t_R = R;
    t_C = C;
  }

  /**
   * Retrieves the two-dimensional grid size and stores the value in a given fields
   * 
   * @param &t_R field where to store the information for the first dimension
   * @param &t_C field where to store the information for the second dimension
   * @param &t_Z field where to store the information for the third dimension
   */
  void dims(int &t_R, int &t_C, int &t_Z) const {
    t_R = R;
    t_C = C;
    t_Z = Z;
  }
  
  /**
   * Retrieves the poisition of the process in the one-dimensional grid
   * 
   * @param &t_R field where to store the information
   */
  void coords(int &t_R) const {
    t_R = r;
  }

  /**
   * Retrieves the poisition of the process in the two-dimensional grid
   *
   * @param &t_R field where to store the information for the first dimension
   * @param &t_C field where to store the information for the second dimension
   */
  void coords(int &t_R, int &t_C) const {
    t_R = r;
    t_C = c;
  }

  /**
   * Retrieves the poisition of the process in the three-dimensional grid
   *
   * @param &t_R field where to store the information for the first dimension
   * @param &t_C field where to store the information for the second dimension
   * @param &t_Z field where to store the information for the third dimension
   */
  void coords(int &t_R, int &t_C, int &t_Z) const {
    t_R = r;
    t_C = c;
    t_Z = z;
  }

  /**
   * @returns process id
   */
  int getprocid() {
    return pid;
  }
  
  /**
   * Returns the process ID of the process with given relative coordinates with
   * respect to the caller process in a one-dimensional grid
   * 
   * @param Y Integer value, relative position
   * @return Integer value, MPI process id
   */
  int proc(int Y) const {
    int coords[1], res;
    
    coords[0] = r + Y;
    if( coords[0] < 0 || coords[0] >= R )
      return -1;
    
    MPI_Cart_rank( m_communicator, coords, &res );
    return res;
  }

  /**
   * Returns the process ID of the process with given relative coordinates with
   * respect to the caller process in a two-dimensional grid
   * 
   * @param X Integer value, relative position first dimension
   * @param Y Integer value, relative position second dimension
   * @return Integer value, MPI process id
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
   * Returns the process ID of the process with given relative coordinates with
   * respect to the caller process in a two-dimensional grid
   * 
   * @param X Integer value, relative position first dimension
   * @param Y Integer value, relative position second dimension
   * @param Z Integer value, relative position third dimension
   * @return Integer value, MPI process id
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
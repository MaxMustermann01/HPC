#ifndef _TRANSMITBUFFER_H_
#define _TRANSMITBUFFER_H_

#include <mpi.h>
#include <cmath>
#include <stdexcept>
#include "Proc_Grid.h"
#include "Buffers.h"

  /*       (x, y, z)
   * 
   * 
   * 
   *                  ------------------------------- 
   *                 /         /         /         /|
   *                /         /         /         / |
   *               -------------------------------  | 
   *              /         /         /         /|  |
   *             /         /         /         / |  |
   *            -------------------------------  | /|
   *           /         /         /         /|  |/ |
   *  ^       /         /         /         / |  |  |
   *  |y / j -------------------------------  | /|  | 
   *  |      |         |         |         |  |/ |  |   
   *  |      |         |         |         |  |  | /|
   *  | 1    |         |         |         | /|  |/ |
   *  |      |         |         |         |/ |  |  |
   *  |      -------------------------------  | /|  |         ^
   *  |      |         |         |         |  |/ |  |        / z / k
   *  |      |         |         |         |  |  | /        /
   *  | 0    |         |         |         | /|  |/    1   /
   *  |      |         |         |         |/ |  |        /
   *  |      -------------------------------  | /        /
   *  |      |         |         |         |  |/    0   /
   *  |      |         |         |         |  |        /
   *  | -1   |         |         |         | /        /
   *  |      |         |         |         |/   -1   /
   *  |      -------------------------------        /
   *  |                                            / 
   *  |           -1        0        1            /                  x / i
   *  |-------------------------------------------------------------->
   * 
   */

  
/**
 * @brief Performs the data exchange
 * The TransmitBuffer executes non-blocking sending and receiving of data and the synchronization with the neighbouring MPI processes.
 * 
 * @tparam DIMS Arrangemet dimension of the MPI processes
 */  
template <int DIMS>
class TransmitBuffer {
  
  /**
   * Maps one-, two- or three-dimensional coordiantes to an associated one-dimensional position in the array of data buffers
   */
  struct request_t {
    MPI_Request request[(int)pow(3, DIMS)];		/**< an array of MPI_Request, storing references to neighbours in one array */
    /**
     * @param y relative one-dimensional coordinate
     * @return an MPI_Request for the associated neighbouring process 
     */
    MPI_Request& operator()(int y) { 
      if (y < -1 || y > 1) {
        throw std::out_of_range("Wrong indizes: must be -1 <= i <= 1!");
	exit(1);
      }
      return request[ y+1 ]; 
    }
    /** 
     * @param x relative coordinate first dimension
     * @param y relative coordinate second dimension
     * @return an MPI_Request for the associated neighbouring process 
     */
    MPI_Request& operator()(int x, int y) { 
      if (x < -1 || y < -1 || x > 1 || y > 1) {
        throw std::out_of_range("Wrong indizes: must be -1 <= i <= 1!");
	exit(1);
      }
      return request[ (x+1)*3 + (y+1) ];
    }
    /**
     * @param x relative coordinate first dimension
     * @param y relative coordinate second dimension
     * @param z relative coordinate third dimension
     * @return an MPI_Request for the associated neighbouring process 
     */
    MPI_Request& operator()(int x, int y, int z) { 
      if (x < -1 || y < -1 || z < -1 || x > 1 || y > 1 || z > 1) {
        throw std::out_of_range("Wrong indizes: must be -1 <= i <= 1!");
	exit(1);
      }
      return request[ (x+1)*9 + (y+1)*3 + z+1 ];
    }
  };
    /**
     * @param y relative one-dimensional coordinate
     * @return Integer index in an one-dimensional array
     */
  int getTag(int Y) {
    if (Y < -1 || Y > 1) {
      throw std::out_of_range("Wrong indizes: must be -1 <= i <= 1!");
    }
    return Y+1;
  }
    /** 
     * @param x relative coordinate first dimension
     * @param y relative coordinate second dimension
     * @return Integer index in an one-dimensional array
     */  
  int getTag(int X, int Y) {
    if (X < -1 || Y < -1 || X > 1 || Y > 1) {
      throw std::out_of_range("Wrong indizes: must be -1 <= i <= 1!");
    }
    return (X + 1)*3 + Y+1;
  }
    /**
     * @param x relative coordinate first dimension
     * @param y relative coordinate second dimension
     * @param z relative coordinate third dimension
     * @return Integer index in an one-dimensional array
     */
  int getTag(int X, int Y, int Z) {
    if (X < -1 || Y < -1 || Z < -1 || X > 1 || Y > 1 || Z > 1) {
      throw std::out_of_range("Wrong indizes: must be -1 <= i <= 1!");
    }
    return (X + 1)*9 + (Y+1)*3 + Z+1;
  }

  
private:
  
  const Proc_Grid<DIMS> m_proc_grid;			/**< Proc_Grid object: maps the relative coordinates to process ids */
  Buffers<(int)pow(3, DIMS)> m_send_buffers;		/**< Stores pointers to the send data buffes */
  Buffers<(int)pow(3, DIMS)> m_recv_buffers;		/**< Stores pointers to the receive data buffes */
  request_t send_request;				/**< Object to map relative coordinates to one-dimensional coordinates in the buffer array */
  request_t recv_request;				/**< Object to map relative coordinates to one-dimensional coordinates in the buffer array */
  
  /**
   * Performs a non-blocking receive operation from a neighbouring process
   * 
   * @param Y relative coordinate
   */  
  void Irecv(int Y) {
    MPI_Irecv(
      static_cast<char *>(m_recv_buffers.buffer(Y)),		// pointer to buffer
      m_recv_buffers.size(Y),					// buffer size (in byte)
      m_recv_buffers.datatype(),				// MPI datatype
      m_proc_grid.proc(Y),					// coordinates
      getTag(-Y),						// Message tag
      m_proc_grid.communicator(),				// MPI communicator
      &recv_request(-Y));
  }
  
  /**
   * Performs a non-blocking receive operation from a neighbouring process
   * 
   * @param X relative coordinate for the first dimension
   * @param Y relative coordinate for the second dimension
   */  
  void Irecv(int X, int Y) {
    MPI_Irecv(
      static_cast<char *>(m_recv_buffers.buffer(X, Y)),		// pointer to buffer
      m_recv_buffers.size(X, Y),				// buffer size (in byte)
      m_recv_buffers.datatype(),				// MPI datatype
      m_proc_grid.proc(X, Y),					// coordinates
      getTag(-X, -Y),						// Message tag
      m_proc_grid.communicator(),				// MPI communicator
      &recv_request(-X, -Y));
  }
  
  /**
   * Performs a non-blocking receive operation from a neighbouring process
   * 
   * @param X relative coordinate for the first dimension
   * @param Y relative coordinate for the second dimension
   * @param Z relative coordinate for the third dimension
   */  
  void Irecv(int X, int Y, int Z) {
    MPI_Irecv(
      static_cast<char *>(m_recv_buffers.buffer(X, Y, Z)),	// pointer to buffer
      m_recv_buffers.size(X, Y, Z),				// buffer size (in byte)
      m_recv_buffers.datatype(),				// MPI datatype
      m_proc_grid.proc(X, Y, Z),				// coordinates
      getTag(-X, -Y, -Z),					// Message tag
      m_proc_grid.communicator(),				// MPI communicator
      &recv_request(-X, -Y, -Z));
  }
  
  /**
   * Performs a non-blocking send operation to a neighbouring process
   * 
   * @param Y relative coordinate
   */  
  void Isend(int Y) {
    MPI_Isend(
      static_cast<char *>(m_send_buffers.buffer(Y)),		// pointer to buffer
      m_send_buffers.size(Y),					// buffer size (in byte)
      m_send_buffers.datatype(),				// MPI datatype
      m_proc_grid.proc(Y),					// coordinates
      getTag(Y),						// Message tag
      m_proc_grid.communicator(),				// MPI communicator
      &send_request(Y));
  }

  /**
   * Performs a non-blocking send operation to a neighbouring process
   * 
   * @param X relative coordinate for the first dimension
   * @param Y relative coordinate for the second dimension
   */  
  void Isend(int X, int Y) {
    MPI_Isend(
      static_cast<char *>(m_send_buffers.buffer(X, Y)),		// pointer to buffer
      m_send_buffers.size(X, Y),				// buffer size (in byte)
      m_send_buffers.datatype(),				// MPI datatype
      m_proc_grid.proc(X, Y),					// coordinates
      getTag(X, Y),						// Message tag
      m_proc_grid.communicator(),				// MPI communicator
      &send_request(X, Y));
  }
  
  /**
   * Performs a non-blocking send operation to a neighbouring process
   * 
   * @param X relative coordinate for the first dimension
   * @param Y relative coordinate for the second dimension
   * @param Z relative coordinate for the third dimension
   */  
  void Isend(int X, int Y, int Z) {
    MPI_Isend(
      static_cast<char *>(m_send_buffers.buffer(X, Y, Z)),	// pointer to buffer
      m_send_buffers.size(X, Y, Z),				// buffer size (in byte)
      m_send_buffers.datatype(),				// MPI datatype
      m_proc_grid.proc(X, Y, Z),				// coordinates
      getTag(X, Y, Z),						// Message tag
      m_proc_grid.communicator(),				// MPI communicator
      &send_request(X, Y, Z));
  }
  
  /**
   * Performs a simple MPI-Wait and synchronizes with a neighbouring process
   * 
   * @param Y relative coordinate
   */
  void wait(int Y) {
    MPI_Status status;
    MPI_Wait(&recv_request(-Y), &status);
  }

  /**
   * Performs a simple MPI-Wait and synchronizes with a neighbouring process
   * 
   * @param X relative coordinate for the first dimension
   * @param Y relative coordinate for the second dimension
   */
  void wait(int X, int Y) {
    MPI_Status status;
    MPI_Wait(&recv_request(-X, -Y), &status);
  }
  
  /**
   * Performs a simple MPI-Wait and synchronizes with a neighbouring process
   * 
   * @param X relative coordinate for the first dimension
   * @param Y relative coordinate for the second dimension
   * @param Z relative coordinate for the third dimension
   */
  void wait(int X, int Y, int Z) {
    MPI_Status status;
    MPI_Wait(&recv_request(-X, -Y, -Z), &status);
  }
  
public:

  /**
   * Constructor that initializes all fields and stores the topology of the processes
   * 
   * @param _communicator MPI_Comm object representing the grid's topology
   */
  
  TransmitBuffer(MPI_Comm& _communicator) : m_proc_grid(_communicator), 
                                            m_send_buffers(), 
                                            m_recv_buffers() {}
  
  /**
   * Default constructor 
   */
  TransmitBuffer() { }
  
  /**
   * Initializes the send buffer for data exchange with a neighbouring process in a one-dimensional grid
   * 
   * @param *p pointer to the data 
   * @param &DT MPI_Datatype 
   * @param s size of the buffer
   * @param Y coordinate in the process gird
   */
  void init_send_buffer( void *p, MPI_Datatype const &DT, int s, int Y ) {
    m_send_buffers.buffer(Y) = reinterpret_cast<char *>(p);
    m_send_buffers.datatype() = DT;
    m_send_buffers.size(Y) = s;
  }
  
  /**
   * Initializes the send buffer for data exchange with a neighbouring process in a two-dimensional grid
   * 
   * @param *p pointer to the data 
   * @param &DT MPI_Datatype 
   * @param s size of the buffer
   * @param X coordinate in the process gird
   * @param Y coordinate in the process gird
   */
  void init_send_buffer( void *p, MPI_Datatype const &DT, int s, int X, int Y ) {
    m_send_buffers.buffer(X, Y) = reinterpret_cast<char *>(p);
    m_send_buffers.datatype() = DT;
    m_send_buffers.size(X, Y) = s;
  }
  
  /**
   * Initializes the send buffer for data exchange with a neighbouring process in a three-dimensional grid
   * 
   * @param *p pointer to the data 
   * @param &DT MPI_Datatype 
   * @param s size of the buffer
   * @param X coordinate in the process gird
   * @param Y coordinate in the process gird
   * @param Z coordinate in the process gird
   */
  void init_send_buffer( void *p, MPI_Datatype const &DT, int s, int X, int Y, int Z ) {
    m_send_buffers.buffer(X, Y, Z) = reinterpret_cast<char *>(p);
    m_send_buffers.datatype() = DT;
    m_send_buffers.size(X, Y, Z) = s;
  }
  
  /**
   * Initializes the receive buffer for data exchange with a neighbouring process in a one-dimensional grid
   * 
   * @param *p pointer to the data 
   * @param &DT MPI_Datatype 
   * @param s size of the buffer
   * @param Y coordinate in the process gird
   */
  void init_recv_buffer( void *p, MPI_Datatype const &DT, int s, int Y ) {
    m_recv_buffers.buffer(Y) = reinterpret_cast<char *>(p);
    m_recv_buffers.datatype() = DT;
    m_recv_buffers.size(Y) = s;
  }
  
  /**
   * Initializes the receive buffer for data exchange with a neighbouring process in a two-dimensional grid
   * 
   * @param *p pointer to the data 
   * @param &DT MPI_Datatype 
   * @param s size of the buffer
   * @param X coordinate in the process gird
   * @param Y coordinate in the process gird
   */
  void init_recv_buffer( void *p, MPI_Datatype const &DT, int s, int X, int Y ) {
    m_recv_buffers.buffer(X, Y) = reinterpret_cast<char *>(p);
    m_recv_buffers.datatype() = DT;
    m_recv_buffers.size(X, Y) = s;
  }
  
  /**
   * Initializes the receive buffer for data exchange with a neighbouring process in a three-dimensional grid
   * 
   * @param *p pointer to the data 
   * @param &DT MPI_Datatype 
   * @param s size of the buffer
   * @param X coordinate in the process gird
   * @param Y coordinate in the process gird
   * @param Z coordinate in the process gird
   */
  void init_recv_buffer( void *p, MPI_Datatype const &DT, int s, int X, int Y, int Z ) {
    m_recv_buffers.buffer(X, Y, Z) = reinterpret_cast<char *>(p);
    m_recv_buffers.datatype() = DT;
    m_recv_buffers.size(X, Y, Z) = s;
  }
  
  /**
   * Performs the non-blocking data receive operation for all neighbouring proesses in the grid
   */
  void do_receives() {
  /* Posting receives */
  for (auto i=-1; i<=1; i++) {
      if (DIMS == 1) {
        if (i!=0 && m_proc_grid.proc(i)!=-1 && (m_recv_buffers.getsize(i)!=0))
          Irecv(i);
      }
      else {
        for (auto j=-1; j<=1; j++) {
          if (DIMS == 2) {
            if ((i!=0 || j!=0) && m_proc_grid.proc(i, j)!=-1 && (m_recv_buffers.getsize(i, j)!=0))
              Irecv(i, j);
          }
          else {
            if (DIMS == 3) {
              for (auto k=-1; k<=1; k++)
                if ((i!=0 || j!=0 || k!=0) && m_proc_grid.proc(i, j, k)!=-1 && (m_recv_buffers.getsize(i, j, k)!=0))
                  Irecv(i, j, k);
            }
          }
        }
      }
    }
  }
  
  /**
   * Performs the non-blocking data send operation for all neighbouring proesses in the grid
   */
  void do_sends(void) {
    /* Sending data */
    for (auto i=-1; i<=1; i++) {
      if (DIMS == 1) {
        if (i!=0 && m_proc_grid.proc(i)!=-1 && (m_send_buffers.getsize(i)!=0))
          Isend(i);
      }
      else {
        for (auto j=-1; j<=1; j++) {
          if (DIMS == 2) {
            if ((i!=0 || j!=0) && m_proc_grid.proc(i, j)!=-1 && (m_send_buffers.getsize(i, j)!=0))
              Isend(i, j);
          }
          else {
            if (DIMS == 3) {
              for (auto k=-1; k<=1; k++)
                if ((i!=0 || j!=0 || k!=0) && m_proc_grid.proc(i, j, k)!=-1 && (m_send_buffers.getsize(i, j, k)!=0))
                  Isend(i, j, k);
            }
          }
        }
      }
    }
  }
  
  /**
   * Performs the synchronization operation for all neighbouring proesses in the grid
   */
  void do_waits() {
    /* Actual receives */
    for (auto i=-1; i<=1; i++) {
      if (DIMS == 1) {
        if (i!=0 && m_proc_grid.proc(i)!=-1 && (m_send_buffers.getsize(i)!=0))
          wait(i);
      }
      else {
        for (auto j=-1; j<=1; j++) {
          if (DIMS == 2) {
            if ((i!=0 || j!=0) && m_proc_grid.proc(i, j)!=-1 && (m_send_buffers.getsize(i, j)!=0))
              wait(i, j);
          }
          else {
            if (DIMS == 3) {
              for (auto k=-1; k<=1; k++)
                if ((i!=0 || j!=0 || k!=0) && m_proc_grid.proc(i, j, k)!=-1 && (m_send_buffers.getsize(i, j, k)!=0))
                  wait(i, j, k);
            }
          }
        }
      }
    }
  }
  
  /**
   * Saves the size of the one-dimensional process-grid in a field
   * @param &t_R Integer field, where to store the information
   */
  void getdims(int &t_R) const {
    m_proc_grid.dims(t_R);
  }
  
  /**
   * Saves the size of the two-dimensional process-grid in a field
   * @param &t_R Integer field, where to store the information for the first dimension
   * @param &t_C Integer field, where to store the information for the second dimension
   */
  void getdims(int &t_R, int &t_C) const {
    m_proc_grid.dims(t_R, t_C);
  }

  /**
   * Saves the size of the three-dimensional process-grid in a field
   * @param &t_R Integer field, where to store the information for the first dimension
   * @param &t_C Integer field, where to store the information for the second dimension
   * @param &t_Z Integer field, where to store the information for the third dimension
   */
  void getdims(int &t_R, int &t_C, int &t_Z) const {
    m_proc_grid.dims(t_R, t_C, t_Z);
  }
  
  /**
   * Saves the coordinat of one process in the one-dimensional process-grid in a field
   * @param &t_R Integer field, where to store the information
   */
  void getcoords(int &t_R) const {
    m_proc_grid.coords(t_R);
  }

  /**
   * Saves the coordinat of one process in the two-dimensional process-grid in a field
   * @param &t_R Integer field, where to store the information for the first dimension
   * @param &t_C Integer field, where to store the information for the second dimension
   */
  void getcoords(int &t_R, int &t_C) const {
    m_proc_grid.coords(t_R, t_C);
  }

  /**
   * Saves the coordinat of one process in the three-dimensional process-grid in a field
   * @param &t_R Integer field, where to store the information for the first dimension
   * @param &t_C Integer field, where to store the information for the second dimension
   * @param &t_Z Integer field, where to store the information for the third dimension
   */
  void getcoords(int &t_R, int &t_C, int &t_Z) const {
    m_proc_grid.coords(t_R, t_C, t_Z);
  }
  
  /**
   * Returns the process of id of the process with given relative coordiantes in the one-dimensional grid
   * 
   * @param Y relative coordinate
   * @return process id
   */
  int getprocid(int Y) const {
    return m_proc_grid.proc(Y);
  }

  /**
   * Returns the process of id of the process with given relative coordiantes in the two-dimensional grid
   * 
   * @param X relative coordinate
   * @param Y relative coordinate
   * @return process id
   */
  int getprocid(int X, int Y) const {
    return m_proc_grid.proc(X, Y);
  }

  /**
   * Returns the process of id of the process with given relative coordiantes in the two-dimensional grid
   * 
   * @param X relative coordinate
   * @param Y relative coordinate
   * @param Z relatvie coordinate
   * @return process id
   */
  int getprocid(int X, int Y, int Z) const {
    return m_proc_grid.proc(X, Y, Z);
  }
  
};

#endif
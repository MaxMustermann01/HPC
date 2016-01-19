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

template <int DIMS>
class TransmitBuffer {
  
  struct request_t {
    MPI_Request request[(int)pow(3, DIMS)];
    MPI_Request& operator()(int y) { 
      if (y < -1 || y > 1) {
        throw std::out_of_range("Wrong indizes: must be -1 <= i <= 1!");
	exit(1);
      }
      return request[ y+1 ]; 
    }
    MPI_Request& operator()(int x, int y) { 
      if (x < -1 || y < -1 || x > 1 || y > 1) {
        throw std::out_of_range("Wrong indizes: must be -1 <= i <= 1!");
	exit(1);
      }
      return request[ (x+1)*3 + (y+1) ];
    }
    MPI_Request& operator()(int x, int y, int z) { 
      if (x < -1 || y < -1 || z < -1 || x > 1 || y > 1 || z > 1) {
        throw std::out_of_range("Wrong indizes: must be -1 <= i <= 1!");
	exit(1);
      }
      return request[ (x+1)*9 + (y+1)*3 + z+1 ];
    }
  };
  
  int getTag(int Y) {
    if (Y < -1 || Y > 1) {
      throw std::out_of_range("Wrong indizes: must be -1 <= i <= 1!");
    }
    return Y+1;
  }
  
  int getTag(int X, int Y) {
    if (X < -1 || Y < -1 || X > 1 || Y > 1) {
      throw std::out_of_range("Wrong indizes: must be -1 <= i <= 1!");
    }
    return (X + 1)*3 + Y+1;
  }
  
  int getTag(int X, int Y, int Z) {
    if (X < -1 || Y < -1 || Z < -1 || X > 1 || Y > 1 || Z > 1) {
      throw std::out_of_range("Wrong indizes: must be -1 <= i <= 1!");
    }
    return (X + 1)*9 + (Y+1)*3 + Z+1;
  }

  
private:
  
  const Proc_Grid<DIMS> m_proc_grid;
  Buffers<(int)pow(3, DIMS)> m_send_buffers;
  Buffers<(int)pow(3, DIMS)> m_recv_buffers;
  request_t send_request;
  request_t recv_request;
  
  /* method to perform a non-blocking receive */  
  void Irecv(int Y) {
    MPI_Irecv(
      static_cast<char *>(m_recv_buffers.buffer(Y)),	// pointer to buffer
      m_recv_buffers.size(Y),				// buffer size (in byte)
      m_recv_buffers.datatype(),				// MPI datatype
      m_proc_grid.proc(Y),				// coordinates
      getTag(-Y),						// Message tag
      m_proc_grid.communicator(),				// MPI communicator
      &recv_request(-Y));
  }

  void Irecv(int X, int Y) {
    MPI_Irecv(
      static_cast<char *>(m_recv_buffers.buffer(X, Y)),	// pointer to buffer
      m_recv_buffers.size(X, Y),				// buffer size (in byte)
      m_recv_buffers.datatype(),				// MPI datatype
      m_proc_grid.proc(X, Y),				// coordinates
      getTag(-X, -Y),					// Message tag
      m_proc_grid.communicator(),				// MPI communicator
      &recv_request(-X, -Y));
  }

  void Irecv(int X, int Y, int Z) {
    MPI_Irecv(
      static_cast<char *>(m_recv_buffers.buffer(X, Y, Z)),// pointer to buffer
      m_recv_buffers.size(X, Y, Z),			// buffer size (in byte)
      m_recv_buffers.datatype(),				// MPI datatype
      m_proc_grid.proc(X, Y, Z),				// coordinates
      getTag(-X, -Y, -Z),					// Message tag
      m_proc_grid.communicator(),				// MPI communicator
      &recv_request(-X, -Y, -Z));
  }
  
  /* method to perform a non-blocking send */
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
  
  /* Perform a simple MPI-Wait */
  void wait(int Y) {
    MPI_Status status;
    MPI_Wait(&recv_request(-Y), &status);
  }

  void wait(int X, int Y) {
    MPI_Status status;
    MPI_Wait(&recv_request(-X, -Y), &status);
  }

  void wait(int X, int Y, int Z) {
    MPI_Status status;
    MPI_Wait(&recv_request(-X, -Y, -Z), &status);
  }
  
public:

  TransmitBuffer(MPI_Comm& _communicator) : m_proc_grid(_communicator), 
                                            m_send_buffers(), 
                                            m_recv_buffers() {}
  
  /* Default constructor */
  TransmitBuffer() { }
  
  void init_send_buffer( void *p, MPI_Datatype const &DT, int s, int Y ) {
    m_send_buffers.buffer(Y) = reinterpret_cast<char *>(p);
    m_send_buffers.datatype() = DT;
    m_send_buffers.size(Y) = s;
  }

  void init_send_buffer( void *p, MPI_Datatype const &DT, int s, int X, int Y ) {
    m_send_buffers.buffer(X, Y) = reinterpret_cast<char *>(p);
    m_send_buffers.datatype() = DT;
    m_send_buffers.size(X, Y) = s;
  }

  void init_send_buffer( void *p, MPI_Datatype const &DT, int s, int X, int Y, int Z ) {
    m_send_buffers.buffer(X, Y, Z) = reinterpret_cast<char *>(p);
    m_send_buffers.datatype() = DT;
    m_send_buffers.size(X, Y, Z) = s;
  }
  
  void init_recv_buffer( void *p, MPI_Datatype const &DT, int s, int Y ) {
    m_recv_buffers.buffer(Y) = reinterpret_cast<char *>(p);
    m_recv_buffers.datatype() = DT;
    m_recv_buffers.size(Y) = s;
  }

  void init_recv_buffer( void *p, MPI_Datatype const &DT, int s, int X, int Y ) {
    m_recv_buffers.buffer(X, Y) = reinterpret_cast<char *>(p);
    m_recv_buffers.datatype() = DT;
    m_recv_buffers.size(X, Y) = s;
  }

  void init_recv_buffer( void *p, MPI_Datatype const &DT, int s, int X, int Y, int Z ) {
    m_recv_buffers.buffer(X, Y, Z) = reinterpret_cast<char *>(p);
    m_recv_buffers.datatype() = DT;
    m_recv_buffers.size(X, Y, Z) = s;
  }
  
  void do_receives() {
  /* Posting receives */
    for (auto i=-1; i<=1; i++)
      if (i!=0 && m_proc_grid.proc(i)!=-1 && DIMS == 1)
        Irecv(i);
      else
        for (auto j=-1; j<=1; j++)
          if ((i!=0 || j!=0) && m_proc_grid.proc(i, j)!=-1 && DIMS == 2)
            Irecv(i, j);
          else
            for (auto k=-1; k<=1; k++)
              if ((i!=0 || j!=0 || k!=0) && m_proc_grid.proc(i, j, k)!=-1 && DIMS == 3)
                Irecv(i, j, k);
}
  
  void do_sends(void) {
    /* Sending data */
    for (auto i=-1; i<=1; i++)
      if (i!=0 && m_proc_grid.proc(i)!=-1 && DIMS == 1)
        Isend(i);
      else
        for (auto j=-1; j<=1; j++)
          if ((i!=0 || j!=0) && m_proc_grid.proc(i, j)!=-1 && DIMS == 2)
            Isend(i, j);
          else
            for (auto k=-1; k<=1; k++)
              if ((i!=0 || j!=0 || k!=0) && m_proc_grid.proc(i, j, k)!=-1 && DIMS == 3)
                Isend(i, j, k);
}
  
  void do_waits() {
    /* Actual receives */
    for (auto i=-1; i<=1; i++)
      if (i!=0 && m_proc_grid.proc(i)!=-1 && DIMS == 1)
        wait(i);
      else
        for (auto j=-1; j<=1; j++)
          if ((i!=0 || j!=0) && m_proc_grid.proc(i, j)!=-1 && DIMS == 2)
            wait(i, j);
          else
            for (auto k=-1; k<=1; k++)
              if ((i!=0 || j!=0 || k!=0) && m_proc_grid.proc(i, j, k)!=-1 && DIMS == 3)
                wait(i, j, k);
}
  
  void getdims(int &t_R) const {
    m_proc_grid.dims(t_R);
  }

  void getdims(int &t_R, int &t_C) const {
    m_proc_grid.dims(t_R, t_C);
  }

  void getdims(int &t_R, int &t_C, int &t_Z) const {
    m_proc_grid.dims(t_R, t_C, t_Z);
  }
  
  void getcoords(int &t_R) const {
    m_proc_grid.coords(t_R);
  }

  void getcoords(int &t_R, int &t_C) const {
    m_proc_grid.coords(t_R, t_C);
  }

  void getcoords(int &t_R, int &t_C, int &t_Z) const {
    m_proc_grid.coords(t_R, t_C, t_Z);
  }
  
  int getprocid(int Y) const {
    return m_proc_grid.proc(Y);
  }

  int getprocid(int X, int Y) const {
    return m_proc_grid.proc(X, Y);
  }

  int getprocid(int X, int Y, int Z) const {
    return m_proc_grid.proc(X, Y, Z);
  }
  
};

#endif
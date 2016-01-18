#ifndef _TRANSMITBUFFER_H_
#define _TRANSMITBUFFER_H_

#include <mpi.h>
#include <cmath>
#include "Proc_Grid.h"
#include "Buffers.h"

  /*       (y)
   *       ------------------------- 
   *       |                       | 
   *       |          -1           | 
   *       |                       | 
   *       ------------------------- 
   *       |                       | 
   *       |           0           | 
   *       |                       | 
   *       ------------------------- 
   *       |                       | 
   *       |           1           | 
   *       |                       | 
   *       ------------------------- 
   */
template <int DIMS>
class TransmitBuffer {
  
  struct request_t {
    MPI_Request request[(int)pow(3, DIMS)];
    MPI_Request& operator()(int y) { return request[ y+1 ]; }
  };
  
  int getTag(int Y) {
    return Y+1;
  }

private:
  
  //int m_buff_size;
  //int m_dims;
  const Proc_Grid_t<DIMS> m_proc_grid;
  Buffers<(int)pow(3, DIMS)> m_send_buffers;
  Buffers<(int)pow(3, DIMS)> m_recv_buffers;
  request_t send_request;
  request_t recv_request;
  
  /* method to perform a non-blocking receive */
  void Irecv(int Y) {
    MPI_Irecv(
      static_cast<char *>(m_recv_buffers.buffer(Y)),	// pointer to buffer
      m_recv_buffers.size(Y),				// buffer size (in byte)
      m_recv_buffers.datatype(),			// MPI datatype
      m_proc_grid.proc(Y),				// coordinates
      getTag(-Y),
      m_proc_grid.communicator(),			// MPI communicator
      &recv_request(-Y));
  }
  
  /* method to perform a non-blocking send */
  void Isend(int Y) {
    MPI_Isend(
      static_cast<char *>(m_send_buffers.buffer(Y)),	// pointer to buffer
      m_send_buffers.size(Y),				// buffer size (in byte)
      m_send_buffers.datatype(),			// MPI datatype
      m_proc_grid.proc(Y),				// coordinates
      getTag(Y),
      m_proc_grid.communicator(),			// MPI communicator
      &send_request(Y));
  }
  
  /* Perform a simple MPI-Wait */
  void wait(int Y) {
    MPI_Status status;
    MPI_Wait(&recv_request(-Y), &status);
  }
  
public:

  TransmitBuffer(MPI_Comm& _communicator) : m_proc_grid(_communicator), 
                                            m_send_buffers(), 
                                            m_recv_buffers()
  {}
  
  /* Default constructor */
  TransmitBuffer() { }
  
  void init_send_buffer( void *p, MPI_Datatype const &DT, int s, int Y ) {
    m_send_buffers.buffer(Y) = reinterpret_cast<char *>(p);
    m_send_buffers.datatype() = DT;
    m_send_buffers.size(Y) = s;
  }
  
  void init_recv_buffer( void *p, MPI_Datatype const &DT, int s, int Y) {
    m_recv_buffers.buffer(Y) = reinterpret_cast<char *>(p);
    m_recv_buffers.datatype() = DT;
    m_recv_buffers.size(Y) = s;
  }
  
  void do_receives() {
    /* Posting receives */
    for(auto i=-1; i<1; i++)
      if (i!=0 && m_proc_grid.proc(i)!=-1)
        Irecv(1);
  }
  
  void do_sends(void) {
    /* Sending data */
    for(auto i=-1; i<1; i++)
      if (i!=0 && m_proc_grid.proc(i)!=-1)
        Isend(i);
  }
  
  void do_waits() {
    /* Actual receives */
    for(auto i=-1; i<1; i++)
      if (i!=0 && m_proc_grid.proc(i)!=-1)
        wait(i);
  }
  
  void getdims(int &t_R) const {
    m_proc_grid.dims(t_R);
  }
  
  void getcoords(int &t_R) const {
    m_proc_grid.coords(t_R);
  }
  
  int getprocid(int Y) const {
    return m_proc_grid.proc(Y);
  }
  
};

#endif
#ifndef _TRANSMITBUFFER_1D_H_
#define _TRANSMITBUFFER_1D_H_

#include <mpi.h>
#include "Proc_Grid_1D.h"

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
  
class TransmitBuffer_1D {
  
  class Buffers {
    char *m_buffers[3];		// pointer to buffers
    int m_size[3];		// size of buffers (in bytes)
    MPI_Datatype m_datatype;	// datatype
    
  public:
    explicit Buffers() {
      /* set all array values to zero */
      std::fill(&m_buffers[0], &m_buffers[2], nullptr);
      std::fill(&m_size[0], &m_size[2], 0);
    }
    /* returns reference of a pointer to a char (or the specific buffer) */
    char*& buffer(int y) { return m_buffers[ y+1 ]; }
    /* returns reference to used MPI datatype */
    MPI_Datatype& datatype(void) { return m_datatype; }
    /* returns reference to specific size of the buffer */
    int& size(int y) { return m_size[ y+1 ]; }
    
  };
  
  struct request_t {
    MPI_Request request[3];
    MPI_Request& operator()(int y) { return request[ y+1 ]; }
  };
  
  /*template <int Y>
  struct TAG {
    static const int value = Y+1;
  };*/
  int getTag(int Y) {
    return Y+1;
  }

private:
  
  const Proc_Grid_1D_t* m_proc_grid;
  Buffers m_send_buffers;
  Buffers m_recv_buffers;
  request_t send_request;
  request_t recv_request;
  
  /* method to perform a non-blocking receive */
  //template <int Y>
  void Irecv(int Y) {
    MPI_Irecv(
      static_cast<char *>(m_recv_buffers.buffer(Y)),	// pointer to buffer
      m_recv_buffers.size(Y),				// buffer size (in byte)
      m_recv_buffers.datatype(),			// MPI datatype
      m_proc_grid->proc(Y),				// coordinates
      //TAG<-Y>::value,					// message tag
      getTag(Y),
      m_proc_grid->communicator(),			// MPI communicator
      &recv_request(-Y));
  }
  
  /* method to perform a non-blocking send */
  //template <int Y>
  void Isend(int Y) {
    MPI_Isend(
      static_cast<char *>(m_send_buffers.buffer(Y)),	// pointer to buffer
      m_send_buffers.size(Y),				// buffer size (in byte)
      m_send_buffers.datatype(),			// MPI datatype
      m_proc_grid->proc(Y),				// coordinates
      //TAG<Y>::value,					// message tag
      getTag(Y),
      m_proc_grid->communicator(),			// MPI communicator
      &send_request(Y));
  }
  
  /* Perform a simple MPI-Wait */
  //template <int Y>
  void wait(int Y) {
    MPI_Status status;
    MPI_Wait(&recv_request(-Y), &status);
  }
  
public:

  TransmitBuffer_1D(Proc_Grid_1D_t* _pg) : m_proc_grid(_pg), m_send_buffers(), m_recv_buffers()
  {}
  
  /* Default constructor */
  TransmitBuffer_1D() { }
  
  /*template <int Y>
  void init_send_buffer( void *p, MPI_Datatype const &DT, int s ){
    init_send_buffer(p, DT, s, Y);
  }*/
  
  void init_send_buffer( void *p, MPI_Datatype const &DT, int s, int Y ) {
    m_send_buffers.buffer(Y) = reinterpret_cast<char *>(p);
    m_send_buffers.datatype() = DT;
    m_send_buffers.size(Y) = s;
  }
  
  /*template <int Y>
  void init_recv_buffer( void *p, MPI_Datatype const &DT, int s) {
    init_recv_buffer(p, DT, s, Y);
  }*/
  
  void init_recv_buffer( void *p, MPI_Datatype const &DT, int s, int Y) {
    m_recv_buffers.buffer(Y) = reinterpret_cast<char *>(p);
    m_recv_buffers.datatype() = DT;
    m_recv_buffers.size(Y) = s;
  }
  
  void do_receives() {
    /* Posting receives */
    if (m_proc_grid->proc(1)!=-1) {
      Irecv(1);
    }

    if (m_proc_grid->proc(-1)!=-1) {
      Irecv(-1);
    }
  }
  
  void do_sends(void) {
    /* Sending data */
    if (m_proc_grid->proc(-1)!=-1) {
      Isend(-1);
    }

    if (m_proc_grid->proc(1)!=-1) {
      Isend(1);
    }
  }
  
  void do_waits() {
    /* Actual receives */
    if (m_proc_grid->proc(1)!=-1) {
      wait(1);
    }

    if (m_proc_grid->proc(-1)!=-1) {
      wait(-1);
    }
  }
  
};

#endif
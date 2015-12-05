
#include <mpi.h>
#include "2D_proc_grid.h"

class Halo_2D {
  /*       ( x, y)
   *       ------------------------- 
   *       |       |       |       | 
   *       | -1,-1 | -1,0  | -1,1  | 
   *       |       |       |       | 
   *       ------------------------- 
   *       |       |       |       | 
   *       |  0,-1 |  0,0  |  0,1  | 
   *       |       |       |       | 
   *       ------------------------- 
   *       |       |       |       | 
   *       |  1,-1 |  1,0  |  1,1  | 
   *       |       |       |       | 
   *       ------------------------- 
   */
  
  class Buffers {
    char *m_buffers[9];		// pointer to buffers
    int m_size[9];		// size of buffers (in bytes)
    MPI_Datatype m_datatype;	// datatype
    
  public:
    explicit Buffers() {
      /* c++11 feature to set all array values */
      //std::fill(&m_buffers[0], &m_buffers[8], NULL);
      std::fill(&m_size[0], &m_size[8], 0);
    }
    /* returns reference of a pointer to a char (or the specific buffer) */
    char*& buffer(int x, int y) { return m_buffers[ (x+1)*3 + y+1 ]; }
    /* returns reference to used MPI datatype */
    MPI_Datatype& datatype(void) { return m_datatype; }
    /* returns reference to specific size of the buffer */
    int& size(int x, int y) { return m_size[ (x+1)*3 + y+1 ]; }
    
  };
  
  struct request_t {
    MPI_Request request[9];
    MPI_Request& operator()(int x, int y) { return request[ (x+1)*3 + y+1 ]; }
  };
  
  template <int X, int Y>
  struct TAG {
    static const int value = (X + 1)*3 + Y+1;
  };

private:
  
  const MPI_2D_proc_grid_t m_proc_grid;
  Buffers m_send_buffers;
  Buffers m_recv_buffers;
  request_t send_request;
  request_t recv_request;
  
  /* method to perform a non-blocking receive */
  template <int X, int Y>
  void Irecv(void) {
    MPI_Irecv(
      static_cast<char *>(m_recv_buffers.buffer(X, Y)),	// pointer to buffer
      m_recv_buffers.size(X, Y),			// buffer size (in byte)
      m_recv_buffers.datatype(/*X, Y*/),		// MPI datatype
      m_proc_grid.template proc<X, Y>(),		// coordinates
      TAG<-X, -Y>::value,				// message tag
      /*m_proc_grid.communicator*/ MPI_COMM_WORLD,				// MPI communicator
      &recv_request(-X, -Y));
  }
  
  /* method to perform a non-blocking send */
  template <int X, int Y>
  void Isend(void) {
    MPI_Isend(
      static_cast<char *>(m_recv_buffers.buffer(X, Y)),	// pointer to buffer
      m_send_buffers.size(X, Y),			// buffer size (in byte)
      m_send_buffers.datatype(/*X, Y*/),		// MPI datatype
      m_proc_grid.template proc<X, Y>(),		// coordinates
      TAG<-X, -Y>::value,				// message tag
      /*m_proc_grid.communicator*/ MPI_COMM_WORLD,				// MPI communicator
      &send_request(-X, -Y));
  }
  
  /* Perform a simple MPI-Wait */
  template <int X, int Y>
  void wait(void) {
    MPI_Status status;
    MPI_Wait(&recv_request(-X, -Y), &status);
  }
  
public:

  Halo_2D(MPI_2D_proc_grid_t _pg) : m_proc_grid(_pg), m_send_buffers(), m_recv_buffers()
  {}
  
  template <int X, int Y>
  void init_send_buffer( void *p, MPI_Datatype const &DT, int s ){
    init_send_buffer(p, DT, s, X, Y);
  }
  
  void init_send_buffer( void *p, MPI_Datatype const &DT, int s, int X, int Y ) {
    m_send_buffers.buffer(X, Y) = reinterpret_cast<char *>(p);
    m_send_buffers.datatype(/*X, Y*/) = DT;
    m_send_buffers.size(X, Y) = s;
  }
  
  template <int X, int Y>
  void init_recv_buffer( void *p, MPI_Datatype const &DT, int s) {
    init_recv_buffer(p, DT, s, X, Y);
  }
  
  void init_recv_buffer( void *p, MPI_Datatype const &DT, int s, int X, int Y) {
    m_recv_buffers.buffer(X, Y) = reinterpret_cast<char *>(p);
    m_recv_buffers.datatype(/*X, Y*/) = DT;
    m_recv_buffers.size(X, Y) = s;
  }
  
  void do_receives() {
    /* Posting receives */
    if (m_proc_grid.template proc<1,0>()!=-1) {
      Irecv<1,0>();
    }

    if (m_proc_grid.template proc<-1,0>()!=-1) {
      Irecv<-1,0>();
    }

    if (m_proc_grid.template proc<0,1>()!=-1) {
      Irecv<0,1>();
    }

    if (m_proc_grid.template proc<0,-1>()!=-1) {
      Irecv<0,-1>();
    }
      
    /* Posting receives FOR CORNERS */
    if (m_proc_grid.template proc<1,1>()!=-1) {
      Irecv<1,1>();
    }

    if (m_proc_grid.template proc<-1,-1>()!=-1) {
      Irecv<-1,-1>();
    }

    if (m_proc_grid.template proc<1,-1>()!=-1) {
      Irecv<1,-1>();
    }

    if (m_proc_grid.template proc<-1,1>()!=-1) {
      Irecv<-1,1>();
    }
  }
  
  void do_sends(void) {
    /* Sending data */
    if (m_proc_grid.template proc<-1,0>()!=-1) {
      Isend<-1,0>();
    }

    if (m_proc_grid.template proc<1,0>()!=-1) {
      Isend<1,0>();
    }

    if (m_proc_grid.template proc<0,-1>()!=-1) {
      Isend<0,-1>();
    }

    if (m_proc_grid.template proc<0,1>()!=-1) {
      Isend<0,1>();
    }

    /* Sending data CORNERS */
    if (m_proc_grid.template proc<-1,-1>()!=-1) {
      Isend<-1,-1>();
    }

    if (m_proc_grid.template proc<1,1>()!=-1) {
      Isend<1,1>();
    }

    if (m_proc_grid.template proc<1,-1>()!=-1) {
      Isend<1,-1>();
    }

    if (m_proc_grid.template proc<-1,1>()!=-1) {
      Isend<-1,1>();
    }
  }
  
  void do_waits() {
    /* Actual receives */
    if (m_proc_grid.template proc<1,0>()!=-1) {
      wait<1,0>();
    }

    if (m_proc_grid.template proc<-1,0>()!=-1) {
      wait<-1,0>();
    }

    if (m_proc_grid.template proc<0,1>()!=-1) {
      wait<0,1>();
    }
          
    if (m_proc_grid.template proc<0,-1>()!=-1) {
      wait<0,-1>();
    }

    if (m_proc_grid.template proc<1,1>()!=-1) {
      wait<1,1>();
    }

    if (m_proc_grid.template proc<-1,-1>()!=-1) {
      wait<-1,-1>();
    }

    if (m_proc_grid.template proc<-1,1>()!=-1) {
      wait<-1,1>();
    }

    if (m_proc_grid.template proc<1,-1>()!=-1) {
      wait<1,-1>();
    }
  }
  
};
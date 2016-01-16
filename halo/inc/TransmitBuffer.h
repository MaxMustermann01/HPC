#ifndef _TRANSMITBUFFER_H_
#define _TRANSMITBUFFER_H_

#include <mpi.h>
#include "Buffers.h"
#include "ProcGrid.h"

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

  
template<int DIMS, int BUFFERSIZE>
class TransmitBuffer {

  
  struct request_t {
    MPI_Request request[BUFFERSIZE];
    MPI_Request& operator()(int x, int y) { 
      if (x < -1 || y < -1 || x > 1 || y > 1) {
	std::cout << "Wrong indizes: must be -1 <= i <= 1!" << std::endl;
      }
      return request[ (x+1)*3 + (y+1) ];
    }
    MPI_Request& operator()(int x, int y, int z) { 
      if (x < -1 || y < -1 || z < -1 || x > 1 || y > 1 || z > 1) {
	std::cout << "Wrong indizes: must be -1 <= i <= 1!" << std::endl;
      }
      return request[ (x+1)*9 + (y+1)*3 + z+1 ];
    }
  };
  
  template <int X, int Y>
  struct TAG_2D {
    static const int value = (X + 1)*3 + Y+1;
  };
  
  template <int X, int Y, int Z>
  struct TAG_3D {
    static const int value = (X + 1)*9 + (Y+1)*3 + Z+1;
  };

private:
  
  const Proc_Grid_t<DIMS> m_proc_grid;
  Buffers<BUFFERSIZE> m_send_buffers;
  Buffers<BUFFERSIZE> m_recv_buffers;
  request_t send_request;
  request_t recv_request;

  
  /* 2D templates */
  /* method to perform a non-blocking receive */
  template <int X, int Y>
  void Irecv(void) {
    MPI_Irecv(
      static_cast<char *>(m_recv_buffers.buffer(X, Y)),	// pointer to buffer
      m_recv_buffers.size(X, Y),			// buffer size (in byte)
      m_recv_buffers.datatype(/*X, Y*/),		// MPI datatype
      m_proc_grid.template proc<X, Y>(),		// coordinates
      TAG_2D<-X, -Y>::value,				// message tag
      m_proc_grid.communicator(),			// MPI communicator
      &recv_request(-X, -Y));
  }
  
  /* method to perform a non-blocking send */
  template <int X, int Y>
  void Isend(void) {
    MPI_Isend(
      static_cast<char *>(m_send_buffers.buffer(X, Y)),	// pointer to buffer
      m_send_buffers.size(X, Y),			// buffer size (in byte)
      m_send_buffers.datatype(/*X, Y*/),		// MPI datatype
      m_proc_grid.template proc<X, Y>(),		// coordinates
      TAG_2D<X, Y>::value,					// message tag
      m_proc_grid.communicator(),			// MPI communicator
      &send_request(X, Y));
  }
  
  /* Perform a simple MPI-Wait */
  template <int X, int Y>
  void wait(void) {
    MPI_Status status;
    MPI_Wait(&recv_request(-X, -Y), &status);
  }
  
  /* 3D templates */
  /* method to perform a non-blocking receive */
  template <int X, int Y, int Z>
  void Irecv(void) {
    MPI_Irecv(
      static_cast<char *>(m_recv_buffers.buffer(X, Y, Z)),	// pointer to buffer
      m_recv_buffers.size(X, Y, Z),				// buffer size (in byte)
      m_recv_buffers.datatype(/*X, Y Z*/),			// MPI datatype
      m_proc_grid.template proc<X, Y, Z>(),			// coordinates
      TAG_3D<-X, -Y, -Z>::value,					// message tag
      m_proc_grid.communicator(),				// MPI communicator
      &recv_request(-X, -Y, -Z));
  }
  
  /* method to perform a non-blocking send */
  template <int X, int Y, int Z>
  void Isend(void) {
    MPI_Isend(
      static_cast<char *>(m_send_buffers.buffer(X, Y, Z)),	// pointer to buffer
      m_send_buffers.size(X, Y, Z),				// buffer size (in byte)
      m_send_buffers.datatype(/*X, Y Z*/),			// MPI datatype
      m_proc_grid.template proc<X, Y, Z>(),			// coordinates
      TAG_3D<X, Y, Z>::value,					// message tag
      m_proc_grid.communicator(),				// MPI communicator
      &send_request(X, Y, Z));
  }
  
  /* Perform a simple MPI-Wait */
  template <int X, int Y, int Z>
  void wait(void) {
    MPI_Status status;
    MPI_Wait(&recv_request(-X, -Y, -Z), &status);
  }
       
  void init_send_buffer_2D( void *p, MPI_Datatype const &DT, int s, int X, int Y ) {
    m_send_buffers.buffer(X, Y) = reinterpret_cast<char *>(p);
    m_send_buffers.datatype(/*X, Y*/) = DT;
    m_send_buffers.size(X, Y) = s;
  }
  
  void init_send_buffer_3D( void *p, MPI_Datatype const &DT, int s, int X, int Y, int Z ) {
    m_send_buffers.buffer(X, Y, Z) = reinterpret_cast<char *>(p);
    m_send_buffers.datatype(/*X, Y*/) = DT;
    m_send_buffers.size(X, Y, Z) = s;
  }  
    
  void init_recv_buffer_2D( void *p, MPI_Datatype const &DT, int s, int X, int Y) {
    m_recv_buffers.buffer(X, Y) = reinterpret_cast<char *>(p);
    m_recv_buffers.datatype(/*X, Y*/) = DT;
    m_recv_buffers.size(X, Y) = s;
  }  
  
  void init_recv_buffer_3D( void *p, MPI_Datatype const &DT, int s, int X, int Y, int Z) {
    m_recv_buffers.buffer(X, Y, Z) = reinterpret_cast<char *>(p);
    m_recv_buffers.datatype(/*X, Y*/) = DT;
    m_recv_buffers.size(X, Y, Z) = s;
  }
  
    
void do_receives_2D() {
    /* Posting receives */
    if (m_proc_grid.template proc<1,0>()!=-1) {      Irecv<1,0>();    }
    if (m_proc_grid.template proc<-1,0>()!=-1) {      Irecv<-1,0>();    }
    if (m_proc_grid.template proc<0,1>()!=-1) {      Irecv<0,1>();    }
    if (m_proc_grid.template proc<0,-1>()!=-1) {      Irecv<0,-1>();    }      
    /* Posting receives FOR CORNERS */
    if (m_proc_grid.template proc<1,1>()!=-1) {      Irecv<1,1>();    }
    if (m_proc_grid.template proc<-1,-1>()!=-1) {      Irecv<-1,-1>();    }
    if (m_proc_grid.template proc<1,-1>()!=-1) {      Irecv<1,-1>();    }
    if (m_proc_grid.template proc<-1,1>()!=-1) {      Irecv<-1,1>();    }
    
  }
  
  void do_sends_2D(void) {
    /* Sending data */
    if (m_proc_grid.template proc<-1,0>()!=-1) {      Isend<-1,0>();    }
    if (m_proc_grid.template proc<1,0>()!=-1) {      Isend<1,0>();    }
    if (m_proc_grid.template proc<0,-1>()!=-1) {      Isend<0,-1>();    }
    if (m_proc_grid.template proc<0,1>()!=-1) {      Isend<0,1>();    }
    /* Sending data CORNERS */
    if (m_proc_grid.template proc<-1,-1>()!=-1) {      Isend<-1,-1>();    }
    if (m_proc_grid.template proc<1,1>()!=-1) {      Isend<1,1>();    }
    if (m_proc_grid.template proc<1,-1>()!=-1) {      Isend<1,-1>();    }
    if (m_proc_grid.template proc<-1,1>()!=-1) {      Isend<-1,1>();    }
  }
  
  void do_waits_2D() {
    /* Actual receives */
    if (m_proc_grid.template proc<1,0>()!=-1) {      wait<1,0>();    }
    if (m_proc_grid.template proc<-1,0>()!=-1) {      wait<-1,0>();    }
    if (m_proc_grid.template proc<0,1>()!=-1) {      wait<0,1>();    }          
    if (m_proc_grid.template proc<0,-1>()!=-1) {      wait<0,-1>();    }
    if (m_proc_grid.template proc<1,1>()!=-1) {      wait<1,1>();    }
    if (m_proc_grid.template proc<-1,-1>()!=-1) {      wait<-1,-1>();    }
    if (m_proc_grid.template proc<-1,1>()!=-1) {      wait<-1,1>();    }
    if (m_proc_grid.template proc<1,-1>()!=-1) {      wait<1,-1>();    }
  }

  void do_receives_3D() {
    /* Posting receives */
    if (m_proc_grid.template proc<1,0,0>()!=-1)		Irecv<1,0,0>();		if (m_proc_grid.template proc<-1,0,0>()!=-1)       	Irecv<-1,0,0>();
    if (m_proc_grid.template proc<0,1,0>()!=-1)		Irecv<0,1,0>();		if (m_proc_grid.template proc<0,-1,0>()!=-1)       	Irecv<0,-1,0>();    
    if (m_proc_grid.template proc<0,0,1>()!=-1)		Irecv<0,1,0>();		if (m_proc_grid.template proc<0,0,-1>()!=-1)		Irecv<0,-1,0>();       
    /* Posting receives FOR CORNERS */
    /*in the z=-1 plane: */
    if (m_proc_grid.template proc<-1,-1,-1>()!=-1) 	Irecv<-1,-1,-1>();	if (m_proc_grid.template proc<-1,0,-1>()!=-1)   	Irecv<-1,0,-1>();
    if (m_proc_grid.template proc<-1,1,-1>()!=-1) 	Irecv<-1,1,-1>();	if (m_proc_grid.template proc<0,-1,-1>()!=-1)		Irecv<0,-1,-1>();
    if (m_proc_grid.template proc<0,1,-1>()!=-1) 	Irecv<0,1,-1>();	if (m_proc_grid.template proc<1,-1,-1>()!=-1) 		Irecv<1,-1,-1>();
    if (m_proc_grid.template proc<1,0,-1>()!=-1) 	Irecv<1,0,-1>();	if (m_proc_grid.template proc<1,1,-1>()!=-1) 		Irecv<1,1,-1>();  
    /* in the z=0 plane: */
    if (m_proc_grid.template proc<1,1,0>()!=-1) 	Irecv<1,1,0>();		if (m_proc_grid.template proc<-1,-1,0>()!=-1) 		Irecv<-1,-1,0>();
    if (m_proc_grid.template proc<1,-1,0>()!=-1) 	Irecv<1,-1,0>();	if (m_proc_grid.template proc<-1,1,0>()!=-1) 		Irecv<-1,1,0>();    
    /*in the z=1 plane: */
    if (m_proc_grid.template proc<-1,-1,1>()!=-1) 	Irecv<-1,-1,1>();	if (m_proc_grid.template proc<-1,0,1>()!=-1) 		Irecv<-1,0,1>();    
    if (m_proc_grid.template proc<-1,1,1>()!=-1) 	Irecv<-1, 1, 1>();	if (m_proc_grid.template proc<0,-1,1>()!=-1) 		Irecv<0,-1,1>();    
    if (m_proc_grid.template proc<0,1,1>()!=-1) 	Irecv<0,1,1>();		if (m_proc_grid.template proc<1,-1,1>()!=-1) 		Irecv<1,-1,1>();    
    if (m_proc_grid.template proc<1,0,1>()!=-1) 	Irecv<1,0,1>();		if (m_proc_grid.template proc<1,1,1>()!=-1) 		Irecv<1,1,1>();
    
  }
  
  void do_sends_3D(void) {
    /* Sending data */
    if (m_proc_grid.template proc<1,0,0>()!=-1) {      Isend<1,0,0>();    }   	 if (m_proc_grid.template proc<-1,0,0>()!=-1) {      Isend<-1,0,0>();    }
    if (m_proc_grid.template proc<0,1,0>()!=-1) {      Isend<0,1,0>();    }   	 if (m_proc_grid.template proc<0,-1,0>()!=-1) {      Isend<0,-1,0>();    }    
    if (m_proc_grid.template proc<0,0,1>()!=-1) {      Isend<0,1,0>();    }    	if (m_proc_grid.template proc<0,0,-1>()!=-1) {      Isend<0,-1,0>();    }      
    /* Posting receives FOR CORNERS */
    /*in the z=-1 plane: */
    if (m_proc_grid.template proc<-1,-1,-1>()!=-1) {      Isend<-1,-1,-1>();    }	if (m_proc_grid.template proc<-1,0,-1>()!=-1) {      Isend<-1,0,-1>();    }
    if (m_proc_grid.template proc<-1,1,-1>()!=-1) {      Isend<-1,1,-1>();    }  	if (m_proc_grid.template proc<0,-1,-1>()!=-1) {      Isend<0,-1,-1>();    }
    if (m_proc_grid.template proc<0,1,-1>()!=-1) {      Isend<0,1,-1>();    }    	if (m_proc_grid.template proc<1,-1,-1>()!=-1) {      Isend<1,-1,-1>();    }
    if (m_proc_grid.template proc<1,0,-1>()!=-1) {      Isend<1,0,-1>();    }    	if (m_proc_grid.template proc<1,1,-1>()!=-1) {      Isend<1,1,-1>();    }    
    /* in the z=0 plane: */
    if (m_proc_grid.template proc<1,1,0>()!=-1) {      Isend<1,1,0>();    }    		if (m_proc_grid.template proc<-1,-1,0>()!=-1) {      Isend<-1,-1,0>();    }
    if (m_proc_grid.template proc<1,-1,0>()!=-1) {      Isend<1,-1,0>();    }   	if (m_proc_grid.template proc<-1,1,0>()!=-1) {      Isend<-1,1,0>();    }    
    /*in the z=1 plane: */
    if (m_proc_grid.template proc<-1,-1,1>()!=-1) {      Isend<-1,-1,1>();    }    	if (m_proc_grid.template proc<-1,0,1>()!=-1) {      Isend<-1,0,1>();    }
    if (m_proc_grid.template proc<-1,1,1>()!=-1) {      Isend<-1,1,1>();    }   	if (m_proc_grid.template proc<0,-1,1>()!=-1) {      Isend<0,-1,1>();    }    
    if (m_proc_grid.template proc<0,1,1>()!=-1) {      Isend<0,1,1>();    }    		if (m_proc_grid.template proc<1,-1,1>()!=-1) {      Isend<1,-1,1>();    }
    if (m_proc_grid.template proc<1,0,1>()!=-1) {      Isend<1,0,1>();    }    		if (m_proc_grid.template proc<1,1,1>()!=-1) {      Isend<1,1,1>();    }
  }
  
  void do_waits_3D() {
    /* Actual receives */
    if (m_proc_grid.template proc<1,0,0>()!=-1) {      wait<1,0,0>();    }    if (m_proc_grid.template proc<-1,0,0>()!=-1) {      wait<-1,0,0>();    }
    if (m_proc_grid.template proc<0,1,0>()!=-1) {      wait<0,1,0>();    }    if (m_proc_grid.template proc<0,-1,0>()!=-1) {      wait<0,-1,0>();    }    
    if (m_proc_grid.template proc<0,0,1>()!=-1) {      wait<0,1,0>();    }    if (m_proc_grid.template proc<0,0,-1>()!=-1) {      wait<0,-1,0>();    }      
    /* Posting receives FOR CORNERS */
    /*in the z=-1 plane: */
    if (m_proc_grid.template proc<-1,-1,-1>()!=-1) {      wait<-1,-1,-1>();    }    	if (m_proc_grid.template proc<-1,0,-1>()!=-1) {      wait<-1,0,-1>();    }
    if (m_proc_grid.template proc<-1,1,-1>()!=-1) {      wait<-1,1,-1>();    }    	if (m_proc_grid.template proc<0,-1,-1>()!=-1) {      wait<0,-1,-1>();    }
    if (m_proc_grid.template proc<0,1,-1>()!=-1) {      wait<0,1,-1>();    }    	if (m_proc_grid.template proc<1,-1,-1>()!=-1) {      wait<1,-1,-1>();    }
    if (m_proc_grid.template proc<1,0,-1>()!=-1) {      wait<1,0,-1>();    }    	if (m_proc_grid.template proc<1,1,-1>()!=-1) {      wait<1,1,-1>();    }        
    /* in the z=0 plane: */
    if (m_proc_grid.template proc<1,1,0>()!=-1) {      wait<1,1,0>();    }    	if (m_proc_grid.template proc<-1,-1,0>()!=-1) {      wait<-1,-1,0>();    }
    if (m_proc_grid.template proc<1,-1,0>()!=-1) {      wait<1,-1,0>();    }    if (m_proc_grid.template proc<-1,1,0>()!=-1) {      wait<-1,1,0>();    }    
    /*in the z=1 plane: */
    if (m_proc_grid.template proc<-1,-1,1>()!=-1) {      wait<-1,-1,1>();    }    	if (m_proc_grid.template proc<-1,0,1>()!=-1) {      wait<-1,0,1>();    }
    if (m_proc_grid.template proc<-1,1,1>()!=-1) {      wait<-1,1,1>();    }    	if (m_proc_grid.template proc<0,-1,1>()!=-1) {      wait<0,-1,1>();    }
    if (m_proc_grid.template proc<0,1,1>()!=-1) {      wait<0,1,1>();    }    		if (m_proc_grid.template proc<1,-1,1>()!=-1) {      wait<1,-1,1>();    }
    if (m_proc_grid.template proc<1,0,1>()!=-1) {      wait<1,0,1>();    }    		if (m_proc_grid.template proc<1,1,1>()!=-1) {      wait<1,1,1>();    }
}
  
  
public:
  /* Constructor 2D */
  TransmitBuffer(Proc_Grid_t<2> _pg) : m_proc_grid(_pg), m_send_buffers(), m_recv_buffers()  {}
  /* Constructor 3D */
  TransmitBuffer(Proc_Grid_t<3> _pg) : m_proc_grid(_pg), m_send_buffers(), m_recv_buffers()  {}
  
  template <int X, int Y>
  void init_send_buffer( void *p, MPI_Datatype const &DT, int s ){
    init_send_buffer_2D(p, DT, s, X, Y);
  }
  
  template <int X, int Y, int Z>
  void init_send_buffer( void *p, MPI_Datatype const &DT, int s ){
    init_send_buffer_3D(p, DT, s, X, Y, Z);
  }
 
  
  template <int X, int Y>
  void init_recv_buffer( void *p, MPI_Datatype const &DT, int s) {
    init_recv_buffer_2D(p, DT, s, X, Y);
  }
  
  template <int X, int Y, int Z>
  void init_recv_buffer( void *p, MPI_Datatype const &DT, int s) {
    init_recv_buffer_3D(p, DT, s, X, Y, Z);
  }
  
  void do_sends() {
    switch(DIMS) {
      case 2:
	do_sends_2D();
	break;
      case 3:
	do_sends_3D();
	break;
      default:
	std::cout << "Error: " << DIMS << " dimensions are not supported!" << std::endl;
    }
  }
  
  void do_receives() {
    switch(DIMS) {
      case 2:
	do_receives_2D();
	break;
      case 3:
	do_receives_3D();
	break;
      default:
	std::cout << "Error: " << DIMS << " dimensions are not supported!" << std::endl;
    }
  }  
  
  void do_waits() {
    switch(DIMS) {
      case 2:
	do_waits_2D();
	break;
      case 3:
	do_waits_3D();
	break;
      default:
	std::cout << "Error: " << DIMS << " dimensions are not supported!" << std::endl;
    }
  }
  
       
};


#endif

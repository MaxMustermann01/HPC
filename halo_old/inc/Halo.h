#include <iostream>
#include <stdexcept>
#include <Proc_Grid_2D.h>
#include <TransmitBuffer_2D.h>
#include <mpi.h>
  
template <int GRID_DIM>
struct get_grid;

template <>
struct get_grid<2> {
  using proc_grid_type = Proc_Grid_2D_t;
  using transmit_buffer_type = TransmitBuffer_2D;
};

/template <>
struct get_grid<3> {
  using proc_grid_type = Proc_Grid_3D_t;
  using transmit_buffer_type = TransmitBuffer_3D;
};

/*
 * Main class ...
 */
template <typename DATATYPE,
          int PROC_DIMS,	// Dimensions of process grid
          int TIME_DIM,
          int HALO_WIDTH>
class Grid {
  
private:
  /* Type of the processor grid */
  using proc_grid_type = typename get_grid<PROC_DIMS>::proc_grid_type;
  using transmit_buffer_type = typename get_grid<PROC_DIMS>::transmit_buffer_type;
  
  proc_grid_type* proc_grid;
  transmit_buffer_type* transmit_buffer;
  
  int m_x, m_y, m_z;
  
  DATATYPE* m_data[TIME_DIM];
  
public:
  /* Constructor: needs argv and argc from main for MPI initalization */
  Grid(int _argc, char** _argv, int _x, int _y, int _z) {
    int pid, nprocs;
    MPI_Comm CartComm;
  
    /* Initialize MPI */
    MPI_Init(&_argc, &_argv);
    /* Get process ID and proc. count */
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  
    int dims[PROC_DIMS], period[PROC_DIMS];
    std::fill(dims, dims+PROC_DIMS, 0);
    std::fill(period, period+PROC_DIMS, 0);
    /* Create MPI communicator with respect of chosen dimension */
    MPI_Dims_create(nprocs-1, PROC_DIMS, dims);
    MPI_Cart_create(MPI_COMM_WORLD, PROC_DIMS, dims, period, false, &CartComm);
    /* Create the process grid */
    proc_grid = new proc_grid_type(CartComm);
    /* Create the transmit object */
    transmit_buffer = new transmit_buffer_type(proc_grid);
    
    /* Master process */
    if( pid == 0) {
      m_x = _x;
      m_y = _y;
      m_z = _z;
      if( m_x == 0 || m_y == 0 || m_z == 0 )
        throw std::invalid_argument("[ERROR] Array is zero");
      /* Allocate memory for data grid */
      for(int i = 0; i < TIME_DIM; i++)
        m_data[i] = new DATATYPE[ m_x * m_y * m_z ];
    }
    /* Slave process */
    else {
      m_x = 0;
      m_y = 0;
      m_z = 0;
    }
  }
  
  void scatter(void) {
    int x, y, z;
    int x_begin, x_end, x_size, y_begin, y_size, y_end, z_begin, z_size, z_end;
    if( proc_grid->getPID() == 0 ) {
      if( PROC_DIMS == 2 ) {
        proc_grid->dims(x, y);
        z = 1;
      }
      else if( PROC_DIMS == 3) {
        proc_grid->dims(x, y, z);
      }
      for( int i = 0; i < proc_grid->size(); i++ ) {
        x_begin = 
      }
    }
    else {
      
    }
  }
  
  void gather(int _slicenum) {
    
  }
  
  DATATYPE& operator() (unsigned _time, unsigned _x, unsigned _y, unsigned _z) {
    if ( _x > m_x || _y > m_y || _z > m_z )
      throw std::out_of_range("Matrix subscript out of bounds");
    return m_data[_time][ m_y * m_x * _z + m_x * _y + _x ];
    //HEIGHT*WIDTH*z + WIDTH*y + x
  }
  
  DATATYPE& operator() (unsigned _time, unsigned _x, unsigned _y) {
    if ( _x > m_x || _y > m_y )
      throw std::out_of_range("Matrix subscript out of bounds");
    return m_data[_time][ m_y * _x + _y ];
  }
  
  ~Grid() {
    /* Collect all threads and finalize MPI */
    MPI_Barrier( proc_grid->communicator() );
    MPI_Finalize();
    //delete[] m_data;
  }

  
};
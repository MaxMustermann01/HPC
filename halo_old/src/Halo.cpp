
#include <Halo.h>

template <int DIMS>
Grid::Grid(int _argc, char** _argv) {
  int pid, nprocs;
  MPI_Comm CartComm;
  
  /* Initialize MPI */
  MPI_Init(&_argc, &_argv)
  /* Get process ID and proc. count */
  MPI_rank(MPI_COMM_WORLD, &pid);
  MPI_size(MPI_COMM_WORLD, &nprocs);
  
  int dims[DIMS], period[DIMS];
  std::fill(dims, dims+DIM, 0);
  std::fill(period, period+DIM, 0);
  /* Create MPI communicator with respect of chosen dimension */
  MPI_Dims_create(nprocs, DIMS, dims);
  MPI_Cart_create(MPI_COMM_WORLD, DIMS, dims, period, false, &CartComm);
  /* Create the process grid */
  proc_grid(CartComm);
  /* Create the transmit object */
  transmit_buffer(proc_grid);
}

Grid::~Grid() {
  /* Collect all threads and finalize MPI */
  MPI_Barrier( proc_grid.communicator() );
  MPI_Finalize();
}


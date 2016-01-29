#include <iostream>
#include <stdio.h>
#include <vector>
#include <mpi.h>
#include <TransmitBuffer.h>
#include <chrono>
#include <fstream>
#include <sstream>

#define ITERATIONS 1

int main(int argc, char** argv) {
  int pid, nprocs, N, M;
  int dims[2] = {0, 0}, period[2] = {0, 0};
  MPI_Comm CartComm;
  
  /* Initialize MPI */
  MPI_Init(&argc, &argv);
  
  std::istringstream ss(argv[1]);
  int gridsize;
  if (!(ss >> gridsize))
    std::cerr << "Invalid Argument " << argv[1] << '\n';
  
  MPI_Comm_rank(MPI_COMM_WORLD, &pid);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  /* Create cart communicator */
  MPI_Dims_create(nprocs, 2, dims);
  MPI_Cart_create(MPI_COMM_WORLD, 2, dims, period, false, &CartComm);
  /* Create TransmitBuffer object */
  TransmitBuffer<2> tb(CartComm);
  
  std::stringstream fname ;
  fname << "benchmark/halo_2D_" << nprocs << ".dat" ;
  std::ofstream f_mr;
  if (pid == 0) f_mr.open(fname.str(), std::ios_base::app);
  
  tb.getdims(N, M);
  
  /* ... */
  //for (auto gridsize = 32; gridsize < 16384; gridsize *= 2) {
    int ysize = gridsize/N;
    int xsize = gridsize/M;
    
    std::vector<double> iminus(gridsize*xsize), iplus(gridsize*xsize);
    std::vector<double> iminus_r(gridsize*xsize), iplus_r(gridsize*xsize);
    std::vector<double> jminus(gridsize*ysize), jplus(gridsize*ysize);
    std::vector<double> jminus_r(gridsize*ysize), jplus_r(gridsize*ysize);
    
    tb.init_send_buffer(&iminus[0], MPI_DOUBLE, (gridsize*xsize), -1, 0);
    tb.init_send_buffer(&iplus[0], MPI_DOUBLE, (gridsize*xsize), 1, 0);
    tb.init_send_buffer(&jminus[0], MPI_DOUBLE, (gridsize*ysize), 0, -1);
    tb.init_send_buffer(&jplus[0], MPI_DOUBLE, (gridsize*ysize), 0, 1);
    
    tb.init_recv_buffer(&iminus_r[0], MPI_DOUBLE, (gridsize*xsize), -1, 0);
    tb.init_recv_buffer(&iplus_r[0], MPI_DOUBLE, (gridsize*xsize), 1, 0);
    tb.init_recv_buffer(&jminus_r[0], MPI_DOUBLE, (gridsize*ysize), 0, -1);
    tb.init_recv_buffer(&jplus_r[0], MPI_DOUBLE, (gridsize*ysize), 0, 1);
    
    std::fill(&iminus[0], &iminus[gridsize*xsize-1], 1.1);
    std::fill(&iplus[0], &iplus[gridsize*xsize-1], 1.1);
    std::fill(&jminus[0], &jminus[gridsize*ysize-1], 1.1);
    std::fill(&jplus[0], &jplus[gridsize*ysize-1], 1.1);
    
    auto t1 = std::chrono::high_resolution_clock::now();
    for (auto it = 0; it < ITERATIONS; it++) {
      tb.do_receives();
      tb.do_sends();
      tb.do_waits();
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    if (pid == 0) f_mr << gridsize << "\t" << int_ms.count()/ITERATIONS << std::endl;
  //}
  if (pid == 0) f_mr.close();
  
  MPI_Barrier(CartComm);
  
  MPI_Finalize();
  
  return 0;
}
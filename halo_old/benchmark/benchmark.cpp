#include <iostream>
#include <stdio.h>
#include <vector>
#include <mpi.h>
#include <TransmitBuffer.h>
#include <chrono>
#include <fstream>
#include <sstream>

#define ITERATIONS 100

int main(int argc, char** argv) {
  int pid, nprocs;
  /* 1D case */
  int dims_1D[1] = {0}, period_1D[1] = {0};
  MPI_Comm CartComm_1D;
  
  /* Initialize MPI */
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &pid);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  /* Create cart communicator for 1D */
  MPI_Dims_create(nprocs, 1, dims_1D);
  MPI_Cart_create(MPI_COMM_WORLD, 1, dims_1D, period_1D, false, &CartComm_1D);
  /* Create TransmitBuffer object for every dimension case */
  TransmitBuffer<1> tb_1D(CartComm_1D);
  
  std::stringstream fname ;
  fname << "benchmark/halo_1D_" << nprocs << ".dat" ;
  std::ofstream f_mr;
  if (pid == 0) f_mr.open(fname.str());
    
  /* ... */
  for (auto gridsize = 1024; gridsize < 4194304; gridsize *= 2) {
    std::vector<double> iminus(gridsize*gridsize), iplus(gridsize*gridsize);
    std::vector<double> iminus_r(gridsize*gridsize), iplus_r(gridsize*gridsize);
    
    tb_1D.init_send_buffer(&iminus[0], MPI_DOUBLE, (gridsize*gridsize), -1);
    tb_1D.init_send_buffer(&iplus[0], MPI_DOUBLE, (gridsize*gridsize), 1);
    
    tb_1D.init_recv_buffer(&iminus_r[0], MPI_DOUBLE, (gridsize*gridsize), -1);
    tb_1D.init_recv_buffer(&iplus_r[0], MPI_DOUBLE, (gridsize*gridsize), 1);
    
    std::fill(&iminus[0], &iminus[gridsize*gridsize-1], 1.1);
    std::fill(&iplus[0], &iplus[gridsize*gridsize-1], 1.1);
    
    auto t1 = std::chrono::high_resolution_clock::now();
    for (auto it = 0; it < ITERATIONS; it++) {
      tb_1D.do_receives();
      tb_1D.do_sends();
      tb_1D.do_waits();
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    if (pid == 0) f_mr << gridsize << "\t" << int_ms.count()/ITERATIONS << std::endl;
  }
  if (pid == 0) f_mr.close();
  
  MPI_Barrier(CartComm_1D);
  
  MPI_Finalize();
  
  return 0;
}
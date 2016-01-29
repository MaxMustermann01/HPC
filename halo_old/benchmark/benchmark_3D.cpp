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
  int pid, nprocs, N, M, O;
  int dims[3] = {0, 0, 0}, period[3] = {0, 0, 0};
  MPI_Comm CartComm;
  
  /* Initialize MPI */
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &pid);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  /* Create cart communicator */
  MPI_Dims_create(nprocs, 3, dims);
  MPI_Cart_create(MPI_COMM_WORLD, 3, dims, period, false, &CartComm);
  /* Create TransmitBuffer object */
  TransmitBuffer<3> tb(CartComm);
  
  std::stringstream fname ;
  fname << "benchmark/halo_3D_" << nprocs << ".dat" ;
  std::ofstream f_mr;
  if (pid == 0) f_mr.open(fname.str());
  
  tb.getdims(N, M, O);
  
  /* ... */
  for (auto gridsize = 32; gridsize < 16384; gridsize *= 2) {
    int ysize = gridsize/N;
    int xsize = gridsize/M;
    int zsize = gridsize/O;
    
    std::vector<double> iminus(zsize*xsize), iplus(zsize*xsize);
    std::vector<double> iminus_r(zsize*xsize), iplus_r(zsize*xsize);
    std::vector<double> jminus(zsize*ysize), jplus(zsize*ysize);
    std::vector<double> jminus_r(zsize*ysize), jplus_r(zsize*ysize);
    std::vector<double> kminus(xsize*ysize), kplus(xsize*ysize);
    std::vector<double> kminus_r(xsize*ysize), kplus_r(xsize*ysize);
    
    tb.init_send_buffer(&iminus[0], MPI_DOUBLE, (zsize*xsize), -1, 0, 0);
    tb.init_send_buffer(&iplus[0], MPI_DOUBLE, (zsize*xsize), 1, 0, 0);
    tb.init_send_buffer(&jminus[0], MPI_DOUBLE, (zsize*ysize), 0, -1, 0);
    tb.init_send_buffer(&jplus[0], MPI_DOUBLE, (zsize*ysize), 0, 1, 0);
    tb.init_send_buffer(&kminus[0], MPI_DOUBLE, (xsize*ysize), 0, 0, -1);
    tb.init_send_buffer(&kplus[0], MPI_DOUBLE, (xsize*ysize), 0, 0, 1);
    
    tb.init_recv_buffer(&iminus_r[0], MPI_DOUBLE, (zsize*xsize), -1, 0, 0);
    tb.init_recv_buffer(&iplus_r[0], MPI_DOUBLE, (zsize*xsize), 1, 0, 0);
    tb.init_recv_buffer(&jminus_r[0], MPI_DOUBLE, (zsize*ysize), 0, -1, 0);
    tb.init_recv_buffer(&jplus_r[0], MPI_DOUBLE, (zsize*ysize), 0, 1, 0);
    tb.init_recv_buffer(&kminus_r[0], MPI_DOUBLE, (xsize*ysize), 0, 0, -1);
    tb.init_recv_buffer(&kplus_r[0], MPI_DOUBLE, (xsize*ysize), 0, 0, 1);
    
    std::fill(&iminus[0], &iminus[zsize*xsize-1], 1.1);
    std::fill(&iplus[0], &iplus[zsize*xsize-1], 1.1);
    std::fill(&jminus[0], &jminus[zsize*ysize-1], 1.1);
    std::fill(&jplus[0], &jplus[zsize*ysize-1], 1.1);
    std::fill(&kminus[0], &kminus[xsize*ysize-1], 1.1);
    std::fill(&kplus[0], &kplus[xsize*ysize-1], 1.1);
    
    auto t1 = std::chrono::high_resolution_clock::now();
    for (auto it = 0; it < ITERATIONS; it++) {
      tb.do_receives();
      tb.do_sends();
      tb.do_waits();
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    if (pid == 0) f_mr << gridsize << "\t" << int_ms.count()/ITERATIONS << std::endl;
  }
  if (pid == 0) f_mr.close();
  
  MPI_Barrier(CartComm);
  
  MPI_Finalize();
  
  return 0;
}
#include <iostream>
#include <stdio.h>
#include <vector>
#include <array>
#include <mpi.h>
#include <TransmitBuffer.h>

#define N 1024
#define ITERATIONS 1000
#define FI 24/100

inline double calcStencil( double ijminus, 
			   double iminusj, 
			   double ij, 
			   double iplusj,
			   double ijplus ) {
  return ij + FI * ( (-4) * ij + iplusj + iminusj + ijplus + ijminus );
}

int main(int argc, char** argv) {
  int nprocs, dims[1] = {0}, period[1] = {0};
  int Index = 0;
  MPI_Comm CartComm;
  
  //using trans_buffer_type = typename TransmitBuffer<1>;
  
  /* Initialize MPI */
  MPI_Init(&argc, &argv);
  
  /* Create 1D cart communicator */
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Dims_create(nprocs, 1, dims);
  MPI_Cart_create(MPI_COMM_WORLD, 1, dims, period, false, &CartComm);
  
  /* Create transmit buffers
   * Note: We use two transmit buffers (also two grids), so that we can
   *       efficiently overlap communication with computation.
   */
  std::array<TransmitBuffer<1> *, 2> tb;
  //std::fill(tb.begin(), tb.end(), TransmitBuffer<1>(CartComm));
  tb[0] = new TransmitBuffer<1>(CartComm);
  tb[1] = new TransmitBuffer<1>(CartComm);
  //tb[1](CartComm);
  
  /* Allocate actual grids */
  std::array<std::vector<double>, 2> grid;
  std::fill(grid.begin(), grid.end(), std::vector<double>(N*(N+2)));
  
  /* Initialize send-/receive-buffer for halos */
  tb[0]->init_send_buffer(&grid[0][N*1], MPI_DOUBLE, N, -1);
  tb[0]->init_send_buffer(&grid[0][N*N], MPI_DOUBLE, N, 1);
  tb[0]->init_recv_buffer(&grid[0][N*0], MPI_DOUBLE, N, -1);
  tb[0]->init_recv_buffer(&grid[0][N*(N+1)], MPI_DOUBLE, N, 1);
  
  tb[1]->init_send_buffer(&grid[1][N*1], MPI_DOUBLE, N, -1);
  tb[1]->init_send_buffer(&grid[1][N*N], MPI_DOUBLE, N, 1);
  tb[1]->init_recv_buffer(&grid[1][N*0], MPI_DOUBLE, N, -1);
  tb[1]->init_recv_buffer(&grid[1][N*(N+1)], MPI_DOUBLE, N, 1);
  
  /* Start with the stencil computation */
  for(auto i=0; i<ITERATIONS; i++) {
    /* Pre-post all halos (non-blocking) */
    tb[Index]->do_receives();
    tb[Index]->do_sends();
    /* Calculate all interior elements while halos are exchanged */
    for(auto x=2; x<N; x++)
      for(auto y=1; y<N-1; y++)
        grid[1-Index][x+N*y] = calcStencil( grid[Index][x+N*(y-1)],
                                            grid[Index][x-1+N*y],
                                            grid[Index][x+N*y],
                                            grid[Index][x+1+N*y],
                                            grid[Index][x+N*(y+1)] );
    /* Wait for halos to be exchanged */
    tb[Index]->do_waits();
    /* Finally calculate the border elements */
    auto x = 1;
    for(auto xx=0; xx<2; xx++)
      for(auto y=1; y<N-1; y++) {
        grid[1-Index][x+N*y] = calcStencil( grid[Index][x+N*(y-1)],
                                            grid[Index][x-1+N*y],
                                            grid[Index][x+N*y],
                                            grid[Index][x+1+N*y],
                                            grid[Index][x+N*(y+1)] );
        x = N;
      }
    /* Reverse index for next iteration */
    Index = 1 - Index;
  }
  
  /* Finalize MPI */
  MPI_Finalize();
  
  return 0;
}
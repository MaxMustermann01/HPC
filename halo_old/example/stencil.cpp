#include <iostream>
#include <stdio.h>
#include <vector>
#include <array>
#include <mpi.h>
#include <Proc_Grid_1D.h>
#include <TransmitBuffer_1D.h>

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
  int pid, nprocs, dims[1] = {0}, period[1] = {0};
  int Index = 0;
  MPI_Comm CartComm;
  
  /* Initialize MPI */
  MPI_Init(&argc, &argv);
  
  //MPI_Comm_rank(MPI_COMM_WORLD, &pid);
  //MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Dims_create(nprocs, 1, dims);
  MPI_Cart_create(MPI_COMM_WORLD, 1, dims, period, false, &CartComm);
  
  /* Create 1D processor grid for communication */
  Proc_Grid_1D_t proc_grid(CartComm);
  
  /* Create transmit buffers
   * Note: We use two transmit buffers (also two grids), so that we can
   *       overlap communication with computation efficiently.
   */
  std::array<TransmitBuffer_1D, 2> tb;
  std::fill(tb.begin(), tb.end(), TransmitBuffer_1D (&proc_grid));
  
  /* Allocate actual grids */
  std::array<std::vector<double>, 2> grid;
  std::fill(grid.begin(), grid.end(), std::vector<double>(N*(N+2)));
  
  /* Initialize send-/receive-buffer for halos */
  tb[0].init_send_buffer<-1>(&grid[0]+N*1, MPI_DOUBLE, N);
  tb[0].init_send_buffer<1>(&grid[0]+N*N, MPI_DOUBLE, N);
  tb[0].init_recv_buffer<-1>(&grid[0]+N*0, MPI_DOUBLE, N);
  tb[0].init_recv_buffer<1>(&grid[0]+N*(N+1), MPI_DOUBLE, N);
  
  tb[1].init_send_buffer<-1>(&grid[1]+N*1, MPI_DOUBLE, N);
  tb[1].init_send_buffer<1>(&grid[1]+N*N, MPI_DOUBLE, N);
  tb[1].init_recv_buffer<-1>(&grid[1]+N*0, MPI_DOUBLE, N);
  tb[1].init_recv_buffer<1>(&grid[1]+N*(N+1), MPI_DOUBLE, N);
  
  /* Start with the stencil computation */
  for(auto i=0; i<ITERATIONS; i++) {
    /* Pre-post all halos (non-blocking) */
    tb[Index].do_receives();
    tb[Index].do_sends();
    /* Calculate all interior elements while halos are exchanged */
    for(auto x=2; x<N; x++)
      for(auto y=0; y<N; y++)
        grid[1-Index][x+N*y] = calcStencil( grid[Index][x+N*(y-1)],
                                            grid[Index][x-1+N*y],
                                            grid[Index][x+N*y],
                                            grid[Index][x+1+N*y],
                                            grid[Index][x+N*(y+1)] );
    /* Wait for halos to be exchanged */
    tb[Index].do_waits();
    /* Finally calculate the border elements */
    auto x = 1;
    for(auto xx=0; xx<2; xx++)
      for(auto y=0; y<N; y++) {
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
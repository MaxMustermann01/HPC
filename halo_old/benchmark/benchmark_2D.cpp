#include <iostream>
#include <stdio.h>
#include <vector>
#include <mpi.h>
#include <TransmitBuffer.h>
#include <chrono>
#include <fstream>
#include <sstream>
#include <random>

#define EDGE_LENGTH 64
#define FI 24/100

inline double calcStencil( double ijminus, 
			   double iminusj, 
			   double kminus,
			   double ij, 
			   double iplusj,
			   double ijplus,
			   double kplus ) {
  return ij + FI * ( (-6) * ij + iplusj + iminusj + kminus + ijplus + ijminus + kplus );
}

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
  
  /* Choose an apropiate number of iterations */
  int iterations;
  if (gridsize < 2000) iterations = 100;
  else if (gridsize < 4000) iterations = 10;
  else iterations = 1;
  
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
  
  double grid[2][EDGE_LENGTH][EDGE_LENGTH][EDGE_LENGTH];
  /* Init grid with random values */
  std::default_random_engine generator;
  std::uniform_int_distribution<int> distribution(1,6);
  for (auto h=0; h < 2; h++)
    for (auto i=0; i < EDGE_LENGTH; i++)
      for (auto j=0; j < EDGE_LENGTH; j++)
        for (auto k=0; k < EDGE_LENGTH; k++)
            grid[h][i][j][k] = distribution(generator);
  
  int ysize = gridsize/N;
  int xsize = gridsize/M;
    
  /* Main halo elements */
  std::vector<double> iminus(gridsize*xsize), iplus(gridsize*xsize);
  std::vector<double> iminus_r(gridsize*xsize), iplus_r(gridsize*xsize);
  std::vector<double> jminus(gridsize*ysize), jplus(gridsize*ysize);
  std::vector<double> jminus_r(gridsize*ysize), jplus_r(gridsize*ysize);
  /* Edge halo elements */
  double iminusjminus=1.1, iminusjminus_r;
  double iplusjminus=1.1, iplusjminus_r;
  double iminusjplus=1.1, iminusjplus_r;
  double iplusjplus=1.1, iplusjplus_r;
  
  /* Init main send buffers */
  tb.init_send_buffer(&iminus[0], MPI_DOUBLE, (gridsize*xsize), -1, 0);
  tb.init_send_buffer(&iplus[0], MPI_DOUBLE, (gridsize*xsize), 1, 0);
  tb.init_send_buffer(&jminus[0], MPI_DOUBLE, (gridsize*ysize), 0, -1);
  tb.init_send_buffer(&jplus[0], MPI_DOUBLE, (gridsize*ysize), 0, 1);
  /* Init edge send buffers */
  tb.init_send_buffer(&iminusjminus, MPI_DOUBLE, 1, -1, -1);
  tb.init_send_buffer(&iplusjminus, MPI_DOUBLE, 1, 1, -1);
  tb.init_send_buffer(&iminusjplus, MPI_DOUBLE, 1, -1, 1);
  tb.init_send_buffer(&iplusjplus, MPI_DOUBLE, 1, 1, 1);
  
  /* Init main recv buffers */
  tb.init_recv_buffer(&iminus_r[0], MPI_DOUBLE, (gridsize*xsize), -1, 0);
  tb.init_recv_buffer(&iplus_r[0], MPI_DOUBLE, (gridsize*xsize), 1, 0);
  tb.init_recv_buffer(&jminus_r[0], MPI_DOUBLE, (gridsize*ysize), 0, -1);
  tb.init_recv_buffer(&jplus_r[0], MPI_DOUBLE, (gridsize*ysize), 0, 1);
  /* Init edge recv buffers */
  tb.init_recv_buffer(&iminusjminus_r, MPI_DOUBLE, 1, -1, -1);
  tb.init_recv_buffer(&iplusjminus_r, MPI_DOUBLE, 1, 1, -1);
  tb.init_recv_buffer(&iminusjplus_r, MPI_DOUBLE, 1, -1, 1);
  tb.init_recv_buffer(&iplusjplus_r, MPI_DOUBLE, 1, 1, 1);
    
  std::fill(&iminus[0], &iminus[gridsize*xsize-1], 1.1);
  std::fill(&iplus[0], &iplus[gridsize*xsize-1], 1.1);
  std::fill(&jminus[0], &jminus[gridsize*ysize-1], 1.1);
  std::fill(&jplus[0], &jplus[gridsize*ysize-1], 1.1);
  
  /* Measure halo exchange time without computation */
  auto t1 = std::chrono::high_resolution_clock::now();
  for (auto it = 0; it < iterations; it++) {
    tb.do_receives();
    tb.do_sends();
    tb.do_waits();
    MPI_Barrier(CartComm);
  }
  auto t2 = std::chrono::high_resolution_clock::now();
  auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
  
  /* Measure halo exchange time with computation */
  int index = 0;
  auto t3 = std::chrono::high_resolution_clock::now();
  for (auto it = 0; it < iterations; it++) {
    tb.do_receives();
    tb.do_sends();
    
    /* Do the actual stencil calculation */
    int ii=1, jj=1, kk=1;
    for (auto i=0; i < xsize; i++) {
      if (ii == (EDGE_LENGTH-3)) ii=1;
      else ii++;
      
      for (auto j=0; j < ysize; j++) {
        if (jj == (EDGE_LENGTH-3)) jj=1;
        else jj++;

        for (auto k=0; k < gridsize; k++) {
          if (kk == (EDGE_LENGTH-3)) kk=1;
          else kk++;

          grid[1-index][ii][jj][kk] = calcStencil( grid[index][ii-1][jj][kk],
                                                   grid[index][ii][jj-1][kk],
                                                   grid[index][ii][jj][kk-1],
                                                   grid[index][ii][jj][kk],
                                                   grid[index][ii+1][jj][kk],
                                                   grid[index][ii][jj+1][kk],
                                                   grid[index][ii][jj][kk+1] );
        }
      }
    }
    
    tb.do_waits();
    MPI_Barrier(CartComm);
    /* Reverse index */
    index = 1 - index;
  }
  auto t4 = std::chrono::high_resolution_clock::now();
  auto int_ms_calc = std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3);
  
  if (pid == 0) f_mr << gridsize << "\t" << int_ms.count()/iterations << "\t" << int_ms_calc.count()/iterations << std::endl;
  
  if (pid == 0) f_mr.close();
  
  MPI_Finalize();
  
  return 0;
}
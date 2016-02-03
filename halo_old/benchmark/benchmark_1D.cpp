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
  int pid, nprocs;
  int dims[1] = {0}, period[1] = {0};
  MPI_Comm CartComm;
  int N;
  
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
  MPI_Dims_create(nprocs, 1, dims);
  MPI_Cart_create(MPI_COMM_WORLD, 1, dims, period, false, &CartComm);
  /* Create TransmitBuffer object */
  TransmitBuffer<1> tb(CartComm);
  
  std::stringstream fname ;
  fname << "benchmark/halo_1D_comm_" << nprocs << ".dat" ;
  std::ofstream f_mr;
  if (pid == 0) f_mr.open(fname.str(), std::ios_base::app);
  
  std::stringstream fnamecalc ;
  fnamecalc << "benchmark/halo_1D_calc_" << nprocs << ".dat" ;
  std::ofstream f_mr_calc;
  if (pid == 0) f_mr_calc.open(fnamecalc.str(), std::ios_base::app);
  
  tb.getdims(N);
  
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
    
  std::vector<double> iminus(gridsize*gridsize), iplus(gridsize*gridsize);
  std::vector<double> iminus_r(gridsize*gridsize), iplus_r(gridsize*gridsize);
    
  tb.init_send_buffer(&iminus[0], MPI_DOUBLE, (gridsize*gridsize), -1);
  tb.init_send_buffer(&iplus[0], MPI_DOUBLE, (gridsize*gridsize), 1);
    
  tb.init_recv_buffer(&iminus_r[0], MPI_DOUBLE, (gridsize*gridsize), -1);
  tb.init_recv_buffer(&iplus_r[0], MPI_DOUBLE, (gridsize*gridsize), 1);
    
  std::fill(&iminus[0], &iminus[gridsize*gridsize-1], 1.1);
  std::fill(&iplus[0], &iplus[gridsize*gridsize-1], 1.1);
  
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
    for (auto i=0; i < gridsize; i++) {
      if (ii > (EDGE_LENGTH-2)) ii=1;
      else ii++;
      
      for (auto j=0; j < ysize; j++) {
        if (jj > (EDGE_LENGTH-2)) jj=1;
        else jj++;

        for (auto k=0; k < gridsize; k++) {
          if (kk > (EDGE_LENGTH-2)) kk=1;
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
  
  if (pid == 0) f_mr << gridsize << "\t" << int_ms.count()/iterations << std::endl;
  if (pid == 0) f_mr_calc << gridsize << "\t" << int_ms_calc.count()/iterations << std::endl;
  
  if (pid == 0) f_mr.close();
  if (pid == 0) f_mr_calc.close();
  
  MPI_Finalize();
  
  return 0;
}
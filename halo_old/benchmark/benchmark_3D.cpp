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
  int pid, nprocs, N, M, O;
  int dims[3] = {0, 0, 0}, period[3] = {0, 0, 0};
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
  MPI_Dims_create(nprocs, 3, dims);
  MPI_Cart_create(MPI_COMM_WORLD, 3, dims, period, false, &CartComm);
  /* Create TransmitBuffer object */
  TransmitBuffer<3> tb(CartComm);
  
  std::stringstream fname ;
  fname << "benchmark/halo_3D_comm_" << nprocs << ".dat" ;
  std::ofstream f_mr;
  if (pid == 0) f_mr.open(fname.str(), std::ios_base::app);
  
  std::stringstream fnamecalc ;
  fnamecalc << "benchmark/halo_3D_calc_" << nprocs << ".dat" ;
  std::ofstream f_mr_calc;
  if (pid == 0) f_mr_calc.open(fnamecalc.str(), std::ios_base::app);
  
  tb.getdims(N, M, O);
  
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
  int zsize = gridsize/O;
  
  /* Main halo elements */
  std::vector<double> iminus(zsize*xsize), iplus(zsize*xsize);
  std::vector<double> iminus_r(zsize*xsize), iplus_r(zsize*xsize);
  std::vector<double> jminus(zsize*ysize), jplus(zsize*ysize);
  std::vector<double> jminus_r(zsize*ysize), jplus_r(zsize*ysize);
  std::vector<double> kminus(xsize*ysize), kplus(xsize*ysize);
  std::vector<double> kminus_r(xsize*ysize), kplus_r(xsize*ysize);
  /* Edge halo elements */
  double iplusjpluskminus=1.1, iplusjpluskminus_r;
  double jpluskminus=1.1, jpluskminus_r;
  double iminusjpluskminus=1.1, iminusjpluskminus_r;
  double ipluskminus=1.1, ipluskminus_r;
  double iminuskminus=1.1, iminuskminus_r;
  double iplusjminuskminus=1.1, iplusjminuskminus_r;
  double jminuskminus=1.1, jminuskminus_r;
  double iminusjminuskminus=1.1, iminusjminuskminus_r;
  double iplusjplus=1.1, iplusjplus_r;
  double iminusjplus=1.1, iminusjplus_r;
  double iplusjminus=1.1, iplusjminus_r;
  double iminusjminus=1.1, iminusjminus_r;
  double iplusjpluskplus=1.1, iplusjpluskplus_r;
  double jpluskplus=1.1, jpluskplus_r;
  double iminusjpluskplus=1.1, iminusjpluskplus_r;
  double ipluskplus=1.1, ipluskplus_r;
  double iminuskplus=1.1, iminuskplus_r;
  double iplusjminuskplus=1.1, iplusjminuskplus_r;
  double jminuskplus=1.1, jminuskplus_r;
  double iminusjminuskplus=1.1, iminusjminuskplus_r;
  
  /* Init main send buffers */
  tb.init_send_buffer(&iminus[0], MPI_DOUBLE, (zsize*xsize), -1, 0, 0);
  tb.init_send_buffer(&iplus[0], MPI_DOUBLE, (zsize*xsize), 1, 0, 0);
  tb.init_send_buffer(&jminus[0], MPI_DOUBLE, (zsize*ysize), 0, -1, 0);
  tb.init_send_buffer(&jplus[0], MPI_DOUBLE, (zsize*ysize), 0, 1, 0);
  tb.init_send_buffer(&kminus[0], MPI_DOUBLE, (xsize*ysize), 0, 0, -1);
  tb.init_send_buffer(&kplus[0], MPI_DOUBLE, (xsize*ysize), 0, 0, 1);
  /* Init edge send buffers */
  tb.init_send_buffer(&iplusjpluskminus, MPI_DOUBLE, 1, 1, 1, -1);
  tb.init_send_buffer(&jpluskminus, MPI_DOUBLE, 1, 0, 1, -1);
  tb.init_send_buffer(&iminusjpluskminus, MPI_DOUBLE, 1, -1, 1, -1);
  tb.init_send_buffer(&ipluskminus, MPI_DOUBLE, 1, 1, 0, -1);
  tb.init_send_buffer(&iminuskminus, MPI_DOUBLE, 1, -1, 0, -1);
  tb.init_send_buffer(&iplusjminuskminus, MPI_DOUBLE, 1, 1, -1, -1);
  tb.init_send_buffer(&jminuskminus, MPI_DOUBLE, 1, 0, -1, -1);
  tb.init_send_buffer(&iminusjminuskminus, MPI_DOUBLE, 1, -1, -1, -1);
  tb.init_send_buffer(&iplusjplus, MPI_DOUBLE, 1, 1, 1, 0);
  tb.init_send_buffer(&iminusjplus, MPI_DOUBLE, 1, -1, 1, 0);
  tb.init_send_buffer(&iplusjminus, MPI_DOUBLE, 1, 1, -1, 0);
  tb.init_send_buffer(&iminusjminus, MPI_DOUBLE, 1, -1, -1, 0);
  tb.init_send_buffer(&iplusjpluskplus, MPI_DOUBLE, 1, 1, 1, 1);
  tb.init_send_buffer(&jpluskplus, MPI_DOUBLE, 1, 0, 1, 1);
  tb.init_send_buffer(&iminusjpluskplus, MPI_DOUBLE, 1, -1, 1, 1);
  tb.init_send_buffer(&ipluskplus, MPI_DOUBLE, 1, 1, 0, 1);
  tb.init_send_buffer(&iminuskplus, MPI_DOUBLE, 1, -1, 0, 1);
  tb.init_send_buffer(&iplusjminuskplus, MPI_DOUBLE, 1, 1, -1, 1);
  tb.init_send_buffer(&jminuskplus, MPI_DOUBLE, 1, 0, -1, 1);
  tb.init_send_buffer(&iminusjminuskplus, MPI_DOUBLE, 1, -1, -1, 1);
  
  /* Init main recv buffers */
  tb.init_recv_buffer(&iminus_r[0], MPI_DOUBLE, (zsize*xsize), -1, 0, 0);
  tb.init_recv_buffer(&iplus_r[0], MPI_DOUBLE, (zsize*xsize), 1, 0, 0);
  tb.init_recv_buffer(&jminus_r[0], MPI_DOUBLE, (zsize*ysize), 0, -1, 0);
  tb.init_recv_buffer(&jplus_r[0], MPI_DOUBLE, (zsize*ysize), 0, 1, 0);
  tb.init_recv_buffer(&kminus_r[0], MPI_DOUBLE, (xsize*ysize), 0, 0, -1);
  tb.init_recv_buffer(&kplus_r[0], MPI_DOUBLE, (xsize*ysize), 0, 0, 1);
  /* Init edge recv buffers */
  tb.init_recv_buffer(&iplusjpluskminus_r, MPI_DOUBLE, 1, 1, 1, -1);
  tb.init_recv_buffer(&jpluskminus_r, MPI_DOUBLE, 1, 0, 1, -1);
  tb.init_recv_buffer(&iminusjpluskminus_r, MPI_DOUBLE, 1, -1, 1, -1);
  tb.init_recv_buffer(&ipluskminus_r, MPI_DOUBLE, 1, 1, 0, -1);
  tb.init_recv_buffer(&iminuskminus_r, MPI_DOUBLE, 1, -1, 0, -1);
  tb.init_recv_buffer(&iplusjminuskminus_r, MPI_DOUBLE, 1, 1, -1, -1);
  tb.init_recv_buffer(&jminuskminus_r, MPI_DOUBLE, 1, 0, -1, -1);
  tb.init_recv_buffer(&iminusjminuskminus_r, MPI_DOUBLE, 1, -1, -1, -1);
  tb.init_recv_buffer(&iplusjplus_r, MPI_DOUBLE, 1, 1, 1, 0);
  tb.init_recv_buffer(&iminusjplus_r, MPI_DOUBLE, 1, -1, 1, 0);
  tb.init_recv_buffer(&iplusjminus_r, MPI_DOUBLE, 1, 1, -1, 0);
  tb.init_recv_buffer(&iminusjminus_r, MPI_DOUBLE, 1, -1, -1, 0);
  tb.init_recv_buffer(&iplusjpluskplus_r, MPI_DOUBLE, 1, 1, 1, 1);
  tb.init_recv_buffer(&jpluskplus_r, MPI_DOUBLE, 1, 0, 1, 1);
  tb.init_recv_buffer(&iminusjpluskplus_r, MPI_DOUBLE, 1, -1, 1, 1);
  tb.init_recv_buffer(&ipluskplus_r, MPI_DOUBLE, 1, 1, 0, 1);
  tb.init_recv_buffer(&iminuskplus_r, MPI_DOUBLE, 1, -1, 0, 1);
  tb.init_recv_buffer(&iplusjminuskplus_r, MPI_DOUBLE, 1, 1, -1, 1);
  tb.init_recv_buffer(&jminuskplus_r, MPI_DOUBLE, 1, 0, -1, 1);
  tb.init_recv_buffer(&iminusjminuskplus_r, MPI_DOUBLE, 1, -1, -1, 1);
    
  std::fill(&iminus[0], &iminus[zsize*xsize-1], 1.1);
  std::fill(&iplus[0], &iplus[zsize*xsize-1], 1.1);
  std::fill(&jminus[0], &jminus[zsize*ysize-1], 1.1);
  std::fill(&jplus[0], &jplus[zsize*ysize-1], 1.1);
  std::fill(&kminus[0], &kminus[xsize*ysize-1], 1.1);
  std::fill(&kplus[0], &kplus[xsize*ysize-1], 1.1);
  
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
      if (ii > (EDGE_LENGTH-2)) ii=1;
      else ii++;
      
      for (auto j=0; j < ysize; j++) {
        if (jj > (EDGE_LENGTH-2)) jj=1;
        else jj++;

        for (auto k=0; k < zsize; k++) {
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
  
  /* Write results into file and close files */
  if (pid == 0) f_mr << gridsize << "\t" << int_ms.count()/iterations << std::endl;
  if (pid == 0) f_mr_calc << gridsize << "\t" << int_ms_calc.count()/iterations << std::endl;
  
  if (pid == 0) f_mr.close();
  if (pid == 0) f_mr_calc.close();
  
  MPI_Finalize();
  
  return 0;
}
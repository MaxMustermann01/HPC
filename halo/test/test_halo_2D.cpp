
#include <iostream>
#include <stdio.h>
#include <vector>
#include <mpi.h>
#include <2D_proc_grid.h>
#include <Halo_2D.h>

int main(int argc, char** argv) {
  int pid, nprocs, dims[2] = {0, 0}, period[2] = {0, 0};
  int k = 16;
  int i, j, N, M;
  MPI_Comm CartComm;
  
  MPI_Init(&argc, &argv);
  
  MPI_Comm_rank(MPI_COMM_WORLD, &pid);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Dims_create(nprocs, 2, dims);
  MPI_Cart_create(MPI_COMM_WORLD, 2, dims, period, false, &CartComm);
  
  MPI_2D_proc_grid_t pg(CartComm);
  
  //pg(CartComm);
  
  Halo_2D he(pg);
  
  pg.coords(i, j);
  pg.dims(N, M);
  
  std::vector<int> iminus( k+i+j );
  std::vector<int> iplus( k+i+j );
  std::vector<int> jminus( k+i+j );
  std::vector<int> jplus( k+i+j );
  std::vector<int> iminusjminus( k+i+j );
  std::vector<int> iplusjminus( k+i+j );
  std::vector<int> iminusjplus( k+i+j );
  std::vector<int> iplusjplus( k+i+j );

  std::vector<int> iminus_r( k+i+j-1 );
  std::vector<int> iplus_r( k+i+j+1 );
  std::vector<int> jminus_r( k+i+j-1 );
  std::vector<int> jplus_r( k+i+j+1 );
  std::vector<int> iminusjminus_r( k+i+j-2 );
  std::vector<int> iplusjminus_r( k+i+j );
  std::vector<int> iminusjplus_r( k+i+j );
  std::vector<int> iplusjplus_r( k+i+j+2 );

  he.init_send_buffer<-1,-1>(&iminusjminus[0], MPI_INT, (k+i+j)*sizeof(int));
  he.init_send_buffer<-1, 1>(&iminusjplus[0], MPI_INT, (k+i+j)*sizeof(int));
  he.init_send_buffer< 1,-1>(&iplusjminus[0], MPI_INT, (k+i+j)*sizeof(int));
  he.init_send_buffer< 1, 1>(&iplusjplus[0], MPI_INT, (k+i+j)*sizeof(int));
  he.init_send_buffer<-1, 0>(&iminus[0], MPI_INT, (k+i+j)*sizeof(int));
  he.init_send_buffer< 1, 0>(&iplus[0], MPI_INT, (k+i+j)*sizeof(int));
  he.init_send_buffer< 0,-1>(&jminus[0], MPI_INT, (k+i+j)*sizeof(int));
  he.init_send_buffer< 0, 1>(&jplus[0], MPI_INT, (k+i+j)*sizeof(int));

  he.init_recv_buffer<-1,-1>(&iminusjminus_r[0], MPI_INT, (k+i+j-2)*sizeof(int));
  he.init_recv_buffer<-1, 1>(&iminusjplus_r[0], MPI_INT, (k+i+j)*sizeof(int));
  he.init_recv_buffer< 1,-1>(&iplusjminus_r[0], MPI_INT, (k+i+j)*sizeof(int));
  he.init_recv_buffer< 1, 1>(&iplusjplus_r[0], MPI_INT, (k+i+j+2)*sizeof(int));
  he.init_recv_buffer<-1, 0>(&iminus_r[0], MPI_INT, (k+i+j-1)*sizeof(int));
  he.init_recv_buffer< 1, 0>(&iplus_r[0], MPI_INT, (k+i+j+1)*sizeof(int));
  he.init_recv_buffer< 0,-1>(&jminus_r[0], MPI_INT, (k+i+j-1)*sizeof(int));
  he.init_recv_buffer< 0, 1>(&jplus_r[0], MPI_INT, (k+i+j+1)*sizeof(int));

  std::fill(&iminus[0], &iminus[k+i+j], pid);
  std::fill(&iplus[0], &iplus[k+i+j], pid);
  std::fill(&jminus[0], &jminus[k+i+j], pid);
  std::fill(&jplus[0], &jplus[k+i+j], pid);
  std::fill(&iminusjminus[0], &iminusjminus[k+i+j], pid);
  std::fill(&iplusjplus[0], &iplusjplus[k+i+j], pid);
  std::fill(&iplusjminus[0], &iplusjminus[k+i+j], pid);
  std::fill(&iminusjplus[0], &iminusjplus[k+i+j], pid);
  
  he.do_receives();
  he.do_sends();
  he.do_waits();
  
  std::vector<int> res_iminus_r( k+i+j-1 );
  std::vector<int> res_iplus_r( k+i+j+1 );
  std::vector<int> res_jminus_r( k+i+j-1 );
  std::vector<int> res_jplus_r( k+i+j+1 );
  std::vector<int> res_iminusjminus_r( k+i+j-2 );
  std::vector<int> res_iplusjminus_r( k+i+j );
  std::vector<int> res_iminusjplus_r( k+i+j );
  std::vector<int> res_iplusjplus_r( k+i+j+2 );

  std::fill(&res_iminus_r[0], &res_iminus_r[k+i+j-1], pg.proc<-1,0>());
  std::fill(&res_iplus_r[0], &res_iplus_r[k+i+j+1], pg.proc<1,0>());
  std::fill(&res_jminus_r[0], &res_jminus_r[k+i+j-1], pg.proc<0,-1>());
  std::fill(&res_jplus_r[0], &res_jplus_r[k+i+j+1], pg.proc<0,1>());
  std::fill(&res_iminusjminus_r[0], &res_iminusjminus_r[k+i+j-2], pg.proc<-1,-1>());
  std::fill(&res_iplusjplus_r[0], &res_iplusjplus_r[k+i+j+2], pg.proc<1,1>());
  std::fill(&res_iplusjminus_r[0], &res_iplusjminus_r[k+i+j], pg.proc<1,-1>());
  std::fill(&res_iminusjplus_r[0], &res_iminusjplus_r[k+i+j], pg.proc<-1,1>());

  int res = 1;
  
  if (i>0) {
    res &= std::equal(&iminus_r[0], &iminus_r[k+i+j-1], &res_iminus_r[0]);
  }
  if (i<N-1) {
    res &= std::equal(&iplus_r[0], &iplus_r[k+i+j+1], &res_iplus_r[0]);
  }
  if (j>0) {
    res &= std::equal(&jminus_r[0], &jminus_r[k+i+j-1], &res_jminus_r[0]);
  }
  if (j<M-1) {
    res &= std::equal(&jplus_r[0], &jplus_r[k+i+j+1], &res_jplus_r[0]);
  }
  if (i>0 && j>0) {
    res &= std::equal(&iminusjminus_r[0], &iminusjminus_r[k+i+j-2], &res_iminusjminus_r[0]);
  }
  if (i<N-1 && j>0) {
    res &= std::equal(&iplusjminus_r[0], &iplusjminus_r[k+i+j], &res_iplusjminus_r[0]);
  }
  if (i>0 && j<M-1) {
    res &= std::equal(&iminusjplus_r[0], &iminusjplus_r[k+i+j], &res_iminusjplus_r[0]);
  }
  if (i<N-1 && j<M-1) {
    res &= std::equal(&iplusjplus_r[0], &iplusjplus_r[k+i+j+2], &res_iplusjplus_r[0]);
  }

  /*if (res)
    file << "RES is TRUE\n";
  else
    file << "RES is FALSE\n";*/
  
  int final;
  MPI_Reduce(&res, &final, 1, MPI_INT, MPI_LAND, 0, CartComm);

  if (pid==0) {
    if (!final) {
      std::cout << "@" << pid << "@ FAILED!\n";
    } else
      std::cout << "@" << pid << "@ PASSED!\n";
  }
  
  MPI_Barrier(CartComm);
  
  MPI_Finalize();
  
  return 0;
}
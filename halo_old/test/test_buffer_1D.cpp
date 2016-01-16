#include <iostream>
#include <stdio.h>
#include <vector>
#include <mpi.h>
#include <Proc_Grid_1D.h>
#include <TransmitBuffer_1D.h>

int main(int argc, char** argv) {
  int pid, nprocs, dims[1] = {0}, period[1] = {0};
  int k = 16;
  int i, N;
  MPI_Comm CartComm;
  
  MPI_Init(&argc, &argv);
  
  MPI_Comm_rank(MPI_COMM_WORLD, &pid);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Dims_create(nprocs, 1, dims);
  MPI_Cart_create(MPI_COMM_WORLD, 1, dims, period, false, &CartComm);
  
  Proc_Grid_1D_t proc_grid(CartComm);
  
  TransmitBuffer_1D tb(&proc_grid);
  
  proc_grid.coords(i);
  proc_grid.dims(N);
  
  std::vector<int> iminus( k+i );
  std::vector<int> iplus( k+i );

  std::vector<int> iminus_r( k+i-1 );
  std::vector<int> iplus_r( k+i+1 );

  tb.init_send_buffer<-1>(&iminus[0], MPI_INT, (k+i));
  tb.init_send_buffer<1>(&iplus[0], MPI_INT, (k+i));

  tb.init_recv_buffer<-1>(&iminus_r[0], MPI_INT, (k+i-1));
  tb.init_recv_buffer<1>(&iplus_r[0], MPI_INT, (k+i+1));

  std::fill(&iminus[0], &iminus[k+i], pid);
  std::fill(&iplus[0], &iplus[k+i], pid);
  
  tb.do_receives();
  tb.do_sends();
  tb.do_waits();
  
  std::vector<int> res_iminus_r( k+i-1 );
  std::vector<int> res_iplus_r( k+i+1 );

  std::fill(&res_iminus_r[0], &res_iminus_r[k+i-1], proc_grid.proc<-1>());
  std::fill(&res_iplus_r[0], &res_iplus_r[k+i+1], proc_grid.proc<1>());

  int res = 1;
  
  if (i>0) {
    res &= std::equal(&iminus_r[0], &iminus_r[k+i-1], &res_iminus_r[0]);
  }
  if (i<N-1) {
    res &= std::equal(&iplus_r[0], &iplus_r[k+i+1], &res_iplus_r[0]);
  }
  
  int final;
  MPI_Reduce(&res, &final, 1, MPI_INT, MPI_LAND, 0, CartComm);

  if (pid==0) {
    if (!final) {
      std::cout << "**" << pid << "1D BUFFER TEST FAILED!\n";
    } else
      std::cout << "**" << pid << "1D BUFFER TEST PASSED!\n";
  }
  
  MPI_Barrier(CartComm);
  
  MPI_Finalize();
  
  return 0;
}
#include <iostream>
#include <stdio.h>
#include <vector>
#include <mpi.h>
#include <ProcGrid.h>
#include <TransmitBuffer.h>

int main(int argc, char** argv) {
  int pid, nprocs;
  int dims[3] = {0, 0, 0}, period[3] = {0, 0, 0};
  int t = 16;
  int i, j, k;
  int N, M, O;
  MPI_Comm CartComm;
  
  MPI_Init(&argc, &argv);
  
  MPI_Comm_rank(MPI_COMM_WORLD, &pid);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Dims_create(nprocs, 3, dims);
  MPI_Cart_create(MPI_COMM_WORLD, 3, dims, period, false, &CartComm);
  
  Proc_Grid_t<3> proc_grid(CartComm);
  
  TransmitBuffer<3, 26> tb(proc_grid);
  
  proc_grid.coords(i, j, k);
  proc_grid.dims(N, M, O);
  
  /*Data Vectors to be sent*/
  /*Direct Neighbors*/
  std::vector<int> kplus( t+i+j+k );
  std::vector<int> kminus ( t+i+j+k );
  std::vector<int> iplus( t+i+j+k );
  std::vector<int> iminus( t+i+j+k );
  std::vector<int> jplus( t+i+j+k );
  std::vector<int> jminus( t+i+j+k );
  /* Rest of the k=-1 plane */
  std::vector<int> iplusjpluskminus( t+i+j+k );
  std::vector<int> jpluskminus( t+i+j+k );
  std::vector<int> iminusjpluskminus( t+i+j+k );
  std::vector<int> ipluskminus( t+i+j+k );
  std::vector<int> iminuskminus( t+i+j+k );
  std::vector<int> iplusjminuskminus( t+i+j+k );
  std::vector<int> jminuskminus( t+i+j+k );
  std::vector<int> iminusjminuskminus( t+i+j+k );
  /* Rest of the k=0 plane */
  std::vector<int> iplusjplus( t+i+j+k );
  std::vector<int> iminusjplus( t+i+j+k );
  std::vector<int> iplusjminus( t+i+j+k );
  std::vector<int> iminusjminus( t+i+j+k );
  /* Rest of the k=1 plane */
  std::vector<int> iplusjpluskplus( t+i+j+k );
  std::vector<int> jpluskplus( t+i+j+k );
  std::vector<int> iminusjpluskplus( t+i+j+k );
  std::vector<int> ipluskplus( t+i+j+k );
  std::vector<int> iminuskplus( t+i+j+k );
  std::vector<int> iplusjminuskplus( t+i+j+k );
  std::vector<int> jminuskplus( t+i+j+k );
  std::vector<int> iminusjminuskplus( t+i+j+k );
  
  /*
  std::vector<int> iminus( t+i+j );
  std::vector<int> iplus( t+i+j );
  std::vector<int> jminus( t+i+j );
  std::vector<int> jplus( t+i+j );
  std::vector<int> iminusjminus( t+i+j );
  std::vector<int> iplusjminus( t+i+j );
  std::vector<int> iminusjplus( t+i+j );
  std::vector<int> iplusjplus( t+i+j );
  */

  /* Data structures to store the received data */
  /* Direct neighbours */
  std::vector<int> kplus_r( t+i+j+k );
  std::vector<int> kminus_r ( t+i+j+k );
  std::vector<int> iplus_r( t+i+j+k );
  std::vector<int> iminus_r( t+i+j+k );
  std::vector<int> jplus_r( t+i+j+k );
  std::vector<int> jminus_r( t+i+j+k );
  /* Rest of the k=-1 plane */
  std::vector<int> iplusjpluskminus_r( t+i+j+k );
  std::vector<int> jpluskminus_r( t+i+j+k );
  std::vector<int> iminusjpluskminus_r( t+i+j+k );
  std::vector<int> ipluskminus_r( t+i+j+k );
  std::vector<int> iminuskminus_r( t+i+j+k );
  std::vector<int> iplusjminuskminus_r( t+i+j+k );
  std::vector<int> jminuskminus_r( t+i+j+k );
  std::vector<int> iminusjminuskminus_r( t+i+j+k );
  /* Rest of the k=0 plane */
  std::vector<int> iplusjplus_r( t+i+j+k );
  std::vector<int> iminusjplus_r( t+i+j+k );
  std::vector<int> iplusjminus_r( t+i+j+k );
  std::vector<int> iminusjminus_r( t+i+j+k );
  /* Rest of the k=1 plane */
  std::vector<int> iplusjpluskplus_r( t+i+j+k );
  std::vector<int> jpluskplus_r( t+i+j+k );
  std::vector<int> iminusjpluskplus_r( t+i+j+k );
  std::vector<int> ipluskplus_r( t+i+j+k );
  std::vector<int> iminuskplus_r( t+i+j+k );
  std::vector<int> iplusjminuskplus_r( t+i+j+k );
  std::vector<int> jminuskplus_r( t+i+j+k );
  std::vector<int> iminusjminuskplus_r( t+i+j+k );
  
//   std::vector<int> iminus_r( t+i+j-1 );
//   std::vector<int> iplus_r( t+i+j+1 );
//   std::vector<int> jminus_r( t+i+j-1 );
//   std::vector<int> jplus_r( t+i+j+1 );
//   std::vector<int> iminusjminus_r( t+i+j-2 );
//   std::vector<int> iplusjminus_r( t+i+j );
//   std::vector<int> iminusjplus_r( t+i+j );
//   std::vector<int> iplusjplus_r( t+i+j+2 );
  
  /*Initializing send buffers */
  /*Direct neighbours */
  tb.init_send_buffer<0,0,1>(&kplus[0], MPI_INT, ( t+i+j+k ) );
  tb.init_send_buffer<0,0,-1>(&kminus[0], MPI_INT, ( t+i+j+k ) );
  tb.init_send_buffer<1,0,0>(&iplus[0], MPI_INT, ( t+i+j+k ) );
  tb.init_send_buffer<-1,0,0>(&iminus[0], MPI_INT, ( t+i+j+k ) );
  tb.init_send_buffer<0,1,0>(&jplus[0], MPI_INT, ( t+i+j+k ) );
  tb.init_send_buffer<0,-1,0>(&jminus[0], MPI_INT, ( t+i+j+k ) );
  
  /*Rest of the k=-1 plane */
  tb.init_send_buffer<1,1,-1>(&iplusjpluskminus[0], MPI_INT, ( t+i+j+k ));
  tb.init_send_buffer<0,1,-1>(&jpluskminus[0], MPI_INT, ( t+i+j+k ));
  tb.init_send_buffer<-1,1,-1>(&iminusjpluskminus[0], MPI_INT, ( t+i+j+k ));
  tb.init_send_buffer<1,0,-1>( &ipluskminus[0], MPI_INT, ( t+i+j+k ));
  tb.init_send_buffer<-1,0,-1>( &iminuskminus[0], MPI_INT, ( t+i+j+k ));
  tb.init_send_buffer<1,-1,-1>( &iplusjminuskminus[0], MPI_INT, ( t+i+j+k ));
  tb.init_send_buffer<0,-1,-1>( &jminuskminus[0], MPI_INT, ( t+i+j+k ));
  tb.init_send_buffer<-1,-1,-1>( &iminusjminuskminus[0], MPI_INT, ( t+i+j+k ));
  
  /*Rest of the k=0 plane */
  tb.init_send_buffer<1,1,0>(&iplusjplus[0], MPI_INT, ( t+i+j+k ));
  tb.init_send_buffer<-1,1,0>(&iminusjplus[0], MPI_INT, ( t+i+j+k ));
  tb.init_send_buffer<1,-1,0>(&iplusjminus[0], MPI_INT, ( t+i+j+k ));
  tb.init_send_buffer<-1,-1,0>( &iminusjminus[0], MPI_INT, ( t+i+j+k ));
  
  /*Rest of the k=1 plane */
  tb.init_send_buffer<1,1,1>(&iplusjpluskplus[0], MPI_INT, ( t+i+j+k ));
  tb.init_send_buffer<0,1,1>(&jpluskplus[0], MPI_INT, ( t+i+j+k ));
  tb.init_send_buffer<-1,1,1>(&iminusjpluskplus[0], MPI_INT, ( t+i+j+k ));
  tb.init_send_buffer<1,0,1>( &ipluskplus[0], MPI_INT, ( t+i+j+k ));
  tb.init_send_buffer<-1,0,1>( &iminuskplus[0], MPI_INT, ( t+i+j+k ));
  tb.init_send_buffer<1,-1,1>( &iplusjminuskplus[0], MPI_INT, ( t+i+j+k ));
  tb.init_send_buffer<0,-1,1>( &jminuskplus[0], MPI_INT, ( t+i+j+k ));
  tb.init_send_buffer<-1,-1,1>( &iminusjminuskplus[0], MPI_INT, ( t+i+j+k ));

//   tb.init_send_buffer<-1,-1>(&iminusjminus[0], MPI_INT, (t+i+j));
//   tb.init_send_buffer<-1, 1>(&iminusjplus[0], MPI_INT, (t+i+j));
//   tb.init_send_buffer< 1,-1>(&iplusjminus[0], MPI_INT, (t+i+j));
//   tb.init_send_buffer< 1, 1>(&iplusjplus[0], MPI_INT, (t+i+j));
//   tb.init_send_buffer<-1, 0>(&iminus[0], MPI_INT, (t+i+j));
//   tb.init_send_buffer< 1, 0>(&iplus[0], MPI_INT, (t+i+j));
//   tb.init_send_buffer< 0,-1>(&jminus[0], MPI_INT, (t+i+j));
//   tb.init_send_buffer< 0, 1>(&jplus[0], MPI_INT, (t+i+j));
  
  /* Initializing receive buffers */
    /*Direct neighbours */
  tb.init_recv_buffer<	0,	0,	1>	(&kplus_r[0], 	MPI_INT, ( t+i+j+k ) );
  tb.init_recv_buffer<	0,	0,	-1>	(&kminus_r[0],	MPI_INT, ( t+i+j+k ) );
  tb.init_recv_buffer<	1,	0,	0>	(&iplus_r[0], 	MPI_INT, ( t+i+j+k ) );
  tb.init_recv_buffer<	-1,	0,	0>	(&iminus_r[0], 	MPI_INT, ( t+i+j+k ) );
  tb.init_recv_buffer<	0,	1,	0>	(&jplus_r[0], 	MPI_INT, ( t+i+j+k ) );
  tb.init_recv_buffer<	0,	-1,	0>	(&jminus_r[0], 	MPI_INT, ( t+i+j+k ) );
  
  /*Rest of the k=-1 plane */
  tb.init_recv_buffer<1,1,-1>(&iplusjpluskminus_r[0], MPI_INT, ( t+i+j+k ));
  tb.init_recv_buffer<0,1,-1>(&jpluskminus_r[0], MPI_INT, ( t+i+j+k ));
  tb.init_recv_buffer<-1,1,-1>(&iminusjpluskminus_r[0], MPI_INT, ( t+i+j+k ));
  tb.init_recv_buffer<1,0,-1>( &ipluskminus_r[0], MPI_INT, ( t+i+j+k ));
  tb.init_recv_buffer<-1,0,-1>( &iminuskminus_r[0], MPI_INT, ( t+i+j+k ));
  tb.init_recv_buffer<1,-1,-1>( &iplusjminuskminus_r[0], MPI_INT, ( t+i+j+k ));
  tb.init_recv_buffer<0,-1,-1>( &jminuskminus_r[0], MPI_INT, ( t+i+j+k ));
  tb.init_recv_buffer<-1,-1,-1>( &iminusjminuskminus_r[0], MPI_INT, ( t+i+j+k ));
  
  /*Rest of the k=0 plane */
  tb.init_recv_buffer<1,1,0>(&iplusjplus_r[0], MPI_INT, ( t+i+j+k ));
  tb.init_recv_buffer<-1,1,0>(&iminusjplus_r[0], MPI_INT, ( t+i+j+k ));
  tb.init_recv_buffer<1,-1,0>(&iplusjminus_r[0], MPI_INT, ( t+i+j+k ));
  tb.init_recv_buffer<-1,-1,0>( &iminusjminus_r[0], MPI_INT, ( t+i+j+k ));
  
  /*Rest of the k=1 plane */
  tb.init_recv_buffer<1,1,1>(&iplusjpluskplus_r[0], MPI_INT, ( t+i+j+k ));
  tb.init_recv_buffer<0,1,1>(&jpluskplus_r[0], MPI_INT, ( t+i+j+k ));
  tb.init_recv_buffer<-1,1,1>(&iminusjpluskplus_r[0], MPI_INT, ( t+i+j+k ));
  tb.init_recv_buffer<1,0,1>( &ipluskplus_r[0], MPI_INT, ( t+i+j+k ));
  tb.init_recv_buffer<-1,0,1>( &iminuskplus_r[0], MPI_INT, ( t+i+j+k ));
  tb.init_recv_buffer<1,-1,1>( &iplusjminuskplus_r[0], MPI_INT, ( t+i+j+k ));
  tb.init_recv_buffer<0,-1,1>( &jminuskplus_r[0], MPI_INT, ( t+i+j+k ));
  tb.init_recv_buffer<-1,-1,1>( &iminusjminuskplus_r[0], MPI_INT, ( t+i+j+k ));

  
  
  
  
//   tb.init_recv_buffer<-1,-1>(&iminusjminus_r[0], MPI_INT, (t+i+j-2));
//   tb.init_recv_buffer<-1, 1>(&iminusjplus_r[0], MPI_INT, (t+i+j));
//   tb.init_recv_buffer< 1,-1>(&iplusjminus_r[0], MPI_INT, (t+i+j));
//   tb.init_recv_buffer< 1, 1>(&iplusjplus_r[0], MPI_INT, (t+i+j+2));
//   tb.init_recv_buffer<-1, 0>(&iminus_r[0], MPI_INT, (t+i+j-1));
//   tb.init_recv_buffer< 1, 0>(&iplus_r[0], MPI_INT, (t+i+j+1));
//   tb.init_recv_buffer< 0,-1>(&jminus_r[0], MPI_INT, (t+i+j-1));
//   tb.init_recv_buffer< 0, 1>(&jplus_r[0], MPI_INT, (t+i+j+1));
  
  
  /* Fill the send buffers with the unique process id */
    /* Direct neighbours */
std::fill(&kplus[0], &kplus[ t+i+j+k ], pid );
std::fill(&kminus[0], &kminus[t+i+j+k], pid );
std::fill(&iplus[0], &iplus[t+i+j+k], pid );
std::fill(&iminus[0], &iminus[t+i+j+k], pid );
std::fill(&jplus[0], &jplus[t+i+j+k], pid );
std::fill(&jminus[0], &jminus[t+i+j+k], pid );
  /* Rest of the k=-1 plane */
std::fill(&iplusjpluskminus[0], &iplusjpluskminus[t+i+j+k], pid);
std::fill(&jpluskminus[0], &jpluskminus[t+i+j+k], pid);
std::fill(&iminusjpluskminus[0], &iminusjpluskminus[t+i+j+k], pid);
std::fill(&ipluskminus[0], &ipluskminus[t+i+j+k], pid);
std::fill(&iminuskminus[0], &iminuskminus[t+i+j+k], pid);
std::fill(&iplusjminuskminus[0], &iplusjminuskminus[t+i+j+k], pid);
std::fill(&jminuskminus[0], &jminuskminus[t+i+j+k], pid);
std::fill(&iminusjminuskminus[0], &iminusjminuskminus[t+i+j+k], pid);
  /* Rest of the k=0 plane */
std::fill(&iplusjplus[0], &iplusjplus[t+i+j+k], pid);
std::fill(&iminusjplus[0], &iminusjplus[t+i+j+k], pid);
std::fill(&iplusjminus[0], &iplusjminus[t+i+j+k], pid);
std::fill(&iminusjminus[0], &iminusjminus[t+i+j+k], pid);
  /* Rest of the k=1 plane */
std::fill(&iplusjpluskplus[0], &iplusjpluskplus[t+i+j+k], pid);
std::fill(&jpluskplus[0], &jpluskplus[t+i+j+k], pid);
std::fill(&iminusjpluskplus[0], &iminusjpluskplus[t+i+j+k], pid);
std::fill(&ipluskplus[0], &ipluskplus[t+i+j+k], pid);
std::fill(&iminuskplus[0], &iminuskplus[t+i+j+k], pid);
std::fill(&iplusjminuskplus[0], &iplusjminuskplus[t+i+j+k], pid);
std::fill(&jminuskplus[0], &jminuskplus[t+i+j+k], pid);
std::fill(&iminusjminuskplus[0], &iminusjminuskplus[t+i+j+k], pid);
  
  tb.do_receives();
  tb.do_sends();
  tb.do_waits();
  
  
  /* Data to reference the received data */
  /* Direct neighbours */
  std::vector<int> res_kplus_r( t+i+j+k );
  std::vector<int> res_kminus_r ( t+i+j+k );
  std::vector<int> res_iplus_r( t+i+j+k );
  std::vector<int> res_iminus_r( t+i+j+k );
  std::vector<int> res_jplus_r( t+i+j+k );
  std::vector<int> res_jminus_r( t+i+j+k );
  /* Rest of the k=-1 plane */
  std::vector<int> res_iplusjpluskminus_r( t+i+j+k );
  std::vector<int> res_jpluskminus_r( t+i+j+k );
  std::vector<int> res_iminusjpluskminus_r( t+i+j+k );
  std::vector<int> res_ipluskminus_r( t+i+j+k );
  std::vector<int> res_iminuskminus_r( t+i+j+k );
  std::vector<int> res_iplusjminuskminus_r( t+i+j+k );
  std::vector<int> res_jminuskminus_r( t+i+j+k );
  std::vector<int> res_iminusjminuskminus_r( t+i+j+k );
  /* Rest of the k=0 plane */
  std::vector<int> res_iplusjplus_r( t+i+j+k );
  std::vector<int> res_iminusjplus_r( t+i+j+k );
  std::vector<int> res_iplusjminus_r( t+i+j+k );
  std::vector<int> res_iminusjminus_r( t+i+j+k );
  /* Rest of the k=1 plane */
  std::vector<int> res_iplusjpluskplus_r( t+i+j+k );
  std::vector<int> res_jpluskplus_r( t+i+j+k );
  std::vector<int> res_iminusjpluskplus_r( t+i+j+k );
  std::vector<int> res_ipluskplus_r( t+i+j+k );
  std::vector<int> res_iminuskplus_r( t+i+j+k );
  std::vector<int> res_iplusjminuskplus_r( t+i+j+k );
  std::vector<int> res_jminuskplus_r( t+i+j+k );
  std::vector<int> res_iminusjminuskplus_r( t+i+j+k );
  
//   std::vector<int> res_iminus_r( t+i+j-1 );
//   std::vector<int> res_iplus_r( t+i+j+1 );
//   std::vector<int> res_jminus_r( t+i+j-1 );
//   std::vector<int> res_jplus_r( t+i+j+1 );
//   std::vector<int> res_iminusjminus_r( t+i+j-2 );
//   std::vector<int> res_iplusjminus_r( t+i+j );
//   std::vector<int> res_iminusjplus_r( t+i+j );
//   std::vector<int> res_iplusjplus_r( t+i+j+2 );
  
    /* Fill the reference receive buffers with the expected process id */
    /* Direct neighbours */
std::fill(&res_kplus_r[0], &res_kplus_r[ t+i+j+k ], proc_grid.proc<0,0,1 >() );
std::fill(&res_kminus_r[0], &res_kminus_r[t+i+j+k], proc_grid.proc<0,0,-1 >() );
std::fill(&res_iplus_r[0], &res_iplus_r[t+i+j+k], proc_grid.proc<1,0,0 >() );
std::fill(&res_iminus_r[0], &res_iminus_r[t+i+j+k], proc_grid.proc<-1,0,0 >() );
std::fill(&res_jplus_r[0], &res_jplus_r[t+i+j+k], proc_grid.proc< 0,1,0>() );
std::fill(&res_jminus_r[0], &res_jminus_r[t+i+j+k], proc_grid.proc<0,-1,0 >() );
  /* Rest of the k=-1 plane */
std::fill(&res_iplusjpluskminus_r[0], &res_iplusjpluskminus_r[t+i+j+k], proc_grid.proc< 1,1,-1>());
std::fill(&res_jpluskminus_r[0], &res_jpluskminus_r[t+i+j+k], proc_grid.proc< 0,1,-1>());
std::fill(&res_iminusjpluskminus_r[0], &res_iminusjpluskminus_r[t+i+j+k], proc_grid.proc<-1,1,-1 >());
std::fill(&res_ipluskminus_r[0], &res_ipluskminus_r[t+i+j+k], proc_grid.proc< 1,0,-1>());
std::fill(&res_iminuskminus_r[0], &res_iminuskminus_r[t+i+j+k], proc_grid.proc< -1,0,-1>());
std::fill(&res_iplusjminuskminus_r[0], &res_iplusjminuskminus_r[t+i+j+k], proc_grid.proc< 1,-1,-1>());
std::fill(&res_jminuskminus_r[0], &res_jminuskminus_r[t+i+j+k], proc_grid.proc< 0,-1,-1>());
std::fill(&res_iminusjminuskminus_r[0], &res_iminusjminuskminus_r[t+i+j+k], proc_grid.proc< -1,-1,-1>());
  /* Rest of the k=0 plane */
std::fill(&res_iplusjplus_r[0], &res_iplusjplus_r[t+i+j+k], proc_grid.proc<1,1,0 >());
std::fill(&res_iminusjplus_r[0], &res_iminusjplus_r[t+i+j+k], proc_grid.proc<-1,1,0 >());
std::fill(&res_iplusjminus_r[0], &res_iplusjminus_r[t+i+j+k], proc_grid.proc< 1,-1,0>());
std::fill(&res_iminusjminus_r[0], &res_iminusjminus_r[t+i+j+k], proc_grid.proc<-1,-1,0 >());
  /* Rest of the k=1 plane */
std::fill(&res_iplusjpluskplus_r[0], &res_iplusjpluskplus_r[t+i+j+k], proc_grid.proc< 1,1,1>());
std::fill(&res_jpluskplus_r[0], &res_jpluskplus_r[t+i+j+k], proc_grid.proc<0,1,1 >());
std::fill(&res_iminusjpluskplus_r[0], &res_iminusjpluskplus_r[t+i+j+k], proc_grid.proc< -1,1,1>());
std::fill(&res_ipluskplus_r[0], &res_ipluskplus_r[t+i+j+k], proc_grid.proc<1,0,1 >());
std::fill(&res_iminuskplus_r[0], &res_iminuskplus_r[t+i+j+k], proc_grid.proc<-1,0,1 >());
std::fill(&res_iplusjminuskplus_r[0], &res_iplusjminuskplus_r[t+i+j+k], proc_grid.proc<1,-1,1 >());
std::fill(&res_jminuskplus_r[0], &res_jminuskplus_r[t+i+j+k], proc_grid.proc<0,-1,1 >());
std::fill(&res_iminusjminuskplus_r[0], &res_iminusjminuskplus_r[t+i+j+k], proc_grid.proc<-1,-1,1 >());

//   std::fill(&res_iminus_r[0], &res_iminus_r[t+i+j-1], proc_grid.proc<-1,0>());
//   std::fill(&res_iplus_r[0], &res_iplus_r[t+i+j+1], proc_grid.proc<1,0>());
//   std::fill(&res_jminus_r[0], &res_jminus_r[t+i+j-1], proc_grid.proc<0,-1>());
//   std::fill(&res_jplus_r[0], &res_jplus_r[t+i+j+1], proc_grid.proc<0,1>());
//   std::fill(&res_iminusjminus_r[0], &res_iminusjminus_r[t+i+j-2], proc_grid.proc<-1,-1>());
//   std::fill(&res_iplusjplus_r[0], &res_iplusjplus_r[t+i+j+2], proc_grid.proc<1,1>());
//   std::fill(&res_iplusjminus_r[0], &res_iplusjminus_r[t+i+j], proc_grid.proc<1,-1>());
//   std::fill(&res_iminusjplus_r[0], &res_iminusjplus_r[t+i+j], proc_grid.proc<-1,1>());

  int res = 1;
  
  if (i>0) {
    res &= std::equal(&iminus_r[0], &iminus_r[t+i+j+k], &res_iminus_r[0]);
  }
  if (i<N-1) {
    res &= std::equal(&iplus_r[0], &iplus_r[t+i+j+k], &res_iplus_r[0]);
  }
  if (j>0) {
    res &= std::equal(&jminus_r[0], &jminus_r[t+i+j+k], &res_jminus_r[0]);
  }
  if (j<M-1) {
    res &= std::equal(&jplus_r[0], &jplus_r[t+i+j+k], &res_jplus_r[0]);
  }
  if (k>0) {
    res &= std::equal(&kplus_r[0], &kplus_r[t+i+j+k], &res_kplus_r[0]);
  }
  if (k <O-1) {
    res &= std::equal(&kminus_r[0], &kminus_r[t+i+j+k], &res_kminus_r[0]);
  }
  if (i>0 && j>0) {
    res &= std::equal(&iminusjminus_r[0], &iminusjminus_r[t+i+j+k], &res_iminusjminus_r[0]);
  }
  if (i<N-1 && j>0) {
    res &= std::equal(&iplusjminus_r[0], &iplusjminus_r[t+i+j+k], &res_iplusjminus_r[0]);
  }
  if (i>0 && j<M-1) {
    res &= std::equal(&iminusjplus_r[0], &iminusjplus_r[t+i+j+k], &res_iminusjplus_r[0]);
  }
  if (i<N-1 && j<M-1) {
    res &= std::equal(&iplusjplus_r[0], &iplusjplus_r[t+i+j+k], &res_iplusjplus_r[0]);
  }
  if (i > 0 && k > 0) {
    res &= std::equal(&iminuskminus_r[0], &iminuskminus_r[t+i+j+k], &res_iminuskminus_r[0]);
  }
  if (i<N-1 && k > 0) {
    res &= std::equal(&ipluskminus_r[0], &ipluskminus_r[t+i+j+k], &res_ipluskminus_r[0]);
  }
  if (i > 0 && k < O-1) {
    res &= std::equal(&iminuskplus_r[0], &iminuskplus_r[t+i+j+k], &res_iminuskplus_r[0]);
  }
  if(i < N-1 && k < O-1) {
    res &= std::equal(&ipluskplus_r[0], &ipluskplus_r[t+i+j+k], &res_ipluskplus_r[0]);
  }
  if (j > 0 && k > 0) {
    res &= std::equal(&jminuskminus_r[0], &jminuskminus_r[t+i+j+k], &res_iminuskminus_r[0]);
  }
  if (j < M-1 && k > 0) {
    res &= std::equal(&jpluskminus_r[0], &jpluskminus_r[t+i+j+k], &res_jpluskminus_r[0]);
  }
  if(j > 0 && k < O-1) {
    res &= std::equal(&jminuskplus_r[0], &jminuskplus_r[t+i+j+k], &res_jminuskplus_r[0]);
  }
  if(j < M-1 && k < O-1) {
    res &= std::equal(&jpluskplus_r[0], &jpluskplus_r[t+i+j+k], &res_jpluskplus_r[0]);
  }
  if(i > 0 && j > 0 && k > 0) {
    res &= std::equal(&iminusjminuskminus_r[0], &iminusjminuskminus_r[t+i+j+k], &res_iminusjminuskminus_r[0]);
  }
  if(i < N-1 && j > 0 && k > 0) {
    res &= std::equal(&iplusjminuskminus_r[0], &iplusjminuskminus_r[t+i+j+k], &res_iplusjminuskminus_r[0]);
  }
  if(i > 0 && j < M-1 && k > 0) {
    res &= std::equal(&iminusjpluskminus_r[0], &iminusjpluskminus_r[t+i+j+k], &res_iminusjpluskminus_r[0]);
  }
  if(i < N-1 && j < M-1 && k > 0) {
    res &= std::equal(&iplusjpluskminus_r[0], &iplusjpluskminus_r[t+i+j+k], &res_iplusjpluskminus_r[0]);
  }
  if(i > 0 && j > 0 && k < O-1) {
    res &= std::equal(&iminusjminuskplus_r[0], &iminusjminuskplus_r[t+i+j+k], &res_iminusjminuskplus_r[0]);
  }
  if(i < N-1 && j > 0 && k < O-1) {
    res &= std::equal(&iplusjminuskplus_r[0], &iplusjminuskplus_r[t+i+j+k], &res_iplusjminuskplus_r[0]);
  }
  if(i > 0 && j < M-1 && k < O-1) {
    res &= std::equal(&iminusjpluskplus_r[0], &iminusjpluskplus_r[t+i+j+k], &res_iminusjpluskplus_r[0]);
  }
  if(i < N-1 && j < M-1 && k < O-1) {
    res &= std::equal(&iplusjpluskplus_r[0], &iplusjpluskplus_r[t+i+j+k], &res_iplusjpluskplus_r[0]);
  }
  
  int final;
  MPI_Reduce(&res, &final, 1, MPI_INT, MPI_LAND, 0, CartComm);

  if (pid==0) {
    if (!final) {
      std::cout << "**" << pid << " 3D BUFFER TEST FAILED!\n";
    } else
      std::cout << "**" << pid << " 3D BUFFER TEST PASSED!\n";
  }
  
  MPI_Barrier(CartComm);
  
  MPI_Finalize();
  
  return 0;
}
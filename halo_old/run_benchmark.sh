#!/bin/bash

if [ ! -e ./bin/benchmark ]
then
  echo "[INFO] Building Benchmark"
  make benchmark
fi

if [ -e ./bin/benchmark ]
then
  echo "[INFO] Starting Benchmark for 2 Nodes and 8 Threads."
  mpirun -host creek04,creek05 -np 8 ./bin/benchmark
fi

#if [ -e ./bin/benchmark ]
#then
  echo "[INFO] Plotting Benchmark"
  gnuplot benchmark.pl
#fi

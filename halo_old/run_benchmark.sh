#!/bin/bash

if [ ! -e ./bin/benchmark_1D ]
then
  echo "[INFO] Building 1D Benchmark"
  make benchmark_1D
fi

if [ -e ./bin/benchmark_1D ]
then
  echo "[INFO] Starting 1D Benchmark for 2 Nodes and 8 Threads."
  mpirun -host creek04,creek05 -np 8 ./bin/benchmark_1D
fi

if [ -e ./bin/benchmark_1D ]
then
  echo "[INFO] Starting 1D Benchmark for 4 Nodes and 16 Threads."
  mpirun -host creek02,creek04,creek05,creek06 -np 16 ./bin/benchmark_1D
fi

if [ -e ./bin/benchmark_1D ]
then
  echo "[INFO] Starting 1D Benchmark for 5 Nodes and 20 Threads."
  mpirun -host creek01,creek02,creek04,creek05,creek06 -np 20 ./bin/benchmark_1D
fi

if [ ! -e ./bin/benchmark_2D ]
then
  echo "[INFO] Building 2D Benchmark"
  make benchmark_2D
fi

if [ -e ./bin/benchmark_2D ]
then
  echo "[INFO] Starting 2D Benchmark for 2 Nodes and 8 Threads."
  mpirun -host creek04,creek05 -np 8 ./bin/benchmark_2D
fi

if [ -e ./bin/benchmark_2D ]
then
  echo "[INFO] Starting 2D Benchmark for 4 Nodes and 16 Threads."
  mpirun -host creek02,creek04,creek05,creek06 -np 16 ./bin/benchmark_2D
fi

if [ -e ./bin/benchmark_2D ]
then
  echo "[INFO] Starting 2D Benchmark for 5 Nodes and 20 Threads."
  mpirun -host creek01,creek02,creek04,creek05,creek06 -np 20 ./bin/benchmark_2D
fi

if [ ! -e ./bin/benchmark_3D ]
then
  echo "[INFO] Building 3D Benchmark"
  make benchmark_3D
fi

if [ -e ./bin/benchmark_3D ]
then
  echo "[INFO] Starting 3D Benchmark for 2 Nodes and 8 Threads."
  mpirun -host creek04,creek05 -np 8 ./bin/benchmark_3D
fi

if [ -e ./bin/benchmark_3D ]
then
  echo "[INFO] Starting 3D Benchmark for 4 Nodes and 16 Threads."
  mpirun -host creek02,creek04,creek05,creek06 -np 16 ./bin/benchmark_3D
fi

if [ -e ./bin/benchmark_3D ]
then
  echo "[INFO] Starting 3D Benchmark for 5 Nodes and 20 Threads."
  mpirun -host creek01,creek02,creek04,creek05,creek06 -np 20 ./bin/benchmark_3D
fi

echo "[INFO] Benchmark finished."

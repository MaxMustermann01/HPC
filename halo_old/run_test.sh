#!/bin/bash

if [ ! -e ./bin/test_buffer_1D ]
then
  echo "[INFO] Building 1D Buffer Test."
  make test_buffer_1D
fi

if [ -e ./bin/test_buffer_1D ]
then
  echo "[INFO] Start 1D Buffer Test."
  mpirun -host creek02,creek04,creek05,creek06 -np 16 ./bin/test_buffer_1D
fi

if [ ! -e ./bin/test_buffer_2D ]
then
  echo "[INFO] Building 2D Buffer Test."
  make test_buffer_2D
fi

if [ -e ./bin/test_buffer_2D ]
then
  echo "[INFO] Start 2D Buffer Test."
  mpirun -host creek02,creek04,creek05,creek06 -np 16 ./bin/test_buffer_2D
fi

if [ ! -e ./bin/test_buffer_3D ]
then
  echo "[INFO] Building 3D Buffer Test."
  make test_buffer_3D
fi

if [ -e ./bin/test_buffer_3D ]
then
  echo "[INFO] Start 3D Buffer Test."
  mpirun -host creek02,creek04,creek05,creek06 -np 16 ./bin/test_buffer_3D
fi
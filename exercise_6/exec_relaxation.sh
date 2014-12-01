#!/usr/bin/env bash

if [ $# -ne 4 ]
then
  echo "Check Usage: sbatch relaxation <NUM_PROCESSES> <GRID_SIZE> <NUM_ITERATIONS> <OPTION>"
  echo "<OPTION> - (1) Print Matrix, (0) Don't Print Matrix"
  exit 1
fi

#SBATCH -o job-output

mpirun -host creek01,creek02,creek03,creek04,creek05,creek06,creek07,creek08 -np $1 ./relaxation $2 $3 $4

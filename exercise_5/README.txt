
I. File List
------------
Makefile
src/main.c
src/matrix.c
src/matrix.h
src/time_measurement.c
src/time_measurement.h

II. Compilation
---------------
In order to compile, use the Makefile:

$ make relaxation

To delete the compiled files:

$ make clean

III. Usage
----------
Run the multiplication: 

$ relaxation <MATRIX_SIZE> <ITERATIONS> <OPTION>

MATRIX_SIZE	Size of the matrix.
NUM_ITERATIONS	How many iterations should be made.
MODE		Should the matrix be printed or not.
		  (1) print, (0) don't print

(Output:

  Matrix size    : --
  Elapsed Time   : --s 
  Flops          : -- 
  GFLOPS/s       : -- 
 )
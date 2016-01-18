#CC = mpicc
CPP = mpic++
CFLAGS = -Wall -Wextra -std=c++11

test_buffer_2D :
	$(CPP) $(CFLAGS) -o test_buffer_2D test/test_buffer_2D.cpp -I./inc/

test_buffer_1D :
	$(CPP) $(CFLAGS) -o test_buffer_1D test/test_buffer_1D.cpp -I./inc/

stencil :
	$(CPP) $(CFLAGS) -o stencil example/stencil.cpp -I./inc/

clean :
	rm -f 	test_buffer_2D test_buffer_1D stencil
#CC = mpicc
CPP = mpic++
CFLAGS = -Wall -Wextra -std=c++11

test_buffer_2D :
	$(CPP) $(CFLAGS) -o test_buffer_2D test/test_buffer_2D.cpp -I./inc/

test_halo_2D :
	$(CPP) $(CFLAGS) -o test_halo_2D test/test_halo_2D.cpp -I./inc/
clean :
	rm -f 	test_buffer_2D test_halo_2D
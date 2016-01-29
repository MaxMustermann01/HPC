#CC = mpicc
CPP = mpic++
CFLAGS = -Wall -Wextra -std=c++11

benchmark_1D :
	$(CPP) $(CFLAGS) -o bin/benchmark_1D benchmark/benchmark_1D.cpp -I./inc/

benchmark_2D :
	$(CPP) $(CFLAGS) -o bin/benchmark_2D benchmark/benchmark_2D.cpp -I./inc/

benchmark_3D :
	$(CPP) $(CFLAGS) -o bin/benchmark_3D benchmark/benchmark_3D.cpp -I./inc/

test_buffer_1D :
	$(CPP) $(CFLAGS) -o bin/test_buffer_1D test/test_buffer_1D.cpp -I./inc/ 

test_buffer_2D :
	$(CPP) $(CFLAGS) -o bin/test_buffer_2D test/test_buffer_2D.cpp -I./inc/

test_buffer_3D :
	$(CPP) $(CFLAGS) -o bin/test_buffer_3D test/test_buffer_3D.cpp -I./inc/

stencil :
	$(CPP) $(CFLAGS) -o bin/stencil example/stencil.cpp -I./inc/

clean :
	rm -f 	bin/benchmark_3D bin/benchmark_2D bin/benchmark_1D bin/test_buffer_3D bin/test_buffer_2D bin/test_buffer_1D bin/stencil
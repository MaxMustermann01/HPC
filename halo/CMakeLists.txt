cmake_minimum_required(VERSION 2.8)

project(helo_exchange)

# Project depends on MPI
find_package(MPI REQUIRED)

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11")

# Output directory is ./bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
file(MAKE_DIRECTORY bin)

# Include header files
include_directories(SYSTEM ${MPI_INCLUDE_PATH})
include_directories(inc)

# Helo test files
add_executable(test_buffer_2D test/test_buffer_2D.cpp)
add_executable(test_buffer_3D test/test_buffer_3D.cpp)

# Link against MPI libraries
target_link_libraries(test_buffer_2D ${MPI_C_LIBRARIES})
target_link_libraries(test_buffer_3D ${MPI_C_LIBRARIES})

# Install binaries in ./bin
install(TARGETS test_buffer_3D test_buffer_2D DESTINATION bin)
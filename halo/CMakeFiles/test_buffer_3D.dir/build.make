# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/max/Studium/WS15_16/C++-Practice/domain-decomposition-with-halos

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/max/Studium/WS15_16/C++-Practice/domain-decomposition-with-halos

# Include any dependencies generated for this target.
include CMakeFiles/test_buffer_3D.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/test_buffer_3D.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test_buffer_3D.dir/flags.make

CMakeFiles/test_buffer_3D.dir/test/test_buffer_3D.cpp.o: CMakeFiles/test_buffer_3D.dir/flags.make
CMakeFiles/test_buffer_3D.dir/test/test_buffer_3D.cpp.o: test/test_buffer_3D.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/max/Studium/WS15_16/C++-Practice/domain-decomposition-with-halos/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/test_buffer_3D.dir/test/test_buffer_3D.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/test_buffer_3D.dir/test/test_buffer_3D.cpp.o -c /home/max/Studium/WS15_16/C++-Practice/domain-decomposition-with-halos/test/test_buffer_3D.cpp

CMakeFiles/test_buffer_3D.dir/test/test_buffer_3D.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_buffer_3D.dir/test/test_buffer_3D.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/max/Studium/WS15_16/C++-Practice/domain-decomposition-with-halos/test/test_buffer_3D.cpp > CMakeFiles/test_buffer_3D.dir/test/test_buffer_3D.cpp.i

CMakeFiles/test_buffer_3D.dir/test/test_buffer_3D.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_buffer_3D.dir/test/test_buffer_3D.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/max/Studium/WS15_16/C++-Practice/domain-decomposition-with-halos/test/test_buffer_3D.cpp -o CMakeFiles/test_buffer_3D.dir/test/test_buffer_3D.cpp.s

CMakeFiles/test_buffer_3D.dir/test/test_buffer_3D.cpp.o.requires:
.PHONY : CMakeFiles/test_buffer_3D.dir/test/test_buffer_3D.cpp.o.requires

CMakeFiles/test_buffer_3D.dir/test/test_buffer_3D.cpp.o.provides: CMakeFiles/test_buffer_3D.dir/test/test_buffer_3D.cpp.o.requires
	$(MAKE) -f CMakeFiles/test_buffer_3D.dir/build.make CMakeFiles/test_buffer_3D.dir/test/test_buffer_3D.cpp.o.provides.build
.PHONY : CMakeFiles/test_buffer_3D.dir/test/test_buffer_3D.cpp.o.provides

CMakeFiles/test_buffer_3D.dir/test/test_buffer_3D.cpp.o.provides.build: CMakeFiles/test_buffer_3D.dir/test/test_buffer_3D.cpp.o

# Object files for target test_buffer_3D
test_buffer_3D_OBJECTS = \
"CMakeFiles/test_buffer_3D.dir/test/test_buffer_3D.cpp.o"

# External object files for target test_buffer_3D
test_buffer_3D_EXTERNAL_OBJECTS =

bin/test_buffer_3D: CMakeFiles/test_buffer_3D.dir/test/test_buffer_3D.cpp.o
bin/test_buffer_3D: CMakeFiles/test_buffer_3D.dir/build.make
bin/test_buffer_3D: /usr/lib/x86_64-linux-gnu/libmpich.so
bin/test_buffer_3D: /usr/lib/x86_64-linux-gnu/libopa.so
bin/test_buffer_3D: /usr/lib/x86_64-linux-gnu/libmpl.so
bin/test_buffer_3D: /usr/lib/x86_64-linux-gnu/librt.so
bin/test_buffer_3D: /usr/lib/libcr.so
bin/test_buffer_3D: /usr/lib/x86_64-linux-gnu/libpthread.so
bin/test_buffer_3D: CMakeFiles/test_buffer_3D.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable bin/test_buffer_3D"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_buffer_3D.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test_buffer_3D.dir/build: bin/test_buffer_3D
.PHONY : CMakeFiles/test_buffer_3D.dir/build

CMakeFiles/test_buffer_3D.dir/requires: CMakeFiles/test_buffer_3D.dir/test/test_buffer_3D.cpp.o.requires
.PHONY : CMakeFiles/test_buffer_3D.dir/requires

CMakeFiles/test_buffer_3D.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test_buffer_3D.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test_buffer_3D.dir/clean

CMakeFiles/test_buffer_3D.dir/depend:
	cd /home/max/Studium/WS15_16/C++-Practice/domain-decomposition-with-halos && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/max/Studium/WS15_16/C++-Practice/domain-decomposition-with-halos /home/max/Studium/WS15_16/C++-Practice/domain-decomposition-with-halos /home/max/Studium/WS15_16/C++-Practice/domain-decomposition-with-halos /home/max/Studium/WS15_16/C++-Practice/domain-decomposition-with-halos /home/max/Studium/WS15_16/C++-Practice/domain-decomposition-with-halos/CMakeFiles/test_buffer_3D.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test_buffer_3D.dir/depend


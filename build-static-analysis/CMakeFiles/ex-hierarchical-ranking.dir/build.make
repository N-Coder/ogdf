# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_COMMAND = /cmake/bin/cmake

# The command to remove a file.
RM = /cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /__w/ogdf/ogdf

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /__w/ogdf/ogdf/build-static-analysis

# Include any dependencies generated for this target.
include CMakeFiles/ex-hierarchical-ranking.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/ex-hierarchical-ranking.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ex-hierarchical-ranking.dir/flags.make

CMakeFiles/ex-hierarchical-ranking.dir/doc/examples/layout/hierarchical-ranking.cpp.o: CMakeFiles/ex-hierarchical-ranking.dir/flags.make
CMakeFiles/ex-hierarchical-ranking.dir/doc/examples/layout/hierarchical-ranking.cpp.o: ../doc/examples/layout/hierarchical-ranking.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/__w/ogdf/ogdf/build-static-analysis/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/ex-hierarchical-ranking.dir/doc/examples/layout/hierarchical-ranking.cpp.o"
	ccache /usr/bin/clang++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ex-hierarchical-ranking.dir/doc/examples/layout/hierarchical-ranking.cpp.o -c /__w/ogdf/ogdf/doc/examples/layout/hierarchical-ranking.cpp

CMakeFiles/ex-hierarchical-ranking.dir/doc/examples/layout/hierarchical-ranking.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ex-hierarchical-ranking.dir/doc/examples/layout/hierarchical-ranking.cpp.i"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /__w/ogdf/ogdf/doc/examples/layout/hierarchical-ranking.cpp > CMakeFiles/ex-hierarchical-ranking.dir/doc/examples/layout/hierarchical-ranking.cpp.i

CMakeFiles/ex-hierarchical-ranking.dir/doc/examples/layout/hierarchical-ranking.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ex-hierarchical-ranking.dir/doc/examples/layout/hierarchical-ranking.cpp.s"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /__w/ogdf/ogdf/doc/examples/layout/hierarchical-ranking.cpp -o CMakeFiles/ex-hierarchical-ranking.dir/doc/examples/layout/hierarchical-ranking.cpp.s

# Object files for target ex-hierarchical-ranking
ex__hierarchical__ranking_OBJECTS = \
"CMakeFiles/ex-hierarchical-ranking.dir/doc/examples/layout/hierarchical-ranking.cpp.o"

# External object files for target ex-hierarchical-ranking
ex__hierarchical__ranking_EXTERNAL_OBJECTS =

../doc/examples/layout/ex-hierarchical-ranking: CMakeFiles/ex-hierarchical-ranking.dir/doc/examples/layout/hierarchical-ranking.cpp.o
../doc/examples/layout/ex-hierarchical-ranking: CMakeFiles/ex-hierarchical-ranking.dir/build.make
../doc/examples/layout/ex-hierarchical-ranking: libOGDF.so
../doc/examples/layout/ex-hierarchical-ranking: libCOIN.so
../doc/examples/layout/ex-hierarchical-ranking: /usr/lib/x86_64-linux-gnu/libgmpxx.so
../doc/examples/layout/ex-hierarchical-ranking: /usr/lib/x86_64-linux-gnu/libmpfr.so
../doc/examples/layout/ex-hierarchical-ranking: /usr/lib/x86_64-linux-gnu/libgmp.so
../doc/examples/layout/ex-hierarchical-ranking: CMakeFiles/ex-hierarchical-ranking.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/__w/ogdf/ogdf/build-static-analysis/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../doc/examples/layout/ex-hierarchical-ranking"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ex-hierarchical-ranking.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ex-hierarchical-ranking.dir/build: ../doc/examples/layout/ex-hierarchical-ranking

.PHONY : CMakeFiles/ex-hierarchical-ranking.dir/build

CMakeFiles/ex-hierarchical-ranking.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ex-hierarchical-ranking.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ex-hierarchical-ranking.dir/clean

CMakeFiles/ex-hierarchical-ranking.dir/depend:
	cd /__w/ogdf/ogdf/build-static-analysis && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /__w/ogdf/ogdf /__w/ogdf/ogdf /__w/ogdf/ogdf/build-static-analysis /__w/ogdf/ogdf/build-static-analysis /__w/ogdf/ogdf/build-static-analysis/CMakeFiles/ex-hierarchical-ranking.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ex-hierarchical-ranking.dir/depend

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
include CMakeFiles/ex-multilevelmixer.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/ex-multilevelmixer.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ex-multilevelmixer.dir/flags.make

CMakeFiles/ex-multilevelmixer.dir/doc/examples/layout/multilevelmixer.cpp.o: CMakeFiles/ex-multilevelmixer.dir/flags.make
CMakeFiles/ex-multilevelmixer.dir/doc/examples/layout/multilevelmixer.cpp.o: ../doc/examples/layout/multilevelmixer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/__w/ogdf/ogdf/build-static-analysis/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/ex-multilevelmixer.dir/doc/examples/layout/multilevelmixer.cpp.o"
	ccache /usr/bin/clang++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ex-multilevelmixer.dir/doc/examples/layout/multilevelmixer.cpp.o -c /__w/ogdf/ogdf/doc/examples/layout/multilevelmixer.cpp

CMakeFiles/ex-multilevelmixer.dir/doc/examples/layout/multilevelmixer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ex-multilevelmixer.dir/doc/examples/layout/multilevelmixer.cpp.i"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /__w/ogdf/ogdf/doc/examples/layout/multilevelmixer.cpp > CMakeFiles/ex-multilevelmixer.dir/doc/examples/layout/multilevelmixer.cpp.i

CMakeFiles/ex-multilevelmixer.dir/doc/examples/layout/multilevelmixer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ex-multilevelmixer.dir/doc/examples/layout/multilevelmixer.cpp.s"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /__w/ogdf/ogdf/doc/examples/layout/multilevelmixer.cpp -o CMakeFiles/ex-multilevelmixer.dir/doc/examples/layout/multilevelmixer.cpp.s

# Object files for target ex-multilevelmixer
ex__multilevelmixer_OBJECTS = \
"CMakeFiles/ex-multilevelmixer.dir/doc/examples/layout/multilevelmixer.cpp.o"

# External object files for target ex-multilevelmixer
ex__multilevelmixer_EXTERNAL_OBJECTS =

../doc/examples/layout/ex-multilevelmixer: CMakeFiles/ex-multilevelmixer.dir/doc/examples/layout/multilevelmixer.cpp.o
../doc/examples/layout/ex-multilevelmixer: CMakeFiles/ex-multilevelmixer.dir/build.make
../doc/examples/layout/ex-multilevelmixer: libOGDF.so
../doc/examples/layout/ex-multilevelmixer: libCOIN.so
../doc/examples/layout/ex-multilevelmixer: /usr/lib/x86_64-linux-gnu/libgmpxx.so
../doc/examples/layout/ex-multilevelmixer: /usr/lib/x86_64-linux-gnu/libmpfr.so
../doc/examples/layout/ex-multilevelmixer: /usr/lib/x86_64-linux-gnu/libgmp.so
../doc/examples/layout/ex-multilevelmixer: CMakeFiles/ex-multilevelmixer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/__w/ogdf/ogdf/build-static-analysis/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../doc/examples/layout/ex-multilevelmixer"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ex-multilevelmixer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ex-multilevelmixer.dir/build: ../doc/examples/layout/ex-multilevelmixer

.PHONY : CMakeFiles/ex-multilevelmixer.dir/build

CMakeFiles/ex-multilevelmixer.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ex-multilevelmixer.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ex-multilevelmixer.dir/clean

CMakeFiles/ex-multilevelmixer.dir/depend:
	cd /__w/ogdf/ogdf/build-static-analysis && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /__w/ogdf/ogdf /__w/ogdf/ogdf /__w/ogdf/ogdf/build-static-analysis /__w/ogdf/ogdf/build-static-analysis /__w/ogdf/ogdf/build-static-analysis/CMakeFiles/ex-multilevelmixer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ex-multilevelmixer.dir/depend

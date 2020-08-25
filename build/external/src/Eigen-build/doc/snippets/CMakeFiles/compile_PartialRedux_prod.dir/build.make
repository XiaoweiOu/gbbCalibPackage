# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.11

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
CMAKE_COMMAND = /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/x86_64/Cmake/3.11.0/Linux-x86_64/bin/cmake

# The command to remove a file.
RM = /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/x86_64/Cmake/3.11.0/Linux-x86_64/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build

# Include any dependencies generated for this target.
include doc/snippets/CMakeFiles/compile_PartialRedux_prod.dir/depend.make

# Include the progress variables for this target.
include doc/snippets/CMakeFiles/compile_PartialRedux_prod.dir/progress.make

# Include the compile flags for this target's objects.
include doc/snippets/CMakeFiles/compile_PartialRedux_prod.dir/flags.make

doc/snippets/CMakeFiles/compile_PartialRedux_prod.dir/compile_PartialRedux_prod.cpp.o: doc/snippets/CMakeFiles/compile_PartialRedux_prod.dir/flags.make
doc/snippets/CMakeFiles/compile_PartialRedux_prod.dir/compile_PartialRedux_prod.cpp.o: doc/snippets/compile_PartialRedux_prod.cpp
doc/snippets/CMakeFiles/compile_PartialRedux_prod.dir/compile_PartialRedux_prod.cpp.o: /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/doc/snippets/PartialRedux_prod.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object doc/snippets/CMakeFiles/compile_PartialRedux_prod.dir/compile_PartialRedux_prod.cpp.o"
	cd /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/snippets && /cvmfs/sft.cern.ch/lcg/releases/gcc/6.2.0-b9934/x86_64-slc6/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/compile_PartialRedux_prod.dir/compile_PartialRedux_prod.cpp.o -c /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/snippets/compile_PartialRedux_prod.cpp

doc/snippets/CMakeFiles/compile_PartialRedux_prod.dir/compile_PartialRedux_prod.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/compile_PartialRedux_prod.dir/compile_PartialRedux_prod.cpp.i"
	cd /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/snippets && /cvmfs/sft.cern.ch/lcg/releases/gcc/6.2.0-b9934/x86_64-slc6/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/snippets/compile_PartialRedux_prod.cpp > CMakeFiles/compile_PartialRedux_prod.dir/compile_PartialRedux_prod.cpp.i

doc/snippets/CMakeFiles/compile_PartialRedux_prod.dir/compile_PartialRedux_prod.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/compile_PartialRedux_prod.dir/compile_PartialRedux_prod.cpp.s"
	cd /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/snippets && /cvmfs/sft.cern.ch/lcg/releases/gcc/6.2.0-b9934/x86_64-slc6/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/snippets/compile_PartialRedux_prod.cpp -o CMakeFiles/compile_PartialRedux_prod.dir/compile_PartialRedux_prod.cpp.s

# Object files for target compile_PartialRedux_prod
compile_PartialRedux_prod_OBJECTS = \
"CMakeFiles/compile_PartialRedux_prod.dir/compile_PartialRedux_prod.cpp.o"

# External object files for target compile_PartialRedux_prod
compile_PartialRedux_prod_EXTERNAL_OBJECTS =

doc/snippets/compile_PartialRedux_prod: doc/snippets/CMakeFiles/compile_PartialRedux_prod.dir/compile_PartialRedux_prod.cpp.o
doc/snippets/compile_PartialRedux_prod: doc/snippets/CMakeFiles/compile_PartialRedux_prod.dir/build.make
doc/snippets/compile_PartialRedux_prod: doc/snippets/CMakeFiles/compile_PartialRedux_prod.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable compile_PartialRedux_prod"
	cd /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/snippets && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/compile_PartialRedux_prod.dir/link.txt --verbose=$(VERBOSE)
	cd /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/snippets && ./compile_PartialRedux_prod >/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/snippets/PartialRedux_prod.out

# Rule to build all files generated by this target.
doc/snippets/CMakeFiles/compile_PartialRedux_prod.dir/build: doc/snippets/compile_PartialRedux_prod

.PHONY : doc/snippets/CMakeFiles/compile_PartialRedux_prod.dir/build

doc/snippets/CMakeFiles/compile_PartialRedux_prod.dir/clean:
	cd /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/snippets && $(CMAKE_COMMAND) -P CMakeFiles/compile_PartialRedux_prod.dir/cmake_clean.cmake
.PHONY : doc/snippets/CMakeFiles/compile_PartialRedux_prod.dir/clean

doc/snippets/CMakeFiles/compile_PartialRedux_prod.dir/depend:
	cd /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/doc/snippets /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/snippets /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/snippets/CMakeFiles/compile_PartialRedux_prod.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : doc/snippets/CMakeFiles/compile_PartialRedux_prod.dir/depend


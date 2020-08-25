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
include doc/examples/CMakeFiles/class_VectorBlock.dir/depend.make

# Include the progress variables for this target.
include doc/examples/CMakeFiles/class_VectorBlock.dir/progress.make

# Include the compile flags for this target's objects.
include doc/examples/CMakeFiles/class_VectorBlock.dir/flags.make

doc/examples/CMakeFiles/class_VectorBlock.dir/class_VectorBlock.cpp.o: doc/examples/CMakeFiles/class_VectorBlock.dir/flags.make
doc/examples/CMakeFiles/class_VectorBlock.dir/class_VectorBlock.cpp.o: /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/doc/examples/class_VectorBlock.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object doc/examples/CMakeFiles/class_VectorBlock.dir/class_VectorBlock.cpp.o"
	cd /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/examples && /cvmfs/sft.cern.ch/lcg/releases/gcc/6.2.0-b9934/x86_64-slc6/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/class_VectorBlock.dir/class_VectorBlock.cpp.o -c /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/doc/examples/class_VectorBlock.cpp

doc/examples/CMakeFiles/class_VectorBlock.dir/class_VectorBlock.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/class_VectorBlock.dir/class_VectorBlock.cpp.i"
	cd /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/examples && /cvmfs/sft.cern.ch/lcg/releases/gcc/6.2.0-b9934/x86_64-slc6/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/doc/examples/class_VectorBlock.cpp > CMakeFiles/class_VectorBlock.dir/class_VectorBlock.cpp.i

doc/examples/CMakeFiles/class_VectorBlock.dir/class_VectorBlock.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/class_VectorBlock.dir/class_VectorBlock.cpp.s"
	cd /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/examples && /cvmfs/sft.cern.ch/lcg/releases/gcc/6.2.0-b9934/x86_64-slc6/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/doc/examples/class_VectorBlock.cpp -o CMakeFiles/class_VectorBlock.dir/class_VectorBlock.cpp.s

# Object files for target class_VectorBlock
class_VectorBlock_OBJECTS = \
"CMakeFiles/class_VectorBlock.dir/class_VectorBlock.cpp.o"

# External object files for target class_VectorBlock
class_VectorBlock_EXTERNAL_OBJECTS =

doc/examples/class_VectorBlock: doc/examples/CMakeFiles/class_VectorBlock.dir/class_VectorBlock.cpp.o
doc/examples/class_VectorBlock: doc/examples/CMakeFiles/class_VectorBlock.dir/build.make
doc/examples/class_VectorBlock: doc/examples/CMakeFiles/class_VectorBlock.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable class_VectorBlock"
	cd /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/examples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/class_VectorBlock.dir/link.txt --verbose=$(VERBOSE)
	cd /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/examples && ./class_VectorBlock >/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/examples/class_VectorBlock.out

# Rule to build all files generated by this target.
doc/examples/CMakeFiles/class_VectorBlock.dir/build: doc/examples/class_VectorBlock

.PHONY : doc/examples/CMakeFiles/class_VectorBlock.dir/build

doc/examples/CMakeFiles/class_VectorBlock.dir/clean:
	cd /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/examples && $(CMAKE_COMMAND) -P CMakeFiles/class_VectorBlock.dir/cmake_clean.cmake
.PHONY : doc/examples/CMakeFiles/class_VectorBlock.dir/clean

doc/examples/CMakeFiles/class_VectorBlock.dir/depend:
	cd /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/doc/examples /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/examples /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/examples/CMakeFiles/class_VectorBlock.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : doc/examples/CMakeFiles/class_VectorBlock.dir/depend


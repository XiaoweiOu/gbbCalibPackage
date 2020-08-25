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
include test/CMakeFiles/integer_types_4.dir/depend.make

# Include the progress variables for this target.
include test/CMakeFiles/integer_types_4.dir/progress.make

# Include the compile flags for this target's objects.
include test/CMakeFiles/integer_types_4.dir/flags.make

test/CMakeFiles/integer_types_4.dir/integer_types.cpp.o: test/CMakeFiles/integer_types_4.dir/flags.make
test/CMakeFiles/integer_types_4.dir/integer_types.cpp.o: /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/test/integer_types.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object test/CMakeFiles/integer_types_4.dir/integer_types.cpp.o"
	cd /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/test && /cvmfs/sft.cern.ch/lcg/releases/gcc/6.2.0-b9934/x86_64-slc6/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/integer_types_4.dir/integer_types.cpp.o -c /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/test/integer_types.cpp

test/CMakeFiles/integer_types_4.dir/integer_types.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/integer_types_4.dir/integer_types.cpp.i"
	cd /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/test && /cvmfs/sft.cern.ch/lcg/releases/gcc/6.2.0-b9934/x86_64-slc6/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/test/integer_types.cpp > CMakeFiles/integer_types_4.dir/integer_types.cpp.i

test/CMakeFiles/integer_types_4.dir/integer_types.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/integer_types_4.dir/integer_types.cpp.s"
	cd /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/test && /cvmfs/sft.cern.ch/lcg/releases/gcc/6.2.0-b9934/x86_64-slc6/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/test/integer_types.cpp -o CMakeFiles/integer_types_4.dir/integer_types.cpp.s

# Object files for target integer_types_4
integer_types_4_OBJECTS = \
"CMakeFiles/integer_types_4.dir/integer_types.cpp.o"

# External object files for target integer_types_4
integer_types_4_EXTERNAL_OBJECTS =

test/integer_types_4: test/CMakeFiles/integer_types_4.dir/integer_types.cpp.o
test/integer_types_4: test/CMakeFiles/integer_types_4.dir/build.make
test/integer_types_4: test/CMakeFiles/integer_types_4.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable integer_types_4"
	cd /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/integer_types_4.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/CMakeFiles/integer_types_4.dir/build: test/integer_types_4

.PHONY : test/CMakeFiles/integer_types_4.dir/build

test/CMakeFiles/integer_types_4.dir/clean:
	cd /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/test && $(CMAKE_COMMAND) -P CMakeFiles/integer_types_4.dir/cmake_clean.cmake
.PHONY : test/CMakeFiles/integer_types_4.dir/clean

test/CMakeFiles/integer_types_4.dir/depend:
	cd /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/test /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/test /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/test/CMakeFiles/integer_types_4.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/integer_types_4.dir/depend


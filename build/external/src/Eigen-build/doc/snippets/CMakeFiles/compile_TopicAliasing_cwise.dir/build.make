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
include doc/snippets/CMakeFiles/compile_TopicAliasing_cwise.dir/depend.make

# Include the progress variables for this target.
include doc/snippets/CMakeFiles/compile_TopicAliasing_cwise.dir/progress.make

# Include the compile flags for this target's objects.
include doc/snippets/CMakeFiles/compile_TopicAliasing_cwise.dir/flags.make

doc/snippets/CMakeFiles/compile_TopicAliasing_cwise.dir/compile_TopicAliasing_cwise.cpp.o: doc/snippets/CMakeFiles/compile_TopicAliasing_cwise.dir/flags.make
doc/snippets/CMakeFiles/compile_TopicAliasing_cwise.dir/compile_TopicAliasing_cwise.cpp.o: doc/snippets/compile_TopicAliasing_cwise.cpp
doc/snippets/CMakeFiles/compile_TopicAliasing_cwise.dir/compile_TopicAliasing_cwise.cpp.o: /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/doc/snippets/TopicAliasing_cwise.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object doc/snippets/CMakeFiles/compile_TopicAliasing_cwise.dir/compile_TopicAliasing_cwise.cpp.o"
	cd /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/snippets && /cvmfs/sft.cern.ch/lcg/releases/gcc/6.2.0-b9934/x86_64-slc6/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/compile_TopicAliasing_cwise.dir/compile_TopicAliasing_cwise.cpp.o -c /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/snippets/compile_TopicAliasing_cwise.cpp

doc/snippets/CMakeFiles/compile_TopicAliasing_cwise.dir/compile_TopicAliasing_cwise.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/compile_TopicAliasing_cwise.dir/compile_TopicAliasing_cwise.cpp.i"
	cd /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/snippets && /cvmfs/sft.cern.ch/lcg/releases/gcc/6.2.0-b9934/x86_64-slc6/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/snippets/compile_TopicAliasing_cwise.cpp > CMakeFiles/compile_TopicAliasing_cwise.dir/compile_TopicAliasing_cwise.cpp.i

doc/snippets/CMakeFiles/compile_TopicAliasing_cwise.dir/compile_TopicAliasing_cwise.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/compile_TopicAliasing_cwise.dir/compile_TopicAliasing_cwise.cpp.s"
	cd /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/snippets && /cvmfs/sft.cern.ch/lcg/releases/gcc/6.2.0-b9934/x86_64-slc6/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/snippets/compile_TopicAliasing_cwise.cpp -o CMakeFiles/compile_TopicAliasing_cwise.dir/compile_TopicAliasing_cwise.cpp.s

# Object files for target compile_TopicAliasing_cwise
compile_TopicAliasing_cwise_OBJECTS = \
"CMakeFiles/compile_TopicAliasing_cwise.dir/compile_TopicAliasing_cwise.cpp.o"

# External object files for target compile_TopicAliasing_cwise
compile_TopicAliasing_cwise_EXTERNAL_OBJECTS =

doc/snippets/compile_TopicAliasing_cwise: doc/snippets/CMakeFiles/compile_TopicAliasing_cwise.dir/compile_TopicAliasing_cwise.cpp.o
doc/snippets/compile_TopicAliasing_cwise: doc/snippets/CMakeFiles/compile_TopicAliasing_cwise.dir/build.make
doc/snippets/compile_TopicAliasing_cwise: doc/snippets/CMakeFiles/compile_TopicAliasing_cwise.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable compile_TopicAliasing_cwise"
	cd /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/snippets && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/compile_TopicAliasing_cwise.dir/link.txt --verbose=$(VERBOSE)
	cd /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/snippets && ./compile_TopicAliasing_cwise >/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/snippets/TopicAliasing_cwise.out

# Rule to build all files generated by this target.
doc/snippets/CMakeFiles/compile_TopicAliasing_cwise.dir/build: doc/snippets/compile_TopicAliasing_cwise

.PHONY : doc/snippets/CMakeFiles/compile_TopicAliasing_cwise.dir/build

doc/snippets/CMakeFiles/compile_TopicAliasing_cwise.dir/clean:
	cd /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/snippets && $(CMAKE_COMMAND) -P CMakeFiles/compile_TopicAliasing_cwise.dir/cmake_clean.cmake
.PHONY : doc/snippets/CMakeFiles/compile_TopicAliasing_cwise.dir/clean

doc/snippets/CMakeFiles/compile_TopicAliasing_cwise.dir/depend:
	cd /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/doc/snippets /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/snippets /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/doc/snippets/CMakeFiles/compile_TopicAliasing_cwise.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : doc/snippets/CMakeFiles/compile_TopicAliasing_cwise.dir/depend


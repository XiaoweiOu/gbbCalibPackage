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
CMAKE_SOURCE_DIR = /home/ouxiaowei/gbbCalibPackage/source

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ouxiaowei/gbbCalibPackage/build

# Utility rule file for Package_gbbCalibration.

# Include the progress variables for this target.
include gbbCalibration/CMakeFiles/Package_gbbCalibration.dir/progress.make

gbbCalibration/CMakeFiles/Package_gbbCalibration:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/ouxiaowei/gbbCalibPackage/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Built package gbbCalibration"
	cd /home/ouxiaowei/gbbCalibPackage/build/gbbCalibration && /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/x86_64/Cmake/3.11.0/Linux-x86_64/bin/cmake -E echo gbbCalibration:\ Package\ build\ succeeded

Package_gbbCalibration: gbbCalibration/CMakeFiles/Package_gbbCalibration
Package_gbbCalibration: gbbCalibration/CMakeFiles/Package_gbbCalibration.dir/build.make

.PHONY : Package_gbbCalibration

# Rule to build all files generated by this target.
gbbCalibration/CMakeFiles/Package_gbbCalibration.dir/build: Package_gbbCalibration

.PHONY : gbbCalibration/CMakeFiles/Package_gbbCalibration.dir/build

gbbCalibration/CMakeFiles/Package_gbbCalibration.dir/clean:
	cd /home/ouxiaowei/gbbCalibPackage/build/gbbCalibration && $(CMAKE_COMMAND) -P CMakeFiles/Package_gbbCalibration.dir/cmake_clean.cmake
.PHONY : gbbCalibration/CMakeFiles/Package_gbbCalibration.dir/clean

gbbCalibration/CMakeFiles/Package_gbbCalibration.dir/depend:
	cd /home/ouxiaowei/gbbCalibPackage/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ouxiaowei/gbbCalibPackage/source /home/ouxiaowei/gbbCalibPackage/source/gbbCalibration /home/ouxiaowei/gbbCalibPackage/build /home/ouxiaowei/gbbCalibPackage/build/gbbCalibration /home/ouxiaowei/gbbCalibPackage/build/gbbCalibration/CMakeFiles/Package_gbbCalibration.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : gbbCalibration/CMakeFiles/Package_gbbCalibration.dir/depend

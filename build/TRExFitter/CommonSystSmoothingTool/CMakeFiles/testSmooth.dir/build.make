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

# Include any dependencies generated for this target.
include TRExFitter/CommonSystSmoothingTool/CMakeFiles/testSmooth.dir/depend.make

# Include the progress variables for this target.
include TRExFitter/CommonSystSmoothingTool/CMakeFiles/testSmooth.dir/progress.make

# Include the compile flags for this target's objects.
include TRExFitter/CommonSystSmoothingTool/CMakeFiles/testSmooth.dir/flags.make

TRExFitter/CommonSystSmoothingTool/CMakeFiles/testSmooth.dir/util/testSmooth.cxx.o: TRExFitter/CommonSystSmoothingTool/CMakeFiles/testSmooth.dir/flags.make
TRExFitter/CommonSystSmoothingTool/CMakeFiles/testSmooth.dir/util/testSmooth.cxx.o: /home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonSystSmoothingTool/util/testSmooth.cxx
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ouxiaowei/gbbCalibPackage/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object TRExFitter/CommonSystSmoothingTool/CMakeFiles/testSmooth.dir/util/testSmooth.cxx.o"
	cd /home/ouxiaowei/gbbCalibPackage/build/TRExFitter/CommonSystSmoothingTool && /cvmfs/sft.cern.ch/lcg/releases/gcc/6.2.0-b9934/x86_64-slc6/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/testSmooth.dir/util/testSmooth.cxx.o -c /home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonSystSmoothingTool/util/testSmooth.cxx

TRExFitter/CommonSystSmoothingTool/CMakeFiles/testSmooth.dir/util/testSmooth.cxx.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/testSmooth.dir/util/testSmooth.cxx.i"
	cd /home/ouxiaowei/gbbCalibPackage/build/TRExFitter/CommonSystSmoothingTool && /cvmfs/sft.cern.ch/lcg/releases/gcc/6.2.0-b9934/x86_64-slc6/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonSystSmoothingTool/util/testSmooth.cxx > CMakeFiles/testSmooth.dir/util/testSmooth.cxx.i

TRExFitter/CommonSystSmoothingTool/CMakeFiles/testSmooth.dir/util/testSmooth.cxx.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/testSmooth.dir/util/testSmooth.cxx.s"
	cd /home/ouxiaowei/gbbCalibPackage/build/TRExFitter/CommonSystSmoothingTool && /cvmfs/sft.cern.ch/lcg/releases/gcc/6.2.0-b9934/x86_64-slc6/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonSystSmoothingTool/util/testSmooth.cxx -o CMakeFiles/testSmooth.dir/util/testSmooth.cxx.s

# Object files for target testSmooth
testSmooth_OBJECTS = \
"CMakeFiles/testSmooth.dir/util/testSmooth.cxx.o"

# External object files for target testSmooth
testSmooth_EXTERNAL_OBJECTS =

generic/bin/testSmooth: TRExFitter/CommonSystSmoothingTool/CMakeFiles/testSmooth.dir/util/testSmooth.cxx.o
generic/bin/testSmooth: TRExFitter/CommonSystSmoothingTool/CMakeFiles/testSmooth.dir/build.make
generic/bin/testSmooth: generic/lib/libCommonSystSmoothingToolLib.so
generic/bin/testSmooth: /cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBaseExternals/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/lib/libCore.so
generic/bin/testSmooth: /cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBaseExternals/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/lib/libHist.so
generic/bin/testSmooth: /cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBaseExternals/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/lib/libRIO.so
generic/bin/testSmooth: /cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBaseExternals/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/lib/libTree.so
generic/bin/testSmooth: /cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBaseExternals/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/lib/libGpad.so
generic/bin/testSmooth: TRExFitter/CommonSystSmoothingTool/CMakeFiles/testSmooth.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ouxiaowei/gbbCalibPackage/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../generic/bin/testSmooth"
	cd /home/ouxiaowei/gbbCalibPackage/build/TRExFitter/CommonSystSmoothingTool && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/testSmooth.dir/link.txt --verbose=$(VERBOSE)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Detaching debug info of testSmooth into testSmooth.dbg"
	cd /home/ouxiaowei/gbbCalibPackage/build/generic/bin && /cvmfs/sft.cern.ch/lcg/releases/binutils/2.28-a983d/x86_64-slc6/bin/objcopy --only-keep-debug testSmooth testSmooth.dbg
	cd /home/ouxiaowei/gbbCalibPackage/build/generic/bin && /cvmfs/sft.cern.ch/lcg/releases/binutils/2.28-a983d/x86_64-slc6/bin/objcopy --strip-debug testSmooth
	cd /home/ouxiaowei/gbbCalibPackage/build/generic/bin && /cvmfs/sft.cern.ch/lcg/releases/binutils/2.28-a983d/x86_64-slc6/bin/objcopy --add-gnu-debuglink=testSmooth.dbg testSmooth

# Rule to build all files generated by this target.
TRExFitter/CommonSystSmoothingTool/CMakeFiles/testSmooth.dir/build: generic/bin/testSmooth

.PHONY : TRExFitter/CommonSystSmoothingTool/CMakeFiles/testSmooth.dir/build

TRExFitter/CommonSystSmoothingTool/CMakeFiles/testSmooth.dir/clean:
	cd /home/ouxiaowei/gbbCalibPackage/build/TRExFitter/CommonSystSmoothingTool && $(CMAKE_COMMAND) -P CMakeFiles/testSmooth.dir/cmake_clean.cmake
.PHONY : TRExFitter/CommonSystSmoothingTool/CMakeFiles/testSmooth.dir/clean

TRExFitter/CommonSystSmoothingTool/CMakeFiles/testSmooth.dir/depend:
	cd /home/ouxiaowei/gbbCalibPackage/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ouxiaowei/gbbCalibPackage/source /home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonSystSmoothingTool /home/ouxiaowei/gbbCalibPackage/build /home/ouxiaowei/gbbCalibPackage/build/TRExFitter/CommonSystSmoothingTool /home/ouxiaowei/gbbCalibPackage/build/TRExFitter/CommonSystSmoothingTool/CMakeFiles/testSmooth.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : TRExFitter/CommonSystSmoothingTool/CMakeFiles/testSmooth.dir/depend


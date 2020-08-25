# Install script for directory: /home/ouxiaowei/gbbCalibPackage/source/gbbCalibration

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/InstallArea/generic")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RelWithDebInfo")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/src/gbbCalibration" TYPE DIRECTORY FILES "/home/ouxiaowei/gbbCalibPackage/source/gbbCalibration/" USE_SOURCE_PERMISSIONS REGEX "/\\.svn$" EXCLUDE REGEX "/\\.git$" EXCLUDE REGEX "/[^/]*\\~$" EXCLUDE)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE OPTIONAL FILES "/home/ouxiaowei/gbbCalibPackage/build/generic/lib/libgbbCommonLib_rdict.pcm")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  execute_process( COMMAND ${CMAKE_COMMAND}
      -E make_directory
      $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/include )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  execute_process( COMMAND ${CMAKE_COMMAND}
         -E create_symlink ../src/gbbCalibration/helpers
         $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/include/helpers )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDebugx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE OPTIONAL FILES "/home/ouxiaowei/gbbCalibPackage/build/generic/lib/libgbbCommonLib.so.dbg")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY OPTIONAL FILES "/home/ouxiaowei/gbbCalibPackage/build/generic/lib/libgbbCommonLib.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libgbbCommonLib.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libgbbCommonLib.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/cvmfs/sft.cern.ch/lcg/releases/binutils/2.28-a983d/x86_64-slc6/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libgbbCommonLib.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  execute_process( COMMAND ${CMAKE_COMMAND}
      -E make_directory
      $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/include )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  execute_process( COMMAND ${CMAKE_COMMAND}
         -E create_symlink ../src/gbbCalibration/histMaker
         $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/include/histMaker )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDebugx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE OPTIONAL FILES "/home/ouxiaowei/gbbCalibPackage/build/generic/lib/libgbbTupleLib.so.dbg")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY OPTIONAL FILES "/home/ouxiaowei/gbbCalibPackage/build/generic/lib/libgbbTupleLib.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libgbbTupleLib.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libgbbTupleLib.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/cvmfs/sft.cern.ch/lcg/releases/binutils/2.28-a983d/x86_64-slc6/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libgbbTupleLib.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  execute_process( COMMAND ${CMAKE_COMMAND}
      -E make_directory
      $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/include )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  execute_process( COMMAND ${CMAKE_COMMAND}
         -E create_symlink ../src/gbbCalibration/scaleFactorCalculator
         $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/include/scaleFactorCalculator )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDebugx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE OPTIONAL FILES "/home/ouxiaowei/gbbCalibPackage/build/generic/lib/libScaleFactorCalcLib.so.dbg")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY OPTIONAL FILES "/home/ouxiaowei/gbbCalibPackage/build/generic/lib/libScaleFactorCalcLib.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libScaleFactorCalcLib.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libScaleFactorCalcLib.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/cvmfs/sft.cern.ch/lcg/releases/binutils/2.28-a983d/x86_64-slc6/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libScaleFactorCalcLib.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDebugx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE OPTIONAL FILES "/home/ouxiaowei/gbbCalibPackage/build/generic/bin/run_gbbTupleAna.dbg")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE PROGRAM FILES "/home/ouxiaowei/gbbCalibPackage/build/generic/bin/run_gbbTupleAna.exe")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE OPTIONAL FILES "/home/ouxiaowei/gbbCalibPackage/build/generic/bin/run_gbbTupleAna")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/run_gbbTupleAna" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/run_gbbTupleAna")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/cvmfs/sft.cern.ch/lcg/releases/binutils/2.28-a983d/x86_64-slc6/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/run_gbbTupleAna")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDebugx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE OPTIONAL FILES "/home/ouxiaowei/gbbCalibPackage/build/generic/bin/run_calculateSF.dbg")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE PROGRAM FILES "/home/ouxiaowei/gbbCalibPackage/build/generic/bin/run_calculateSF.exe")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE OPTIONAL FILES "/home/ouxiaowei/gbbCalibPackage/build/generic/bin/run_calculateSF")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/run_calculateSF" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/run_calculateSF")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/cvmfs/sft.cern.ch/lcg/releases/binutils/2.28-a983d/x86_64-slc6/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/run_calculateSF")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE PROGRAM RENAME "run_trex.sh" FILES "/home/ouxiaowei/gbbCalibPackage/source/gbbCalibration/util/run_trex.sh")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/python/gbbCalibration" TYPE FILE RENAME "makeReweightHistos.py" FILES "/home/ouxiaowei/gbbCalibPackage/source/gbbCalibration/python/makeReweightHistos.py")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/python/gbbCalibration" TYPE FILE FILES "/home/ouxiaowei/gbbCalibPackage/build/generic/python/gbbCalibration/makeReweightHistos.pyc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/data/gbbCalibration" TYPE FILE RENAME "PMGxsecDB_mc16_2020-03-09.txt" FILES "/home/ouxiaowei/gbbCalibPackage/source/gbbCalibration/data/PMGxsecDB_mc16_2020-03-09.txt")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/data/gbbCalibration" TYPE FILE RENAME "SampleInfo_MuonFilterTest.txt" FILES "/home/ouxiaowei/gbbCalibPackage/source/gbbCalibration/data/SampleInfo_MuonFilterTest.txt")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/data/gbbCalibration" TYPE DIRECTORY FILES "/home/ouxiaowei/gbbCalibPackage/source/gbbCalibration/data/configs" USE_SOURCE_PERMISSIONS REGEX "/\\.svn$" EXCLUDE)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/data/gbbCalibration" TYPE DIRECTORY FILES "/home/ouxiaowei/gbbCalibPackage/source/gbbCalibration/data/examples" USE_SOURCE_PERMISSIONS REGEX "/\\.svn$" EXCLUDE)
endif()


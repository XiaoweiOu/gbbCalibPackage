# Install script for directory: /home/ouxiaowei/gbbCalibPackage/source/TRExFitter

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/ouxiaowei/gbbCalibPackage/build/generic/lib/libTRExFitter.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libTRExFitter.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libTRExFitter.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/cvmfs/sft.cern.ch/lcg/releases/binutils/2.28-a983d/x86_64-slc6/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libTRExFitter.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/TRExFitter" TYPE FILE FILES
    "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/TRExFitter/Common.h"
    "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/TRExFitter/ConfigParser.h"
    "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/TRExFitter/ConfigReader.h"
    "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/TRExFitter/ConfigReaderMulti.h"
    "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/TRExFitter/CorrelationMatrix.h"
    "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/TRExFitter/FitResults.h"
    "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/TRExFitter/FittingTool.h"
    "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/TRExFitter/HistoTools.h"
    "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/TRExFitter/MultiFit.h"
    "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/TRExFitter/NormFactor.h"
    "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/TRExFitter/NuisParameter.h"
    "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/TRExFitter/PruningUtil.h"
    "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/TRExFitter/Region.h"
    "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/TRExFitter/Sample.h"
    "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/TRExFitter/SampleHist.h"
    "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/TRExFitter/ShapeFactor.h"
    "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/TRExFitter/StatusLogbook.h"
    "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/TRExFitter/Systematic.h"
    "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/TRExFitter/SystematicHist.h"
    "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/TRExFitter/TRExFit.h"
    "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/TRExFitter/TRExPlot.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/ouxiaowei/gbbCalibPackage/build/generic/lib/libCommonSystSmoothingTool.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libCommonSystSmoothingTool.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libCommonSystSmoothingTool.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/cvmfs/sft.cern.ch/lcg/releases/binutils/2.28-a983d/x86_64-slc6/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libCommonSystSmoothingTool.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/ouxiaowei/gbbCalibPackage/build/generic/lib/libAtlasUtils.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libAtlasUtils.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libAtlasUtils.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/cvmfs/sft.cern.ch/lcg/releases/binutils/2.28-a983d/x86_64-slc6/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libAtlasUtils.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/home/ouxiaowei/gbbCalibPackage/build/generic/bin/trex-fitter")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/trex-fitter" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/trex-fitter")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/cvmfs/sft.cern.ch/lcg/releases/binutils/2.28-a983d/x86_64-slc6/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/trex-fitter")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/home/ouxiaowei/gbbCalibPackage/build/generic/bin/hupdate.exe")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/hupdate.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/hupdate.exe")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/cvmfs/sft.cern.ch/lcg/releases/binutils/2.28-a983d/x86_64-slc6/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/hupdate.exe")
    endif()
  endif()
endif()


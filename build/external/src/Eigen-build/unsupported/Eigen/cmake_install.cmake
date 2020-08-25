# Install script for directory: /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/unsupported/Eigen

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/ouxiaowei/gbbCalibPackage/build/external/Eigen")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
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

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/unsupported/Eigen" TYPE FILE FILES
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/unsupported/Eigen/AdolcForward"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/unsupported/Eigen/AlignedVector3"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/unsupported/Eigen/ArpackSupport"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/unsupported/Eigen/AutoDiff"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/unsupported/Eigen/BVH"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/unsupported/Eigen/EulerAngles"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/unsupported/Eigen/FFT"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/unsupported/Eigen/IterativeSolvers"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/unsupported/Eigen/KroneckerProduct"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/unsupported/Eigen/LevenbergMarquardt"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/unsupported/Eigen/MatrixFunctions"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/unsupported/Eigen/MoreVectorization"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/unsupported/Eigen/MPRealSupport"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/unsupported/Eigen/NonLinearOptimization"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/unsupported/Eigen/NumericalDiff"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/unsupported/Eigen/OpenGLSupport"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/unsupported/Eigen/Polynomials"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/unsupported/Eigen/Skyline"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/unsupported/Eigen/SparseExtra"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/unsupported/Eigen/SpecialFunctions"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/unsupported/Eigen/Splines"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/unsupported/Eigen" TYPE DIRECTORY FILES "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/unsupported/Eigen/src" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen-build/unsupported/Eigen/CXX11/cmake_install.cmake")

endif()


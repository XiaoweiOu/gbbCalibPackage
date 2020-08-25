# Install script for directory: /home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/Eigen

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/Eigen" TYPE FILE FILES
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/Eigen/Cholesky"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/Eigen/CholmodSupport"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/Eigen/Core"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/Eigen/Dense"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/Eigen/Eigen"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/Eigen/Eigenvalues"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/Eigen/Geometry"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/Eigen/Householder"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/Eigen/IterativeLinearSolvers"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/Eigen/Jacobi"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/Eigen/LU"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/Eigen/MetisSupport"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/Eigen/OrderingMethods"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/Eigen/PaStiXSupport"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/Eigen/PardisoSupport"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/Eigen/QR"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/Eigen/QtAlignedMalloc"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/Eigen/SPQRSupport"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/Eigen/SVD"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/Eigen/Sparse"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/Eigen/SparseCholesky"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/Eigen/SparseCore"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/Eigen/SparseLU"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/Eigen/SparseQR"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/Eigen/StdDeque"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/Eigen/StdList"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/Eigen/StdVector"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/Eigen/SuperLUSupport"
    "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/Eigen/UmfPackSupport"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/Eigen" TYPE DIRECTORY FILES "/home/ouxiaowei/gbbCalibPackage/build/external/src/Eigen/Eigen/src" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()


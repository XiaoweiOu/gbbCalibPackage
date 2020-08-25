#!/bin/bash
#
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# Script used for installing a project for CPack. Hiding any build errors
# from the package generator.
#

# Turn off error reporting:
set +e

# Set the output directory:
OUTDIR=${DESTDIR}/usr/WorkDir/21.2.83/InstallArea/generic

# Create the bin and lib directories:
/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/x86_64/Cmake/3.11.0/Linux-x86_64/bin/cmake -E make_directory ${OUTDIR}/bin
/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/x86_64/Cmake/3.11.0/Linux-x86_64/bin/cmake -E make_directory ${OUTDIR}/lib

# Copy the debug files into the place CPack expects to find them:
/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/x86_64/Cmake/3.11.0/Linux-x86_64/bin/cmake -E copy /home/ouxiaowei/gbbCalibPackage/build/generic/bin/*.dbg \
    ${OUTDIR}/bin/
/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/x86_64/Cmake/3.11.0/Linux-x86_64/bin/cmake -E copy /home/ouxiaowei/gbbCalibPackage/build/generic/lib/*.dbg \
    ${OUTDIR}/lib/

# Exit gracefully:
exit 0

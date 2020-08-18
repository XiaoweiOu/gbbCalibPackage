#!/bin/bash
export PATH=/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBaseExternals/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/bin/:$PATH
cd /home/ouxiaowei/gbbCalibPackage
source ./setup.sh
echo "!!! source complete !!!"
cd /home/ouxiaowei/gbbCalibPackage/run_data
run_gbbTupleAna ./filelist_data/filelist_data_43.txt ./result_data/filelist_data_43.root FlavourTagging_Nominal run_GbbTupleAna_Calib.cfg
run_gbbTupleAna ./filelist_data/filelist_data_44.txt ./result_data/filelist_data_44.root FlavourTagging_Nominal run_GbbTupleAna_Calib.cfg
run_gbbTupleAna ./filelist_data/filelist_data_45.txt ./result_data/filelist_data_45.root FlavourTagging_Nominal run_GbbTupleAna_Calib.cfg
#!/bin/bash
export PATH=/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBaseExternals/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/bin/:$PATH
cd /home/ouxiaowei/gbbCalibPackage
source ./setup.sh
echo "!!! source complete !!!"
cd /home/ouxiaowei/gbbCalibPackage/run_data
run_gbbTupleAna ./filelist_data/filelist_301.txt ./result_data/filelist_301.root FlavourTagging_Nominal run_GbbTupleAna_Calib.cfg
run_gbbTupleAna ./filelist_data/filelist_302.txt ./result_data/filelist_302.root FlavourTagging_Nominal run_GbbTupleAna_Calib.cfg
run_gbbTupleAna ./filelist_data/filelist_303.txt ./result_data/filelist_303.root FlavourTagging_Nominal run_GbbTupleAna_Calib.cfg
run_gbbTupleAna ./filelist_data/filelist_304.txt ./result_data/filelist_304.root FlavourTagging_Nominal run_GbbTupleAna_Calib.cfg
run_gbbTupleAna ./filelist_data/filelist_305.txt ./result_data/filelist_305.root FlavourTagging_Nominal run_GbbTupleAna_Calib.cfg

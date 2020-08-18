#!/bin/bash
export PATH=/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBaseExternals/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/bin/:$PATH
cd /home/ouxiaowei/gbbCalibPackage
source ./setup.sh
echo "!!! source complete !!!"
cd /home/ouxiaowei/gbbCalibPackage/run_data
run_gbbTupleAna ./filelist_data/filelist_271.txt ./result_data/filelist_271.root FlavourTagging_Nominal run_GbbTupleAna_Calib.cfg
run_gbbTupleAna ./filelist_data/filelist_272.txt ./result_data/filelist_272.root FlavourTagging_Nominal run_GbbTupleAna_Calib.cfg
run_gbbTupleAna ./filelist_data/filelist_273.txt ./result_data/filelist_273.root FlavourTagging_Nominal run_GbbTupleAna_Calib.cfg
run_gbbTupleAna ./filelist_data/filelist_274.txt ./result_data/filelist_274.root FlavourTagging_Nominal run_GbbTupleAna_Calib.cfg
run_gbbTupleAna ./filelist_data/filelist_275.txt ./result_data/filelist_275.root FlavourTagging_Nominal run_GbbTupleAna_Calib.cfg
run_gbbTupleAna ./filelist_data/filelist_276.txt ./result_data/filelist_276.root FlavourTagging_Nominal run_GbbTupleAna_Calib.cfg
run_gbbTupleAna ./filelist_data/filelist_277.txt ./result_data/filelist_277.root FlavourTagging_Nominal run_GbbTupleAna_Calib.cfg
run_gbbTupleAna ./filelist_data/filelist_278.txt ./result_data/filelist_278.root FlavourTagging_Nominal run_GbbTupleAna_Calib.cfg
run_gbbTupleAna ./filelist_data/filelist_279.txt ./result_data/filelist_279.root FlavourTagging_Nominal run_GbbTupleAna_Calib.cfg
run_gbbTupleAna ./filelist_data/filelist_280.txt ./result_data/filelist_280.root FlavourTagging_Nominal run_GbbTupleAna_Calib.cfg

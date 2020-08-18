#!/bin/bash
export PATH=/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBaseExternals/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/bin/:$PATH
cd /home/ouxiaowei/gbbCalibPackage
source ./setup.sh
echo "!!! source complete !!!"
cd /home/ouxiaowei/gbbCalibPackage/run_data
run_gbbTupleAna ./filelist_data/filelist_261.txt ./result_data/filelist_261.root FlavourTagging_Nominal run_GbbTupleAna_Calib.cfg
run_gbbTupleAna ./filelist_data/filelist_262.txt ./result_data/filelist_262.root FlavourTagging_Nominal run_GbbTupleAna_Calib.cfg
run_gbbTupleAna ./filelist_data/filelist_263.txt ./result_data/filelist_263.root FlavourTagging_Nominal run_GbbTupleAna_Calib.cfg
run_gbbTupleAna ./filelist_data/filelist_264.txt ./result_data/filelist_264.root FlavourTagging_Nominal run_GbbTupleAna_Calib.cfg
run_gbbTupleAna ./filelist_data/filelist_265.txt ./result_data/filelist_265.root FlavourTagging_Nominal run_GbbTupleAna_Calib.cfg
run_gbbTupleAna ./filelist_data/filelist_266.txt ./result_data/filelist_266.root FlavourTagging_Nominal run_GbbTupleAna_Calib.cfg
run_gbbTupleAna ./filelist_data/filelist_267.txt ./result_data/filelist_267.root FlavourTagging_Nominal run_GbbTupleAna_Calib.cfg
run_gbbTupleAna ./filelist_data/filelist_268.txt ./result_data/filelist_268.root FlavourTagging_Nominal run_GbbTupleAna_Calib.cfg
run_gbbTupleAna ./filelist_data/filelist_269.txt ./result_data/filelist_269.root FlavourTagging_Nominal run_GbbTupleAna_Calib.cfg
run_gbbTupleAna ./filelist_data/filelist_270.txt ./result_data/filelist_270.root FlavourTagging_Nominal run_GbbTupleAna_Calib.cfg

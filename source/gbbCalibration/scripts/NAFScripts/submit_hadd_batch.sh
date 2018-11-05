#!/bin/bash

#Submit script for hadd on batch
echo "Submit hadd jobs on batch"

FILEPATH_STUB="/nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output_Calibration2016/Output_Calib"
LOG_FOLDER="/nfs/dust/atlas/user/ruth/Ruth/QualiTask/JobLogs/"

#Nominal
#qsub -o ${LOG_FOLDER} -j y -l h_rt=01:00:00,distro=sld6,h_vmem=6G -v FILEDIR=${FILEPATH_STUB},DO_MCONLY=0 scripts/NAFScripts/run_hadd_hist_batch.sh
#condor_submit FILEDIR=${FILEPATH_STUB} DO_MCONLY=0 scripts/HTCscripts/hadd_batch.sub



#2-sided-systematics
for sys in MUON_ID MUON_MS MUON_SCALE MUON_SAGITTA_RESBIAS MUON_SAGITTA_RHO MUON_EFF_STAT MUON_EFF_SYS MUON_EFF_STAT_LOWPT MUON_EFF_SYS_LOWPT MUON_TTVA_STAT MUON_TTVA_SYS #JET_Rtrk_Baseline_Kin JET_Rtrk_Modelling_Kin JET_Rtrk_Tracking_Kin JET_Rtrk_TotalStat_Kin JET_Rtrk_Baseline_Sub JET_Rtrk_Modelling_Sub JET_Rtrk_Tracking_Sub JET_Rtrk_TotalStat_Sub 
do
    for var in __1up __1down
    do
	#qsub -o ${LOG_FOLDER} -j y -l h_rt=01:00:00,distro=sld6,h_vmem=4G -v FILEDIR=${FILEPATH_STUB}_${sys}${var},DO_MCONLY=1 scripts/NAFScripts/run_hadd_hist_batch.sh
	condor_submit FILEDIR=${FILEPATH_STUB}_${sys}${var} DO_MCONLY=1 scripts/HTCscripts/hadd_batch.sub
    done

done

#1-sided-systematics
for sys in FATJET_JER FATJET_JMR
do
    for var in __1up
    do
	#qsub -o ${LOG_FOLDER} -j y -l h_rt=01:00:00,distro=sld6,h_vmem=4G -v FILEDIR=${FILEPATH_STUB}_${sys}${var},DO_MCONLY=1 scripts/NAFScripts/run_hadd_hist_batch.sh
	#condor_submit FILEDIR=${FILEPATH_STUB}_${sys}${var} DO_MCONLY=1 scripts/HTCscripts/hadd_batch.sub
    done
done
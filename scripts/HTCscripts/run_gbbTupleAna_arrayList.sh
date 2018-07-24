#!/bin/bash

HOMEDIR=/nfs/dust/atlas/user/ruth/Ruth/QualiTask/Code/gbbTupleAna_improved

cd ${HOMEDIR}

export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/user/atlasLocalSetup.sh
lsetup "root 6.02.12-x86_64-slc6-gcc48-opt --quiet"

#for testing only
#IN_PATH="/nfs/dust/atlas/user/ruth/Ruth/QualiTask/Inputs/FTagNTuples_Calib/v00-01-06"
#SAMP="user.ruth.mc15_13TeV.427005.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-06b.e5660_s2726_r7773_r7676_p3323_tuple.root"
#SGE_TASK_ID=38
#JOB_ID=37
#NFILESPERJOB=5

IN_PATH=${1}
SAMP=${2}
OUTDIR=${3}
TREENAME=${4}
CONFIGNAME=${5}
N_FILESPERJOB=${6}
Cluster=${7}
Process=${8}

NTUP=$(ls ${IN_PATH}/${SAMP}/*.root)
#NFILES=$(echo ${NTUP} | wc -w)

#echo ${NFILES}

arr=(${NTUP})

#for i in ${arr[@]}; do echo $i; done

#echo "task ID is: ${SGE_TASK_ID}" 

#theTask=${arr[${SGE_TASK_ID}]}
#theTask=${arr[${SGE_TASK_ID}-1]} 


OUTFILE="hist_${Cluster}_${Process}.root"

echo "Job is ${Cluster} and task is ${Process}"

#INFILE="${IN_PATH}/${SAMP}/${theTask}"

#ls ${INFILE}

INFILE="list"

START=$((N_FILESPERJOB*Process))
FILELIST=${arr[@]:${START}:${N_FILESPERJOB}}

#exit 1

echo "LD_LIBRARY_PATH: ${LD_LIBRARYPATH}"
echo " User name: $USER"
echo " User home: $HOME"

echo -e "Here is the program run dir: `pwd` \n"

echo "Processing....."

echo ${FILELIST}
./run_gbbTupleAna.exe ${INFILE} ${TMP}/${OUTFILE} ${TREENAME} ${CONFIGNAME} ${FILELIST}

echo "Write Output File to: ${OUTDIR} ....."

mkdir -p ${OUTDIR}

cd ${TMP}
ls
cp -rv *.root* $OUTDIR

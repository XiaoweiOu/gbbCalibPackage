#!/bin/bash

HOMEDIR=/nfs/dust/atlas/user/ruth/Ruth/QualiTask/Code/gbbTupleAna_improved

cd ${HOMEDIR}

export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/user/atlasLocalSetup.sh
lsetup "root 6.02.12-x86_64-slc6-gcc48-opt --quiet"


NTUP=$(ls ${IN_PATH}/${SAMP})

arr=(${NTUP})

#for i in ${arr[@]}; do echo $i; done

echo "task ID is: ${SGE_TASK_ID}" 

#theTask=${arr[${SGE_TASK_ID}]}
theTask=${arr[${SGE_TASK_ID}-1]} 


OUTFILE="hist_${theTask}"

echo "Job is ${JOB_ID} and task is ${theTask}"

INFILE="${IN_PATH}/${SAMP}/${theTask}"

ls ${INFILE}

#exit 1

echo "LD_LIBRARY_PATH: ${LD_LIBRARYPATH}"
echo " User name: $USER"
echo " User home: $HOME"

echo -e "Here is the program run dir: `pwd` \n"

echo "Processing....."

./run_gbbTupleAna.exe ${INFILE} ${TMPDIR}/${OUTFILE} ${TREENAME} ${CONFIGNAME}

echo "Write Output File to: ${OUTDIR} ....."

mkdir -p ${OUTDIR}

cd ${TMPDIR}
ls
cp -rv *.root* $OUTDIR

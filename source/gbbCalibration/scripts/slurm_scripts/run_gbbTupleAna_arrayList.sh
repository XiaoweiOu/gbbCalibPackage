#!/bin/bash

#SBATCH --time=24:00:00
#SBATCH --tasks-per-node=1

#---------------- initial setup -----------------

#homedir
cd ${HOMEDIR}

# setup missing cori's missing library
export LD_LIBRARY_PATH=/global/project/projectdirs/atlas/scripts/extra_libs_180822:$LD_LIBRARY_PATH

# setup ATLAS and root version
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/user/atlasLocalSetup.sh
lsetup "root 6.02.12-x86_64-slc6-gcc48-opt --quiet"

# AB setup
cd ../source
if [ -f ".asetup.save" ]; then
  asetup --restore
else
  asetup AnalysisBase,21.2.35,here
fi
cd ../run

#setup environment variables
source ../build/x86_64-slc6-gcc62-opt/setup.sh

echo -e "\n which python \n"
which python
echo -e "\n which root \n"
which root

#------------------------------------------------
#for testing only
#IN_PATH="/data/atlas/atlasdata/stankait/gbbTuples_R207/MC/mufilter/"
#SAMP="user.ruth.mc15_13TeV.427005.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07b.e5660_s2726_r7773_r7676_p3323_tuple.root/"
##SAMP="user.ruth.mc15_13TeV.361023.DAOD_FTAG1.FTNtupCalib.gbb_v00-01-07b.e3668_s2576_s2132_r7725_r7676_p3323_tuple.root"
##OUTDIR="./output/"
#TREENAME="FlavourTagging_Nominal"
#CONFIGNAME="configs/run_GbbTupleAna_Calib.cfg"
#N_FILESPERJOB="1000000000000"


IN_LIST="${LISTDIR}/${SAMP}.txt"
#if [ -f $IN_LIST ] ; then
#      rm $IN_LIST
#fi

#ls ${IN_PATH}/${SAMP}/*.root > ${IN_LIST} 
cat ${IN_PATH}/${SAMP}.txt > ${IN_LIST} 


#NFILES=$(echo ${NTUP} | wc -w)

#echo ${NFILES}

#arr=(${NTUP})

#for i in ${arr[@]}; do echo $i; done

#echo "task ID is: ${SGE_TASK_ID}" 

#theTask=${arr[${SGE_TASK_ID}]}
#theTask=${arr[${SGE_TASK_ID}-1]} 


OUTFILE="outhist_${SAMP}"

#echo "Job is ${JOB_ID} and task is ${theTask}"

#INFILE="${IN_PATH}/${SAMP}/${theTask}"

#ls ${INFILE}

#START=$((N_FILESPERJOB*(SGE_TASK_ID-1)))
#FILELIST=${arr[@]:${START}:${N_FILESPERJOB}}

#exit 1

echo "LD_LIBRARY_PATH: ${LD_LIBRARYPATH}"
echo " User name: $USER"
echo " User home: $HOME"

echo -e "Here is the program run dir: `pwd` \n"

echo "Processing....."

mkdir -p ${OUTDIR}

echo ${FILELIST}
echo ${CONFIGNAME}

echo ./run_gbbTupleAna.exe ${IN_LIST} ${OUTDIR}/${OUTFILE} ${TREENAME} ${CONFIGNAME} 
run_gbbTupleAna ${IN_LIST} ${OUTDIR}/${OUTFILE}.root ${TREENAME} ${CONFIGNAME}

echo "Write Output File to: ${OUTDIR} ....."



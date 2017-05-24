#!/bin/bash

#
# Ruth Jacobs 10-03-16
#
# Shell script
# add all output histograms of GbbTupleAna
#
# run: source hadd_hist PATH_TO_OUTPUTFOLDER/
#
#



HOMEDIR=${PWD}

FILEDIR=${1}

if [ -z "${FILEDIR}" ] 
then
    FILEDIR=${PWD}
fi

echo "File directory:  ${FILEDIR}"

#export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
#source /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/user/atlasLocalSetup.sh
#lsetup "root 6.02.12-x86_64-slc6-gcc48-opt --quiet"


if [ -d "${FILEDIR}/user.ruth.data15_13TeV.all.root/" ]
then
    rm -r "${FILEDIR}/user.ruth.data15_13TeV.all.root/"

fi

cd ${FILEDIR}

echo $(ls -d *data*)

for dir in $(ls -d *data*)
do 
    cd ${dir}

    if [ -e "merged_hist_${dir}" ] 
    then
	rm "merged_hist_${dir}"
    fi

    echo `hadd merged_hist_${dir} *.root*`  

    if [ ! -d "${FILEDIR}/user.ruth.data15_13TeV.all.root/" ]
    then
	mkdir "${FILEDIR}/user.ruth.data15_13TeV.all.root/"

    fi

    cp "merged_hist_${dir}" "${FILEDIR}/user.ruth.data15_13TeV.all.root/"
    cd ${FILEDIR}
done



cd "${FILEDIR}/user.ruth.data15_13TeV.all.root"

if [ -e "merged_hist_user.ruth.data15_13TeV.all.root" ]
then
    rm "merged_hist_user.ruth.data15_13TeV.all.root"
fi

echo `hadd merged_hist_user.ruth.data15_13TeV.all.root *.root*`

cd ${FILEDIR}

for dir in $(ls -d *mc*)
do
    cd ${dir}
    if [ -e "merged_hist_${dir}" ]
    then
        rm "merged_hist_${dir}"
    fi
    echo `hadd merged_hist_${dir} *.root*`
    
    cd ${FILEDIR}

done


cd ${HOMEDIR}
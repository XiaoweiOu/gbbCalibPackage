#!/bin/bash

HOMEDIR=/nfs/dust/atlas/user/ruth/Ruth/QualiTask/Code/gbbTupleAna_improved

cd ${HOMEDIR}

export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/user/atlasLocalSetup.sh
lsetup "root 6.02.12-x86_64-slc6-gcc48-opt --quiet"


if [ ${DO_MCONLY} -eq 0 ]
then 
    if [ -d "${FILEDIR}/user.ruth.data1516_13TeV.all.root/" ]
    then
	rm -r "${FILEDIR}/user.ruth.data1516_13TeV.all.root/"

    fi

    if [ ! -d "${TMPDIR}/user.ruth.data1516_13TeV.all.root/" ]
    then
	mkdir "${TMPDIR}/user.ruth.data1516_13TeV.all.root/"
	
    fi


    cd ${FILEDIR}

    for dir in $(ls -d *data*)
    do 
	cd ${dir}
	echo `hadd -k ${TMPDIR}/user.ruth.data1516_13TeV.all.root/merged_hist_${dir} *.root*`  
	cd ${FILEDIR}
    done

    cd ${TMPDIR}/user.ruth.data1516_13TeV.all.root/

    echo `hadd -k merged_hist_user.ruth.data1516_13TeV.all.root *.root*`
fi

cd ${FILEDIR}

for dir in $(ls -d *mc*)
do
    cd ${dir}
    echo `hadd -k ${TMPDIR}/merged_hist_${dir} *.root*`

    cd ${FILEDIR}

done


########
#NOW DELETE OLD FILES AND COPY NEW FILES OVER
########
echo "Copying files!!!"

cp -r ${TMPDIR}/user.ruth.data1516_13TeV.all.root ${FILEDIR}

cd ${FILEDIR}
for dir in $(ls -d *mc*)
do
    cd ${dir}
    if [ -e "merged_hist_${dir}" ]
    then
        rm "merged_hist_${dir}"
    fi

    cp "${TMPDIR}/merged_hist_${dir}" .
    
    cd ${FILEDIR}

done

cd ${HOMEDIR}


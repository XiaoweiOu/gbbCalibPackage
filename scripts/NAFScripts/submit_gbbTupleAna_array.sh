#!/bin/bash

#Submit script for gbb Tuple Ana
echo "Submit jobs gbbTupleAna......"

DO_SYS=${1}

CFG_NAME=${2}

OUT_TAG=${3}

if [ -z "${DO_SYS}" ] 
    then
    DO_SYS=0
    echo "Running without systematics, to turn on, do: source scripts/submit_gbbTupleAna_array.sh 1"
    echo "To run only systematics run: source scripts/submit_gbbTupleAna_array.sh 2"
fi

INPATH="/nfs/dust/atlas/user/ruth/Ruth/QualiTask/Inputs/FTagNTuples_Calib/v00-01-04/"
#INPATH="/nfs/dust/atlas/user/ruth/Ruth/QualiTask/Inputs/FTagNtuples"
OUTPATH="/nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output_Calibration2016/Output_CrossCheck"
LOG_FOLDER="/nfs/dust/atlas/user/ruth/Ruth/QualiTask/JobLogs/"

#for now don't run over Herwig samples
listOfSamples=$(ls -d ${INPATH}/*v00-01-04* | xargs -n1 basename | grep -v "42711")
listOfMCSamples=$(ls -d ${INPATH}/*mc15*v00-01-04* | xargs -n1 basename | grep -v "42711")
#listOfSamples=$(ls -d ${INPATH}/*v00-01-03* | xargs -n1 basename)
#listOfMCSamples=$(ls -d ${INPATH}/*mc15*v00-01-03* | xargs -n1 basename)

#echo ${listOfSamples}
#echo ${listOfMCSamples}

echo "Logs in: ${LOG_FOLDER}"
echo "Output in: ${OUTPATH}"


#=========================
#Nominal
#=========================

if [ "$DO_SYS" -ne 2 ]
then
    echo "Running Nominal!!!"
    mkdir -p /nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output_Calibration2016/Output_CrossCheck

    for dir in /nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output_Calibration2016/Output_CrossCheck/${OUT_TAG}_*.root
    do
	if [ -d "${dir}" ];
	then
	    rm -rv $dir
	    #echo "directory exists: ${dir}"
	fi
    done



    for samp in ${listOfSamples}
    #for samp in ${listOfMCSamples}
    do
	echo "Sample is: $samp"
   
	NTUP=$(ls ${INPATH}/${samp})
	export NTUP
	NJOBS=$(echo ${NTUP} | wc -w)
        #NJOBS=$(expr ${NJOBS} - 1)

	echo "Number of jobs ${NJOBS}"
        #   echo "Ntup: ${NTUP}"

	qsub -t 1-${NJOBS} -o ${LOG_FOLDER} -j y -l h_rt=01:00:00,distro=sld6,h_vmem=4G -v IN_PATH="${INPATH}",SAMP="${samp}",OUTDIR="${OUTPATH}/${OUT_TAG}_${samp}",TREENAME="FlavourTagging_Nominal",CONFIGNAME="${CFG_NAME}" scripts/NAFScripts/run_gbbTupleAna_array.sh  

    done
fi

#========================
# 2-sided systematics
#========================

if [ "$DO_SYS" -gt 0 ]
then
    echo "Running 2-sided Kin systematics!!!"

    for sys in JET_Rtrk_Baseline_Kin JET_Rtrk_Modelling_Kin JET_Rtrk_Tracking_Kin JET_Rtrk_TotalStat_Kin JET_Rtrk_Baseline_Sub JET_Rtrk_Modelling_Sub JET_Rtrk_Tracking_Sub JET_Rtrk_TotalStat_Sub #JET_Rtrk_Baseline_D2 JET_Rtrk_Modelling_D2 JET_Rtrk_Tracking_D2 JET_Rtrk_TotalStat_D2
    do
	for var in __1up __1down
	do
	    mkdir -p /nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output_Calibration2016/Output_CrossCheck_${sys}$var

	    for dir in /nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output_Calibration2016/Output_CrossCheck_${sys}$var/*.root
	    do
		if [ -d "${dir}" ];
		then
		    rm -rv $dir
		fi
	    done


	    
	    for samp in ${listOfMCSamples}
	    do
		NTUP=$(ls ${INPATH}/${samp})
		export NTUP
		NJOBS=$(echo ${NTUP} | wc -w)
		#NJOBS=$(expr ${NJOBS} - 1)
		
		qsub -t 1-${NJOBS} -o ${LOG_FOLDER} -j y -l h_rt=01:00:00,distro=sld6,h_vmem=4G -v IN_PATH="${INPATH}",SAMP="${samp}",OUTDIR="${OUTPATH}_${sys}${var}/${OUT_TAG}_${samp}",TREENAME="FlavourTagging_${sys}${var}",CONFIGNAME="${CFG_NAME}" scripts/NAFScripts/run_gbbTupleAna_array.sh

	    done


	done

    done
fi



#========================                                                                                                                                                                                                                   
# 1-sided systematics                                                                                                                                                                       
#========================

if [ "$DO_SYS" -gt 0 ]
then
    echo "Running 1-sided systematics!!!"

    for sys in FATJET_JER FATJET_JMR
    do
	for var in __1up
	do
	     mkdir -p /nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output_Calibration2016/Output_CrossCheck_${sys}$var

            for dir in /nfs/dust/atlas/user/ruth/Ruth/QualiTask/Output_Calibration2016/Output_CrossCheck_${sys}$var/*.root
            do
		if [ -d "${dir}" ];
		    then
                    rm -rv $dir
		    fi
            done



            for samp in ${listOfMCSamples}
            do
		NTUP=$(ls ${INPATH}/${samp})
		export NTUP
		NJOBS=$(echo ${NTUP} | wc -w)
		#NJOBS=$(expr ${NJOBS} - 1)

		qsub -t 1-${NJOBS} -o ${LOG_FOLDER} -j y -l h_rt=01:00:00,distro=sld6,h_vmem=4G -v IN_PATH="${INPATH}",SAMP="${samp}",OUTDIR="${OUTPATH}_${sys}${var}/${OUT_TAG}_${samp}",TREENAME="FlavourTagging_${sys}${var}",CONFIGNAME="${CFG_NAME}" scripts/NAFScripts/run_gbbTupleAna_array.sh

            done


done

    done
fi

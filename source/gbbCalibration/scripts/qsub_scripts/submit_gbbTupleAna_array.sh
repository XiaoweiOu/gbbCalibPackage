#!/bin/bash

#==========================================
#------------- user settings --------------

# do you want to remove all dirrectories before running?
clean=false

# homedir
HOMEDIR=/home/stankaityte/QT/AnaFW/GbbCalibrationFW2018/gbbCalibPackage/run/

# qsub script dirrectory
SCR_DIR="../source/gbbCalibration/scripts/qsub_scripts"

# qsub job tag
jtag="hmcam"

# --- input ---
in_path_base="/data/atlas/atlasdata/stankait/gbbTuples/"

#in_type="data/"
in_type="MC/mufilter/mc16a/"
#in_type="MC/mufilter/mc16d/"
#in_type="MC/inclusive/mc16a/"
#in_type="MC/inclusive/mc16d/"

# --- output ---
# pt-eta reweighting
#out_path_base="/data/atlas/atlasdata/stankait/QT/Hists/22Jan/R21/reweight"

# calibration
#out_path_base="/data/atlas/atlasdata/stankait/QT/Hists/22Jan/R21/calibration"
out_path_base="/data/atlas/atlasdata/stankait/QT/Hists/29Jan/R21/calibration_nr"

# syslist
syslist=("JET_Comb_Baseline_Kin__1up" "JET_Comb_Baseline_Kin__1down" "JET_Comb_Modelling_Kin__1up" "JET_Comb_Modelling_Kin__1down" "JET_Comb_Tracking_Kin__1up" "JET_Comb_Tracking_Kin__1down" "JET_Comb_TotalStat_Kin__1up" "JET_Comb_TotalStat_Kin__1down" "JET_Rtrk_Baseline_Sub__1up" "JET_Rtrk_Baseline_Sub__1down" "JET_Rtrk_Modelling_Sub__1up" "JET_Rtrk_Modelling_Sub__1down" "JET_Rtrk_Tracking_Sub__1up" "JET_Rtrk_Tracking_Sub__1down" "JET_Rtrk_TotalStat_Sub__1up" "JET_Rtrk_TotalStat_Sub__1down" "FATJET_JER__1up" "JET_MassRes_Hbb__1up")

#==========================================
#                 RUNNING

#----------- input variables --------------

# systematics? 0 / 1 / 2 -- nom / nom+sys / sys
DO_SYS=${1}

# config path/name
CFG_NAME=${2}

# output tag
OUT_TAG=${3}


#-------------- dirrectorries --------------

# input
INPATH="${in_path_base}/${in_type}"
listOfSamples=$(ls -d ${INPATH}/*DAOD_FTAG1.FTNtupCalib* | xargs -n1 basename )

# output
OUTPATH="${out_path_base}/${OUT_TAG}"
LIST_FOLDER="${out_path_base}/${OUT_TAG}/lists/"
LOG_FOLDER="${out_path_base}/${OUT_TAG}/logs/"


#---------------- salute ------------------

echo "- - - - - - - - - - - - - - - - - - - - - - "
echo "Submitting jobs to qsub:"
echo
echo "Rewiriting? -- $clean "
echo

# systematics explanation
if [ "${DO_SYS}" -eq 0 ] 
then
  echo "Running nominal only (setting ${DO_SYS})"
  echo
elif [ "${DO_SYS}" -eq 1 ] 
then
  echo "Running nominal + systematics (setting ${DO_SYS})"
  echo
else
  echo "Running systematics only (setting ${DO_SYS})"
  echo
fi

# outputs
echo "Logs in: ${LOG_FOLDER}"
echo "Lists in: ${LIST_FOLDER}"
echo "Output in: ${OUTPATH}"
echo


#==========================================
#                SUBMITTING

#------------ setting run mode ---------------

if [ "$DO_SYS" -eq 0 ]
then
  varlist=("Nominal")
elif [ "$DO_SYS" -eq 1 ]
then 
  varlist=("Nominal" "${syslist[@]}")
else
  varlist=("${syslist[@]}")
fi

echo "Varlist: ${varlist[@]}"
echo

#----------------- submitting ----------------

for var in ${varlist[@]}
do
  
  # making dirrectories
  mkdir -p ${OUTPATH}/${var}
  mkdir -p ${LIST_FOLDER}/${var}
  mkdir -p ${LOG_FOLDER}/${var}
  
  echo "-----------------------------------"
  echo "--- Variation: ${var}  "

  #---------
  for samp in ${listOfSamples}
  do
    # sample
    echo "Sample is: $samp"
    
    # overwriting
    if [ "$clean" = true ]
    then
      echo "Removing existing dirrectories" 
      for dir in ${OUTPATH}/${var}/${samp}
      do
        if [ -d "${dir}" ];
        then
          rm -rv $dir
          echo "directory exists: ${dir}"
        fi
      done
    fi
      
    # --- does output alredy exist? ---
    # look for dirrectory
    if [ -d "$OUTPATH/${var}/${samp}" ];
    then
      echo "Sample dirrectory already exists."
      if [ -z "$(ls -A "${OUTPATH}/${var}/${samp}")" ];
      then
        echo "No output file! -- resubmitting:"
        qsub -o "${LOG_FOLDER}/${var}/${samp}" -j oe -l cput=12:00:00 -q short -N "${jtag}_${var}" -v LISTDIR="${LIST_FOLDER}",HOMEDIR="${HOMEDIR}",IN_PATH="${INPATH}",SAMP="${samp}",OUTDIR="${OUTPATH}/${var}/${samp}",TREENAME="FlavourTagging_${var}",CONFIGNAME="${CFG_NAME}",N_FILESPERJOB="${NFILESPERJOB}" $SCR_DIR/run_gbbTupleAna_arrayList.sh
      fi
    else
      qsub -o "${LOG_FOLDER}/${var}/${samp}" -j oe -l cput=12:00:00 -q short -N "${jtag}_${var}" -v LISTDIR="${LIST_FOLDER}",HOMEDIR="${HOMEDIR}",IN_PATH="${INPATH}",SAMP="${samp}",OUTDIR="${OUTPATH}/${var}/${samp}",TREENAME="FlavourTagging_${var}",CONFIGNAME="${CFG_NAME}",N_FILESPERJOB="${NFILESPERJOB}" $SCR_DIR/run_gbbTupleAna_arrayList.sh
    fi

  done
  #---------

done


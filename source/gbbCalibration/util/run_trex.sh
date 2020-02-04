#!/bin/bash

function loud_exit {
  echo "ERROR: $1"
  exit
}

function usage {
  echo "Usage: $(basename ${BASH_SOURCE[0]}) -i <input> -o <output> [-h] [-e <args>]"
  echo "       -o <output> Name of directory to store TRExFitter results"
  echo "       -i <input>  Name of histogram file for TRExFitter input"
  echo "       -h          Print this help message"
  echo "       -c          Name of config file (located in data/configs)"
  echo "       -e          Additional arguments to be passed directly"
  echo "                   to generate_trex_config.py"
}

INFILE=
DIRNAME=
CONFIGARGS=
CONFIGNAME=
while getopts 'hi:o:e:c:' OPTION; do
  case "$OPTION" in
    i)
      INFILE="$OPTARG"
      ;;
    o)
      DIRNAME="$OPTARG"
      ;;
    e)
      CONFIGARGS="$OPTARG"
      ;;
    c)
      CONFIGNAME="--cfg $OPTARG"
      ;;
    h)
      usage
      exit 0
      ;;
    :)
      echo "Invalid option: $OPTARG requires an argument"
      usage
      exit 2
      ;;
    ?)
      echo "Invalid option"
      usage
      exit 2
      ;;
  esac
done
shift "$(($OPTIND -1))"

# check input arguments
[ -z $INFILE ] && loud_exit "Input file not set"
[ -z $DIRNAME ] && loud_exit "Output directory not set"
[ -d ${DIRNAME} ] && loud_exit "${DIRNAME} exists already"

mkdir -p "${DIRNAME}"
cp ${INFILE} ${DIRNAME}/trex_input.root

# make TRExFitter config files
python ${GBB_BUILD_DIR}/../source/gbbCalibration/python/generate_trex_config.py ${DIRNAME} ${CONFIGARGS} ${CONFIGNAME}
[ $? -eq 0 ] || loud_exit "Failed to make config files"

CONFIGFILES=(`ls ${DIRNAME}`)
for _file in ${CONFIGFILES[@]}; do
  # check that the file is a regular file and ends with .config
  if [ -f ${DIRNAME}/${_file} ] && [[ $_file == *.config ]]; then
    # runs fit and produces pre- and post-fit histograms and ranking plots
    trex-fitter hwfr "${DIRNAME}/${_file}"
  fi
done


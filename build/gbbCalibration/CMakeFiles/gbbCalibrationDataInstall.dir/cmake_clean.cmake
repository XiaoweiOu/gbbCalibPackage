file(REMOVE_RECURSE
  "../generic/include/helpers"
  "../generic/include/histMaker"
  "../generic/include/scaleFactorCalculator"
  "../generic/bin/run_trex.sh"
  "../generic/python/gbbCalibration/makeReweightHistos.py"
  "../generic/python/gbbCalibration/makeReweightHistos.pyc"
  "CMakeFiles/makeReweightHistospycGen.py"
  "../generic/data/gbbCalibration/PMGxsecDB_mc16_2020-03-09.txt"
  "../generic/data/gbbCalibration/SampleInfo_MuonFilterTest.txt"
  "../generic/data/gbbCalibration/configs"
  "../generic/data/gbbCalibration/examples"
  "../generic/data/gbbCalibration/PMGxsecDB_mc16_2020-03-09.txt"
  "../generic/data/gbbCalibration/SampleInfo_MuonFilterTest.txt"
  "../generic/data/gbbCalibration/configs"
  "../generic/data/gbbCalibration/examples"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/gbbCalibrationDataInstall.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()

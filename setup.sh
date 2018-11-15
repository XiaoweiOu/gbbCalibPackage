setupATLAS
cd source
if [ -f ".asetup.save" ]; then
  asetup --restore
else
  asetup AnalysisBase,21.2.35,here
fi
cd ..
source build/${AnalysisBase_PLATFORM}/setup.sh

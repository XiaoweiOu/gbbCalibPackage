################################################################################
# Build configuration for gbbCalibration
################################################################################

# Declare the name of the package:
atlas_subdir( gbbCalibration )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC 
   Tools/PathResolver
   )

find_package( ROOT COMPONENTS Core MathCore RIO Hist Tree Matrix Gpad Graf Physics Minuit)

# Build a dictionary for the library:
atlas_add_root_dictionary( gbbTupleLib _dictionarySource
   ROOT_HEADERS gbbCalibration/TupleAna.h gbbCalibration/GbbTupleAna.h Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

atlas_add_library( gbbTupleLib
   Root/GbbTupleAna.cxx Root/GbbSelectionHelp.cxx Root/GbbFillHelp.cxx Root/TupleAna.cxx
   Root/HistogramService.cxx Root/FlavourFracCorrector.cxx
   gbbCalibration/GbbTupleAna.h gbbCalibration/TupleAna.h gbbCalibration/HistogramService.h gbbCalibration/FlavourFracCorrector.h
   Root/*.h ${_dictionarySource}
   PUBLIC_HEADERS GbbTupleAna
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} PathResolver
   )
atlas_add_library( ScaleFactorCalcLib
   Root/ScaleFactorCalculator.cxx Root/FitData.cxx Root/Fitter.cxx Root/Config.cxx
   gbbCalibration/ScaleFactorCalculator.h gbbCalibration/FitData.h gbbCalibration/Fitter.h gbbCalibration/Config.h
   PUBLIC_HEADERS ScaleFactorCalc
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} PathResolver
   )

# Install files from the package:
atlas_add_executable( run_gbbTupleAna
   util/run_gbbTupleAna.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} gbbTupleLib
   )
atlas_add_executable( run_calculateSF
   util/run_calculateSF.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ScaleFactorCalcLib
   )

atlas_install_python_modules( python/*.py )
atlas_install_data( data/* )

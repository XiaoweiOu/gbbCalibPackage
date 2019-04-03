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

# TODO: can't find boost unit test in atlas
#find_package( Boost COMPONENTS boost_unit_test_framework)
#IF (Boost_FOUND)
#    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
#    ADD_DEFINITIONS( "-DHAS_BOOST" )
#ENDIF()

# Build a dictionary for the library:
atlas_add_root_dictionary( gbbTupleLib _gbbDictSource
   ROOT_HEADERS histMaker/TupleAna.h histMaker/GbbTupleAna.h histMaker/LinkDef.h
   EXTERNAL_PACKAGES ROOT )
atlas_add_root_dictionary( configLib _configDictSource
   ROOT_HEADERS helpers/GlobalConfig.h helpers/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

atlas_add_library( configLib
   helpers/GlobalConfig.cxx helpers/GlobalConfig.h
   ${_configDictSource}
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} PathResolver
   )
atlas_add_library( gbbTupleLib
   histMaker/*.cxx histMaker/*.h
   histMaker/BTagging/*.cxx histMaker/BTagging/*.h
   helpers/*.cxx helpers/*.h
   ${_gbbDictSource}
   PUBLIC_HEADERS GbbTupleAna
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} PathResolver
   )
atlas_add_library( ScaleFactorCalcLib
   scaleFactorCalculator/*.cxx scaleFactorCalculator/*.h
   helpers/*.cxx helpers/*.h  helpers/atlasstyle/*.C helpers/atlasstyle/*.h
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

 # add a test exe
 atlas_add_executable( run_GbbTest
   tests/run_tests.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} gbbTupleLib
   )

 # add boost test suite; TODO boost not found yet
# atlas_add_executable( run_GbbBoostTest
#   GbbTests/TestRunner.cxx
#   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
#   LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} gbbTupleLib
#   )

atlas_install_python_modules( python/makeReweightHistos.py LINK_LIBRARIES configLib )
#atlas_install_python_modules( python/*.py )
atlas_install_data( data/* )

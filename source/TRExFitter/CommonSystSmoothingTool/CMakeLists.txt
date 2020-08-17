#
# Build configuration for the SmoothSystematics package
#

# Set the name of the package:
atlas_subdir( CommonSystSmoothingTool )

# Set up which packages this package depends on:
atlas_depends_on_subdirs(
   PUBLIC
   PRIVATE
)

# External(s) used by the package:
find_package( ROOT COMPONENTS Core Hist RIO Tree Gpad )

# # Generate a dictionary for the library:
 atlas_add_root_dictionary( CommonSystSmoothingToolLib CommonSystSmoothingToolLibCintDict
   ROOT_HEADERS  SmoothSystematics/*.h Root/LinkDef.h 
    EXTERNAL_PACKAGES ROOT 
    )

# Build the shared library of the package:
atlas_add_library( CommonSystSmoothingToolLib SmoothSystematics/*.h Root/*.cxx Root/LinkDef.h ${CommonSystSmoothingToolLibCintDict}
   PUBLIC_HEADERS SmoothSystematics 
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
   LINK_LIBRARIES ${ROOT_LIBRARIES} 
)


# Build the executables of the package:
atlas_add_executable(testSmooth util/testSmooth.cxx
                     LINK_LIBRARIES CommonSystSmoothingToolLib )

atlas_add_executable(testInputFile util/testInputFile.cxx 
                     LINK_LIBRARIES CommonSystSmoothingToolLib  )
# Install the script(s) of the package:
atlas_install_scripts( python/*.py scripts/* )

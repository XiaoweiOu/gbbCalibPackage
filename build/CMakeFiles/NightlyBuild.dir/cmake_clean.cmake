file(REMOVE_RECURSE
  "generic/setup.sh"
  "generic/lib/WorkDir.rootmap"
  "CMakeFiles/NightlyBuild"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/NightlyBuild.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()

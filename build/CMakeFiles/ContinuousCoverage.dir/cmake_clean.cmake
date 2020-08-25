file(REMOVE_RECURSE
  "generic/setup.sh"
  "generic/lib/WorkDir.rootmap"
  "CMakeFiles/ContinuousCoverage"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/ContinuousCoverage.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()

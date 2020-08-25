file(REMOVE_RECURSE
  "../../generic/include/SmoothSystematics"
  "../../generic/bin/CopyHists.py"
  "../../generic/bin/Ratio.py"
  "../../generic/bin/testInputFile.py"
  "../../generic/bin/testSmooth.py"
  "../../generic/include/SmoothSystematics"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/CommonSystSmoothingToolHeaderInstall.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()

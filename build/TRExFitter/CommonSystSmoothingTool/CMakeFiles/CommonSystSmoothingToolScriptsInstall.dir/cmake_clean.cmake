file(REMOVE_RECURSE
  "../../generic/include/SmoothSystematics"
  "../../generic/bin/CopyHists.py"
  "../../generic/bin/Ratio.py"
  "../../generic/bin/testInputFile.py"
  "../../generic/bin/testSmooth.py"
  "../../generic/bin/CopyHists.py"
  "../../generic/bin/Ratio.py"
  "../../generic/bin/testInputFile.py"
  "../../generic/bin/testSmooth.py"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/CommonSystSmoothingToolScriptsInstall.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()

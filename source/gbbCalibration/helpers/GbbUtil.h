#ifndef GBBUTIL_H_
#define GBBUTIL_H_

#include <vector>
#include "TString.h"

namespace GbbUtil {

  // expand env vars in filename
  TString expandFilename(const char* filename);
  // prepend path to config files before expanding
  TString expandConfigFilename(const char* filename);
  // use PathResolver to find file
  TString findConfigFile(const TString filename);

  std::vector<TString> splitString(TString str, char delim);
  std::vector<float> splitStringD(TString str, char delim);
}
#endif

#ifndef GBBUTIL_H_
#define GBBUTIL_H_

#include <map>
#include <vector>
#include <string>
#include "TString.h"

namespace GbbUtil {

  // expand env vars in filename
  std::string expandFilename(const char* filename);
  // prepend path to config files before expanding
  std::string expandConfigFilename(const char* filename);
  // use PathResolver to find file
  TString findConfigFile(const TString filename);

  // split s into multiple parts with delim
  std::vector<std::string> splitString(std::string str, std::string delimiter);
  std::vector<std::string> splitStringD(std::string str, std::string delimiter);

  // split each string in vector to a pair using delimiter
  std::map<std::string,std::string> splitEach
    (std::vector<std::string> &v, std::string delimiter);

  // chaining the first 2;
  // split with dilimiter1 of s; then split each element with delimiter2
  // mainly used to get config settings
  std::map<std::string,std::string> splitWholeThenEach
    (std::string s, std::string delimiter1, std::string delimiter2);

  std::vector<TString> SplitString(TString str, char delim);
  std::vector<float> SplitStringD(TString str, char delim);
}
#endif

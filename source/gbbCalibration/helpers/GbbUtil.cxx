#include "GbbUtil.h"
#include <wordexp.h>
#include <iostream>

#include "TObjString.h"
#include "TObjArray.h"

#include "PathResolver/PathResolver.h"

namespace GbbUtil {

  TString expandFilename(const char* filename) {
    // Use wordexp to expand env vars in filename
    wordexp_t wexp{};
    int code = wordexp(filename, &wexp, WRDE_SHOWERR | WRDE_UNDEF | WRDE_NOCMD);
    if (code) {
      switch (code) {
      case WRDE_BADCHAR:
        throw "Invalid character in file name";
        break;
      case WRDE_BADVAL:
        throw "Undefined variable in file name";
        break;
      case WRDE_CMDSUB:
        throw "Command substitution in file name";
        break;
      case WRDE_NOSPACE:
        throw "No space";
        break;
      case WRDE_SYNTAX:
        throw "Syntax error in file name";
        break;
      }
      exit(2);
    }

    if (wexp.we_wordc != 1) {
        throw "There must be exactly one filename";
        exit(2);
    }
    return wexp.we_wordv[0];
  }

  TString expandConfigFilename(const char* filename) {
    TString config_path = "${GBB_BUILD_DIR}/../source/gbbCalibration/data/configs/";
    try {
      return expandFilename((config_path.Append(filename)).Data());
    } catch (const char* exc) {
      std::cerr << exc << std::endl;
      exit(2);
    }
  }

  TString findConfigFile(const TString filename) {
    return PathResolverFindCalibFile(("gbbCalibration/configs/"+filename).Data());
  }

  std::vector<TString> splitString(TString str, char delim){

    std::vector<TString> tokens;
    TObjArray *Strings=str.Tokenize(delim);

    for(int i=0; i<Strings->GetEntriesFast(); i++){
      tokens.push_back(((TObjString*) (*Strings)[i])->GetString());
    }

    return tokens;
  }

  std::vector<float> splitStringD(TString str, char delim){

    std::vector<float> tokens;
    TObjArray *Strings=str.Tokenize(delim);

    for(int i=0; i<Strings->GetEntriesFast(); i++){
      tokens.push_back((((TObjString*) (*Strings)[i])->GetString()).Atof());
      //std::cout<<"Token: "<<((TObjString*) (*Strings)[i])->GetString()<<" vs "<<tokens[i]<<std::endl;
    }

    return tokens;
  }

}

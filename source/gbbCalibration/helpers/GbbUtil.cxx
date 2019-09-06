#include "GbbUtil.h"
#include <string>
#include <memory>

#include <iostream>

#include <boost/algorithm/string.hpp> // for trimming
#include <wordexp.h>

namespace GbbUtil {

  std::string expandFilename(const char* filename) {
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

  std::string expandConfigFilename(const char* filename) {
    std::string config_path = "${GBB_BUILD_DIR}/../source/gbbCalibration/data/configs/";
    try {
      return expandFilename((config_path.append(filename)).data());
    } catch (const char* exc) {
      std::cerr << exc << std::endl;
      exit(2);
    }
  }

  std::vector<std::string> splitString(std::string str, std::string delimiter){
    std::string s = str;
    size_t pos = 0;
    std::string token;
    std::vector<std::string> res;
    while ((pos = s.find(delimiter)) != std::string::npos) {
      token = s.substr(0, pos);
      // remove white spaces
      boost::algorithm::trim(token);
      res.push_back(token);
      // std::cout <<"!"<< token << "!"<< std::endl;
      s.erase(0, pos + delimiter.length());
    }
    boost::algorithm::trim(s);
    res.push_back(s); // the last piece
    return res;
  }


  std::map<std::string,std::string> splitEach
  (std::vector<std::string> &v, std::string delimiter){
    std::map<std::string,std::string> res;
    for (std::string s : v){
      // split s into 2 parts
      std::vector<std::string> pair = splitString(s,delimiter);
      if(pair.size() != 2 ){
        throw "not 2";
      }
      res[pair[0]]= pair[1];
    }

    return res;
  }

  std::map<std::string,std::string> splitWholeThenEach
  (std::string s, std::string delimiter1, std::string delimiter2){
    std::vector<std::string> vp = splitString(s,delimiter1);

    return splitEach(vp,delimiter2);
  }

}

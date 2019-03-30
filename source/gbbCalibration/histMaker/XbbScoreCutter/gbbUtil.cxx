

#include "gbbUtil.h"
#include <string>
#include <memory>

#include <iostream>

#include <boost/algorithm/string.hpp> // for trimming

std::vector<std::string> GbbUtil::splitString(std::string str, std::string delimiter){
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


std::map<std::string,std::string> GbbUtil::splitEach
(std::vector<std::string> &v, std::string delimiter){
  std::map<std::string,std::string> res;
  for (std::string s : v){
    // split s into 2 parts
    std::vector<std::string> pair = GbbUtil::splitString(s,delimiter);
    if(pair.size() != 2 ){
      throw "not 2";
    }
    res[pair[0]]= pair[1];
  }

  return res;
}

std::map<std::string,std::string> GbbUtil::splitWholeThenEach
(std::string s, std::string delimiter1, std::string delimiter2){
  std::vector<std::string> vp = GbbUtil::splitString(s,delimiter1);

  return GbbUtil::splitEach(vp,delimiter2);
}


#ifndef GBBUTIL_H_
#define GBBUTIL_H_

#include <map>
#include <vector>

class GbbUtil {

 public:
  // split s into multiple parts with delim
  static std::vector<std::string> splitString(std::string str, std::string delimiter);

  // split each string in vector to a pair using delimiter
  static std::map<std::string,std::string> splitEach
    (std::vector<std::string> &v, std::string delimiter);

  // chaining the first 2;
  // split with dilimiter1 of s; then split each element with delimiter2
  // mainly used to get config settings
  static std::map<std::string,std::string> splitWholeThenEach
    (std::string s, std::string delimiter1, std::string delimiter2);

};
#endif

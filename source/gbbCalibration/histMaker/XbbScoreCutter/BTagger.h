
/* this objects selects the correct tagger from config 
   and preforms tagging
*/

#ifndef BTAGGER_H_
#define BTAGGER_H_

#include "XbbScoreCutter.h"
// #include <memory> // for unique pointer
#include <map>
#include <string>

class BTagger {

 public:

  // possible cutters
  XbbScoreCutter* xbbScoreCutter_;
  std::string configString_;
  // tagger=XbbScore|f=0.2|eff=60
  std::map<std::string,std::string>* config_;

  
  BTagger() = delete;

  // construct with config String
  // configString format:
  // MV2c10_FixedCutBEff_70_R20p7
  // XbbScore|f=0.2|eff=60
  BTagger(std::string configString);

  ~BTagger(){
    delete xbbScoreCutter_;
    delete config_;
  }

 private:
  void readConfigString(std::string configString);

  
  
};


#endif /*BTAGGER_H_*/

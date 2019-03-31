
/* this objects selects the correct tagger from config 
   and preforms tagging.

   example:
   1. For XbbScore Tagger
   The config string is tagger=XbbScore|f=<0.2>|eff=<60>
   BTagger("tagger=XbbScore|f=0.2|eff=60", false);

   For hybrid XbbScore ues: "tagger=XbbScoreHybrid|eff=60|f=0.2"
   only eff=60 and f=0.2 or f=0 is supported

   2. For MV2c10 Tagger
   config String is : "tagger=MV2c10|eff=60"

   3. For MV2c10R20.7
   "tagger=MV2c10R20.7|eff=70"

   4. TODO: not yet implemented
   "tagger=<>|cutValue=<>"
   
   
*/

#ifndef BTAGGER_H_
#define BTAGGER_H_

#include "XbbScoreCutter.h"
#include "XbbScoreHybridCutter.h"
#include "MV2c10Cutter.h"
// #include <memory> // for unique pointer
#include <map>
#include <string>
#include "../GbbTupleAna.h" // for gbbcandidate

class BTagger {

 public:

  
  //std::string configString_;
  bool useVRTrkJets_;
  // tagger=XbbScore|f=0.2|eff=60
  std::map<std::string,std::string> config_;
  
  // possible cutters
  XbbScoreCutter* xbbScoreCutter_;
  MV2c10Cutter* mV2c10Cutter_;
  XbbScoreHybridCutter* xbbScoreHybridCutter_;
  
  
  BTagger() = delete;
  // construct with config String
  // configString format:
  // MV2c10_FixedCutBEff_70_R20p7
  // XbbScore|f=0.2|eff=60
  BTagger(std::string configString, bool useVRTrkJets);

  // tag this gbb candidate and return number of b tagged
  int tag(const GbbTupleAna& gbbtuple, const GbbCandidate& gbbcand);

  ~BTagger(){
    delete xbbScoreCutter_;
    delete mV2c10Cutter_;
    delete xbbScoreHybridCutter_;
    // delete config_;
  }

  std::string getTaggerType();
  std::string getEff();

 private:
  void throwException();
  
  
  
};


#endif /*BTAGGER_H_*/

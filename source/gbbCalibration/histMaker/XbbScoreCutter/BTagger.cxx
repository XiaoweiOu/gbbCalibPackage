
#include "BTagger.h"
#include <memory> // for unique pointer
#include <string>
#include "gbbUtil.h"
#include "MV2c10Cutter.h"

BTagger::BTagger(std::string configString, bool useVRTrkJets):
  configString_(configString), useVRTrkJets_(useVRTrkJets),
  xbbScoreCutter_(nullptr), mV2c10Cutter_(nullptr) {

  // parse the config string and populate the config_ map
  this->readConfigString(configString);

  // select tagger and constructs cutter according to config_ map
  if((*config_)["tagger"] == "XbbScore"){
    float f = ::atof((*config_)["f"].c_str());
    int eff = ::atoi((*config_)["eff"].c_str());
    // check is number or not.. todo..
    this -> xbbScoreCutter_ = new XbbScoreCutter(f,eff);
  }else if((*config_)["tagger"] == "MV2c10"){
    // to do check error
    int eff = ::atoi((*config_)["eff"].c_str());

    this -> mV2c10Cutter_ = new MV2c10Cutter(eff,useVRTrkJets);
  }else{

    throw std::invalid_argument( "non recognized tagger type: " +
				 (*config_)["tagger"] + "/n" +
				 "check your config string for btagging");
  }
  
}


void BTagger::readConfigString(std::string configString){
  // extract taggertype f and eff value
  // tagger=XbbScore|f=0.2|eff=60
  // split in with | and then with = get
  this->config_ = GbbUtil::splitWholeThenEach(configString,"|","=");
  
}


// really bad chaining, fix need to restructure gbbpackage
// GbbTupleAna has all the data.
// return 1 if tagged, 0 if not
// todo: fix the return value for both taggers.
int BTagger::tag(const GbbTupleAna& gbbtuple, const GbbCandidate& gbbcand){
  // match each cutter and cut
  if((*config_)["tagger"] == "XbbScore"){
    // read xbbscore parameters, then use xbbcutter to determine
    // if this gbb candidate is b tagged.
    float p_h = gbbtuple.fat_XbbScoreHiggs->at(gbbcand.fat_index);
    float p_qcd = gbbtuple.fat_XbbScoreQCD->at(gbbcand.fat_index);
    float p_top = gbbtuple.fat_XbbScoreTop->at(gbbcand.fat_index);
    int isTagged = this->xbbScoreCutter_->cut(p_h,p_qcd,p_top);
    return isTagged;
  }

  if((*config_)["tagger"] == "MV2C10"){
    float muvalue= gbbtuple.trkjet_MV2c10->at(gbbcand.muojet_index);
    float nonmuvalue= gbbtuple.trkjet_MV2c10->at(gbbcand.nonmuojet_index);
    return this->mV2c10Cutter_ -> cut(muvalue, nonmuvalue);
  }

  
  throw std::invalid_argument( "non recognized tagger type: " +
				 (*config_)["tagger"] + "/n" +
				 "check your config string for btagging");
}



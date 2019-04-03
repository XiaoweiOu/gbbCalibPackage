
#include "BTagger.h"
// #include <memory> // for unique pointer
#include <string>
#include "gbbUtil.h"
#include "MV2c10Cutter.h"
#include "XbbScoreHybridCutter.h"

BTagger::BTagger(std::string configString, bool useVRTrkJets):
  useVRTrkJets_(useVRTrkJets),
  xbbScoreCutter_(nullptr), mV2c10Cutter_(nullptr),xbbScoreHybridCutter_(nullptr) {

  // parse the config string and populate the config_ map with | and =
  // example: f=0.2|eff=60
  this->config_ = GbbUtil::splitWholeThenEach(configString,"|","=");

  // select tagger and constructs cutter according to config_ map
  if(this->getTaggerType() == "XbbScore"){
    // convert string to numerals (handle exceptions)
    float f = std::stof(config_["f"]);
    int eff = std::stoi(config_["eff"],nullptr);
    
    this -> xbbScoreCutter_ = new XbbScoreCutter(f,eff);
  }else if(this->getTaggerType() == "MV2c10"){
    
    int eff = std::stoi(config_["eff"]);

    this -> mV2c10Cutter_ = new MV2c10Cutter(eff,useVRTrkJets);

  }else if (this->getTaggerType() == "MV2c10R20.7"){
    // todo?? : input cutvalue
    // directly construct with cutvalue
    this -> mV2c10Cutter_ = new MV2c10Cutter(0.6455);
    
  }else if (this->getTaggerType() == "XbbScoreHybrid"){

    float f = std::stof(config_["f"]);
    int eff = std::stoi(config_["eff"],nullptr);
    // only support these values
    // std::cout << f << " " << eff << std::endl;
    if( (f != float(0.2) && f != float(0)) || eff != 60){
      throwException();
    }
    
    this -> xbbScoreHybridCutter_ = new XbbScoreHybridCutter(f);
    
  }else{
    throwException();
  }
  
}



std::string BTagger::getTaggerType(){
  return this->config_["tagger"];
}

std::string BTagger::getEff(){
  return this->config_["eff"];
}


// really bad chaining, fix need to restructure gbbpackage
// GbbTupleAna has all the data.
// return 1 if tagged, 0 if not
// todo: fix the return value for both taggers.
int BTagger::tag(const TupleAna& gbbtuple, const GbbCandidate& gbbcand){
  // match each cutter and cut
  if(this->getTaggerType() == "XbbScore"){
    // read xbbscore parameters, then use xbbcutter to determine
    // if this gbb candidate is b tagged.
    float p_h = gbbtuple.fat_XbbScoreHiggs->at(gbbcand.fat_index);
    float p_qcd = gbbtuple.fat_XbbScoreQCD->at(gbbcand.fat_index);
    float p_top = gbbtuple.fat_XbbScoreTop->at(gbbcand.fat_index);
    return this->xbbScoreCutter_->cut(p_h,p_qcd,p_top);
  }

  if(this->getTaggerType() == "MV2c10"){
    float muvalue= gbbtuple.trkjet_MV2c10->at(gbbcand.muojet_index);
    float nonmuvalue= gbbtuple.trkjet_MV2c10->at(gbbcand.nonmuojet_index);
    return this->mV2c10Cutter_ -> cut(muvalue, nonmuvalue);
  }

  if(this->getTaggerType() == "XbbScoreHybrid"){
    float p_h = gbbtuple.fat_XbbScoreHiggs->at(gbbcand.fat_index);
    float p_qcd = gbbtuple.fat_XbbScoreQCD->at(gbbcand.fat_index);
    float p_top = gbbtuple.fat_XbbScoreTop->at(gbbcand.fat_index);
    float fat_pt_val = gbbtuple.fat_pt->at(gbbcand.fat_index);
    return this->xbbScoreHybridCutter_->cut(p_h,p_qcd,p_top,fat_pt_val);
  }
  
  throwException();
  return -99;
}


void BTagger::throwException(){
   throw std::invalid_argument("Btagging config error. \n Check you config String");
}

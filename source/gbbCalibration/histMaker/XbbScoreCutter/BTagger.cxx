
#include "BTagger.h"
#include <memory> // for unique pointer
#include <string>
#include "gbbUtil.h"

BTagger::BTagger(std::string configString):
  configString_(configString), xbbScoreCutter_(nullptr){

  this->readConfigString(configString);

  if((*config_)["tagger"] == "XbbScore"){
    float f = ::atof((*config_)["f"].c_str());
    int eff = ::atoi((*config_)["eff"].c_str());
    // check is number or not.. todo..
    this -> xbbScoreCutter_ = new XbbScoreCutter(f,eff);
  }
  
}


void BTagger::readConfigString(std::string configString){
  // extract taggertype f and eff value
  // tagger=XbbScore|f=0.2|eff=60
  // split in with | and then with = get
  this->config_ = GbbUtil::splitWholeThenEach(configString,"|","=");
  
}






#include "XbbScoreCutter.h"
#include <algorithm> // use vector find

XbbScoreCutter::XbbScoreCutter(float f, int eff): f_(f),eff_(eff){
  // check for valid input
  if(! (std::find(validFs.begin(), validFs.end(), f) != validFs.end())) {
    /* v does not contain f */
    // throw
    throw std::invalid_argument( "invalid input f: " + std::to_string(f) );
  }

  if(! (std::find(validEffs.begin(), validEffs.end(), eff) != validEffs.end())) {
    /* v does not contain Eff */
    // throw
    throw std::invalid_argument( "invalid input eff: " + std::to_string(eff) );
  }

  // initialize
  this->map_ = std::unique_ptr<std::map<int,std::map<float,float>>>(this->loadStaticMap());

  this->cutValue_ = getCutValue(f_, eff_);
}

XbbScoreCutter::~XbbScoreCutter(){

}

float XbbScoreCutter::getCutValue(int eff, float f){
  return (*(this -> map_)) [eff][f];
}

std::map<int,std::map<float,float>>* XbbScoreCutter::loadStaticMap(){
  // this map represents the table in the documentation
  // eff -> f -> cut value
  std::map<int,std::map<float,float>>* constMap
    = new std::map<int,std::map<float,float>> {
    {50, {{0, 5.1}, {0.2, 4.5}, {1, 3.6}} },
    {60, {{0, 4.8}, {0.2, 3.9}, {1, 3.0}} },
    {70, {{0, 3.9}, {0.2, 3.0}, {1, 2.1}} },
  };

  return constMap;
}


// need p_h, p_qcd, p_top
// int XbbScoreCutter::cut(const GbbCandidate& gbbcand) {
// p_h = higgs
int XbbScoreCutter::cut(float p_h, float p_qcd, float p_top) {
  
  float taggerVar = -999.; // D in the formula

  // calculate from the formula D given in the doc
  taggerVar = std::log( p_h / ( (1 - f_)*p_qcd + f_ * p_top) );

  // compare with cutValue and decided if it is btagged
  if(taggerVar > cutValue_) {
    // Double-b-tagged
    return 1;
  } else {
    // Not double-b-tagged
    return -1;
  }
}





#include "XbbScoreCutter.h"

XbbScoreCutter::XbbScoreCutter(float f, int eff): f_(f){
  // check for valid input
  this -> checkInput(f,eff);
  // set cut value
  this->cutValue_ = getCutValue(f, eff);
}

XbbScoreCutter::~XbbScoreCutter(){

}

// check if the input value is valid
void XbbScoreCutter::checkInput(float f, int eff){

  // check eff
  if(map_.count(eff) == 0){
  /* v does not contain Eff */
    // throw
    throw std::invalid_argument( "invalid input eff: " + std::to_string(eff) );
  }
  
  // check f, read the first inner map
  if(map_[50].count(f) == 0){
    /* v does not contain f */
    // throw
    throw std::invalid_argument( "invalid input f : " + std::to_string(f) );
  }



}



float XbbScoreCutter::getCutValue(float f, int eff){
  return this -> map_[eff][f];
}


// need p_h, p_qcd, p_top
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


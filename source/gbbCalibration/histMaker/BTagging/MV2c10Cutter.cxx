
#include "MV2c10Cutter.h"

#include <map>
#include <stdexcept>

MV2c10Cutter::MV2c10Cutter(int eff, bool useVRTrkJets): eff_(eff){
  // find eff in map 
  if ( this->cutValues_.find(eff) == cutValues_.end() ) {
    // not found
    throw std::invalid_argument( "Infalid eff in MV2c10Cutter " + eff );
  }

  if(useVRTrkJets){
    this->cutValue_ = this -> cutValuesVR_[eff]; 
  }else{
    this->cutValue_ = this -> cutValues_[eff];
  }
  
}

// directly set cut value
MV2c10Cutter::MV2c10Cutter(float cutValue): cutValue_(cutValue){}


int MV2c10Cutter::cut(float muvalue, float nonmuvalue){

  int count = 0;
  if(muvalue > this->cutValue_){
    count++;
  }

  if(nonmuvalue > this->cutValue_){
    count++;
  }

  // for back compatibility return 1 is 2 btagged; 0 is 1 tagged; -1 is 0 tagged
  return count - 1;
}

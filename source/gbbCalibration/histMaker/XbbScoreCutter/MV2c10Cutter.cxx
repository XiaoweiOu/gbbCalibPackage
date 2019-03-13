
#include "MV2c10Cutter.h"

#include <map>
#include <stdexcept>

MV2c10Cutter::MV2c10Cutter(int eff, bool useVRTrkJets):
  useVRTrkJets_(useVRTrkJets), eff_(eff){
  // find eff in map 
  if ( this->cutValue_.find(eff) == cutValue_.end() ) {
    // not found
    throw std::invalid_argument( "Infalid eff in MV2c10Cutter " + eff );
  }

  if(useVRTrkJets){
    this->taggerCut_ = this -> cutValueVR_[eff]; 
  }else{
    this->taggerCut_ = this -> cutValue_[eff];
  }
  
}


int MV2c10Cutter::cut(float muvalue, float nonmuvalue){

  int count = 0;
  if(muvalue > this->taggerCut_){
    count++;
  }

  if(nonmuvalue > this->taggerCut_){
    count++;
  }

  // for back compatibility return 1 is 2 btagged; 0 is 1 tagged; -1 is 0 tagged
  return count -1;
}

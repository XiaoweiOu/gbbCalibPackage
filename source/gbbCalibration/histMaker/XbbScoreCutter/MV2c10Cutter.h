#ifndef MV2C10CUTTER_H_
#define MV2C10CUTTER_H_

#include <map>
#include <vector>

class MV2c10Cutter {

  // MV2c10_FixedCutBEff_70_R20p7
 public:
  bool useVRTrkJets_;
  int eff_;
  // allowed eff
  // std::vector<float> validEffs= {60, 70, 77, 85};
  // some table from some place todo
  std::map<int,float> cutValueVR_ = {{60,0.92},{70, 0.79},{77,0.58},{85,0.05}};
  std::map<int,float> cutValue_ = {{60,0.86},{70, 0.66},{77,0.38},{85,-0.15}};
  // set according to eff and vr 
  float taggerCut_;
  
  MV2c10Cutter() = delete;
  
  MV2c10Cutter(int eff, bool useVRTrkJets);
  

  int cut(float muvalue, float nonmuvalue);
  
 private:
  

};


#endif /*MV2C10CUTTER_H_*/

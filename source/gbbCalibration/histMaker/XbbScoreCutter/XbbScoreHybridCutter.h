#ifndef XBBSCOREHYBRIDCUTTER_H_
#define XBBSCOREHYBRIDCUTTER_H_


class XbbScoreHybridCutter {
// no input
 public:
  // mix fraction
  float f_;
  
  XbbScoreHybridCutter() = delete;
 XbbScoreHybridCutter(float f) : f_(f) {};
  // uses fat_pt and ... 4 floats to calculate taggin
  // return 1 if b tagged -1 else
  // input momentum of each var, found in tupleana.h
  int cut(float p_h, float p_qcd, float p_top, float fat_pt_val);

 private:
  double XbbTag_antiQCD_flat60eff(double x);
  double XbbTag_antiQCDtop_flat60eff(double x);
  // double XbbTag_spline_flatQCDrejection_mH_0(double x);
};








#endif /*XBBSCOREHYBRIDCUTTER_H_*/


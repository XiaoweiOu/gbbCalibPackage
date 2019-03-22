#ifndef XBBSCORECUTTER_H_
#define XBBSCORECUTTER_H_

#include <TString.h>
#include <map>

class XbbScoreCutter{
  
 public:
  // fields;
  // Cut-value table
  std::map<int,std::map<float,float>> map_ =
    {
    {50, {{0, 5.1}, {0.2, 4.5}, {1, 3.6}} },
    {60, {{0, 4.8}, {0.2, 3.9}, {1, 3.0}} },
    {70, {{0, 3.9}, {0.2, 3.0}, {1, 2.1}} },
  };


  // mixing fraction
  float f_; 

  // cut value read it directly from map
  float cutValue_;
  
  XbbScoreCutter() = delete;
  // construct cutter with f and eff as parameter
  XbbScoreCutter(float f, int eff);
  ~XbbScoreCutter();
 
  // get the cut value of specified efficiency and f
  float getCutValue(float f, int eff);

  // return 1 if b tagged -1 else
  // input momentum of each var, found in tupleana.h
  int cut(float p_h, float p_qcd, float p_top);
  
 private:
  // check if f and eff are in range
  void checkInput(float f, int eff);
  
};


#endif /* XBBSCORECUTTER_H_ */

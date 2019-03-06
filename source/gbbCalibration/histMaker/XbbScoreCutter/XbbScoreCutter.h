#ifndef XBBSCORECUTTER_H_
#define XBBSCORECUTTER_H_

#include <TString.h>
#include <map>
#include <vector>

#include <memory> // for unique pointer

// #include "../GbbTupleAna.h" // for gbbcandidate


// from gbbtupleana <temp>
//struct GbbCandidate{
  
//  unsigned int fat_index;
//  unsigned int muojet_index;
//  unsigned int nonmuojet_index;
//   float fat_pt;
//  bool hasleading2trackjets;
//  bool hasTruthMuon;
// };


class XbbScoreCutter{
  
 public:
  // fields;
  // Cut-value table
  std::unique_ptr<std::map<int,std::map<float,float>>> map_;
  // mixing fraction
  float f_; 
  // higgs efficiency optional
  int eff_;
  // valid f values;
  std::vector<float> validFs= {0, 0.2, 1};
  // valid eff values;
  std::vector<float> validEffs= {50, 60, 70};

  // cut value read it directly from map
  float cutValue_;
  
  XbbScoreCutter();
  // the config string is set in config file
  XbbScoreCutter(const TString configString);
  XbbScoreCutter(float f, int eff);
  ~XbbScoreCutter();
 
  // get the cut value of specified efficiency and f
  float getCutValue(int eff, float f);

  // return 1 if b tagged -1 else
  // input momentum of each var, found in tupleana.h
  int cut(float p_h, float p_qcd, float p_top);

  // to do
  // int cut(const GbbCandidate& gbbcand);
  
 private:
  // initialize the static table
  std::map<int,std::map<float,float>>* loadStaticMap();
  
};


#endif /* XBBSCORECUTTER_H_ */

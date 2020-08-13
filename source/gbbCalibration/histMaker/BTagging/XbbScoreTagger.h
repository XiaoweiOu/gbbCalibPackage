#ifndef XBBSCORETAGGER_H_
#define XBBSCORETAGGER_H_

#include "BJetTagger.h"
#include <map>

class XbbScoreTagger : public BJetTagger {
 public :
  XbbScoreTagger(TString operatingPt, float xbbTopFrac);

  int initialize(const TupleAna& gbbtuple);
  int accept(const GbbCandidate& gbbcand);
  // return -1 for SFs. should it return MV2c10_FixedCutBEff_70 SFs instead?
  int accept(const GbbCandidate& gbbcand, float& sf);
  void setFlags(const std::vector<bool>& flags) {};

 private:
  int m_eff;
  float m_topFrac;
  float m_fixed_cut;
  bool m_useHybWP;
  std::vector<float>* v_SubjetBScoreHiggs;
  std::vector<float>* v_SubjetBScoreQCD;
  std::vector<float>* v_SubjetBScoreTop;
  std::vector<float>* v_fat_pt;

  std::map<int,std::map<float,float>> m_fixedCutMap = {
    {0  , {{50, 4.03}, {60, 3.63}, {70, 3.05}, {80, 2.34}} },
    {0.25, {{50, 3.13}, {60, 2.55}, {70, 1.92}, {80, 1.20}} },
  };
  void setFixedCut();
};

#endif /*XBBSCORETAGGER_H_*/

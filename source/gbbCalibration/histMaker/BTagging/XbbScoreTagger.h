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
  std::vector<float>* v_XbbScoreHiggs;
  std::vector<float>* v_XbbScoreQCD;
  std::vector<float>* v_XbbScoreTop;
  std::vector<float>* v_fat_pt;

  std::map<int,std::map<float,float>> m_fixedCutMap = {
    {0  , {{50, 5.1}, {60, 4.8}, {70, 3.9}} },
    {0.2, {{50, 4.5}, {60, 3.9}, {70, 3.0}} },
    {1  , {{50, 3.6}, {60, 3.0}, {70, 2.1}} },
  };
  void setFixedCut();
  double XbbTag_antiQCD_flat60eff(double x);
  double XbbTag_antiQCDtop_flat60eff(double x);
};

#endif /*XBBSCORETAGGER_H_*/

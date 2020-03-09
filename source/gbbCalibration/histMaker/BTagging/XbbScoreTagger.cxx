#include "XbbScoreTagger.h"
#include "helpers/GbbUtil.h"
#include <cmath> // for log

XbbScoreTagger::XbbScoreTagger(TString operatingPt, float xbbTopFrac) :
  BJetTagger("SubjetBScore",operatingPt),
  m_topFrac(xbbTopFrac),
  m_fixed_cut(-1.),
  m_useHybWP(false),
  v_SubjetBScoreHiggs(nullptr),
  v_SubjetBScoreQCD(nullptr),
  v_SubjetBScoreTop(nullptr),
  v_fat_pt(nullptr) {
}

int XbbScoreTagger::initialize(const TupleAna& gbbtuple) {
  // get address of containers for relevant variables
  // WARNING: no protection from TupleAna deleting these pointers
  // this is bad practice, so be careful
  v_SubjetBScoreHiggs = gbbtuple.fat_SubjetBScore_Higgs;
  v_SubjetBScoreQCD   = gbbtuple.fat_SubjetBScore_QCD;
  v_SubjetBScoreTop   = gbbtuple.fat_SubjetBScore_Top;
  v_fat_pt        = gbbtuple.fat_pt;

  auto configV = GbbUtil::splitString(m_operatingPt,'_');
  if (configV.size() != 2)
    throw std::invalid_argument(Form("invalid XbbScore operating point %s",m_operatingPt.Data()));

  m_eff = configV[1].Atoi();
  if (configV[0].Contains("HybBEff")) {
    m_useHybWP = true;
    throw std::invalid_argument("no hybrid cuts currently defined for HbbTagger");
  } else {
    setFixedCut();
  }
  return 0;
}

void XbbScoreTagger::setFixedCut() {

  // check that requested top fraction is in the map
  if(m_fixedCutMap.count(m_topFrac) == 0) {
    throw std::invalid_argument( "invalid input top frac: " + std::to_string(m_topFrac) );
  }
  // check that requested efficiency point exists
  if(m_fixedCutMap[m_topFrac].count(m_eff) == 0) {
    throw std::invalid_argument( "invalid input eff: " + std::to_string(m_eff) );
  }

  // set cut value
  m_fixed_cut = m_fixedCutMap[m_topFrac][m_eff];
}

int XbbScoreTagger::accept(const GbbCandidate& gbbcand, float& sf) {
  sf = -1.;
  return accept(gbbcand);
}

int XbbScoreTagger::accept(const GbbCandidate& gbbcand) {

  float p_higgs = v_SubjetBScoreHiggs->at(gbbcand.ind_fj);
  float p_qcd   = v_SubjetBScoreQCD->at(gbbcand.ind_fj);
  float p_top   = v_SubjetBScoreTop->at(gbbcand.ind_fj);
  float fat_pt  = v_fat_pt->at(gbbcand.ind_fj);

  float xbb_discriminant = std::log( p_higgs / ( (1-m_topFrac)*p_qcd + m_topFrac*p_top) );
  float cut_val = 0.;
  if (m_useHybWP) {
    return -1;
  } else {
    cut_val = m_fixed_cut;
  }

  // return 2 for double-tagged or 0
  return (xbb_discriminant > cut_val) ? 2 : 0;
}

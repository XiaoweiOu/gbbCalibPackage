#include "XbbScoreTagger.h"
#include "helpers/GbbUtil.h"
#include <cmath> // for log

XbbScoreTagger::XbbScoreTagger(std::string operatingPt, float xbbTopFrac) :
  BJetTagger("XbbScore",operatingPt),
  m_topFrac(xbbTopFrac),
  m_fixed_cut(-1.),
  m_useHybWP(false),
  v_XbbScoreHiggs(nullptr),
  v_XbbScoreQCD(nullptr),
  v_XbbScoreTop(nullptr),
  v_fat_pt(nullptr) {
}

int XbbScoreTagger::initialize(const TupleAna& gbbtuple) {
  // get address of containers for relevant variables
  // WARNING: no protection from TupleAna deleting these pointers
  // this is bad practice, so be careful
  v_XbbScoreHiggs = gbbtuple.fat_XbbScoreHiggs;
  v_XbbScoreQCD   = gbbtuple.fat_XbbScoreQCD;
  v_XbbScoreTop   = gbbtuple.fat_XbbScoreTop;
  v_fat_pt        = gbbtuple.fat_pt;

  auto configV = GbbUtil::splitString(m_operatingPt,"_");
  if (configV.size() != 2) {
    std::cerr << "ERROR : invalid XbbScore operating point " << m_operatingPt.c_str() << std::endl;
    return -1;
  }
  m_eff = std::stoi(configV[1], nullptr);
  if (configV[0].find("HybBEff") != std::string::npos) {
    m_useHybWP = true;
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

  float p_higgs = v_XbbScoreHiggs->at(gbbcand.ind_fj);
  float p_qcd   = v_XbbScoreQCD->at(gbbcand.ind_fj);
  float p_top   = v_XbbScoreTop->at(gbbcand.ind_fj);
  float fat_pt  = v_fat_pt->at(gbbcand.ind_fj);

  float xbb_discriminant = std::log( p_higgs / ( (1-m_topFrac)*p_qcd + m_topFrac*p_top) );
  float cut_val = 0.;
  if (m_useHybWP) {
    if ( m_topFrac == 0.2 ) {
      cut_val = XbbTag_antiQCDtop_flat60eff(fat_pt);
    } else if ( m_topFrac == 0 ) {
      cut_val = XbbTag_antiQCD_flat60eff(fat_pt);
    } else {
      return -1;
    }
  } else {
    cut_val = m_fixed_cut;
  }

  // return 2 for double-tagged or 0
  return (xbb_discriminant > cut_val) ? 2 : 0;
}

double XbbScoreTagger::XbbTag_antiQCD_flat60eff(double x) {

  const int fNp = 12, fKstep = 0;
  const double fDelta = -1, fXmin = 0, fXmax = 2750;
  const double fX[12] = { 0, 250, 500, 750, 1000,
                       1250, 1500, 1750, 2000, 2250,
                       2500, 2750 };
  const double fY[12] = { 0, 4.75, 4.875, 4.875, 4.5,
                       4, 3, 1.25, -1.375, -5,
                       -10.375, -14.125 };
  const double fB[12] = { 0.0360118, 0.00586912, -0.000988246, -0.000416141, -0.00184719,
                       -0.00269509, -0.00537245, -0.00881512, -0.0118671, -0.0187165,
                       -0.0212667, -0.00571655 };
  const double fC[12] = { -8.35705e-05, -3.7e-05, 9.57053e-06, -7.28211e-06, 1.5579e-06,
                       -4.94949e-06, -5.75995e-06, -8.01073e-06, -4.19714e-06, -2.32007e-05,
                       1.3e-05, 250 };
  const double fD[12] = { 6.2094e-08, 6.2094e-08, -2.24702e-08, 1.17867e-08, -8.67652e-09,
                       -1.08061e-09, -3.00104e-09, 5.08479e-09, -2.53381e-08, 4.82676e-08,
                       4.82676e-08, 116.025 };
  int klow=0;
  if(x<=fXmin) klow=0;
  else if(x>=fXmax) klow=fNp-1;
  else {
    if(fKstep) {
      // Equidistant knots, use histogramming
      klow = int((x-fXmin)/fDelta);
      if (klow < fNp-1) klow = fNp-1;
    } else {
      int khig=fNp-1, khalf;
      // Non equidistant knots, binary search
      while(khig-klow>1) {
        if(x>fX[khalf=(klow+khig)/2]) klow=khalf;
        else khig=khalf;
      }
    }
  }
  // Evaluate now
  double dx=x-fX[klow];
  return (fY[klow]+dx*(fB[klow]+dx*(fC[klow]+dx*fD[klow])));
}

double XbbScoreTagger::XbbTag_antiQCDtop_flat60eff(double x) {
   // 20% fraction for top
   const int fNp = 12, fKstep = 0;
   const double fDelta = -1, fXmin = 0, fXmax = 2750;
   const double fX[12] = { 0, 250, 500, 750, 1000,
                        1250, 1500, 1750, 2000, 2250,
                        2500, 2750 };
   const double fY[12] = { 0, 4, 4.125, 4.125, 3.625,
                        2.875, 1.875, 0.5, -1.625, -5,
                        -10.25, -15.75 };
   const double fB[12] = { 0.0302896, 0.0049802, -0.000710406, -0.000638579, -0.00273528,
                        -0.0034203, -0.00458351, -0.00674566, -0.0104339, -0.0175189,
                        -0.0229906, -0.0195189 };
   const double fC[12] = { -7.02376e-05, -3.1e-05, 8.23756e-06, -7.95025e-06, -4.36552e-07,
                        -2.30354e-06, -2.34929e-06, -6.2993e-06, -8.45352e-06, -1.98866e-05,
                        -2e-06, 250 };
   const double fD[12] = { 5.23168e-08, 5.23168e-08, -2.15838e-08, 1.00183e-08, -2.48932e-09,
                        -6.10022e-11, -5.26667e-09, -2.8723e-09, -1.52441e-08, 2.38488e-08,
                        2.38488e-08, 116.025 };
   int klow=0;
   if(x<=fXmin) klow=0;
   else if(x>=fXmax) klow=fNp-1;
   else {
     if(fKstep) {
       // Equidistant knots, use histogramming
       klow = int((x-fXmin)/fDelta);
       if (klow < fNp-1) klow = fNp-1;
     } else {
       int khig=fNp-1, khalf;
       // Non equidistant knots, binary search
       while(khig-klow>1)
         if(x>fX[khalf=(klow+khig)/2]) klow=khalf;
         else khig=khalf;
     }
   }
   // Evaluate now
   double dx=x-fX[klow];
   return (fY[klow]+dx*(fB[klow]+dx*(fC[klow]+dx*fD[klow])));
}

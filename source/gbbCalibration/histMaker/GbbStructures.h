
/* This file includes the common structure/ datatypes used in Gbbhistmaking step
*/

#ifndef GBBSTRUCTURE_H_
#define GBBSTRUCTURE_H_

//
// Struct storing the defining characteristics of a g->bb jet
// The indices can be used to access information on the
// association objects (fatjet, track-jets, muon).
//
struct GbbCandidate{
  unsigned int ind_fj;
  std::vector<unsigned int> ind_tj;
  std::vector<unsigned int> ind_tj_mu;
  bool hasLeadTrkJets;
  unsigned int nTruthMuons;
  unsigned int nRecoMuons;
};

//
// Struct storing all the various impact parameter variables
// being considered for the template fit
//
struct trkjetSd0Info {
  float maxSd0;
  float maxSd0_dR;
  float subSd0;
  float thirdSd0;
  float meanSd0_sd0;

  float maxSd0_pt;
  float subSd0_pt;
  float thirdSd0_pt;
  float meanSd0_pt;

  float maxd0;
  float subd0;
  float thirdd0;
  float meand0_sd0;

  float maxd0_pt;
  float subd0_pt;
  float thirdd0_pt;
  float meand0_pt;

  float max_z0sintheta_pt;
  float sub_z0sintheta_pt;
  float third_z0sintheta_pt;

  float maxd0err;
  float subd0err;
  float thirdd0err;

  float maxd0err_pt;
  float subd0err_pt;
  float thirdd0err_pt;

  int nMean;
};

#endif /*GBBSTRUCTURE_H_*/

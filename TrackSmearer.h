/*                                                                                                                                                                                                                                           * TrackSmearer.h                                                                                                                                                                                                                            * Based on InDetTrackSmearingTool-00-00-07, adapted for ntuples                                                                                                                                                                                             
 *
 *  Created on: Jun 27, 2016                                                                                                                                                                                                                 *      Author: rjacobs                                                                                                                                                                                                                      */

#ifndef TRACKSMEARER_H_
#define TRACKSMEARER_H_

#include "TFile.h"
#include "TString.h"
#include "TH1.h"
#include <vector>
#include <map>
#include <iostream>
#include "TMinuit.h"
#include <utility>
#include <memory>
#include <algorithm>
#include "TMath.h"
#include "TRandom3.h"
#include "TH2.h"

class TrackSmearer{

 private:
	std::shared_ptr<TFile> m_func_infile;
	std::shared_ptr<TFile> m_hist_infile;                                                                        
	std::shared_ptr<TRandom3> m_rndm;
	std::shared_ptr<TH2> m_quaddiff;
	std::shared_ptr<TH1> m_dead_hist;

	bool m_do_Dead;


 public:
	TrackSmearer();
	TrackSmearer( TString hist_filename,TString func_filename, bool doDeadModules );
	
	void Initialize();


	float getD0SmearSigma( float pt, float eta);
	float getSmearedD0(float pt, float eta, float d0);
	float readHistogram(TH2* hist, float pt,float eta);

};


#endif /*__TRACKSMEARER_H__*/

/*
 * FlavourFracCorrector.h
 *
 *  Created on: Apr 4, 2016
 *      Author: rjacobs
 */

#ifndef FLAVOURFRACCORRECTOR_H_
#define FLAVOURFRACCORRECTOR_H_

#include "TFile.h"
#include "TString.h"
#include <memory>
#include <map>
#include "TH1D.h"

class FlavourFracCorrector {

private:

	std::map<TString, std::shared_ptr<TH1D>> m_FitResults;
	std::vector<float> m_muojet_pt_bins;
	std::vector<float> m_nonmuojet_pt_bins;


	TString getPtLabel(float muojet_pt,float nonmuojet_pt);
	void ReadInFitResults(TString infilename);


public:
	FlavourFracCorrector();
	FlavourFracCorrector(TString infilename);
	virtual ~FlavourFracCorrector();

	float GetCorrectionFactorNom(TString flav_category,float mu_pt, float nonmu_pt);
	float GetCorrectionFactorStatUp(TString flav_category,float mu_pt, float nonmu_pt);
	float GetCorrectionFactorStatDown(TString flav_category,float mu_pt, float nonmu_pt);

	float GetCorrectionFactorSys(TString flav_category,float mu_pt, float nonmu_pt,TString sys_name_variation);
	float GetCorrectionFactorTotalError(float mu_pt, float nonmu_pt);

	float GetCovarianceMatrixElement(int i, int j, TString ptlabel);
	std::vector<float> GetCovarianceMatrix(TString ptlabel);


};

#endif /* FLAVOURFRACCORRECTOR_H_ */

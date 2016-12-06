/*
 * SampleInfo.h
 *
 *  Created on: Feb 22, 2016
 *      Author: rjacobs
 */

#ifndef SAMPLEINFO_H_
#define SAMPLEINFO_H_

#include <map>
#include "Riostream.h"
#include <fstream>
#include "TObjArray.h"
#include "TObjString.h"

class SampleInfo {

private:

	std::map<int,float> m_xsec; //cross section including k-factor, key is datasetID
	std::map<int,float> m_filtereff; //filter efficiency, key is datasetID
	std::map<int,float> m_xAODevtcount; //xAOD event count, key is datasetID
	std::map<int,float> m_DxAODevtcount; //DxAOD event count, key is datasetID


public:
	SampleInfo();
	SampleInfo(TString filename);
	virtual ~SampleInfo();

	void ReadConfig(TString filename);
	float FilterEfficiency(int DSID);
	float FilterEfficiency(TString samplename);

	float CrossSection(int DSID);
	float CrossSection(TString samplename);

	float xAODEventCount(int DSID);
	float xAODEventCount(TString samplename);
	
	float DxAODEventCount(int DSID);


	int getDSID(TString samplename);

};

#endif /* SAMPLEINFO_H_ */

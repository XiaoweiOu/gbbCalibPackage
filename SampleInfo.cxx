/*
 * SampleInfo.cxx
 *
 *  Created on: Feb 22, 2016
 *      Author: rjacobs
 */
#define SampleInfo_cxx
#include "SampleInfo.h"

SampleInfo::SampleInfo() {
	// TODO Auto-generated constructor stub

}

SampleInfo::~SampleInfo() {
	// TODO Auto-generated destructor stub
}

SampleInfo::SampleInfo(TString infilename) {
	// TODO Auto-generated constructor stub

}


float SampleInfo::FilterEfficiency(int DSID){
	if(m_filtereff.find(DSID)!=m_filtereff.end()) return m_filtereff[DSID];
	else std::cout<<"Unknown DSID"<<std::endl;
	return 0.;
}

float SampleInfo::FilterEfficiency(TString samplename){

	return this->FilterEfficiency(this->getDSID(samplename));

}

float SampleInfo::CrossSection(int DSID){
	if(m_xsec.find(DSID)!=m_xsec.end()) return m_xsec[DSID];
	else std::cout<<"Unknown DSID"<<std::endl;
	return 0.;
}

float SampleInfo::CrossSection(TString samplename){

	return this->CrossSection(this->getDSID(samplename));

}

float SampleInfo::xAODEventCount(int DSID){
	if(m_xAODevtcount.find(DSID)!=m_xAODevtcount.end()) return m_xAODevtcount[DSID];
	else std::cout<<"Unknown DSID"<<std::endl;
	return 0.;
}

float SampleInfo::xAODEventCount(TString samplename){

	return this->xAODEventCount(this->getDSID(samplename));

}

float SampleInfo::DxAODEventCount(int DSID){
  if(m_DxAODevtcount.find(DSID)!=m_DxAODevtcount.end()) return m_DxAODevtcount[DSID];
  else std::cout<<"Unknown DSID"<<std::endl;
  return 0.;
}



void SampleInfo::ReadConfig(TString filename){

	std::string fname=filename.Data();
	std::ifstream in;
	in.open(filename,std::ifstream::in);

	int DSID;
	float xsec,filtereff, xAODevtcount, DxAODevtcount;

	while(in.good()){

	  in >> DSID >> xsec >> filtereff >> xAODevtcount >> DxAODevtcount;

	  std::cout<<"Read in xsec"<<xsec<<std::endl;

		m_xsec[DSID]=xsec;
		m_filtereff[DSID]=filtereff;
		m_xAODevtcount[DSID]=xAODevtcount;
		m_DxAODevtcount[DSID]=DxAODevtcount;

	}

	in.close();
}

int SampleInfo::getDSID(TString samplename){
	//assume user.ruth.mc15_13TeV.DSID....

	TObjArray* tokens=samplename.Tokenize('.');
	TString dsid=((TObjString*) (*tokens)[3])->GetString();

	return dsid.Atoi();

}


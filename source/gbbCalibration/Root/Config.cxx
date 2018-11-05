#include "gbbCalibration/Config.h"
#include "TEnv.h"
#include "TObjString.h"
#include "TObjArray.h"
#include "PathResolver/PathResolver.h"

Config::Config() {
  // TODO Auto-generated constructor stub

}

Config::~Config() {
  // TODO Auto-generated destructor stub

}

Config::Config(TString& config_path){

  std::cout<<"=============================================="<<std::endl;   
  TString m_config_path = config_path;
  //if ( !(gSystem->AccessPathName(m_config_path.Data())) )
  m_config_path = PathResolverFindCalibFile(m_config_path.Data());

  if (config_path == "") {
    std::cout << "Cannot find settings file " + config_path + "\n  also searched in " + m_config_path << std::endl;
    abort();
  } else std::cout << "Config file is set to: " << m_config_path << std::endl;

  TEnv* config = new TEnv("env");
  if (config->ReadFile(m_config_path.Data(),EEnvLevel(0)) == -1) {
    std::cout << "Could not read config file " << m_config_path.Data() << std::endl;
    abort();
  }
  
  m_inputfile   = config->GetValue("InputFile",         "./data/inputs.root");
  std::cout<<"InputFile: "<<m_inputfile<<std::endl;     
  
  m_Debug = config->GetValue("doDebug",false); 
  std::cout<<"doDebug: "<<m_Debug<<std::endl; 

  m_doMCStatsNP = config->GetValue("doMCStatsNP",false); 
  std::cout<<"doNPStatsMC: "<<m_doMCStatsNP<<std::endl; 

  m_doFitInFatJetPtBins = config->GetValue("doFitInFatJetPtBins",false); 
  std::cout<<"doFitInFatJetPtBins: "<<m_doFitInFatJetPtBins<<std::endl; 

  m_systematics=SplitString(config->GetValue("Systematics",""),',');
  std::cout<<"Systematics: "<<config->GetValue("Systematics","")<<std::endl;
  
  m_pairs=SplitString(config->GetValue("FlavourPairs",""),',');
  std::cout<<"FlavourPairs: "<<config->GetValue("FlavourPairs","")<<std::endl; 

  m_chans=SplitString(config->GetValue("Channels","muo,nonmuo"),',');
  std::cout<<"Channels: "<<config->GetValue("Channels","")<<std::endl;   

  m_mutrackjetbins=SplitStringD(config->GetValue("MuTrackJetBins",""),',');
  std::cout<<"MuTrackJetBins: "<<config->GetValue("MuTrackJetBins","")<<std::endl; 

  m_nonmutrackjetbins=SplitStringD(config->GetValue("NonmuTrackJetBins",""),',');
  std::cout<<"NonmuTrackJetBins: "<<config->GetValue("NonmuTrackJetBins","")<<std::endl;

  m_Ntimes_smooth=config->GetValue("smoothTemplatesNtimes",0);
  std::cout<<"smoothTemplatesNtimes: "<<m_Ntimes_smooth<<std::endl;

  m_plotvariables=SplitString(config->GetValue("PlotVariables",""),',');
  std::cout<<"PlotVariables: "<<config->GetValue("PlotVariables","")<<std::endl;

  for(int i=0; i<m_plotvariables.size(); i++){
    TString name="PlotBins"+m_plotvariables[i];
    TString name_posttag=name+"_PREFITPOSTTAG";
    TString plotvar_posttag=m_plotvariables[i]+"_PREFITPOSTTAG";
    m_binning[m_plotvariables[i]]=SplitStringD(config->GetValue(name.Data(),""),',');
    m_binning[plotvar_posttag]=SplitStringD(config->GetValue(name.Data(),""),',');
    std::cout<<name<<config->GetValue(name.Data(),"")<<std::endl;
  }


  m_fatjetbins=SplitStringD(config->GetValue("FatJetBins",""),',');
  std::cout<<"FatJetBins: "<<config->GetValue("FatJetBins","")<<std::endl;
  
  m_params_names=SplitString(config->GetValue("ParameterNames",""),',');
  std::cout<<"ParameterNames: "<<config->GetValue("ParameterNames","")<<std::endl;
  
  m_params_in=SplitStringD(config->GetValue("ParameterStart",""),',');
  std::cout<<"ParameterStart: "<<config->GetValue("ParameterStart","")<<std::endl;
  
  m_params_low=SplitStringD(config->GetValue("ParameterLow",""),',');
  std::cout<<"ParameterLow: "<<config->GetValue("ParameterLow","")<<std::endl;
  
  m_params_high=SplitStringD(config->GetValue("ParameterHigh",""),',');
  std::cout<<"ParameterHigh: "<<config->GetValue("ParameterHigh","")<<std::endl;

  m_N_pseudo_exp=config->GetValue("NPseudoExperiments",1000);
  std::cout<<"NPseudoExperiments: "<<m_N_pseudo_exp<<std::endl;

  m_xlabel   = config->GetValue("XAxisLabel",         "Large-R Jet p_{T} [GeV]");
  std::cout<<"XAxisLabel: "<<m_xlabel<<std::endl;

  m_ylabel   = config->GetValue("YAxisLabel",         "Scale Factor");
  std::cout<<"XAxisLabel: "<<m_ylabel<<std::endl;
  
  m_plot_label   = config->GetValue("PlotLabel",         "Internal");
  std::cout<<"PlotLabel: "<<m_plot_label<<std::endl;
 
  m_sub_label   = config->GetValue("SubLabel",         "#sqrt{s}=13 TeV, 36.1 fb^{-1}");
  std::cout<<"SubLabel: "<<m_sub_label<<std::endl;
   
  m_subsub_label   = config->GetValue("SubSubLabel",         "#bf{g #rightarrow bb calibration}");
  std::cout<<"SubSubLabel: "<<m_subsub_label<<std::endl;

  m_statThr_Rebin = config->GetValue("RebinStatThreshold",0.5);
  std::cout<<"RebinStatThreshold: "<<m_statThr_Rebin<<std::endl;

  delete config;

}


std::vector<TString> Config::SplitString(TString str, char delim){
  std::vector<TString> tokens;   TObjArray *Strings=str.Tokenize(delim);
  for(int i=0; i<Strings->GetEntriesFast(); i++){
    tokens.push_back(((TObjString*) (*Strings)[i])->GetString());
  } 
  return tokens; 
} 

std::vector<float> Config::SplitStringD(TString str, char delim){
  std::vector<float> tokens;
  TObjArray *Strings=str.Tokenize(delim);
  for(int i=0; i<Strings->GetEntriesFast(); i++){
    tokens.push_back((((TObjString*) (*Strings)[i])->GetString()).Atof()); 
    //tokens.push_back((*((TString*)(*Strings)[i])).Atof());
    //std::cout<<"Token: "<<((TObjString*) (*Strings)[i])->GetString()<<" vs "<<tokens[i]<<std::endl;
  }
  return tokens;
}


std::vector<TString> Config::GetAllRegions(){

  std::vector<TString> pt_all_regions;
  TString name;

  std::vector<TString> mu_pt_regions;
  std::vector<TString> nonmu_pt_regions;
  
  if(!m_doFitInFatJetPtBins){

    for(int i=0; i<m_mutrackjetbins.size(); i++){

      if(i==0) mu_pt_regions.push_back(Form("mjpt_l%.0f",m_mutrackjetbins[i]));

      if(i<m_mutrackjetbins.size()-1) mu_pt_regions.push_back(Form("mjpt_g%.0fl%.0f",m_mutrackjetbins[i],m_mutrackjetbins[i+1]));
      else mu_pt_regions.push_back(Form("mjpt_g%.0f",m_mutrackjetbins[i])); 

    }

    for(int i=0; i<m_nonmutrackjetbins.size(); i++){

      if(i==0) nonmu_pt_regions.push_back(Form("nmjpt_l%.0f",m_nonmutrackjetbins[i]));

      if(i<m_nonmutrackjetbins.size()-1) nonmu_pt_regions.push_back(Form("nmjpt_g%.0fl%.0f",m_nonmutrackjetbins[i],m_nonmutrackjetbins[i+1]));
      else nonmu_pt_regions.push_back(Form("nmjpt_g%.0f",m_nonmutrackjetbins[i]));

    }


    for(auto& mb : mu_pt_regions){
    
      for(auto& nmb : nonmu_pt_regions){

	name=mb+"_"+nmb;
	//if(name.EqualTo("mjpt_l100_nmjpt_l20")) continue;
	pt_all_regions.push_back(name);
	    
      }
    
    }

  }else{

    for(int i=0; i<m_fatjetbins.size()-1; i++){
      pt_all_regions.push_back(Form("fjpt_g%.0fl%.0f",m_fatjetbins[i],m_fatjetbins[i+1]));
      
    }

  }


  return pt_all_regions;
  
}



std::vector<TString> Config::GetMCHistNames(TString& pt_region, TString &sys, TString &variable){

  std::vector<TString> hist_names;
  for(auto fp : m_pairs){
    
    hist_names.push_back("h"+fp+sys+"_"+pt_region+"_"+variable);
	  
  }
  
  return hist_names;
  
}


TString Config::GetDataHistName(TString& pt_region, TString &variable){

  TString name="hDataNom_"+pt_region+"_"+variable;
  return name;
}



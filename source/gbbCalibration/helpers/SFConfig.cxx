#include "SFConfig.h"
#include "TEnv.h"
#include "TObjString.h"
#include "TObjArray.h"
#include "PathResolver/PathResolver.h"

SFConfig::SFConfig() {
  // TODO Auto-generated constructor stub

}

SFConfig::~SFConfig() {
  // TODO Auto-generated destructor stub

}

SFConfig::SFConfig(TString& config_path){

  std::cout<<"=============================================="<<std::endl;   
  TString m_config_path = config_path;
  //if ( !(gSystem->AccessPathName(m_config_path.Data())) )
  m_config_path = PathResolverFindCalibFile(m_config_path.Data());

  if (config_path == "") {
    std::cout << "Cannot find settings file " + config_path + "\n  also searched in " + m_config_path << std::endl;
    abort();
  } else std::cout << "SFConfig file is set to: " << m_config_path << std::endl;

  TEnv* config = new TEnv("env");
  if (config->ReadFile(m_config_path.Data(),EEnvLevel(0)) == -1) {
    std::cout << "Could not read config file " << m_config_path.Data() << std::endl;
    abort();
  }
  
  m_inputfile   = config->GetValue("InputFile","./data/inputs.root");
  std::cout<<"InputFile: "<<m_inputfile<<std::endl;     
  
  m_Debug = config->GetValue("doDebug",false); 
  std::cout<<"doDebug: "<<m_Debug<<std::endl; 

  m_global_config = new GlobalConfig(PathResolverFindCalibFile("gbbCalibration/configs/GlobalConfig.cfg"));
  std::cout<<"Loaded GlobalConfig"<<std::endl;

  m_doMCStatsNP = config->GetValue("doMCStatsNP",false); 
  std::cout<<"doNPStatsMC: "<<m_doMCStatsNP<<std::endl; 

  m_doFitInFatJetPtBins = config->GetValue("doFitInFatJetPtBins",false); 
  std::cout<<"doFitInFatJetPtBins: "<<m_doFitInFatJetPtBins<<std::endl; 

  m_chans=SplitString(config->GetValue("Channels","muo,nonmuo"),',');
  std::cout<<"Channels: "<<config->GetValue("Channels","")<<std::endl;   

  m_Ntimes_smooth=config->GetValue("smoothTemplatesNtimes",0);
  std::cout<<"smoothTemplatesNtimes: "<<m_Ntimes_smooth<<std::endl;
  
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

std::vector<TString> SFConfig::SplitString(TString str, char delim){
  std::vector<TString> tokens;   TObjArray *Strings=str.Tokenize(delim);
  for(int i=0; i<Strings->GetEntriesFast(); i++){
    tokens.push_back(((TObjString*) (*Strings)[i])->GetString());
  } 
  return tokens; 
} 

std::vector<float> SFConfig::SplitStringD(TString str, char delim){
  std::vector<float> tokens;
  TObjArray *Strings=str.Tokenize(delim);
  for(int i=0; i<Strings->GetEntriesFast(); i++){
    tokens.push_back((((TObjString*) (*Strings)[i])->GetString()).Atof()); 
    //tokens.push_back((*((TString*)(*Strings)[i])).Atof());
    //std::cout<<"Token: "<<((TObjString*) (*Strings)[i])->GetString()<<" vs "<<tokens[i]<<std::endl;
  }
  return tokens;
}

#include "GlobalConfig.h"
#include "TEnv.h"
#include "TObjString.h"
#include "TObjArray.h"
#include "PathResolver/PathResolver.h"

GlobalConfig::~GlobalConfig() {
  // TODO Auto-generated destructor stub
}

GlobalConfig::GlobalConfig(const TString& config_path) {

  std::cout<<"=============================================="<<std::endl;   
  TString m_config_path = config_path;
  //if ( !(gSystem->AccessPathName(m_config_path.Data())) )
  m_config_path = PathResolverFindCalibFile(m_config_path.Data());

  if (m_config_path == "") {
    std::cout << "Cannot find settings file " + config_path + "\n  also searched in " + m_config_path << std::endl;
    abort();
  } else std::cout << "GlobalConfig file is set to: " << m_config_path << std::endl;

  TEnv* config = new TEnv("env");
  if (config->ReadFile(m_config_path.Data(),EEnvLevel(0)) == -1) {
    std::cout << "Could not read config file " << m_config_path.Data() << std::endl;
    abort();
  }

  m_Systematics=SplitString(config->GetValue("Systematics",""),',');
  std::cout<<"Systematics: "<<config->GetValue("Systematics","")<<std::endl;

  m_FlavourPairs=SplitString(config->GetValue("FlavourPairs",""),',');
  std::cout<<"FlavourPairs: "<<config->GetValue("FlavourPairs","")<<std::endl; 

  m_MuonJetPtBins = SplitStringD(config->GetValue("MuonJetPtBins",""),',');
  std::cout<<"MuonJetPtBins: "<<config->GetValue("MuonJetPtBins","")<<std::endl;
  m_MuonJetRegions = MakeLabels(m_MuonJetPtBins, "mjpt");

  m_NonMuJetPtBins = SplitStringD(config->GetValue("NonMuJetPtBins",""),',');
  std::cout<<"NonMuoJetPtBins: "<<config->GetValue("NonMuJetPtBins","")<<std::endl;
  m_NonMuJetRegions = MakeLabels(m_NonMuJetPtBins, "nmjpt");

  m_FatJetPtBins = SplitStringD(config->GetValue("FatJetPtBins",""),',');
  std::cout<<"FatJetPtBins: "<<config->GetValue("FatJetPtBins","")<<std::endl;
  m_FatJetRegions = MakeLabels(m_FatJetPtBins, "fjpt");

  m_PlotVariables = SplitString(config->GetValue("PlotVariables",""),',');
  std::cout<<"PlotVariables: "<<config->GetValue("PlotVariables","")<<std::endl;

  for (TString var : m_PlotVariables) {
    std::vector<float> binning = SplitStringD(config->GetValue(("PlotBins."+var).Data(),""),',');
    m_PlotBinning.emplace(var, binning);
    std::cout <<("PlotBins."+var).Data()<<": "<<config->GetValue(("PlotBins."+var).Data(),"")<<std::endl;
  }

  std::cout<<"=============================================="<<std::endl;   
  delete config;
}

std::vector<TString> GlobalConfig::SplitString(TString str, char delim){
  std::vector<TString> tokens;
  TObjArray *strings = str.Tokenize(delim);
  for(int i=0; i < strings->GetEntriesFast(); i++){
    tokens.push_back(((TObjString*) (*strings)[i])->GetString());
  } 
  delete strings;
  return tokens; 
} 

std::vector<float> GlobalConfig::SplitStringD(TString str, char delim){
  std::vector<float> tokens;
  TObjArray *strings=str.Tokenize(delim);
  for(int i=0; i < strings->GetEntriesFast(); i++){
    tokens.push_back((((TObjString*) (*strings)[i])->GetString()).Atof()); 
  }
  delete strings;
  return tokens;
}

// Returns string in format:
//  1.23 -> 1p23
//  1.20 -> 1p2
//  1.00 -> 1
TString GlobalConfig::FloatToStr(const float val) {
  TString str = Form("%.2f",val);
  str.Remove(TString::EStripType::kTrailing,'0');
  if (str.EndsWith(".")) str.Remove(str.First("."));
  else str.ReplaceAll(".","p");
  return str;
}

std::vector<TString> GlobalConfig::MakeLabels(const std::vector<float> bins, const TString prefix) {
  if (bins.size() == 0) return std::vector<TString>();
  std::vector<TString> labels;
  labels.push_back( (prefix+"_l"+FloatToStr(bins[0])).Data() );
  for (unsigned int i=1; i < bins.size(); i++) {
    labels.push_back( (prefix+"_g"+FloatToStr(bins[i-1])+"l"+FloatToStr(bins[i])).Data() );
  }
  labels.push_back( (prefix+"_g"+FloatToStr(bins[bins.size()-1])).Data() );
  return labels;
}

TString GlobalConfig::GetPtLabel(float pt, std::vector<float> ptBins, std::vector<TString> ptRegions) {
  unsigned int i=0;
  while (pt > ptBins[i]) {
    i++;
    if (i == ptBins.size()) break;
  }
  return ptRegions[i];
}

std::vector<TString> GlobalConfig::GetTrkJetRegions() {
  std::vector<TString> regions;
  for (TString muRegion : m_MuonJetRegions) {
    for (TString nonmuRegion : m_NonMuJetRegions) {
      regions.push_back(muRegion+"_"+nonmuRegion);
    }
  }
  return regions;
}

TString GlobalConfig::GetMCHistName(const TString sys, const TString ptLabel, const TString flav, const TString var) {
  TString _sys(sys); if (_sys!="") _sys=="Nom"; _sys+="_";
  TString _ptLabel(ptLabel); if (_ptLabel=="") _ptLabel=="Incl"; _ptLabel+="_";
  TString _flav(flav); if (_flav!="") _flav+="_";
  return "h_"+_sys+_ptLabel+_flav+var;
}

std::vector<TString> GlobalConfig::GetMCHistNamesBySys(const TString sys, const TString ptLabel, const TString var) {
  std::vector<TString> names;
  for (TString flav : m_FlavourPairs) names.push_back(GetMCHistName(sys,ptLabel,flav,var));
  return names;
}

std::map<TString,std::vector<TString> > GlobalConfig::GetMCHistNames(const TString ptLabel, const TString var) {
  std::map<TString,std::vector<TString> > names;
  for (TString sys : m_Systematics) names.emplace(sys,GetMCHistNamesBySys(sys,ptLabel,var));
  return names;
}

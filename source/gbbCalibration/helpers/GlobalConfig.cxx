#include "GlobalConfig.h"
#include "TEnv.h"
#include "TObjString.h"
#include "TObjArray.h"
#include "TRegexp.h"
#include "GbbUtil.h"

GlobalConfig::~GlobalConfig() {
  // TODO Auto-generated destructor stub
}

GlobalConfig::GlobalConfig(const TString& config_path) {

  TEnv* config = new TEnv("env");
  if (config->ReadFile(GbbUtil::findConfigFile(config_path).Data(),EEnvLevel(0)) == -1) {
    std::cout << "Could not read config file " << config_path.Data() << std::endl;
    abort();
  }

  m_isR20p7 = config->GetValue("isR20p7",false);
  std::cout<<"isR20p7: "<<m_isR20p7<<std::endl;

  m_Systematics_R20p7 = SplitString(config->GetValue("Systematics_R20p7",""),',');
  std::cout<<"Systematics_R20p7: "<<config->GetValue("Systematics_R20p7","")<<std::endl;

  m_Systematics_R21 = SplitString(config->GetValue("Systematics_R21",""),',');
  std::cout<<"Systematics_R21: "<<config->GetValue("Systematics_R21","")<<std::endl;

  m_Systematics_Sd0 = SplitString(config->GetValue("Systematics_Sd0",""),',');
  std::cout<<"Systematics_Sd0: "<<config->GetValue("Systematics_Sd0","")<<std::endl;

  m_Systematics_WeightVar = SplitString(config->GetValue("Systematics_WeightVar",""),',');
  std::cout<<"Systematics_WeightVar: "<<config->GetValue("Systematics_WeightVar","")<<std::endl;

  m_doMergeFlavours = config->GetValue("doMergeFlavours",true);
  std::cout<<"doMergeFlavours: "<<m_doMergeFlavours<<std::endl;

  m_FlavourPairs = SplitString(config->GetValue("FlavourPairs",""),',');
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

  m_TemplateVariables = SplitString(config->GetValue("TemplateVariables",""),',');
  std::cout<<"TemplateVariable: "<<config->GetValue("TemplateVariables","")<<std::endl;

  m_PlotVariables = SplitString(config->GetValue("PlotVariables",""),',');
  std::cout<<"PlotVariables: "<<config->GetValue("PlotVariables","")<<std::endl;

  for (TString var : m_PlotVariables) {
    std::vector<float> tempBins = SplitStringD(config->GetValue(("PlotBins."+var).Data(),""),',');
    std::vector<double> binning; //TH1::Rebin only works with double values
    // Format is either exactly 3 values (min,max,step) or a list of bin edges
    if (tempBins.size() == 3) {
      for (double bin = tempBins[0]; bin <= tempBins[1]; bin += tempBins[2]) binning.push_back(bin);
    } else {
      for (float bin : tempBins) binning.push_back((double)bin);
    }
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

std::vector<double> GlobalConfig::GetBinning(const TString var) {
  TString temp = var;
  // If var has a tag, like _2TAG, then remove it
  if (temp.Contains(TRegexp("_[A-Z0-9]+"))) temp = temp(0,temp.Index(TRegexp("_[A-Z0-9]+")));
  // Check the binning map for var
  if (m_PlotBinning.find(temp) != m_PlotBinning.end())
    return m_PlotBinning[temp];
  else return std::vector<double>();
}

char GlobalConfig::GetFlavour(int truthType) {
  switch (std::abs(truthType)) {
    case 5:
      return 'B';
      break;
    case 4:
      return 'C';
      break;
    case 0:
      return 'L';
      break;
    case 15:
      return 'T';
      break;
    default:
      std::cout<<"GlobalConfig - Unrecognized truth type: "<<truthType<<std::endl;
      return 'O';
      break;
  }
}

TString GlobalConfig::GetFlavourPair(int muJetTruth, int nonMuJetTruth) {
  TString output = "";
  output += GetFlavour(muJetTruth);
  output += GetFlavour(nonMuJetTruth);
  if (output.Contains("O") || output.Contains("T")) return "Other";
  if (!m_doMergeFlavours) return output;
  else {
    //FIXME: Hard-coding this is sub-optimal if categories ever change
    if (output == "BC") return "BL";
    else if (output == "CB") return "CL";
    else if (output == "LB") return "LL";
    else if (output == "LC") return "LL";
    else return output;
  }
}

TString GlobalConfig::GetMCHistName(const TString sys, const TString ptLabel, const TString flav, const TString var) {
  TString _sys(sys); if (_sys!="") _sys=="Nom";
  TString _ptLabel(ptLabel); if (_ptLabel=="") _ptLabel=="Incl";
  //TString _flav(flav); if (_flav!="") _flav+="_";
  return "h_"+_ptLabel+"_"+var+"_"+flav+"_"+_sys;
  //return "h"+flav+_sys+"_"+_ptLabel+"_"+var;
}

std::vector<TString> GlobalConfig::GetMCHistNamesBySys(const TString sys, const TString ptLabel, const TString var) {
  std::vector<TString> names;
  for (TString flav : m_FlavourPairs) names.push_back(GetMCHistName(sys,ptLabel,flav,var));
  return names;
}

std::map<TString,std::vector<TString> > GlobalConfig::GetMCHistNames(const TString ptLabel, const TString var) {
  std::map<TString,std::vector<TString> > names;
  if (m_isR20p7) {
    for (TString sys : m_Systematics_R20p7) names.emplace(sys,GetMCHistNamesBySys(sys,ptLabel,var));
  } else {
    for (TString sys : m_Systematics_R21) names.emplace(sys,GetMCHistNamesBySys(sys,ptLabel,var));
  }
  for (TString sys : m_Systematics_Sd0) names.emplace(sys,GetMCHistNamesBySys(sys,ptLabel,var));
  for (TString sys : m_Systematics_WeightVar) names.emplace(sys,GetMCHistNamesBySys("Nom",ptLabel,var+"_"+sys));
  return names;
}

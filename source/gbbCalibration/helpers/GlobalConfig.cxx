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

  m_Systematics_R20p7 = GbbUtil::splitString(config->GetValue("Systematics_R20p7",""),',');
  std::cout<<"Systematics_R20p7: "<<config->GetValue("Systematics_R20p7","")<<std::endl;

  m_Systematics_R21 = GbbUtil::splitString(config->GetValue("Systematics_R21",""),',');
  std::cout<<"Systematics_R21: "<<config->GetValue("Systematics_R21","")<<std::endl;

  m_Systematics_Sd0 = GbbUtil::splitString(config->GetValue("Systematics_Sd0",""),',');
  std::cout<<"Systematics_Sd0: "<<config->GetValue("Systematics_Sd0","")<<std::endl;

  m_Systematics_WeightVar = GbbUtil::splitString(config->GetValue("Systematics_WeightVar",""),',');
  std::cout<<"Systematics_WeightVar: "<<config->GetValue("Systematics_WeightVar","")<<std::endl;

  m_MergeMap = std::map<TString, std::unordered_set<TString> >();
  TString mergeMap = config->GetValue("MergeMap","");
  std::cout<<"MergeMap: "<<mergeMap.Data()<<std::endl;
  for (TString entry : GbbUtil::splitString(mergeMap,'|')) {
    auto keyval = GbbUtil::splitString(entry,':');
    if (keyval.size() != 2) {
      std::cout<<"Could not extract key-value pair from "<<keyval.data()<<std::endl;
      abort();
    }
    std::vector<TString> flavs = GbbUtil::splitString(keyval[1],',');
    m_MergeMap.emplace(std::make_pair(keyval[0], std::unordered_set<TString>(flavs.begin(), flavs.end())));
    m_FlavourGroups.push_back(keyval[0]);
  }

  m_mergeFlavours = config->GetValue("mergeFlavours",false);
  std::cout<<"mergeFlavours: "<<m_mergeFlavours<<std::endl;

  m_useLeadingJets = config->GetValue("useLeadingJets",false);
  std::cout<<"useLeadingJets: "<<m_useLeadingJets<<std::endl;

  m_FatJetFlavours = GbbUtil::splitString(config->GetValue("FatJetFlavours",""),',');
  std::cout<<"FatJetFlavours: "<<config->GetValue("FatJetFlavours","")<<std::endl;
  if (m_FatJetFlavours.size() == 0) {
    for (char c1 : {'B','C','L'}) {
      for (char c2 : {'B','C','L'}) {
        for (char c3 : {'B','C','L','x'}) {
          m_FatJetFlavours.push_back(Form("%c%c%c",c1,c2,c3));
        }
      }
      m_FatJetFlavours.push_back(Form("%cxx",c1));
    }
  }

  std::cout<<"TrkJetPtBins: "<<config->GetValue("TrkJetPtBins","")<<std::endl;
  std::vector<TString> ptBinsStrV = GbbUtil::splitString(config->GetValue("TrkJetPtBins",""),'|');
  if (ptBinsStrV.size() < 2)
    throw std::invalid_argument("TrkJetPtBins must contain at least 2 sets of bins");
  for (unsigned int i=0; i < ptBinsStrV.size(); i++) {
    auto ptBinsStr = ptBinsStrV[i];
    std::vector<float> ptBins = GbbUtil::splitStringD(ptBinsStr,',');
    m_TrkJetPtBins.push_back(ptBins);
    m_TrkJetRegions.push_back(MakeLabels(ptBins, GetTrkJetName(i)+"pt"));
  }

  m_FatJetPtBins = GbbUtil::splitStringD(config->GetValue("FatJetPtBins",""),',');
  std::cout<<"FatJetPtBins: "<<config->GetValue("FatJetPtBins","")<<std::endl;
  m_FatJetRegions = MakeLabels(m_FatJetPtBins, "fjpt");

  m_TemplateVariables = GbbUtil::splitString(config->GetValue("TemplateVariables",""),',');
  std::cout<<"TemplateVariable: "<<config->GetValue("TemplateVariables","")<<std::endl;

  m_PlotVariables = GbbUtil::splitString(config->GetValue("PlotVariables",""),',');
  std::cout<<"PlotVariables: "<<config->GetValue("PlotVariables","")<<std::endl;

  for (TString var : m_PlotVariables) {
    std::vector<float> tempBins = GbbUtil::splitStringD(config->GetValue(("PlotBins."+var).Data(),""),',');
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

std::vector<TString> GlobalConfig::GetDiTrkJetRegions() {
  std::vector<TString> regions;
  for (TString muRegion : m_TrkJetRegions[0]) {
    for (TString nonmuRegion : m_TrkJetRegions[1]) {
      regions.push_back(muRegion+"_"+nonmuRegion);
    }
  }
  return regions;
}

std::vector<TString> GlobalConfig::GetTrkJetRegions(unsigned int nJets) {
  std::vector<TString> regions;
  auto addRegion = [this, &regions, nJets](TString prev, unsigned int idx, const auto& ref) -> void {
    for (TString region : m_TrkJetRegions[std::min((int)idx, 2)]) {
      if (idx == nJets-1) regions.push_back(prev+"_"+region);
      else ref(prev+"_"+region, idx+1, ref);
    }
  };
  addRegion("", 0, addRegion);
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
    case 99:
      return 'x';
      break;
    default:
      std::cout<<"GlobalConfig - Unrecognized truth type: "<<truthType<<std::endl;
      return 'O';
      break;
  }
}

TString GlobalConfig::GetFatJetFlavour(int jetTruth1, int jetTruth2, int jetTruth3) {
  TString output = "";
  output += GetFlavour(jetTruth1);
  output += GetFlavour(jetTruth2);
  output += GetFlavour(jetTruth3);
  if (output.Contains("O") || output.Contains("T")) return "Other";
  if (m_mergeFlavours) {
    return GetFlavourGroup(output);
  } else {
    return output;
  }
}

std::unordered_set<TString> GlobalConfig::GetFlavoursInGroup(const TString key) {
  if (m_MergeMap.find(key) != m_MergeMap.end()) {
    return m_MergeMap[key];
  } else {
    return std::unordered_set<TString>();
  }
}

TString GlobalConfig::GetFlavourGroup(const TString key) {
  for (auto iter = m_MergeMap.begin(); iter != m_MergeMap.end(); ++iter) {
    if (iter->second.find(key) != iter->second.end()) {
      return iter->first;
    }
  }
  return "";
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
  if (m_mergeFlavours) {
    for (TString flav : m_FlavourGroups) names.push_back(GetMCHistName(sys,ptLabel,flav,var));
  } else {
    for (TString flav : m_FatJetFlavours) names.push_back(GetMCHistName(sys,ptLabel,flav,var));
  }
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

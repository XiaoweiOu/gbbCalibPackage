#include "BJetTagger.h"
#include "helpers/GbbUtil.h"
#include "AsgToolTagger.h"
#include "XbbScoreTagger.h"

BJetTagger::BJetTagger(TString taggerName, TString operatingPt) :
    m_taggerName(taggerName), m_operatingPt(operatingPt) {
}

BJetTagger::~BJetTagger() {
}

BJetTagger* TaggerFactory::Create(TString config) {
  auto configV = GbbUtil::splitString(config,'|');
  if (configV.size() < 2)
    throw std::invalid_argument(Form("invalid config string %s",config.Data()));

  auto taggerName = configV[0];
  auto operatingPt = configV[1];
  float xbbTopFrac = 0.2;
  if (configV.size() > 2) {
    xbbTopFrac = configV[2].Atof();
  }

  if (taggerName == "XbbScore") {
    return new XbbScoreTagger(operatingPt,xbbTopFrac);
    // TODO: implement case for custom cut values (or cut functions)
  } else {
    return new AsgToolTagger(taggerName,operatingPt);
  }
}

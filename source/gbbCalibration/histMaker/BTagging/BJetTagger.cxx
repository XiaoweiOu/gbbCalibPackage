#include "BJetTagger.h"
#include "gbbUtil.h"
#include "AsgToolTagger.h"
#include "XbbScoreTagger.h"

BJetTagger::BJetTagger(std::string taggerName, std::string operatingPt) :
    m_taggerName(taggerName), m_operatingPt(operatingPt) {
}

BJetTagger::~BJetTagger() {
}

BJetTagger* TaggerFactory::Create(std::string config) {
  auto configV = GbbUtil::splitString(config,"|");
  if (configV.size() < 2) {
    std::cerr << "ERROR : invalid config string " << config.c_str() << std::endl;
    return nullptr;
  }
  auto taggerName = configV[0];
  auto operatingPt = configV[1];
  float xbbTopFrac = 0.2;
  if (configV.size() > 2) {
    xbbTopFrac = std::stof(configV[2]);
  }

  if (taggerName == "XbbScore") {
    return new XbbScoreTagger(operatingPt,xbbTopFrac);
    // TODO: implement case for custom cut values (or cut functions)
  } else {
    return new AsgToolTagger(taggerName,operatingPt);
  }
}

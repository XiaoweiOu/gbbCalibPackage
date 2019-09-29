
#include "AsgToolTagger.h"
#include "helpers/GbbUtil.h"
#include <AsgTools/MessageCheck.h>
#include "CalibrationDataInterface/CalibrationDataVariables.h"

AsgToolTagger::AsgToolTagger(std::string taggerName, std::string operatingPt) :
  BJetTagger(taggerName, operatingPt),
  v_pt(nullptr),
  v_eta(nullptr),
  v_truth(nullptr),
  v_tagWgt(nullptr) {
}

void AsgToolTagger::setFlags(const std::vector<bool>& flags) {
  size_t n = flags.size();
  if (n > 0) m_isMC = flags[0];
  if (n > 1) m_useVRTrkJets = flags[1];
}

// Set up ASG tools
int AsgToolTagger::initialize(const TupleAna& gbbtuple) {
  if (m_useVRTrkJets) {
    m_jetAuthor = "AntiKtVR30Rmax4Rmin02TrackJets";
  }

  // get address of containers for relevant variables
  // WARNING: no protection from TupleAna deleting these pointers
  // this is bad practice, so be careful
  v_pt    = gbbtuple.trkjet_pt;
  v_eta   = gbbtuple.trkjet_eta;
  v_truth = gbbtuple.trkjet_truth;
  if (m_taggerName == "MV2c10") {
    v_tagWgt = gbbtuple.trkjet_MV2c10;
  // TODO: implement other cases
  } else {
    return -1;
  }

  ANA_CHECK( m_BJetSelectTool_handle.setProperty("MaxEta",2.5));
  ANA_CHECK( m_BJetSelectTool_handle.setProperty("MinPt",20000.));
  ANA_CHECK( m_BJetSelectTool_handle.setProperty("FlvTagCutDefinitionsFileName",m_corrFileName.c_str()));
  // configurable parameters
  ANA_CHECK( m_BJetSelectTool_handle.setProperty("TaggerName",          m_taggerName));
  ANA_CHECK( m_BJetSelectTool_handle.setProperty("OperatingPoint",      m_operatingPt));
  ANA_CHECK( m_BJetSelectTool_handle.setProperty("JetAuthor",           m_jetAuthor));
  ANA_CHECK( m_BJetSelectTool_handle.setProperty("OutputLevel", msg().level() ));
  ANA_CHECK( m_BJetSelectTool_handle.retrieve());
  ANA_MSG_DEBUG("Retrieved tool: " << m_BJetSelectTool_handle);

  // initialize the BJetEfficiencyCorrectionTool
  ANA_CHECK( m_BJetEffSFTool_handle.setProperty("TaggerName",          m_taggerName));
  ANA_CHECK( m_BJetEffSFTool_handle.setProperty("SystematicsStrategy", m_systematicsStrategy ));
  ANA_CHECK( m_BJetEffSFTool_handle.setProperty("OperatingPoint",      m_operatingPt));
  ANA_CHECK( m_BJetEffSFTool_handle.setProperty("JetAuthor",           m_jetAuthor));
  ANA_CHECK( m_BJetEffSFTool_handle.setProperty("ScaleFactorFileName", m_corrFileName));
  ANA_CHECK( m_BJetEffSFTool_handle.setProperty("UseDevelopmentFile",  m_useDevelopmentFile));
  ANA_CHECK( m_BJetEffSFTool_handle.setProperty("ConeFlavourLabel",    m_coneFlavourLabel));
  ANA_CHECK( m_BJetEffSFTool_handle.setProperty("OutputLevel", msg().level() ));
  //if(gbbtuple.eve_isMC) {
  //  // Pythia8
  //  std::string calibration = "410501";
  //  //    case HelperFunctions::Pythia8:
  //  //calibration="410501";
  //  //    case HelperFunctions::Herwig7:
  //  //calibration="410558";
  //  //    case HelperFunctions::Sherpa21:
  //  //calibration="426131";
  //  //    case HelperFunctions::Sherpa22:
  //  //calibration="410250";
  //  ANA_CHECK( m_BJetEffSFTool_handle.setProperty("EfficiencyBCalibrations"    ,  calibration));
  //  ANA_CHECK( m_BJetEffSFTool_handle.setProperty("EfficiencyCCalibrations"    ,  calibration));
  //  ANA_CHECK( m_BJetEffSFTool_handle.setProperty("EfficiencyTCalibrations"    ,  calibration));
  //  ANA_CHECK( m_BJetEffSFTool_handle.setProperty("EfficiencyLightCalibrations",  calibration));
  //}
  ANA_CHECK( m_BJetEffSFTool_handle.retrieve());
  ANA_MSG_DEBUG("Retrieved tool: " << m_BJetEffSFTool_handle);

  return 0;
}

bool AsgToolTagger::accept(const unsigned int idx, float& SF) {

  bool tag = false;
  CP::CorrectionCode BJetEffCode;
  Analysis::CalibrationDataVariables vars;
  float pt = v_pt->at(idx);
  float eta = v_eta->at(idx);
  float truth = v_truth->at(idx);
  float tagWgt = v_tagWgt->at(idx);

  vars.jetPt = pt;
  vars.jetEta = eta;
  vars.jetTagWeight = tagWgt;
  vars.jetAuthor = m_jetAuthor;

  // Get result and SF from asg tools
  // if passes cut take the efficiency scale factor
  // if failed cut take the inefficiency scale factor
  tag = m_BJetSelectTool_handle->accept(pt,eta,tagWgt);

  if (m_isMC) {
    if (tag) {
      BJetEffCode = m_BJetEffSFTool_handle->getScaleFactor( truth, vars, SF );
    } else {
      BJetEffCode = m_BJetEffSFTool_handle->getInefficiencyScaleFactor( truth, vars, SF );
    }
    if (BJetEffCode == CP::CorrectionCode::Error) {
      ANA_MSG_ERROR( "Error in getEfficiencyScaleFactor");
      return -1;
    } else if (BJetEffCode == CP::CorrectionCode::OutOfValidityRange) {
      ANA_MSG_DEBUG( "Jet is out of validity range");
    }
  }

  return tag;
}

int AsgToolTagger::accept(const GbbCandidate& gbbcand, float& mjSF, float& nmjSF) {
  int tag = 0;
  if (accept(gbbcand.ind_mj,  mjSF) ) tag++;
  if (accept(gbbcand.ind_nmj, nmjSF)) tag++;
  return tag;
}

int AsgToolTagger::accept(const GbbCandidate& gbbcand) {
  float sf1(-1.), sf2(-1);
  return accept(gbbcand, sf1, sf2);
}

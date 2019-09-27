#ifndef ASGTOOLTAGGER_H_
#define ASGTOOLTAGGER_H_

#include "BJetTagger.h"

#include <AsgTools/MsgStream.h>
#include "AsgTools/AnaToolHandle.h"
#include "FTagAnalysisInterfaces/IBTaggingSelectionTool.h"
#include "FTagAnalysisInterfaces/IBTaggingEfficiencyTool.h"

class AsgToolTagger : public BJetTagger {
 public:
  AsgToolTagger(std::string taggerName, std::string operatingPt);

  int initialize(const TupleAna& gbbtuple);
  int accept(const GbbCandidate& gbbcand);
  int accept(const GbbCandidate& gbbcand, float& mjSF, float& nmjSF);
  void setFlags(const std::vector<bool>& flags);

  // Static object that provides athena-based message logging functionality
  MsgStream& msg( MSG::Level lvl = MSG::INFO ) {
    static MsgStream msgStream( "BJetTagger" );
    msgStream << lvl;
    return msgStream;
  }

 private:
  bool m_isMC;
  bool m_useVRTrkJets;
  std::string m_corrFileName = "xAODBTaggingEfficiency/13TeV/2017-21-13TeV-MC16-CDI-2019-07-30_v1.root";

  std::string m_jetAuthor = "AntiKt2PV0TrackJets";
  std::string m_taggerName = "MV2c10";
  // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/BTaggingCalibrationDataInterface#xAOD_interface
  std::string m_operatingPt = "FixedCutBEff_70";
  bool        m_useDevelopmentFile = true;
  bool        m_coneFlavourLabel = true;
  std::string m_systematicsStrategy = "SFEigen";

  std::vector<float>* v_pt;
  std::vector<float>* v_eta;
  std::vector<int>*   v_truth;
  std::vector<float>* v_tagWgt;

  bool accept(const unsigned int idx, float& SF);

  // tools
  asg::AnaToolHandle<IBTaggingSelectionTool> m_BJetSelectTool_handle{"BTaggingSelectionTool"}; //!
  asg::AnaToolHandle<IBTaggingEfficiencyTool> m_BJetEffSFTool_handle{"BTaggingEfficiencyTool"}; //!
};

#endif /*ASGTOOLTAGGER_H_*/

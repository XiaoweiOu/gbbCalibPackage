#ifndef BJETTAGGER_H_
#define BJETTAGGER_H_

#include <TString.h>
#include "../TupleAna.h"
#include "../GbbStructures.h"
#include <stdexcept>

class BJetTagger {
 public:
  BJetTagger(TString taggerName, TString operatingPt);
  virtual ~BJetTagger();

  virtual int initialize(const TupleAna& gbbtuple) = 0;
  virtual int accept(const GbbCandidate& gbbcand) = 0;
  virtual int accept(const GbbCandidate& gbbcand, float& sf) = 0;
  virtual void setFlags(const std::vector<bool>& flags) = 0;

 protected:
  TString m_taggerName;
  TString m_operatingPt;
};

class TaggerFactory {
 public:
  static BJetTagger* Create(TString config);
};

#endif /*BJETTAGGER_H_*/

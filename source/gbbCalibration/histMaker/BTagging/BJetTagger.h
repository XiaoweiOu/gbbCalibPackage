#ifndef BJETTAGGER_H_
#define BJETTAGGER_H_

#include <string>
#include "../TupleAna.h"
#include "../GbbStructures.h"

class BJetTagger {
 public:
  BJetTagger(std::string taggerName, std::string operatingPt);
  virtual ~BJetTagger();

  virtual int initialize(const TupleAna& gbbtuple) = 0;
  virtual int accept(const GbbCandidate& gbbcand) = 0;
  virtual int accept(const GbbCandidate& gbbcand, float& mjSF, float& nmjSF) = 0;
  virtual void setFlags(const std::vector<bool>& flags) = 0;

 protected:
  std::string m_taggerName;
  std::string m_operatingPt;
};

class TaggerFactory {
 public:
  static BJetTagger* Create(std::string config);
};

#endif /*BJETTAGGER_H_*/

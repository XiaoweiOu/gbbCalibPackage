#ifndef PruningUtil_H
#define PruningUtil_H

#include "TH1F.h"
#include <iostream>

class PruningUtil {
public:
    PruningUtil();
    ~PruningUtil();

    // 0 = sample-by-sample, 1 = relative to tot background, 2 = relative to tot S+B
    void SetStrategy(int strat);
    void SetThresholdNorm(double thres);
    void SetThresholdShape(double thres);
    void SetThresholdIsLarge(double thres);
    
    // if hTot is not set, just prun w.r.t. only this nominal, 
    // otherwise use hTot and do relative pruning
    // return scheme:
    // 0 : nothing pruned
    // 1 : shape pruned
    // 2 : norm pruned
    // 3 : all pruned
    // -2 : bad norm
    // -3 : bad shape
    // -4 : all bad
    int CheckSystPruning(const TH1* const hUp,const TH1* const hDown,const TH1* const hNom,const TH1* hTot=nullptr);
    bool HasShapeRelative(const TH1* const hNom, const TH1* const hUp, const TH1* const hDown, const TH1* const combined, double threshold) const;
    
// private:
    int fStrategy;
    double fThresholdNorm;
    double fThresholdShape;
    double fThresholdIsLarge;
};

#endif

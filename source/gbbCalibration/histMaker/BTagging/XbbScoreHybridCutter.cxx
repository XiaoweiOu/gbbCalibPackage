

#include "XbbScoreHybridCutter.h"
#include <stdexcept>
#include <cmath> // for log
// need p_h, p_qcd, p_top
// p_h = higgs
int XbbScoreHybridCutter::cut(float p_h, float p_qcd, float p_top, float fat_pt_val) {
  // D in the formula
  // calculate from the formula D given in the doc
  float candidateValue = std::log( p_h / ( (1 - f_)*p_qcd + f_ * p_top) );
  // compare with cutValue and decided if it is btagged
  float cutValue;
  // comput cutValue
  if ( f_ == 0.2 ){
    cutValue = XbbTag_antiQCDtop_flat60eff(fat_pt_val);
  } else if ( f_ == 0 ) {
    cutValue = XbbTag_antiQCD_flat60eff(fat_pt_val);
  } else{
    throw std::invalid_argument("invalide f in xbbscoreHybrid" );
  }
  
  if(candidateValue > cutValue) {
    // Double-b-tagged
    return 1;
  } else {
    // Not double-b-tagged
    return -1;
  }
}  


double XbbScoreHybridCutter::XbbTag_antiQCD_flat60eff(double x) {

  const int fNp = 12, fKstep = 0;
  const double fDelta = -1, fXmin = 0, fXmax = 2750;
  const double fX[12] = { 0, 250, 500, 750, 1000,
                       1250, 1500, 1750, 2000, 2250,
                       2500, 2750 };
  const double fY[12] = { 0, 4.75, 4.875, 4.875, 4.5,
                       4, 3, 1.25, -1.375, -5,
                       -10.375, -14.125 };
  const double fB[12] = { 0.0360118, 0.00586912, -0.000988246, -0.000416141, -0.00184719,
                       -0.00269509, -0.00537245, -0.00881512, -0.0118671, -0.0187165,
                       -0.0212667, -0.00571655 };
  const double fC[12] = { -8.35705e-05, -3.7e-05, 9.57053e-06, -7.28211e-06, 1.5579e-06,
                       -4.94949e-06, -5.75995e-06, -8.01073e-06, -4.19714e-06, -2.32007e-05,
                       1.3e-05, 250 };
  const double fD[12] = { 6.2094e-08, 6.2094e-08, -2.24702e-08, 1.17867e-08, -8.67652e-09,
                       -1.08061e-09, -3.00104e-09, 5.08479e-09, -2.53381e-08, 4.82676e-08,
                       4.82676e-08, 116.025 };
  int klow=0;
  if(x<=fXmin) klow=0;
  else if(x>=fXmax) klow=fNp-1;
  else {
    if(fKstep) {
      // Equidistant knots, use histogramming
      klow = int((x-fXmin)/fDelta);
      if (klow < fNp-1) klow = fNp-1;
    } else {
      int khig=fNp-1, khalf;
      // Non equidistant knots, binary search
      while(khig-klow>1) {
        if(x>fX[khalf=(klow+khig)/2]) klow=khalf;
        else khig=khalf;
      }
    }
  }
  // Evaluate now
  double dx=x-fX[klow];
  return (fY[klow]+dx*(fB[klow]+dx*(fC[klow]+dx*fD[klow])));
}

double XbbScoreHybridCutter::XbbTag_antiQCDtop_flat60eff(double x) {
   // 20% fraction for top
   const int fNp = 12, fKstep = 0;
   const double fDelta = -1, fXmin = 0, fXmax = 2750;
   const double fX[12] = { 0, 250, 500, 750, 1000,
                        1250, 1500, 1750, 2000, 2250,
                        2500, 2750 };
   const double fY[12] = { 0, 4, 4.125, 4.125, 3.625,
                        2.875, 1.875, 0.5, -1.625, -5,
                        -10.25, -15.75 };
   const double fB[12] = { 0.0302896, 0.0049802, -0.000710406, -0.000638579, -0.00273528,
                        -0.0034203, -0.00458351, -0.00674566, -0.0104339, -0.0175189,
                        -0.0229906, -0.0195189 };
   const double fC[12] = { -7.02376e-05, -3.1e-05, 8.23756e-06, -7.95025e-06, -4.36552e-07,
                        -2.30354e-06, -2.34929e-06, -6.2993e-06, -8.45352e-06, -1.98866e-05,
                        -2e-06, 250 };
   const double fD[12] = { 5.23168e-08, 5.23168e-08, -2.15838e-08, 1.00183e-08, -2.48932e-09,
                        -6.10022e-11, -5.26667e-09, -2.8723e-09, -1.52441e-08, 2.38488e-08,
                        2.38488e-08, 116.025 };
   int klow=0;
   if(x<=fXmin) klow=0;
   else if(x>=fXmax) klow=fNp-1;
   else {
     if(fKstep) {
       // Equidistant knots, use histogramming
       klow = int((x-fXmin)/fDelta);
       if (klow < fNp-1) klow = fNp-1;
     } else {
       int khig=fNp-1, khalf;
       // Non equidistant knots, binary search
       while(khig-klow>1)
         if(x>fX[khalf=(klow+khig)/2]) klow=khalf;
         else khig=khalf;
     }
   }
   // Evaluate now
   double dx=x-fX[klow];
   return (fY[klow]+dx*(fB[klow]+dx*(fC[klow]+dx*fD[klow])));
}

// // what is this?? not used??
// double XbbTag_spline_flatQCDrejection_mH_0(double x) {
//    const int fNp = 11, fKstep = 0;
//    const double fDelta = -1, fXmin = 0, fXmax = 185;
//    const double fX[11] = { 0, 50, 65, 80, 95,
//                         110, 125, 140, 155, 170,
//                         185 };
//    const double fY[11] = { 0, 2.875, 3.625, 4.75, 5.25,
//                         5.5, 5.375, 4.75, 4.5, 3.75,
//                         3.625 };
//    const double fB[11] = { 0.137238, 0.0346611, 0.0704599, 0.0584995, 0.0205421,
//                         0.00933219, -0.0328708, -0.0278489, -0.0307337, -0.0492165,
//                         0.0525997 };
//    const double fC[11] = { -0.00273274, 0.000681202, 0.00170538, -0.00250274, -2.77557e-05,
//                         -0.00071957, -0.00209396, 0.00242876, -0.00262108, 0.00138889,
//                         15 };
//    const double fD[11] = { 2.27596e-05, 2.27596e-05, -9.35139e-05, 5.49997e-05, -1.53737e-05,
//                         -3.05421e-05, 0.000100505, -0.000112219, 8.91104e-05, 8.91104e-05,
//                         6.96152 };
//    int klow=0;
//    if(x<=fXmin) klow=0;
//    else if(x>=fXmax) klow=fNp-1;
//    else {
//      if(fKstep) {
//        // Equidistant knots, use histogramming
//        klow = int((x-fXmin)/fDelta);
//        if (klow < fNp-1) klow = fNp-1;
//      } else {
//        int khig=fNp-1, khalf;
//        // Non equidistant knots, binary search
//        while(khig-klow>1)
//          if(x>fX[khalf=(klow+khig)/2]) klow=khalf;
//          else khig=khalf;
//      }
//    }
//    // Evaluate now
//    double dx=x-fX[klow];
//    return (fY[klow]+dx*(fB[klow]+dx*(fC[klow]+dx*fD[klow])));
// }

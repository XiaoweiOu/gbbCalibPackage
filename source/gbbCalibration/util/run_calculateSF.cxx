/*
 * Main for ScaleFactorCalculator
 * Ruth Jacobs 12/01/19
 *
 */

#include "gbbCalibration/ScaleFactorCalculator.h"

int main(int argc, const char* argv[] ){

  TString configname;

  if(argc==1){
    
    configname="./configs/testconfig.cfg";
    std::cout<<"No arguments, run on test file!"<<std::endl;
    
  }else{
    
    configname=argv[1];

  }


  ScaleFactorCalculator sf_calculator(configname);
  


}


#include "XbbScoreCutter.h"
#include <iostream>

#include <memory> // for unique pointer

int main(){

  std::unique_ptr<XbbScoreCutter> x(new XbbScoreCutter(0.2,60));

  std::cout<< x->getCutValue(60, 0.2)<<std::endl;

  // std::unique_ptr<XbbScoreCutter> y(new XbbScoreCutter(0.3,60));
}

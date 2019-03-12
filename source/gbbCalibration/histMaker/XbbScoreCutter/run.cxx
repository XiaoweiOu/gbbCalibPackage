
#include "XbbScoreCutter.h"
#include <iostream>

#include <memory> // for unique pointer

#include "gbbUtil.h"

#include "BTagger.h"

int main(){

  BTagger bt("tagger=XbbScore|f=0.2|eff=60");
  std::cout<< bt.xbbScoreCutter_->f_ << std::endl;
  std::cout<< bt.xbbScoreCutter_->eff_ << std::endl;

  
  // std::unique_ptr<std::map<std::string,std::string>> mp
  //   (GbbUtil::splitWholeThenEach("tagger=XbbScore|f=0.2|eff=60","|","="));
  //  for(auto elem : *mp)
  //   {
  //     std::cout << elem.first << " " << elem.second << "\n";
  //   }
  
  // std::unique_ptr<XbbScoreCutter> x(new XbbScoreCutter(0.2,60));
  // std::cout<< x->getCutValue(60, 0.2)<<std::endl;
  //std::unique_ptr<std::vector<std::string>> vp(GbbUtil::splitString
  //					       ("tagger=XbbScore|f=0.2|eff=60","|"));

  //for(std::string s : *vp){
    // std::cout<< s << std::endl;
  //}					   
  //std::unique_ptr<std::map<std::string,std::string>> mp(GbbUtil::splitEach(*vp,"="));
  

  
}

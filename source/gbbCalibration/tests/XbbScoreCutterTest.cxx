




#include <histMaker/XbbScoreCutter/XbbScoreCutter.h>
// #include <assert.h>
// list functions here
void run_XbbScoreCutterTests();
void TestConstructor();



void run_XbbScoreCutterTests(){
  TestConstructor();
}


void TestConstructor(){

  std::unique_ptr<XbbScoreCutter> x(new XbbScoreCutter(0.2,60));
  std::unique_ptr<std::vector<std::string>> vp(GbbUtil::splitString
  					       ("tagger=XbbScore|f=0.2|eff=60","|"));
  
  for(std::string s : *vp){
     std::cout<< s << std::endl;
  }					   
  
}

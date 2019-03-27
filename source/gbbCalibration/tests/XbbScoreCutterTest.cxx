




#include <histMaker/XbbScoreCutter/XbbScoreCutter.h>
#include "TestUtil.h"
#include <string>

namespace xbbscorecuttertests{
// list functions here
void run_XbbScoreCutterTests();
void TestConstructor();
void TestCut();



void run_XbbScoreCutterTests(){
  println("run XbbScoreCutterTests");
  TestConstructor();
  TestCut();
  println("  ");
}


void TestConstructor(){

  // test all valid inputs
  assertTrue(XbbScoreCutter(0,50).cutValue_ == float(5.1));
  assertTrue(XbbScoreCutter(0,60).cutValue_ == float(4.8));
  assertTrue(XbbScoreCutter(0,70).cutValue_ == float(3.9));
  assertTrue(XbbScoreCutter(0.2,50).cutValue_ == float(4.5));
  assertTrue(XbbScoreCutter(0.2,60).cutValue_ == float(3.9));
  assertTrue(XbbScoreCutter(0.2,70).cutValue_ == float(3.0));
  assertTrue(XbbScoreCutter(1,50).cutValue_ == float(3.6));
  assertTrue(XbbScoreCutter(1,60).cutValue_ == float(3.0));
  assertTrue(XbbScoreCutter(1,70).cutValue_ == float(2.1));

  // test invalid input
  try{
    XbbScoreCutter(0.3,50);//should fail
    assertTrue(false); // the test fail print stack trace
  }catch( const std::invalid_argument& e ) {
    // println("success");
  }

  try{
    XbbScoreCutter(0.2,100);//should fail
    assertTrue(false); // the test fail print stack trace
  }catch( const std::invalid_argument& e ) {
    // println("success");
  }

  
  println("test Constructor pass");
  
}


void TestCut(){

  float f = 0;
  XbbScoreCutter x(f,50);

  float p_h = 10;
  float p_qcd = 20;
  float p_top = 30;
  float d = std::log(p_h/((1-f)*p_qcd + f*p_top));

  int num = d > x.cutValue_ ? 1 : -1;
  assertTrue(num == x.cut(p_h,p_qcd,p_top));

  println( "test cut pass");


}


}

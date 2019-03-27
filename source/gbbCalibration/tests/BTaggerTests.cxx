#include <histMaker/XbbScoreCutter/BTagger.h>
#include "TestUtil.h"
#include <string>


namespace btaggertests{
// declaration
  void run_BTaggerTests();
  void TestConstructor();
  void TestConstructor2();
  void TestTag();




  void run_BTaggerTests(){
    println("run BTaggerTests");
    TestConstructor();
    TestConstructor2();
    //TestCut();
    TestTag();
  }

  void TestConstructor(){

    BTagger x("tagger=XbbScore|f=0.2|eff=60",false);
    // println(x.getTaggerType()); 
    assertTrue(x.getTaggerType() == "XbbScore");
    assertTrue(x.getEff() == "60");
    assertTrue((*(x.config_))["f"] == "0.2");


    // test invalid input
    try{
      BTagger("tagger=badtype",false);//should fail
      assertTrue(false); // the test fail print stack trace
    }catch( const std::invalid_argument& e ) {
      // println("success");
    }

    try{
      BTagger("tagger=XbbScore|f=0.15|eff=60",false);//should fail
      assertTrue(false); // the test fail print stack trace
    }catch( const std::invalid_argument& e ) {
      //println(e.what());
    }

    try{
      BTagger("tagger=XbbScore|f=0.2|eff=badvalue",false);//should fail
      assertTrue(false); // the test fail print stack trace
    }catch( const std::invalid_argument& e ) {
      //println(e.what());
    }

    try{
      BTagger("tagger=XbbScore|f=badvalue|eff=60",false);//should fail
      assertTrue(false); // the test fail print stack trace
    }catch( const std::invalid_argument& e ) {
      //println(e.what());
    }
    

    println("text Constructor pass");
  }


  void TestConstructor2(){
    
    BTagger y("tagger=MV2c10|eff=60",false);
    assertTrue(y.getTaggerType() == "MV2c10");
    assertTrue(y.getEff() == "60");

    // try{
    //   BTagger("tagger=XbbScore|f=0.15|eff=60",false);//should fail
    //   assertTrue(false); // the test fail print stack trace
    // }catch( const std::invalid_argument& e ) {
    //   println(e.what());
    // }


    try{
      BTagger("tagger=badvalue|f=0.2|eff=60",false);//should fail
      assertTrue(false); // the test fail print stack trace
    }catch( const std::invalid_argument& e ) {
      //println(e.what());
    }

    try{
      BTagger("tagger=MV2C10|f=0.2|eff=60",false);//should fail
      assertTrue(false); // the test fail print stack trace
    }catch( const std::invalid_argument& e ) {
      //println(e.what());
    }

    //to do test more mv2c10
    
    println("text Constructor2 pass");
  }


  void TestTag(){
    BTagger x("tagger=XbbScore|f=0.2|eff=60",false);

    // todo not able to test because gbb coupling
  }
  




} // end namespace btaggertests

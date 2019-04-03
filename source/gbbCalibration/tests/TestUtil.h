
#ifndef TESTUTIL_H_
#define TESTUTIL_H_
// this file includes functions useful for testing.


#include <stdexcept>
// try
#include <boost/stacktrace.hpp>
#include <string>
#include <iostream>

void assertTrue(bool b){

  if(b != true){
    // assertion fail, print out stacktrace
    std::cerr << "assertion failed" << std::endl;
    std::cout << boost::stacktrace::stacktrace();
    exit(1);
  }
  
}

void println(std::string s){
  std::cout<< s << std::endl;
}




#endif

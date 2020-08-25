// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME G__ExoStats

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Since CINT ignores the std namespace, we need to do so in this file.
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonStatTools/RooExpandedFitResult.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void delete_RooExpandedFitResult(void *p);
   static void deleteArray_RooExpandedFitResult(void *p);
   static void destruct_RooExpandedFitResult(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::RooExpandedFitResult*)
   {
      ::RooExpandedFitResult *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::RooExpandedFitResult >(0);
      static ::ROOT::TGenericClassInfo 
         instance("RooExpandedFitResult", ::RooExpandedFitResult::Class_Version(), "TRExFitter/CommonStatTools/RooExpandedFitResult.h", 29,
                  typeid(::RooExpandedFitResult), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::RooExpandedFitResult::Dictionary, isa_proxy, 4,
                  sizeof(::RooExpandedFitResult) );
      instance.SetDelete(&delete_RooExpandedFitResult);
      instance.SetDeleteArray(&deleteArray_RooExpandedFitResult);
      instance.SetDestructor(&destruct_RooExpandedFitResult);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::RooExpandedFitResult*)
   {
      return GenerateInitInstanceLocal((::RooExpandedFitResult*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::RooExpandedFitResult*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr RooExpandedFitResult::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *RooExpandedFitResult::Class_Name()
{
   return "RooExpandedFitResult";
}

//______________________________________________________________________________
const char *RooExpandedFitResult::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RooExpandedFitResult*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int RooExpandedFitResult::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RooExpandedFitResult*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *RooExpandedFitResult::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RooExpandedFitResult*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *RooExpandedFitResult::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RooExpandedFitResult*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void RooExpandedFitResult::Streamer(TBuffer &R__b)
{
   // Stream an object of class RooExpandedFitResult.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(RooExpandedFitResult::Class(),this);
   } else {
      R__b.WriteClassBuffer(RooExpandedFitResult::Class(),this);
   }
}

namespace ROOT {
   // Wrapper around operator delete
   static void delete_RooExpandedFitResult(void *p) {
      delete ((::RooExpandedFitResult*)p);
   }
   static void deleteArray_RooExpandedFitResult(void *p) {
      delete [] ((::RooExpandedFitResult*)p);
   }
   static void destruct_RooExpandedFitResult(void *p) {
      typedef ::RooExpandedFitResult current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::RooExpandedFitResult

namespace {
  void TriggerDictionaryInitialization_libExoStats_Impl() {
    static const char* headers[] = {
"/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonStatTools/RooExpandedFitResult.h",
0
    };
    static const char* includePaths[] = {
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/RootCore/include",
"/home/ouxiaowei/gbbCalibPackage/source",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBaseExternals/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/include",
"",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBaseExternals/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/include",
"/home/ouxiaowei/gbbCalibPackage/build/TRExFitter/CommonStatTools/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "libExoStats dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate(R"ATTRDUMP(Container class for expanded fit result)ATTRDUMP"))) __attribute__((annotate("$clingAutoload$/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonStatTools/RooExpandedFitResult.h")))  RooExpandedFitResult;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "libExoStats dictionary payload"

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif
#ifndef HAVE_PRETTY_FUNCTION
  #define HAVE_PRETTY_FUNCTION 1
#endif
#ifndef HAVE_64_BITS
  #define HAVE_64_BITS 1
#endif
#ifndef __IDENTIFIER_64BIT__
  #define __IDENTIFIER_64BIT__ 1
#endif
#ifndef ATLAS
  #define ATLAS 1
#endif
#ifndef ROOTCORE
  #define ROOTCORE 1
#endif
#ifndef XAOD_STANDALONE
  #define XAOD_STANDALONE 1
#endif
#ifndef XAOD_ANALYSIS
  #define XAOD_ANALYSIS 1
#endif
#ifndef ROOTCORE
  #define ROOTCORE 1
#endif
#ifndef XAOD_STANDALONE
  #define XAOD_STANDALONE 1
#endif
#ifndef XAOD_ANALYSIS
  #define XAOD_ANALYSIS 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonStatTools/RooExpandedFitResult.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"RooExpandedFitResult", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libExoStats",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libExoStats_Impl, {}, classesHeaders, /*has no C++ module*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libExoStats_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libExoStats() {
  TriggerDictionaryInitialization_libExoStats_Impl();
}

// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME CommonSystSmoothingToolLibCintDict

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
#include "SmoothSystematics/PlotHist.h"
#include "SmoothSystematics/SmoothHist.h"
#include "SmoothSystematics/SmoothingTool.h"
#include "SmoothSystematics/TRExTools.h"
#include "SmoothSystematics/TRExTools.h"
#include "SmoothSystematics/SmoothHist.h"
#include "SmoothSystematics/PlotHist.h"
#include "SmoothSystematics/SmoothingTool.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static TClass *SmoothHist_Dictionary();
   static void SmoothHist_TClassManip(TClass*);
   static void *new_SmoothHist(void *p = 0);
   static void *newArray_SmoothHist(Long_t size, void *p);
   static void delete_SmoothHist(void *p);
   static void deleteArray_SmoothHist(void *p);
   static void destruct_SmoothHist(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SmoothHist*)
   {
      ::SmoothHist *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::SmoothHist));
      static ::ROOT::TGenericClassInfo 
         instance("SmoothHist", "SmoothSystematics/SmoothHist.h", 22,
                  typeid(::SmoothHist), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &SmoothHist_Dictionary, isa_proxy, 4,
                  sizeof(::SmoothHist) );
      instance.SetNew(&new_SmoothHist);
      instance.SetNewArray(&newArray_SmoothHist);
      instance.SetDelete(&delete_SmoothHist);
      instance.SetDeleteArray(&deleteArray_SmoothHist);
      instance.SetDestructor(&destruct_SmoothHist);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SmoothHist*)
   {
      return GenerateInitInstanceLocal((::SmoothHist*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::SmoothHist*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *SmoothHist_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::SmoothHist*)0x0)->GetClass();
      SmoothHist_TClassManip(theClass);
   return theClass;
   }

   static void SmoothHist_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *PlotHist_Dictionary();
   static void PlotHist_TClassManip(TClass*);
   static void *new_PlotHist(void *p = 0);
   static void *newArray_PlotHist(Long_t size, void *p);
   static void delete_PlotHist(void *p);
   static void deleteArray_PlotHist(void *p);
   static void destruct_PlotHist(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::PlotHist*)
   {
      ::PlotHist *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::PlotHist));
      static ::ROOT::TGenericClassInfo 
         instance("PlotHist", "SmoothSystematics/PlotHist.h", 15,
                  typeid(::PlotHist), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &PlotHist_Dictionary, isa_proxy, 4,
                  sizeof(::PlotHist) );
      instance.SetNew(&new_PlotHist);
      instance.SetNewArray(&newArray_PlotHist);
      instance.SetDelete(&delete_PlotHist);
      instance.SetDeleteArray(&deleteArray_PlotHist);
      instance.SetDestructor(&destruct_PlotHist);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::PlotHist*)
   {
      return GenerateInitInstanceLocal((::PlotHist*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::PlotHist*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *PlotHist_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::PlotHist*)0x0)->GetClass();
      PlotHist_TClassManip(theClass);
   return theClass;
   }

   static void PlotHist_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *SmoothingTool_Dictionary();
   static void SmoothingTool_TClassManip(TClass*);
   static void *new_SmoothingTool(void *p = 0);
   static void *newArray_SmoothingTool(Long_t size, void *p);
   static void delete_SmoothingTool(void *p);
   static void deleteArray_SmoothingTool(void *p);
   static void destruct_SmoothingTool(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SmoothingTool*)
   {
      ::SmoothingTool *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::SmoothingTool));
      static ::ROOT::TGenericClassInfo 
         instance("SmoothingTool", "SmoothSystematics/SmoothingTool.h", 19,
                  typeid(::SmoothingTool), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &SmoothingTool_Dictionary, isa_proxy, 4,
                  sizeof(::SmoothingTool) );
      instance.SetNew(&new_SmoothingTool);
      instance.SetNewArray(&newArray_SmoothingTool);
      instance.SetDelete(&delete_SmoothingTool);
      instance.SetDeleteArray(&deleteArray_SmoothingTool);
      instance.SetDestructor(&destruct_SmoothingTool);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SmoothingTool*)
   {
      return GenerateInitInstanceLocal((::SmoothingTool*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::SmoothingTool*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *SmoothingTool_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::SmoothingTool*)0x0)->GetClass();
      SmoothingTool_TClassManip(theClass);
   return theClass;
   }

   static void SmoothingTool_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_SmoothHist(void *p) {
      return  p ? new(p) ::SmoothHist : new ::SmoothHist;
   }
   static void *newArray_SmoothHist(Long_t nElements, void *p) {
      return p ? new(p) ::SmoothHist[nElements] : new ::SmoothHist[nElements];
   }
   // Wrapper around operator delete
   static void delete_SmoothHist(void *p) {
      delete ((::SmoothHist*)p);
   }
   static void deleteArray_SmoothHist(void *p) {
      delete [] ((::SmoothHist*)p);
   }
   static void destruct_SmoothHist(void *p) {
      typedef ::SmoothHist current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::SmoothHist

namespace ROOT {
   // Wrappers around operator new
   static void *new_PlotHist(void *p) {
      return  p ? new(p) ::PlotHist : new ::PlotHist;
   }
   static void *newArray_PlotHist(Long_t nElements, void *p) {
      return p ? new(p) ::PlotHist[nElements] : new ::PlotHist[nElements];
   }
   // Wrapper around operator delete
   static void delete_PlotHist(void *p) {
      delete ((::PlotHist*)p);
   }
   static void deleteArray_PlotHist(void *p) {
      delete [] ((::PlotHist*)p);
   }
   static void destruct_PlotHist(void *p) {
      typedef ::PlotHist current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::PlotHist

namespace ROOT {
   // Wrappers around operator new
   static void *new_SmoothingTool(void *p) {
      return  p ? new(p) ::SmoothingTool : new ::SmoothingTool;
   }
   static void *newArray_SmoothingTool(Long_t nElements, void *p) {
      return p ? new(p) ::SmoothingTool[nElements] : new ::SmoothingTool[nElements];
   }
   // Wrapper around operator delete
   static void delete_SmoothingTool(void *p) {
      delete ((::SmoothingTool*)p);
   }
   static void deleteArray_SmoothingTool(void *p) {
      delete [] ((::SmoothingTool*)p);
   }
   static void destruct_SmoothingTool(void *p) {
      typedef ::SmoothingTool current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::SmoothingTool

namespace ROOT {
   static TClass *vectorlEstringgR_Dictionary();
   static void vectorlEstringgR_TClassManip(TClass*);
   static void *new_vectorlEstringgR(void *p = 0);
   static void *newArray_vectorlEstringgR(Long_t size, void *p);
   static void delete_vectorlEstringgR(void *p);
   static void deleteArray_vectorlEstringgR(void *p);
   static void destruct_vectorlEstringgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<string>*)
   {
      vector<string> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<string>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<string>", -2, "vector", 214,
                  typeid(vector<string>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEstringgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<string>) );
      instance.SetNew(&new_vectorlEstringgR);
      instance.SetNewArray(&newArray_vectorlEstringgR);
      instance.SetDelete(&delete_vectorlEstringgR);
      instance.SetDeleteArray(&deleteArray_vectorlEstringgR);
      instance.SetDestructor(&destruct_vectorlEstringgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<string> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<string>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEstringgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<string>*)0x0)->GetClass();
      vectorlEstringgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEstringgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEstringgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<string> : new vector<string>;
   }
   static void *newArray_vectorlEstringgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<string>[nElements] : new vector<string>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEstringgR(void *p) {
      delete ((vector<string>*)p);
   }
   static void deleteArray_vectorlEstringgR(void *p) {
      delete [] ((vector<string>*)p);
   }
   static void destruct_vectorlEstringgR(void *p) {
      typedef vector<string> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<string>

namespace ROOT {
   static TClass *vectorlEdoublegR_Dictionary();
   static void vectorlEdoublegR_TClassManip(TClass*);
   static void *new_vectorlEdoublegR(void *p = 0);
   static void *newArray_vectorlEdoublegR(Long_t size, void *p);
   static void delete_vectorlEdoublegR(void *p);
   static void deleteArray_vectorlEdoublegR(void *p);
   static void destruct_vectorlEdoublegR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<double>*)
   {
      vector<double> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<double>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<double>", -2, "vector", 214,
                  typeid(vector<double>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEdoublegR_Dictionary, isa_proxy, 0,
                  sizeof(vector<double>) );
      instance.SetNew(&new_vectorlEdoublegR);
      instance.SetNewArray(&newArray_vectorlEdoublegR);
      instance.SetDelete(&delete_vectorlEdoublegR);
      instance.SetDeleteArray(&deleteArray_vectorlEdoublegR);
      instance.SetDestructor(&destruct_vectorlEdoublegR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<double> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<double>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEdoublegR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<double>*)0x0)->GetClass();
      vectorlEdoublegR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEdoublegR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEdoublegR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<double> : new vector<double>;
   }
   static void *newArray_vectorlEdoublegR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<double>[nElements] : new vector<double>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEdoublegR(void *p) {
      delete ((vector<double>*)p);
   }
   static void deleteArray_vectorlEdoublegR(void *p) {
      delete [] ((vector<double>*)p);
   }
   static void destruct_vectorlEdoublegR(void *p) {
      typedef vector<double> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<double>

namespace ROOT {
   static TClass *maplEstringcOvectorlEstringgRsPgR_Dictionary();
   static void maplEstringcOvectorlEstringgRsPgR_TClassManip(TClass*);
   static void *new_maplEstringcOvectorlEstringgRsPgR(void *p = 0);
   static void *newArray_maplEstringcOvectorlEstringgRsPgR(Long_t size, void *p);
   static void delete_maplEstringcOvectorlEstringgRsPgR(void *p);
   static void deleteArray_maplEstringcOvectorlEstringgRsPgR(void *p);
   static void destruct_maplEstringcOvectorlEstringgRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const map<string,vector<string> >*)
   {
      map<string,vector<string> > *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(map<string,vector<string> >));
      static ::ROOT::TGenericClassInfo 
         instance("map<string,vector<string> >", -2, "map", 96,
                  typeid(map<string,vector<string> >), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &maplEstringcOvectorlEstringgRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(map<string,vector<string> >) );
      instance.SetNew(&new_maplEstringcOvectorlEstringgRsPgR);
      instance.SetNewArray(&newArray_maplEstringcOvectorlEstringgRsPgR);
      instance.SetDelete(&delete_maplEstringcOvectorlEstringgRsPgR);
      instance.SetDeleteArray(&deleteArray_maplEstringcOvectorlEstringgRsPgR);
      instance.SetDestructor(&destruct_maplEstringcOvectorlEstringgRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::MapInsert< map<string,vector<string> > >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const map<string,vector<string> >*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *maplEstringcOvectorlEstringgRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const map<string,vector<string> >*)0x0)->GetClass();
      maplEstringcOvectorlEstringgRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void maplEstringcOvectorlEstringgRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_maplEstringcOvectorlEstringgRsPgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) map<string,vector<string> > : new map<string,vector<string> >;
   }
   static void *newArray_maplEstringcOvectorlEstringgRsPgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) map<string,vector<string> >[nElements] : new map<string,vector<string> >[nElements];
   }
   // Wrapper around operator delete
   static void delete_maplEstringcOvectorlEstringgRsPgR(void *p) {
      delete ((map<string,vector<string> >*)p);
   }
   static void deleteArray_maplEstringcOvectorlEstringgRsPgR(void *p) {
      delete [] ((map<string,vector<string> >*)p);
   }
   static void destruct_maplEstringcOvectorlEstringgRsPgR(void *p) {
      typedef map<string,vector<string> > current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class map<string,vector<string> >

namespace {
  void TriggerDictionaryInitialization_libCommonSystSmoothingToolLib_Impl() {
    static const char* headers[] = {
"SmoothSystematics/PlotHist.h",
"SmoothSystematics/SmoothHist.h",
"SmoothSystematics/SmoothingTool.h",
"SmoothSystematics/TRExTools.h",
0
    };
    static const char* includePaths[] = {
"/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonSystSmoothingTool",
"/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonSystSmoothingTool",
"/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonSystSmoothingTool",
"/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonSystSmoothingTool",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBaseExternals/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/include",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/RootCore/include",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/RootCore/include",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/RootCore/include",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/RootCore/include",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBaseExternals/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/include",
"/home/ouxiaowei/gbbCalibPackage/build/TRExFitter/CommonSystSmoothingTool/CMakeFiles/makeCommonSystSmoothingToolLibCintDict.unbLSk/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "libCommonSystSmoothingToolLib dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate("$clingAutoload$SmoothSystematics/SmoothHist.h")))  SmoothHist;
class __attribute__((annotate("$clingAutoload$SmoothSystematics/PlotHist.h")))  PlotHist;
class __attribute__((annotate("$clingAutoload$SmoothSystematics/SmoothingTool.h")))  SmoothingTool;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "libCommonSystSmoothingToolLib dictionary payload"

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
#ifndef ROOTCORE_RELEASE_SERIES
  #define ROOTCORE_RELEASE_SERIES 25
#endif
#ifndef PACKAGE_VERSION
  #define PACKAGE_VERSION "CommonSystSmoothingTool-00-00-00"
#endif
#ifndef PACKAGE_VERSION_UQ
  #define PACKAGE_VERSION_UQ CommonSystSmoothingTool-00-00-00
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "SmoothSystematics/PlotHist.h"
#include "SmoothSystematics/SmoothHist.h"
#include "SmoothSystematics/SmoothingTool.h"
#include "SmoothSystematics/TRExTools.h"
#include "SmoothSystematics/TRExTools.h"
#include "SmoothSystematics/SmoothHist.h"
#include "SmoothSystematics/PlotHist.h"
#include "SmoothSystematics/SmoothingTool.h"

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

#pragma link C++ function TREx::Smooth_Ttres(TH1*, TH1*, bool)+;
#pragma link C++ function TREx::Smooth_maxVariations(TH1*, TH1*, int , double)+;
#pragma link C++ class SmoothHist+;
#pragma link C++ class PlotHist+;
#pragma link C++ class SmoothingTool+;

#endif

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"PlotHist", payloadCode, "@",
"SmoothHist", payloadCode, "@",
"SmoothingTool", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libCommonSystSmoothingToolLib",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libCommonSystSmoothingToolLib_Impl, {}, classesHeaders, /*has no C++ module*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libCommonSystSmoothingToolLib_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libCommonSystSmoothingToolLib() {
  TriggerDictionaryInitialization_libCommonSystSmoothingToolLib_Impl();
}

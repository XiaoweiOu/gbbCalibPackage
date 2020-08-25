// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME G__StatisticsTools

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
#include "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonStatTools/StatisticsTools/include/RooMultiVarGaussianHighPrecision.h"
#include "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonStatTools/StatisticsTools/include/ExtendedModel.hxx"
#include "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonStatTools/StatisticsTools/include/ExtendedMinimizer.hxx"
#include "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonStatTools/StatisticsTools/atlasrootstyle/AtlasLabels.h"
#include "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonStatTools/StatisticsTools/atlasrootstyle/AtlasStyle.h"
#include "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonStatTools/StatisticsTools/atlasrootstyle/AtlasUtils.h"
#include "utils.hxx"
#include "ExtendedModel.hxx"
#include "ExtendedMinimizer.hxx"
#include "RooMultiVarGaussianHighPrecision.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void *new_TOwnedList(void *p = 0);
   static void *newArray_TOwnedList(Long_t size, void *p);
   static void delete_TOwnedList(void *p);
   static void deleteArray_TOwnedList(void *p);
   static void destruct_TOwnedList(void *p);
   static Long64_t merge_TOwnedList(void *obj, TCollection *coll,TFileMergeInfo *info);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TOwnedList*)
   {
      ::TOwnedList *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::TOwnedList >(0);
      static ::ROOT::TGenericClassInfo 
         instance("TOwnedList", ::TOwnedList::Class_Version(), "utils.hxx", 88,
                  typeid(::TOwnedList), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::TOwnedList::Dictionary, isa_proxy, 4,
                  sizeof(::TOwnedList) );
      instance.SetNew(&new_TOwnedList);
      instance.SetNewArray(&newArray_TOwnedList);
      instance.SetDelete(&delete_TOwnedList);
      instance.SetDeleteArray(&deleteArray_TOwnedList);
      instance.SetDestructor(&destruct_TOwnedList);
      instance.SetMerge(&merge_TOwnedList);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TOwnedList*)
   {
      return GenerateInitInstanceLocal((::TOwnedList*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::TOwnedList*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void delete_ExtendedModel(void *p);
   static void deleteArray_ExtendedModel(void *p);
   static void destruct_ExtendedModel(void *p);
   static void streamer_ExtendedModel(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::ExtendedModel*)
   {
      ::ExtendedModel *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::ExtendedModel >(0);
      static ::ROOT::TGenericClassInfo 
         instance("ExtendedModel", ::ExtendedModel::Class_Version(), "ExtendedModel.hxx", 43,
                  typeid(::ExtendedModel), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::ExtendedModel::Dictionary, isa_proxy, 16,
                  sizeof(::ExtendedModel) );
      instance.SetDelete(&delete_ExtendedModel);
      instance.SetDeleteArray(&deleteArray_ExtendedModel);
      instance.SetDestructor(&destruct_ExtendedModel);
      instance.SetStreamerFunc(&streamer_ExtendedModel);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::ExtendedModel*)
   {
      return GenerateInitInstanceLocal((::ExtendedModel*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::ExtendedModel*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void delete_ExtendedMinimizer(void *p);
   static void deleteArray_ExtendedMinimizer(void *p);
   static void destruct_ExtendedMinimizer(void *p);
   static void streamer_ExtendedMinimizer(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::ExtendedMinimizer*)
   {
      ::ExtendedMinimizer *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::ExtendedMinimizer >(0);
      static ::ROOT::TGenericClassInfo 
         instance("ExtendedMinimizer", ::ExtendedMinimizer::Class_Version(), "ExtendedMinimizer.hxx", 52,
                  typeid(::ExtendedMinimizer), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::ExtendedMinimizer::Dictionary, isa_proxy, 16,
                  sizeof(::ExtendedMinimizer) );
      instance.SetDelete(&delete_ExtendedMinimizer);
      instance.SetDeleteArray(&deleteArray_ExtendedMinimizer);
      instance.SetDestructor(&destruct_ExtendedMinimizer);
      instance.SetStreamerFunc(&streamer_ExtendedMinimizer);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::ExtendedMinimizer*)
   {
      return GenerateInitInstanceLocal((::ExtendedMinimizer*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::ExtendedMinimizer*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_RooMultiVarGaussianHighPrecision(void *p = 0);
   static void *newArray_RooMultiVarGaussianHighPrecision(Long_t size, void *p);
   static void delete_RooMultiVarGaussianHighPrecision(void *p);
   static void deleteArray_RooMultiVarGaussianHighPrecision(void *p);
   static void destruct_RooMultiVarGaussianHighPrecision(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::RooMultiVarGaussianHighPrecision*)
   {
      ::RooMultiVarGaussianHighPrecision *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::RooMultiVarGaussianHighPrecision >(0);
      static ::ROOT::TGenericClassInfo 
         instance("RooMultiVarGaussianHighPrecision", ::RooMultiVarGaussianHighPrecision::Class_Version(), "RooMultiVarGaussianHighPrecision.h", 49,
                  typeid(::RooMultiVarGaussianHighPrecision), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::RooMultiVarGaussianHighPrecision::Dictionary, isa_proxy, 4,
                  sizeof(::RooMultiVarGaussianHighPrecision) );
      instance.SetNew(&new_RooMultiVarGaussianHighPrecision);
      instance.SetNewArray(&newArray_RooMultiVarGaussianHighPrecision);
      instance.SetDelete(&delete_RooMultiVarGaussianHighPrecision);
      instance.SetDeleteArray(&deleteArray_RooMultiVarGaussianHighPrecision);
      instance.SetDestructor(&destruct_RooMultiVarGaussianHighPrecision);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::RooMultiVarGaussianHighPrecision*)
   {
      return GenerateInitInstanceLocal((::RooMultiVarGaussianHighPrecision*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::RooMultiVarGaussianHighPrecision*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr TOwnedList::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *TOwnedList::Class_Name()
{
   return "TOwnedList";
}

//______________________________________________________________________________
const char *TOwnedList::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TOwnedList*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int TOwnedList::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TOwnedList*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *TOwnedList::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TOwnedList*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *TOwnedList::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TOwnedList*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr ExtendedModel::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *ExtendedModel::Class_Name()
{
   return "ExtendedModel";
}

//______________________________________________________________________________
const char *ExtendedModel::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::ExtendedModel*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int ExtendedModel::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::ExtendedModel*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *ExtendedModel::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::ExtendedModel*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *ExtendedModel::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::ExtendedModel*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr ExtendedMinimizer::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *ExtendedMinimizer::Class_Name()
{
   return "ExtendedMinimizer";
}

//______________________________________________________________________________
const char *ExtendedMinimizer::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::ExtendedMinimizer*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int ExtendedMinimizer::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::ExtendedMinimizer*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *ExtendedMinimizer::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::ExtendedMinimizer*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *ExtendedMinimizer::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::ExtendedMinimizer*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr RooMultiVarGaussianHighPrecision::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *RooMultiVarGaussianHighPrecision::Class_Name()
{
   return "RooMultiVarGaussianHighPrecision";
}

//______________________________________________________________________________
const char *RooMultiVarGaussianHighPrecision::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RooMultiVarGaussianHighPrecision*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int RooMultiVarGaussianHighPrecision::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RooMultiVarGaussianHighPrecision*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *RooMultiVarGaussianHighPrecision::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RooMultiVarGaussianHighPrecision*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *RooMultiVarGaussianHighPrecision::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RooMultiVarGaussianHighPrecision*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void TOwnedList::Streamer(TBuffer &R__b)
{
   // Stream an object of class TOwnedList.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(TOwnedList::Class(),this);
   } else {
      R__b.WriteClassBuffer(TOwnedList::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_TOwnedList(void *p) {
      return  p ? new(p) ::TOwnedList : new ::TOwnedList;
   }
   static void *newArray_TOwnedList(Long_t nElements, void *p) {
      return p ? new(p) ::TOwnedList[nElements] : new ::TOwnedList[nElements];
   }
   // Wrapper around operator delete
   static void delete_TOwnedList(void *p) {
      delete ((::TOwnedList*)p);
   }
   static void deleteArray_TOwnedList(void *p) {
      delete [] ((::TOwnedList*)p);
   }
   static void destruct_TOwnedList(void *p) {
      typedef ::TOwnedList current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around the merge function.
   static Long64_t  merge_TOwnedList(void *obj,TCollection *coll,TFileMergeInfo *) {
      return ((::TOwnedList*)obj)->Merge(coll);
   }
} // end of namespace ROOT for class ::TOwnedList

//______________________________________________________________________________
void ExtendedModel::Streamer(TBuffer &R__b)
{
   // Stream an object of class ExtendedModel.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TNamed::Streamer(R__b);
      { TString R__str; R__str.Streamer(R__b); fFileName = R__str.Data(); }
      { TString R__str; R__str.Streamer(R__b); fWsName = R__str.Data(); }
      { TString R__str; R__str.Streamer(R__b); fModelConfigName = R__str.Data(); }
      { TString R__str; R__str.Streamer(R__b); fDataName = R__str.Data(); }
      { TString R__str; R__str.Streamer(R__b); fSnapshotName = R__str.Data(); }
      R__b >> fBinnedLikelihood;
      { TString R__str; R__str.Streamer(R__b); fTagAsMeasurement = R__str.Data(); }
      R__b >> fFixCache;
      R__b >> fFixMulti;
      R__b >> fInterpolationCode;
      R__b >> fFile;
      R__b >> fWorkspace;
      R__b >> fModelConfig;
      R__b >> fPdf;
      R__b >> fData;
      R__b >> fNuis;
      R__b >> fGlobs;
      R__b >> fPOIs;
      R__b >> fObs;
      R__b.CheckByteCount(R__s, R__c, ExtendedModel::IsA());
   } else {
      R__c = R__b.WriteVersion(ExtendedModel::IsA(), kTRUE);
      TNamed::Streamer(R__b);
      { TString R__str = fFileName.c_str(); R__str.Streamer(R__b);}
      { TString R__str = fWsName.c_str(); R__str.Streamer(R__b);}
      { TString R__str = fModelConfigName.c_str(); R__str.Streamer(R__b);}
      { TString R__str = fDataName.c_str(); R__str.Streamer(R__b);}
      { TString R__str = fSnapshotName.c_str(); R__str.Streamer(R__b);}
      R__b << fBinnedLikelihood;
      { TString R__str = fTagAsMeasurement.c_str(); R__str.Streamer(R__b);}
      R__b << fFixCache;
      R__b << fFixMulti;
      R__b << fInterpolationCode;
      R__b << fFile;
      R__b << fWorkspace;
      R__b << fModelConfig;
      R__b << fPdf;
      R__b << fData;
      R__b << fNuis;
      R__b << fGlobs;
      R__b << fPOIs;
      R__b << fObs;
      R__b.SetByteCount(R__c, kTRUE);
   }
}

namespace ROOT {
   // Wrapper around operator delete
   static void delete_ExtendedModel(void *p) {
      delete ((::ExtendedModel*)p);
   }
   static void deleteArray_ExtendedModel(void *p) {
      delete [] ((::ExtendedModel*)p);
   }
   static void destruct_ExtendedModel(void *p) {
      typedef ::ExtendedModel current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_ExtendedModel(TBuffer &buf, void *obj) {
      ((::ExtendedModel*)obj)->::ExtendedModel::Streamer(buf);
   }
} // end of namespace ROOT for class ::ExtendedModel

//______________________________________________________________________________
void ExtendedMinimizer::Streamer(TBuffer &R__b)
{
   // Stream an object of class ExtendedMinimizer.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TNamed::Streamer(R__b);
      R__b >> fFile;
      R__b >> fPdf;
      R__b >> fData;
      R__b >> fNll;
      R__b >> fMinimizer;
      R__b >> fFitResult;
      fHesseMatrix.Streamer(R__b);
      R__b >> fEigenValues;
      R__b >> fEigenVectors;
      fFitCmdList.Streamer(R__b);
      fScanCmdList.Streamer(R__b);
      fNllCmdList.Streamer(R__b);
      R__b >> fOptConst;
      R__b >> fVerbose;
      R__b >> fSave;
      R__b >> fTimer;
      R__b >> fPrintLevel;
      R__b >> fDefaultStrategy;
      R__b >> fHesse;
      R__b >> fMinos;
      R__b >> fScan;
      R__b >> fNumee;
      R__b >> fDoEEWall;
      R__b >> fRetry;
      R__b >> fEigen;
      R__b >> fReuseMinimizer;
      R__b >> fReuseNLL;
      R__b >> fMaxCalls;
      R__b >> fMaxIters;
      R__b >> fEps;
      R__b >> fNsigma;
      R__b >> fPrecision;
      R__b >> fMinosSet;
      R__b >> fCondSet;
      R__b >> fScanSet;
      { TString R__str; R__str.Streamer(R__b); fMinimizerType = R__str.Data(); }
      { TString R__str; R__str.Streamer(R__b); fMinimizerAlgo = R__str.Data(); }
      R__b >> fMinNll;
      R__b.CheckByteCount(R__s, R__c, ExtendedMinimizer::IsA());
   } else {
      R__c = R__b.WriteVersion(ExtendedMinimizer::IsA(), kTRUE);
      TNamed::Streamer(R__b);
      R__b << fFile;
      R__b << fPdf;
      R__b << fData;
      R__b << fNll;
      R__b << fMinimizer;
      R__b << fFitResult;
      fHesseMatrix.Streamer(R__b);
      R__b << fEigenValues;
      R__b << fEigenVectors;
      fFitCmdList.Streamer(R__b);
      fScanCmdList.Streamer(R__b);
      fNllCmdList.Streamer(R__b);
      R__b << fOptConst;
      R__b << fVerbose;
      R__b << fSave;
      R__b << fTimer;
      R__b << fPrintLevel;
      R__b << fDefaultStrategy;
      R__b << fHesse;
      R__b << fMinos;
      R__b << fScan;
      R__b << fNumee;
      R__b << fDoEEWall;
      R__b << fRetry;
      R__b << fEigen;
      R__b << fReuseMinimizer;
      R__b << fReuseNLL;
      R__b << fMaxCalls;
      R__b << fMaxIters;
      R__b << fEps;
      R__b << fNsigma;
      R__b << fPrecision;
      R__b << fMinosSet;
      R__b << fCondSet;
      R__b << fScanSet;
      { TString R__str = fMinimizerType.c_str(); R__str.Streamer(R__b);}
      { TString R__str = fMinimizerAlgo.c_str(); R__str.Streamer(R__b);}
      R__b << fMinNll;
      R__b.SetByteCount(R__c, kTRUE);
   }
}

namespace ROOT {
   // Wrapper around operator delete
   static void delete_ExtendedMinimizer(void *p) {
      delete ((::ExtendedMinimizer*)p);
   }
   static void deleteArray_ExtendedMinimizer(void *p) {
      delete [] ((::ExtendedMinimizer*)p);
   }
   static void destruct_ExtendedMinimizer(void *p) {
      typedef ::ExtendedMinimizer current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_ExtendedMinimizer(TBuffer &buf, void *obj) {
      ((::ExtendedMinimizer*)obj)->::ExtendedMinimizer::Streamer(buf);
   }
} // end of namespace ROOT for class ::ExtendedMinimizer

//______________________________________________________________________________
void RooMultiVarGaussianHighPrecision::Streamer(TBuffer &R__b)
{
   // Stream an object of class RooMultiVarGaussianHighPrecision.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(RooMultiVarGaussianHighPrecision::Class(),this);
   } else {
      R__b.WriteClassBuffer(RooMultiVarGaussianHighPrecision::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_RooMultiVarGaussianHighPrecision(void *p) {
      return  p ? new(p) ::RooMultiVarGaussianHighPrecision : new ::RooMultiVarGaussianHighPrecision;
   }
   static void *newArray_RooMultiVarGaussianHighPrecision(Long_t nElements, void *p) {
      return p ? new(p) ::RooMultiVarGaussianHighPrecision[nElements] : new ::RooMultiVarGaussianHighPrecision[nElements];
   }
   // Wrapper around operator delete
   static void delete_RooMultiVarGaussianHighPrecision(void *p) {
      delete ((::RooMultiVarGaussianHighPrecision*)p);
   }
   static void deleteArray_RooMultiVarGaussianHighPrecision(void *p) {
      delete [] ((::RooMultiVarGaussianHighPrecision*)p);
   }
   static void destruct_RooMultiVarGaussianHighPrecision(void *p) {
      typedef ::RooMultiVarGaussianHighPrecision current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::RooMultiVarGaussianHighPrecision

namespace {
  void TriggerDictionaryInitialization_utils_Impl() {
    static const char* headers[] = {
"/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonStatTools/StatisticsTools/include/RooMultiVarGaussianHighPrecision.h",
"/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonStatTools/StatisticsTools/include/ExtendedModel.hxx",
"/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonStatTools/StatisticsTools/include/ExtendedMinimizer.hxx",
"/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonStatTools/StatisticsTools/atlasrootstyle/AtlasLabels.h",
"/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonStatTools/StatisticsTools/atlasrootstyle/AtlasStyle.h",
"/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonStatTools/StatisticsTools/atlasrootstyle/AtlasUtils.h",
0
    };
    static const char* includePaths[] = {
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/RootCore/include",
"/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonStatTools/StatisticsTools/include",
"/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonStatTools/StatisticsTools",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBaseExternals/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/include",
"",
"/home/ouxiaowei/gbbCalibPackage/build/external/Eigen/include/eigen3",
"",
"/home/ouxiaowei/gbbCalibPackage/build/external/yaml/include",
"",
"/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonStatTools/StatisticsTools/atlasrootstyle",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBaseExternals/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/include",
"/home/ouxiaowei/gbbCalibPackage/build/TRExFitter/CommonStatTools/StatisticsTools/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "utils dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
struct __attribute__((annotate("$clingAutoload$utils.hxx")))  TOwnedList;
class __attribute__((annotate("$clingAutoload$ExtendedModel.hxx")))  ExtendedModel;
class __attribute__((annotate("$clingAutoload$ExtendedMinimizer.hxx")))  ExtendedMinimizer;
class __attribute__((annotate(R"ATTRDUMP(Multivariate Gaussian PDF with correlations)ATTRDUMP"))) __attribute__((annotate("$clingAutoload$RooMultiVarGaussianHighPrecision.h")))  RooMultiVarGaussianHighPrecision;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "utils dictionary payload"

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
#include "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonStatTools/StatisticsTools/include/RooMultiVarGaussianHighPrecision.h"
#include "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonStatTools/StatisticsTools/include/ExtendedModel.hxx"
#include "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonStatTools/StatisticsTools/include/ExtendedMinimizer.hxx"
#include "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonStatTools/StatisticsTools/atlasrootstyle/AtlasLabels.h"
#include "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonStatTools/StatisticsTools/atlasrootstyle/AtlasStyle.h"
#include "/home/ouxiaowei/gbbCalibPackage/source/TRExFitter/CommonStatTools/StatisticsTools/atlasrootstyle/AtlasUtils.h"
#include "utils.hxx"
#include "ExtendedModel.hxx"
#include "ExtendedMinimizer.hxx"
#include "RooMultiVarGaussianHighPrecision.h"

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;

#pragma link C++ class TOwnedList+ ;
#pragma link C++ class ExtendedModel+;
#pragma link C++ class ExtendedMinimizer+;
#pragma link C++ class RooMultiVarGaussianHighPrecision+;

#endif // __CINT__

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"ExtendedMinimizer", payloadCode, "@",
"ExtendedModel", payloadCode, "@",
"RooMultiVarGaussianHighPrecision", payloadCode, "@",
"TOwnedList", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("utils",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_utils_Impl, {}, classesHeaders, /*has no C++ module*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_utils_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_utils() {
  TriggerDictionaryInitialization_utils_Impl();
}

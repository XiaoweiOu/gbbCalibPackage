// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME gbbCommonLibCintDict

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
#include "helpers/GbbUtil.h"
#include "helpers/GlobalConfig.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static TClass *GlobalConfig_Dictionary();
   static void GlobalConfig_TClassManip(TClass*);
   static void delete_GlobalConfig(void *p);
   static void deleteArray_GlobalConfig(void *p);
   static void destruct_GlobalConfig(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::GlobalConfig*)
   {
      ::GlobalConfig *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::GlobalConfig));
      static ::ROOT::TGenericClassInfo 
         instance("GlobalConfig", "helpers/GlobalConfig.h", 35,
                  typeid(::GlobalConfig), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &GlobalConfig_Dictionary, isa_proxy, 4,
                  sizeof(::GlobalConfig) );
      instance.SetDelete(&delete_GlobalConfig);
      instance.SetDeleteArray(&deleteArray_GlobalConfig);
      instance.SetDestructor(&destruct_GlobalConfig);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::GlobalConfig*)
   {
      return GenerateInitInstanceLocal((::GlobalConfig*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::GlobalConfig*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *GlobalConfig_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::GlobalConfig*)0x0)->GetClass();
      GlobalConfig_TClassManip(theClass);
   return theClass;
   }

   static void GlobalConfig_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrapper around operator delete
   static void delete_GlobalConfig(void *p) {
      delete ((::GlobalConfig*)p);
   }
   static void deleteArray_GlobalConfig(void *p) {
      delete [] ((::GlobalConfig*)p);
   }
   static void destruct_GlobalConfig(void *p) {
      typedef ::GlobalConfig current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::GlobalConfig

namespace ROOT {
   static TClass *vectorlEvectorlEfloatgRsPgR_Dictionary();
   static void vectorlEvectorlEfloatgRsPgR_TClassManip(TClass*);
   static void *new_vectorlEvectorlEfloatgRsPgR(void *p = 0);
   static void *newArray_vectorlEvectorlEfloatgRsPgR(Long_t size, void *p);
   static void delete_vectorlEvectorlEfloatgRsPgR(void *p);
   static void deleteArray_vectorlEvectorlEfloatgRsPgR(void *p);
   static void destruct_vectorlEvectorlEfloatgRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<vector<float> >*)
   {
      vector<vector<float> > *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<vector<float> >));
      static ::ROOT::TGenericClassInfo 
         instance("vector<vector<float> >", -2, "vector", 214,
                  typeid(vector<vector<float> >), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEvectorlEfloatgRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<vector<float> >) );
      instance.SetNew(&new_vectorlEvectorlEfloatgRsPgR);
      instance.SetNewArray(&newArray_vectorlEvectorlEfloatgRsPgR);
      instance.SetDelete(&delete_vectorlEvectorlEfloatgRsPgR);
      instance.SetDeleteArray(&deleteArray_vectorlEvectorlEfloatgRsPgR);
      instance.SetDestructor(&destruct_vectorlEvectorlEfloatgRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<vector<float> > >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<vector<float> >*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEvectorlEfloatgRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<vector<float> >*)0x0)->GetClass();
      vectorlEvectorlEfloatgRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEvectorlEfloatgRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEvectorlEfloatgRsPgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<vector<float> > : new vector<vector<float> >;
   }
   static void *newArray_vectorlEvectorlEfloatgRsPgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<vector<float> >[nElements] : new vector<vector<float> >[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEvectorlEfloatgRsPgR(void *p) {
      delete ((vector<vector<float> >*)p);
   }
   static void deleteArray_vectorlEvectorlEfloatgRsPgR(void *p) {
      delete [] ((vector<vector<float> >*)p);
   }
   static void destruct_vectorlEvectorlEfloatgRsPgR(void *p) {
      typedef vector<vector<float> > current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<vector<float> >

namespace ROOT {
   static TClass *vectorlEvectorlETStringgRsPgR_Dictionary();
   static void vectorlEvectorlETStringgRsPgR_TClassManip(TClass*);
   static void *new_vectorlEvectorlETStringgRsPgR(void *p = 0);
   static void *newArray_vectorlEvectorlETStringgRsPgR(Long_t size, void *p);
   static void delete_vectorlEvectorlETStringgRsPgR(void *p);
   static void deleteArray_vectorlEvectorlETStringgRsPgR(void *p);
   static void destruct_vectorlEvectorlETStringgRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<vector<TString> >*)
   {
      vector<vector<TString> > *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<vector<TString> >));
      static ::ROOT::TGenericClassInfo 
         instance("vector<vector<TString> >", -2, "vector", 214,
                  typeid(vector<vector<TString> >), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEvectorlETStringgRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<vector<TString> >) );
      instance.SetNew(&new_vectorlEvectorlETStringgRsPgR);
      instance.SetNewArray(&newArray_vectorlEvectorlETStringgRsPgR);
      instance.SetDelete(&delete_vectorlEvectorlETStringgRsPgR);
      instance.SetDeleteArray(&deleteArray_vectorlEvectorlETStringgRsPgR);
      instance.SetDestructor(&destruct_vectorlEvectorlETStringgRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<vector<TString> > >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<vector<TString> >*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEvectorlETStringgRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<vector<TString> >*)0x0)->GetClass();
      vectorlEvectorlETStringgRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEvectorlETStringgRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEvectorlETStringgRsPgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<vector<TString> > : new vector<vector<TString> >;
   }
   static void *newArray_vectorlEvectorlETStringgRsPgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<vector<TString> >[nElements] : new vector<vector<TString> >[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEvectorlETStringgRsPgR(void *p) {
      delete ((vector<vector<TString> >*)p);
   }
   static void deleteArray_vectorlEvectorlETStringgRsPgR(void *p) {
      delete [] ((vector<vector<TString> >*)p);
   }
   static void destruct_vectorlEvectorlETStringgRsPgR(void *p) {
      typedef vector<vector<TString> > current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<vector<TString> >

namespace ROOT {
   static TClass *vectorlEfloatgR_Dictionary();
   static void vectorlEfloatgR_TClassManip(TClass*);
   static void *new_vectorlEfloatgR(void *p = 0);
   static void *newArray_vectorlEfloatgR(Long_t size, void *p);
   static void delete_vectorlEfloatgR(void *p);
   static void deleteArray_vectorlEfloatgR(void *p);
   static void destruct_vectorlEfloatgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<float>*)
   {
      vector<float> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<float>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<float>", -2, "vector", 214,
                  typeid(vector<float>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEfloatgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<float>) );
      instance.SetNew(&new_vectorlEfloatgR);
      instance.SetNewArray(&newArray_vectorlEfloatgR);
      instance.SetDelete(&delete_vectorlEfloatgR);
      instance.SetDeleteArray(&deleteArray_vectorlEfloatgR);
      instance.SetDestructor(&destruct_vectorlEfloatgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<float> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<float>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEfloatgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<float>*)0x0)->GetClass();
      vectorlEfloatgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEfloatgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEfloatgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<float> : new vector<float>;
   }
   static void *newArray_vectorlEfloatgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<float>[nElements] : new vector<float>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEfloatgR(void *p) {
      delete ((vector<float>*)p);
   }
   static void deleteArray_vectorlEfloatgR(void *p) {
      delete [] ((vector<float>*)p);
   }
   static void destruct_vectorlEfloatgR(void *p) {
      typedef vector<float> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<float>

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
   static TClass *vectorlETStringgR_Dictionary();
   static void vectorlETStringgR_TClassManip(TClass*);
   static void *new_vectorlETStringgR(void *p = 0);
   static void *newArray_vectorlETStringgR(Long_t size, void *p);
   static void delete_vectorlETStringgR(void *p);
   static void deleteArray_vectorlETStringgR(void *p);
   static void destruct_vectorlETStringgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<TString>*)
   {
      vector<TString> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<TString>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<TString>", -2, "vector", 214,
                  typeid(vector<TString>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlETStringgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<TString>) );
      instance.SetNew(&new_vectorlETStringgR);
      instance.SetNewArray(&newArray_vectorlETStringgR);
      instance.SetDelete(&delete_vectorlETStringgR);
      instance.SetDeleteArray(&deleteArray_vectorlETStringgR);
      instance.SetDestructor(&destruct_vectorlETStringgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<TString> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<TString>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlETStringgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<TString>*)0x0)->GetClass();
      vectorlETStringgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlETStringgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlETStringgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TString> : new vector<TString>;
   }
   static void *newArray_vectorlETStringgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TString>[nElements] : new vector<TString>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlETStringgR(void *p) {
      delete ((vector<TString>*)p);
   }
   static void deleteArray_vectorlETStringgR(void *p) {
      delete [] ((vector<TString>*)p);
   }
   static void destruct_vectorlETStringgR(void *p) {
      typedef vector<TString> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<TString>

namespace ROOT {
   static TClass *unordered_setlETStringgR_Dictionary();
   static void unordered_setlETStringgR_TClassManip(TClass*);
   static void *new_unordered_setlETStringgR(void *p = 0);
   static void *newArray_unordered_setlETStringgR(Long_t size, void *p);
   static void delete_unordered_setlETStringgR(void *p);
   static void deleteArray_unordered_setlETStringgR(void *p);
   static void destruct_unordered_setlETStringgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const unordered_set<TString>*)
   {
      unordered_set<TString> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(unordered_set<TString>));
      static ::ROOT::TGenericClassInfo 
         instance("unordered_set<TString>", -2, "unordered_set", 93,
                  typeid(unordered_set<TString>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &unordered_setlETStringgR_Dictionary, isa_proxy, 0,
                  sizeof(unordered_set<TString>) );
      instance.SetNew(&new_unordered_setlETStringgR);
      instance.SetNewArray(&newArray_unordered_setlETStringgR);
      instance.SetDelete(&delete_unordered_setlETStringgR);
      instance.SetDeleteArray(&deleteArray_unordered_setlETStringgR);
      instance.SetDestructor(&destruct_unordered_setlETStringgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Insert< unordered_set<TString> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const unordered_set<TString>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *unordered_setlETStringgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const unordered_set<TString>*)0x0)->GetClass();
      unordered_setlETStringgR_TClassManip(theClass);
   return theClass;
   }

   static void unordered_setlETStringgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_unordered_setlETStringgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) unordered_set<TString> : new unordered_set<TString>;
   }
   static void *newArray_unordered_setlETStringgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) unordered_set<TString>[nElements] : new unordered_set<TString>[nElements];
   }
   // Wrapper around operator delete
   static void delete_unordered_setlETStringgR(void *p) {
      delete ((unordered_set<TString>*)p);
   }
   static void deleteArray_unordered_setlETStringgR(void *p) {
      delete [] ((unordered_set<TString>*)p);
   }
   static void destruct_unordered_setlETStringgR(void *p) {
      typedef unordered_set<TString> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class unordered_set<TString>

namespace ROOT {
   static TClass *maplETStringcOvectorlEdoublegRsPgR_Dictionary();
   static void maplETStringcOvectorlEdoublegRsPgR_TClassManip(TClass*);
   static void *new_maplETStringcOvectorlEdoublegRsPgR(void *p = 0);
   static void *newArray_maplETStringcOvectorlEdoublegRsPgR(Long_t size, void *p);
   static void delete_maplETStringcOvectorlEdoublegRsPgR(void *p);
   static void deleteArray_maplETStringcOvectorlEdoublegRsPgR(void *p);
   static void destruct_maplETStringcOvectorlEdoublegRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const map<TString,vector<double> >*)
   {
      map<TString,vector<double> > *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(map<TString,vector<double> >));
      static ::ROOT::TGenericClassInfo 
         instance("map<TString,vector<double> >", -2, "map", 96,
                  typeid(map<TString,vector<double> >), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &maplETStringcOvectorlEdoublegRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(map<TString,vector<double> >) );
      instance.SetNew(&new_maplETStringcOvectorlEdoublegRsPgR);
      instance.SetNewArray(&newArray_maplETStringcOvectorlEdoublegRsPgR);
      instance.SetDelete(&delete_maplETStringcOvectorlEdoublegRsPgR);
      instance.SetDeleteArray(&deleteArray_maplETStringcOvectorlEdoublegRsPgR);
      instance.SetDestructor(&destruct_maplETStringcOvectorlEdoublegRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::MapInsert< map<TString,vector<double> > >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const map<TString,vector<double> >*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *maplETStringcOvectorlEdoublegRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const map<TString,vector<double> >*)0x0)->GetClass();
      maplETStringcOvectorlEdoublegRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void maplETStringcOvectorlEdoublegRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_maplETStringcOvectorlEdoublegRsPgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) map<TString,vector<double> > : new map<TString,vector<double> >;
   }
   static void *newArray_maplETStringcOvectorlEdoublegRsPgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) map<TString,vector<double> >[nElements] : new map<TString,vector<double> >[nElements];
   }
   // Wrapper around operator delete
   static void delete_maplETStringcOvectorlEdoublegRsPgR(void *p) {
      delete ((map<TString,vector<double> >*)p);
   }
   static void deleteArray_maplETStringcOvectorlEdoublegRsPgR(void *p) {
      delete [] ((map<TString,vector<double> >*)p);
   }
   static void destruct_maplETStringcOvectorlEdoublegRsPgR(void *p) {
      typedef map<TString,vector<double> > current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class map<TString,vector<double> >

namespace ROOT {
   static TClass *maplETStringcOunordered_setlETStringgRsPgR_Dictionary();
   static void maplETStringcOunordered_setlETStringgRsPgR_TClassManip(TClass*);
   static void *new_maplETStringcOunordered_setlETStringgRsPgR(void *p = 0);
   static void *newArray_maplETStringcOunordered_setlETStringgRsPgR(Long_t size, void *p);
   static void delete_maplETStringcOunordered_setlETStringgRsPgR(void *p);
   static void deleteArray_maplETStringcOunordered_setlETStringgRsPgR(void *p);
   static void destruct_maplETStringcOunordered_setlETStringgRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const map<TString,unordered_set<TString> >*)
   {
      map<TString,unordered_set<TString> > *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(map<TString,unordered_set<TString> >));
      static ::ROOT::TGenericClassInfo 
         instance("map<TString,unordered_set<TString> >", -2, "map", 96,
                  typeid(map<TString,unordered_set<TString> >), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &maplETStringcOunordered_setlETStringgRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(map<TString,unordered_set<TString> >) );
      instance.SetNew(&new_maplETStringcOunordered_setlETStringgRsPgR);
      instance.SetNewArray(&newArray_maplETStringcOunordered_setlETStringgRsPgR);
      instance.SetDelete(&delete_maplETStringcOunordered_setlETStringgRsPgR);
      instance.SetDeleteArray(&deleteArray_maplETStringcOunordered_setlETStringgRsPgR);
      instance.SetDestructor(&destruct_maplETStringcOunordered_setlETStringgRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::MapInsert< map<TString,unordered_set<TString> > >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const map<TString,unordered_set<TString> >*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *maplETStringcOunordered_setlETStringgRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const map<TString,unordered_set<TString> >*)0x0)->GetClass();
      maplETStringcOunordered_setlETStringgRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void maplETStringcOunordered_setlETStringgRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_maplETStringcOunordered_setlETStringgRsPgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) map<TString,unordered_set<TString> > : new map<TString,unordered_set<TString> >;
   }
   static void *newArray_maplETStringcOunordered_setlETStringgRsPgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) map<TString,unordered_set<TString> >[nElements] : new map<TString,unordered_set<TString> >[nElements];
   }
   // Wrapper around operator delete
   static void delete_maplETStringcOunordered_setlETStringgRsPgR(void *p) {
      delete ((map<TString,unordered_set<TString> >*)p);
   }
   static void deleteArray_maplETStringcOunordered_setlETStringgRsPgR(void *p) {
      delete [] ((map<TString,unordered_set<TString> >*)p);
   }
   static void destruct_maplETStringcOunordered_setlETStringgRsPgR(void *p) {
      typedef map<TString,unordered_set<TString> > current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class map<TString,unordered_set<TString> >

namespace {
  void TriggerDictionaryInitialization_libgbbCommonLib_Impl() {
    static const char* headers[] = {
"helpers/GbbUtil.h",
"helpers/GlobalConfig.h",
0
    };
    static const char* includePaths[] = {
"/home/ouxiaowei/gbbCalibPackage/source/gbbCalibration",
"/home/ouxiaowei/gbbCalibPackage/source/gbbCalibration",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/src/Control/AthToolSupport/AsgTools",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/src/Control/xAODRootAccessInterfaces",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/src/Control/xAODRootAccess",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/src/Control/CxxUtils",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/src/Control/AthContainersInterfaces",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/src/Control/AthContainers",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/src/Control/AthLinksSA",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/src/Event/xAOD/xAODCore",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/src/Event/xAOD/xAODEventFormat",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/src/Tools/PathResolver",
"/home/ouxiaowei/gbbCalibPackage/source/gbbCalibration",
"/home/ouxiaowei/gbbCalibPackage/source/gbbCalibration",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBaseExternals/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/include",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/RootCore/include",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/RootCore/include",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/src/Tools/PathResolver",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/src/Control/AthToolSupport/AsgTools",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/src/Control/xAODRootAccessInterfaces",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/../../../../AnalysisBaseExternals/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/include",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/src/Control/xAODRootAccess",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/../../../../AnalysisBaseExternals/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/include",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/src/Control/AthContainers",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/src/Control/AthContainersInterfaces",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/../../../../AnalysisBaseExternals/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/include",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/src/Control/AthLinksSA",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/../../../../AnalysisBaseExternals/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/include",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/src/Control/CxxUtils",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/../../../../AnalysisBaseExternals/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/include",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/src/Event/xAOD/xAODCore",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/../../../../AnalysisBaseExternals/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/include",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/../../../../AnalysisBaseExternals/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/include",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/src/Event/xAOD/xAODEventFormat",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/RootCore/include",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/RootCore/include",
"/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBaseExternals/21.2.83/InstallArea/x86_64-slc6-gcc62-opt/include",
"/home/ouxiaowei/gbbCalibPackage/build/gbbCalibration/CMakeFiles/makegbbCommonLibCintDict.Aoop10/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "libgbbCommonLib dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate("$clingAutoload$helpers/GlobalConfig.h")))  GlobalConfig;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "libgbbCommonLib dictionary payload"

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
  #define PACKAGE_VERSION "gbbCalibration-00-00-00"
#endif
#ifndef PACKAGE_VERSION_UQ
  #define PACKAGE_VERSION_UQ gbbCalibration-00-00-00
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "helpers/GbbUtil.h"
#include "helpers/GlobalConfig.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"GlobalConfig", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libgbbCommonLib",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libgbbCommonLib_Impl, {}, classesHeaders, /*has no C++ module*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libgbbCommonLib_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libgbbCommonLib() {
  TriggerDictionaryInitialization_libgbbCommonLib_Impl();
}

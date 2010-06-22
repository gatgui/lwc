#ifndef __lwc_moduleutils_h__
#define __lwc_moduleutils_h__

#include <lwc/factory.h>

namespace lwc {
  
  // Help class to create a factory for a simple object type
  // "parent" attribute must also be a SimpleFactory
  template <typename T>
  class SimpleFactory : public Factory {
    public:
      
      SimpleFactory(MethodDecl *methods, size_t n, Factory *parent=0)
        : Factory() {
        mMethodsDecl = methods;
        mNumMethods = n;
        mMethods = new MethodsTable(parent ? parent->getMethods(0) : 0);
        mMethods->fromDeclaration(methods, n);
      }
      
      virtual ~SimpleFactory() {
        for (size_t i=0; i<mNumMethods; ++i) {
          delete mMethodsDecl[i].ptr;
        }
        delete mMethods;
      }
      
      virtual const MethodsTable* getMethods(const char *) {
        return mMethods;
      }
      
      virtual Object* create(const char *) {
        return new T();
      }
      
      virtual void destroy(Object *o) {
        if (o) {
          delete o;
        }
      }
      
    protected:
      
      MethodsTable *mMethods;
      size_t mNumMethods;
      MethodDecl *mMethodsDecl;
      Factory *mParent;
  };

}

#ifdef _WIN32
# define MODULE_EXPORT extern "C" __declspec(dllexport)
#else
# define MODULE_EXPORT extern "C"
#endif

#define LWC_BEGIN_MODULE(ntypes) \
  static const size_t gsNumTypes = ntypes;\
  static const char*  gsTypeNames[ntypes];\
  static lwc::Factory* gsFactories[ntypes];\
  \
  MODULE_EXPORT void LWC_ModuleInit() {

#define LWC_MODULE_TYPE(Index, Name, Type, MethodsDecl) \
    gsTypeNames[Index] = Name;\
    gsFactories[Index] = new lwc::SimpleFactory<Type>(MethodsDecl, LWC_NUMMETHODS(MethodsDecl), 0);

#define LWC_MODULE_DERIVED_TYPE(Index, Name, Type, MethodsDecl, ParentIndex) \
    gsTypeNames[Index] = Name;\
    gsFactories[Index] = new lwc::SimpleFactory<Type>(MethodsDecl, LWC_NUMMETHODS(MethodsDecl), gsFactories[ParentIndex]);

#define LWC_END_MODULE() \
  }\
  MODULE_EXPORT size_t LWC_ModuleGetTypeCount() {\
    return gsNumTypes;\
  }\
  MODULE_EXPORT const char* LWC_ModuleGetTypeName(size_t i) {\
    return (i < gsNumTypes ? gsTypeNames[i] : 0);\
  }\
  MODULE_EXPORT lwc::Factory* LWC_ModuleGetTypeFactory(size_t i) {\
    return (i < gsNumTypes ? gsFactories[i] : 0);\
  }\
  MODULE_EXPORT void LWC_ModuleExit() {\
    for (size_t i=0; i<gsNumTypes; ++i) {\
      delete gsFactories[gsNumTypes-1-i];\
    }\
  }

#endif

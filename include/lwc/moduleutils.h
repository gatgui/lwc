#ifndef __lwc_moduleutils_h__
#define __lwc_moduleutils_h__

#include <lwc/factory.h>

namespace lwc {
  
  // Helper class to create a factory for a simple object type
  // "parent" attribute must also be a SimpleFactory
  template <typename T>
  class SimpleFactory : public Factory {
    public:
      
      SimpleFactory(MethodDecl *methods, size_t n, bool singleton=false, const char *desc=0, Factory *parent=0)
        : Factory() {
        mMethodsDecl = methods;
        mNumMethods = n;
        mSingleton = singleton;
        mMethods = new MethodsTable(parent ? parent->getMethods(0) : 0);
        mMethods->fromDeclaration(methods, n);
        mDesc = (desc == 0 ? "" : desc);
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
      
      virtual bool isSingleton(const char *) {
        return mSingleton;
      }
      
      virtual const char* getDescription(const char *) {
        return mDesc.c_str();
      }
      
      virtual std::string docString(const char *n, const std::string &indent="") {
        std::ostringstream oss;
        oss << indent << n << ":" << std::endl;
        oss << indent << "  " << getDescription(n) << std::endl;
        oss << std::endl;
        oss << mMethods->docString(indent+"  ") << std::endl;
        return oss.str();
      }
      
    protected:
      
      MethodsTable *mMethods;
      size_t mNumMethods;
      MethodDecl *mMethodsDecl;
      Factory *mParent;
      bool mSingleton;
      std::string mDesc;
  };

}

#ifdef _WIN32
# define LWC_MODULE_EXPORT extern "C" __declspec(dllexport)
#else
# define LWC_MODULE_EXPORT extern "C" __attribute__ ((visibility ("default")))
#endif

#define LWC_BEGIN_MODULE(ntypes) \
  static const size_t gsNumTypes = ntypes;\
  static const char*  gsTypeNames[ntypes];\
  static lwc::Factory* gsFactories[ntypes];\
  \
  LWC_MODULE_EXPORT void LWC_ModuleInit() {

#define LWC_MODULE_TYPE(Index, Name, Type, MethodsDecl, Singleton, Description) \
    gsTypeNames[Index] = Name;\
    try {\
      gsFactories[Index] = new lwc::SimpleFactory<Type>(MethodsDecl, LWC_NUMMETHODS(MethodsDecl), Singleton, Description, 0);\
    } catch (std::exception &e) {\
      gsFactories[Index] = 0;\
      std::cout << "cloader: Failed to register type \"" << Name << "\": " << e.what() << std::endl;\
    }

#define LWC_MODULE_DERIVED_TYPE(Index, Name, Type, MethodsDecl, Singleton, Description, ParentIndex) \
    gsTypeNames[Index] = Name;\
    try {\
      gsFactories[Index] = new lwc::SimpleFactory<Type>(MethodsDecl, LWC_NUMMETHODS(MethodsDecl), Singleton, Description, gsFactories[ParentIndex]);\
    } catch (std::exception &e) {\
      gsFactories[Index] = 0;\
      std::cout << "cloader: Failed to register type \"" << Name << "\": " << e.what() << std::endl;\
    }

#define LWC_END_MODULE() \
  }\
  LWC_MODULE_EXPORT size_t LWC_ModuleGetTypeCount() {\
    return gsNumTypes;\
  }\
  LWC_MODULE_EXPORT const char* LWC_ModuleGetTypeName(size_t i) {\
    return (i < gsNumTypes ? gsTypeNames[i] : 0);\
  }\
  LWC_MODULE_EXPORT lwc::Factory* LWC_ModuleGetTypeFactory(size_t i) {\
    return (i < gsNumTypes ? gsFactories[i] : 0);\
  }\
  LWC_MODULE_EXPORT void LWC_ModuleExit() {\
    for (size_t i=0; i<gsNumTypes; ++i) {\
      if (gsFactories[gsNumTypes-1-i] != 0) {\
        delete gsFactories[gsNumTypes-1-i];\
      }\
    }\
  }

#endif

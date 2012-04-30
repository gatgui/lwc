/*

Copyright (C) 2009, 2010  Gaetan Guidet

This file is part of lwc.

lwc is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at
your option) any later version.

lwc is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
USA.

*/

#include <lwc/loader.h>
#include <lwc/registry.h>

// ---

typedef void (*InitFunc)();
typedef size_t (*GetNumTypesFunc)();
typedef const char* (*GetTypeNameFunc)(size_t);
typedef lwc::Factory* (*GetTypeFactoryFunc)(size_t);
typedef void (*ExitFunc)();

// ---

class CLoader : public lwc::Loader {
  public:
    
    CLoader() {
    }
    
    virtual ~CLoader() {
      for (size_t i=0; i<mModules.size(); ++i) {
        ExitFunc mexit = (ExitFunc) mModules[i]->_getSymbol("LWC_ModuleExit");
        if (mexit) {
          mexit();
        }
        delete mModules[i];
      }
      mModules.clear();
    }
    
    virtual bool canLoad(const gcore::Path &path) {
#ifdef _WIN32
      return path.checkExtension("dll");
#else
# ifdef __APPLE__
      return path.checkExtension("bundle");
# else
      return path.checkExtension("so");
# endif
#endif
    }
    
    virtual void load(const gcore::Path &path, lwc::Registry *reg) {
      gcore::DynamicModule *lib = new gcore::DynamicModule(path);
      if (!lib->_opened()) {
        delete lib;
        return;
      }
      
      InitFunc minit = (InitFunc) lib->_getSymbol("LWC_ModuleInit");
      if (!minit) {
        delete lib;
        return;
      }
      GetNumTypesFunc getNumTypes = (GetNumTypesFunc) lib->_getSymbol("LWC_ModuleGetTypeCount");
      if (!getNumTypes) {
        delete lib;
        return;
      }
      GetTypeNameFunc getTypeName = (GetTypeNameFunc) lib->_getSymbol("LWC_ModuleGetTypeName");
      if (!getTypeName) {
        delete lib;
        return;
      }
      GetTypeFactoryFunc getTypeFactory = (GetTypeFactoryFunc) lib->_getSymbol("LWC_ModuleGetTypeFactory");
      if (!getTypeFactory) {
        delete lib;
        return;
      }
      ExitFunc mexit = (ExitFunc) lib->_getSymbol("LWC_ModuleExit");
      if (!mexit) {
        delete lib;
        return;
      }
      
      minit();
      
      size_t ntypes = getNumTypes();
      size_t loadedTypes = 0;
      
      for (size_t i=0; i<ntypes; ++i) {
        const char *tn = getTypeName(i);
        if (registerType(tn, getTypeFactory(i), reg)) {
          loadedTypes += 1;
        } else {
          std::cout << "cloader: Type \"" << tn << "\" already registered" << std::endl;
        }
      }
      
      if (loadedTypes > 0) {
        mModules.push_back(lib);
      }
    }
    
    virtual const char* getName() const {
      return "cloader";
    }
    
  private:
    
    std::vector<gcore::DynamicModule*> mModules;
};



// ---

extern "C" {

#ifdef _WIN32
__declspec(dllexport)
#else
__attribute__ ((visibility ("default")))
#endif
  lwc::Loader* LWC_CreateLoader(const char*, void*) {
    lwc::Loader *l = new CLoader();
    return l;
  }

#ifdef _WIN32
__declspec(dllexport)
#else
__attribute__ ((visibility ("default")))
#endif
  void LWC_DestroyLoader(lwc::Loader *l) {
    if (l) {
      delete l;
    }
  }

}



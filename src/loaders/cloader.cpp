/*

This file is part of lwc.

lwc is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

lwc is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with lwc.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <lwc/loader.h>
#include <lwc/dynlib.h>
#include <lwc/file.h>
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
        ExitFunc mexit = (ExitFunc) mModules[i]->getSymbol("LWC_ModuleExit");
        if (mexit) {
          mexit();
        }
        delete mModules[i];
      }
      mModules.clear();
    }
    
    virtual bool canLoad(const std::string &path) {
#ifdef _WIN32
      return lwc::file::CheckFileExtension(path, ".dll");
#else
# ifdef __APPLE__
      return lwc::file::CheckFileExtension(path, ".bundle");
# else
      return lwc::file::CheckFileExtension(path, ".so");
# endif
#endif
    }
    
    virtual void load(const std::string &path, lwc::Registry *reg) {
      lwc::DynLib *lib = new lwc::DynLib(path);
      if (!lib->opened()) {
        delete lib;
        return;
      }
      
      InitFunc minit = (InitFunc) lib->getSymbol("LWC_ModuleInit");
      if (!minit) {
        delete lib;
        return;
      }
      GetNumTypesFunc getNumTypes = (GetNumTypesFunc) lib->getSymbol("LWC_ModuleGetTypeCount");
      if (!getNumTypes) {
        delete lib;
        return;
      }
      GetTypeNameFunc getTypeName = (GetTypeNameFunc) lib->getSymbol("LWC_ModuleGetTypeName");
      if (!getTypeName) {
        delete lib;
        return;
      }
      GetTypeFactoryFunc getTypeFactory = (GetTypeFactoryFunc) lib->getSymbol("LWC_ModuleGetTypeFactory");
      if (!getTypeFactory) {
        delete lib;
        return;
      }
      ExitFunc mexit = (ExitFunc) lib->getSymbol("LWC_ModuleExit");
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
          std::cout << "cloader: Could not register type \"" << tn << "\"" << std::endl;
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
    
    std::vector<lwc::DynLib*> mModules;
};



// ---

extern "C" {

#ifdef _WIN32
__declspec(dllexport)
#endif
  lwc::Loader* LWC_CreateLoader(const char*, void*) {
    lwc::Loader *l = new CLoader();
    return l;
  }

#ifdef _WIN32
__declspec(dllexport)
#endif
  void LWC_DestroyLoader(lwc::Loader *l) {
    if (l) {
      delete l;
    }
  }

}



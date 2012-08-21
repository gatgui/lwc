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

#include <lwc/registry.h>
#include <lwc/memory.h>
#include <gcore/path.h>

namespace lwc {

Registry* Registry::msInstance = 0;

Registry* Registry::Initialize(const char *hostLang, void *userData) {
  if (!msInstance) {
    msInstance = new Registry(hostLang, userData);
  }
  return msInstance;
}

Registry* Registry::Initialize() {
  //if (!msInstance) {
  //  msInstance = new Registry();
  //}
  //return msInstance;
  return Initialize("C/C++", NULL);
}

Registry* Registry::Instance() {
  return msInstance;
}

void Registry::DeInitialize() {
  if (msInstance) {
    msInstance->destroySingletons();
    delete msInstance;
    msInstance = 0;
#ifdef LWC_MEMTRACK
    std::cout << "=== lwc library: Memory status" << std::endl;
    std::cout << Object::GetInstanceCount() << " remaining object(s)" << std::endl;
    lwc::memory::PrintAllocationInfo();
#endif
  }
}

/*
Don't do this, as we want to set host when lwc is initialized from a different language

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID)
{
   switch (fdwReason)
   {
   case DLL_PROCESS_ATTACH:
      Registry::Initialize();
      break;
   case DLL_PROCESS_DETACH:
      Registry::DeInitialize();
   default:
      break;
   }
   return TRUE;
}
#else
#ifdef __GNUC__
__attribute__((constructor)) int _lwcinit()
#else
int init()
#endif
{
   Initialize();
   return 0;
}
#ifdef __GNUC__
__attribute__((destructor)) void _lwcexit()
#else
void fini()
#endif
{
   Registry::DeInitialize();
}
#endif
*/

Registry::Registry(const char *hostLang, void *userData)
  : mHostLang(hostLang), mUserData(userData) {
  msInstance = this;
  gcore::Env::EachInPathFunc enumerator;
  gcore::Bind(this, METHOD(Registry, enumLoaderPath), enumerator);
  gcore::Env::EachInPath("LWC_LOADER_PATH", enumerator);
  gcore::Bind(this, METHOD(Registry, enumModulePath), enumerator);
  gcore::Env::EachInPath("LWC_MODULE_PATH", enumerator);
}

Registry::~Registry() {
  for (size_t i=0; i<mLoaders.size(); ++i) {
    LoaderEntry &le = mLoaders[i];
    LWC_DestroyLoader deinit = (LWC_DestroyLoader) le.lib->_getSymbol(LWC_DESTROYLOADER_STR);
    deinit(le.loader);
    delete le.lib;
  }
  mLoaders.clear();
}

bool Registry::enumLoaders(const gcore::Path &path) {
  if (path.isFile()) {
#ifdef _WIN32
    if (path.checkExtension("dll")) {
#else
# ifdef __APPLE__
    if (path.checkExtension("bundle")) {
# else
    if (path.checkExtension("so")) {
# endif
#endif
      addLoader(path);
    }
  }
  return true;
}

bool Registry::enumModules(const gcore::Path &path) {
  if (path.isFile()) {
    Loader *loader = findLoader(path.basename());
    if (loader) {
      loader->load(path, this);
    }
  }
  return true;
}

bool Registry::enumLoaderPath(const gcore::Path &path) {
  addLoaderPath(path);
  return true;
}

bool Registry::enumModulePath(const gcore::Path &path) {
  addModulePath(path);
  return true;
}

Loader* Registry::findLoader(const gcore::Path &path) {
  for (size_t i=0; i<mLoaders.size(); ++i) {
    LoaderEntry &le = mLoaders[i];
    if (le.loader && le.loader->canLoad(path)) {
      return le.loader;
    }
  }
  return 0;
}

void Registry::addLoader(const gcore::Path &path) {
  
  for (size_t i=0; i<mLoaders.size(); ++i) {
    if (mLoaders[i].path == path) {
      return;
    }
  }
  
  LoaderEntry le;
  
  le.path = path;
  
  le.lib = new gcore::DynamicModule(path);
  if (!le.lib->_opened()) {
    std::cout << "Could not load dynamic module: " << le.lib->_getError() << std::endl;
    delete le.lib;
    return;
  }
  
  void *sym_init   = le.lib->_getSymbol(LWC_CREATELOADER_STR);
  void *sym_deinit = le.lib->_getSymbol(LWC_DESTROYLOADER_STR);

  if (sym_init && sym_deinit) {
    
    LWC_CreateLoader init = (LWC_CreateLoader)sym_init;
    
    le.loader = init(mHostLang.c_str(), mUserData);
  
    if (le.loader) {
      mLoaders.push_back(le);
    }
  } else {
    std::cout << "LWC_CreateLoader and/or LWC_DestroyLoader entry point(s) not found" << std::endl;
  }
}

void Registry::addLoaderPath(const gcore::Path &path) {
  gcore::Path::EachFunc enumerator;
  gcore::Bind(this, METHOD(Registry, enumLoaders), enumerator);
  path.each(enumerator, false);
}

bool Registry::hasType(const char *name) const {
  return (mObjectLoaders.find(name) != mObjectLoaders.end());
}

bool Registry::isSingletonType(const char *name) const {
  return (mSingletons.find(name) != mSingletons.end());
}

bool Registry::registerType(const char *name, Loader *l, bool singleton) {
  if (hasType(name)) {
    return false;
  }
  mObjectLoaders[name] = l;
  if (singleton) {
    mSingletons[name] = 0;
  }
  return true;
}

void Registry::addModulePath(const gcore::Path &path) {
  gcore::Path::EachFunc enumerator;
  gcore::Bind(this, METHOD(Registry, enumModules), enumerator);
  path.each(enumerator, false);
}

size_t Registry::numTypes() const {
  return mObjectLoaders.size();
}

const char* Registry::getTypeName(size_t idx) const {
  if (idx > mObjectLoaders.size()) {
    return 0;
  } else {
    std::map<std::string, Loader*>::const_iterator it = mObjectLoaders.begin();
    for (size_t i=0; i<idx; ++i, ++it) {}
    return it->first.c_str();
  }
}

const MethodsTable* Registry::getMethods(const char *name) {
  if (hasType(name)) {
    return mObjectLoaders[name]->getMethods(name);
  } else {
    return 0;
  }
}

const char* Registry::getDescription(const char *name) {
  if (hasType(name)) {
    return mObjectLoaders[name]->getDescription(name);
  } else {
    return 0;
  }
}

std::string Registry::docString(const char *n, const std::string &indent) {
  if (hasType(n)) {
    return mObjectLoaders[n]->docString(n, indent);
  } else {
    return "";
  }
}

Object* Registry::create(const char *name) {
  if (hasType(name)) {
    std::map<std::string, Object*>::iterator it = mSingletons.find(name);
    if (it != mSingletons.end()) {
      if (it->second == 0) {
        it->second = mObjectLoaders[name]->create(name);
      }
      return it->second;
    } else {
      return mObjectLoaders[name]->create(name);
    }
  } else {
    return 0;
  }
}

Object* Registry::get(const char *name) {
  std::map<std::string, Object*>::iterator it = mSingletons.find(name);
  if (it != mSingletons.end()) {
    if (it->second == 0) {
      it->second = mObjectLoaders[name]->create(name);
    }
    return it->second;
  } else {
    return 0;
  }
}

void Registry::destroy(Object *o) {
  if (!o) {
    return;
  }
  const char *tn = o->mTypeName.c_str();
  if (!hasType(tn)) {
    return;
  }
  mObjectLoaders[tn]->destroy(o);
  std::map<std::string, Object*>::iterator it = mSingletons.find(tn);
  if (it != mSingletons.end()) {
    it->second = 0;
  }
}

void Registry::destroySingletons() {
  std::map<std::string, Object*>::iterator it = mSingletons.begin();
  while (it != mSingletons.end()) {
    destroy(it->second);
    ++it;
  }
}

}




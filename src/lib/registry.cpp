/*

Copyright (C) 2009  Gaetan Guidet

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
#include <lwc/dynlib.h>
#include <lwc/file.h>
#include <lwc/memory.h>

namespace lwc {

bool enumloaders(const std::string &d, const std::string &f, file::Type type, void *ud) {
  if (type == file::FT_FILE) {
#ifdef _WIN32
    if (file::CheckFileExtension(f, ".dll")) {
#else
# ifdef __APPLE__
    if (file::CheckFileExtension(f, ".bundle")) {
# else
    if (file::CheckFileExtension(f, ".so")) {
# endif
#endif
      Registry *reg = (Registry*)ud;
      
      reg->addLoader(file::JoinPath(d, f));
    }
  }
  return true;
}

bool enummodules(const std::string &d, const std::string &f, file::Type type, void *ud) {
  if (type == file::FT_FILE) {
    Registry *reg = (Registry*)ud;
    Loader *loader = reg->findLoader(f);
    if (loader) {
      loader->load(file::JoinPath(d, f), reg);
    }
  }
  return true;
}

// ---

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
    delete msInstance;
    msInstance = 0;
#ifdef LWC_MEMTRACK
    std::cout << "=== lwc library: Memory status" << std::endl;
    std::cout << Object::GetInstanceCount() << " remaining object(s)" << std::endl;
    lwc::memory::PrintAllocationInfo();
#endif
  }
}

Registry::Registry(const char *hostLang, void *userData)
  : mHostLang(hostLang), mUserData(userData) {
  msInstance = this;
}

Registry::~Registry() {
  for (size_t i=0; i<mLoaders.size(); ++i) {
    LoaderEntry &le = mLoaders[i];
    LWC_DestroyLoader deinit = (LWC_DestroyLoader) le.lib->getSymbol(LWC_DESTROYLOADER_STR);
    deinit(le.loader);
    delete le.lib;
  }
  mLoaders.clear();
}

Loader* Registry::findLoader(const std::string &path) {
  for (size_t i=0; i<mLoaders.size(); ++i) {
    LoaderEntry &le = mLoaders[i];
    if (le.loader && le.loader->canLoad(path)) {
      return le.loader;
    }
  }
  return 0;
}

void Registry::addLoader(const std::string &path) {
  LoaderEntry le;
  
  le.path = path;
  
  le.lib = new DynLib(path);
  if (!le.lib->opened()) {
    std::cout << "Could not load dynamic module: " << le.lib->getError() << std::endl;
    delete le.lib;
    return;
  }
  
  void *sym_init   = le.lib->getSymbol(LWC_CREATELOADER_STR);
  void *sym_deinit = le.lib->getSymbol(LWC_DESTROYLOADER_STR);

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

void Registry::addLoaderPath(const std::string &path) {
  ForEachInDir(path, &enumloaders, false, (void*)this);
}

bool Registry::hasType(const char *name) const {
  return (mObjectLoaders.find(name) != mObjectLoaders.end());
}

bool Registry::registerType(const char *name, Loader *l) {
  if (hasType(name)) {
    return false;
  }
  mObjectLoaders[name] = l;
  return true;
}

void Registry::addModulePath(const std::string &path) {
  ForEachInDir(path, &enummodules, false, (void*)this);
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

Object* Registry::create(const char *name) {
  if (hasType(name)) {
    return mObjectLoaders[name]->create(name);
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
}

}




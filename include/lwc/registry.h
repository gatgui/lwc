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

#ifndef __lwc_registry_h__
#define __lwc_registry_h__

#include <lwc/object.h>
#include <lwc/loader.h>
#include <gcore/dmodule.h>
#include <gcore/path.h>
#include <gcore/env.h>
#include <deque>

namespace lwc {
  
  typedef Loader* (*LWC_CreateLoader)(const char*,void*);
  typedef void (*LWC_DestroyLoader)(Loader*);
  
  #define LWC_CREATELOADER_STR  "LWC_CreateLoader"
  #define LWC_DESTROYLOADER_STR "LWC_DestroyLoader"
  
  class LWC_API Registry {
    public:
      
      static Registry* Initialize();
      static Registry* Initialize(const char *hostLang, void *userData);
      static Registry* Instance();
      static void DeInitialize();
      
      inline static Object* Create(const char *n) {
        return Instance()->create(n);
      }
      
      inline static Object* Get(const char *n) {
        return Instance()->get(n);
      }
      
      inline void Destroy(Object *o) {
        Instance()->destroy(o);
      }
      
    public:
      
      ~Registry();
      
      void addLoaderPath(const gcore::Path &path);
      void addLoader(const gcore::Path &path);
      Loader* findLoader(const gcore::Path &path);
      
      void addModulePath(const gcore::Path &path);
      
      bool registerType(const char *name, Loader *l, bool singleton=false);
      bool hasType(const char *name) const;
      bool isSingletonType(const char *name) const;
      size_t numTypes() const;
      const char* getTypeName(size_t idx) const;
      
      const MethodsTable* getMethods(const char*n);
      const char* getDescription(const char *n);
      std::string docString(const char *n, const std::string &indent="");
      Object* create(const char *n);
      Object* get(const char *n);
      void destroy(Object *o);
      void destroySingletons();
      
      bool enumLoaders(const gcore::Path &p);
      bool enumModules(const gcore::Path &p);
      bool enumLoaderPath(const gcore::Path &p);
      bool enumModulePath(const gcore::Path &p);
      
    protected:
      
      Registry(const char *hostLang, void *userData);
    
    protected:
      
      static Registry *msInstance;
    
      struct LoaderEntry {
        gcore::Path path;
        Loader *loader;
        gcore::DynamicModule *lib;
      };
      
      std::deque<LoaderEntry> mLoaders;
      
      std::map<std::string, Loader*> mObjectLoaders;
      
      std::map<std::string, Object*> mSingletons;
      
      std::string mHostLang;
      void *mUserData;
  };
  
}

#endif



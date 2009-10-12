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

#ifndef __lwc_registry_h__
#define __lwc_registry_h__

#include <lwc/object.h>
#include <lwc/loader.h>
#include <lwc/dynlib.h>
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
      
    public:
      
      ~Registry();
      
      void addLoaderPath(const std::string &path);
      void addLoader(const std::string &path);
      Loader* findLoader(const std::string &path);
      
      void addModulePath(const std::string &path);
      
      bool registerType(const char *name, Loader *l);
      bool hasType(const char *name) const;
      size_t numTypes() const;
      const char* getTypeName(size_t idx) const;
      
      const MethodsTable* getMethods(const char*);
      Object* create(const char *n);
      void destroy(Object *o);
      
    protected:
      
      Registry(const char *hostLang, void *userData);
    
    protected:
      
      static Registry *msInstance;
    
      struct LoaderEntry {
        std::string path;
        Loader *loader;
        DynLib *lib;
      };
      
      std::deque<LoaderEntry> mLoaders;
      
      std::map<std::string, Loader*> mObjectLoaders;
      
      std::string mHostLang;
      void *mUserData;
  };
  
}

#endif



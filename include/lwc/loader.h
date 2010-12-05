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

#ifndef __lwc_loader_h__
#define __lwc_loader_h__

#include <lwc/factory.h>
#include <gcore/path.h>

namespace lwc {
  
  // must define entry points for loaders -> create and delete entry point only
  
  class LWC_API Registry;
  
  class LWC_API Loader {
    public:
      
      Loader();
      virtual ~Loader();
      
      virtual bool canLoad(const gcore::Path &path) = 0;
      virtual void load(const gcore::Path &path, class Registry *reg) = 0;
      virtual const char* getName() const = 0;
      
      inline size_t numTypes() const {return mFactories.size();}
      inline bool hasType(const char *tn) const {return (mFactories.find(tn) != mFactories.end());}
      const char* getTypeName(size_t idx) const throw(std::runtime_error);
      bool registerType(const char *name, Factory *f, Registry *reg);
      
      const MethodsTable* getMethods(const char *name);
      Object* create(const char *name);
      void destroy(Object *o);
      
    protected:
      
      std::map<std::string, Factory*> mFactories;
  };
  
}

#endif

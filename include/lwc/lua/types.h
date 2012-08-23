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

#ifndef __lwc_lua_types_h__
#define __lwc_lua_types_h__

#include <lwc/lua/config.h>

namespace lua {
  
  inline void CheckArgCount(lua_State *L, int n) {
    if (lua_gettop(L) != n) {
      lua_pushfstring(L, "%d arguments expected (including self when it applies)", n);
      lua_error(L);
    }  
  }
  
  class LWCLUA_API LuaArgument {
    public:
      
      lwc::Argument arg;
      
      LuaArgument();
      LuaArgument(const lwc::Argument &a);
      ~LuaArgument();

      static size_t AllocSize();
      static const char* RegistryKey();
      static int New(lua_State *L);
      static int Wrap(lua_State *L, const lwc::Argument &a);
      static lwc::Argument& UnWrap(lua_State *L, int idx, bool upValue=false);
      static int Del(lua_State *L);
  };
  
  class LWCLUA_API LuaMethod {
    public:
      
      lwc::Method meth;
      
      LuaMethod();
      LuaMethod(const lwc::Method &m);
      ~LuaMethod();

      static size_t AllocSize();
      static const char* RegistryKey();
      static int New(lua_State *L);
      static int Wrap(lua_State *L, const lwc::Method &a);
      static lwc::Method& UnWrap(lua_State *L, int idx, bool upValue=false);
      static int Del(lua_State *L);
  };
  
  class LWCLUA_API LuaMethodsTable {
    public:
      
      const lwc::MethodsTable *table;
      
      LuaMethodsTable();
      LuaMethodsTable(const lwc::MethodsTable *tbl);
      ~LuaMethodsTable();

      static size_t AllocSize();
      static const char* RegistryKey();
      static int New(lua_State *L);
      static int Wrap(lua_State *L, const lwc::MethodsTable *t);
      static const lwc::MethodsTable* UnWrap(lua_State *L, int idx, bool upValue=false);
      static int Del(lua_State *L);
  };
  
  class LWCLUA_API LuaObject {
    public:
    
      lwc::Object *obj;
    
      LuaObject();
      LuaObject(lwc::Object *o);
      ~LuaObject();

      static size_t AllocSize();
      static const char* RegistryKey();
      static int New(lua_State *L);
      static int Wrap(lua_State *L, lwc::Object *o);
      static lwc::Object* UnWrap(lua_State *L, int idx, bool upValue=false);
      static int Del(lua_State *L);
  };
  
  class LWCLUA_API LuaRegistry {
    public:
    
      lwc::Registry *reg;
    
      LuaRegistry();
      LuaRegistry(lwc::Registry *r);
      ~LuaRegistry();

      static size_t AllocSize();
      static const char* RegistryKey();
      static int New(lua_State *L);
      static int Wrap(lua_State *L, lwc::Registry *r);
      static lwc::Registry* UnWrap(lua_State *L, int idx, bool upValue=false);
      static int Del(lua_State *L);
  };
  
  
  LWCLUA_API bool InitArgument(lua_State *L, int module);
  LWCLUA_API bool InitMethod(lua_State *L, int module);
  LWCLUA_API bool InitMethodsTable(lua_State *L, int module);
  LWCLUA_API bool InitObject(lua_State *L, int module);
  LWCLUA_API bool InitRegistry(lua_State *L, int module);
  LWCLUA_API int CreateModule(lua_State *L);

  LWCLUA_API void CleanupModule();
  
  LWCLUA_API bool SetArgDefault(lwc::Argument &a, lua_State *L, int idx);
}

#endif


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

#include <lwc/lua/types.h>

namespace lua {

LuaRegistry::LuaRegistry()
  : reg(0) {
}

LuaRegistry::LuaRegistry(lwc::Registry *r)
  : reg(r) {
}

LuaRegistry::~LuaRegistry() {
}

size_t LuaRegistry::AllocSize() {
  return sizeof(LuaRegistry);
}

const char* LuaRegistry::RegistryKey() {
  return "__lwc_lua_registry";
}

int LuaRegistry::New(lua_State *L) {
  CheckArgCount(L, 0);
  void *ud = lua_newuserdata(L, LuaRegistry::AllocSize());
  new (ud) LuaRegistry();
  lua_getfield(L, LUA_REGISTRYINDEX, LuaRegistry::RegistryKey());
  lua_setmetatable(L, -2);
  return 1;
}

int LuaRegistry::Wrap(lua_State *L, lwc::Registry *r) {
  void *ud = lua_newuserdata(L, LuaRegistry::AllocSize());
  new (ud) LuaRegistry(r);
  lua_getfield(L, LUA_REGISTRYINDEX, LuaRegistry::RegistryKey());
  lua_setmetatable(L, -2);
  return 1;
}

lwc::Registry* LuaRegistry::UnWrap(lua_State *L, int narg, bool upValue) {
  int idx = (upValue ? lua_upvalueindex(narg) : narg);
  // could be a subclass?
  void *p = lua_touserdata(L, idx);
  if (!p) {
    luaL_typerror(L, narg, "llwc.Registry");
  }
  lua_getfield(L, LUA_REGISTRYINDEX, LuaRegistry::RegistryKey());
  if (!lua_getmetatable(L, idx) || !lua_rawequal(L, -1, -2)) {
    luaL_typerror(L, narg, "llwc.Registry");
  }
  lua_pop(L, 2);
  return ((LuaRegistry*) p)->reg;
}

int LuaRegistry::Del(lua_State *L) {
  CheckArgCount(L, 1);
  void *ud = lua_touserdata(L, 1);
  ((LuaRegistry*)ud)->~LuaRegistry();
  lua_pop(L, 1);
  return 0;
}

// ---

static int luareg_new(lua_State *L) {
  return LuaRegistry::New(L);
}

static int luareg_del(lua_State *L) {
  return LuaRegistry::Del(L);
}

static int luareg_create(lua_State *L) {
  CheckArgCount(L, 2);
  lwc::Registry *reg = LuaRegistry::UnWrap(L, 1);
  if (!reg) {
    reg = lwc::Registry::Instance();
    if (!reg) {
      lua_pushstring(L, "llwc.Registry has not been initialized");
      return lua_error(L);
    }
  }
  if (!lua_isstring(L, 2)) {
    return luaL_typerror(L, 2, "string");
  }
  const char *t = lua_tostring(L, 2);
  lua_pop(L, 2);
  lwc::Object *o = reg->create(t);
  if (!o) {
    lua_pushnil(L);
    return 1;
    
  } else {
    if (!strcmp(o->getLoaderName(), "lualoader")) {
      // lua object has already been created and put in registry so
      // it doesn't get garbage collected if it goes out of scope in lua
      lua_pushlightuserdata(L, (void*)o);
      lua_gettable(L, LUA_REGISTRYINDEX);
      return 1;
      
    } else {
      return LuaObject::Wrap(L, o);
    }
  }
}

static int luareg_get(lua_State *L) {
  CheckArgCount(L, 2);
  lwc::Registry *reg = LuaRegistry::UnWrap(L, 1);
  if (!reg) {
    reg = lwc::Registry::Instance();
    if (!reg) {
      lua_pushstring(L, "llwc.Registry has not been initialized");
      return lua_error(L);
    }
  }
  if (!lua_isstring(L, 2)) {
    return luaL_typerror(L, 2, "string");
  }
  const char *t = lua_tostring(L, 2);
  lua_pop(L, 2);
  lwc::Object *o = reg->get(t);
  if (!o) {
    lua_pushnil(L);
    return 1;
    
  } else {
    if (!strcmp(o->getLoaderName(), "lualoader")) {
      // lua object has already been created and put in registry so
      // it doesn't get garbage collected if it goes out of scope in lua
      lua_pushlightuserdata(L, (void*)o);
      lua_gettable(L, LUA_REGISTRYINDEX);
      return 1;
      
    } else {
      return LuaObject::Wrap(L, o);
    }
  }
}

static int luareg_destroy(lua_State *L) {
  CheckArgCount(L, 2);
  lwc::Registry *reg = LuaRegistry::UnWrap(L, 1);
  if (!reg) {
    reg = lwc::Registry::Instance();
    if (!reg) {
      lua_pushstring(L, "llwc.Registry has not been initialized");
      return lua_error(L);
    }
  }
  /*
  lwc::Object *o = 0;
  if (lua_istable(L, 2)) {
    lua_getfield(L, 2, "lwcobj");
    if (lua_isnil(L, -1)) {
      lua_pushstring(L, "table object has no \"lwcobj\" member");
      return lua_error(L);
    }
    o = LuaObject::UnWrap(L, lua_gettop(L));
    lua_pop(L, 1);
    
  } else {
    o = LuaObject::UnWrap(L, 2);
  }
  */
  lwc::Object *o = LuaObject::UnWrap(L, 2);
  lua_pop(L, 2);
  reg->destroy(o);
  return 0;
}

static int luareg_getTypeName(lua_State *L) {
  CheckArgCount(L, 2);
  lwc::Registry *reg = LuaRegistry::UnWrap(L, 1);
  if (!reg) {
    reg = lwc::Registry::Instance();
    if (!reg) {
      lua_pushstring(L, "llwc.Registry has not been initialized");
      return lua_error(L);
    }
  }
  if (!lua_isnumber(L, 2)) {
    return luaL_typerror(L, 2, "integer");
  }
  lua_Integer idx = lua_tointeger(L, 2);
  lua_pop(L, 2);
  lua_pushstring(L, reg->getTypeName(idx-1));
  return 1;
}

static int luareg_getMethods(lua_State *L) {
  CheckArgCount(L, 2);
  lwc::Registry *reg = LuaRegistry::UnWrap(L, 1);
  if (!reg) {
    reg = lwc::Registry::Instance();
    if (!reg) {
      lua_pushstring(L, "llwc.Registry has not been initialized");
      return lua_error(L);
    }
  }
  if (!lua_isstring(L, 2)) {
    return luaL_typerror(L, 2, "string");
  }
  const char *tn = lua_tostring(L, 2);
  lua_pop(L, 2);
  const lwc::MethodsTable *mt = reg->getMethods(tn);
  if (!mt) {
    lua_pushnil(L);
    return 1;
    
  } else {
    return LuaMethodsTable::Wrap(L, mt);
  }
}

static int luareg_hasType(lua_State *L) {
  CheckArgCount(L, 2);
  lwc::Registry *reg = LuaRegistry::UnWrap(L, 1);
  if (!reg) {
    reg = lwc::Registry::Instance();
    if (!reg) {
      lua_pushstring(L, "llwc.Registry has not been initialized");
      return lua_error(L);
    }
  }
  if (!lua_isstring(L, 2)) {
    return luaL_typerror(L, 2, "string");
  }
  const char *t = lua_tostring(L, 2);
  lua_pop(L, 2);
  lua_pushboolean(L, reg->hasType(t));
  return 1;
}

static int luareg_numTypes(lua_State *L) {
  CheckArgCount(L, 1);
  lwc::Registry *reg = LuaRegistry::UnWrap(L, 1);
  if (!reg) {
    reg = lwc::Registry::Instance();
    if (!reg) {
      lua_pushstring(L, "llwc.Registry has not been initialized");
      return lua_error(L);
    }
  }
  lua_pop(L, 1);
  lua_pushinteger(L, reg->numTypes());
  return 1;
}

static int luareg_addLoaderPath(lua_State *L) {
  CheckArgCount(L, 2);
  lwc::Registry *reg = LuaRegistry::UnWrap(L, 1);
  if (!reg) {
    reg = lwc::Registry::Instance();
    if (!reg) {
      lua_pushstring(L, "llwc.Registry has not been initialized");
      return lua_error(L);
    }
  }
  if (!lua_isstring(L, 2)) {
    return luaL_typerror(L, 2, "string");
  }
  const char *path = lua_tostring(L, 2);
  lua_pop(L, 2);
  reg->addLoaderPath(path);
  return 0;
}

static int luareg_addModulePath(lua_State *L) {
  CheckArgCount(L, 2);
  lwc::Registry *reg = LuaRegistry::UnWrap(L, 1);
  if (!reg) {
    reg = lwc::Registry::Instance();
    if (!reg) {
      lua_pushstring(L, "llwc.Registry has not been initialized");
      return lua_error(L);
    }
  }
  if (!lua_isstring(L, 2)) {
    return luaL_typerror(L, 2, "string");
  }
  const char *path = lua_tostring(L, 2);
  lua_pop(L, 2);
  reg->addModulePath(path);
  return 0;
}

// ---

bool InitRegistry(lua_State *L, int module) {
  lua_newtable(L);
  int klass = lua_gettop(L);
  
  // make the user data callable by forwarding all indexing to its class
  lua_pushvalue(L, klass);
  lua_setfield(L, klass, "__index");
  
  lua_pushcfunction(L, luareg_new);
  lua_setfield(L, klass, "new");
  lua_pushcfunction(L, luareg_del);
  lua_setfield(L, klass, "__gc");
  lua_pushcfunction(L, luareg_addLoaderPath);
  lua_setfield(L, klass, "addLoaderPath");
  lua_pushcfunction(L, luareg_addModulePath);
  lua_setfield(L, klass, "addModulePath");
  lua_pushcfunction(L, luareg_hasType);
  lua_setfield(L, klass, "hasType");
  lua_pushcfunction(L, luareg_getMethods);
  lua_setfield(L, klass, "getMethods");
  lua_pushcfunction(L, luareg_getTypeName);
  lua_setfield(L, klass, "getTypeName");
  lua_pushcfunction(L, luareg_numTypes);
  lua_setfield(L, klass, "numTypes");
  lua_pushcfunction(L, luareg_create);
  lua_setfield(L, klass, "create");
  lua_pushcfunction(L, luareg_get);
  lua_setfield(L, klass, "get");
  lua_pushcfunction(L, luareg_destroy);
  lua_setfield(L, klass, "destroy");
  
  lua_pushvalue(L, klass);
  lua_setfield(L, LUA_REGISTRYINDEX, LuaRegistry::RegistryKey());
  lua_setfield(L, module, "Registry");
  
  return true;
}

}


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

#include <lwc/lua/types.h>
#include <lwc/lua/methodcall.h>

namespace lua {

LuaObject::LuaObject()
  : obj(0) {
}

LuaObject::LuaObject(lwc::Object *o)
  : obj(o) {
}

LuaObject::~LuaObject() {
}

size_t LuaObject::AllocSize() {
  return sizeof(LuaObject);
}

const char* LuaObject::RegistryKey() {
  return "__lcw_lua_object";
}

int LuaObject::New(lua_State *L) {
  CheckArgCount(L, 0);
  void *ud = lua_newuserdata(L, LuaObject::AllocSize());
  new (ud) LuaObject();
  lua_getfield(L, LUA_REGISTRYINDEX, LuaObject::RegistryKey());
  lua_setmetatable(L, -2);
  return 1;
}

int LuaObject::Wrap(lua_State *L, lwc::Object *o) {
  void *ud = lua_newuserdata(L, LuaObject::AllocSize());
  new (ud) LuaObject(o);
  lua_getfield(L, LUA_REGISTRYINDEX, LuaObject::RegistryKey());
  lua_setmetatable(L, -2);
  return 1;
}

lwc::Object* LuaObject::UnWrap(lua_State *L, int narg, bool upValue) {
  int idx = (upValue ? lua_upvalueindex(narg) : narg);
  int top = lua_gettop(L);
  if (lua_istable(L, idx)) {
    lua_getfield(L, idx, "lwcobj");
    if (lua_isnil(L, -1)) {
      luaL_typerror(L, -1, "llwc.Object");
    }
    idx = lua_gettop(L);
  }
  void *p = lua_touserdata(L, idx);
  if (!p) {
    luaL_typerror(L, narg, "llwc.Object");
  }
  // this check for exact type
  lua_getfield(L, LUA_REGISTRYINDEX, LuaObject::RegistryKey());
  if (!lua_getmetatable(L, idx) || !lua_rawequal(L, -1, -2)) {
    luaL_typerror(L, narg, "llwc.Object");
  }
  lua_settop(L, top);
  return ((LuaObject*) p)->obj;
}

int LuaObject::Del(lua_State *L) {
  CheckArgCount(L, 1);
  void *ud = lua_touserdata(L, 1);
  ((LuaObject*)ud)->~LuaObject();
  lua_pop(L, 1);
  return 0;
}

// ---

static int luaobj_new(lua_State *L) {
  return LuaObject::New(L);
}

static int luaobj_del(lua_State *L) {
  return LuaObject::Del(L);
}

static int luaobj_getMethod(lua_State *L) {
  CheckArgCount(L, 2);
  lwc::Object *o = LuaObject::UnWrap(L, 1);
  if (!o) {
    lua_pushstring(L, "llwc.Object: not underlying object");
    return lua_error(L);
  }
  if (!lua_isstring(L, 2)) {
    return luaL_typerror(L, 2, "string");
  }
  const char *mn = lua_tostring(L, 2);
  lua_pop(L, 2);
  try {
    const lwc::Method &m = o->getMethod(mn);
    return LuaMethod::Wrap(L, m);
    
  } catch (std::exception &e) {
    lua_pushstring(L, e.what());
    return lua_error(L);
  }
}

static int luaobj_getMethods(lua_State *L) {
  CheckArgCount(L, 1);
  lwc::Object *o = LuaObject::UnWrap(L, 1);
  if (!o) {
    lua_pushstring(L, "llwc.Object: not underlying object");
    return lua_error(L);
  }
  lua_pop(L, 1);
  const lwc::MethodsTable *tbl = o->getMethods();
  if (!tbl) {
    lua_pushnil(L);
    return 1;
    
  } else {
    return LuaMethodsTable::Wrap(L, tbl);
  }
}

static int luaobj_respondsTo(lua_State *L) {
  CheckArgCount(L, 2);
  lwc::Object *o = LuaObject::UnWrap(L, 1);
  if (!o) {
    lua_pushstring(L, "llwc.Object: not underlying object");
    return lua_error(L);
  }
  if (!lua_isstring(L, 2)) {
    return luaL_typerror(L, 2, "string");
  }
  const char *mn = lua_tostring(L, 2);
  lua_pop(L, 2);
  lua_pushboolean(L, o->respondsTo(mn));
  return 1;
}

static int luaobj_getTypeName(lua_State *L) {
  CheckArgCount(L, 1);
  lwc::Object *o = LuaObject::UnWrap(L, 1);
  if (!o) {
    lua_pushstring(L, "llwc.Object: not underlying object");
    return lua_error(L);
  }
  lua_pop(L, 1);
  const char *s = o->getTypeName();
  if (!s) {
    lua_pushstring(L, "");
    return 1;
    
  } else {
    lua_pushstring(L, s);
    return 1;
  }
}

static int luaobj_getLoaderName(lua_State *L) {
  CheckArgCount(L, 1);
  lwc::Object *o = LuaObject::UnWrap(L, 1);
  if (!o) {
    lua_pushstring(L, "llwc.Object: not underlying object");
    return lua_error(L);
  }
  lua_pop(L, 1);
  const char *s = o->getLoaderName();
  if (!s) {
    lua_pushstring(L, "");
    return 1;
    
  } else {
    lua_pushstring(L, s);
    return 1;
  }
}

static int luaobj_availableMethods(lua_State *L) {
  CheckArgCount(L, 1);
  lwc::Object *o = LuaObject::UnWrap(L, 1);
  if (!o) {
    lua_pushstring(L, "llwc.Object: not underlying object");
    return lua_error(L);
  }
  lua_pop(L, 1);
  std::vector<std::string> names;
  o->availableMethods(names);
  lua_newtable(L);
  for (size_t i=0; i<names.size(); ++i) {
    lua_pushinteger(L, i+1);
    lua_pushstring(L, names[i].c_str());
    lua_settable(L, -3);
  }
  return 1;
}

static int luaobj_call(lua_State *L) {
  int idx = lua_upvalueindex(1);
  const char *mn = lua_tostring(L, idx);
  
  try {
    std::map<size_t, size_t> arraySizes;
    
    lwc::Object *o = LuaObject::UnWrap(L, 1);
    lwc::MethodParams params(o->getMethod(mn));
    size_t nargs = lua_gettop(L) - 1;
  
    int rv = CallMethod(o, mn, params, 0, L, 2, nargs, 0, arraySizes);
    
    if (rv == NO_RETVAL) {
      return 0;
      
    } else {
      // lua does not auto expand array as return value
      //return 1;
      return rv;
    }
  
  } catch (std::exception &e) {
  
    lua_pushstring(L, e.what());
    return lua_error(L);
  }
}

static int luaobj_index(lua_State *L) {
  CheckArgCount(L, 2);
  
  lwc::Object *o = LuaObject::UnWrap(L, 1);
  if (!o) {
    lua_pushstring(L, "llwc.Object: no underlying object");
    return lua_error(L);
  }
  
  const char *mn = lua_tostring(L, 2);
  
  //lua_getmetatable(L, 1);
  lua_getfield(L, LUA_REGISTRYINDEX, LuaObject::RegistryKey());
  lua_pushvalue(L, 2);
  lua_rawget(L, -2);
  
  if (lua_isnil(L, -1)) {
    lua_pushstring(L, mn);
    lua_pushcclosure(L, luaobj_call, 1);
    return 1;
    
  } else {
    return 1;
  }
}

// ---

bool InitObject(lua_State *L, int module) {
  
  lua_newtable(L);
  int klass = lua_gettop(L);
  
  lua_pushcfunction(L, luaobj_index);
  lua_setfield(L, klass, "__index");
  lua_pushcfunction(L, luaobj_new);
  lua_setfield(L, klass, "new");
  lua_pushcfunction(L, luaobj_del);
  lua_setfield(L, klass, "__gc");
  lua_pushcfunction(L, luaobj_getMethod);
  lua_setfield(L, klass, "getMethod");
  lua_pushcfunction(L, luaobj_getMethods);
  lua_setfield(L, klass, "getMethods");
  lua_pushcfunction(L, luaobj_respondsTo);
  lua_setfield(L, klass, "respondsTo");
  lua_pushcfunction(L, luaobj_getLoaderName);
  lua_setfield(L, klass, "getLoaderName");
  lua_pushcfunction(L, luaobj_getTypeName);
  lua_setfield(L, klass, "getTypeName");
  lua_pushcfunction(L, luaobj_availableMethods);
  lua_setfield(L, klass, "availableMethods");
  
  lua_pushvalue(L, klass);
  lua_setfield(L, LUA_REGISTRYINDEX, LuaObject::RegistryKey());
  lua_setfield(L, module, "Object");
  
  return true;
}

}


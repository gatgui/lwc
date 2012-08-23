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

LuaMethodsTable::LuaMethodsTable()
  : table(0) {
}

LuaMethodsTable::LuaMethodsTable(const lwc::MethodsTable *tbl)
  : table(tbl) {
}

LuaMethodsTable::~LuaMethodsTable() {
}

size_t LuaMethodsTable::AllocSize() {
  return sizeof(LuaMethodsTable);
}

const char* LuaMethodsTable::RegistryKey() {
  return "__lua_lwc_methodstable";
}

int LuaMethodsTable::New(lua_State *L) {
  CheckArgCount(L, 0);
  void *ud = lua_newuserdata(L, LuaMethodsTable::AllocSize());
  new (ud) LuaMethodsTable();
  lua_getfield(L, LUA_REGISTRYINDEX, LuaMethodsTable::RegistryKey());
  lua_setmetatable(L, -2);
  return 1;
}

int LuaMethodsTable::Wrap(lua_State *L, const lwc::MethodsTable *t) {
  void *ud = lua_newuserdata(L, LuaMethodsTable::AllocSize());
  new (ud) LuaMethodsTable(t);
  lua_getfield(L, LUA_REGISTRYINDEX, LuaMethodsTable::RegistryKey());
  lua_setmetatable(L, -2);
  return 1;
}

const lwc::MethodsTable* LuaMethodsTable::UnWrap(lua_State *L, int narg, bool upValue) {
  int idx = (upValue ? lua_upvalueindex(narg) : narg);
  // could be a subclass?
  void *p = lua_touserdata(L, idx);
  if (!p) {
    luaL_typerror(L, narg, "llwc.MethodsTable");
  }
  lua_getfield(L, LUA_REGISTRYINDEX, LuaMethodsTable::RegistryKey());
  if (!lua_getmetatable(L, idx) || !lua_rawequal(L, -1, -2)) {
    luaL_typerror(L, narg, "llwc.MethodsTable");
  }
  lua_pop(L, 2);
  return ((LuaMethodsTable*) p)->table;
}

int LuaMethodsTable::Del(lua_State *L) {
  CheckArgCount(L, 1);
  void *ud = lua_touserdata(L, 1);
  ((LuaMethodsTable*)ud)->~LuaMethodsTable();
  lua_pop(L, 1);
  return 0;
}

// ---

static int luamtbl_new(lua_State *L) {
  return LuaMethodsTable::New(L);
}

static int luamtbl_del(lua_State *L) {
  return LuaMethodsTable::Del(L);
}

static int luamtbl_availableMethods(lua_State *L) {
  CheckArgCount(L, 1);
  const lwc::MethodsTable *tbl = LuaMethodsTable::UnWrap(L, 1);
  if (!tbl) {
    lua_pushstring(L, "Underlying lwc::MethodsTable object does not exist");
    return lua_error(L);
  }
  lua_pop(L, 1);
  std::vector<std::string> names;
  tbl->availableMethods(names);
  lua_newtable(L);
  for (size_t i=0; i<names.size(); ++i) {
    lua_pushinteger(L, i+1);
    lua_pushstring(L, names[i].c_str());
    lua_settable(L, -3);
  }
  return 1;
}

static int luamtbl_findMethod(lua_State *L) {
  CheckArgCount(L, 2);
  const lwc::MethodsTable *tbl = LuaMethodsTable::UnWrap(L, 1);
  if (!tbl) {
    lua_pushstring(L, "Underlying lwc::MethodsTable object does not exist");
    return lua_error(L);
  }
  if (!lua_isstring(L, 2)) {
    return luaL_typerror(L, 2, "string");
  }
  const char *mn = lua_tostring(L, 2);
  lua_pop(L, 2);
  const lwc::Method *mth = tbl->findMethod(mn);
  if (!mth) {
    lua_pushnil(L);
    return 1;
  } else {
    return LuaMethod::Wrap(L, *mth);
  }
}

static int luamtbl_numMethods(lua_State *L) {
  CheckArgCount(L, 1);
  const lwc::MethodsTable *tbl = LuaMethodsTable::UnWrap(L, 1);
  if (!tbl) {
    lua_pushstring(L, "Underlying lwc::MethodsTable object does not exist");
    return lua_error(L);
  }
  lua_pop(L, 1);
  lua_pushinteger(L, tbl->numMethods());
  return 1;
}

static int luamtbl_tos(lua_State *L) {
  CheckArgCount(L, 1);
  const lwc::MethodsTable *tbl = LuaMethodsTable::UnWrap(L, 1);
  if (!tbl) {
    lua_pushstring(L, "Underlying lwc::MethodsTable object does not exist");
    return lua_error(L);
  }
  lua_pop(L, 1);
  lua_pushstring(L, tbl->toString().c_str());
  return 1;
}

static int luamtbl_docString(lua_State *L) {
  int nargs = lua_gettop(L);
  if (nargs < 1 || nargs > 2) {
    lua_pushstring(L, "1 or 2 arguments expected (including self)");
    lua_error(L);
    // lua_error doesn't return (long jump) just as rb_raise
    return 0;
  }
  const lwc::MethodsTable *tbl = LuaMethodsTable::UnWrap(L, 1);
  if (!tbl) {
    lua_pushstring(L, "Underlying lwc::MethodsTable object does not exist");
    return lua_error(L);
  }
  std::string indent = "";
  if (nargs == 2) {
    if (!lua_isstring(L, 2)) {
      return luaL_typerror(L, 2, "string");
    }
    indent = lua_tostring(L, 2);
  }
  lua_pop(L, nargs);
  std::string s = tbl->docString(indent);
  lua_pushstring(L, s.c_str());
  return 1;
}

// ---

bool InitMethodsTable(lua_State *L, int module) {
  
  lua_newtable(L);
  int klass = lua_gettop(L);
  
  // make the user data callable by forwarding all indexing to its class
  lua_pushvalue(L, klass);
  lua_setfield(L, klass, "__index");
  
  lua_pushcfunction(L, luamtbl_new);
  lua_setfield(L, klass, "new");
  lua_pushcfunction(L, luamtbl_del);
  lua_setfield(L, klass, "__gc");
  lua_pushcfunction(L, luamtbl_tos);
  lua_setfield(L, klass, "__tostring");
  lua_pushcfunction(L, luamtbl_docString);
  lua_setfield(L, klass, "docString");
  lua_pushcfunction(L, luamtbl_numMethods);
  lua_setfield(L, klass, "numMethods");
  lua_pushcfunction(L, luamtbl_findMethod);
  lua_setfield(L, klass, "findMethod");
  lua_pushcfunction(L, luamtbl_availableMethods);
  lua_setfield(L, klass, "availableMethods");
  
  lua_pushvalue(L, klass);
  lua_setfield(L, LUA_REGISTRYINDEX, LuaMethodsTable::RegistryKey());
  lua_setfield(L, module, "MethodsTable");
  
  return true;
}

}



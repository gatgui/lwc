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

namespace lua {


LuaArgument::LuaArgument() {
}

LuaArgument::LuaArgument(const lwc::Argument &a)
  : arg(a) {
}


LuaArgument::~LuaArgument() {
}

size_t LuaArgument::AllocSize() {
  return (sizeof(LuaArgument));
}

const char* LuaArgument::RegistryKey() {
  return "__lwc_lua_argument";
}

int LuaArgument::New(lua_State *L) {
  CheckArgCount(L, 0);
  void *ud = lua_newuserdata(L, LuaArgument::AllocSize());
  new (ud) LuaArgument();
  lua_getfield(L, LUA_REGISTRYINDEX, LuaArgument::RegistryKey());
  lua_setmetatable(L, -2);
  return 1;
}

int LuaArgument::Wrap(lua_State *L, const lwc::Argument &a) {
  void *ud = lua_newuserdata(L, sizeof(LuaArgument));
  new (ud) LuaArgument(a);
  lua_getfield(L, LUA_REGISTRYINDEX, LuaArgument::RegistryKey());
  lua_setmetatable(L, -2);
  return 1;
}

lwc::Argument& LuaArgument::UnWrap(lua_State *L, int narg, bool upValue) {
  int idx = (upValue ? lua_upvalueindex(narg) : narg);
  // could be a subclass?
  void *p = lua_touserdata(L, idx);
  if (!p) {
    luaL_typerror(L, narg, "llwc.Argument");
  }
  lua_getfield(L, LUA_REGISTRYINDEX, LuaArgument::RegistryKey());
  if (!lua_getmetatable(L, idx) || !lua_rawequal(L, -1, -2)) {
    luaL_typerror(L, narg, "llwc.Argument");
  }
  lua_pop(L, 2);
  return ((LuaArgument*) p)->arg;
}

int LuaArgument::Del(lua_State *L) {
  CheckArgCount(L, 1);
  void *ud = lua_touserdata(L, 1);
  ((LuaArgument*)ud)->~LuaArgument();
  lua_pop(L, 1);
  return 0;
}

// ---

static int luaarg_new(lua_State *L) {
  return LuaArgument::New(L);
}

static int luaarg_del(lua_State *L) {
  return LuaArgument::Del(L);
}

static int luaarg_tos(lua_State *L) {
  CheckArgCount(L, 1);
  lwc::Argument &a = LuaArgument::UnWrap(L, 1);
  lua_pop(L, 1);
  std::string s = a.toString();
  lua_pushstring(L, s.c_str());
  return 1;
}

static int luaarg_isArray(lua_State *L) {
  CheckArgCount(L, 1);
  lwc::Argument &a = LuaArgument::UnWrap(L, 1);
  lua_pop(L, 1);
  lua_pushboolean(L, a.isArray());
  return 1;
}

static int luaarg_arrayArg(lua_State *L) {
  CheckArgCount(L, 1);
  lwc::Argument &a = LuaArgument::UnWrap(L, 1);
  lua_pushinteger(L, lua_Integer(a.arrayArg()));
  return 1;
}

static int luaarg_arraySizeArg(lua_State *L) {
  CheckArgCount(L, 1);
  lwc::Argument &a = LuaArgument::UnWrap(L, 1);
  lua_pop(L, 1);
  lua_pushinteger(L, lua_Integer(a.arraySizeArg()));
  return 1;
}

static int luaarg_getDir(lua_State *L) {
  CheckArgCount(L, 1);
  lwc::Argument &a = LuaArgument::UnWrap(L, 1);
  lua_pop(L, 1);
  lua_pushinteger(L, a.getDir());
  return 1;
}

static int luaarg_getType(lua_State *L) {
  CheckArgCount(L, 1);
  lwc::Argument &a = LuaArgument::UnWrap(L, 1);
  lua_pop(L, 1);
  lua_pushinteger(L, a.getType());
  return 1;
}

static int luaarg_setDir(lua_State *L) {
  CheckArgCount(L, 2);
  lwc::Argument &a = LuaArgument::UnWrap(L, 1);
  if (!lua_isnumber(L, 2)) {
    return luaL_typerror(L, 2, "integer");
  }
  lua_Integer val = lua_tointeger(L, 2);
  lua_pop(L, 2);
  a.setDir(lwc::Direction(val));
  return 0;
}

static int luaarg_setType(lua_State *L) {
  CheckArgCount(L, 2);
  lwc::Argument &a = LuaArgument::UnWrap(L, 1);
  if (!lua_isnumber(L, 2)) {
    return luaL_typerror(L, 2, "integer");
  }
  lua_Integer val = lua_tointeger(L, 2);
  lua_pop(L, 2);
  a.setType(lwc::Type(val));
  return 0;
}

static int luaarg_setArrayArg(lua_State *L) {
  CheckArgCount(L, 2);
  lwc::Argument &a = LuaArgument::UnWrap(L, 1);
  if (!lua_isnumber(L, 2)) {
    return luaL_typerror(L, 2, "integer");
  }
  lua_Integer val = lua_tointeger(L, 2);
  lua_pop(L, 2);
  a.setArrayArg(val);
  return 0;
}

static int luaarg_setArraySizeArg(lua_State *L) {
  CheckArgCount(L, 2);
  lwc::Argument &a = LuaArgument::UnWrap(L, 1);
  if (!lua_isnumber(L, 2)) {
    return luaL_typerror(L, 2, "integer");
  }
  lua_Integer val = lua_tointeger(L, 2);
  lua_pop(L, 2);
  a.setArraySizeArg(val);
  return 0;
}

// ---

bool InitArgument(lua_State *L, int module) {
  
  lua_newtable(L);
  int klass = lua_gettop(L);
  
  // make the user data callable by forwarding all indexing to its class
  lua_pushvalue(L, klass);
  lua_setfield(L, klass, "__index");
  
  lua_pushcfunction(L, luaarg_new);
  lua_setfield(L, klass, "new");
  lua_pushcfunction(L, luaarg_del);
  lua_setfield(L, klass, "__gc");
  lua_pushcfunction(L, luaarg_tos);
  lua_setfield(L, klass, "__tostring");
  lua_pushcfunction(L, luaarg_isArray);
  lua_setfield(L, klass, "isArray");
  lua_pushcfunction(L, luaarg_getDir);
  lua_setfield(L, klass, "getDir");
  lua_pushcfunction(L, luaarg_setDir);
  lua_setfield(L, klass, "setDir");
  lua_pushcfunction(L, luaarg_getType);
  lua_setfield(L, klass, "getType");
  lua_pushcfunction(L, luaarg_setType);
  lua_setfield(L, klass, "setType");
  lua_pushcfunction(L, luaarg_arrayArg);
  lua_setfield(L, klass, "arrayArg");
  lua_pushcfunction(L, luaarg_setArrayArg);
  lua_setfield(L, klass, "setArrayArg");
  lua_pushcfunction(L, luaarg_arraySizeArg);
  lua_setfield(L, klass, "arraySizeArg");
  lua_pushcfunction(L, luaarg_setArraySizeArg);
  lua_setfield(L, klass, "setArraySizeArg");
  
  lua_pushvalue(L, klass);
  lua_setfield(L, LUA_REGISTRYINDEX, LuaArgument::RegistryKey());
  lua_setfield(L, module, "Argument");
  
  return true;
}
  
}


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
  
  
LuaMethod::LuaMethod() {
}

LuaMethod::LuaMethod(const lwc::Method &m)
  : meth(m) {
}


LuaMethod::~LuaMethod() {
}

size_t LuaMethod::AllocSize() {
  return (sizeof(LuaMethod));
}

const char* LuaMethod::RegistryKey() {
  return "__lwc_lua_method";
}

int LuaMethod::New(lua_State *L) {
  CheckArgCount(L, 0);
  void *ud = lua_newuserdata(L, LuaMethod::AllocSize());
  new (ud) LuaMethod();
  lua_getfield(L, LUA_REGISTRYINDEX, LuaMethod::RegistryKey());
  lua_setmetatable(L, -2);
  return 1;
}

int LuaMethod::Wrap(lua_State *L, const lwc::Method &m) {
  void *ud = lua_newuserdata(L, sizeof(LuaMethod));
  new (ud) LuaMethod(m);
  lua_getfield(L, LUA_REGISTRYINDEX, LuaMethod::RegistryKey());
  lua_setmetatable(L, -2);
  return 1;
}

lwc::Method& LuaMethod::UnWrap(lua_State *L, int narg, bool upValue) {
  int idx = (upValue ? lua_upvalueindex(narg) : narg);
  // could be a subclass?
  void *p = lua_touserdata(L, idx);
  if (!p) {
    luaL_typerror(L, narg, "llwc.Method");
  }
  lua_getfield(L, LUA_REGISTRYINDEX, LuaMethod::RegistryKey());
  if (!lua_getmetatable(L, idx) || !lua_rawequal(L, -1, -2)) {
    luaL_typerror(L, narg, "llwc.Method");
  }
  lua_pop(L, 2);
  return ((LuaMethod*) p)->meth;
}

int LuaMethod::Del(lua_State *L) {
  CheckArgCount(L, 1);
  void *ud = lua_touserdata(L, 1);
  ((LuaMethod*)ud)->~LuaMethod();
  lua_pop(L, 1);
  return 0;
}

// ---

static int luameth_new(lua_State *L) {
  return LuaMethod::New(L);
}

static int luameth_del(lua_State *L) {
  return LuaMethod::Del(L);
}

static int luameth_tos(lua_State *L) {
  CheckArgCount(L, 1);
  lwc::Method &m = LuaMethod::UnWrap(L, 1);
  lua_pop(L, 1);
  lua_pushstring(L, m.toString().c_str());
  return 1;
}

static int luameth_docString(lua_State *L) {
  int nargs = lua_gettop(L);
  if (nargs < 1 || nargs > 2) {
    lua_pushstring(L, "1 or 2 arguments expected (including self)");
    lua_error(L);
    // lua_error doesn't return (long jump) just as rb_raise
    return 0;
  }
  lwc::Method &m = LuaMethod::UnWrap(L, 1);
  std::string indent = "";
  if (nargs == 2) {
    if (!lua_isstring(L, 2)) {
      return luaL_typerror(L, 2, "string");
    }
    indent = lua_tostring(L, 2);
  }
  lua_pop(L, nargs);
  std::string s = m.docString(indent);
  lua_pushstring(L, s.c_str());
  return 1;
}

static int luameth_numArgs(lua_State *L) {
  CheckArgCount(L, 1);
  lwc::Method &m = LuaMethod::UnWrap(L, 1);
  lua_pop(L, 1);
  lua_pushinteger(L, m.numArgs());
  return 1;
}

static int luameth_numPositionalArgs(lua_State *L) {
  CheckArgCount(L, 1);
  lwc::Method &m = LuaMethod::UnWrap(L, 1);
  lua_pop(L, 1);
  lua_pushinteger(L, m.numPositionalArgs());
  return 1;
}

static int luameth_namedArgIndex(lua_State *L) {
  CheckArgCount(L, 2);
  lwc::Method &m = LuaMethod::UnWrap(L, 1);
  std::string name = luaL_checkstring(L, 2);
  lua_pop(L, 2);
  try {
    lua_pushinteger(L, m.namedArgIndex(name.c_str()));
  } catch (std::exception &e) {
    lua_pushstring(L, e.what());
    lua_error(L);
  }
  return 1;
}

static int luameth_getDesc(lua_State *L) {
  CheckArgCount(L, 1);
  lwc::Method &m = LuaMethod::UnWrap(L, 1);
  lua_pop(L, 1);
  const char *desc = m.getDescription();
  lua_pushstring(L, (desc ? desc : ""));
  return 1;
}

static int luameth_setDesc(lua_State *L) {
  CheckArgCount(L, 2);
  lwc::Method &m = LuaMethod::UnWrap(L, 1);
  std::string desc = luaL_checkstring(L, 2);
  lua_pop(L, 2);
  m.setDescription(desc.c_str());
  return 0;
}

static int luameth_addArg(lua_State *L) {
  CheckArgCount(L, 2);
  lwc::Method &m = LuaMethod::UnWrap(L, 1);
  lwc::Argument &a = LuaArgument::UnWrap(L, 2);
  lua_pop(L, 2);
  m.addArg(a);
  return 0;
}

static int luameth_setArg(lua_State *L) {
  CheckArgCount(L, 3);
  lwc::Method &m = LuaMethod::UnWrap(L, 1);
  if (!lua_isnumber(L, 2)) {
    luaL_typerror(L, 2, "integer");
  }
  lua_Integer idx = lua_tointeger(L, 2);
  lwc::Argument &a = LuaArgument::UnWrap(L, 3);
  lua_pop(L, 3);
  m[idx-1] = a;
  return 0;
}

static int luameth_getArg(lua_State *L) {
  CheckArgCount(L, 2);
  lwc::Method &m = LuaMethod::UnWrap(L, 1);
  if (!lua_isnumber(L, 2)) {
    luaL_typerror(L, 2, "integer");
  }
  lua_Integer idx = lua_tointeger(L, 2);
  lua_pop(L, 2);
  const lwc::Argument &a = m[idx-1];
  return LuaArgument::Wrap(L, a);
}

// ---

bool InitMethod(lua_State *L, int module) {
  
  lua_newtable(L);
  int klass = lua_gettop(L);
  
  // make the user data callable by forwarding all indexing to its class
  lua_pushvalue(L, klass);
  lua_setfield(L, klass, "__index");
  
  lua_pushcfunction(L, luameth_new);
  lua_setfield(L, klass, "new");
  lua_pushcfunction(L, luameth_del);
  lua_setfield(L, klass, "__gc");
  lua_pushcfunction(L, luameth_tos);
  lua_setfield(L, klass, "__tostring");
  lua_pushcfunction(L, luameth_numArgs);
  lua_setfield(L, klass, "numArgs");
  lua_pushcfunction(L, luameth_addArg);
  lua_setfield(L, klass, "addArg");
  lua_pushcfunction(L, luameth_setArg);
  lua_setfield(L, klass, "setArg");
  lua_pushcfunction(L, luameth_getArg);
  lua_setfield(L, klass, "getArg");
  lua_pushcfunction(L, luameth_setDesc);
  lua_setfield(L, klass, "setDescription");
  lua_pushcfunction(L, luameth_getDesc);
  lua_setfield(L, klass, "getDescription");
  lua_pushcfunction(L, luameth_namedArgIndex);
  lua_setfield(L, klass, "namedArgIndex");
  lua_pushcfunction(L, luameth_numPositionalArgs);
  lua_setfield(L, klass, "numPositionalArgs");
  lua_pushcfunction(L, luameth_docString);
  lua_setfield(L, klass, "docString");
  
  lua_pushvalue(L, klass);
  lua_setfield(L, LUA_REGISTRYINDEX, LuaMethod::RegistryKey());
  lua_setfield(L, module, "Method");
  
  return true;
}
}



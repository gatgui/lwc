/*

This file is part of lwc.

lwc is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

lwc is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with lwc.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <lwc/lua/types.h>
#include <map>
#include <string>

namespace lua {

static std::map<std::string, int> EnumConstants;

static void InitEnumConstants() {
  EnumConstants["AT_UNKNOWN"] = lwc::AT_UNKNOWN;
  EnumConstants["AT_BOOL"] = lwc::AT_BOOL;
  EnumConstants["AT_CHAR"] = lwc::AT_CHAR;
  EnumConstants["AT_SHORT"] = lwc::AT_SHORT;
  EnumConstants["AT_INT"] = lwc::AT_INT;
  EnumConstants["AT_LONG"] = lwc::AT_LONG;
  EnumConstants["AT_UCHAR"] = lwc::AT_UCHAR;
  EnumConstants["AT_USHORT"] = lwc::AT_USHORT;
  EnumConstants["AT_UINT"] = lwc::AT_UINT;
  EnumConstants["AT_ULONG"] = lwc::AT_ULONG;
  EnumConstants["AT_FLOAT"] = lwc::AT_FLOAT;
  EnumConstants["AT_DOUBLE"] = lwc::AT_DOUBLE;
  EnumConstants["AT_STRING"] = lwc::AT_STRING;
  EnumConstants["AT_OBJECT"] = lwc::AT_OBJECT;
  EnumConstants["AD_IN"] = lwc::AD_IN;
  EnumConstants["AD_INOUT"] = lwc::AD_INOUT;
  EnumConstants["AD_RETURN"] = lwc::AD_RETURN;
  EnumConstants["AD_OUT"] = lwc::AD_OUT;
}

// ---

static int lwclua_index(lua_State *L) {
  std::string key = lua_tostring(L, 2);
  std::map<std::string, int>::iterator it = EnumConstants.find(key);
  if (it != EnumConstants.end()) {
    lua_pushinteger(L, it->second);
    return 1;
  } else {
    lua_pushstring(L, "invalid constant");
    lua_error(L);
    return 0;
  }
}

static int lwclua_newindex(lua_State *L) {
  lua_pushstring(L, "llwc module is read only");
  lua_error(L);
  return 0;
}

static int lwclua_init(lua_State *L) {
  lwc::Registry *reg = lwc::Registry::Initialize("lua", (void*)L);
  return LuaRegistry::Wrap(L, reg);
}

static int lwclua_getreg(lua_State *L) {
  return LuaRegistry::Wrap(L, lwc::Registry::Instance());
}

static int lwclua_deinit(lua_State *L) {
  lwc::Registry::DeInitialize();
  return 0;
}

// ---

int CreateModule(lua_State *L) {
  
  lua_newtable(L);
  int module = lua_gettop(L);
  
  InitEnumConstants();
  
  if (!InitArgument(L, module)) {
    lua_pop(L, 1);
    return 0;
  }
  
  if (!InitMethod(L, module)) {
    lua_pop(L, 1);
    return 0;
  }
  
  if (!InitMethodsTable(L, module)) {
    lua_pop(L, 1);
    return 0;
  }
  
  if (!InitObject(L, module)) {
    lua_pop(L, 1);
    return 0;
  }
  
  if (!InitRegistry(L, module)) {
    lua_pop(L, 1);
    return 0;
  }
  
  lua_pushcfunction(L, lwclua_init);
  lua_setfield(L, -2, "Initialize");
  
  lua_pushcfunction(L, lwclua_getreg);
  lua_setfield(L, -2, "GetRegistry");
  
  lua_pushcfunction(L, lwclua_deinit);
  lua_setfield(L, -2, "DeInitialize");
  
  lua_newtable(L);
  lua_pushcfunction(L, lwclua_index);
  lua_setfield(L, -2, "__index");
  lua_pushcfunction(L, lwclua_newindex);
  lua_setfield(L, -2, "__newindex");
  lua_pushvalue(L, -1);
  lua_setmetatable(L, module);
  
  lua_pushvalue(L, module);
  lua_setfield(L, LUA_GLOBALSINDEX, "llwc");
  
  return 1;
}
  
}


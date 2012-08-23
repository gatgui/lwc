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
#include <lwc/lua/convert.h>

namespace lua {

// ---

void FreeDefaultValue(struct DefaultValueEntry &dve);

struct DefaultValueEntry
{
   lwc::Type type;
   bool array;
   size_t len;
   void *ptr;
   
   DefaultValueEntry()
     : type(lwc::AT_UNKNOWN)
     , array(false)
     , len(0)
     , ptr(0) {
   }
   
   ~DefaultValueEntry() {
     FreeDefaultValue(*this);
   }
};

static std::deque<DefaultValueEntry> gsDefaultValues;

void AddDefaultValue(void *ptr, lwc::Type t, bool array, size_t len=0) {
#ifdef LWC_MEMTRACK
  std::cout << "Add lwc lua module default value " << std::hex << ptr << std::dec << ", " << t << ", " << array << ", " << len << std::endl;
#endif
  gsDefaultValues.push_back(DefaultValueEntry());
  DefaultValueEntry &dve = gsDefaultValues.back();
  dve.type = t;
  dve.array = array;
  dve.len = len;
  dve.ptr = ptr;
}

bool SetArgDefault(lwc::Argument &arg, lua_State *L, int idx) {
  if (arg.getDir() != lwc::AD_IN) {
    return false;
  }
  
  bool rv = true;
  
  switch (arg.getType()) {
  case lwc::AT_BOOL:
    if (arg.isArray()) {
      bool *defVal = 0;
      size_t len = 0;
      Lua2C<lwc::AT_BOOL>::ToArray(L, idx, defVal, len);
      arg.setDefaultValue(defVal);
      AddDefaultValue((void*)defVal, arg.getType(), true, len);
    } else {
      bool defVal = false;
      Lua2C<lwc::AT_BOOL>::ToValue(L, idx, defVal);
      arg.setDefaultValue(defVal);
    }
    break;
  case lwc::AT_INT:
    if (arg.isArray()) {
      lwc::Integer *defVal = 0;
      size_t len = 0;
      Lua2C<lwc::AT_INT>::ToArray(L, idx, defVal, len);
      arg.setDefaultValue(defVal);
      AddDefaultValue((void*)defVal, arg.getType(), true, len);
    } else {
      lwc::Integer defVal = 0;
      Lua2C<lwc::AT_INT>::ToValue(L, idx, defVal);
      arg.setDefaultValue(defVal);
    }
    break;
  case lwc::AT_REAL:
    if (arg.isArray()) {
      lwc::Real *defVal = 0;
      size_t len = 0;
      Lua2C<lwc::AT_REAL>::ToArray(L, idx, defVal, len);
      arg.setDefaultValue(defVal);
      AddDefaultValue((void*)defVal, arg.getType(), true, len);
    } else {
      lwc::Real defVal = 0.0;
      Lua2C<lwc::AT_REAL>::ToValue(L, idx, defVal);
      arg.setDefaultValue(defVal);
    }
    break;
  case lwc::AT_STRING:
    if (arg.isArray()) {
      char **defVal = 0;
      size_t len = 0;
      Lua2C<lwc::AT_STRING>::ToArray(L, idx, defVal, len);
      arg.setDefaultValue(defVal);
      AddDefaultValue((void*)defVal, arg.getType(), true, len);
    } else {
      char *defVal = 0;
      Lua2C<lwc::AT_STRING>::ToValue(L, idx, defVal);
      arg.setDefaultValue(defVal);
      AddDefaultValue((void*)defVal, arg.getType(), false);
    }
    break;
  case lwc::AT_OBJECT:
    if (arg.isArray()) {
      lwc::Object **defVal = 0;
      size_t len = 0;
      Lua2C<lwc::AT_OBJECT>::ToArray(L, idx, defVal, len);
      arg.setDefaultValue(defVal);
      AddDefaultValue((void*)defVal, arg.getType(), true, len);
    } else {
      lwc::Object *defVal = 0;
      Lua2C<lwc::AT_OBJECT>::ToValue(L, idx, defVal);
      arg.setDefaultValue(defVal);
      AddDefaultValue((void*)defVal, arg.getType(), false);
    }
    break;
  default:
    rv = false;
    break;
  }
  
  return rv;
}

void FreeDefaultValue(DefaultValueEntry &dve) {
  if (dve.ptr == 0) {
    return;
  }
#ifdef LWC_MEMTRACK
  std::cout << "Free lwc lua module default value " << std::hex << dve.ptr << std::dec << ", " << dve.type << ", " << dve.array << ", " << dve.len << std::endl;
#endif
  switch (dve.type) {
  case lwc::AT_BOOL:
    if (dve.array) {
      bool *ary = (bool*) dve.ptr;
      Lua2C<lwc::AT_BOOL>::DisposeArray(ary, dve.len);
    }
    break;
  case lwc::AT_INT:
    if (dve.array) {
      lwc::Integer *ary = (lwc::Integer*) dve.ptr;
      Lua2C<lwc::AT_INT>::DisposeArray(ary, dve.len);
    }
    break;
  case lwc::AT_REAL:
    if (dve.array) {
      lwc::Real *ary = (lwc::Real*) dve.ptr;
      Lua2C<lwc::AT_REAL>::DisposeArray(ary, dve.len);
    }
    break;
  case lwc::AT_STRING:
    if (dve.array) {
      char **ary = (char**) dve.ptr;
      Lua2C<lwc::AT_STRING>::DisposeArray(ary, dve.len);
    } else {
      char *str = (char*) dve.ptr;
      Lua2C<lwc::AT_STRING>::DisposeValue(str);
    }
    break;
  case lwc::AT_OBJECT:
    if (dve.array) {
      lwc::Object **ary = (lwc::Object**) dve.ptr;
      Lua2C<lwc::AT_OBJECT>::DisposeArray(ary, dve.len);
    } else {
      lwc::Object *obj = (lwc::Object*) dve.ptr;
      Lua2C<lwc::AT_OBJECT>::DisposeValue(obj);
    }
  default:
    break;
  }
}

// ---

void CleanupModule() {
#ifdef LWC_MEMTRACK
  std::cout << "=== lwc library: Memory status after lua module cleanup" << std::endl;
  std::cout << lwc::Object::GetInstanceCount() << " remaining object(s)" << std::endl;
  lwc::memory::PrintAllocationInfo();
#endif
}

// ---

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
  // should work with 0, 2 or 3 args
  //CheckArgCount(L, 0);
  lwc::Direction d = lwc::AD_IN;
  lwc::Type t = lwc::AT_UNKNOWN;
  int lenidx = -1;
  int defarg = -1; 
  std::string name;
  
  int nargs = lua_gettop(L);
  if (nargs != 0 && nargs != 2 && nargs != 3) {
    lua_pushstring(L, "expected 0 or 2 arguments + keywords");
    lua_error(L);
    return 0;
  }
  
  if (nargs >= 2) {
    if (!lua_isnumber(L, 1)) {
      return luaL_typerror(L, 1, "integer");
    }
    d = (lwc::Direction) lua_tointeger(L, 1);
    lua_remove(L, 1);
    
    if (!lua_isnumber(L, 1)) {
      return luaL_typerror(L, 1, "integer");
    }
    t = (lwc::Type) lua_tointeger(L, 2);
    lua_remove(L, 1);
    
    if (nargs == 3) {
      if (!lua_istable(L, 1)) {
        lua_pushstring(L, "expected a table as third argument");
        lua_error(L);
        return 0;
      }
      
      lua_pushstring(L, "lenidx");
      lua_gettable(L, 1);
      if (!lua_isnil(L, -1)) {
        if (!lua_isnumber(L, -1)) {
          lua_pushstring(L, "lenidx keyword argument must be a number");
          lua_error(L);
          return 0;
        }
        lenidx = luaL_checkint(L, -1);
      }
      lua_pop(L, 1);
      
      lua_pushstring(L, "name");
      lua_gettable(L, 1);
      if (!lua_isnil(L, -1)) {
        if (!lua_isstring(L, -1)) {
          lua_pushstring(L, "name keyword argument must be a string");
          lua_error(L);
          return 0;
        }
        name = lua_tostring(L, -1);
      }
      lua_pop(L, 1);
      
      lua_pushstring(L, "def");
      lua_gettable(L, 1);
      if (!lua_isnil(L, -1)) {
        // will keep defval on top of stack
        defarg = 1;
      } else {
        lua_pop(L, 1);
      }
      
      // remove table from stack
      lua_remove(L, 1);
      
      // at this point, stack will be either empty or contain default value
    }
  }
  
  void *ud = lua_newuserdata(L, LuaArgument::AllocSize());
  LuaArgument *arg = new (ud) LuaArgument();
  lua_getfield(L, LUA_REGISTRYINDEX, LuaArgument::RegistryKey());
  lua_setmetatable(L, -2);
  // user data on top of the stack
  
  arg->arg.setDir(d);
  arg->arg.setType(t);
  if (lenidx >= 0) {
    arg->arg.setArraySizeArg(lenidx);
  }
  if (name.length() > 0) {
    arg->arg.setName(name.c_str());
  }
  if (defarg != -1) {
    SetArgDefault(arg->arg, L, defarg);
    lua_remove(L, defarg);
  }
  
#ifdef _DEBUG
  if (lua_gettop(L) != 1) {
    std::cout << "LuaArgument::New: Possible stack corruption" << std::endl;
  }
#endif
  
  return lua_gettop(L);
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

static int luaarg_docString(lua_State *L) {
  int nargs = lua_gettop(L);
  if (nargs < 1 || nargs > 2) {
    lua_pushstring(L, "1 or 2 arguments expected (including self)");
    lua_error(L);
    // lua_error doesn't return (long jump) just as rb_raise
    return 0;
  }
  lwc::Argument &a = LuaArgument::UnWrap(L, 1);
  std::string indent = "";
  if (nargs == 2) {
    if (!lua_isstring(L, 2)) {
      return luaL_typerror(L, 2, "string");
    }
    indent = lua_tostring(L, 2);
  }
  lua_pop(L, nargs);
  std::string s = a.docString(indent);
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

static int luaarg_isNamed(lua_State *L) {
  CheckArgCount(L, 1);
  lwc::Argument &a = LuaArgument::UnWrap(L, 1);
  lua_pop(L, 1);
  lua_pushboolean(L, a.isNamed());
  return 1;
}

static int luaarg_hasDefaultValue(lua_State *L) {
  CheckArgCount(L, 1);
  lwc::Argument &a = LuaArgument::UnWrap(L, 1);
  lua_pop(L, 1);
  lua_pushboolean(L, a.hasDefaultValue());
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

static int luaarg_getName(lua_State *L) {
  CheckArgCount(L, 1);
  lwc::Argument &a = LuaArgument::UnWrap(L, 1);
  lua_pop(L, 1);
  lua_pushstring(L, a.getName().c_str());
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

static int luaarg_setName(lua_State *L) {
  CheckArgCount(L, 2);
  lwc::Argument &a = LuaArgument::UnWrap(L, 1);
  if (!lua_isstring(L, 2)) {
    return luaL_typerror(L, 2, "string");
  }
  const char *name = lua_tostring(L, 2);
  lua_pop(L, 2);
  a.setName(name);
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
  lua_pushcfunction(L, luaarg_docString);
  lua_setfield(L, klass, "docString");
  lua_pushcfunction(L, luaarg_isArray);
  lua_setfield(L, klass, "isArray");
  lua_pushcfunction(L, luaarg_isNamed);
  lua_setfield(L, klass, "isNamed");
  lua_pushcfunction(L, luaarg_hasDefaultValue);
  lua_setfield(L, klass, "hasDefaultValue");
  lua_pushcfunction(L, luaarg_setName);
  lua_setfield(L, klass, "setName");
  lua_pushcfunction(L, luaarg_getName);
  lua_setfield(L, klass, "getName");
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


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

#ifndef __lwc_lua_convert_h__
#define __lwc_lua_convert_h__

#include <lwc/lua/config.h>
#include <lwc/lua/types.h>

namespace lua {
  
  template <typename T> struct LuaType {
    static const char* Name() {return "unknown";}
    static bool Check(lua_State *, int) {return false;}
    static void ToC(lua_State *, int, T &) {}
    static void Dispose(T &) {}
  };
  template <> struct LuaType<bool> {
    static const char* Name() {return "boolean";}
    static bool Check(lua_State *L, int idx) {return lua_isboolean(L, idx);}
    static void ToC(lua_State *L, int idx, bool &val) {val = lua_toboolean(L, idx);}
    static void Dispose(bool &) {}
  };
  template <> struct LuaType<lwc::Integer> {
    static const char* Name() {return "integer";}
    static bool Check(lua_State *L, int idx) {return lua_isnumber(L, idx);}
    static void ToC(lua_State *L, int idx, lwc::Integer &val) {val = (lwc::Integer) lua_tointeger(L, idx);}
    static void Dispose(lwc::Integer &) {}
  };
  template <> struct LuaType<lwc::Real> {
    static const char* Name() {return "real";}
    static bool Check(lua_State *L, int idx) {return lua_isnumber(L, idx);}
    static void ToC(lua_State *L, int idx, lwc::Real &val) {val = (lwc::Real) lua_tonumber(L, idx);}
    static void Dispose(lwc::Real &) {}
  };
  template <> struct LuaType<char*> {
    static const char* Name() {return "string";}
    static bool Check(lua_State *L, int idx) {return lua_isstring(L, idx);}
    static void ToC(lua_State *L, int idx, char* &val) {
      const char *str = lua_tostring(L, idx);
      val = (char*) lwc::memory::Alloc(strlen(str)+1, sizeof(char));
      strcpy(val, str);
    }
    static void Dispose(char* &val) {
      lwc::memory::Free((void*)val);
    }
  };
  template <> struct LuaType<lwc::Object*> {
    static const char* Name() {return "il.Object";}
    static bool Check(lua_State *L, int idx) {
      if (!lua_isuserdata(L, idx)) {
        return false;
      }
      void *p = lua_touserdata(L, idx);
      if (!p) {
        return false;
      }
      lua_getfield(L, LUA_REGISTRYINDEX, LuaObject::RegistryKey());
      if (!lua_getmetatable(L, idx)) {
        lua_pop(L, 1);
        return false;
      }
      bool ret = lua_rawequal(L, -1, -2);
      lua_pop(L, 2);
      return ret;
    }
    static void ToC(lua_State *L, int idx, lwc::Object* &val) {
      val = LuaObject::UnWrap(L, idx);
    }
    static void Dispose(lwc::Object *&) {}
  };

  template <typename T> struct CType {
    static void ToLua(const T &, lua_State *L) {}
  };
  template <> struct CType<bool> {
    static void ToLua(const bool &val, lua_State *L) {
      lua_pushboolean(L, val);
    }
  };
  template <> struct CType<lwc::Integer> {
    static void ToLua(const lwc::Integer &val, lua_State *L) {lua_pushinteger(L, val);}
  };
  template <> struct CType<lwc::Real> {
    static void ToLua(const lwc::Real &val, lua_State *L) {lua_pushnumber(L, val);}
  };
  template <> struct CType<char*> {
    static void ToLua(const char *val, lua_State *L) {lua_pushstring(L, val);}
  };
  template <> struct CType<lwc::Object*> {
    static void ToLua(const lwc::Object *val, lua_State *L) {
      LuaObject::Wrap(L, (lwc::Object*)val);
    }
  };

  template <lwc::Type T> struct Lua2C {
    
    typedef typename lwc::Enum2Type<T>::Type Type;
    typedef typename lwc::Enum2Type<T>::Type* Array;
    
    static void ToValue(lua_State *L, int idx, Type &val) {
      if (!LuaType<Type>::Check(L, idx)) {
        char message[512];
        sprintf(message, "Expected %s argument", LuaType<Type>::Name());
        lua_pushstring(L, message);
        lua_error(L);
        return;
      }
      LuaType<Type>::ToC(L, idx, val);
    }
    
    static void DisposeValue(Type &val) {
      LuaType<Type>::Dispose(val);
    }
    
    static void ToArray(lua_State *L, int idx, Array &ary, size_t &length) {
      if (!lua_istable(L, idx)) {
        lua_pushstring(L, "Expected table (as array) argument");
        lua_error(L);
        return;
      }
      
      length = lua_objlen(L, idx);
      
      if (ary != 0 && length != 0) {
        for (size_t i=0; i<length; ++i) {
          LuaType<Type>::Dispose(ary[i]);
        }
      }
      ary = (Array) lwc::memory::Alloc(length, sizeof(Type), (void*)ary);
      
      for (size_t i=0; i<length; ++i) {
        lua_pushinteger(L, i+1); // remember lua indices starts at 1
        lua_gettable(L, idx);
        int item = lua_gettop(L);
        if (!LuaType<Type>::Check(L, item)) {
          char message[512];
          sprintf(message, "Expected array of %ss argument", LuaType<Type>::Name());
          lua_pushstring(L, message);
          lua_error(L);
          return;
        }
        LuaType<Type>::ToC(L, item, ary[i]);
        lua_pop(L, 1);
      }
    }
    
    static void DisposeArray(Array &ary, size_t length) {
      for (size_t i=0; i<length; ++i) {
        LuaType<Type>::Dispose(ary[i]);
      }
      lwc::memory::Free((void*)ary);
    }
  };
  
  
  template <lwc::Type T> struct C2Lua {
    
    typedef typename lwc::Enum2Type<T>::Type Type;
    typedef typename lwc::Enum2Type<T>::Type* Array;
    
    // beware: const Type & width Type == char* gives char * const &val
    static void ToValue(const Type &val, lua_State *L) {
      CType<Type>::ToLua(val, L);
    }
    
    static void ToArray(const Array &ary, size_t length, lua_State *L, bool onTop=false) {
      if (onTop && lua_istable(L, -1)) {
        // we have an already existing array use it
        size_t sz = lua_objlen(L, -1);
        if (length > sz) {
          // don't need to push nil values, array is resize automatically
          
        } else if (length < sz) {
          // set elements nil
          size_t n = sz - length;
          for (size_t i=0; i<n; ++i) {
            lua_pushinteger(L, length+i+1);
            lua_pushnil(L);
            lua_settable(L, -2);
          }
        }
        
      } else {
        lua_newtable(L);
      }
      
      for (size_t i=0; i<length; ++i) {
        lua_pushinteger(L, i+1);
        CType<Type>::ToLua(ary[i], L);
        lua_settable(L, -2);
      }
    }
  };
  
  
  template <lwc::Type T> struct ParamConverter {
    
    typedef typename lwc::Enum2Type<T>::Type Type;
    typedef typename lwc::Enum2Type<T>::Type* Array;
    
    // after the PostCall, the stack should be unchanged
    // rv is the stack index of the return value(s) array
    
    static void PreCall(const lwc::Argument &desc, size_t idesc,
                        lua_State *L, int firstArg, size_t nargs, size_t &iarg,
                        std::map<size_t,size_t> &arraySizes, Type &val) {
      
      if (desc.getDir() == lwc::AD_IN || desc.getDir() == lwc::AD_INOUT) {
        if (desc.arrayArg() != -1) {
          // this should only be executed for integer types
          val = (Type) arraySizes[desc.arrayArg()];
        } else {
          if (iarg >= nargs) {
            lua_pushstring(L, "Not enough arguments");
            lua_error(L);
            return;
          }
          Lua2C<T>::ToValue(L, firstArg+iarg, val);
          ++iarg;
        }     
      }
    }
    
    static void PostCall(const lwc::Argument &desc, size_t idesc,
                         lua_State *L, int firstArg, size_t nargs, size_t &iarg,
                         std::map<size_t,size_t> &arraySizes, Type &val, int rv) {
      
      if (desc.getDir() == lwc::AD_IN) {
        Lua2C<T>::DisposeValue(val);
        ++iarg;
        
      } else {
        if (desc.arrayArg() != -1) {
          arraySizes[desc.arrayArg()] = size_t(val);
          Lua2C<T>::DisposeValue(val);
          
        } else {
          lua_pushinteger(L, lua_objlen(L, rv)+1);
          C2Lua<T>::ToValue(val, L);
          Lua2C<T>::DisposeValue(val);
          lua_settable(L, rv);
        }
      }
    }
    
    static void PreCallArray(const lwc::Argument &desc, size_t idesc,
                             lua_State *L, int firstArg, size_t nargs, size_t &iarg,
                             std::map<size_t,size_t> &arraySizes, Array &ary) {
      if (desc.getDir() == lwc::AD_IN || desc.getDir() == lwc::AD_INOUT) {
        size_t length;
        if (iarg >= nargs) {
          lua_pushstring(L, "Not enough arguments");
          lua_error(L);
          return;
        }
        Lua2C<T>::ToArray(L, firstArg+iarg, ary, length);
        arraySizes[idesc] = length;
        ++iarg;
      }
    }
    
    static void PostCallArray(const lwc::Argument &desc, size_t idesc,
                              lua_State *L, int firstArg, size_t nargs, size_t &iarg,
                              std::map<size_t,size_t> &arraySizes, Array &ary, int rv) {
      if (desc.getDir() == lwc::AD_IN) {
        Lua2C<T>::DisposeArray(ary, arraySizes[idesc]);
        
      } else {
        bool onTop = false;
        if (desc.getDir() == lwc::AD_INOUT) {
          // array both input and output -> need to set the onTop in C2Lua<>::ToArray
          onTop = true;
          lua_pushvalue(L, firstArg+iarg);
        }  
        C2Lua<T>::ToArray(ary, arraySizes[idesc], L, onTop);
        if (onTop) {
          lua_pop(L, 1);
        }
        if (desc.getDir() != lwc::AD_INOUT) {
          Lua2C<T>::DisposeArray(ary, arraySizes[idesc]);
          // add to output
          lua_pushinteger(L, lua_objlen(L, rv)+1);
          lua_pushvalue(L, -2); // the returned array was pushed on top
          lua_settable(L, rv);
          // also pop return value?
          
        } else {
          Lua2C<T>::DisposeArray(ary, arraySizes[idesc]);
        }
        
        // finally pop the return value of C2Lua<>::ToArray
        lua_pop(L, 1);
      }
    }
  };
  
  
}

#endif


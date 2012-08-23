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
    static bool Check(lua_State *L, int idx) {return (lua_isboolean(L, idx) != 0);}
    static void ToC(lua_State *L, int idx, bool &val) {val = (lua_toboolean(L, idx) == 1);}
    static void Dispose(bool &) {}
  };
  template <> struct LuaType<lwc::Integer> {
    static const char* Name() {return "integer";}
    static bool Check(lua_State *L, int idx) {return (lua_isnumber(L, idx) != 0);}
    static void ToC(lua_State *L, int idx, lwc::Integer &val) {val = (lwc::Integer) lua_tointeger(L, idx);}
    static void Dispose(lwc::Integer &) {}
  };
  template <> struct LuaType<lwc::Real> {
    static const char* Name() {return "real";}
    static bool Check(lua_State *L, int idx) {return (lua_isnumber(L, idx) != 0);}
    static void ToC(lua_State *L, int idx, lwc::Real &val) {val = (lwc::Real) lua_tonumber(L, idx);}
    static void Dispose(lwc::Real &) {}
  };
  template <> struct LuaType<char*> {
    static const char* Name() {return "string";}
    static bool Check(lua_State *L, int idx) {return (lua_isstring(L, idx) || lua_isnil(L, idx));}
    static void ToC(lua_State *L, int idx, char* &val) {
      if (lua_isnil(L, idx)) {
        val = 0;
      } else {
        const char *str = lua_tostring(L, idx);
        val = (char*) lwc::memory::Alloc(strlen(str)+1, sizeof(char));
        strcpy(val, str);
      }
    }
    static void Dispose(char* &val) {
      if (val) {
        lwc::memory::Free((void*)val);
      }
    }
  };
  template <> struct LuaType<lwc::Object*> {
    static const char* Name() {return "il.Object";}
    static bool Check(lua_State *L, int idx) {
      if (lua_isnil(L, idx)) {
        return true;
      }
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
      bool ret = (lua_rawequal(L, -1, -2) == 1);
      lua_pop(L, 2);
      return ret;
    }
    static void ToC(lua_State *L, int idx, lwc::Object* &val) {
      if (lua_isnil(L, idx)) {
        val = 0;
      } else {
        val = LuaObject::UnWrap(L, idx);
      }
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
    static void ToLua(const lwc::Integer &val, lua_State *L) {lua_pushinteger(L, lua_Integer(val));}
  };
  template <> struct CType<lwc::Real> {
    static void ToLua(const lwc::Real &val, lua_State *L) {lua_pushnumber(L, val);}
  };
  template <> struct CType<char*> {
    static void ToLua(const char *val, lua_State *L) {
      if (!val) {
        lua_pushnil(L);
      } else {
        lua_pushstring(L, val);
      }
    }
  };
  template <> struct CType<lwc::Object*> {
    static void ToLua(const lwc::Object *val, lua_State *L) {
      if (!val) {
        lua_pushnil(L);
        return;
      }
      if (!strcmp(val->getLoaderName(), "lualoader")) {
        lua_pushlightuserdata(L, (void*)val);
        lua_gettable(L, LUA_REGISTRYINDEX);
      } else {
        LuaObject::Wrap(L, (lwc::Object*)val);
      }
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
        lua_settable(L, -3);
      }
    }
  };
  
  
  template <lwc::Type T> struct ParamConverter {
    
    typedef typename lwc::Enum2Type<T>::Type Type;
    typedef typename lwc::Enum2Type<T>::Type* Array;
    
    // after the PostCall, the stack should be unchanged
    // rv is the stack index of the return value(s) array
    
    template <typename TT>
    static bool GetDefaultValue(const lwc::Argument &desc, TT &val) {
#ifdef _DEBUG
      std::cout << "Get default value for " << desc.toString() << std::endl;
#endif
      if (!desc.hasDefaultValue()) {
        return false;
      }
      
      const lwc::ArgumentValue &dv = desc.getRawDefaultValue();
      
      lwc::Type dt = desc.getType();
      
      if (desc.isArray()) {
        // if TT is a pointer, gcore::TypeTraits<TT>::Value will be the pointer type
        // that must match the argument type
        if (lwc::Type2Enum<typename gcore::TypeTraits<TT>::Value>::Enum != (int)dt) {
          std::cout << "Array type mismatch" << std::endl;
          return false;
        }
      } else {
        if (lwc::Type2Enum<TT>::Enum != (int)dt) {
          std::cout << "Type mismatch" << std::endl;
          return false;
        }
      }
      
      if (!desc.isArray()) {
        if (dt == lwc::AT_BOOL) {
          if (lwc::Convertion<bool, TT>::Possible()) {
            lwc::Convertion<bool, TT>::Do(dv.boolean, val);
            return true;
          }
        } else if (dt == lwc::AT_INT) {
          if (lwc::Convertion<lwc::Integer, TT>::Possible()) {
            lwc::Convertion<lwc::Integer, TT>::Do(dv.integer, val);
            return true;
          }
        } else if (dt == lwc::AT_REAL) {
          if (lwc::Convertion<lwc::Real, TT>::Possible()) {
            lwc::Convertion<lwc::Real, TT>::Do(dv.real, val);
            return true;
          }
        }
      } else {
        if (lwc::Convertion<void*, TT>::Possible()) {
          lwc::Convertion<void*, TT>::Do(dv.ptr, val);
          return true;
        }
      }
      
      return false;
    }
    
    static bool HasKey(lua_State *L, int arg, const std::string &name) {
      if (!lua_istable(L, arg)) {
        return false;
      }
      lua_pushstring(L, name.c_str());
      lua_gettable(L, arg);
      bool rv = !lua_isnil(L, -1);
      lua_pop(L, 1);
      return rv;
    }
    
    static bool PreCall(const lwc::Argument &desc, size_t /*idesc*/,
                        lua_State *L, int firstArg, size_t nargs, int kwargs, size_t &iarg,
                        std::map<size_t,size_t> &arraySizes, Type &val,
                        std::string &err) {
      
      if (desc.getDir() == lwc::AD_IN || desc.getDir() == lwc::AD_INOUT) {
        if (desc.arrayArg() >= 0) {
          // this should only be executed for integer types
          //val = (Type) arraySizes[desc.arrayArg()];
          unsigned long idx = (unsigned long) arraySizes[size_t(desc.arrayArg())];
          lwc::Convertion<unsigned long, Type>::Do(idx, val);
        } else {
          if (iarg >= nargs) {
            bool failed = true;
            if (desc.isNamed() && kwargs >= 0) {
              lua_pushstring(L, desc.getName().c_str());
              lua_gettable(L, firstArg+kwargs);
              if (!lua_isnil(L, -1)) {
                Lua2C<T>::ToValue(L, lua_gettop(L), val);
                failed = false;
              } else {
                failed = !GetDefaultValue(desc, val);
              }
              lua_pop(L, 1);
            }
            if (failed) {
              err = "Not enough arguments";
              return false;
            }
          } else {
            Lua2C<T>::ToValue(L, firstArg+iarg, val);
            ++iarg;
          }
        }
      }
      return true;
    }
    
    static void PostCall(const lwc::Argument &desc, size_t /*idesc*/,
                         lua_State *L, int firstArg, size_t nargs, int kwargs, size_t &iarg,
                         std::map<size_t,size_t> &arraySizes, Type &val, int rv) {
      
      bool dontDispose = false;
      if (iarg >= nargs && desc.isNamed() && 
          (kwargs < 0 || HasKey(L, firstArg+kwargs, desc.getName())) &&
          desc.hasDefaultValue()) {
        dontDispose = true;
      }
      
      if (desc.getDir() == lwc::AD_IN) {
        if (!dontDispose) {
          Lua2C<T>::DisposeValue(val);
        }
        
      } else {
        if (desc.arrayArg() >= 0) {
          arraySizes[size_t(desc.arrayArg())] = size_t(val);
          if (!dontDispose) {
            Lua2C<T>::DisposeValue(val);
          }
          
        } else {
          lua_pushinteger(L, lua_objlen(L, rv)+1);
          C2Lua<T>::ToValue(val, L);
          if (!dontDispose) {
            Lua2C<T>::DisposeValue(val);
          }
          lua_settable(L, rv);
        }
      }
    }
    
    static bool PreCallArray(const lwc::Argument &desc, size_t idesc, const lwc::Argument &sdesc,
                             lua_State *L, int firstArg, size_t nargs, int kwargs, size_t &iarg,
                             std::map<size_t,size_t> &arraySizes, Array &ary,
                             std::string &err) {
      if (desc.getDir() == lwc::AD_IN || desc.getDir() == lwc::AD_INOUT) {
        size_t length = 0;
        if (iarg >= nargs) {
          bool failed = true;
          if (desc.isNamed() && kwargs >= 0) {
            lua_pushstring(L, desc.getName().c_str());
            lua_gettable(L, firstArg+kwargs);
            if (!lua_isnil(L, -1)) {
              Lua2C<T>::ToArray(L, lua_gettop(L), ary, length);
              arraySizes[idesc] = length;
              failed = false;
              
            } else {
              failed = !GetDefaultValue(desc, ary);
              if (!failed) {
                lwc::Integer tmp;
                failed = !GetDefaultValue(sdesc, tmp);
                if (!failed) {
                  length = (size_t) tmp;
                }
              }
            }
            lua_pop(L, 1);
          }
          if (failed) {
            err = "Not enough arguments";
            return false;
          }
        } else {
          Lua2C<T>::ToArray(L, firstArg+iarg, ary, length);
          arraySizes[idesc] = length;
          ++iarg;
        }
      }
      return true;
    }
    
    static void PostCallArray(const lwc::Argument &desc, size_t idesc, const lwc::Argument &,
                              lua_State *L, int firstArg, size_t nargs, int kwargs, size_t &iarg,
                              std::map<size_t,size_t> &arraySizes, Array &ary, int rv) {
      
      bool dontDispose = false;
      if (iarg >= nargs && desc.isNamed() && 
          (kwargs < 0 || HasKey(L, firstArg+kwargs, desc.getName())) &&
          desc.hasDefaultValue()) {
        dontDispose = true;
      }
      
      if (desc.getDir() == lwc::AD_IN) {
        if (!dontDispose) {
          Lua2C<T>::DisposeArray(ary, arraySizes[idesc]);
        }
        
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
          if (!dontDispose) {
            Lua2C<T>::DisposeArray(ary, arraySizes[idesc]);
          }
          // add to output
          lua_pushinteger(L, lua_objlen(L, rv)+1);
          lua_pushvalue(L, -2); // the returned array was pushed on top
          lua_settable(L, rv);
          // also pop return value?
          
        } else {
          if (!dontDispose) {
            Lua2C<T>::DisposeArray(ary, arraySizes[idesc]);
          }
        }
        
        // finally pop the return value of C2Lua<>::ToArray
        lua_pop(L, 1);
      }
    }
  };
  
  
}

#endif



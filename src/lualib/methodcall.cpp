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

#include <lwc/lua/methodcall.h>
#include <lwc/lua/convert.h>

namespace lua {

int CallMethod(lwc::Object *o, const char *n,
               lwc::MethodParams &params, int cArg,
               lua_State *L, int firstArg, size_t nargs, int kwargs, size_t luaArg,
               std::map<size_t,size_t> &arraySizes) throw(std::runtime_error) {
  
  // are INOUT dealt properly? [see convert.h]
  
  const lwc::Method &m = params.getMethod();
  
  if (m.numArgs() == size_t(cArg)) {
    
    if (luaArg != nargs) {
      lua_pushstring(L, "Invalid arguments. None expected");
      lua_error(L);
      return BAD_METHOD_CALL;
    }
    
    o->call(n, params);
    
    if (m.numArgs() == 0) {
      return NO_RETVAL;
      
    } else {
      lua_newtable(L);
      return lua_gettop(L);
    }
    
  } else {
    int rv;
    
    const lwc::Argument &ad = m[cArg];
    
    if (!ad.isArray() && ad.getDir() == lwc::AD_INOUT) {
      lua_pushstring(L, "inout non array arguments not supported in ruby");
      lua_error(L);
      return BAD_METHOD_CALL;
    }
    
    bool failed = false;
    std::string err = "";
    size_t oldLuaArg = luaArg;
    
    switch (ad.getType()) {
      case lwc::AT_BOOL: {
        if (ad.isArray()) {
          bool *ary=0;
          const lwc::Argument &sad = m[ad.arraySizeArg()];
          if (ParamConverter<lwc::AT_BOOL>::PreCallArray(ad, cArg, sad, L, firstArg, nargs, kwargs, luaArg, arraySizes, ary, err)) {
            try {
              if (ad.getDir() == lwc::AD_IN) {
                params.set(cArg, ary, false);
              } else {
                params.set(cArg, &ary, false);
              }
            } catch (std::exception &e) {
              err = e.what();
              failed = true;
            }
            if (!failed) {
              rv = CallMethod(o, n, params, cArg+1, L, firstArg, nargs, kwargs, luaArg, arraySizes);
            }
            ParamConverter<lwc::AT_BOOL>::PostCallArray(ad, cArg, sad, L, firstArg, nargs, kwargs, oldLuaArg, arraySizes, ary, rv);
            
          } else {
            failed = true;
          }
          
        } else {
          bool val;
          if (ParamConverter<lwc::AT_BOOL>::PreCall(ad, cArg, L, firstArg, nargs, kwargs, luaArg, arraySizes, val, err)) {
            try {
              if (ad.getDir() == lwc::AD_IN) {
                params.set(cArg, val, false);
              } else {
                params.set(cArg, &val, false);
              }
            } catch (std::exception &e) {
              err = e.what();
              failed = true;
            }
            if (!failed) {
              rv = CallMethod(o, n, params, cArg+1, L, firstArg, nargs, kwargs, luaArg, arraySizes);
            }
            ParamConverter<lwc::AT_BOOL>::PostCall(ad, cArg, L, firstArg, nargs, kwargs, oldLuaArg, arraySizes, val, rv);
          
          } else {
            failed = true;
          }
        }
        break;
      }
      case lwc::AT_INT: {
        if (ad.isArray()) {
          lwc::Integer *ary=0;
          const lwc::Argument &sad = m[ad.arraySizeArg()];
          if (ParamConverter<lwc::AT_INT>::PreCallArray(ad, cArg, sad, L, firstArg, nargs, kwargs, luaArg, arraySizes, ary, err)) {
            try {
              if (ad.getDir() == lwc::AD_IN) {
                params.set(cArg, ary, false);
              } else {
                params.set(cArg, &ary, false);
              }
            } catch (std::exception &e) {
              err = e.what();
              failed = true;
            }
            if (!failed) {
              rv = CallMethod(o, n, params, cArg+1, L, firstArg, nargs, kwargs, luaArg, arraySizes);
            }
            ParamConverter<lwc::AT_INT>::PostCallArray(ad, cArg, sad, L, firstArg, nargs, kwargs, oldLuaArg, arraySizes, ary, rv);
            
          } else {
            failed = true;
          }
          
        } else {
          lwc::Integer val;
          if (ParamConverter<lwc::AT_INT>::PreCall(ad, cArg, L, firstArg, nargs, kwargs, luaArg, arraySizes, val, err)) {
            try {
              if (ad.getDir() == lwc::AD_IN) {
                params.set(cArg, val, false);
              } else {
                params.set(cArg, &val, false);
              }
            } catch (std::exception &e) {
              err = e.what();
              failed = true;
            }
            if (!failed) {
              rv = CallMethod(o, n, params, cArg+1, L, firstArg, nargs, kwargs, luaArg, arraySizes);
            }
            ParamConverter<lwc::AT_INT>::PostCall(ad, cArg, L, firstArg, nargs, kwargs, oldLuaArg, arraySizes, val, rv);
            
          } else {
            failed = true;
          }
        }
        break;
      }
      case lwc::AT_REAL: {
        if (ad.isArray()) {
          lwc::Real *ary=0;
          const lwc::Argument &sad = m[ad.arraySizeArg()];
          if (ParamConverter<lwc::AT_REAL>::PreCallArray(ad, cArg, sad, L, firstArg, nargs, kwargs, luaArg, arraySizes, ary, err)) {
            try {
              if (ad.getDir() == lwc::AD_IN) {
                params.set(cArg, ary, false);
              } else {
                params.set(cArg, &ary, false);
              }
            } catch (std::exception &e) {
              err = e.what();
              failed = true;
            }
            if (!failed) {
              rv = CallMethod(o, n, params, cArg+1, L, firstArg, nargs, kwargs, luaArg, arraySizes);
            }
            ParamConverter<lwc::AT_REAL>::PostCallArray(ad, cArg, sad, L, firstArg, nargs, kwargs, oldLuaArg, arraySizes, ary, rv);
            
          } else {
            failed = true;
          }
          
        } else {
          lwc::Real val;
          if (ParamConverter<lwc::AT_REAL>::PreCall(ad, cArg, L, firstArg, nargs, kwargs, luaArg, arraySizes, val, err)) {
            try {
              if (ad.getDir() == lwc::AD_IN) {
                params.set(cArg, val, false);
              } else {
                params.set(cArg, &val, false);
              }
            } catch (std::exception &e) {
              err = e.what();
              failed = true;
            }
            if (!failed) {
              rv = CallMethod(o, n, params, cArg+1, L, firstArg, nargs, kwargs, luaArg, arraySizes);
            }
            ParamConverter<lwc::AT_REAL>::PostCall(ad, cArg, L, firstArg, nargs, kwargs, oldLuaArg, arraySizes, val, rv);
            
          } else {
            failed = true;
          }
        }
        break;
      }
      case lwc::AT_STRING: {
        if (ad.isArray()) {
          char **ary=0;
          const lwc::Argument &sad = m[ad.arraySizeArg()];
          if (ParamConverter<lwc::AT_STRING>::PreCallArray(ad, cArg, sad, L, firstArg, nargs, kwargs, luaArg, arraySizes, ary, err)) {
            try {
              if (ad.getDir() == lwc::AD_IN) {
                params.set(cArg, ary, false);
              } else {
                params.set(cArg, &ary, false);
              }
            } catch (std::exception &e) {
              err = e.what();
              failed = true;
            }
            if (!failed) {
              rv = CallMethod(o, n, params, cArg+1, L, firstArg, nargs, kwargs, luaArg, arraySizes);
            }
            ParamConverter<lwc::AT_STRING>::PostCallArray(ad, cArg, sad, L, firstArg, nargs, kwargs, oldLuaArg, arraySizes, ary, rv);
            
          } else {
            failed = true;
          }
          
        } else {
          char *val;
          if (ParamConverter<lwc::AT_STRING>::PreCall(ad, cArg, L, firstArg, nargs, kwargs, luaArg, arraySizes, val, err)) {
            try {
              if (ad.getDir() == lwc::AD_IN) {
                params.set(cArg, val, false);
              } else {
                params.set(cArg, &val, false);
              }
            } catch (std::exception &e) {
              err = e.what();
              failed = true;
            }
            if (!failed) {
              rv = CallMethod(o, n, params, cArg+1, L, firstArg, nargs, kwargs, luaArg, arraySizes);
            }
            ParamConverter<lwc::AT_STRING>::PostCall(ad, cArg, L, firstArg, nargs, kwargs, oldLuaArg, arraySizes, val, rv);
            
          } else {
            failed = true;
          }
        }
        break;
      }
      case lwc::AT_OBJECT: {
        if (ad.isArray()) {
          lwc::Object **ary=0;
          const lwc::Argument &sad = m[ad.arraySizeArg()];
          if (ParamConverter<lwc::AT_OBJECT>::PreCallArray(ad, cArg, sad, L, firstArg, nargs, kwargs, luaArg, arraySizes, ary, err)) {
            try {
              if (ad.getDir() == lwc::AD_IN) {
                params.set(cArg, ary, false);
              } else {
                params.set(cArg, &ary, false);
              }
            } catch (std::exception &e) {
              err = e.what();
              failed = true;
            }
            if (!failed) {
              rv = CallMethod(o, n, params, cArg+1, L, firstArg, nargs, kwargs, luaArg, arraySizes);
            }
            ParamConverter<lwc::AT_OBJECT>::PostCallArray(ad, cArg, sad, L, firstArg, nargs, kwargs, oldLuaArg, arraySizes, ary, rv);
            
          } else {
            failed = true;
          }
          
        } else {
          lwc::Object *val;
          if (ParamConverter<lwc::AT_OBJECT>::PreCall(ad, cArg, L, firstArg, nargs, kwargs, luaArg, arraySizes, val, err)) {
            try {
              if (ad.getDir() == lwc::AD_IN) {
                params.set(cArg, val, false);
              } else {
                params.set(cArg, &val, false);
              }
            } catch (std::exception &e) {
              err = e.what();
              failed = true;
            }
            if (!failed) {
              rv = CallMethod(o, n, params, cArg+1, L, firstArg, nargs, kwargs, luaArg, arraySizes);
            }
            ParamConverter<lwc::AT_OBJECT>::PostCall(ad, cArg, L, firstArg, nargs, kwargs, oldLuaArg, arraySizes, val, rv);
            
          } else {
            failed = true;
          }
        }
        break;
      }
      default:
        lua_pushstring(L, "Invalid argument type");
        lua_error(L);
        return BAD_METHOD_CALL;
    }
    
    if (failed) {
      lua_pushstring(L, err.c_str());
      lua_error(L);
      return BAD_METHOD_CALL;
    }
    
    if (cArg == 0) {
      // LUA does not auto-expand array value, do that by ourselves
      // and return number of value
      if (rv != NO_RETVAL) {
        if (rv != lua_gettop(L)) {
          std::cout << "*** lcwlua: CallMethod -> return value should be on top of the stack..." << std::endl;
        }
        size_t sz = lua_objlen(L, rv);
        for (size_t i=sz; i>0; --i) {
          lua_pushinteger(L, i);
          lua_gettable(L, rv);
        }
        lua_remove(L, rv);
        return sz;
        
      } else {
        
        return 0;
      }
    }
    
    return rv;
  }
}

}

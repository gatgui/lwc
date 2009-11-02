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

#include <lwc/lua/lobject.h>
#include <lwc/lua/types.h>
#include <lwc/lua/convert.h>

namespace lua {

// NOTE: How to keep reference to the corresponding lua object
//       => go through the registry

Object::Object(lua_State *L, int inst)
  : mState(L) {
  LuaObject::Wrap(L, this);
  lua_setfield(L, inst, "lwcobj");
  lua_pushlightuserdata(L, (void*)this);
  lua_pushvalue(L, inst);
  lua_settable(L, LUA_REGISTRYINDEX);
}

Object::~Object() {
  lua_pushlightuserdata(mState, (void*)this);
  lua_gettable(mState, LUA_REGISTRYINDEX);
  lua_getfield(mState, -1, "lwcobj");
  LuaObject *lo = (LuaObject*) lua_touserdata(mState, -1);
  lo->obj = 0;
  lua_pop(mState, 2);
  // set registry to null so lua object can be garbage collected
  lua_pushlightuserdata(mState, (void*)this);
  lua_pushnil(mState);
  lua_settable(mState, LUA_REGISTRYINDEX);
}

int Object::self() {
  lua_pushlightuserdata(mState, (void*)this);
  lua_gettable(mState, LUA_REGISTRYINDEX);
  lua_getfield(mState, -1, "lwcobj");
  lua_remove(mState, -2);
  return lua_gettop(mState);
}
      
void Object::call(const char *name, lwc::MethodParams &params) throw(std::runtime_error) {
  
  //std::cout << "lua::Object::call(\"" << name << "\")" << std::endl;
  
  int oldtop = lua_gettop(mState);
  //int mSelf = self();
  lua_pushlightuserdata(mState, (void*)this);
  lua_gettable(mState, LUA_REGISTRYINDEX);
  int mSelf = lua_gettop(mState);
  
  if (lua_isnil(mState, mSelf)) {
    throw std::runtime_error("Underlying lua object does not exist");
  }
  
  lua_getmetatable(mState, mSelf);
  lua_getfield(mState, -1, name);
  if (lua_isnil(mState, -1)) {
    std::ostringstream oss;
    oss << "Method missing \"" << name << "\"";
    throw std::runtime_error(oss.str());
  }
  // remove metatable from stack
  lua_remove(mState, -2);
  // save inout insert point
  int inoutPos = lua_gettop(mState);
  // push self as first argument
  lua_pushvalue(mState, -2);
  
  const lwc::Method &meth = params.getMethod();

  size_t ninputs = 0;
  size_t noutputs = 0;
  
  for (size_t i=0; i<meth.numArgs(); ++i) {
    const lwc::Argument &arg = meth[i];
    if (arg.getDir() == lwc::AD_INOUT && !arg.isArray()) {
      throw std::runtime_error("Ruby does not support non-array inout arguments");
    }
    if (arg.arrayArg() < 0) {
      if (arg.getDir() == lwc::AD_IN || arg.getDir() == lwc::AD_INOUT) {
        ++ninputs;
      } else {
        ++noutputs;
      }
    }
  }
  
  std::map<size_t, size_t> inoutArgs;
  std::map<size_t, size_t> inoutInSizes;
  
  size_t cur = 0;
  
  for (size_t i=0; i<meth.numArgs(); ++i) {
    const lwc::Argument &arg = meth[i];
        
    if (arg.arrayArg() >= 0) {
      continue;
    }
    
    if (arg.isArray()) {
      if (arg.getDir() != lwc::AD_IN && arg.getDir() != lwc::AD_INOUT) {
        continue;
      }
      
      bool out = (arg.getDir() == lwc::AD_INOUT);
      
      //if (out) {
      //  //inoutArgs[i] = cur;
      //  inoutArgs[i] = lua_gettop(mState);
      //}
      
      size_t len = 0;
      
      const lwc::Argument &lenarg = meth[size_t(arg.arraySizeArg())];
      
      switch(lenarg.getType()) {
        case lwc::AT_INT: {
          if (!out) {
            lwc::Integer sz;
            params.get(size_t(arg.arraySizeArg()), sz);
            len = size_t(sz);
          } else {
            lwc::Integer *sz;
            params.get(size_t(arg.arraySizeArg()), sz);
            len = size_t(*sz);
          }
          break;
        }
        default:
          throw std::runtime_error("Invalid array size argument type");
      }
      
      if (out) {
        inoutInSizes[i] = len;
      }
      
      switch(arg.getType()) {
        case lwc::AT_BOOL: {
          if (!out) {
            bool *ary;
            params.get(i, ary);
            C2Lua<lwc::AT_BOOL>::ToArray(ary, len, mState);
            
          } else {
            bool **ary;
            params.get(i, ary);
            C2Lua<lwc::AT_BOOL>::ToArray(*ary, len, mState);
          }
          break;
        }
        case lwc::AT_INT: {
          if (!out) {
            lwc::Integer *ary;
            params.get(i, ary);
            C2Lua<lwc::AT_INT>::ToArray(ary, len, mState);
          } else {
            lwc::Integer **ary;
            params.get(i, ary);
            C2Lua<lwc::AT_INT>::ToArray(*ary, len, mState);
          }
          break;
        }
        case lwc::AT_REAL: {
          if (!out) {
            lwc::Real *ary;
            params.get(i, ary);
            C2Lua<lwc::AT_REAL>::ToArray(ary, len, mState);
          } else {
            lwc::Real **ary;
            params.get(i, ary);
            C2Lua<lwc::AT_REAL>::ToArray(*ary, len, mState);
          }
          break;
        }
        case lwc::AT_STRING: {
          if (!out) {
            char **ary;
            params.get(i, ary);
            C2Lua<lwc::AT_STRING>::ToArray(ary, len, mState);
          } else {
            char ***ary;
            params.get(i, ary);
            C2Lua<lwc::AT_STRING>::ToArray(*ary, len, mState);
          }
          break;
        }
        case lwc::AT_OBJECT: {
          if (!out) {
            lwc::Object **ary;
            params.get(i, ary);
            C2Lua<lwc::AT_OBJECT>::ToArray(ary, len, mState);
          } else {
            lwc::Object ***ary;
            params.get(i, ary);
            C2Lua<lwc::AT_OBJECT>::ToArray(*ary, len, mState);
          }
          break;
        }
        default:
          throw std::runtime_error("Invalid array argument type");
      }
      
      if (out) {
        lua_insert(mState, inoutPos);
        lua_pushvalue(mState, inoutPos);
        ++inoutPos;
      }
        
    } else {
      if (arg.getDir() != lwc::AD_IN) {
        continue;
      }
      
      switch(arg.getType()) {
        case lwc::AT_BOOL: {
          bool val;
          params.get(i, val);
          C2Lua<lwc::AT_BOOL>::ToValue(val, mState);
          break;
        }
        case lwc::AT_INT: {
          lwc::Integer val;
          params.get(i, val);
          C2Lua<lwc::AT_INT>::ToValue(val, mState);
          break;
        }
        case lwc::AT_REAL: {
          lwc::Real val;
          params.get(i, val);
          C2Lua<lwc::AT_REAL>::ToValue(val, mState);
          break;
        }
        case lwc::AT_STRING: {
          char *val;
          params.get(i, val);
          C2Lua<lwc::AT_STRING>::ToValue(val, mState);
          break;
        }
        case lwc::AT_OBJECT: {
          lwc::Object *val;
          params.get(i, val);
          C2Lua<lwc::AT_OBJECT>::ToValue(val, mState);
          break;
        }
        default:
          throw std::runtime_error("Invalid argument type");
      }
    }
    
    //args[cur] = rarg;
    //++cur;
  }
  
  // ninputs + 1 because we always add mSelf as first arg
  //lua_call(mState, ninputs+1, noutputs);
  // instead of 0, giving the index of an error function would enable printing of more precise information
  // (need to investigate that)
  int err = lua_pcall(mState, ninputs+1, noutputs, 0);
  if (err != 0) {
    // error string is at the top of the stack
    std::ostringstream oss;
    std::string lines = lua_tostring(mState, -1);
    size_t p0 = 0, p1 = lines.find('\n', p0);
    while (p1 != std::string::npos) {
      std::string line = lines.substr(p0, p1-p0);
      oss << std::endl << "--- Lua --- " << line;
      p0 = p1 + 1;
      p1 = lines.find('\n', p0);
    }
    oss << std::endl;
    throw std::runtime_error(oss.str().c_str());
  }
  
  // convert Lua outputs to C
  
  // params.get(idx, var) is not quite the same as: params.set(idx, var)
  // beware, the last is called by the caller to set the address of an output var
  // that will receive the value.
  int retPos = lua_gettop(mState) - noutputs + 1;
  
  cur = 0;
  //bool array = (noutputs > 0 ? lua_istable(mState, -1) : false);
  
  for (size_t i=0; i<meth.numArgs(); ++i) {
    
    const lwc::Argument &arg = meth[i];
    
    if (arg.getDir() == lwc::AD_IN) {
      continue;
    }
    
    if (arg.arrayArg() >= 0) {
      // array sizes handled when dealing with the array arg
      continue;
    }
    
    if (noutputs == 0) {
      throw std::runtime_error("Expected return value, got None");
    }
    
    //if (cur > 0 && !array) {
    if (cur > noutputs) {
      throw std::runtime_error("Not enough return value(s) on stack");
    }
    
    // can be inout, out or return
    
    if (arg.isArray()) {
      
      size_t len = 0;
      
      int crv;
      
      if (arg.getDir() == lwc::AD_INOUT) {
        
        crv = inoutArgs[i];
        len = inoutInSizes[i];
        
      } else {
        
        crv = retPos + cur;
        if (lua_isnil(mState, crv)) {
          throw std::runtime_error("Expected return value, got nil");
        }
        ++cur;
      }
      
      switch(arg.getType()) {
        case lwc::AT_BOOL: {
          bool **ary;
          params.get(i, ary);
          Lua2C<lwc::AT_BOOL>::ToArray(mState, crv, *ary, len);
          break;
        }
        
        case lwc::AT_INT: {
          lwc::Integer **ary;
          params.get(i, ary);
          Lua2C<lwc::AT_INT>::ToArray(mState, crv, *ary, len);
          break;
        }
        case lwc::AT_REAL: {
          lwc::Real **ary;
          params.get(i, ary);
          Lua2C<lwc::AT_REAL>::ToArray(mState, crv, *ary, len);
          break;
        }
        case lwc::AT_STRING: {
          char ***ary;
          params.get(i, ary);
          Lua2C<lwc::AT_STRING>::ToArray(mState, crv, *ary, len);
          break;
        }
        case lwc::AT_OBJECT: {
          lwc::Object ***ary;
          params.get(i, ary);
          Lua2C<lwc::AT_OBJECT>::ToArray(mState, crv, *ary, len);
          break;
        }
        default:
          throw std::runtime_error("Invalid argument type");
      }
      
      // set length argument here
      const lwc::Argument &lenarg = meth[size_t(arg.arraySizeArg())];
      
      switch(lenarg.getType()) {
        case lwc::AT_INT: {
          lwc::Integer *sz;
          params.get(size_t(arg.arraySizeArg()), sz);
          *sz = (lwc::Integer) len;
          break;
        }
        default:
          throw std::runtime_error("Invalid array size type");
      }
      
    } else {
      
      int crv = retPos + cur;
      if (lua_isnil(mState, crv) && arg.getType() != lwc::AT_STRING && arg.getType() != lwc::AT_OBJECT) {
        throw std::runtime_error("Expected return value, got nil");
      }
      ++cur;
      
      switch(arg.getType()) {
        case lwc::AT_BOOL: {
          bool *val;
          params.get(i, val);
          Lua2C<lwc::AT_BOOL>::ToValue(mState, crv, *val);
          break;
        }
        case lwc::AT_INT: {
          lwc::Integer *val;
          params.get(i, val);
          Lua2C<lwc::AT_INT>::ToValue(mState, crv, *val);
          break;
        }
        case lwc::AT_REAL: {
          lwc::Real *val;
          params.get(i, val);
          Lua2C<lwc::AT_REAL>::ToValue(mState, crv, *val);
          break;
        }
        case lwc::AT_STRING: {
          char **val;
          params.get(i, val);
          Lua2C<lwc::AT_STRING>::ToValue(mState, crv, *val);
          break;
        }
        case lwc::AT_OBJECT: {
          lwc::Object **val;
          params.get(i, val);
          Lua2C<lwc::AT_OBJECT>::ToValue(mState, crv, *val);
          break;
        }
        default:
          throw std::runtime_error("Invalid argument type");
      }
    }
  }
  
  lua_settop(mState, oldtop);
}

}


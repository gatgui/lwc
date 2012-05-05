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

#include <lwc/lua/lobject.h>
#include <lwc/lua/types.h>
#include <lwc/loader.h>
#include <lwc/factory.h>

/* Automatically defined metatable __index:

Klass.__index == function (self, member)
  val = Klass[member]
  if val == nil then
    if self.lwcobj ~= nil then
      val = self.lwcobj[member]
    end
  end
  return val
end
*/

static int Klass_index(lua_State *L)
{
  int key  = lua_gettop(L);
  int self = key - 1;
  
  lua_pushvalue(L, key);
  lua_gettable(L, lua_upvalueindex(1));
  
  if (lua_isnil(L, -1)) {
    
    lua_pushstring(L, "lwcobj");
    lua_gettable(L, self);
    
    int lwcobj = lua_gettop(L);
    
    if (lua_isnil(L, -1)) {
      lua_pop(L, 1);
      
    } else {
      lua_pushvalue(L, key);
      lua_gettable(L, lwcobj);
    }
  }
  
  // Documentation says lua with do the cleanup, just have to say how
  // many values we return
  
  return 1;
}

class LuaFactory : public lwc::Factory {
  public:
  
    LuaFactory(lua_State *L)
      : mState(L) {
    }
    
    virtual ~LuaFactory() {
      std::map<std::string, TypeEntry>::iterator it = mTypes.begin();
      while (it != mTypes.end()) {
        delete it->second.methods;
        ++it;
      }
    }
    
    virtual const lwc::MethodsTable* getMethods(const char *typeName) {
      std::map<std::string, TypeEntry>::iterator it = mTypes.find(typeName);
      if (it == mTypes.end()) {
        return 0;
      } else {
        return it->second.methods;
      }
    }
    
    virtual lwc::Object* create(const char *typeName) {
      std::map<std::string, TypeEntry>::iterator it = mTypes.find(typeName);
      if (it != mTypes.end()) {
        int oldtop = lua_gettop(mState);
        lua::Object *obj = 0;
        lua_getfield(mState, LUA_REGISTRYINDEX, typeName);
        lua_getfield(mState, -1, "new");
        lua_call(mState, 0, 1);
        obj = new lua::Object(mState, lua_gettop(mState));
        // clean stack
        lua_settop(mState, oldtop);
        // done!
        return obj;
        
      } else {
        return 0;
      }
    }
    
    virtual void destroy(lwc::Object *o) {
      if (o) {
        delete o;
      }
    }
    
    virtual bool isSingleton(const char *typeName) {
      std::map<std::string, TypeEntry>::iterator it = mTypes.find(typeName);
      if (it != mTypes.end()) {
        return it->second.singleton;
      } else {
        return false;
      }
    }
    
    //bool addType(const char *modulename, const char *name, int klass) {
    bool addType(const char *name, int klass) {
      
      int oldtop = lua_gettop(mState);
      //std::cout << "LuaFactory::addType [top = " << oldtop << "]" << std::endl;
      
      bool singleton = false;
      lwc::MethodsTable *typeMethods = 0;
      
      lua_getfield(mState, klass, "Singleton");
      if (!lua_isnil(mState, -1)) {
        if (lua_isboolean(mState, -1)) {
          singleton = (lua_toboolean(mState, -1) == 1);
        }
      }
      
      // need to get Methods for the object etcs
      lua_getfield(mState, klass, "Methods");
      if (lua_isnil(mState, -1)) {
        std::cout << "lualoader: Class has no \"Methods\" member" << std::endl;
        lua_settop(mState, oldtop);
        return false;
      }
      
      int tmethods = lua_gettop(mState);
      
      //std::cout << "tmethods @ " << tmethods << std::endl;
      
      // for each: note, to iterate on next, need to leave the previous key on stack
      // lua_next will always pop it
      lua_pushnil(mState);
      
      while (lua_next(mState, tmethods) != 0) {
        
        if (!lua_isstring(mState, -2)) {
          lua_pop(mState, 1);
          continue;
        }
        
        const char *mn = lua_tostring(mState, -2);
        
        if (!lua_istable(mState, -1)) {
          std::cout << "lualoader: Expected table values. Skip method \"" << mn << "\" for type \"" << name << "\"" << std::endl;
          lua_pop(mState, 1);
          continue;
        }
        
        //std::cout << "Process method \"" << mn << "\"" << std::endl;
        //std::cout << "[top = " << lua_gettop(mState) << "]" << std::endl;
        
        bool add = true;
        lwc::Method meth;
        size_t nargs = lua_objlen(mState, -1);
        //std::cout << "  " << nargs << " arguments" << std::endl;
        
        // at this point we have stack +2
        
        for (size_t i=0; i<nargs; ++i) {
          // process args
          //std::cout << "  process " << i << std::endl;
          
          lua_pushinteger(mState, i+1);
          lua_gettable(mState, -2); // stack +3
          
          if (!lua_istable(mState, -1)) {
            std::cout << "lualoader: Arguments must be arrays" << std::endl;
            lua_pop(mState, 1); // stack +2
            add = false;
            break;
          }
          
          size_t alen = lua_objlen(mState, -1);
          if (alen < 2 || alen > 3) {
            std::cout << "lualoader: Arguments must be arrays with 2 to 3 elements" << std::endl;
            lua_pop(mState, 1); // stack +2
            add = false;
            break;
          }
          
          lwc::Argument a;
          
          lua_pushinteger(mState, 1);
          lua_gettable(mState, -2); // stack +4
          if (!lua_isnumber(mState, -1)) {
            std::cout << "lualoader: Argument array first element must be an integer" << std::endl;
            lua_pop(mState, 2); //stack +2
            add = false;
            break;
          }
          a.setDir(lwc::Direction(lua_tointeger(mState, -1)));
          lua_pop(mState, 1); // stack +3
          
          lua_pushinteger(mState, 2);
          lua_gettable(mState, -2); // stack +4
          if (!lua_isnumber(mState, -1)) {
            std::cout << "lualoader: Argument array second element must be an integer" << std::endl;
            lua_pop(mState, 2); // stack +2
            add = false;
            break;
          }
          a.setType(lwc::Type(lua_tointeger(mState, -1)));
          lua_pop(mState, 1); // stack +3
          
          //std::cout << "  for now arg = " << a.toString() << std::endl;
          
          if (alen >= 3) {
            lua_pushinteger(mState, 3);
            lua_gettable(mState, -2); // stack +4
            if (!lua_isnumber(mState, -1)) {
              std::cout << "lualoader: Argument array 3th element must be an integer" << std::endl;
              lua_pop(mState, 2); // stack +2
              add = false;
              break;
            }
            a.setArraySizeArg(lua_tointeger(mState, -1));
            lua_pop(mState, 1); // stack +3
          }
          
          lua_pop(mState, 1); // stack +2
          
          //std::cout << "  final arg = " << a.toString() << std::endl;
          //std::cout << "  [top = " << lua_gettop(mState) << "]" << std::endl;
          
          meth.addArg(a);
        }
        
        //std::cout << "  method = " << mn << meth.toString() << std::endl;
        
        if (!add) {
          std::cout << "lualoader: Skipped method \"" << mn << "\" for type \"" << name << "\"" << std::endl;
        
        } else {
          if (!typeMethods) {
            // if the class is inherited it has a metatable
            if (lua_getmetatable(mState, klass) != 0) {
              lua_pushstring(mState, "__index");
              lua_rawget(mState, -2);
              std::map<std::string, TypeEntry>::iterator it = mTypes.begin();
              while (it != mTypes.end()) {
                lua_getfield(mState, LUA_REGISTRYINDEX, it->first.c_str());
                if (lua_equal(mState, -1, -2) == 1) {
                  lua_pop(mState, 1);
                  typeMethods = new lwc::MethodsTable(it->second.methods);
                  break;
                }
                lua_pop(mState, 1);
                ++it;
              }
              lua_pop(mState, 2); // metatable and its "__index" table
              if (!typeMethods) {
                std::cout << "lualoader: Skipped type \"" << name << "\" (Super class not registered yet)" << std::endl;
                lua_settop(mState, oldtop);
                return false;
              }
            } else {
              typeMethods = new lwc::MethodsTable();
            }
          }
          try {
            typeMethods->addMethod(mn, meth);
            
          } catch (std::runtime_error &e) {
            std::cout << "lualoader: Skipped method \"" << mn << "\" for type \"" << name << "\"" << std::endl;
            std::cout << "  " << e.what() << std::endl;
          }
        }
        
        // pop the value only [suppose we still have stack +2 at this point]
        
        lua_pop(mState, 1);
      }
      
      if (typeMethods) {
        if (typeMethods->numMethods() == 0) {
          std::cout << "lualoader: MethodsTable is empty for type \"" << name << "\"" << std::endl;
          delete typeMethods;
          lua_settop(mState, oldtop);
          return false;
          
        } else {
          TypeEntry te;
          //te.metatable = name;
          //te.module = modulename;
          te.methods = typeMethods;
          te.singleton = singleton;
          mTypes[name] = te;
          lua_pushvalue(mState, klass);
          
          // set __index function of the class so it can be used as metatable for the instance
          lua_pushstring(mState, "__index");
          lua_pushvalue(mState, klass);
          lua_pushcclosure(mState, Klass_index, 1);
          lua_settable(mState, klass);
          
          lua_setfield(mState, LUA_REGISTRYINDEX, name);
          lua_settop(mState, oldtop);
          return true;
        }
      } else {
        std::cout << "lualoader: MethodsTable not allocated for type \"" << name << "\"" << std::endl;
        lua_settop(mState, oldtop);
        return false;
      }
      
    }
    
  protected:
    
    struct TypeEntry {
      //std::string module;
      //std::string metatable;
      lwc::MethodsTable *methods;
      bool singleton;
    };
    
    lua_State *mState;
    std::map<std::string, TypeEntry> mTypes;
};


class LuaLoader : public lwc::Loader {
  public:
    
    LuaLoader(lua_State *L)
      : mState(L) {
      mFactory = new LuaFactory(L);
    }
    
    virtual ~LuaLoader() {
      delete mFactory;
    }
    
    virtual bool canLoad(const gcore::Path &path) {
      return path.checkExtension("lua");
    }
    
    void addSearchPath(const gcore::Path &dirname) {
      gcore::Path modpath = dirname;
      modpath.push("?.lua");
      lua_getfield(mState, LUA_GLOBALSINDEX, "package"); // 1
      lua_getfield(mState, -1, "path"); // 2
      std::string path = lua_tostring(mState, -1);
      //std::cout << "= Add \"" << modpath << "\" to package.path" << std::endl;
      //std::cout << "  package.path = \"" << path << "\"" << std::endl;
      size_t p0 = 0;
      size_t p1 = path.find(';', p0);
      while (p1 != std::string::npos) {
        gcore::Path curpath(path.substr(p0, p1-p0));
        if (curpath == modpath) {
          return;
        }
        p0 = p1 + 1;
        p1 = path.find(';', p0);
      }
      gcore::Path lastpath(path.substr(p0));
      if (lastpath == modpath) {
        return;
      }
      path += ";";
      path += modpath;
      //std::cout << "  -> \"" << path << "\"" << std::endl;
      lua_pushstring(mState, path.c_str()); // 3
      lua_setfield(mState, -3, "path"); // 2
      lua_pop(mState, 2); // 0
    }
    
    virtual void load(const gcore::Path &path, lwc::Registry *reg) {
      
      int oldtop = lua_gettop(mState);
      
      std::string modulename = path.basename();
      modulename = modulename.substr(0, modulename.length()-4);
      
      gcore::Path dirname = path;
      dirname.pop();
      dirname.makeAbsolute().normalize();
      
      // Add to lua CPATH -> pacakage.path !
      addSearchPath(dirname);
      
      std::string requirestr = "require \"" + modulename + "\"";
      if (luaL_dostring(mState, requirestr.c_str()) != 0) {
        std::cout << "lualoader: Error while loading \"" << path << "\" module";
        if (lua_gettop(mState) > 0 && lua_isstring(mState, -1)) {
          std::cout << ":" << std::endl;
          std::cout << "  " << lua_tostring(mState, -1) << std::endl;
          lua_settop(mState, 0);
          
        } else {
          std::cout << std::endl;
        }
        lua_settop(mState, oldtop);
        return;
      }
      
      lua_getfield(mState, LUA_GLOBALSINDEX, modulename.c_str());
      if (lua_isnil(mState, -1)) {
        std::cout << "lualoader: Module not found in lua globals" << std::endl;
        lua_settop(mState, oldtop);
        return;
      }
      int module = lua_gettop(mState);
      
      lua_getfield(mState, module, "LWC_ModuleGetTypeCount");
      if (lua_isnil(mState, -1)) {
        std::cout << "lualoader: LUA module \"" << path << "\"" << std::endl;
        std::cout << "  is missing LWC_ModuleGetTypeCount function" << std::endl;
        lua_settop(mState, oldtop);
        return;
      }
      int getcount = lua_gettop(mState);
      
      lua_getfield(mState, module, "LWC_ModuleGetTypeName");
      if (lua_isnil(mState, -1)) {
        std::cout << "lualoader: LUA module \"" << path << "\"" << std::endl;
        std::cout << "  is missing LWC_ModuleGetTypeName function" << std::endl;
        lua_settop(mState, oldtop);
        return;
      }
      int gettypename = lua_gettop(mState);
      
      lua_getfield(mState, module, "LWC_ModuleGetTypeClass");
      if (lua_isnil(mState, -1)) {
        std::cout << "lualoader: LUA module \"" << path << "\"" << std::endl;
        std::cout << "  is missing LWC_ModuleGetTypeClass function" << std::endl;
        lua_settop(mState, oldtop);
        return;
      }
      int gettypeclass = lua_gettop(mState);
      
      // package.path = package.path .. ";" .. luapath
      // require "modulename"
      lua_pushvalue(mState, getcount); // popped by lua_call
      lua_call(mState, 0, 1);
      size_t ntypes = lua_tointeger(mState, -1);
      lua_pop(mState, 1);
      //std::cout << ntypes << " in module \"" << path << "\"" << std::endl;
      
      for (size_t i=0; i<ntypes; ++i) {
        lua_pushvalue(mState, gettypename);
        lua_pushinteger(mState, i+1);
        lua_call(mState, 1, 1);
        if (lua_isnil(mState, -1)) {
          lua_pop(mState, 1);
          continue;
        }
        const char *tn = lua_tostring(mState, -1);
        lua_pushvalue(mState, gettypeclass);
        lua_pushinteger(mState, i+1);
        lua_call(mState, 1, 1);
        if (lua_isnil(mState, -1)) {
          lua_pop(mState, 2);
          continue;
        }
        
        //std::cout << "  \"" << tn << "\"" << std::endl;
        
        if (!reg->hasType(tn)) {
          //if (mFactory->addType(modulename.c_str(), tn, lua_gettop(mState))) {
          if (mFactory->addType(tn, lua_gettop(mState))) {
            registerType(tn, mFactory, reg);
          
          } else {
            std::cout << "lualoader: Invalid type \"" << tn << "\"" << std::endl;
          }
        } else {
          std::cout << "lualoader: Type \"" << tn << "\" already registered" << std::endl;
        }
        
        lua_pop(mState, 2);
      }
      
      lua_settop(mState, oldtop);
    }
    
    virtual const char* getName() const {
      return "lualoader";
    }
  
  private:
  
    lua_State *mState;
    LuaFactory *mFactory;
};

// ---

static lua_State *LuaState = 0;
static bool WasInitialized = false;

extern "C" {

#ifdef _WIN32
__declspec(dllexport)
#else
__attribute__ ((visibility ("default")))
#endif
  lwc::Loader* LWC_CreateLoader(const char *hostLang, void *userData) {
    if (!strcmp(hostLang, "lua")) {
      LuaState = (lua_State*)userData;
      WasInitialized = true;
    } else {
      LuaState = luaL_newstate();
      luaL_openlibs(LuaState);
      WasInitialized = false;
    }
    
    lwc::Loader *l = new LuaLoader(LuaState);
    return l;
  }

#ifdef _WIN32
__declspec(dllexport)
#else
__attribute__ ((visibility ("default")))
#endif
  void LWC_DestroyLoader(lwc::Loader *l) {
    if (l) {
      delete l;
    }
    if (!WasInitialized) {
      lua_close(LuaState);
      LuaState = 0;
    }
  }
}




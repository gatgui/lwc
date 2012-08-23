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

#include <lwc/ruby/rbobject.h>
#include <lwc/ruby/types.h>
#undef PATH_SEP
#include <lwc/loader.h>
#include <lwc/factory.h>


struct IndexArgs {
  VALUE mod;
  VALUE idx;
};

static VALUE GetTypeCount(VALUE args) {
  return rb_funcall2(args, rb_intern("LWC_ModuleGetTypeCount"), 0, NULL);
}

static VALUE GetTypeName(VALUE args) {
  IndexArgs *a = (IndexArgs*)args;
  return rb_funcall2(a->mod, rb_intern("LWC_ModuleGetTypeName"), 1, &(a->idx));
}

static VALUE GetTypeClass(VALUE args) {
  IndexArgs *a = (IndexArgs*)args;
  return rb_funcall2(a->mod, rb_intern("LWC_ModuleGetTypeClass"), 1, &(a->idx));
}


enum st_retval {ST_CONTINUE, ST_STOP, ST_DELETE, ST_CHECK};

static int keys_i(VALUE key, VALUE /*value*/, VALUE ary) {
  if (key == Qundef) {
    return ST_CONTINUE;
  }
  rb_ary_push(ary, key);
  return ST_CONTINUE;
}

static VALUE rb_hash_keys(VALUE hash) {
  VALUE ary;
  
  ary = rb_ary_new();
  rb_hash_foreach(hash, (int (*)(...))keys_i, ary);
  
  return ary;
}


class RbFactory : public lwc::Factory {
  public:
  
    RbFactory() {
    }
    
    virtual ~RbFactory() {
      std::map<std::string, TypeEntry>::iterator it = mTypes.begin();
      while (it != mTypes.end()) {
        rb::Tracker::Remove(it->second.methods);
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
        VALUE rbObj = rb_funcall(it->second.klass, rb_intern("new"), 0, NULL);
        rb::Object *obj = new rb::Object(rbObj);
        rb::SetObjectPointer(rbObj, obj);
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
    
    virtual const char* getDescription(const char *typeName) {
      std::map<std::string, TypeEntry>::iterator it = mTypes.find(typeName);
      if (it != mTypes.end()) {
        return it->second.desc.c_str();
      } else {
        return 0;
      }
    }
    
    virtual std::string docString(const char *typeName, const std::string &indent="") {
      std::map<std::string, TypeEntry>::iterator it = mTypes.find(typeName);
      if (it != mTypes.end()) {
        std::ostringstream oss;
        oss << indent << typeName << ":" << std::endl;
        oss << indent << "  " << getDescription(typeName) << std::endl;
        oss << std::endl;
        oss << it->second.methods->docString(indent+"  ") << std::endl;
        return oss.str();
      } else {
        return "";
      }
    }
    
    bool addType(const char *name, VALUE klass) {
      
      bool singleton = false;
      if (rb_const_defined(klass, rb_intern("Singleton"))) {
        VALUE sg = rb_const_get(klass, rb_intern("Singleton"));
        singleton = (TYPE(sg) == T_TRUE);
      }
      
      VALUE methods = rb_const_get(klass, rb_intern("Methods"));
      if (methods == Qnil || TYPE(methods) != T_HASH) {
        std::cout << "rbloader: Missing \"Methods\" class constant, or is not a hash" << std::endl;
        return false;
      }
      
      std::string desc;
      VALUE rdesc = rb_check_string_type(rb_const_get(klass, rb_intern("Description")));
      if (!NIL_P(rdesc)) {
        desc = RSTRING(rdesc)->ptr;
      }
      
      lwc::MethodsTable *typeMethods = 0;
      
      VALUE keys = rb_hash_keys(methods);
      for (long i=0; i<RARRAY(keys)->len; ++i) {
        
        VALUE key = RARRAY(keys)->ptr[i];
        VALUE val = rb_hash_aref(methods, key);
        
        if (TYPE(key) != T_SYMBOL) {
          std::cout << "rbloader: \"Methods\" keys must be symbols" << std::endl;
          continue;
        }
        if (TYPE(val) != T_ARRAY) {
          std::cout << "rbloader: \"Methods\" must be a dict of array" << std::endl;
          continue;
        }
        
        // 1 or 2 items
        
        const char *mname = rb_id2name(SYM2ID(key));
        const char *mdesc = 0;
        
        long n = RARRAY(val)->len;
        if (n < 1 || n > 2) {
          std::cout << "rbloader: \"Methods\" dict value array must have 1 or 2 elements" << std::endl;
          continue;
        }
        
        if (n == 2) {
          // second element must be a string
          VALUE rmdesc = rb_check_string_type(RARRAY(val)->ptr[1]);
          if (NIL_P(rmdesc)) {
            std::cout << "rbloader: \"Methods\" dict value array 2nd element must be a string" << std::endl;
            continue;
          }
          mdesc = RSTRING(rmdesc)->ptr;
        }
        
        val = rb_check_array_type(RARRAY(val)->ptr[0]);
        if (NIL_P(val)) {
          std::cout << "rbloader: \"Methods\" dict value array 1st element must be an array" << std::endl;
          continue;
        }
        n = RARRAY(val)->len;
        
        bool add = true;
        lwc::Method meth;
        
        if (mdesc) {
          meth.setDescription(mdesc);
        }
        
        //std::cout << "Add method: " << name << std::endl;
        for (long j=0; j<n; ++j) {
          
          VALUE arg = RARRAY(val)->ptr[j];
          
          if (TYPE(arg) != T_ARRAY || RARRAY(arg)->len < 2 || RARRAY(arg)->len > 6) {
            std::cout << "rbloader: Arguments must be arrays with 2 to 6 elements" << std::endl;
            add = false;
            break;
          }
          
          lwc::Argument a;
          
          a.setDir(lwc::Direction(NUM2LONG(RARRAY(arg)->ptr[0])));
          a.setType(lwc::Type(NUM2LONG(RARRAY(arg)->ptr[1])));
          
          if (RARRAY(arg)->len >= 3) {
            a.setArraySizeArg(NUM2ULONG(RARRAY(arg)->ptr[2]));
          }
          
          if (RARRAY(arg)->len >= 4) {
            bool hasDef = RARRAY(arg)->ptr[3];
            if (hasDef == Qtrue) {
              if (n < 5) {
                std::cout << "rbloader: missing default value" << std::endl;
                add = false;
                break;
              }
              VALUE rdv = RARRAY(arg)->ptr[4];
              rb::SetArgDefault(a, rdv);
            }
          }
          
          if (RARRAY(arg)->len >= 6) {
            VALUE sname = rb_check_string_type(RARRAY(arg)->ptr[5]);
            if (NIL_P(sname)) {
              std::cout << "rbloader: " << std::endl;
              add = false;
              break;
            }
            a.setName(RSTRING(sname)->ptr);
          }
          
          try {
            meth.addArg(a);
          } catch (std::exception &e) {
            std::cout << "rbloader: " << e.what() << std::endl;
            add = false;
            break;
          }
        }
        
        if (!add) {
          std::cout << "rbloader: Skipped method (1) \"" << mname << "\" for type \"" << name << "\"" << std::endl;
          continue;
        
        } else {
          if (!typeMethods) {
            VALUE super = RCLASS(klass)->super;
            if (super != Qnil && super != rb::cLWCObject) {
              std::map<std::string, TypeEntry>::iterator it = mTypes.begin();
              while (it != mTypes.end()) {
                if (it->second.klass == super) {
                  typeMethods = new lwc::MethodsTable(it->second.methods);
                  break;
                }
                ++it;
              }
              if (!typeMethods) {
                std::cout << "rbloader: Skipped type \"" << name << "\" (Super class not registered yet)" << std::endl;
                return false;
              }
            } else {
              typeMethods = new lwc::MethodsTable();
            }
          }
          try {
            typeMethods->addMethod(mname, meth);
          } catch (std::runtime_error &) {
            std::cout << "rbloader: Skipped method (2) \"" << mname << "\" for type \"" << name << "\"" << std::endl;
            continue;
          }
        }
      }
      
      if (typeMethods) {
        if (typeMethods->numMethods() == 0) {
          std::cout << "rbloader: MethodsTable is empty for type \"" << name << "\"" << std::endl;
          delete typeMethods;
          return false;
        } else {
          TypeEntry te;
          te.klass = klass;
          te.singleton = singleton;
          te.methods = typeMethods;
          te.desc = desc;
          mTypes[name] = te;
          rb::Tracker::Add(typeMethods, te.klass);
          return true;
        }
      } else {
        std::cout << "rbloader: MethodsTable not allocated for type \"" << name << "\"" << std::endl;
        return false;
      }
    }
    
  protected:
    
    struct TypeEntry {
      VALUE klass;
      bool singleton;
      std::string desc;
      lwc::MethodsTable *methods;
    };
    
    std::map<std::string, TypeEntry> mTypes;
};


class RbLoader : public lwc::Loader {
  public:
    
    RbLoader() {
      mFactory = new RbFactory();
    }
    
    virtual ~RbLoader() {
      delete mFactory;
    }
    
    virtual bool canLoad(const gcore::Path &path) {
      return path.checkExtension("rb");
    }
    
    virtual void load(const gcore::Path &path, lwc::Registry *reg) {
      
      std::string modulename = path.basename();
      modulename = modulename.substr(0, modulename.length()-3);
      
      gcore::Path dirname = path;
      dirname.pop();
      dirname.makeAbsolute().normalize();
      
      int st;
      
      //std::cout << "$: << \"" << dirname << "\"" << std::endl;
      rb::Embed::AppendToPath(dirname.fullname('/'));
      
      //std::cout << "require '" << modulename << "'" << std::endl;
      st = rb::Embed::Require(path.basename());
      if (st != 0) {
        rb::Lang::Error(std::cout, st);
        std::cout <<  "rbloader: Failed to load module \"" << path.basename() << "\"" << std::endl;
        return;
      }
      
      if (modulename[0] >= 'a' && modulename[0] <= 'z') {
        modulename[0] = modulename[0] + ('A' - 'a');
      }
      
      //std::cout << "Real module name: \"" << modulename << "\"" << std::endl;
      
      VALUE mod = rb_const_get(rb_cModule, rb_intern(modulename.c_str()));
      if (mod == Qnil) {
        std::cout << "rbloader: Could not get module" << std::endl;
        return;
      }
      
      IndexArgs iargs;
      iargs.mod = mod;
      
      VALUE rn = rb_protect(GetTypeCount, mod, &st);
      if (st != 0) {
        std::cout << "rbloader: Missing LWC_ModuleGetTypeCount function" << std::endl;
        return;
      }
      
      unsigned long n = NUM2ULONG(rn);
      //std::cout << "Module has " << n << " types" << std::endl;
      
      //VALUE arg;
      
      for (unsigned long i=0; i<n; ++i) {
        iargs.idx = ULONG2NUM(i);
        
        VALUE rname = rb_protect(GetTypeName, (VALUE)&iargs, &st);
        if (st != 0) {
          std::cout << "rbloader: Missing LWC_ModuleGetTypeName function" << std::endl;
          return;
        }
        if (rname == Qnil) {
          std::cout << "rbloader: No name for type" << std::endl;
          continue;
        }
        
        //std::cout << "Class " << i << " name: " << RSTRING(rname)->ptr << std::endl;
        
        VALUE rclass = rb_protect(GetTypeClass, (VALUE)&iargs, &st);
        if (st != 0) {
          std::cout << "rbloader: Missing LWC_ModuleGetTypeClass function" << std::endl;
          return;
        }
        if (rclass == Qnil) {
          std::cout << "rbloader: No class object for type" << std::endl;
          continue;
        }
        
        const char *tn = RSTRING(rname)->ptr;
        
        if (!reg->hasType(tn)) {
          if (mFactory->addType(tn, rclass)) {
            registerType(tn, mFactory, reg);
          } else {
            std::cout << "rbloader: Invalid type \"" << tn << "\"" << std::endl;
          }
        } else {
          std::cout << "rbloader: Type \"" << tn << "\" already registered" << std::endl;
        }
        
      }
      
    }
    
    virtual const char* getName() const {
      return "rbloader";
    }
  
  private:
  
    RbFactory *mFactory;
};

// ---

static bool WasInitialized = false;

extern "C" {

#ifdef _WIN32
__declspec(dllexport)
#else
__attribute__ ((visibility ("default")))
#endif
  lwc::Loader* LWC_CreateLoader(const char*, void*) {
    if (rb::Embed::IsInitialized()) {
      WasInitialized = true;
    } else {
      WasInitialized = false;
      rb::Embed::Init(0, NULL, "lwc_ruby");
    }
    lwc::Loader *l = new RbLoader();
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
      rb::Embed::Cleanup();
    }
  }
}




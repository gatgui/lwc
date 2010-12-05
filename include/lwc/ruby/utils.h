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

#ifndef __lwc_rb_utils_h_
#define __lwc_rb_utils_h_

#include <lwc/ruby/config.h>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>

#define RB_INIT_TRACKER \
  VALUE rb::Tracker::m_tmap = 0;\
  ID rb::Tracker::m_delete = 0;\
  bool rb::Tracker::m_init = false;

typedef void (*GC_FUNC)(void*);
typedef VALUE (*RBM)(...);

namespace rb {
  
  class LWCRB_API Tracker {
    public:
      inline static bool Init() {
        if (!m_init) {
          m_tmap = rb_hash_new();
          m_delete = rb_intern("delete");
          rb_gc_register_address(&m_tmap);
          m_init = true;
          return true;
        } else {
          return false;
        }
      }
      inline static void Add(void *ptr, VALUE obj) {
        rb_hash_aset(m_tmap, Ptr2Ref(ptr), obj);
      }
      inline static VALUE InstanceFor(void *ptr) {
        return rb_hash_aref(m_tmap, Ptr2Ref(ptr));
      }
      inline static void Unlink(void *ptr) {
        VALUE obj = InstanceFor(ptr);
        if (!NIL_P(obj)) {
          DATA_PTR(obj) = 0;
        }
      }
      inline static void Remove(void *ptr) {
        rb_funcall(m_tmap, m_delete, 1, Ptr2Ref(ptr));
      }
      inline static void Cleanup() {
        rb_gc_unregister_address(&m_tmap);
      }
    private:
      inline static VALUE Ptr2Ref(void *ptr) {
        return ULONG2NUM((unsigned long)ptr);
      }
    private:
      static VALUE m_tmap;
      static ID m_delete;
      static bool m_init;
  };

  class LWCRB_API Lang {
    private:
      struct NewArgs {
        std::string cname;
        int argc;
        VALUE *argv;
      };
      inline static VALUE WrappedNew(VALUE args) {
        NewArgs *a = (NewArgs*)args;
        VALUE klass = GetClass(a->cname);
        return rb_funcall(klass, rb_intern("new"), a->argc, a->argv);
      }
    public:
      inline static VALUE GetClass(const std::string &className) {
        return rb_const_get(rb_cObject, rb_intern(className.c_str()));
      }
      inline static VALUE New(const std::string &className, int n, VALUE *args) {
        int error = 0;
        NewArgs a = {className, n, args};
        VALUE self = rb_protect(WrappedNew, (VALUE)&a, &error);
        Error(std::cerr, error);
        return (error == 0 ? self : Qnil);
      }
      inline static void Error(std::ostream &, int error=-1) {
        if (error != 0) {
          
          int st;
          rb_eval_string_protect("if (e = $!) then STDERR.puts(\"#{e.class}: #{e.message}\", e.backtrace.map {|s| \"\tfrom #{s}\" }) end", &st);
          
          /*
          VALUE errinfo = rb_inspect(ruby_errinfo);
          rb_backtrace();
          fprintf(stderr, "%s", STR2CSTR(errinfo));
          */
          
          /*
          VALUE lasterr = rb_gv_get("$!");
          VALUE klass   = rb_class_path(CLASS_OF(lasterr));
          VALUE message = rb_obj_as_string(lasterr);
          out << "class = " << RSTRING(klass)->ptr << std::endl;
          out << "message = " << RSTRING(message)->ptr << std::endl;
          if (!NIL_P(ruby_errinfo)) {
            out << "backtrace = ";
            VALUE ary = rb_funcall(ruby_errinfo, rb_intern("backtrace"), 0);
            for (int c=0; c<RARRAY(ary)->len; ++c) {
              out << "\tfrom " << RSTRING(RARRAY(ary)->ptr[c])->ptr << std::endl;
            }
          }
          */
        }
      }
  };

  class LWCRB_API Embed {
    public:
      static inline void Init(int argc=0, char **argv=0, const std::string &name = "Embed") {
        ruby_init();
        ruby_init_loadpath();
        ruby_script(name.c_str());
        if (argc > 0 && argv != 0) {
          ruby_set_argv(argc, argv);
        }
        Tracker::Init();
      }
      static inline bool IsInitialized() {
        return (ruby_current_node != 0);
      }
      // ruby_options(int argc, char **argv);
      //   -> not the same as ruby_set_argv
      //   -> ruby_set_argv is for ARGV var in ruby scripts
      //   -> ruby_options is interpreter options (ruby.exe options)
      //   -> should parse automaticaly argc, argv to dispatch this correcly
      //   -> between the two !
      // ruby_run
      //   -> with ruby run never go back to caller (call exit internally)
      static inline int Eval(const std::string &script) {
        int status = 0;
        rb_eval_string_protect(script.c_str(), &status);
        return status;
      }
      static inline int EvalFile(const std::string &rbFile) {
        static char rdbuf[4096] = {};
        FILE *file = fopen(rbFile.c_str(), "r");
        if (file) {
          std::string script = "";
          while (fgets(rdbuf, 4096, file)) {
            script += std::string(rdbuf);
          }
          fclose(file);
          return Eval(script);
        }
        return -1;
      }
      static inline int AppendToPath(const std::string &dir) {
        //std::string cmd = "$: << \""+dir+"\"";
        //return Eval(cmd);
        ruby_incpush(dir.c_str());
        return 0;
      }
      static inline int Require(const std::string &rbFile) {
        std::string cmd = "require \""+rbFile+"\"";
        return Eval(cmd);
      }
      static inline int RequireGem(const std::string &gem) {
        std::string cmd = "require_gem \""+gem+"\"";
        return Eval(cmd);
      }
      static inline int Load(const std::string &rbFile) {
        std::string cmd = "load \""+rbFile+"\"";
        return Eval(cmd);
      }
      static inline void Cleanup() {
        Tracker::Cleanup();
        ruby_finalize();
      }
  };
  
  
  inline bool IsNil(VALUE val) {
    if (NIL_P(val)) {
      return true;
    }
    return false;
  }

  inline bool IsKindOf(VALUE val, VALUE klass) {
    if (TYPE(val)!=T_DATA || rb_obj_is_kind_of(val,klass)==Qfalse) {
      return false;
    }
    return true;
  }
  
  template <typename T> inline VALUE WrapPointer(VALUE klass, T *ptr, GC_FUNC mark, GC_FUNC sweep) {
    return Data_Wrap_Struct(klass, mark, sweep, ptr);
  }
  
  template <typename T> inline bool GetPointer(VALUE obj, T *&ptr) {
    ptr = 0;
    if (! NIL_P(obj)) {
      //Data_Get_Struct(obj, T, ptr);
      Check_Type(obj, T_DATA);
      ptr = (T*) DATA_PTR(obj);
      if (!ptr) {
        return false;
      }
    }
    return true;
  }

  template <typename T> inline bool GetTypedPointer(VALUE val, T *&ptr, VALUE klass) {
    ptr = 0;
    if (!IsKindOf(val, klass)) {
      return false;
    }
    return GetPointer(val, ptr);
  }
  
  inline VALUE ToArray(VALUE v) {
    return rb_check_array_type(v); // --> uses "to_ary"
    //return rb_check_convert_type(v, T_ARRAY, "Array", "to_a");
    // Object#to_a -> [self] ... this could be problematic
  }
  
  inline VALUE ToString(VALUE v) {
    return rb_check_string_type(v); // --> uses "to_str"
    //static char *null_str = "";
    //VALUE str = rb_check_convert_type(v, T_STRING, "String", "to_s");
    //if (!NIL_P(str) && !RSTRING(str)->ptr) {
    //  FL_SET(str, ELTS_SHARED);
    //  RSTRING(str)->ptr = null_str;
    //}
    //return str;
    // Object#to_s -> "#{id}" ... this could be problematic
  }
  
  
  namespace Exc {
    
    // In 'Exc' namespace all function calls raise an exception when failed
  
    inline void IsNil(VALUE val) {
      if (NIL_P(val)) {
        rb_raise(rb_eRuntimeError, "Invalid argument: non-nil expected.");
      }
    }

    inline void IsKindOf(VALUE val, VALUE klass) {
      if (TYPE(val)!=T_DATA || rb_obj_is_kind_of(val,klass)==Qfalse) {
        rb_raise(
          rb_eTypeError,
          "Invalid argument type: expected %s, got %s.",
          rb_class2name(klass), rb_obj_classname(val));
      }
    }
    
    template <typename T> inline void GetPointer(VALUE obj, T *&ptr) {
      ptr = 0;
      if (! NIL_P(obj)) {
        //Data_Get_Struct(obj, T, ptr);
        Check_Type(obj, T_DATA);
        ptr = (T*) DATA_PTR(obj);
        if (!ptr) {
          rb_raise(rb_eRuntimeError, "Underlying object already released.");
        }
      }
    }

    template <typename T> inline void GetTypedPointer(VALUE val, T *&ptr, VALUE klass) {
      ptr = 0;
      Exc::IsKindOf(val, klass);
      Exc::GetPointer(val, ptr);
    }
  }
  
  // MyClass *ptr = 0;
  // 
  // if (rb::GetTypedPointer(self, ptr, rb_cMyClass)) {
  //   doSomethingWith(ptr);
  // } else {
  //   thereIsAnAlternative(); 
  // }
  //
  // rb::Exc::GetTypedPointer(self, ptr, rb_cMyClass);
  // doSmothingWith(ptr);
}

#endif



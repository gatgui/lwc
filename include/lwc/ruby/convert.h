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

#ifndef __lwc_rb_convert_h__
#define __lwc_rb_convert_h__

#include <lwc/ruby/config.h>
#include <lwc/ruby/utils.h>
#include <lwc/ruby/types.h>
#include <lwc/ruby/rbobject.h>

namespace rb {

  template <typename T> struct RubyType {
    static const char* Name() {return "unknown";}
    static bool Check(VALUE obj) {return false;}
    static void ToC(VALUE , T &) {}
    static void Dispose(T &) {}
  };
  template <> struct RubyType<bool> {
    static const char* Name() {return "boolean";}
    static bool Check(VALUE obj) {return (TYPE(obj) == T_TRUE || TYPE(obj) == T_FALSE);}
    static void ToC(VALUE obj, bool &val) {val = (obj == Qtrue);}
    static void Dispose(bool &) {}
  };
  template <> struct RubyType<lwc::Integer> {
    static const char* Name() {return "integer";}
    static bool Check(VALUE obj) {return (TYPE(obj) == T_FIXNUM || TYPE(obj) == T_BIGNUM);}
    static void ToC(VALUE obj, lwc::Integer &val) {val = (lwc::Integer) NUM2LONG(obj);} // review this one
    static void Dispose(lwc::Integer &) {}
  };
  template <> struct RubyType<lwc::Real> {
    static const char* Name() {return "real";}
    static bool Check(VALUE obj) {return (TYPE(obj) == T_FLOAT || TYPE(obj) == T_FIXNUM || TYPE(obj) == T_BIGNUM);}
    static void ToC(VALUE obj, lwc::Real &val) {val = NUM2DBL(obj);}
    static void Dispose(lwc::Real &) {}
  };
  template <> struct RubyType<char*> {
    static const char* Name() {return "string";}
    static bool Check(VALUE obj) {return (TYPE(obj) == T_NIL || TYPE(obj) == T_STRING);}
    static void ToC(VALUE obj, char* &val) {
      if (obj == Qnil) {
        val = 0;
      } else {
        char *str = RSTRING(obj)->ptr;
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
  template <> struct RubyType<lwc::Object*> {
    static const char* Name() {return "il.Object";}
    static bool Check(VALUE obj) {return (TYPE(obj) == T_NIL || rb::IsKindOf(obj, cLWCObject));}
    static void ToC(VALUE obj, lwc::Object* &val) {
      if (obj == Qnil) {
        val = 0;
      } else {
        rb::GetPointer(obj, val);
      }
    }
    static void Dispose(lwc::Object *&) {}
  };

  template <typename T> struct CType {
    static void ToRuby(const T &, VALUE &) {}
  };
  template <> struct CType<bool> {
    static void ToRuby(const bool &val, VALUE &obj) {
      obj = (val == true ? Qtrue : Qfalse);
    }
  };
  template <> struct CType<lwc::Integer> {
    static void ToRuby(const lwc::Integer &val, VALUE &obj) {obj = LONG2NUM(long(val));} // review this one
  };
  template <> struct CType<lwc::Real> {
    static void ToRuby(const lwc::Real &val, VALUE &obj) {obj = rb_float_new(val);}
  };
  template <> struct CType<char*> {
    static void ToRuby(const char *val, VALUE &obj) {
      if (!val) {
        obj = Qnil;
      } else {
        obj = rb_str_new2(val);
      }
    }
  };
  template <> struct CType<lwc::Object*> {
    static void ToRuby(const lwc::Object *val, VALUE &obj) {
      if (!val) {
        obj = Qnil;
      } else {
        if (!strcmp(val->getLoaderName(), "rbloader")) {
          obj = ((rb::Object*)val)->self();
        } else {
          obj = rb_funcall2(cLWCObject, rb_intern("new"), 0, NULL);
          SetObjectPointer(obj, (lwc::Object*)val);
        }
      }
    }
  };

  template <lwc::Type T> struct Ruby2C {
    
    typedef typename lwc::Enum2Type<T>::Type Type;
    typedef typename lwc::Enum2Type<T>::Type* Array;
    
    static void ToValue(VALUE obj, Type &val) {
      if (!RubyType<Type>::Check(obj)) {
        char message[512];
        sprintf(message, "Expected %s argument", RubyType<Type>::Name());
        rb_raise(rb_eRuntimeError, message);
        return;
      }
      RubyType<Type>::ToC(obj, val);
    }
    
    static void DisposeValue(Type &val) {
      RubyType<Type>::Dispose(val);
    }
    
    static void ToArray(VALUE obj, Array &ary, size_t &length) {
      if (TYPE(obj) == T_ARRAY) {
        rb_raise(rb_eRuntimeError, "Expected array argument");
      }
      struct RArray *rary = RARRAY(obj);
      length = rary->len;
      //ary = (Array) lwc::memory::Alloc(length, sizeof(Type));
      if (ary != 0 && length != 0) {
        // should dispose the values ? [not always though]
        // realloc should copy back the values though...
        for (size_t i=0; i<length; ++i) {
          RubyType<Type>::Dispose(ary[i]);
        }
      }
      ary = (Array) lwc::memory::Alloc(length, sizeof(Type), (void*)ary);
      for (size_t i=0; i<length; ++i) {
        VALUE item = rary->ptr[i];
        if (!RubyType<Type>::Check(item)) {
          char message[512];
          sprintf(message, "Expected array of %ss argument", RubyType<Type>::Name());
          return;
        }
        RubyType<Type>::ToC(item, ary[i]);
      }
    }
    
    static void DisposeArray(Array &ary, size_t length) {
      for (size_t i=0; i<length; ++i) {
        RubyType<Type>::Dispose(ary[i]);
      }
      lwc::memory::Free((void*)ary);
    }
  };
  
  template <lwc::Type T> struct C2Ruby {
    
    typedef typename lwc::Enum2Type<T>::Type Type;
    typedef typename lwc::Enum2Type<T>::Type* Array;
    
    // beware: const Type & width Type == char* gives char * const &val
    static void ToValue(const Type &val, VALUE &obj) {
      CType<Type>::ToRuby(val, obj);
    }
    
    static void ToArray(const Array &ary, size_t length, VALUE &obj) {
      if (obj != Qnil && TYPE(obj) == T_ARRAY) {
        struct RArray *rary = RARRAY(obj);
        size_t sz = rary->len;
        if (length > sz) {
          size_t d = length - sz;
          for (size_t i=0; i<d; ++i) {
            rb_ary_push(obj, Qnil);
          }
        } else if (length < sz) {
          size_t n = sz - length;
          for (size_t i=0; i<n; ++i) {
            rb_ary_pop(obj);
          }
        }
      } else {
        obj = rb_ary_new2(length);
      }
      for (size_t i=0; i<length; ++i) {
        VALUE item;
        CType<Type>::ToRuby(ary[i], item);
        rb_ary_store(obj, i, item);
      }
    }
  };
  
  template <lwc::Type T> struct ParamConverter {
    
    typedef typename lwc::Enum2Type<T>::Type Type;
    typedef typename lwc::Enum2Type<T>::Type* Array;
    
    static void PreCall(const lwc::Argument &desc, size_t, VALUE *args, size_t nargs, size_t &iarg, std::map<size_t,size_t> &arraySizes, Type &val) {
      if (desc.getDir() == lwc::AD_IN || desc.getDir() == lwc::AD_INOUT) {
        if (desc.arrayArg() >= 0) {
          // this should only be executed for integer types
          //val = (Type) arraySizes[desc.arrayArg()];
          unsigned long idx = (unsigned long) arraySizes[size_t(desc.arrayArg())];
          lwc::Convertion<unsigned long, Type>::Do(idx, val);
        } else {
          if (iarg >= nargs) {
            rb_raise(rb_eRuntimeError, "Not enough argument");
            return;
          }
          Ruby2C<T>::ToValue(args[iarg], val);
          ++iarg;
        }     
      }
    }
    
    static void PostCall(const lwc::Argument &desc, size_t, VALUE *, size_t, size_t &iarg, std::map<size_t,size_t> &arraySizes, Type &val, VALUE &rv) {
      if (desc.getDir() == lwc::AD_IN) {
        Ruby2C<T>::DisposeValue(val);
        ++iarg;
      } else {
        if (desc.arrayArg() >= 0) {
          arraySizes[size_t(desc.arrayArg())] = size_t(val);
          Ruby2C<T>::DisposeValue(val);
          
        } else {
          VALUE obj = Qnil;
          C2Ruby<T>::ToValue(val, obj);
          Ruby2C<T>::DisposeValue(val);
          rb_ary_push(rv, obj);
        }
      }
    }
    
    static void PreCallArray(const lwc::Argument &desc, size_t idesc, VALUE *args, size_t nargs, size_t &iarg, std::map<size_t,size_t> &arraySizes, Array &ary) {
      if (desc.getDir() == lwc::AD_IN || desc.getDir() == lwc::AD_INOUT) {
        size_t length;
        if (iarg >= nargs) {
          rb_raise(rb_eRuntimeError, "Not enough argument");
          return;
        }
        Ruby2C<T>::ToArray(args[iarg], ary, length);
        arraySizes[idesc] = length;
        ++iarg;
      }
    }
    
    static void PostCallArray(const lwc::Argument &desc, size_t idesc, VALUE *args, size_t, size_t &iarg, std::map<size_t,size_t> &arraySizes, Array &ary, VALUE &rv) {
      if (desc.getDir() == lwc::AD_IN) {
        Ruby2C<T>::DisposeArray(ary, arraySizes[idesc]);
      } else {
        VALUE obj = Qnil;
        if (desc.getDir() == lwc::AD_INOUT) {
          obj = args[iarg];
        }
        C2Ruby<T>::ToArray(ary, arraySizes[idesc], obj);
        if (desc.getDir() != lwc::AD_INOUT) {
          //if (desc.isAllocated()) {
          Ruby2C<T>::DisposeArray(ary, arraySizes[idesc]);
          //}
          rb_ary_push(rv, obj);
        } else {
          Ruby2C<T>::DisposeArray(ary, arraySizes[idesc]);
        }
      }
    }
  };

}

#endif



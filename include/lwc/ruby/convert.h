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
      if (NIL_P(obj) || TYPE(obj) != T_ARRAY) {
        rb_raise(rb_eTypeError, "Expected array argument");
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
    
    template <typename TT>
    static bool GetDefaultValue(const lwc::Argument &desc, TT &val) {
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
    
    static bool PreCall(const lwc::Argument &desc, size_t, VALUE *args, VALUE kwargs, size_t nargs, size_t &iarg, std::map<size_t,size_t> &arraySizes, Type &val, std::string &err) {
      if (desc.getDir() == lwc::AD_IN || desc.getDir() == lwc::AD_INOUT) {
        if (desc.arrayArg() >= 0) {
          // this should only be executed for integer types
          //val = (Type) arraySizes[desc.arrayArg()];
          unsigned long idx = (unsigned long) arraySizes[size_t(desc.arrayArg())];
          lwc::Convertion<unsigned long, Type>::Do(idx, val);
        } else {
          if (iarg >= nargs) {
            bool failed = true;
            if (desc.isNamed()) {
              VALUE dv = (NIL_P(kwargs) ? Qnil : HashGet(kwargs, desc.getName()));
              if (dv != Qnil) {
                Ruby2C<T>::ToValue(dv, val);
                failed = false;
                
              } else if (desc.hasDefaultValue()) {
                failed = !GetDefaultValue(desc, val);
              }
            }
            if (failed) {
              std::cout << "PreCall: not enough argument, expected " << desc.toString() << std::endl;
              err = "Not enough arguments";
              return false;
            }
            
          } else {
            Ruby2C<T>::ToValue(args[iarg], val);
            ++iarg;
          }
        }     
      }
      return true;
    }
    
    static void PostCall(const lwc::Argument &desc, size_t, VALUE *, VALUE kwargs, size_t nargs, size_t &iarg, std::map<size_t,size_t> &arraySizes, Type &val, VALUE &rv) {
      bool dontDispose = false;
      if (iarg >= nargs && desc.isNamed() &&
          (NIL_P(kwargs) || !HashHas(kwargs, desc.getName())) &&
          desc.hasDefaultValue()) {
        dontDispose = true;
      }
      
      if (desc.getDir() == lwc::AD_IN) {
        if (!dontDispose) {
          Ruby2C<T>::DisposeValue(val);
        }
        
      } else {
        if (desc.arrayArg() >= 0) {
          arraySizes[size_t(desc.arrayArg())] = size_t(val);
          if (!dontDispose) {
            Ruby2C<T>::DisposeValue(val);
          }
          
        } else {
          VALUE obj = Qnil;
          C2Ruby<T>::ToValue(val, obj);
          if (!dontDispose) {
            Ruby2C<T>::DisposeValue(val);
          }
          rb_ary_push(rv, obj);
        }
      }
    }
    
    static bool PreCallArray(const lwc::Argument &desc, size_t idesc, const lwc::Argument &sdesc, VALUE *args, VALUE kwargs, size_t nargs, size_t &iarg, std::map<size_t,size_t> &arraySizes, Array &ary, std::string &err) {
      if (desc.getDir() == lwc::AD_IN || desc.getDir() == lwc::AD_INOUT) {
        size_t length;
        if (iarg >= nargs) {
          bool failed = true;
          if (desc.isNamed()) {
            VALUE dv = (NIL_P(kwargs) ? Qnil : HashGet(kwargs, desc.getName()));
            if (dv != Qnil) {
              Ruby2C<T>::ToArray(dv, ary, length);
              arraySizes[idesc] = length;
              failed = false;
              
            } else if (desc.hasDefaultValue()) {
              failed = !GetDefaultValue(desc, ary);
              if (!failed) {
                lwc::Integer tmp;
                failed = !GetDefaultValue(sdesc, tmp);
                if (!failed) {
                  length = (size_t) tmp;
                }
              }
            }
          }
          if (failed) {
            std::cout << "PreCallArray: not enough argument, expected " << desc.toString() << std::endl;
            err = "Not enough argument";
            return false;
          }
        } else {
          Ruby2C<T>::ToArray(args[iarg], ary, length);
          arraySizes[idesc] = length;
          ++iarg;
        }
      }
      return true;
    }
    
    static void PostCallArray(const lwc::Argument &desc, size_t idesc, const lwc::Argument &, VALUE *args, VALUE kwargs, size_t nargs, size_t &iarg, std::map<size_t,size_t> &arraySizes, Array &ary, VALUE &rv) {
      bool dontDispose = false;
      if (iarg >= nargs && desc.isNamed() &&
          (NIL_P(kwargs) || !HashHas(kwargs, desc.getName())) &&
          desc.hasDefaultValue()) {
        dontDispose = true;
      }
      
      if (desc.getDir() == lwc::AD_IN) {
        if (!dontDispose) {
          Ruby2C<T>::DisposeArray(ary, arraySizes[idesc]);
        }
        
      } else {
        VALUE obj = Qnil;
        if (desc.getDir() == lwc::AD_INOUT) {
          if (desc.isNamed()) {
            obj = (NIL_P(kwargs) ? Qnil : HashGet(kwargs, desc.getName()));
          } else {
            obj = args[iarg];
          }
        }
        C2Ruby<T>::ToArray(ary, arraySizes[idesc], obj);
        if (desc.getDir() != lwc::AD_INOUT) {
          if (!dontDispose) {
            Ruby2C<T>::DisposeArray(ary, arraySizes[idesc]);
          }
          rb_ary_push(rv, obj);
          
        } else {
          if (!dontDispose) {
            Ruby2C<T>::DisposeArray(ary, arraySizes[idesc]);
          }
        }
      }
    }
  };

}

#endif



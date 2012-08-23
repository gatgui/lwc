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

#include <lwc/ruby/types.h>
#include <lwc/ruby/utils.h>
#include <lwc/ruby/convert.h>

namespace rb {

VALUE cLWCArgument = Qnil;

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
  std::cout << "Add lwc ruby module default value " << std::hex << ptr << std::dec << ", " << t << ", " << array << ", " << len << std::endl;
#endif
  gsDefaultValues.push_back(DefaultValueEntry());
  DefaultValueEntry &dve = gsDefaultValues.back();
  dve.type = t;
  dve.array = array;
  dve.len = len;
  dve.ptr = ptr;
}

bool SetArgDefault(lwc::Argument &arg, VALUE obj) {
  if (arg.getDir() != lwc::AD_IN) {
    return false;
  }
  
  bool rv = true;
  
  switch (arg.getType()) {
  case lwc::AT_BOOL:
    if (arg.isArray()) {
      bool *defVal = 0;
      size_t len = 0;
      Ruby2C<lwc::AT_BOOL>::ToArray(obj, defVal, len);
      arg.setDefaultValue(defVal);
      AddDefaultValue((void*)defVal, arg.getType(), true, len);
    } else {
      bool defVal = false;
      Ruby2C<lwc::AT_BOOL>::ToValue(obj, defVal);
      arg.setDefaultValue(defVal);
    }
    break;
  case lwc::AT_INT:
    if (arg.isArray()) {
      lwc::Integer *defVal = 0;
      size_t len = 0;
      Ruby2C<lwc::AT_INT>::ToArray(obj, defVal, len);
      arg.setDefaultValue(defVal);
      AddDefaultValue((void*)defVal, arg.getType(), true, len);
    } else {
      lwc::Integer defVal = 0;
      Ruby2C<lwc::AT_INT>::ToValue(obj, defVal);
      arg.setDefaultValue(defVal);
    }
    break;
  case lwc::AT_REAL:
    if (arg.isArray()) {
      lwc::Real *defVal = 0;
      size_t len = 0;
      Ruby2C<lwc::AT_REAL>::ToArray(obj, defVal, len);
      arg.setDefaultValue(defVal);
      AddDefaultValue((void*)defVal, arg.getType(), true, len);
    } else {
      lwc::Real defVal = 0.0;
      Ruby2C<lwc::AT_REAL>::ToValue(obj, defVal);
      arg.setDefaultValue(defVal);
    }
    break;
  case lwc::AT_STRING:
    if (arg.isArray()) {
      char **defVal = 0;
      size_t len = 0;
      Ruby2C<lwc::AT_STRING>::ToArray(obj, defVal, len);
      arg.setDefaultValue(defVal);
      AddDefaultValue((void*)defVal, arg.getType(), true, len);
    } else {
      char *defVal = 0;
      Ruby2C<lwc::AT_STRING>::ToValue(obj, defVal);
      arg.setDefaultValue(defVal);
      AddDefaultValue((void*)defVal, arg.getType(), false);
    }
    break;
  case lwc::AT_OBJECT:
    if (arg.isArray()) {
      lwc::Object **defVal = 0;
      size_t len = 0;
      Ruby2C<lwc::AT_OBJECT>::ToArray(obj, defVal, len);
      arg.setDefaultValue(defVal);
      AddDefaultValue((void*)defVal, arg.getType(), true, len);
    } else {
      lwc::Object *defVal = 0;
      Ruby2C<lwc::AT_OBJECT>::ToValue(obj, defVal);
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
  std::cout << "Free lwc ruby module default value " << std::hex << dve.ptr << std::dec << ", " << dve.type << ", " << dve.array << ", " << dve.len << std::endl;
#endif
  switch (dve.type) {
  case lwc::AT_BOOL:
    if (dve.array) {
      bool *ary = (bool*) dve.ptr;
      Ruby2C<lwc::AT_BOOL>::DisposeArray(ary, dve.len);
    }
    break;
  case lwc::AT_INT:
    if (dve.array) {
      lwc::Integer *ary = (lwc::Integer*) dve.ptr;
      Ruby2C<lwc::AT_INT>::DisposeArray(ary, dve.len);
    }
    break;
  case lwc::AT_REAL:
    if (dve.array) {
      lwc::Real *ary = (lwc::Real*) dve.ptr;
      Ruby2C<lwc::AT_REAL>::DisposeArray(ary, dve.len);
    }
    break;
  case lwc::AT_STRING:
    if (dve.array) {
      char **ary = (char**) dve.ptr;
      Ruby2C<lwc::AT_STRING>::DisposeArray(ary, dve.len);
    } else {
      char *str = (char*) dve.ptr;
      Ruby2C<lwc::AT_STRING>::DisposeValue(str);
    }
    break;
  case lwc::AT_OBJECT:
    if (dve.array) {
      lwc::Object **ary = (lwc::Object**) dve.ptr;
      Ruby2C<lwc::AT_OBJECT>::DisposeArray(ary, dve.len);
    } else {
      lwc::Object *obj = (lwc::Object*) dve.ptr;
      Ruby2C<lwc::AT_OBJECT>::DisposeValue(obj);
    }
  default:
    break;
  }
}

// ---

void CleanupModule() {
#ifdef LWC_MEMTRACK
  std::cout << "=== lwc library: Memory status after ruby module cleanup" << std::endl;
  std::cout << lwc::Object::GetInstanceCount() << " remaining object(s)" << std::endl;
  lwc::memory::PrintAllocationInfo();
#endif
}

// ---

void rbarg_mark(void *) {
}

void rbarg_sweep(void *data) {
  lwc::Argument *arg = (lwc::Argument*)data;
  delete arg;
}

static VALUE rbarg_alloc(VALUE klass) {
  lwc::Argument *arg = new lwc::Argument();
  return rb::WrapPointer(klass, arg, rbarg_mark, rbarg_sweep);
}

static VALUE rbarg_init(int argc, VALUE *argv, VALUE self) {
  // type, direction, arylen=-1, hasdef=false, def=<value>, name=nil
  // should review this one
  if (argc >= 2) {
    lwc::Argument *arg;
    rb::Exc::GetPointer(self, arg);
    
    arg->setDir(lwc::Direction(NUM2INT(argv[0])));
    arg->setType(lwc::Type(NUM2INT(argv[1])));
    
    VALUE kwargs = Qnil;
    
    if (argc == 3) {
      // check if it is a dict
      // raise and exception if fails
      kwargs = rb_check_convert_type(argv[2], T_HASH, "Hash", "to_hash");
      if (NIL_P(kwargs)) {
        rb_raise(rb_eArgError, "RLWC::Argument.initialize: last argument must be a hash (keywords)");
      }
      
      ID haskey = rb_intern("has_key?");
      
      VALUE args[1], rv;
      // use rb_intern for keys?
      
      //rb_intern("lenidx") won't work
      args[0] = rb_eval_string(":lenidx");
      rv = rb_funcall2(kwargs, haskey, 1, args);
      if (rv == Qtrue) {
        arg->setArraySizeArg(NUM2INT(rb_hash_aref(kwargs, args[0])));
      }
      
      args[0] = rb_eval_string(":def");
      rv = rb_funcall2(kwargs, haskey, 1, args);
      if (rv == Qtrue) {
        SetArgDefault(*arg, rb_hash_aref(kwargs, args[0]));
      }
      
      args[0] = rb_eval_string(":name");
      rv = rb_funcall2(kwargs, haskey, 1, args);
      if (rv == Qtrue) {
        VALUE sname = rb_check_string_type(rb_hash_aref(kwargs, args[0]));
        if (NIL_P(sname)) {
          rb_raise(rb_eTypeError, "RLWC::Argument.initialize expects a string for :name keyword");
        }
        arg->setName(RSTRING(sname)->ptr);
      }
      
    } else if (argc > 3) {
      rb_raise(rb_eArgError, "RLWC::Argument.initialize requires 0 or 2 arguments + keywords");
    }
     
  } else if (argc != 0) {
    rb_raise(rb_eArgError, "RLWC::Argument.initialize requires 0 or 2 arguments + keywords");
    
  } else {
    // NOOP
  }
  return self;
}

static VALUE rbarg_isArray(VALUE self) {
  lwc::Argument *arg;
  rb::Exc::GetPointer(self, arg);
  return (arg->isArray() ? Qtrue : Qfalse);
}

static VALUE rbarg_isNamed(VALUE self) {
  lwc::Argument *arg;
  rb::Exc::GetPointer(self, arg);
  return (arg->isNamed() ? Qtrue : Qfalse);
}

static VALUE rbarg_hasDefaultValue(VALUE self) {
  lwc::Argument *arg;
  rb::Exc::GetPointer(self, arg);
  return (arg->hasDefaultValue() ? Qtrue : Qfalse);
}

static VALUE rbarg_getDir(VALUE self) {
  lwc::Argument *arg;
  rb::Exc::GetPointer(self, arg);
  return INT2NUM(arg->getDir());
}

static VALUE rbarg_setDir(VALUE self, VALUE val) {
  lwc::Argument *arg;
  rb::Exc::GetPointer(self, arg);
  lwc::Direction d = lwc::Direction(NUM2INT(val));
  arg->setDir(d);
  return self;
}

static VALUE rbarg_getType(VALUE self) {
  lwc::Argument *arg;
  rb::Exc::GetPointer(self, arg);
  return INT2NUM(arg->getType());
}

static VALUE rbarg_setType(VALUE self, VALUE val) {
  lwc::Argument *arg;
  rb::Exc::GetPointer(self, arg);
  lwc::Type t = lwc::Type(NUM2INT(val));
  arg->setType(t);
  return self;
}

static VALUE rbarg_getName(VALUE self) {
  lwc::Argument *arg;
  rb::Exc::GetPointer(self, arg);
  return rb_str_new2(arg->getName().c_str());
}

static VALUE rbarg_setName(VALUE self, VALUE val) {
  lwc::Argument *arg;
  rb::Exc::GetPointer(self, arg);
  VALUE sval = rb_check_string_type(val);
  if (NIL_P(sval)) {
    rb_raise(rb_eTypeError, "RLWC::Argument.name= expects a string as argument");
  }
  std::string name = RSTRING(sval)->ptr;
  arg->setName(name.c_str());
  return self;
}

static VALUE rbarg_getArySzArg(VALUE self) {
  lwc::Argument *arg;
  rb::Exc::GetPointer(self, arg);
  return INT2NUM(long(arg->arraySizeArg()));
}

static VALUE rbarg_setArySzArg(VALUE self, VALUE val) {
  lwc::Argument *arg;
  rb::Exc::GetPointer(self, arg);
  arg->setArraySizeArg(NUM2INT(val));
  return self;
}

static VALUE rbarg_getAryArg(VALUE self) {
  lwc::Argument *arg;
  rb::Exc::GetPointer(self, arg);
  return INT2NUM(long(arg->arrayArg()));
}

static VALUE rbarg_setAryArg(VALUE self, VALUE val) {
  lwc::Argument *arg;
  rb::Exc::GetPointer(self, arg);
  arg->setArrayArg(NUM2INT(val));
  return self;
}

static VALUE rbarg_tos(VALUE self) {
  lwc::Argument *arg;
  rb::Exc::GetPointer(self, arg);
  std::string s = arg->toString();
  return rb_str_new2(s.c_str());
}

static VALUE rbarg_docString(int argc, VALUE *argv, VALUE self) {
  if (argc > 1) {
    rb_raise(rb_eArgError, "RLWC::Argument.docString accepts at most 1 argument");
  }
  lwc::Argument *arg;
  rb::Exc::GetPointer(self, arg);
  std::string indent = "";
  if (argc == 1) {
    VALUE sval = rb_check_string_type(argv[0]);
    if (NIL_P(sval)) {
      rb_raise(rb_eTypeError, "RLWC::Argument.docString expects a string as argument");
    }
    indent = RSTRING(sval)->ptr;
  }
  std::string s = arg->docString(indent);
  return rb_str_new2(s.c_str());
}

bool InitArgument(VALUE mod) {
  
  cLWCArgument = rb_define_class_under(mod, "Argument", rb_cObject);
  rb_define_alloc_func(cLWCArgument, rbarg_alloc);
  rb_define_method(cLWCArgument, "initialize", RBM(rbarg_init), -1);
  rb_define_method(cLWCArgument, "array?", RBM(rbarg_isArray), 0);
  rb_define_method(cLWCArgument, "named?", RBM(rbarg_isNamed), 0);
  rb_define_method(cLWCArgument, "hasDefault?", RBM(rbarg_hasDefaultValue), 0);
  rb_define_method(cLWCArgument, "name", RBM(rbarg_getName), 0);
  rb_define_method(cLWCArgument, "name=", RBM(rbarg_setName), 1);
  rb_define_method(cLWCArgument, "dir", RBM(rbarg_getDir), 0);
  rb_define_method(cLWCArgument, "dir=", RBM(rbarg_setDir), 1);
  rb_define_method(cLWCArgument, "type", RBM(rbarg_getType), 0);
  rb_define_method(cLWCArgument, "type=", RBM(rbarg_setType), 1);
  rb_define_method(cLWCArgument, "arraySizeArg", RBM(rbarg_getArySzArg), 0);
  rb_define_method(cLWCArgument, "arraySizeArg=", RBM(rbarg_setArySzArg), 1);
  rb_define_method(cLWCArgument, "arrayArg", RBM(rbarg_getAryArg), 0);
  rb_define_method(cLWCArgument, "arrayArg=", RBM(rbarg_setAryArg), 1);
  rb_define_method(cLWCArgument, "to_s", RBM(rbarg_tos), 0);
  rb_define_method(cLWCArgument, "docString", RBM(rbarg_docString), -1);
  
  rb_define_const(mod, "AD_IN", INT2NUM(lwc::AD_IN));
  rb_define_const(mod, "AD_INOUT", INT2NUM(lwc::AD_INOUT));
  rb_define_const(mod, "AD_OUT", INT2NUM(lwc::AD_OUT));
  
  rb_define_const(mod, "AT_UNKNOWN", INT2NUM(lwc::AT_UNKNOWN));
  rb_define_const(mod, "AT_BOOL", INT2NUM(lwc::AT_BOOL));
  rb_define_const(mod, "AT_INT", INT2NUM(lwc::AT_INT));
  rb_define_const(mod, "AT_REAL", INT2NUM(lwc::AT_REAL));
  rb_define_const(mod, "AT_STRING", INT2NUM(lwc::AT_STRING));
  rb_define_const(mod, "AT_OBJECT", INT2NUM(lwc::AT_OBJECT));
  rb_define_const(mod, "AT_BOOL_ARRAY", INT2NUM(lwc::AT_BOOL_ARRAY));
  rb_define_const(mod, "AT_INT_ARRAY", INT2NUM(lwc::AT_INT_ARRAY));
  rb_define_const(mod, "AT_REAL_ARRAY", INT2NUM(lwc::AT_REAL_ARRAY));
  rb_define_const(mod, "AT_STRING_ARRAY", INT2NUM(lwc::AT_STRING_ARRAY));
  rb_define_const(mod, "AT_OBJECT_ARRAY", INT2NUM(lwc::AT_OBJECT_ARRAY));
  
  return true;
}

}


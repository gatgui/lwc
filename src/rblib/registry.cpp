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
#include <lwc/ruby/rbobject.h>

namespace rb {

VALUE cLWCRegistry = Qnil;

void rbreg_mark(void *) {
}

void rbreg_sweep(void *) {
}

static VALUE rbreg_alloc(VALUE klass) {
  lwc::Registry *reg = 0;
  return rb::WrapPointer(klass, reg, rbreg_mark, rbreg_sweep);
}

static VALUE rbreg_init(VALUE self) {
  return self;
}

static VALUE rbreg_addLoaderPath(VALUE self, VALUE rpath) {
  lwc::Registry *reg = lwc::Registry::Instance();
  if (!reg) {
    rb_raise(rb_eRuntimeError, "lwc::Registry has not yet been initialized");
  }
  VALUE spath = rb_check_string_type(rpath);
  if (NIL_P(spath)) {
    rb_raise(rb_eTypeError, "RLWC::Registry.addLoaderPath expects a string as argument");
  }
  char *path = RSTRING(spath)->ptr;
  reg->addLoaderPath(path);
  return self;
}

static VALUE rbreg_addModulePath(VALUE self, VALUE rpath) {
  lwc::Registry *reg = lwc::Registry::Instance();
  if (!reg) {
    rb_raise(rb_eRuntimeError, "lwc::Registry has not yet been initialized");
  }
  VALUE spath = rb_check_string_type(rpath);
  if (NIL_P(spath)) {
    rb_raise(rb_eTypeError, "RLWC::Registry.addModulePath expects a string as argument");
  }
  char *path = RSTRING(spath)->ptr;
  reg->addModulePath(path);
  return self;
}

static VALUE rbreg_numTypes(VALUE) {
  lwc::Registry *reg = lwc::Registry::Instance();
  if (!reg) {
    rb_raise(rb_eRuntimeError, "lwc::Registry has not yet been initialized");
  }
  return ULONG2NUM(reg->numTypes());
}

static VALUE rbreg_getTypeName(VALUE, VALUE ridx) {
  lwc::Registry *reg = lwc::Registry::Instance();
  if (!reg) {
    rb_raise(rb_eRuntimeError, "lwc::Registry has not yet been initialized");
  }
  unsigned long idx = NUM2ULONG(ridx);
  const char *tn = reg->getTypeName(idx);
  if (!tn) {
    return Qnil;
  } else {
    return rb_str_new2(tn);
  }
}

static VALUE rbreg_hasType(VALUE, VALUE rname) {
  lwc::Registry *reg = lwc::Registry::Instance();
  if (!reg) {
    rb_raise(rb_eRuntimeError, "lwc::Registry has not yet been initialized");
  }
  VALUE sname = rb_check_string_type(rname);
  if (NIL_P(sname)) {
    rb_raise(rb_eTypeError, "RLWC::Registry.hasType expects a string as argument");
  }
  char *name = RSTRING(sname)->ptr;
  return (reg->hasType(name) ? Qtrue : Qfalse);
}

static VALUE rbreg_getDesc(VALUE, VALUE rname) {
  lwc::Registry *reg = lwc::Registry::Instance();
  if (!reg) {
    rb_raise(rb_eRuntimeError, "lwc::Registry has not yet been initialized");
  }
  VALUE sname = rb_check_string_type(rname);
  if (NIL_P(sname)) {
    rb_raise(rb_eTypeError, "RLWC::Registry.getDescription expects a string as argument");
  }
  char *name = RSTRING(sname)->ptr;
  const char *desc = reg->getDescription(name);
  return rb_str_new2(desc ? desc : "");
}

static VALUE rbreg_docString(int argc, VALUE *argv, VALUE) {
  if (argc < 1 || argc > 2) {
    rb_raise(rb_eArgError, "RLWC::Registry.docString accepts 1 or 2 arguments");
  }
  lwc::Registry *reg = lwc::Registry::Instance();
  if (!reg) {
    rb_raise(rb_eRuntimeError, "lwc::Registry has not yet been initialized");
  }
  VALUE sname = rb_check_string_type(argv[0]);
  if (NIL_P(sname)) {
    rb_raise(rb_eTypeError, "RLWC::Registry.docString expects a string as first argument");
  }
  char *name = RSTRING(sname)->ptr;
  std::string indent = "";
  if (argc == 2) {
    VALUE sindent = rb_check_string_type(argv[1]);
    if (NIL_P(sindent)) {
      rb_raise(rb_eTypeError, "RLWC::Registry.docString expects a string as second argument");
    }
    indent = RSTRING(sindent)->ptr;
  }
  return rb_str_new2(reg->docString(name, indent).c_str());
}

static VALUE rbreg_getMethods(VALUE, VALUE rname) {
  lwc::Registry *reg = lwc::Registry::Instance();
  if (!reg) {
    rb_raise(rb_eRuntimeError, "lwc::Registry has not yet been initialized");
  }
  VALUE sname = rb_check_string_type(rname);
  if (NIL_P(sname)) {
    rb_raise(rb_eTypeError, "RLWC::Registry.getMethods expects a string as argument");
  }
  char *name = RSTRING(sname)->ptr;
  const lwc::MethodsTable *mt = reg->getMethods(name);
  if (!mt) {
    return Qnil;
  } else {
    VALUE rv = rb_funcall2(cLWCMethodsTable, rb_intern("new"), 0, NULL);
    RDATA(rv)->data = (void*) mt;
    return rv;
  }
}

static VALUE rbreg_create(VALUE, VALUE rname) {
  lwc::Registry *reg = lwc::Registry::Instance();
  if (!reg) {
    rb_raise(rb_eRuntimeError, "lwc::Registry has not yet been initialized");
  }
  VALUE sname = rb_check_string_type(rname);
  if (NIL_P(sname)) {
    rb_raise(rb_eTypeError, "RLWC::Registry.create expects a string as argument");
  }
  char *name = RSTRING(sname)->ptr;
  lwc::Object *obj = reg->create(name);
  if (!obj) {
    return Qnil;
  } else {
    VALUE rv = Qnil;
    //if (!strcmp(obj->getLoaderName(), "rbloader")) {
    //  rv = ((Object*)obj)->self();
    //  
    //} else {
      rv = rb_funcall2(cLWCObject, rb_intern("new"), 0, NULL);
      SetObjectPointer(rv, obj);
    //}
    return rv;
  }
}

static VALUE rbreg_get(VALUE, VALUE rname) {
  lwc::Registry *reg = lwc::Registry::Instance();
  if (!reg) {
    rb_raise(rb_eRuntimeError, "lwc::Registry has not yet been initialized");
  }
  VALUE sname = rb_check_string_type(rname);
  if (NIL_P(sname)) {
    rb_raise(rb_eTypeError, "RLWC::Registry.get expects a string as argument");
  }
  char *name = RSTRING(sname)->ptr;
  lwc::Object *obj = reg->get(name);
  if (!obj) {
    return Qnil;
  } else {
    VALUE rv = Qnil;
    //if (!strcmp(obj->getLoaderName(), "rbloader")) {
    //  rv = ((Object*)obj)->self();
    //  
    //} else {
      rv = rb_funcall2(cLWCObject, rb_intern("new"), 0, NULL);
      SetObjectPointer(rv, obj);
    //}
    return rv;
  }
}

static VALUE rbreg_destroy(VALUE self, VALUE robj) {
  lwc::Registry *reg = lwc::Registry::Instance();
  if (!reg) {
    rb_raise(rb_eRuntimeError, "lwc::Registry has not yet been initialized");
  }
  lwc::Object *obj = 0;
  rb::Exc::GetTypedPointer(robj, obj, cLWCObject);
  reg->destroy(obj);
  //RDATA(robj)->data = (void*)0;
  SetObjectPointer(robj, 0);
  return self;
}

bool InitRegistry(VALUE mod) {
  cLWCRegistry = rb_define_class_under(mod, "Registry", rb_cObject);
  rb_define_alloc_func(cLWCRegistry, rbreg_alloc);
  rb_define_method(cLWCRegistry, "initialize", RBM(rbreg_init), 0);
  rb_define_method(cLWCRegistry, "addLoaderPath", RBM(rbreg_addLoaderPath), 1);
  rb_define_method(cLWCRegistry, "addModulePath", RBM(rbreg_addModulePath), 1);
  rb_define_method(cLWCRegistry, "numTypes", RBM(rbreg_numTypes), 0);
  rb_define_method(cLWCRegistry, "getTypeName", RBM(rbreg_getTypeName), 1);
  rb_define_method(cLWCRegistry, "hasType", RBM(rbreg_hasType), 1);
  rb_define_method(cLWCRegistry, "getMethods", RBM(rbreg_getMethods), 1);
  rb_define_method(cLWCRegistry, "create", RBM(rbreg_create), 1);
  rb_define_method(cLWCRegistry, "get", RBM(rbreg_get), 1);
  rb_define_method(cLWCRegistry, "destroy", RBM(rbreg_destroy), 1);
  rb_define_method(cLWCRegistry, "docString", RBM(rbreg_docString), -1);
  rb_define_method(cLWCRegistry, "getDescription", RBM(rbreg_getDesc), 1);
  return true;
}

}


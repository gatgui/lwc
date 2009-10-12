/*

This file is part of lwc.

lwc is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

lwc is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with lwc.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <lwc/ruby/types.h>
#include <lwc/ruby/utils.h>
#include <lwc/ruby/rbobject.h>

namespace rb {

VALUE cLWCRegistry = Qnil;

void rbreg_mark(void *data) {
}

void rbreg_sweep(void *data) {
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
  char *path = RSTRING(spath)->ptr;
  reg->addModulePath(path);
  return self;
}

static VALUE rbreg_numTypes(VALUE self) {
  lwc::Registry *reg = lwc::Registry::Instance();
  if (!reg) {
    rb_raise(rb_eRuntimeError, "lwc::Registry has not yet been initialized");
  }
  return ULONG2NUM(reg->numTypes());
}

static VALUE rbreg_getTypeName(VALUE self, VALUE ridx) {
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

static VALUE rbreg_hasType(VALUE self, VALUE rname) {
  lwc::Registry *reg = lwc::Registry::Instance();
  if (!reg) {
    rb_raise(rb_eRuntimeError, "lwc::Registry has not yet been initialized");
  }
  VALUE sname = rb_check_string_type(rname);
  char *name = RSTRING(sname)->ptr;
  return (reg->hasType(name) ? Qtrue : Qfalse);
}

static VALUE rbreg_getMethods(VALUE self, VALUE rname) {
  lwc::Registry *reg = lwc::Registry::Instance();
  if (!reg) {
    rb_raise(rb_eRuntimeError, "lwc::Registry has not yet been initialized");
  }
  VALUE sname = rb_check_string_type(rname);
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

static VALUE rbreg_create(VALUE self, VALUE rname) {
  lwc::Registry *reg = lwc::Registry::Instance();
  if (!reg) {
    rb_raise(rb_eRuntimeError, "lwc::Registry has not yet been initialized");
  }
  VALUE sname = rb_check_string_type(rname);
  char *name = RSTRING(sname)->ptr;
  lwc::Object *obj = reg->create(name);
  if (!obj) {
    return Qnil;
  } else {
    VALUE rv = Qnil;
    if (!strcmp(obj->getLoaderName(), "rbloader")) {
      rv = ((Object*)obj)->self();
      
    } else {
      rv = rb_funcall2(cLWCObject, rb_intern("new"), 0, NULL);
      SetObjectPointer(rv, obj);
    }
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
  rb_define_method(cLWCRegistry, "destroy", RBM(rbreg_destroy), 1);
  return true;
}

}


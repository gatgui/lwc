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
#include <lwc/ruby/methodcall.h>
#include <lwc/ruby/rbobject.h>

namespace rb {

VALUE cLWCObject = Qnil;

void SetObjectPointer(VALUE robj, lwc::Object *obj) {
  // add methods to object?
  // rb_define_method(robj, "name", RBM(), -1);
  RDATA(robj)->data = (void*) obj;
}

void rbobj_mark(void *) {
}

void rbobj_sweep(void *) {
}

static VALUE rbobj_alloc(VALUE klass) {
  lwc::Object *obj = 0;
  return rb::WrapPointer(klass, obj, rbobj_mark, rbobj_sweep);
}

static VALUE rbobj_init(VALUE self) {
  return self;
}

static VALUE rbobj_getMethod(VALUE self, VALUE rname) {
  lwc::Object *obj = 0;
  rb::Exc::GetPointer(self, obj);
  VALUE sname = rb_check_string_type(rname);
  if (NIL_P(sname)) {
    rb_raise(rb_eTypeError, "RLWC::Object.getMethod expects a string as argument");
  }
  char *mn = RSTRING(sname)->ptr;
  VALUE rv = Qnil;
  try {
    const lwc::Method &m = obj->getMethod(mn);
    rv = rb_funcall2(cLWCMethod, rb_intern("new"), 0, NULL);
    lwc::Method *rm = 0;
    rb::Exc::GetPointer(rv, rm);
    *rm = m;
  } catch (std::runtime_error &e) {
    rb_raise(rb_eRuntimeError, e.what());
  }
  return rv;
}

static VALUE rbobj_availableMethods(VALUE self) {
  lwc::Object *obj = 0;
  rb::Exc::GetPointer(self, obj);
  std::vector<std::string> names;
  obj->availableMethods(names);
  VALUE rv = rb_ary_new2(names.size());
  for (size_t i=0; i<names.size(); ++i) {
    rb_ary_store(rv, i, rb_str_new2(names[i].c_str()));
  }
  return rv;
}

static VALUE rbobj_respondsTo(VALUE self, VALUE rname) {
  lwc::Object *obj = 0;
  rb::Exc::GetPointer(self, obj);
  VALUE sname = rb_check_string_type(rname);
  if (NIL_P(sname)) {
    rb_raise(rb_eTypeError, "RLWC::Object.respondsTo expects a string as argument");
  }
  char *mn = RSTRING(sname)->ptr;
  return (obj->respondsTo(mn) ? Qtrue : Qfalse);
}

static VALUE rbobj_getMethods(VALUE self) {
  lwc::Object *obj = 0;
  rb::Exc::GetPointer(self, obj);
  const lwc::MethodsTable *mt = obj->getMethods();
  if (!mt) {
    return Qnil;
  } else {
    VALUE rv = rb_funcall2(cLWCMethodsTable, rb_intern("new"), 0, NULL);
    RDATA(rv)->data = (void*) mt;
    return rv;
  }
}

static VALUE rbobj_getLoaderName(VALUE self) {
  lwc::Object *obj = 0;
  rb::Exc::GetPointer(self, obj);
  return rb_str_new2(obj->getLoaderName());
}

static VALUE rbobj_getTypeName(VALUE self) {
  lwc::Object *obj = 0;
  rb::Exc::GetPointer(self, obj);
  return rb_str_new2(obj->getTypeName());
}

static VALUE rbobj_mmissing(int argc, VALUE *argv, VALUE self) {
  lwc::Object *obj = 0;
  rb::Exc::GetPointer(self, obj);
  const char *methodName = rb_id2name(SYM2ID(argv[0]));
  //if (!strcmp(obj->getLoaderName(), "rbloader")) {
  //  std::cout << "  Is a ruby object" << std::endl;
  //  return rb_funcall2(((RbObject*)obj)->self(), rb_intern(methodName), argc-1, argv+1);
  //  
  //} else {
    // DO THIS IN A TRY...CATCH -> Method might be missing
    try {
      const lwc::Method &m = obj->getMethod(methodName);
      std::map<size_t, size_t> arraySizes;
      lwc::MethodParams params(m);
      if (argc-1 == 0) {
        return CallMethod(obj, methodName, params, 0, NULL, 0, 0, arraySizes);
      } else {
        return CallMethod(obj, methodName, params, 0, argv+1, argc-1, 0, arraySizes);
      }
    } catch (std::runtime_error &e) {
      rb_raise(rb_eRuntimeError, e.what());
    }
  //}
}

bool InitObject(VALUE mod) {
  cLWCObject = rb_define_class_under(mod, "Object", rb_cObject);
  rb_define_alloc_func(cLWCObject, rbobj_alloc);
  rb_define_method(cLWCObject, "initialize", RBM(rbobj_init), 0);
  rb_define_method(cLWCObject, "getMethod", RBM(rbobj_getMethod), 1);
  rb_define_method(cLWCObject, "availableMethods", RBM(rbobj_availableMethods), 0);
  rb_define_method(cLWCObject, "respondsTo?", RBM(rbobj_respondsTo), 1);
  rb_define_method(cLWCObject, "getMethods", RBM(rbobj_getMethods), 0);
  rb_define_method(cLWCObject, "getLoaderName", RBM(rbobj_getLoaderName), 0);
  rb_define_method(cLWCObject, "getTypeName", RBM(rbobj_getTypeName), 0);
  rb_define_method(cLWCObject, "method_missing", RBM(rbobj_mmissing), -1);
  return true;
}

}


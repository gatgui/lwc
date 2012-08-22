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

namespace rb {

VALUE cLWCMethodsTable = Qnil;

void rbmtbl_mark(void *) {
}

void rbmtbl_sweep(void *) {
}

static VALUE rbmtbl_alloc(VALUE klass) {
  lwc::MethodsTable *table = 0;
  return rb::WrapPointer(klass, table, rbmtbl_mark, rbmtbl_sweep);
}

static VALUE rbmtbl_init(VALUE self) {
  return self;
}

static VALUE rbmtbl_availables(VALUE self) {
  lwc::MethodsTable *mt = 0;
  rb::Exc::GetPointer(self, mt);
  std::vector<std::string> names;
  mt->availableMethods(names);
  VALUE rv = rb_ary_new2(names.size());
  for (size_t i=0; i<names.size(); ++i) {
    rb_ary_store(rv, i, rb_str_new2(names[i].c_str()));
  }
  return rv;
}

static VALUE rbmtbl_find(VALUE self, VALUE rname) {
  lwc::MethodsTable *mt = 0;
  rb::Exc::GetPointer(self, mt);
  VALUE sname = rb_check_string_type(rname);
  if (NIL_P(sname)) {
    rb_raise(rb_eTypeError, "RLWC::MethodsTable.find expects a string as argument");
  }
  char *mn = RSTRING(sname)->ptr;
  const lwc::Method *m = mt->findMethod(mn);
  if (!m) {
    return Qnil;
  } else {
    VALUE rv = rb_funcall2(cLWCMethod, rb_intern("new"), 0, NULL);
    lwc::Method *rm = 0;
    rb::Exc::GetPointer(rv, rm);
    *rm = *m;
    return rv;
  }
}

static VALUE rbmtbl_count(VALUE self) {
  lwc::MethodsTable *mt = 0;
  rb::Exc::GetPointer(self, mt);
  return ULONG2NUM(mt->numMethods());
}

static VALUE rbmtbl_tos(VALUE self) {
  lwc::MethodsTable *mt = 0;
  rb::Exc::GetPointer(self, mt);
  std::string s = mt->toString();
  return rb_str_new2(s.c_str());
}

static VALUE rbmtbl_docString(int argc, VALUE *argv, VALUE self) {
  if (argc > 1) {
    rb_raise(rb_eArgError, "RLWC::MethodsTable.docString accepts at most 1 argument");
  }
  lwc::MethodsTable *mt;
  rb::Exc::GetPointer(self, mt);
  std::string indent = "";
  if (argc == 1) {
    VALUE sval = rb_check_string_type(argv[0]);
    if (NIL_P(sval)) {
      rb_raise(rb_eTypeError, "RLWC::MethodsTable.docString expects a string as argument");
    }
    indent = RSTRING(sval)->ptr;
  }
  std::string s = mt->docString(indent);
  return rb_str_new2(s.c_str());
}

bool InitMethodsTable(VALUE mod) {
  cLWCMethodsTable = rb_define_class_under(mod, "MethodsTable", rb_cObject);
  rb_define_alloc_func(cLWCMethodsTable, rbmtbl_alloc);
  rb_define_method(cLWCMethodsTable, "initialize", RBM(rbmtbl_init), 0);
  rb_define_method(cLWCMethodsTable, "findMethod", RBM(rbmtbl_find), 1);
  rb_define_method(cLWCMethodsTable, "availableMethods", RBM(rbmtbl_availables), 0);
  rb_define_method(cLWCMethodsTable, "numMethods", RBM(rbmtbl_count), 0);
  rb_define_method(cLWCMethodsTable, "to_s", RBM(rbmtbl_tos), 0);
  rb_define_method(cLWCMethodsTable, "docString", RBM(rbmtbl_docString), -1);
  return true;
}

}


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

VALUE cLWCMethod = Qnil;

void rbmeth_mark(void *) {
}

void rbmeth_sweep(void *data) {
  lwc::Method *meth = (lwc::Method*)data;
  delete meth;
}

static VALUE rbmeth_alloc(VALUE klass) {
  lwc::Method *meth = new lwc::Method();
  return rb::WrapPointer(klass, meth, rbmeth_mark, rbmeth_sweep);
}

static VALUE rbmeth_init(VALUE self) {
  return self;
}

static VALUE rbmeth_numArgs(VALUE self) {
  lwc::Method *meth = 0;
  rb::Exc::GetPointer(self, meth);
  return INT2NUM(meth->numArgs());
}

static VALUE rbmeth_numPositionalArgs(VALUE self) {
  lwc::Method *meth = 0;
  rb::Exc::GetPointer(self, meth);
  return INT2NUM(meth->numPositionalArgs());
}

static VALUE rbmeth_namedArgIndex(VALUE self, VALUE name) {
  lwc::Method *meth = 0;
  rb::Exc::GetPointer(self, meth);
  VALUE sname = rb_check_string_type(name);
  if (NIL_P(sname)) {
    rb_raise(rb_eTypeError, "RLWC::Method.namedArgIndex expects a string as argument");
  }
  try {
    return INT2NUM(meth->namedArgIndex(RSTRING(sname)->ptr));
  } catch (std::exception &e) {
    rb_raise(rb_eRuntimeError, e.what());
    return Qnil;
  }
  
}

static VALUE rbmeth_getDesc(VALUE self) {
  lwc::Method *meth = 0;
  rb::Exc::GetPointer(self, meth);
  const char *desc = meth->getDescription();
  return rb_str_new2(desc ? desc : "");
}

static VALUE rbmeth_setDesc(VALUE self, VALUE desc) {
  lwc::Method *meth = 0;
  rb::Exc::GetPointer(self, meth);
  VALUE sdesc = rb_check_string_type(desc);
  if (NIL_P(sdesc)) {
    rb_raise(rb_eTypeError, "RLWC::Method.description= expects a string as argument");
  }
  meth->setDescription(RSTRING(sdesc)->ptr);
  return self;
}

static VALUE rbmeth_addArg(VALUE self, VALUE rarg) {
  lwc::Method *meth = 0;
  lwc::Argument *arg = 0;
  rb::Exc::GetPointer(self, meth);
  rb::Exc::GetTypedPointer(rarg, arg, cLWCArgument);
  meth->addArg(*arg);
  return self;
}

static VALUE rbmeth_getArg(VALUE self, VALUE ridx) {
  lwc::Method *meth = 0;
  lwc::Argument *arg = 0;
  rb::Exc::GetPointer(self, meth);
  unsigned long idx = NUM2ULONG(ridx);
  if (idx >= meth->numArgs()) {
    rb_raise(rb_eIndexError, "Invalid argument index");
  }
  VALUE rarg = rb_funcall2(cLWCArgument, rb_intern("new"), 0, NULL);
  rb::Exc::GetPointer(rarg, arg);
  *arg = (*meth)[idx];
  return rarg;
}

static VALUE rbmeth_setArg(VALUE self, VALUE ridx, VALUE rarg) {
  lwc::Method *meth = 0;
  lwc::Argument *arg = 0;
  rb::Exc::GetPointer(self, meth);
  unsigned long idx = NUM2ULONG(ridx);
  if (idx >= meth->numArgs()) {
    rb_raise(rb_eIndexError, "Invalid argument index");
  }
  rb::Exc::GetTypedPointer(rarg, arg, cLWCArgument);
  (*meth)[idx] = *arg;
  return self;
}

static VALUE rbmeth_tos(VALUE self) {
  lwc::Method *meth = 0;
  rb::Exc::GetPointer(self, meth);
  std::string s = meth->toString();
  return rb_str_new2(s.c_str());
}

static VALUE rbmeth_docString(int argc, VALUE *argv, VALUE self) {
  if (argc > 1) {
    rb_raise(rb_eArgError, "RLWC::Method.docString accepts at most 1 argument");
  }
  lwc::Method *meth;
  rb::Exc::GetPointer(self, meth);
  std::string indent = "";
  if (argc == 1) {
    VALUE sval = rb_check_string_type(argv[0]);
    if (NIL_P(sval)) {
      rb_raise(rb_eTypeError, "RLWC::Method.docString expects a string as argument");
    }
    indent = RSTRING(sval)->ptr;
  }
  std::string s = meth->docString(indent);
  return rb_str_new2(s.c_str());
}

bool InitMethod(VALUE mod) {
  cLWCMethod = rb_define_class_under(mod, "Method", rb_cObject);
  rb_define_alloc_func(cLWCMethod, rbmeth_alloc);
  rb_define_method(cLWCMethod, "initialize", RBM(rbmeth_init), 0);
  rb_define_method(cLWCMethod, "numArgs", RBM(rbmeth_numArgs), 0);
  rb_define_method(cLWCMethod, "numPositionalArgs", RBM(rbmeth_numPositionalArgs), 0);
  rb_define_method(cLWCMethod, "namedArgIndex", RBM(rbmeth_namedArgIndex), 1);
  rb_define_method(cLWCMethod, "addArg", RBM(rbmeth_addArg), 1);
  rb_define_method(cLWCMethod, "getArg", RBM(rbmeth_getArg), 1);
  rb_define_method(cLWCMethod, "setArg", RBM(rbmeth_setArg), 2);
  rb_define_method(cLWCMethod, "to_s", RBM(rbmeth_tos), 0);
  rb_define_method(cLWCMethod, "description", RBM(rbmeth_getDesc), 0);
  rb_define_method(cLWCMethod, "description=", RBM(rbmeth_setDesc), 1);
  rb_define_method(cLWCMethod, "docString", RBM(rbmeth_docString), -1);
  return true;
}

}


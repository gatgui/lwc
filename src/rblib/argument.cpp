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

#include <lwc/ruby/types.h>
#include <lwc/ruby/utils.h>

namespace rb {

VALUE cLWCArgument = Qnil;

void rbarg_mark(void *data) {
}

void rbarg_sweep(void *data) {
  lwc::Argument *arg = (lwc::Argument*)data;
  delete arg;
}

static VALUE rbarg_alloc(VALUE klass) {
  lwc::Argument *arg = new lwc::Argument();
  return rb::WrapPointer(klass, arg, rbarg_mark, rbarg_sweep);
}

static VALUE rbarg_init(VALUE self) {
  // should review this one
  return self;
}

static VALUE rbarg_isConst(VALUE self) {
  lwc::Argument *arg;
  rb::Exc::GetPointer(self, arg);
  return (arg->isConst() ? Qtrue : Qfalse);
}

static VALUE rbarg_setConst(VALUE self, VALUE val) {
  lwc::Argument *arg;
  rb::Exc::GetPointer(self, arg);
  arg->setConst(val == Qnil);
  return self;
}

static VALUE rbarg_isArray(VALUE self) {
  lwc::Argument *arg;
  rb::Exc::GetPointer(self, arg);
  return (arg->isArray() ? Qtrue : Qfalse);
}

static VALUE rbarg_setArray(VALUE self, VALUE val) {
  lwc::Argument *arg;
  rb::Exc::GetPointer(self, arg);
  arg->setArray(val == Qnil);
  return self;
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

static VALUE rbarg_getArySzArg(VALUE self) {
  lwc::Argument *arg;
  rb::Exc::GetPointer(self, arg);
  return INT2NUM(arg->arraySizeArg());
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
  return INT2NUM(arg->arrayArg());
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

bool InitArgument(VALUE mod) {
  
  cLWCArgument = rb_define_class_under(mod, "Argument", rb_cObject);
  rb_define_alloc_func(cLWCArgument, rbarg_alloc);
  rb_define_method(cLWCArgument, "initialize", RBM(rbarg_init), 0);
  rb_define_method(cLWCArgument, "const?", RBM(rbarg_isConst), 0);
  rb_define_method(cLWCArgument, "const=", RBM(rbarg_setConst), 1);
  rb_define_method(cLWCArgument, "array?", RBM(rbarg_isArray), 0);
  rb_define_method(cLWCArgument, "array=", RBM(rbarg_setArray), 1);
  rb_define_method(cLWCArgument, "dir", RBM(rbarg_getDir), 0);
  rb_define_method(cLWCArgument, "dir=", RBM(rbarg_setDir), 1);
  rb_define_method(cLWCArgument, "type", RBM(rbarg_getType), 0);
  rb_define_method(cLWCArgument, "type=", RBM(rbarg_setType), 1);
  rb_define_method(cLWCArgument, "arraySizeArg", RBM(rbarg_getArySzArg), 0);
  rb_define_method(cLWCArgument, "arraySizeArg=", RBM(rbarg_setArySzArg), 1);
  rb_define_method(cLWCArgument, "arrayArg", RBM(rbarg_getAryArg), 0);
  rb_define_method(cLWCArgument, "arrayArg=", RBM(rbarg_setAryArg), 1);
  rb_define_method(cLWCArgument, "to_s", RBM(rbarg_tos), 0);
  
  rb_define_const(mod, "AD_IN", INT2NUM(lwc::AD_IN));
  rb_define_const(mod, "AD_INOUT", INT2NUM(lwc::AD_INOUT));
  rb_define_const(mod, "AD_OUT", INT2NUM(lwc::AD_OUT));
  rb_define_const(mod, "AD_RETURN", INT2NUM(lwc::AD_RETURN));
  
  rb_define_const(mod, "AT_UNKNOWN", INT2NUM(lwc::AT_UNKNOWN));
  rb_define_const(mod, "AT_BOOL", INT2NUM(lwc::AT_BOOL));
  rb_define_const(mod, "AT_CHAR", INT2NUM(lwc::AT_CHAR));
  rb_define_const(mod, "AT_UCHAR", INT2NUM(lwc::AT_UCHAR));
  rb_define_const(mod, "AT_SHORT", INT2NUM(lwc::AT_SHORT));
  rb_define_const(mod, "AT_USHORT", INT2NUM(lwc::AT_USHORT));
  rb_define_const(mod, "AT_INT", INT2NUM(lwc::AT_INT));
  rb_define_const(mod, "AT_UINT", INT2NUM(lwc::AT_UINT));
  rb_define_const(mod, "AT_LONG", INT2NUM(lwc::AT_LONG));
  rb_define_const(mod, "AT_ULONG", INT2NUM(lwc::AT_ULONG));
  rb_define_const(mod, "AT_FLOAT", INT2NUM(lwc::AT_FLOAT));
  rb_define_const(mod, "AT_DOUBLE", INT2NUM(lwc::AT_DOUBLE));
  rb_define_const(mod, "AT_STRING", INT2NUM(lwc::AT_STRING));
  rb_define_const(mod, "AT_OBJECT", INT2NUM(lwc::AT_OBJECT));
  
  return true;
}

}


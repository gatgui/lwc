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

namespace rb {

VALUE mLWC = Qnil;

static VALUE rbil_init(VALUE mod) {
  lwc::Registry::Initialize("ruby", NULL);
  rb::Tracker::Init();
  return rb_funcall2(cLWCRegistry, rb_intern("new"), 0, NULL);
}

static VALUE rbil_getreg(VALUE mod) {
  if (lwc::Registry::Instance()) {
    return rb_funcall2(cLWCRegistry, rb_intern("new"), 0, NULL);
  } else {
    return Qnil;
  }
}

static VALUE rbil_deinit(VALUE mod) {
  lwc::Registry::DeInitialize();
  rb::Tracker::Cleanup();
  return Qnil;
}

void CreateModule() {
  mLWC = rb_define_module("RLWC");
  
  rb_define_module_function(mLWC, "Initialize", RBM(rbil_init), 0);
  rb_define_module_function(mLWC, "DeInitialize", RBM(rbil_deinit), 0);
  rb_define_module_function(mLWC, "GetRegistry", RBM(rbil_getreg), 0);
  
  if (!InitArgument(mLWC)) {
    rb_raise(rb_eRuntimeError, "Could not initialize RLWC::Argument class");
    return;
  }
  if (!InitMethod(mLWC)) {
    rb_raise(rb_eRuntimeError, "Could not initialize RLWC::Method class");
    return;
  }
  if (!InitMethodsTable(mLWC)) {
    rb_raise(rb_eRuntimeError, "Could not initialize RLWC::MethodsTable class");
    return;
  }
  if (!InitObject(mLWC)) {
    rb_raise(rb_eRuntimeError, "Could not initialize RLWC::Object class");
    return;
  }
  if (!InitRegistry(mLWC)) {
    rb_raise(rb_eRuntimeError, "Could not initialize RLWC::Registry class");
    return;
  }
}


}


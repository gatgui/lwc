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

#include <lwc/python/types.h>

namespace py {

// This should not be called for Python objects
void SetObjectPointer(PyLWCObject *self, lwc::Object *o) {
  std::vector<std::string> methods;
  
  if (!o) {
    return;
  }
  
  self->obj = o;
  
  size_t n = o->availableMethods(methods);
  
  for (size_t i=0; i<n; ++i) {
    PyObject *omethod = PyObject_CallObject((PyObject*)&PyLWCMethodCallType, NULL);

    PyLWCMethodCall *method = (PyLWCMethodCall*) omethod;
    method->obj = self->obj;
    size_t len = methods[i].length();
    method->method = (char*) malloc(len+1);
    strcpy(method->method, methods[i].c_str());
  
    self->methods[methods[i]] = omethod;
  }
}

}


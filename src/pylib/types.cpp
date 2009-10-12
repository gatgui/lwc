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


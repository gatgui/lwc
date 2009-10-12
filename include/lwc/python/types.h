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

#ifndef __lwc_py_types_h__
#define __lwc_py_types_h__

#include <lwc/python/config.h>

namespace py {

  struct LWCPY_API PyLWCArgument {
    PyObject_HEAD
    lwc::Argument arg;
  };

  struct LWCPY_API PyLWCMethod {
    PyObject_HEAD
    lwc::Method meth;
    bool own;
  };

  struct LWCPY_API PyLWCObject {
    PyObject_HEAD
    lwc::Object *obj;
    std::map<std::string, PyObject*> methods;
  };

  struct LWCPY_API PyLWCMethodCall {
    PyObject_HEAD
    lwc::Object *obj;
    char *method;
  };

  struct LWCPY_API PyLWCMethodsTable {
    PyObject_HEAD
    lwc::MethodsTable *table;
  };

  struct LWCPY_API PyLWCRegistry {
    PyObject_HEAD
  };

  LWCPY_DATA_API PyTypeObject PyLWCArgumentType;
  LWCPY_DATA_API PyTypeObject PyLWCMethodType;
  LWCPY_DATA_API PyTypeObject PyLWCRegistryType;
  LWCPY_DATA_API PyTypeObject PyLWCObjectType;
  LWCPY_DATA_API PyTypeObject PyLWCMethodCallType;
  LWCPY_DATA_API PyTypeObject PyLWCMethodsTableType;


  LWCPY_API void SetObjectPointer(PyLWCObject *self, lwc::Object *o);
  LWCPY_API bool InitArgument(PyObject *);
  LWCPY_API bool InitMethod(PyObject *);
  LWCPY_API bool InitMethodsTable(PyObject *);
  LWCPY_API bool InitMethodCall(PyObject *);
  LWCPY_API bool InitObject(PyObject *);
  LWCPY_API bool InitRegistry(PyObject *);
  LWCPY_API PyObject* CreateModule();

}

#endif




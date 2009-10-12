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

static PyObject* lwc_init(PyObject *m, PyObject *args) {
  lwc::Registry::Initialize("python", NULL);
  return PyObject_CallObject((PyObject*)&PyLWCRegistryType, NULL);
}

static PyObject* lwc_getreg(PyObject *m, PyObject *args) {
  if (lwc::Registry::Instance()) {
    return PyObject_CallObject((PyObject*)&PyLWCRegistryType, NULL);
  } else {
    Py_INCREF(Py_None);
    return Py_None;
  }
}

static PyObject* lwc_deinit(PyObject *m, PyObject *args) {
  lwc::Registry::DeInitialize();
  Py_INCREF(Py_None);
  return Py_None;
}

static PyMethodDef lwc_methods[] = {
  {"Initialize", lwc_init, METH_VARARGS, "Initialize LWC"},
  {"GetRegistry", lwc_getreg, METH_VARARGS, "Get LWC registry"},
  {"DeInitialize", lwc_deinit, METH_VARARGS, "DeInitialize LWC"},
  NULL
};

PyObject* CreateModule() {
  
  PyObject *m = Py_InitModule3("lwcpy", lwc_methods, "");
  
  if (!InitArgument(m)) {
    PyErr_SetString(PyExc_RuntimeError, "Could not intialize lwcpy.Argument class");
    Py_DECREF(m);
    return 0;
  }
  if (!InitMethod(m)) {
    PyErr_SetString(PyExc_RuntimeError, "Could not intialize lwcpy.Method class");
    Py_DECREF(m);
    return 0;
  }
  if (!InitMethodCall(m)) {
    PyErr_SetString(PyExc_RuntimeError, "Could not intialize lwcpy.MethodCall class");
    Py_DECREF(m);
    return 0;
  }
  if (!InitMethodsTable(m)) {
    PyErr_SetString(PyExc_RuntimeError, "Could not intialize lwcpy.MethodsTable class");
    Py_DECREF(m);
    return 0;
  }
  if (!InitObject(m)) {
    PyErr_SetString(PyExc_RuntimeError, "Could not intialize lwcpy.Object class");
    Py_DECREF(m);
    return 0;
  }
  if (!InitRegistry(m)) {
    PyErr_SetString(PyExc_RuntimeError, "Could not intialize lwcpy.Registry class");
    Py_DECREF(m);
    return 0;
  }
  
  return m;
}

}


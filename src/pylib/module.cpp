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

#include <lwc/python/types.h>

namespace py {

static PyObject* lwc_init(PyObject *, PyObject *) {
  lwc::Registry::Initialize("python", NULL);
  return PyObject_CallObject((PyObject*)&PyLWCRegistryType, NULL);
}

static PyObject* lwc_getreg(PyObject *, PyObject *) {
  if (lwc::Registry::Instance()) {
    return PyObject_CallObject((PyObject*)&PyLWCRegistryType, NULL);
  } else {
    Py_INCREF(Py_None);
    return Py_None;
  }
}

static PyObject* lwc_deinit(PyObject *, PyObject *) {
  lwc::Registry::DeInitialize();
  Py_INCREF(Py_None);
  return Py_None;
}

static PyMethodDef lwc_methods[] = {
  {"Initialize", lwc_init, METH_VARARGS, "Initialize LWC"},
  {"GetRegistry", lwc_getreg, METH_VARARGS, "Get LWC registry"},
  {"DeInitialize", lwc_deinit, METH_VARARGS, "DeInitialize LWC"},
  {NULL, NULL, 0, NULL}
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


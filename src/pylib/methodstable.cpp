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

PyTypeObject PyLWCMethodsTableType = {
  PyObject_HEAD_INIT(NULL)
  0,
  "lwcpy.MethodsTable",
  sizeof(PyLWCMethodsTable)
};

// ---

// do not wrap "addMethod" and "fromDeclaration"
// as we cannot create a MethodsTable from scratch in python

static PyObject* lwcmtbl_new(PyTypeObject *type, PyObject *, PyObject *) {
  PyLWCMethodsTable *self = (PyLWCMethodsTable*) type->tp_alloc(type, 0);
  self->table = 0;
  return (PyObject*)self;
}

static int lwcmtbl_init(PyObject *, PyObject *, PyObject *) {
  return 0;
}

static void lwcmtbl_free(PyObject *pself) {
  pself->ob_type->tp_free(pself);
}

static PyObject* lwcmtbl_availableMethods(PyObject *pself, PyObject *) {
  PyLWCMethodsTable *self = (PyLWCMethodsTable*) pself;
  if (!self->table) {
    PyErr_SetString(PyExc_RuntimeError, "Underlying object does not exist");
    return NULL;
  }
  std::vector<std::string> names;
  self->table->availableMethods(names);
  PyObject *rv = PyList_New(names.size());
  for (size_t i=0; i<names.size(); ++i) {
    PyList_SetItem(rv, i, PyString_FromString(names[i].c_str()));
  }
  return rv;
}

static PyObject* lwcmtbl_find(PyObject *pself, PyObject *args) {
  PyLWCMethodsTable *self = (PyLWCMethodsTable*) pself;
  if (!self->table) {
    PyErr_SetString(PyExc_RuntimeError, "Underlying object does not exist");
    return NULL;
  }
  char *name;
  if (!PyArg_ParseTuple(args, "s", &name)) {
    return NULL;
  }
  const lwc::Method *m = self->table->findMethod(name);
  if (!m) {
    Py_INCREF(Py_None);
    return Py_None;
  } else {
    PyObject *rv = PyObject_CallObject((PyObject*)&PyLWCMethodType, NULL);
    ((PyLWCMethod*)rv)->meth = *m;
    return rv;
  }
}

static PyObject* lwcmtbl_str(PyObject *pself) {
  PyLWCMethodsTable *self = (PyLWCMethodsTable*) pself;
  if (!self->table) {
    PyErr_SetString(PyExc_RuntimeError, "Underlying object does not exist");
    return NULL;
  }
  return PyString_FromString(self->table->toString().c_str());
}

static PyObject* lwcmtbl_nummeth(PyObject *pself, PyObject *) {
  PyLWCMethodsTable *self = (PyLWCMethodsTable*) pself;
  if (!self->table) {
    PyErr_SetString(PyExc_RuntimeError, "Underlying object does not exist");
    return NULL;
  }
  return PyInt_FromLong(self->table->numMethods());
}

static PyMethodDef lwcmtbl_methods[] = {
  {"availableMethods", lwcmtbl_availableMethods, METH_VARARGS, "Get names of available methods"},
  {"findMethod", lwcmtbl_find, METH_VARARGS, "Find method in table"},
  {"numMethods", lwcmtbl_nummeth, METH_VARARGS, "Get methods count"},
  {NULL}
};

// ---

bool InitMethodsTable(PyObject *m) {

  PyLWCMethodsTableType.tp_flags = Py_TPFLAGS_DEFAULT;
  PyLWCMethodsTableType.tp_doc = "Methods table class";
  PyLWCMethodsTableType.tp_new = lwcmtbl_new;
  PyLWCMethodsTableType.tp_init = lwcmtbl_init;
  PyLWCMethodsTableType.tp_dealloc = lwcmtbl_free;
  PyLWCMethodsTableType.tp_methods = lwcmtbl_methods;
  PyLWCMethodsTableType.tp_str = lwcmtbl_str;
  if (PyType_Ready(&PyLWCMethodsTableType) < 0) {
    return false;
  }
  
  Py_INCREF(&PyLWCMethodsTableType);
  PyModule_AddObject(m, "MethodsTable", (PyObject*)&PyLWCMethodsTableType);
  
  return true;
}

}


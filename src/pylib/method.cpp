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

PyTypeObject PyLWCMethodType;

// ---

// lwcpy.Method takes copies of the method... not good

static PyObject* lwcmeth_new(PyTypeObject *type, PyObject *, PyObject *) {
  PyLWCMethod *self = (PyLWCMethod*) type->tp_alloc(type, 0);
  new (&(self->meth)) lwc::Method();
  return (PyObject*)self;
}

static int lwcmeth_init(PyObject *, PyObject *, PyObject *) {
  return 0;
}

static void lwcmeth_free(PyObject *pself) {
  PyLWCMethod *self = (PyLWCMethod*) pself;
  (&(self->meth))->~Method();
  pself->ob_type->tp_free(pself);
}

static PyObject* lwcmeth_numArgs(PyObject *pself, PyObject *) {
  PyLWCMethod *self = (PyLWCMethod*) pself;
  return PyInt_FromLong(self->meth.numArgs());
}

static PyObject* lwcmeth_numPositionalArgs(PyObject *pself, PyObject *) {
  PyLWCMethod *self = (PyLWCMethod*) pself;
  return PyInt_FromLong(self->meth.numPositionalArgs());
}

static PyObject* lwcmeth_namedArgIndex(PyObject *pself, PyObject *args) {
  PyLWCMethod *self = (PyLWCMethod*) pself;
  char *name;
  if (!PyArg_ParseTuple(args, "s", &name)) {
    return NULL;
  }
  try {
    size_t n = self->meth.namedArgIndex(name);
    return PyInt_FromLong(n);
  } catch (std::exception &e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
    return NULL;
  }
}

static PyObject* lwcmeth_addArg(PyObject *pself, PyObject *args) {
  PyLWCMethod *self = (PyLWCMethod*) pself;
  PyObject *oarg;
  if (!PyArg_ParseTuple(args, "O", &oarg)) {
    return NULL;
  }
  if (!PyObject_TypeCheck(oarg, &PyLWCArgumentType)) {
    PyErr_SetString(PyExc_RuntimeError, "Expected argument of type lwcpy.Argument");
    return NULL;
  }
  PyLWCArgument *arg = (PyLWCArgument*) oarg;
  self->meth.addArg(arg->arg);
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* lwcmeth_getArg(PyObject *pself, PyObject *args) {
  PyLWCMethod *self = (PyLWCMethod*) pself;
  unsigned long idx;
  if (!PyArg_ParseTuple(args, "k", &idx)) {
    return NULL;
  }
  if (idx >= self->meth.numArgs()) {
    PyErr_Format(PyExc_RuntimeError, "Invalid argument index: %lu", idx);
    return NULL;
  }
  PyObject *oarg = PyObject_CallObject((PyObject*)&PyLWCArgumentType, NULL);
  PyLWCArgument *arg = (PyLWCArgument*) oarg;
  arg->arg = self->meth[idx];
  return oarg;
}

static PyObject* lwcmeth_setArg(PyObject *pself, PyObject *args) {
  PyLWCMethod *self = (PyLWCMethod*) pself;
  unsigned int idx;
  PyObject *oarg;
  if (!PyArg_ParseTuple(args, "kO", &idx, &oarg)) {
    return NULL;
  }
  if (!PyObject_TypeCheck(oarg, &PyLWCArgumentType)) {
    PyErr_SetString(PyExc_RuntimeError, "Expected second argument of type lwcpy.Argument");
    return NULL;
  }
  if (idx >= self->meth.numArgs()) {
    PyErr_Format(PyExc_RuntimeError, "Invalid argument index: %d", idx);
    return NULL;
  }
  PyLWCArgument *arg = (PyLWCArgument*) oarg;
  self->meth[idx] = arg->arg;
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* lwcmeth_str(PyObject *pself) {
  PyLWCMethod *self = (PyLWCMethod*) pself;
  return PyString_FromString(self->meth.toString().c_str());
}

static PyObject* lwcmeth_docString(PyObject *pself, PyObject *, PyObject *kwargs) {
  PyLWCMethod *self = (PyLWCMethod*) pself;
  std::string indent = "";
  PyObject *pIndent = PyDict_GetItemString(kwargs, "indent");
  if (pIndent) {
    if (!PyString_Check(pIndent)) {
      PyErr_SetString(PyExc_RuntimeError, "string value expected fror \"indent\" keyword");
      return NULL;
    }
    indent = PyString_AsString(pIndent);
  }
  return PyString_FromString(self->meth.docString(indent).c_str());
}

static PyObject* lwcmeth_getDesc(PyObject *pself, void*) {
  PyLWCMethod *self = (PyLWCMethod*) pself;
  const char *desc = self->meth.getDescription();
  return PyString_FromString((desc ? desc : "")); 
}

static int lwcmeth_setDesc(PyObject *pself, PyObject *val, void*) {
  PyLWCMethod *self = (PyLWCMethod*) pself;
  char *desc = PyString_AsString(val);
  self->meth.setDescription(desc);
  return 0;
}

static PyGetSetDef lwcmeth_getset[] = {
  {"description", lwcmeth_getDesc, lwcmeth_setDesc, "Method description", NULL},
  {NULL, NULL, NULL, NULL, NULL}
};

static PyMethodDef lwcmeth_methods[] = {
  {"numArgs", lwcmeth_numArgs, METH_VARARGS, "Get method argument count"},
  {"numPositionalArgs", lwcmeth_numPositionalArgs, METH_VARARGS, "Get method positional argument count"},
  {"namedArgIndex", lwcmeth_namedArgIndex, METH_VARARGS, "Get method named argument index"},
  {"addArg", lwcmeth_addArg, METH_VARARGS, "Add method argument"},
  {"setArg", lwcmeth_setArg, METH_VARARGS, "Set method argument at position"},
  {"getArg", lwcmeth_getArg, METH_VARARGS, "Get argument at position"},
  {"docString", (PyCFunction) lwcmeth_docString, METH_VARARGS|METH_KEYWORDS, "Get method documentation string"},
  {NULL, NULL, 0, NULL},
};

// ---

bool InitMethod(PyObject *m) {
  
  memset(&PyLWCMethodType, 0, sizeof(PyTypeObject));
  PyLWCMethodType.ob_refcnt = 1;
  PyLWCMethodType.ob_size = 0;
  PyLWCMethodType.tp_name = "lwcpy.Method";
  PyLWCMethodType.tp_basicsize = sizeof(PyLWCMethod);
  PyLWCMethodType.tp_flags = Py_TPFLAGS_DEFAULT;
  PyLWCMethodType.tp_doc = "Method class";
  PyLWCMethodType.tp_new = lwcmeth_new;
  PyLWCMethodType.tp_init = lwcmeth_init;
  PyLWCMethodType.tp_dealloc = lwcmeth_free;
  PyLWCMethodType.tp_methods = lwcmeth_methods;
  PyLWCMethodType.tp_getset = lwcmeth_getset;
  PyLWCMethodType.tp_str = lwcmeth_str;
  if (PyType_Ready(&PyLWCMethodType) < 0) {
    return false;
  }
  
  Py_INCREF((PyObject*) &PyLWCMethodType);
  PyModule_AddObject(m, "Method", (PyObject*)&PyLWCMethodType);
  
  return true;
}

}


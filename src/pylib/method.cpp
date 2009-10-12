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

PyTypeObject PyLWCMethodType = {
  PyObject_HEAD_INIT(NULL)
  0,
  "lwcpy.Method",
  sizeof(PyLWCMethod)
};

// ---

// lwcpy.Method takes copies of the method... not good

static PyObject* lwcmeth_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
  PyLWCMethod *self = (PyLWCMethod*) type->tp_alloc(type, 0);
  new (&(self->meth)) lwc::Method();
  return (PyObject*)self;
}

static int lwcmeth_init(PyObject *self, PyObject *args, PyObject *kwargs) {
  return 0;
}

static void lwcmeth_free(PyObject *pself) {
  PyLWCMethod *self = (PyLWCMethod*) pself;
  (&(self->meth))->~Method();
  pself->ob_type->tp_free(pself);
}

static PyObject* lwcmeth_numArgs(PyObject *pself, PyObject *args) {
  PyLWCMethod *self = (PyLWCMethod*) pself;
  return PyInt_FromLong(self->meth.numArgs());
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
    PyErr_Format(PyExc_RuntimeError, "Invalid argument index: %d", idx);
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

static PyMethodDef lwcmeth_methods[] = {
  {"numArgs", lwcmeth_numArgs, METH_VARARGS, "Get method argument count"},
  {"addArg", lwcmeth_addArg, METH_VARARGS, "Add method argument"},
  {"setArg", lwcmeth_setArg, METH_VARARGS, "Set method argument at position"},
  {"getArg", lwcmeth_getArg, METH_VARARGS, "Get argument at position"},
  NULL
};

// ---

bool InitMethod(PyObject *m) {
  
  PyLWCMethodType.tp_flags = Py_TPFLAGS_DEFAULT;
  PyLWCMethodType.tp_doc = "Method class";
  PyLWCMethodType.tp_new = lwcmeth_new;
  PyLWCMethodType.tp_init = lwcmeth_init;
  PyLWCMethodType.tp_dealloc = lwcmeth_free;
  PyLWCMethodType.tp_methods = lwcmeth_methods;
  PyLWCMethodType.tp_str = lwcmeth_str;
  if (PyType_Ready(&PyLWCMethodType) < 0) {
    return false;
  }
  
  Py_INCREF(&PyLWCMethodType);
  PyModule_AddObject(m, "Method", (PyObject*)&PyLWCMethodType);
  
  return true;
}

}


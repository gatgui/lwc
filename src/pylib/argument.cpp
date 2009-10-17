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

PyTypeObject PyLWCArgumentType = {
  PyObject_HEAD_INIT(NULL)
  0,
  "lwcpy.Argument",
  sizeof(PyLWCArgument)
};

// ---

static PyObject* lwcarg_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
  PyLWCArgument *self = (PyLWCArgument*) type->tp_alloc(type, 0);
  new (&(self->arg)) lwc::Argument();
  return (PyObject*)self;
}

static int lwcarg_init(PyObject *pself, PyObject *args, PyObject *kwargs) {
  Py_ssize_t nargs = PyTuple_Size(args);
  if (nargs == 2) {
    PyLWCArgument *self = (PyLWCArgument*) pself;
    if (!PyInt_Check(PyTuple_GetItem(args,0)) ||
        !PyInt_Check(PyTuple_GetItem(args,1))) {
      PyErr_SetString(PyExc_RuntimeError, "lwcpy.Argument.__init__: first 2 arguments must be constant integers");
      return -1;
    }
    lwc::Direction dir = lwc::Direction(PyInt_AsLong(PyTuple_GetItem(args,0)));
    lwc::Type typ = lwc::Type(PyInt_AsLong(PyTuple_GetItem(args,1)));
    self->arg.setType(typ);
    self->arg.setDir(dir);
    if (self->arg.isArray()) {
      PyObject *val = 0;
      val = PyDict_GetItemString(kwargs, "arraySizeArg");
      if (!val) {
        PyErr_SetString(PyExc_RuntimeError, "lwcpy.Argument.__init__: expected \"arraySizeArg\" keyword arguments for array type argument");
        return -1;
      }
      if (!PyInt_Check(val)) {
        PyErr_SetString(PyExc_RuntimeError, "lwcpy.Argument.__init__: expected integer value for \"arraySizeArg\" keyword");
        return -1;
      }
      self->arg.setArraySizeArg(PyInt_AsLong(val));
    }
    
  } else if (nargs != 0) {
    PyErr_SetString(PyExc_RuntimeError, "lwcpy.Argument.__init__: 0 or 2 arguments required");
    return -1;
  }
  return 0;
}

static void lwcarg_free(PyObject *pself) {
  PyLWCArgument *self = (PyLWCArgument*) pself;
  (&(self->arg))->~Argument();
  pself->ob_type->tp_free(pself);
}

static PyObject* lwcarg_getArray(PyObject *pself, void *) {
  PyLWCArgument *self = (PyLWCArgument*)pself;
  if (self->arg.isArray()) {
    Py_INCREF(Py_True);
    return Py_True;
  } else {
    Py_INCREF(Py_False);
    return Py_False;
  }
}

static PyObject* lwcarg_getType(PyObject *pself, void *) {
  PyLWCArgument *self = (PyLWCArgument*)pself;
  return PyInt_FromLong(self->arg.getType());
}

static PyObject* lwcarg_getDir(PyObject *pself, void *) {
  PyLWCArgument *self = (PyLWCArgument*)pself;
  return PyInt_FromLong(self->arg.getDir());
}

static PyObject* lwcarg_getArraySizeArg(PyObject *pself, void *) {
  PyLWCArgument *self = (PyLWCArgument*)pself;
  return PyInt_FromLong(self->arg.arraySizeArg());
}

static PyObject* lwcarg_getArrayArg(PyObject *pself, void *) {
  PyLWCArgument *self = (PyLWCArgument*)pself;
  return PyInt_FromLong(self->arg.arrayArg());
}

static int lwcarg_setType(PyObject *pself, PyObject *val, void *) {
  PyLWCArgument *self = (PyLWCArgument*)pself;
  self->arg.setType(lwc::Type(PyInt_AsLong(val)));
  return 0;
}

static int lwcarg_setDir(PyObject *pself, PyObject *val, void *) {
  PyLWCArgument *self = (PyLWCArgument*)pself;
  self->arg.setDir(lwc::Direction(PyInt_AsLong(val)));
  return 0;
}

static int lwcarg_setArray(PyObject *pself, PyObject *val, void *) {
  PyErr_SetString(PyExc_RuntimeError, "\"array\" attribute is not setable");
  return -1;
}

static int lwcarg_setArraySizeArg(PyObject *pself, PyObject *val, void *) {
  PyLWCArgument *self = (PyLWCArgument*)pself;
  self->arg.setArraySizeArg(PyInt_AsLong(val));
  return 0;
}

static int lwcarg_setArrayArg(PyObject *pself, PyObject *val, void *) {
  PyLWCArgument *self = (PyLWCArgument*)pself;
  self->arg.setArrayArg(PyInt_AsLong(val));
  return 0;
}

static PyObject* lwcarg_str(PyObject *pself) {
  PyLWCArgument *self = (PyLWCArgument*) pself;
  return PyString_FromString(self->arg.toString().c_str());
}

static PyGetSetDef lwcarg_getset[] = {
  {"array", lwcarg_getArray, lwcarg_setArray, "Is argument array", NULL},
  {"dir", lwcarg_getDir, lwcarg_setDir, "Argument direction", NULL},
  {"type", lwcarg_getType, lwcarg_setType, "Argument type", NULL},
  {"arraySizeArg", lwcarg_getArraySizeArg, lwcarg_setArraySizeArg, "Index of argument containing array size", NULL},
  {"arrayArg", lwcarg_getArrayArg, lwcarg_setArrayArg, "Index or refering array", NULL},
  NULL
};

// ---

bool InitArgument(PyObject *m) {
  
  PyModule_AddIntConstant(m, "AT_UNKNOWN", lwc::AT_UNKNOWN);
  PyModule_AddIntConstant(m, "AT_BOOL", lwc::AT_BOOL);
  PyModule_AddIntConstant(m, "AT_INT", lwc::AT_INT);
  PyModule_AddIntConstant(m, "AT_REAL", lwc::AT_REAL);
  PyModule_AddIntConstant(m, "AT_STRING", lwc::AT_STRING);
  PyModule_AddIntConstant(m, "AT_OBJECT", lwc::AT_OBJECT);
  PyModule_AddIntConstant(m, "AT_BOOL_ARRAY", lwc::AT_BOOL_ARRAY);
  PyModule_AddIntConstant(m, "AT_INT_ARRAY", lwc::AT_INT_ARRAY);
  PyModule_AddIntConstant(m, "AT_REAL_ARRAY", lwc::AT_REAL_ARRAY);
  PyModule_AddIntConstant(m, "AT_STRING_ARRAY", lwc::AT_STRING_ARRAY);
  PyModule_AddIntConstant(m, "AT_OBJECT_ARRAY", lwc::AT_OBJECT_ARRAY);
  
  PyModule_AddIntConstant(m, "AD_IN", lwc::AD_IN);
  PyModule_AddIntConstant(m, "AD_INOUT", lwc::AD_INOUT);
  PyModule_AddIntConstant(m, "AD_OUT", lwc::AD_OUT);
  
  PyLWCArgumentType.tp_flags = Py_TPFLAGS_DEFAULT;
  PyLWCArgumentType.tp_doc = "Method argument class";
  PyLWCArgumentType.tp_new = lwcarg_new;
  PyLWCArgumentType.tp_init = lwcarg_init;
  PyLWCArgumentType.tp_dealloc = lwcarg_free;
  PyLWCArgumentType.tp_getset = lwcarg_getset;
  PyLWCArgumentType.tp_str = lwcarg_str;
  if (PyType_Ready(&PyLWCArgumentType) < 0) {
    return false;
  }
  
  Py_INCREF(&PyLWCArgumentType);
  PyModule_AddObject(m, "Argument", (PyObject*)&PyLWCArgumentType);
  
  return true;
}

}


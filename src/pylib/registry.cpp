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
#include <lwc/python/pobject.h>

namespace py {

PyTypeObject PyLWCRegistryType;

// ---

static PyObject* lwcreg_new(PyTypeObject *type, PyObject *, PyObject *) {
  return type->tp_alloc(type, 0);
}

static int lwcreg_init(PyObject *, PyObject *, PyObject *) {
  return 0;
}

static void lwcreg_free(PyObject *pself) {
  pself->ob_type->tp_free(pself);
}

static PyObject* lwcreg_addLoaderPath(PyObject *, PyObject *args) {
  lwc::Registry *reg = lwc::Registry::Instance();
  if (!reg) {
    PyErr_SetString(PyExc_RuntimeError, "lwcpy.Registry has not yet been initialized");
    return NULL;
  }
  const char *path;
  if (!PyArg_ParseTuple(args, "s", &path)) {
    return NULL;
  }
  reg->addLoaderPath(path);
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* lwcreg_addModulePath(PyObject *, PyObject *args) {
  lwc::Registry *reg = lwc::Registry::Instance();
  if (!reg) {
    PyErr_SetString(PyExc_RuntimeError, "lwcpy.Registry has not yet been initialized");
    return NULL;
  }
  const char *path;
  if (!PyArg_ParseTuple(args, "s", &path)) {
    return NULL;
  }
  reg->addModulePath(path);
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* lwcreg_create(PyObject *, PyObject *args) {
  lwc::Registry *reg = lwc::Registry::Instance();
  if (!reg) {
    PyErr_SetString(PyExc_RuntimeError, "lwcpy.Registry has not yet been initialized");
    return NULL;
  }
  const char *name;
  if (!PyArg_ParseTuple(args, "s", &name)) {
    return NULL;
  }
  lwc::Object *o = reg->create(name);
  if (!o) {
    Py_INCREF(Py_None);
    return Py_None;
  } else {
    PyObject *rv = 0;
    if (!strcmp(o->getLoaderName(), "pyloader")) {
      rv = ((Object*)o)->self();
      Py_INCREF(rv);
      
    } else {
      rv = PyObject_CallObject((PyObject*)&PyLWCObjectType, NULL);
      SetObjectPointer((PyLWCObject*)rv, o);
    }
    return rv;
  }
}

static PyObject* lwcreg_destroy(PyObject *, PyObject *args) {
  lwc::Registry *reg = lwc::Registry::Instance();
  if (!reg) {
    PyErr_SetString(PyExc_RuntimeError, "lwcpy.Registry has not yet been initialized");
    return NULL;
  }
  PyObject *oobj;
  if (!PyArg_ParseTuple(args, "O", &oobj)) {
    return NULL;
  }
  if (!PyObject_TypeCheck(oobj, &PyLWCObjectType)) {
    PyErr_SetString(PyExc_RuntimeError, "Expected argument of type lwcpy.Object");
    return NULL;
  }
  PyLWCObject *obj = (PyLWCObject*) oobj;
  if (obj->obj) {
    reg->destroy(obj->obj);
    obj->obj = 0;
  }
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* lwcreg_numTypes(PyObject *, PyObject *) {
  lwc::Registry *reg = lwc::Registry::Instance();
  if (!reg) {
    PyErr_SetString(PyExc_RuntimeError, "lwcpy.Registry has not yet been initialized");
    return NULL;
  }
  return PyInt_FromLong(reg->numTypes());
}

static PyObject* lwcreg_getTypeName(PyObject *, PyObject *args) {
  lwc::Registry *reg = lwc::Registry::Instance();
  if (!reg) {
    PyErr_SetString(PyExc_RuntimeError, "lwcpy.Registry has not yet been initialized");
    return NULL;
  }
  unsigned long idx;
  if (!PyArg_ParseTuple(args, "k", &idx)) {
    return NULL;
  }
  const char *name = reg->getTypeName(idx);
  if (!name) {
    Py_INCREF(Py_None);
    return Py_None;
  } else {
    return PyString_FromString(name);
  }
}

static PyObject* lwcreg_hasType(PyObject *, PyObject *args) {
  lwc::Registry *reg = lwc::Registry::Instance();
  if (!reg) {
    PyErr_SetString(PyExc_RuntimeError, "lwcpy.Registry has not yet been initialized");
    return NULL;
  }
  char *name;
  if (!PyArg_ParseTuple(args, "s", &name)) {
    return NULL;
  }
  if (reg->hasType(name)) {
    Py_INCREF(Py_True);
    return Py_True;
  } else {
    Py_INCREF(Py_False);
    return Py_False;
  }
}

static PyObject* lwcreg_getMethods(PyObject *, PyObject *args) {
  lwc::Registry *reg = lwc::Registry::Instance();
  if (!reg) {
    PyErr_SetString(PyExc_RuntimeError, "lwcpy.Registry has not yet been initialized");
    return NULL;
  }
  char *name;
  if (!PyArg_ParseTuple(args, "s", &name)) {
    return NULL;
  }
  const lwc::MethodsTable *mt = reg->getMethods(name);
  if (!mt) {
    Py_INCREF(Py_None);
    return Py_None;
  } else {
    PyObject *pmt = PyObject_CallObject((PyObject*)&PyLWCMethodsTableType, NULL);
    ((PyLWCMethodsTable*)pmt)->table = (lwc::MethodsTable*)mt;
    return pmt;
  }
}

static PyMethodDef lwcreg_methods[] = {
  {"addLoaderPath", lwcreg_addLoaderPath, METH_VARARGS, "Add path to look loaders for"},
  {"addModulePath", lwcreg_addModulePath, METH_VARARGS, "Add path to look modules for"},
  {"numTypes", lwcreg_numTypes, METH_VARARGS, "Get number or registered types"},
  {"getTypeName", lwcreg_getTypeName, METH_VARARGS, "Get name of the ith type"},
  {"hasType", lwcreg_hasType, METH_VARARGS, "Check if a type is registered"},
  {"getMethods", lwcreg_getMethods, METH_VARARGS, "Get method table of a type"},
  {"create", lwcreg_create, METH_VARARGS, "Create a new object"},
  {"destroy", lwcreg_destroy, METH_VARARGS, "Destroy an object"},
  {NULL, NULL, 0, NULL}
};

// ---

bool InitRegistry(PyObject *m) {
  
  memset(&PyLWCRegistryType, 0, sizeof(PyTypeObject));
  PyLWCRegistryType.ob_refcnt = 1;
  PyLWCRegistryType.ob_size = 0;
  PyLWCRegistryType.tp_name = "lwcpy.Registry";
  PyLWCRegistryType.tp_basicsize = sizeof(PyLWCRegistry);
  PyLWCRegistryType.tp_flags = Py_TPFLAGS_DEFAULT;
  PyLWCRegistryType.tp_doc = "Registry class";
  PyLWCRegistryType.tp_new = lwcreg_new;
  PyLWCRegistryType.tp_init = lwcreg_init;
  PyLWCRegistryType.tp_dealloc = lwcreg_free;
  PyLWCRegistryType.tp_methods = lwcreg_methods;
  if (PyType_Ready(&PyLWCRegistryType) < 0) {
    return false;
  }
  
  Py_INCREF((PyObject*) &PyLWCRegistryType);
  PyModule_AddObject(m, "Registry", (PyObject*)&PyLWCRegistryType);
  
  return true;
}

}


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

PyTypeObject PyLWCObjectType = {
  PyObject_HEAD_INIT(NULL)
  0,
  "lwcpy.Object",
  sizeof(PyLWCObject)
};

// ---

static PyObject* lwcobj_new(PyTypeObject *type, PyObject *, PyObject *) {
  PyLWCObject *self = (PyLWCObject*) type->tp_alloc(type, 0);
  self->obj = 0;
  new (&(self->methods)) std::map<std::string, PyObject*>();
  return (PyObject*)self;
}

static int lwcobj_init(PyObject *, PyObject *, PyObject *) {
  return 0;
}

static void lwcobj_free(PyObject *pself) {
  PyLWCObject *self = (PyLWCObject*) pself;
  std::map<std::string, PyObject*>::iterator it = self->methods.begin();
  while (it != self->methods.end()) {
    Py_DECREF(it->second);
    ++it;
  }
  (&(self->methods))->~map<std::string, PyObject*>();
  pself->ob_type->tp_free(pself);
}

static PyObject* lwcobj_respondsTo(PyObject *pself, PyObject *args) {
  PyLWCObject *self = (PyLWCObject*) pself;
  if (!self->obj) {
    PyErr_SetString(PyExc_RuntimeError, "lwcpy.Object: underlying object does not exists");
    return NULL;
  }
  const char *name = 0;
  if (!PyArg_ParseTuple(args, "s", &name)) {
    return NULL;
  }
  if (self->obj->respondsTo(name)) {
    Py_INCREF(Py_True);
    return Py_True;
  } else {
    Py_INCREF(Py_False);
    return Py_False;
  }
}

static PyObject* lwcobj_availableMethods(PyObject *pself, PyObject *) {
  PyLWCObject *self = (PyLWCObject*) pself;
  if (!self->obj) {
    PyErr_SetString(PyExc_RuntimeError, "lwcpy.Object: underlying object does not exists");
    return NULL;
  }
  std::vector<std::string> names;
  size_t n = self->obj->availableMethods(names);
  PyObject *ml = PyList_New(n);
  for (size_t i=0; i<n; ++i) {
    PyList_SetItem(ml, i, PyString_FromString(names[i].c_str()));
  }
  return ml;
}

static PyObject* lwcobj_getMethod(PyObject *pself, PyObject *args) {
  PyLWCObject *self = (PyLWCObject*) pself;
  if (!self->obj) {
    PyErr_SetString(PyExc_RuntimeError, "lwcpy.Object: underlying object does not exists");
    return NULL;
  }
  const char *name = 0;
  if (!PyArg_ParseTuple(args, "s", &name)) {
    return NULL;
  }
  PyObject *rv = 0;
  try {
    const lwc::Method &m = self->obj->getMethod(name);
    rv = PyObject_CallObject((PyObject*)&PyLWCMethodType, NULL);
    ((PyLWCMethod*)rv)->meth = m;
  } catch (std::runtime_error &e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
    return NULL;
  }
  if (!rv) {
    Py_INCREF(Py_None);
    return Py_None;
  } else {
    return rv;
  }
}

static PyObject* lwcobj_getMethods(PyObject *pself, PyObject *) {
  PyLWCObject *self = (PyLWCObject*) pself;
  if (!self->obj) {
    PyErr_SetString(PyExc_RuntimeError, "lwcpy.Object: underlying object does not exists");
    return NULL;
  }
  const lwc::MethodsTable *mt = self->obj->getMethods();
  PyObject *rv = PyObject_CallObject((PyObject*)&PyLWCMethodsTableType, NULL);
  ((PyLWCMethodsTable*)rv)->table = (lwc::MethodsTable*) mt;
  return rv;
}

static PyObject* lwcobj_getLoaderName(PyObject *pself, PyObject *) {
  PyLWCObject *self = (PyLWCObject*) pself;
  if (!self->obj) {
    PyErr_SetString(PyExc_RuntimeError, "lwcpy.Object: underlying object does not exists");
    return NULL;
  }
  return PyString_FromString(self->obj->getLoaderName());
}

static PyObject* lwcobj_getTypeName(PyObject *pself, PyObject *) {
  PyLWCObject *self = (PyLWCObject*) pself;
  if (!self->obj) {
    PyErr_SetString(PyExc_RuntimeError, "lwcpy.Object: underlying object does not exists");
    return NULL;
  }
  return PyString_FromString(self->obj->getTypeName());
}

static PyMethodDef lwcobj_methods[] = {
  {"getMethod", lwcobj_getMethod, METH_VARARGS, "Get method by name"},
  {"availableMethods", lwcobj_availableMethods, METH_VARARGS, "List available methods"},
  {"respondsTo", lwcobj_respondsTo, METH_VARARGS, "Check if method exists"},
  {"getMethods", lwcobj_getMethods, METH_VARARGS, "Get object methods table"},
  {"getTypeName", lwcobj_getTypeName, METH_VARARGS, "Get object type name"},
  {"getLoaderName", lwcobj_getLoaderName, METH_VARARGS, "Get object loader name"},
  NULL
};

static PyObject* lwcobj_getattr(PyObject *pself, char *name) {
  // This one replace the "call" method
  
  PyObject *os = PyString_FromString(name);
  PyObject *o = PyObject_GenericGetAttr(pself, os);
  
  if (!o) {
    
    PyLWCObject *self = (PyLWCObject*) pself;
    
    if (!self->obj) {
      PyErr_SetString(PyExc_AttributeError, "lwcpy.Object: underlying object does not exists");
      return NULL;
    }
    
    //if (!strcmp(self->obj->getLoaderName(), "pyloader")) {
    //  
    //  PObject *pobj = (PObject*) self->obj;
    //  o = PyObject_GenericGetAttr(pobj->self(), os);
    //
    //} else {
    
      std::map<std::string, PyObject*>::iterator it = self->methods.find(name);
    
      if (it == self->methods.end()) {
        PyErr_Format(PyExc_AttributeError, "lwcpy.Object: no method named \"%s\"", name);
        return NULL;
      }
      o = it->second;
      Py_INCREF(o);
      
    //}
  }
  Py_DECREF(os);
  return o;
}

// ---

bool InitObject(PyObject *m) {
  
  PyLWCObjectType.tp_flags = Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE;
  PyLWCObjectType.tp_doc = "Object class";
  PyLWCObjectType.tp_new = lwcobj_new;
  PyLWCObjectType.tp_init = lwcobj_init;
  PyLWCObjectType.tp_dealloc = lwcobj_free;
  PyLWCObjectType.tp_methods = lwcobj_methods;
  PyLWCObjectType.tp_getattr = lwcobj_getattr;
  if (PyType_Ready(&PyLWCObjectType) < 0) {
    return false;
  }
  
  Py_INCREF(&PyLWCObjectType);
  PyModule_AddObject(m, "Object", (PyObject*)&PyLWCObjectType);
  
  return true;
}


}



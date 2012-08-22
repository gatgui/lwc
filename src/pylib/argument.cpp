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
#include <lwc/python/convert.h>

namespace py {

PyTypeObject PyLWCArgumentType;

// ---

void FreeDefaultValue(struct DefaultValueEntry &dve);

struct DefaultValueEntry
{
   lwc::Type type;
   bool array;
   size_t len;
   void *ptr;
   
   DefaultValueEntry()
     : type(lwc::AT_UNKNOWN)
     , array(false)
     , len(0)
     , ptr(0) {
   }
   
   ~DefaultValueEntry() {
     FreeDefaultValue(*this);
   }
};

static std::deque<DefaultValueEntry> gsDefaultValues;

void AddDefaultValue(void *ptr, lwc::Type t, bool array, size_t len=0) {
#ifdef _DEBUG
  std::cout << "Add lwc python module default value " << std::hex << ptr << std::dec << ", " << t << ", " << array << ", " << len << std::endl;
#endif
  //DefaultValueEntry &dve = {t, array, len, ptr};
  //gsDefaultValues.push_back(dve);
  gsDefaultValues.push_back(DefaultValueEntry());
  DefaultValueEntry &dve = gsDefaultValues.back();
  dve.type = t;
  dve.array = array;
  dve.len = len;
  dve.ptr = ptr;
}

bool SetArgDefault(lwc::Argument &arg, PyObject *obj) {
  if (arg.getDir() != lwc::AD_IN) {
    return false;
  }
  
  bool rv = true;
  
  switch (arg.getType()) {
  case lwc::AT_BOOL:
    if (arg.isArray()) {
      bool *defVal = 0;
      size_t len = 0;
      Python2C<lwc::AT_BOOL>::ToArray(obj, defVal, len);
      arg.setDefaultValue(defVal);
      AddDefaultValue((void*)defVal, arg.getType(), true, len);
    } else {
      bool defVal = false;
      Python2C<lwc::AT_BOOL>::ToValue(obj, defVal);
      arg.setDefaultValue(defVal);
    }
    break;
  case lwc::AT_INT:
    if (arg.isArray()) {
      lwc::Integer *defVal = 0;
      size_t len = 0;
      Python2C<lwc::AT_INT>::ToArray(obj, defVal, len);
      arg.setDefaultValue(defVal);
      AddDefaultValue((void*)defVal, arg.getType(), true, len);
    } else {
      lwc::Integer defVal = 0;
      Python2C<lwc::AT_INT>::ToValue(obj, defVal);
      arg.setDefaultValue(defVal);
    }
    break;
  case lwc::AT_REAL:
    if (arg.isArray()) {
      lwc::Real *defVal = 0;
      size_t len = 0;
      Python2C<lwc::AT_REAL>::ToArray(obj, defVal, len);
      arg.setDefaultValue(defVal);
      AddDefaultValue((void*)defVal, arg.getType(), true, len);
    } else {
      lwc::Real defVal = 0.0;
      Python2C<lwc::AT_REAL>::ToValue(obj, defVal);
      arg.setDefaultValue(defVal);
    }
    break;
  case lwc::AT_STRING:
    if (arg.isArray()) {
      char **defVal = 0;
      size_t len = 0;
      Python2C<lwc::AT_STRING>::ToArray(obj, defVal, len);
      arg.setDefaultValue(defVal);
      AddDefaultValue((void*)defVal, arg.getType(), true, len);
    } else {
      char *defVal = 0;
      Python2C<lwc::AT_STRING>::ToValue(obj, defVal);
      arg.setDefaultValue(defVal);
      AddDefaultValue((void*)defVal, arg.getType(), false);
    }
    break;
  case lwc::AT_OBJECT:
    if (arg.isArray()) {
      lwc::Object **defVal = 0;
      size_t len = 0;
      Python2C<lwc::AT_OBJECT>::ToArray(obj, defVal, len);
      arg.setDefaultValue(defVal);
      AddDefaultValue((void*)defVal, arg.getType(), true, len);
    } else {
      lwc::Object *defVal = 0;
      Python2C<lwc::AT_OBJECT>::ToValue(obj, defVal);
      arg.setDefaultValue(defVal);
      AddDefaultValue((void*)defVal, arg.getType(), false);
    }
    break;
  default:
    rv = false;
    break;
  }
  
  return rv;
}

void FreeDefaultValue(DefaultValueEntry &dve) {
  if (dve.ptr == 0) {
    return;
  }
#ifdef _DEBUG
  std::cout << "Free lwc python module default value " << std::hex << dve.ptr << std::dec << ", " << dve.type << ", " << dve.array << ", " << dve.len << std::endl;
#endif
  switch (dve.type) {
  case lwc::AT_BOOL:
    if (dve.array) {
      bool *ary = (bool*) dve.ptr;
      Python2C<lwc::AT_BOOL>::DisposeArray(ary, dve.len);
    }
    break;
  case lwc::AT_INT:
    if (dve.array) {
      lwc::Integer *ary = (lwc::Integer*) dve.ptr;
      Python2C<lwc::AT_INT>::DisposeArray(ary, dve.len);
    }
    break;
  case lwc::AT_REAL:
    if (dve.array) {
      lwc::Real *ary = (lwc::Real*) dve.ptr;
      Python2C<lwc::AT_REAL>::DisposeArray(ary, dve.len);
    }
    break;
  case lwc::AT_STRING:
    if (dve.array) {
      char **ary = (char**) dve.ptr;
      Python2C<lwc::AT_STRING>::DisposeArray(ary, dve.len);
    } else {
      char *str = (char*) dve.ptr;
      Python2C<lwc::AT_STRING>::DisposeValue(str);
    }
    break;
  case lwc::AT_OBJECT:
    if (dve.array) {
      lwc::Object **ary = (lwc::Object**) dve.ptr;
      Python2C<lwc::AT_OBJECT>::DisposeArray(ary, dve.len);
    } else {
      lwc::Object *obj = (lwc::Object*) dve.ptr;
      Python2C<lwc::AT_OBJECT>::DisposeValue(obj);
    }
  default:
    break;
  }
}

// ---

void CleanupModule() {
  // Don't need to care about that anymore, now handled in
  // DefaultValueEntry destructor
  //
  //for (size_t i=0; i<gsDefaultValues.size(); ++i) {
  //  FreeDefaultValue(gsDefaultValues[i]);
  //}
  //gsDefaultValues.clear();
#ifdef LWC_MEMTRACK
  std::cout << "=== lwc library: Memory status after python module cleanup" << std::endl;
  std::cout << lwc::Object::GetInstanceCount() << " remaining object(s)" << std::endl;
  lwc::memory::PrintAllocationInfo();
#endif
}

// ---

static PyObject* lwcarg_new(PyTypeObject *type, PyObject *, PyObject *) {
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
      val = PyDict_GetItemString(kwargs, "lenidx");
      if (!val) {
        PyErr_SetString(PyExc_RuntimeError, "lwcpy.Argument.__init__: expected \"lenidx\" keyword arguments for array type argument");
        return -1;
      }
      if (!PyInt_Check(val)) {
        PyErr_SetString(PyExc_RuntimeError, "lwcpy.Argument.__init__: expected integer value for \"lenidx\" keyword");
        return -1;
      }
      self->arg.setArraySizeArg(PyInt_AsLong(val));
    }
    PyObject *pDefVal = PyDict_GetItemString(kwargs, "def");
    if (pDefVal) {
      SetArgDefault(self->arg, pDefVal);
    }
    if (dir != lwc::AD_OUT) {
      // check for name value only on IN/INOUT attributes
      PyObject *pName = PyDict_GetItemString(kwargs, "name");
      if (pName) {
        if (!PyString_Check(pName)) {
          PyErr_SetString(PyExc_RuntimeError, "lwcpy.Argument.__init__: expected string value for \"name\" keyword");
          return -1;
        }
        const char *name = PyString_AsString(pName);
        self->arg.setName(name);
      }
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

static PyObject* lwcarg_getName(PyObject *pself, void *) {
  PyLWCArgument *self = (PyLWCArgument*)pself;
  return PyString_FromString(self->arg.getName().c_str());
}

static PyObject* lwcarg_getArraySizeArg(PyObject *pself, void *) {
  PyLWCArgument *self = (PyLWCArgument*)pself;
  return PyInt_FromLong(long(self->arg.arraySizeArg()));
}

static PyObject* lwcarg_getArrayArg(PyObject *pself, void *) {
  PyLWCArgument *self = (PyLWCArgument*)pself;
  return PyInt_FromLong(long(self->arg.arrayArg()));
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

static int lwcarg_setName(PyObject *pself, PyObject *val, void *) {
  PyLWCArgument *self = (PyLWCArgument*)pself;
  self->arg.setName(PyString_AsString(val));
  return 0;
}

static int lwcarg_setArray(PyObject *, PyObject *, void *) {
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

static PyObject* lwcarg_docString(PyObject *pself, PyObject *, PyObject *kwargs) {
  PyLWCArgument *self = (PyLWCArgument*) pself;
  std::string indent = "";
  PyObject *pIndent = PyDict_GetItemString(kwargs, "indent");
  if (pIndent) {
    if (!PyString_Check(pIndent)) {
      PyErr_SetString(PyExc_RuntimeError, "string value expected fror \"indent\" keyword");
      return NULL;
    }
    indent = PyString_AsString(pIndent);
  }
  return PyString_FromString(self->arg.docString(indent).c_str());
}

static PyObject* lwcarg_isNamed(PyObject *pself) {
  PyLWCArgument *self = (PyLWCArgument*)pself;
  if (self->arg.isNamed()) {
    Py_INCREF(Py_True);
    return Py_True;
  } else {
    Py_INCREF(Py_False);
    return Py_False;
  }
}

static PyObject* lwcarg_hasDefault(PyObject *pself) {
  PyLWCArgument *self = (PyLWCArgument*)pself;
  if (self->arg.hasDefaultValue()) {
    Py_INCREF(Py_True);
    return Py_True;
  } else {
    Py_INCREF(Py_False);
    return Py_False;
  }
}

static PyGetSetDef lwcarg_getset[] = {
  {"array", lwcarg_getArray, lwcarg_setArray, "Is argument array", NULL},
  {"dir", lwcarg_getDir, lwcarg_setDir, "Argument direction", NULL},
  {"type", lwcarg_getType, lwcarg_setType, "Argument type", NULL},
  {"name", lwcarg_getName, lwcarg_setName, "Argument name", NULL},
  {"arraySizeArg", lwcarg_getArraySizeArg, lwcarg_setArraySizeArg, "Index of argument containing array size", NULL},
  {"arrayArg", lwcarg_getArrayArg, lwcarg_setArrayArg, "Index of refering array", NULL},
  {NULL, NULL, NULL, NULL, NULL}
};

static PyMethodDef lwcarg_methods[] = {
  {"docString", (PyCFunction) lwcarg_docString, METH_VARARGS|METH_KEYWORDS, "Argument document string"},
  {"isNamed", (PyCFunction) lwcarg_isNamed, METH_NOARGS, "Is argument named"},
  {"hasDefault", (PyCFunction) lwcarg_hasDefault, METH_NOARGS, "Does argument have a default value"},
  {NULL, NULL, 0, NULL}
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
  
  memset(&PyLWCArgumentType, 0, sizeof(PyTypeObject));
  PyLWCArgumentType.ob_refcnt = 1;
  PyLWCArgumentType.ob_size = 0;
  PyLWCArgumentType.tp_name = "lwcpy.Argument";
  PyLWCArgumentType.tp_basicsize = sizeof(PyLWCArgument);
  PyLWCArgumentType.tp_flags = Py_TPFLAGS_DEFAULT;
  PyLWCArgumentType.tp_doc = "Method argument class";
  PyLWCArgumentType.tp_new = lwcarg_new;
  PyLWCArgumentType.tp_init = lwcarg_init;
  PyLWCArgumentType.tp_dealloc = lwcarg_free;
  PyLWCArgumentType.tp_getset = lwcarg_getset;
  PyLWCArgumentType.tp_str = lwcarg_str;
  PyLWCArgumentType.tp_methods = lwcarg_methods;
  if (PyType_Ready(&PyLWCArgumentType) < 0) {
    return false;
  }
  
  Py_INCREF((PyObject*) &PyLWCArgumentType);
  PyModule_AddObject(m, "Argument", (PyObject*)&PyLWCArgumentType);
  
  return true;
}

}


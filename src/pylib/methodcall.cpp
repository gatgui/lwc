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
#include <lwc/python/convert.h>

namespace py {

PyTypeObject PyLWCMethodCallType = {
  PyObject_HEAD_INIT(NULL)
  0,
  "lwcpy.MethodCall",
  sizeof(PyLWCMethodCall)
};

// ---

static PyObject* methcall_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
  PyLWCMethodCall *self = (PyLWCMethodCall*) type->tp_alloc(type, 0);
  self->obj = 0;
  self->method = 0;
  return (PyObject*)self;
}

static int methcall_init(PyObject *self, PyObject *args, PyObject *kwargs) {
  return 0;
}

static void methcall_free(PyObject *pself) {
  PyLWCMethodCall *self = (PyLWCMethodCall*) pself;
  if (self->method) {
    free(self->method);
  }
  pself->ob_type->tp_free(pself);
}

// This needs to be a recursive call rather than a for loop inside
// because real parameters are created on the stack [important for return value]
PyObject* CallMethod(lwc::Object *o, const char *n, lwc::MethodParams &params, int cArg,
                     PyObject *args, size_t pyArg, std::map<size_t,size_t> &arraySizes) {
  
  const lwc::Method &m = params.getMethod();
  
  if (m.numArgs() == cArg) {
    if (pyArg != PyTuple_Size(args)) {
      PyErr_SetString(PyExc_RuntimeError, "Invalid arguments. None expected");
      return NULL;
    }
    
    o->call(n, params);
    
    if (m.numArgs() == 0) {
      Py_INCREF(Py_None);
      return Py_None;
    } else {
      return PyTuple_New(0);
    }
    
  } else {
    
    PyObject *rv = 0;
    
    const lwc::Argument &ad = m[cArg];
    
    if (!ad.isArray() && ad.getDir() == lwc::AD_INOUT) {
      PyErr_SetString(PyExc_RuntimeError, "inout non array arguments not supported in python");
      return NULL;
    }
    
    // PreCallArray will increment pyArg if necessarys
    size_t oldPyArg = pyArg;
    
    switch (ad.getType()) {
      case lwc::AT_BOOL: {
        if (ad.isArray()) {
          bool *ary=0;
          ParamConverter<lwc::AT_BOOL>::PreCallArray(ad, cArg, args, pyArg, arraySizes, ary);
          if (ad.getDir() == lwc::AD_IN) {
            params.set(cArg, ary);
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_BOOL>::PostCallArray(ad, cArg, args, oldPyArg, arraySizes, ary, rv);
          
        } else {
          bool val;
          ParamConverter<lwc::AT_BOOL>::PreCall(ad, cArg, args, pyArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            params.set(cArg, val);
          } else {
            params.set(cArg, &val);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_BOOL>::PostCall(ad, cArg, args, oldPyArg, arraySizes, val, rv);
        }
        break;
      }
      case lwc::AT_INT: {
        if (ad.isArray()) {
          lwc::Integer *ary=0;
          ParamConverter<lwc::AT_INT>::PreCallArray(ad, cArg, args, pyArg, arraySizes, ary);
          if (ad.getDir() == lwc::AD_IN) {
            params.set(cArg, ary);
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_INT>::PostCallArray(ad, cArg, args, oldPyArg, arraySizes, ary, rv);
          
        } else {
          lwc::Integer val;
          ParamConverter<lwc::AT_INT>::PreCall(ad, cArg, args, pyArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            params.set(cArg, val);
          } else {
            params.set(cArg, &val);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_INT>::PostCall(ad, cArg, args, oldPyArg, arraySizes, val, rv);
        }
        break;
      }
      case lwc::AT_REAL: {
        if (ad.isArray()) {
          lwc::Real *ary=0;
          ParamConverter<lwc::AT_REAL>::PreCallArray(ad, cArg, args, pyArg, arraySizes, ary);
          if (ad.getDir() == lwc::AD_IN) {
            params.set(cArg, ary);
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_REAL>::PostCallArray(ad, cArg, args, oldPyArg, arraySizes, ary, rv);
          
        } else {
          lwc::Real val;
          ParamConverter<lwc::AT_REAL>::PreCall(ad, cArg, args, pyArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            params.set(cArg, val);
          } else {
            params.set(cArg, &val);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_REAL>::PostCall(ad, cArg, args, oldPyArg, arraySizes, val, rv);
        }
        break;
      }
      case lwc::AT_STRING: {
        if (ad.isArray()) {
          char **ary=0;
          ParamConverter<lwc::AT_STRING>::PreCallArray(ad, cArg, args, pyArg, arraySizes, ary);
          if (ad.getDir() == lwc::AD_IN) {
            params.set(cArg, ary);
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_STRING>::PostCallArray(ad, cArg, args, oldPyArg, arraySizes, ary, rv);
          
        } else {
          char *val;
          ParamConverter<lwc::AT_STRING>::PreCall(ad, cArg, args, pyArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            params.set(cArg, val);
          } else {
            params.set(cArg, &val);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_STRING>::PostCall(ad, cArg, args, oldPyArg, arraySizes, val, rv);
        }
        break;
      }
      case lwc::AT_OBJECT: {
        if (ad.isArray()) {
          lwc::Object **ary=0;
          ParamConverter<lwc::AT_OBJECT>::PreCallArray(ad, cArg, args, pyArg, arraySizes, ary);
          if (ad.getDir() == lwc::AD_IN) {
            params.set(cArg, ary);
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_OBJECT>::PostCallArray(ad, cArg, args, oldPyArg, arraySizes, ary, rv);
          
        } else {
          lwc::Object *val;
          ParamConverter<lwc::AT_OBJECT>::PreCall(ad, cArg, args, pyArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            params.set(cArg, val);
          } else {
            params.set(cArg, &val);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_OBJECT>::PostCall(ad, cArg, args, oldPyArg, arraySizes, val, rv);
        }
        break;
      }
      default:
        PyErr_SetString(PyExc_RuntimeError, "Invalid argument type");
        return NULL;
    }
    
    if (cArg == 0) {
      if (!rv) {
        rv = Py_None;
        Py_INCREF(rv);
      } else {
        size_t sz = PyTuple_Size(rv);
        if (sz == 0) {
          Py_DECREF(rv);
          rv = Py_None;
          Py_INCREF(rv);
        } else if (sz == 1) {
          PyObject *o = PyTuple_GetItem(rv, 0);
          Py_INCREF(o);
          Py_DECREF(rv);
          rv = o;
        }
      }
    }
    
    return rv;
  }
}

static PyObject *methcall_doit(PyObject *pself, PyObject *args, PyObject *kwargs) {
  PyLWCMethodCall *self = (PyLWCMethodCall*) pself;
  try {
    const lwc::Method &m = self->obj->getMethod(self->method);
    std::map<size_t, size_t> arraySizes;
    lwc::MethodParams params = lwc::MethodParams(m);
    return CallMethod(self->obj, self->method, params, 0, args, 0, arraySizes);
  } catch (std::runtime_error &e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
    return 0;
  }
}

// ---

bool InitMethodCall(PyObject *m) {
  PyLWCMethodCallType.tp_flags = Py_TPFLAGS_DEFAULT;
  PyLWCMethodCallType.tp_doc = "MethodCall class";
  PyLWCMethodCallType.tp_new = methcall_new;
  PyLWCMethodCallType.tp_init = methcall_init;
  PyLWCMethodCallType.tp_dealloc = methcall_free;
  PyLWCMethodCallType.tp_call = methcall_doit;
  if (PyType_Ready(&PyLWCMethodCallType) < 0) {
    return false;
  }
  
  Py_INCREF(&PyLWCMethodCallType);
  PyModule_AddObject(m, "MethodCall", (PyObject*)&PyLWCMethodCallType);
  
  return true;
}

}

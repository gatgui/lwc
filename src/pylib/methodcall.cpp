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
    
    if (ad.isPtr()) {
      PyErr_SetString(PyExc_RuntimeError, "Pointer arguments not supported in python");
      return NULL;
    }
    
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
            if (ad.isConst()) {
              params.set(cArg, (const bool*)ary);
            } else {
              params.set(cArg, ary);
            }
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_BOOL>::PostCallArray(ad, cArg, args, oldPyArg, arraySizes, ary, rv);
          
        } else {
          bool val;
          ParamConverter<lwc::AT_BOOL>::PreCall(ad, cArg, args, pyArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const bool)val);
            } else {
              params.set(cArg, val);
            }
          } else {
            params.set(cArg, &val);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_BOOL>::PostCall(ad, cArg, args, oldPyArg, arraySizes, val, rv);
        }
        break;
      }
      case lwc::AT_CHAR: {
        if (ad.isArray()) {
          char *ary=0;
          ParamConverter<lwc::AT_CHAR>::PreCallArray(ad, cArg, args, pyArg, arraySizes, ary);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const char*)ary);
            } else {
              params.set(cArg, ary);
            }
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_CHAR>::PostCallArray(ad, cArg, args, oldPyArg, arraySizes, ary, rv);
          
        } else {
          char val;
          ParamConverter<lwc::AT_CHAR>::PreCall(ad, cArg, args, pyArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const char)val);
            } else {
              params.set(cArg, val);
            }
          } else {
            params.set(cArg, &val);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_CHAR>::PostCall(ad, cArg, args, oldPyArg, arraySizes, val, rv);
        }
        break;
      }
      case lwc::AT_UCHAR: {
        if (ad.isArray()) {
          unsigned char *ary=0;
          ParamConverter<lwc::AT_UCHAR>::PreCallArray(ad, cArg, args, pyArg, arraySizes, ary);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const unsigned char*)ary);
            } else {
              params.set(cArg, ary);
            }
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_UCHAR>::PostCallArray(ad, cArg, args, oldPyArg, arraySizes, ary, rv);
          
        } else {
          unsigned char val;
          ParamConverter<lwc::AT_UCHAR>::PreCall(ad, cArg, args, pyArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const unsigned char)val);
            } else {
              params.set(cArg, val);
            }
          } else {
            params.set(cArg, &val);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_UCHAR>::PostCall(ad, cArg, args, oldPyArg, arraySizes, val, rv);
        }
        break;
      }
      case lwc::AT_SHORT: {
        if (ad.isArray()) {
          short *ary=0;
          ParamConverter<lwc::AT_SHORT>::PreCallArray(ad, cArg, args, pyArg, arraySizes, ary);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const short*)ary);
            } else {
              params.set(cArg, ary);
            }
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_SHORT>::PostCallArray(ad, cArg, args, oldPyArg, arraySizes, ary, rv);
          
        } else {
          short val;
          ParamConverter<lwc::AT_SHORT>::PreCall(ad, cArg, args, pyArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const short)val);
            } else {
              params.set(cArg, val);
            }
          } else {
            params.set(cArg, &val);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_SHORT>::PostCall(ad, cArg, args, oldPyArg, arraySizes, val, rv);
        }
        break;
      }
      case lwc::AT_USHORT: {
        if (ad.isArray()) {
          unsigned short *ary=0;
          ParamConverter<lwc::AT_USHORT>::PreCallArray(ad, cArg, args, pyArg, arraySizes, ary);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const unsigned short*)ary);
            } else {
              params.set(cArg, ary);
            }
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_USHORT>::PostCallArray(ad, cArg, args, oldPyArg, arraySizes, ary, rv);
          
        } else {
          unsigned short val;
          ParamConverter<lwc::AT_USHORT>::PreCall(ad, cArg, args, pyArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const unsigned short)val);
            } else {
              params.set(cArg, val);
            }
          } else {
            params.set(cArg, &val);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_USHORT>::PostCall(ad, cArg, args, oldPyArg, arraySizes, val, rv);
        }
        break;
      }
      case lwc::AT_INT: {
        if (ad.isArray()) {
          int *ary=0;
          ParamConverter<lwc::AT_INT>::PreCallArray(ad, cArg, args, pyArg, arraySizes, ary);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const int*)ary);
            } else {
              params.set(cArg, ary);
            }
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_INT>::PostCallArray(ad, cArg, args, oldPyArg, arraySizes, ary, rv);
          
        } else {
          int val;
          ParamConverter<lwc::AT_INT>::PreCall(ad, cArg, args, pyArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const int)val);
            } else {
              params.set(cArg, val);
            }
          } else {
            params.set(cArg, &val);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_INT>::PostCall(ad, cArg, args, oldPyArg, arraySizes, val, rv);
        }
        break;
      }
      case lwc::AT_UINT: {
        if (ad.isArray()) {
          unsigned int *ary=0;
          ParamConverter<lwc::AT_UINT>::PreCallArray(ad, cArg, args, pyArg, arraySizes, ary);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const unsigned int*)ary);
            } else {
              params.set(cArg, ary);
            }
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_UINT>::PostCallArray(ad, cArg, args, oldPyArg, arraySizes, ary, rv);
          
        } else {
          unsigned int val;
          ParamConverter<lwc::AT_UINT>::PreCall(ad, cArg, args, pyArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const unsigned int)val);
            } else {
              params.set(cArg, val);
            }
          } else {
            params.set(cArg, &val);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_UINT>::PostCall(ad, cArg, args, oldPyArg, arraySizes, val, rv);
        }
        break;
      }
      case lwc::AT_LONG: {
        if (ad.isArray()) {
          long *ary=0;
          ParamConverter<lwc::AT_LONG>::PreCallArray(ad, cArg, args, pyArg, arraySizes, ary);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const long*)ary);
            } else {
              params.set(cArg, ary);
            }
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_LONG>::PostCallArray(ad, cArg, args, oldPyArg, arraySizes, ary, rv);
          
        } else {
          long val;
          ParamConverter<lwc::AT_LONG>::PreCall(ad, cArg, args, pyArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const long)val);
            } else {
              params.set(cArg, val);
            }
          } else {
            params.set(cArg, &val);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_LONG>::PostCall(ad, cArg, args, oldPyArg, arraySizes, val, rv);
        }
        break;
      }
      case lwc::AT_ULONG: {
        if (ad.isArray()) {
          unsigned long *ary=0;
          ParamConverter<lwc::AT_ULONG>::PreCallArray(ad, cArg, args, pyArg, arraySizes, ary);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const unsigned long*)ary);
            } else {
              params.set(cArg, ary);
            }
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_ULONG>::PostCallArray(ad, cArg, args, oldPyArg, arraySizes, ary, rv);
          
        } else {
          unsigned long val;
          ParamConverter<lwc::AT_ULONG>::PreCall(ad, cArg, args, pyArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const unsigned long)val);
            } else {
              params.set(cArg, val);
            }
          } else {
            params.set(cArg, &val);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_ULONG>::PostCall(ad, cArg, args, oldPyArg, arraySizes, val, rv);
        }
        break;
      }
      case lwc::AT_FLOAT: {
        if (ad.isArray()) {
          float *ary=0;
          ParamConverter<lwc::AT_FLOAT>::PreCallArray(ad, cArg, args, pyArg, arraySizes, ary);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const float*)ary);
            } else {
              params.set(cArg, ary);
            }
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_FLOAT>::PostCallArray(ad, cArg, args, oldPyArg, arraySizes, ary, rv);
          
        } else {
          float val;
          ParamConverter<lwc::AT_FLOAT>::PreCall(ad, cArg, args, pyArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const float)val);
            } else {
              params.set(cArg, val);
            }
          } else {
            params.set(cArg, &val);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_FLOAT>::PostCall(ad, cArg, args, oldPyArg, arraySizes, val, rv);
        }
        break;
      }
      case lwc::AT_DOUBLE: {
        if (ad.isArray()) {
          double *ary=0;
          ParamConverter<lwc::AT_DOUBLE>::PreCallArray(ad, cArg, args, pyArg, arraySizes, ary);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const double*)ary);
            } else {
              params.set(cArg, ary);
            }
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_DOUBLE>::PostCallArray(ad, cArg, args, oldPyArg, arraySizes, ary, rv);
          
        } else {
          double val;
          ParamConverter<lwc::AT_DOUBLE>::PreCall(ad, cArg, args, pyArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const double)val);
            } else {
              params.set(cArg, val);
            }
          } else {
            params.set(cArg, &val);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_DOUBLE>::PostCall(ad, cArg, args, oldPyArg, arraySizes, val, rv);
        }
        break;
      }
      case lwc::AT_STRING: {
        if (ad.isArray()) {
          char **ary=0;
          ParamConverter<lwc::AT_STRING>::PreCallArray(ad, cArg, args, pyArg, arraySizes, ary);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const char**)ary);
            } else {
              params.set(cArg, ary);
            }
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_STRING>::PostCallArray(ad, cArg, args, oldPyArg, arraySizes, ary, rv);
          
        } else {
          char *val;
          ParamConverter<lwc::AT_STRING>::PreCall(ad, cArg, args, pyArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const char*)val);
            } else {
              params.set(cArg, val);
            }
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
            if (ad.isConst()) {
              params.set(cArg, (const lwc::Object**)ary);
            } else {
              params.set(cArg, ary);
            }
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, pyArg, arraySizes);
          ParamConverter<lwc::AT_OBJECT>::PostCallArray(ad, cArg, args, oldPyArg, arraySizes, ary, rv);
          
        } else {
          lwc::Object *val;
          ParamConverter<lwc::AT_OBJECT>::PreCall(ad, cArg, args, pyArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const lwc::Object*)val);
            } else {
              params.set(cArg, val);
            }
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


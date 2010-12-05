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

#include <lwc/python/pobject.h>
#include <lwc/python/types.h>
#include <lwc/python/convert.h>

namespace py {

Object::Object(PyObject *self)
  : lwc::Object(), mSelf(self) {
}

Object::~Object() {
  if (mSelf != 0) {
    PyLWCObject *po = (PyLWCObject*)mSelf;
    po->obj = 0;
    Py_DECREF(mSelf);
  }
}

void Object::call(const char *name, lwc::MethodParams &params) throw(std::runtime_error) {
  
  if (mSelf == 0) {
    throw std::runtime_error("Underlying python object does not exist");
  }
  
  const lwc::Method &meth = params.getMethod();

  size_t ninputs = 0;
  
  for (size_t i=0; i<meth.numArgs(); ++i) {
    const lwc::Argument &arg = meth[i];
    if (arg.getDir() == lwc::AD_INOUT && !arg.isArray()) {
      throw std::runtime_error("Python does not support non-array inout arguments");
    }
    if ((arg.getDir() == lwc::AD_IN || arg.getDir() == lwc::AD_INOUT) && arg.arrayArg() < 0) {
      ++ninputs;
    }
  }
  
  PyObject *args = PyTuple_New(ninputs);
  
  std::map<size_t, size_t> inoutArgs;
  std::map<size_t, size_t> inoutInSizes;
  
  Py_ssize_t cur = 0;
  
  for (size_t i=0; i<meth.numArgs(); ++i) {
    const lwc::Argument &arg = meth[i];
    
    PyObject *parg = 0;
    
    if (arg.arrayArg() >= 0) {
      continue;
    }
    
    if (arg.isArray()) {
      if (arg.getDir() != lwc::AD_IN && arg.getDir() != lwc::AD_INOUT) {
        continue;
      }
      
      bool out = (arg.getDir() == lwc::AD_INOUT);
      
      if (out) {
        inoutArgs[i] = cur;
      }
      
      size_t len = 0;
      
      const lwc::Argument &lenarg = meth[size_t(arg.arraySizeArg())];
      
      switch(lenarg.getType()) {
        case lwc::AT_INT: {
          if (!out) {
            lwc::Integer sz;
            params.get(size_t(arg.arraySizeArg()), sz);
            len = size_t(sz);
          } else {
            lwc::Integer *sz;
            params.get(size_t(arg.arraySizeArg()), sz);
            len = size_t(*sz);
          }
          break;
        }
        default:
          throw std::runtime_error("Invalid array size argument type");
      }
      
      if (out) {
        inoutInSizes[i] = len;
      }
      
      switch(arg.getType()) {
        case lwc::AT_BOOL: {
          if (!out) {
            bool *ary;
            params.get(i, ary);
            C2Python<lwc::AT_BOOL>::ToArray(ary, len, parg);
          } else {
            bool **ary;
            params.get(i, ary);
            C2Python<lwc::AT_BOOL>::ToArray(*ary, len, parg);
          }
          break;
        }
        case lwc::AT_INT: {
          if (!out) {
            lwc::Integer *ary;
            params.get(i, ary);
            C2Python<lwc::AT_INT>::ToArray(ary, len, parg);
          } else {
            lwc::Integer **ary;
            params.get(i, ary);
            C2Python<lwc::AT_INT>::ToArray(*ary, len, parg);
          }
          break;
        }
        case lwc::AT_REAL: {
          if (!out) {
            lwc::Real *ary;
            params.get(i, ary);
            C2Python<lwc::AT_REAL>::ToArray(ary, len, parg);
          } else {
            lwc::Real **ary;
            params.get(i, ary);
            C2Python<lwc::AT_REAL>::ToArray(*ary, len, parg);
          }
          break;
        }
        case lwc::AT_STRING: {
          if (!out) {
            char **ary;
            params.get(i, ary);
            C2Python<lwc::AT_STRING>::ToArray(ary, len, parg);
          } else {
            char ***ary;
            params.get(i, ary);
            C2Python<lwc::AT_STRING>::ToArray(*ary, len, parg);
          }
          break;
        }
        case lwc::AT_OBJECT: {
          if (!out) {
            lwc::Object **ary;
            params.get(i, ary);
            C2Python<lwc::AT_OBJECT>::ToArray(ary, len, parg);
          } else {
            lwc::Object ***ary;
            params.get(i, ary);
            C2Python<lwc::AT_OBJECT>::ToArray(*ary, len, parg);
          }
          break;
        }
        default:
          throw std::runtime_error("Invalid array argument type");
      }
      
    } else {
      if (arg.getDir() != lwc::AD_IN) {
        continue;
      }
      
      switch(arg.getType()) {
        case lwc::AT_BOOL: {
          bool val;
          params.get(i, val);
          C2Python<lwc::AT_BOOL>::ToValue(val, parg);
          break;
        }
        case lwc::AT_INT: {
          lwc::Integer val;
          params.get(i, val);
          C2Python<lwc::AT_INT>::ToValue(val, parg);
          break;
        }
        case lwc::AT_REAL: {
          lwc::Real val;
          params.get(i, val);
          C2Python<lwc::AT_REAL>::ToValue(val, parg);
          break;
        }
        case lwc::AT_STRING: {
          char *val;
          params.get(i, val);
          C2Python<lwc::AT_STRING>::ToValue(val, parg);
          break;
        }
        case lwc::AT_OBJECT: {
          lwc::Object *val;
          params.get(i, val);
          C2Python<lwc::AT_OBJECT>::ToValue(val, parg);
          break;
        }
        default:
          throw std::runtime_error("Invalid argument type");
      }
    }
    
    PyTuple_SetItem(args, cur, parg);
    ++cur;
  }
  
  // call python method
  PyObject *func = PyObject_GetAttrString(mSelf, name);
  if (!func) {
    std::ostringstream oss;
    oss << "Python object has no method named \"" << name << "\"";
    throw std::runtime_error(oss.str());
  }
  // Check for errors
  PyObject *rv = PyObject_CallObject(func, args);
  Py_DECREF(func);
  if (rv == NULL) {
    // exception was raised
    std::ostringstream oss;
    
    PyObject *et=0, *ev=0, *etb=0, *s=0;
    
    PyErr_Fetch(&et, &ev, &etb);
    
    //s = PyObject_Str(et);
    //oss << std::endl << "--- Python --- " << PyString_AsString(s);
    //Py_DECREF(s);
    
    if (ev) {
      s = PyObject_Str(ev);
      oss << std::endl << "--- Python --- " << PyString_AsString(s);
      Py_DECREF(s);
    }
    
    if (etb) {
      PyObject *tbmn = PyString_FromString("traceback");
      PyObject *tbm = PyImport_Import(tbmn);
      Py_DECREF(tbmn);
      if (tbm) {
        PyObject *mdict = PyModule_GetDict(tbm);
        PyObject *func = PyDict_GetItemString(mdict, "format_tb"); // borrowed reference
        if (func && PyCallable_Check(func)) {
          PyObject *tbargs = PyTuple_New(1);
          PyTuple_SetItem(tbargs, 0, etb);
          PyObject *tbl = PyObject_CallObject(func, tbargs);
          if (tbl) {
            Py_ssize_t nf = PyList_Size(tbl);
            for (Py_ssize_t f=0; f<nf; ++f) {
              PyObject *fs = PyList_GetItem(tbl, f);
              std::string lines = PyString_AsString(fs);
              size_t p0 = 0, p1 = lines.find('\n', p0);
              while (p1 != std::string::npos) {
                std::string line = lines.substr(p0, p1-p0);
                oss << std::endl << "--- Python --- " << line;
                p0 = p1 + 1;
                p1 = lines.find('\n', p0);
              }
              oss << std::endl << "--- Python --- " << lines.substr(p0);
            }
            Py_DECREF(tbl);
          }
          Py_DECREF(tbargs);
        }
        Py_DECREF(tbm);
      }
    }
    
    Py_XDECREF(et);
    Py_XDECREF(ev);
    Py_XDECREF(etb);
    
    oss << std::endl;
    
    // Should this be called? -> NO PyErr_Fetch does it
    //PyErr_Clear();
    
    throw std::runtime_error(oss.str().c_str());
  }
  
  // convert Python outputs to C
  
  // params.get(idx, var) is not quite the same as: params.set(idx, var)
  // beware, the last is called by the caller to set the address of an output var
  // that will receive the value.
  
  cur = 0;
  bool tuple = (PyTuple_Check(rv) == 1);
  
  for (size_t i=0; i<meth.numArgs(); ++i) {
    const lwc::Argument &arg = meth[i];
    
    if (arg.getDir() == lwc::AD_IN) {
      continue;
    }
    
    if (arg.arrayArg() >= 0) {
      // array sizes handled when dealing with the array arg
      continue;
    }
    
    //if (rv == Py_None) {
    //  throw std::runtime_error("Expected return value, got None");
    //}
    if (cur > 0 && !tuple) {
      throw std::runtime_error("Expected tuple as return value (multiple output)");
    }
    
    // can be inout, out or return
    
    if (arg.isArray()) {
      // if inout -> do re-allocation
      // this gonna be fun
      size_t len = 0;
      
      PyObject *crv = 0;
      
      if (arg.getDir() == lwc::AD_INOUT) {
        
        crv = PyTuple_GetItem(args, inoutArgs[i]);
        
        len = inoutInSizes[i];
        
      } else {
        
        if (rv == Py_None) {
          throw std::runtime_error("Expected return value, got None");
        }
        
        if (tuple) {
          crv = PyTuple_GetItem(rv, cur);
        } else {
          crv = rv;
        }
        ++cur;
        
        if (!PyList_Check(crv)) {
          throw std::runtime_error("Expected list return argument");
        }
      }
      
      switch(arg.getType()) {
        case lwc::AT_BOOL: {
          bool **ary;
          params.get(i, ary);
          Python2C<lwc::AT_BOOL>::ToArray(crv, *ary, len);
          break;
        }
        case lwc::AT_INT: {
          lwc::Integer **ary;
          params.get(i, ary);
          Python2C<lwc::AT_INT>::ToArray(crv, *ary, len);
          break;
        }
        case lwc::AT_REAL: {
          lwc::Real **ary;
          params.get(i, ary);
          Python2C<lwc::AT_REAL>::ToArray(crv, *ary, len);
          break;
        }
        case lwc::AT_STRING: {
          char ***ary;
          params.get(i, ary);
          // if inout ... might need to free stuffs
          // as ToArray will re-alloc but not necessarily free elements
          // len is in/out -> could use it to free the required number
          Python2C<lwc::AT_STRING>::ToArray(crv, *ary, len);
          break;
        }
        case lwc::AT_OBJECT: {
          lwc::Object ***ary;
          params.get(i, ary);
          Python2C<lwc::AT_OBJECT>::ToArray(crv, *ary, len);
          break;
        }
        default:
          throw std::runtime_error("Invalid argument type");
      }
      
      // set length argument here
      const lwc::Argument &lenarg = meth[size_t(arg.arraySizeArg())];
      
      switch(lenarg.getType()) {
        case lwc::AT_INT: {
          lwc::Integer *sz;
          params.get(size_t(arg.arraySizeArg()), sz);
          *sz = (lwc::Integer) len;
          break;
        }
        default:
          throw std::runtime_error("Invalid array size type");
      }
      
    } else {
      
      PyObject *crv; // get from PyTuple or rv
      
      if (tuple) {
        if (rv == Py_None) {
          throw std::runtime_error("Expected return value, got None");
        }
        crv = PyTuple_GetItem(rv, cur);
      } else {
        crv = rv;
      }
      if (crv == Py_None && arg.getType() != lwc::AT_STRING && arg.getType() != lwc::AT_OBJECT) {
        throw std::runtime_error("Expected return value, got None");
      }
      ++cur;
      
      switch(arg.getType()) {
        case lwc::AT_BOOL: {
          bool *val;
          params.get(i, val);
          Python2C<lwc::AT_BOOL>::ToValue(crv, *val);
          break;
        }
        case lwc::AT_INT: {
          lwc::Integer *val;
          params.get(i, val);
          Python2C<lwc::AT_INT>::ToValue(crv, *val);
          break;
        }
        case lwc::AT_REAL: {
          lwc::Real *val;
          params.get(i, val);
          Python2C<lwc::AT_REAL>::ToValue(crv, *val);
          break;
        }
        case lwc::AT_STRING: {
          char **val;
          params.get(i, val);
          Python2C<lwc::AT_STRING>::ToValue(crv, *val);
          break;
        }
        case lwc::AT_OBJECT: {
          lwc::Object **val;
          params.get(i, val);
          Python2C<lwc::AT_OBJECT>::ToValue(crv, *val);
          break;
        }
        default:
          throw std::runtime_error("Invalid argument type");
      }
    }
    
  }
  
  Py_DECREF(args);
}

}

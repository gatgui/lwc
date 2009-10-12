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
    if (arg.isPtr()) {
      throw std::runtime_error("Python does not support pointer arguments");
    }
    if (arg.getDir() == lwc::AD_INOUT && !arg.isArray()) {
      throw std::runtime_error("Python does not support non-array inout arguments");
    }
    if ((arg.getDir() == lwc::AD_IN || arg.getDir() == lwc::AD_INOUT) && arg.arrayArg() == -1) {
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
    
    if (arg.arrayArg() != -1) {
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
      
      const lwc::Argument &lenarg = meth[arg.arraySizeArg()];
      
      switch(lenarg.getType()) {
        case lwc::AT_CHAR: {
          if (!out) {
            char sz;
            params.get(arg.arraySizeArg(), sz);
            len = size_t(sz);
          } else {
            char *sz;
            params.get(arg.arraySizeArg(), sz);
            len = size_t(*sz);
          }
          break;
        }
        case lwc::AT_UCHAR: {
          if (!out) {
            unsigned char sz;
            params.get(arg.arraySizeArg(), sz);
            len = size_t(sz);
          } else {
            unsigned char *sz;
            params.get(arg.arraySizeArg(), sz);
            len = size_t(*sz);
          }
          break;
        }
        case lwc::AT_SHORT: {
          if (!out) {
            short sz;
            params.get(arg.arraySizeArg(), sz);
            len = size_t(sz);
          } else {
            short *sz;
            params.get(arg.arraySizeArg(), sz);
            len = size_t(*sz);
          }
          break;
        }
        case lwc::AT_USHORT: {
          if (!out) {
            unsigned short sz;
            params.get(arg.arraySizeArg(), sz);
            len = size_t(sz);
          } else {
            unsigned short *sz;
            params.get(arg.arraySizeArg(), sz);
            len = size_t(*sz);
          }
          break;
        }
        case lwc::AT_INT: {
          if (!out) {
            int sz;
            params.get(arg.arraySizeArg(), sz);
            len = size_t(sz);
          } else {
            int *sz;
            params.get(arg.arraySizeArg(), sz);
            len = size_t(*sz);
          }
          break;
        }
        case lwc::AT_UINT: {
          if (!out) {
            unsigned int sz;
            params.get(arg.arraySizeArg(), sz);
            len = size_t(sz);
          } else {
            unsigned int *sz;
            params.get(arg.arraySizeArg(), sz);
            len = size_t(*sz);
          }
          break;
        }
        case lwc::AT_LONG: {
          if (!out) {
            long sz;
            params.get(arg.arraySizeArg(), sz);
            len = size_t(sz);
          } else {
            long *sz;
            params.get(arg.arraySizeArg(), sz);
            len = size_t(*sz);
          }
          break;
        }
        case lwc::AT_ULONG: {
          if (!out) {
            unsigned long sz;
            params.get(arg.arraySizeArg(), sz);
            len = size_t(sz);
          } else {
            unsigned long *sz;
            params.get(arg.arraySizeArg(), sz);
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
        case lwc::AT_CHAR: {
          if (!out) {
            char *ary;
            params.get(i, ary);
            C2Python<lwc::AT_CHAR>::ToArray(ary, len, parg);
          } else {
            char **ary;
            params.get(i, ary);
            C2Python<lwc::AT_CHAR>::ToArray(*ary, len, parg);
          }
          break;
        }
        case lwc::AT_UCHAR: {
          if (!out) {
            unsigned char *ary;
            params.get(i, ary);
            C2Python<lwc::AT_UCHAR>::ToArray(ary, len, parg);
          } else {
            unsigned char **ary;
            params.get(i, ary);
            C2Python<lwc::AT_UCHAR>::ToArray(*ary, len, parg);
          }
          break;
        }
        case lwc::AT_SHORT: {
          if (!out) {
            short *ary;
            params.get(i, ary);
            C2Python<lwc::AT_SHORT>::ToArray(ary, len, parg);
          } else {
            short **ary;
            params.get(i, ary);
            C2Python<lwc::AT_SHORT>::ToArray(*ary, len, parg);
          }
          break;
        }
        case lwc::AT_USHORT: {
          if (!out) {
            unsigned short *ary;
            params.get(i, ary);
            C2Python<lwc::AT_USHORT>::ToArray(ary, len, parg);
          } else {
            unsigned short **ary;
            params.get(i, ary);
            C2Python<lwc::AT_USHORT>::ToArray(*ary, len, parg);
          }
          break;
        }
        case lwc::AT_INT: {
          if (!out) {
            int *ary;
            params.get(i, ary);
            C2Python<lwc::AT_INT>::ToArray(ary, len, parg);
          } else {
            int **ary;
            params.get(i, ary);
            C2Python<lwc::AT_INT>::ToArray(*ary, len, parg);
          }
          break;
        }
        case lwc::AT_UINT: {
          if (!out) {
            unsigned int *ary;
            params.get(i, ary);
            C2Python<lwc::AT_UINT>::ToArray(ary, len, parg);
          } else {
            unsigned int **ary;
            params.get(i, ary);
            C2Python<lwc::AT_UINT>::ToArray(*ary, len, parg);
          }
          break;
        }
        case lwc::AT_LONG: {
          if (!out) {
            long *ary;
            params.get(i, ary);
            C2Python<lwc::AT_LONG>::ToArray(ary, len, parg);
          } else {
            long **ary;
            params.get(i, ary);
            C2Python<lwc::AT_LONG>::ToArray(*ary, len, parg);
          }
          break;
        }
        case lwc::AT_ULONG: {
          if (!out) {
            unsigned long *ary;
            params.get(i, ary);
            C2Python<lwc::AT_ULONG>::ToArray(ary, len, parg);
          } else {
            unsigned long **ary;
            params.get(i, ary);
            C2Python<lwc::AT_ULONG>::ToArray(*ary, len, parg);
          }
          break;
        }
        case lwc::AT_FLOAT: {
          if (!out) {
            float *ary;
            params.get(i, ary);
            C2Python<lwc::AT_FLOAT>::ToArray(ary, len, parg);
          } else {
            float **ary;
            params.get(i, ary);
            C2Python<lwc::AT_FLOAT>::ToArray(*ary, len, parg);
          }
          break;
        }
        case lwc::AT_DOUBLE: {
          if (!out) {
            double *ary;
            params.get(i, ary);
            C2Python<lwc::AT_DOUBLE>::ToArray(ary, len, parg);
          } else {
            double **ary;
            params.get(i, ary);
            C2Python<lwc::AT_DOUBLE>::ToArray(*ary, len, parg);
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
        case lwc::AT_CHAR: {
          char val;
          params.get(i, val);
          C2Python<lwc::AT_CHAR>::ToValue(val, parg);
          break;
        }
        case lwc::AT_UCHAR: {
          unsigned char val;
          params.get(i, val);
          C2Python<lwc::AT_UCHAR>::ToValue(val, parg);
          break;
        }
        case lwc::AT_SHORT: {
          short val;
          params.get(i, val);
          C2Python<lwc::AT_SHORT>::ToValue(val, parg);
          break;
        }
        case lwc::AT_USHORT: {
          unsigned short val;
          params.get(i, val);
          C2Python<lwc::AT_USHORT>::ToValue(val, parg);
          break;
        }
        case lwc::AT_INT: {
          int val;
          params.get(i, val);
          C2Python<lwc::AT_INT>::ToValue(val, parg);
          break;
        }
        case lwc::AT_UINT: {
          unsigned int val;
          params.get(i, val);
          C2Python<lwc::AT_UINT>::ToValue(val, parg);
          break;
        }
        case lwc::AT_LONG: {
          long val;
          params.get(i, val);
          C2Python<lwc::AT_LONG>::ToValue(val, parg);
          break;
        }
        case lwc::AT_ULONG: {
          unsigned long val;
          params.get(i, val);
          C2Python<lwc::AT_ULONG>::ToValue(val, parg);
          break;
        }
        case lwc::AT_FLOAT: {
          float val;
          params.get(i, val);
          C2Python<lwc::AT_FLOAT>::ToValue(val, parg);
          break;
        }
        case lwc::AT_DOUBLE: {
          double val;
          params.get(i, val);
          C2Python<lwc::AT_DOUBLE>::ToValue(val, parg);
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
  PyObject *rv = PyObject_CallObject(func, args);
  Py_DECREF(func);
  
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
    
    if (rv == Py_None) {
      throw std::runtime_error("Expected return value, got None");
    }
    if (cur > 0 && !tuple) {
      throw std::runtime_error("Expected tuple as return value (multiple output)");
    }
    
    // can be inout, out or return
    
    if (arg.arrayArg() != -1) {
      // array sizes handled when dealing with the array arg
      continue;
    }
    
    if (arg.isArray()) {
      // if inout -> do re-allocation
      // this gonna be fun
      size_t len = 0;
      
      PyObject *crv = 0;
      
      if (arg.getDir() == lwc::AD_INOUT) {
        
        crv = PyTuple_GetItem(args, inoutArgs[i]);
        
        len = inoutInSizes[i];
        
      } else {
        
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
        case lwc::AT_CHAR: {
          char **ary;
          params.get(i, ary);
          Python2C<lwc::AT_CHAR>::ToArray(crv, *ary, len);
          break;
        }
        case lwc::AT_UCHAR: {
          unsigned char **ary;
          params.get(i, ary);
          Python2C<lwc::AT_UCHAR>::ToArray(crv, *ary, len);
          break;
        }
        case lwc::AT_SHORT: {
          short **ary;
          params.get(i, ary);
          Python2C<lwc::AT_SHORT>::ToArray(crv, *ary, len);
          break;
        }
        case lwc::AT_USHORT: {
          unsigned short **ary;
          params.get(i, ary);
          Python2C<lwc::AT_USHORT>::ToArray(crv, *ary, len);
          break;
        }
        case lwc::AT_INT: {
          int **ary;
          params.get(i, ary);
          Python2C<lwc::AT_INT>::ToArray(crv, *ary, len);
          break;
        }
        case lwc::AT_UINT: {
          unsigned int **ary;
          params.get(i, ary);
          Python2C<lwc::AT_UINT>::ToArray(crv, *ary, len);
          break;
        }
        case lwc::AT_LONG: {
          long **ary;
          params.get(i, ary);
          Python2C<lwc::AT_LONG>::ToArray(crv, *ary, len);
          break;
        }
        case lwc::AT_ULONG: {
          unsigned long **ary;
          params.get(i, ary);
          Python2C<lwc::AT_ULONG>::ToArray(crv, *ary, len);
          break;
        }
        case lwc::AT_FLOAT: {
          float **ary;
          params.get(i, ary);
          Python2C<lwc::AT_FLOAT>::ToArray(crv, *ary, len);
          break;
        }
        case lwc::AT_DOUBLE: {
          double **ary;
          params.get(i, ary);
          Python2C<lwc::AT_DOUBLE>::ToArray(crv, *ary, len);
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
      const lwc::Argument &lenarg = meth[arg.arraySizeArg()];
      
      switch(lenarg.getType()) {
        case lwc::AT_CHAR: {
          char *sz;
          params.get(arg.arraySizeArg(), sz);
          *sz = (char) len;
          break;
        }
        case lwc::AT_UCHAR: {
          unsigned char *sz;
          params.get(arg.arraySizeArg(), sz);
          *sz = (unsigned char) len;
          break;
        }
        case lwc::AT_SHORT: {
          short *sz;
          params.get(arg.arraySizeArg(), sz);
          *sz = (short) len;
          break;
        }
        case lwc::AT_USHORT: {
          unsigned short *sz;
          params.get(arg.arraySizeArg(), sz);
          *sz = (unsigned short) len;
          break;
        }
        case lwc::AT_INT: {
          int *sz;
          params.get(arg.arraySizeArg(), sz);
          *sz = (int) len;
          break;
        }
        case lwc::AT_UINT: {
          unsigned int *sz;
          params.get(arg.arraySizeArg(), sz);
          *sz = (unsigned int) len;
          break;
        }
        case lwc::AT_LONG: {
          long *sz;
          params.get(arg.arraySizeArg(), sz);
          *sz = (long) len;
          break;
        }
        case lwc::AT_ULONG: {
          unsigned long *sz;
          params.get(arg.arraySizeArg(), sz);
          *sz = (unsigned long) len;
          break;
        }
        default:
          throw std::runtime_error("Invalid array size type");
      }
      
    } else {
      
      PyObject *crv; // get from PyTuple or rv
      
      if (tuple) {
        crv = PyTuple_GetItem(rv, cur);
      } else {
        crv = rv;
      }
      ++cur;
      
      switch(arg.getType()) {
        case lwc::AT_BOOL: {
          bool *val;
          params.get(i, val);
          Python2C<lwc::AT_BOOL>::ToValue(crv, *val);
          break;
        }
        case lwc::AT_CHAR: {
          char *val;
          params.get(i, val);
          Python2C<lwc::AT_CHAR>::ToValue(crv, *val);
          break;
        }
        case lwc::AT_UCHAR: {
          unsigned char *val;
          params.get(i, val);
          Python2C<lwc::AT_UCHAR>::ToValue(crv, *val);
          break;
        }
        case lwc::AT_SHORT: {
          short *val;
          params.get(i, val);
          Python2C<lwc::AT_SHORT>::ToValue(crv, *val);
          break;
        }
        case lwc::AT_USHORT: {
          unsigned short *val;
          params.get(i, val);
          Python2C<lwc::AT_USHORT>::ToValue(crv, *val);
          break;
        }
        case lwc::AT_INT: {
          int *val;
          params.get(i, val);
          Python2C<lwc::AT_INT>::ToValue(crv, *val);
          break;
        }
        case lwc::AT_UINT: {
          unsigned int *val;
          params.get(i, val);
          Python2C<lwc::AT_UINT>::ToValue(crv, *val);
          break;
        }
        case lwc::AT_LONG: {
          long *val;
          params.get(i, val);
          Python2C<lwc::AT_LONG>::ToValue(crv, *val);
          break;
        }
        case lwc::AT_ULONG: {
          unsigned long *val;
          params.get(i, val);
          Python2C<lwc::AT_ULONG>::ToValue(crv, *val);
          break;
        }
        case lwc::AT_FLOAT: {
          float *val;
          params.get(i, val);
          Python2C<lwc::AT_FLOAT>::ToValue(crv, *val);
          break;
        }
        case lwc::AT_DOUBLE: {
          double *val;
          params.get(i, val);
          Python2C<lwc::AT_DOUBLE>::ToValue(crv, *val);
          break;
        }
        case lwc::AT_STRING: {
          char **val;
          params.get(i, val);
          // Python2C for string allocates ... does the arg specify this properly?
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

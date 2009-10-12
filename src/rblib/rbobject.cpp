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

#include <lwc/ruby/rbobject.h>
#include <lwc/ruby/convert.h>

namespace rb {

Object::Object(VALUE self)
  : mSelf(self) {
  if (mSelf != Qnil) {
    rb::Tracker::Add(this, self);
  }
}

Object::~Object() {
  if (mSelf != Qnil) {
    // This sets pointer to 0
    rb::Tracker::Unlink(this);
    // This remove from tracker table so that object won't be marked except
    //   if there are reference in Ruby
    rb::Tracker::Remove(this);
  }
}

void Object::call(const char *name, lwc::MethodParams &params) throw(std::runtime_error) {
  
  if (mSelf == Qnil) {
    throw std::runtime_error("Underlying ruby object does not exist");
  }
  
  const lwc::Method &meth = params.getMethod();

  size_t ninputs = 0;
  
  for (size_t i=0; i<meth.numArgs(); ++i) {
    const lwc::Argument &arg = meth[i];
    if (arg.isPtr()) {
      throw std::runtime_error("Ruby does not support pointer arguments");
    }
    if (arg.getDir() == lwc::AD_INOUT && !arg.isArray()) {
      throw std::runtime_error("Ruby does not support non-array inout arguments");
    }
    if ((arg.getDir() == lwc::AD_IN || arg.getDir() == lwc::AD_INOUT) && arg.arrayArg() != -1) {
      ++ninputs;
    }
  }
  
  VALUE *args = (VALUE*) malloc(ninputs*sizeof(VALUE));
  
  std::map<size_t, size_t> inoutArgs;
  std::map<size_t, size_t> inoutInSizes;
  
  size_t cur = 0;
  
  for (size_t i=0; i<meth.numArgs(); ++i) {
    const lwc::Argument &arg = meth[i];
    
    VALUE rarg = Qnil;
    
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
            C2Ruby<lwc::AT_BOOL>::ToArray(ary, len, rarg);
          } else {
            bool **ary;
            params.get(i, ary);
            C2Ruby<lwc::AT_BOOL>::ToArray(*ary, len, rarg);
          }
          break;
        }
        case lwc::AT_CHAR: {
          if (!out) {
            char *ary;
            params.get(i, ary);
            C2Ruby<lwc::AT_CHAR>::ToArray(ary, len, rarg);
          } else {
            char **ary;
            params.get(i, ary);
            C2Ruby<lwc::AT_CHAR>::ToArray(*ary, len, rarg);
          }
          break;
        }
        case lwc::AT_UCHAR: {
          if (!out) {
            unsigned char *ary;
            params.get(i, ary);
            C2Ruby<lwc::AT_UCHAR>::ToArray(ary, len, rarg);
          } else {
            unsigned char **ary;
            params.get(i, ary);
            C2Ruby<lwc::AT_UCHAR>::ToArray(*ary, len, rarg);
          }
          break;
        }
        case lwc::AT_SHORT: {
          if (!out) {
            short *ary;
            params.get(i, ary);
            C2Ruby<lwc::AT_SHORT>::ToArray(ary, len, rarg);
          } else {
            short **ary;
            params.get(i, ary);
            C2Ruby<lwc::AT_SHORT>::ToArray(*ary, len, rarg);
          }
          break;
        }
        case lwc::AT_USHORT: {
          if (!out) {
            unsigned short *ary;
            params.get(i, ary);
            C2Ruby<lwc::AT_USHORT>::ToArray(ary, len, rarg);
          } else {
            unsigned short **ary;
            params.get(i, ary);
            C2Ruby<lwc::AT_USHORT>::ToArray(*ary, len, rarg);
          }
          break;
        }
        case lwc::AT_INT: {
          if (!out) {
            int *ary;
            params.get(i, ary);
            C2Ruby<lwc::AT_INT>::ToArray(ary, len, rarg);
          } else {
            int **ary;
            params.get(i, ary);
            C2Ruby<lwc::AT_INT>::ToArray(*ary, len, rarg);
          }
          break;
        }
        case lwc::AT_UINT: {
          if (!out) {
            unsigned int *ary;
            params.get(i, ary);
            C2Ruby<lwc::AT_UINT>::ToArray(ary, len, rarg);
          } else {
            unsigned int **ary;
            params.get(i, ary);
            C2Ruby<lwc::AT_UINT>::ToArray(*ary, len, rarg);
          }
          break;
        }
        case lwc::AT_LONG: {
          if (!out) {
            long *ary;
            params.get(i, ary);
            C2Ruby<lwc::AT_LONG>::ToArray(ary, len, rarg);
          } else {
            long **ary;
            params.get(i, ary);
            C2Ruby<lwc::AT_LONG>::ToArray(*ary, len, rarg);
          }
          break;
        }
        case lwc::AT_ULONG: {
          if (!out) {
            unsigned long *ary;
            params.get(i, ary);
            C2Ruby<lwc::AT_ULONG>::ToArray(ary, len, rarg);
          } else {
            unsigned long **ary;
            params.get(i, ary);
            C2Ruby<lwc::AT_ULONG>::ToArray(*ary, len, rarg);
          }
          break;
        }
        case lwc::AT_FLOAT: {
          if (!out) {
            float *ary;
            params.get(i, ary);
            C2Ruby<lwc::AT_FLOAT>::ToArray(ary, len, rarg);
          } else {
            float **ary;
            params.get(i, ary);
            C2Ruby<lwc::AT_FLOAT>::ToArray(*ary, len, rarg);
          }
          break;
        }
        case lwc::AT_DOUBLE: {
          if (!out) {
            double *ary;
            params.get(i, ary);
            C2Ruby<lwc::AT_DOUBLE>::ToArray(ary, len, rarg);
          } else {
            double **ary;
            params.get(i, ary);
            C2Ruby<lwc::AT_DOUBLE>::ToArray(*ary, len, rarg);
          }
          break;
        }
        case lwc::AT_STRING: {
          if (!out) {
            char **ary;
            params.get(i, ary);
            C2Ruby<lwc::AT_STRING>::ToArray(ary, len, rarg);
          } else {
            char ***ary;
            params.get(i, ary);
            C2Ruby<lwc::AT_STRING>::ToArray(*ary, len, rarg);
          }
          break;
        }
        case lwc::AT_OBJECT: {
          if (!out) {
            lwc::Object **ary;
            params.get(i, ary);
            C2Ruby<lwc::AT_OBJECT>::ToArray(ary, len, rarg);
          } else {
            lwc::Object ***ary;
            params.get(i, ary);
            C2Ruby<lwc::AT_OBJECT>::ToArray(*ary, len, rarg);
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
          C2Ruby<lwc::AT_BOOL>::ToValue(val, rarg);
          break;
        }
        case lwc::AT_CHAR: {
          char val;
          params.get(i, val);
          C2Ruby<lwc::AT_CHAR>::ToValue(val, rarg);
          break;
        }
        case lwc::AT_UCHAR: {
          unsigned char val;
          params.get(i, val);
          C2Ruby<lwc::AT_UCHAR>::ToValue(val, rarg);
          break;
        }
        case lwc::AT_SHORT: {
          short val;
          params.get(i, val);
          C2Ruby<lwc::AT_SHORT>::ToValue(val, rarg);
          break;
        }
        case lwc::AT_USHORT: {
          unsigned short val;
          params.get(i, val);
          C2Ruby<lwc::AT_USHORT>::ToValue(val, rarg);
          break;
        }
        case lwc::AT_INT: {
          int val;
          params.get(i, val);
          C2Ruby<lwc::AT_INT>::ToValue(val, rarg);
          break;
        }
        case lwc::AT_UINT: {
          unsigned int val;
          params.get(i, val);
          C2Ruby<lwc::AT_UINT>::ToValue(val, rarg);
          break;
        }
        case lwc::AT_LONG: {
          long val;
          params.get(i, val);
          C2Ruby<lwc::AT_LONG>::ToValue(val, rarg);
          break;
        }
        case lwc::AT_ULONG: {
          unsigned long val;
          params.get(i, val);
          C2Ruby<lwc::AT_ULONG>::ToValue(val, rarg);
          break;
        }
        case lwc::AT_FLOAT: {
          float val;
          params.get(i, val);
          C2Ruby<lwc::AT_FLOAT>::ToValue(val, rarg);
          break;
        }
        case lwc::AT_DOUBLE: {
          double val;
          params.get(i, val);
          C2Ruby<lwc::AT_DOUBLE>::ToValue(val, rarg);
          break;
        }
        case lwc::AT_STRING: {
          char *val;
          params.get(i, val);
          C2Ruby<lwc::AT_STRING>::ToValue(val, rarg);
          break;
        }
        case lwc::AT_OBJECT: {
          lwc::Object *val;
          params.get(i, val);
          C2Ruby<lwc::AT_OBJECT>::ToValue(val, rarg);
          break;
        }
        default:
          throw std::runtime_error("Invalid argument type");
      }
    }
    
    args[cur] = rarg;
    ++cur;
  }
  
  // call ruby method
  VALUE rv = rb_funcall2(mSelf, rb_intern(name), ninputs, args);
  
  // convert Ruby outputs to C
  
  // params.get(idx, var) is not quite the same as: params.set(idx, var)
  // beware, the last is called by the caller to set the address of an output var
  // that will receive the value.
  
  cur = 0;
  bool array = (TYPE(rv) == T_ARRAY);
  
  for (size_t i=0; i<meth.numArgs(); ++i) {
    
    const lwc::Argument &arg = meth[i];
    
    if (arg.getDir() == lwc::AD_IN) {
      continue;
    }
    
    if (rv == Qnil) {
      throw std::runtime_error("Expected return value, got None");
    }
    if (cur > 0 && !array) {
      throw std::runtime_error("Expected array as return value (multiple output)");
    }
    
    // can be inout, out or return
    
    if (arg.arrayArg() != -1) {
      // array sizes handled when dealing with the array arg
      continue;
    }
    
    if (arg.isArray()) {
      
      size_t len = 0;
      
      VALUE crv = Qnil;
      
      if (arg.getDir() == lwc::AD_INOUT) {
        
        crv = args[inoutArgs[i]];
        
        len = inoutInSizes[i];
        
      } else {
        
        if (array) {
          crv = RARRAY(rv)->ptr[cur];
        } else {
          crv = rv;
        }
        ++cur;
        
        if (TYPE(crv) != T_ARRAY) {
          throw std::runtime_error("Expected array return argument");
        }
      }
      
      switch(arg.getType()) {
        case lwc::AT_BOOL: {
          bool **ary;
          params.get(i, ary);
          Ruby2C<lwc::AT_BOOL>::ToArray(crv, *ary, len);
          break;
        }
        case lwc::AT_CHAR: {
          char **ary;
          params.get(i, ary);
          Ruby2C<lwc::AT_CHAR>::ToArray(crv, *ary, len);
          break;
        }
        case lwc::AT_UCHAR: {
          unsigned char **ary;
          params.get(i, ary);
          Ruby2C<lwc::AT_UCHAR>::ToArray(crv, *ary, len);
          break;
        }
        case lwc::AT_SHORT: {
          short **ary;
          params.get(i, ary);
          Ruby2C<lwc::AT_SHORT>::ToArray(crv, *ary, len);
          break;
        }
        case lwc::AT_USHORT: {
          unsigned short **ary;
          params.get(i, ary);
          Ruby2C<lwc::AT_USHORT>::ToArray(crv, *ary, len);
          break;
        }
        case lwc::AT_INT: {
          int **ary;
          params.get(i, ary);
          Ruby2C<lwc::AT_INT>::ToArray(crv, *ary, len);
          break;
        }
        case lwc::AT_UINT: {
          unsigned int **ary;
          params.get(i, ary);
          Ruby2C<lwc::AT_UINT>::ToArray(crv, *ary, len);
          break;
        }
        case lwc::AT_LONG: {
          long **ary;
          params.get(i, ary);
          Ruby2C<lwc::AT_LONG>::ToArray(crv, *ary, len);
          break;
        }
        case lwc::AT_ULONG: {
          unsigned long **ary;
          params.get(i, ary);
          Ruby2C<lwc::AT_ULONG>::ToArray(crv, *ary, len);
          break;
        }
        case lwc::AT_FLOAT: {
          float **ary;
          params.get(i, ary);
          Ruby2C<lwc::AT_FLOAT>::ToArray(crv, *ary, len);
          break;
        }
        case lwc::AT_DOUBLE: {
          double **ary;
          params.get(i, ary);
          Ruby2C<lwc::AT_DOUBLE>::ToArray(crv, *ary, len);
          break;
        }
        case lwc::AT_STRING: {
          char ***ary;
          params.get(i, ary);
          // if inout ... might need to free stuffs
          // as ToArray will re-alloc but not necessarily free elements
          // len is in/out -> could use it to free the required number
          Ruby2C<lwc::AT_STRING>::ToArray(crv, *ary, len);
          break;
        }
        case lwc::AT_OBJECT: {
          lwc::Object ***ary;
          params.get(i, ary);
          Ruby2C<lwc::AT_OBJECT>::ToArray(crv, *ary, len);
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
      
      VALUE crv; // get from PyTuple or rv
      
      if (array) {
        crv = RARRAY(rv)->ptr[cur];
      } else {
        crv = rv;
      }
      ++cur;
      
      switch(arg.getType()) {
        case lwc::AT_BOOL: {
          bool *val;
          params.get(i, val);
          Ruby2C<lwc::AT_BOOL>::ToValue(crv, *val);
          break;
        }
        case lwc::AT_CHAR: {
          char *val;
          params.get(i, val);
          Ruby2C<lwc::AT_CHAR>::ToValue(crv, *val);
          break;
        }
        case lwc::AT_UCHAR: {
          unsigned char *val;
          params.get(i, val);
          Ruby2C<lwc::AT_UCHAR>::ToValue(crv, *val);
          break;
        }
        case lwc::AT_SHORT: {
          short *val;
          params.get(i, val);
          Ruby2C<lwc::AT_SHORT>::ToValue(crv, *val);
          break;
        }
        case lwc::AT_USHORT: {
          unsigned short *val;
          params.get(i, val);
          Ruby2C<lwc::AT_USHORT>::ToValue(crv, *val);
          break;
        }
        case lwc::AT_INT: {
          int *val;
          params.get(i, val);
          Ruby2C<lwc::AT_INT>::ToValue(crv, *val);
          break;
        }
        case lwc::AT_UINT: {
          unsigned int *val;
          params.get(i, val);
          Ruby2C<lwc::AT_UINT>::ToValue(crv, *val);
          break;
        }
        case lwc::AT_LONG: {
          long *val;
          params.get(i, val);
          Ruby2C<lwc::AT_LONG>::ToValue(crv, *val);
          break;
        }
        case lwc::AT_ULONG: {
          unsigned long *val;
          params.get(i, val);
          Ruby2C<lwc::AT_ULONG>::ToValue(crv, *val);
          break;
        }
        case lwc::AT_FLOAT: {
          float *val;
          params.get(i, val);
          Ruby2C<lwc::AT_FLOAT>::ToValue(crv, *val);
          break;
        }
        case lwc::AT_DOUBLE: {
          double *val;
          params.get(i, val);
          Ruby2C<lwc::AT_DOUBLE>::ToValue(crv, *val);
          break;
        }
        case lwc::AT_STRING: {
          char **val;
          params.get(i, val);
          // Ruby2C for string allocates ... does the arg specify this properly?
          Ruby2C<lwc::AT_STRING>::ToValue(crv, *val);
          break;
        }
        case lwc::AT_OBJECT: {
          lwc::Object **val;
          params.get(i, val);
          Ruby2C<lwc::AT_OBJECT>::ToValue(crv, *val);
          break;
        }
        default:
          throw std::runtime_error("Invalid argument type");
      }
    }
  }
  
}


}


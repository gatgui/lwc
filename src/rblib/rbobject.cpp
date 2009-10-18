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

#include <lwc/ruby/rbobject.h>
#include <lwc/ruby/convert.h>

namespace rb {

struct CallArgs {
  VALUE self;
  const char *name;
  int argc;
  VALUE *argv;
};

inline static VALUE WrappedCall(VALUE args) {
  CallArgs *a = (CallArgs*)args;
  return rb_funcall2(a->self, rb_intern(a->name), a->argc, a->argv);
}

// ---

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
        case lwc::AT_INT: {
          if (!out) {
            lwc::Integer sz;
            params.get(arg.arraySizeArg(), sz);
            len = size_t(sz);
          } else {
            lwc::Integer *sz;
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
        case lwc::AT_INT: {
          if (!out) {
            lwc::Integer *ary;
            params.get(i, ary);
            C2Ruby<lwc::AT_INT>::ToArray(ary, len, rarg);
          } else {
            lwc::Integer **ary;
            params.get(i, ary);
            C2Ruby<lwc::AT_INT>::ToArray(*ary, len, rarg);
          }
          break;
        }
        case lwc::AT_REAL: {
          if (!out) {
            lwc::Real *ary;
            params.get(i, ary);
            C2Ruby<lwc::AT_REAL>::ToArray(ary, len, rarg);
          } else {
            lwc::Real **ary;
            params.get(i, ary);
            C2Ruby<lwc::AT_REAL>::ToArray(*ary, len, rarg);
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
        case lwc::AT_INT: {
          lwc::Integer val;
          params.get(i, val);
          C2Ruby<lwc::AT_INT>::ToValue(val, rarg);
          break;
        }
        case lwc::AT_REAL: {
          lwc::Real val;
          params.get(i, val);
          C2Ruby<lwc::AT_REAL>::ToValue(val, rarg);
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
  
  //VALUE rv = rb_funcall2(mSelf, rb_intern(name), ninputs, args);
  CallArgs callargs;
  callargs.self = mSelf;
  callargs.name = name;
  callargs.argc = ninputs;
  callargs.argv = args;
  
  int err = 0;
  
  VALUE rv = rb_protect(WrappedCall, (VALUE)&callargs, &err);
  
  if (err != 0) {
    std::ostringstream oss;
    VALUE lasterr = rb_gv_get("$!");
    VALUE message = rb_obj_as_string(lasterr);
    oss << std::endl << "--- Ruby --- " << RSTRING(message)->ptr;
    if (!NIL_P(ruby_errinfo)) {
      oss << std::endl << "--- Ruby --- Backtrace:";
      VALUE ary = rb_funcall(ruby_errinfo, rb_intern("backtrace"), 0);
      for (int c=0; c<RARRAY(ary)->len; ++c) {
        oss << std::endl << "--- Ruby ---   from " << RSTRING(RARRAY(ary)->ptr[c])->ptr;
      }
    }
    oss << std::endl;
    throw std::runtime_error(oss.str().c_str());
  }
  
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
    
    if (arg.arrayArg() != -1) {
      // array sizes handled when dealing with the array arg
      continue;
    }
    
    //if (rv == Qnil) {
    //  throw std::runtime_error("Expected return value, got None");
    //}
    if (cur > 0 && !array) {
      throw std::runtime_error("Expected array as return value (multiple output)");
    }
    
    // can be inout, out or return
    
    if (arg.isArray()) {
      
      size_t len = 0;
      
      VALUE crv = Qnil;
      
      if (arg.getDir() == lwc::AD_INOUT) {
        
        crv = args[inoutArgs[i]];
        
        len = inoutInSizes[i];
        
      } else {
        
        if (rv == Qnil) {
          throw std::runtime_error("Expected return value, got nil");
        }
        
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
        case lwc::AT_INT: {
          lwc::Integer **ary;
          params.get(i, ary);
          Ruby2C<lwc::AT_INT>::ToArray(crv, *ary, len);
          break;
        }
        case lwc::AT_REAL: {
          lwc::Real **ary;
          params.get(i, ary);
          Ruby2C<lwc::AT_REAL>::ToArray(crv, *ary, len);
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
        case lwc::AT_INT: {
          lwc::Integer *sz;
          params.get(arg.arraySizeArg(), sz);
          *sz = (lwc::Integer) len;
          break;
        }
        default:
          throw std::runtime_error("Invalid array size type");
      }
      
    } else {
      
      VALUE crv; // get from PyTuple or rv
      
      if (array) {
        if (rv == Qnil) {
          throw std::runtime_error("Expected return value, got nil");
        }
        crv = RARRAY(rv)->ptr[cur];
      } else {
        crv = rv;
      }
      if (crv == Qnil && arg.getType() != lwc::AT_STRING && arg.getType() != lwc::AT_OBJECT) {
        throw std::runtime_error("Expected return value, got nil");
      }
      ++cur;
      
      switch(arg.getType()) {
        case lwc::AT_BOOL: {
          bool *val;
          params.get(i, val);
          Ruby2C<lwc::AT_BOOL>::ToValue(crv, *val);
          break;
        }
        case lwc::AT_INT: {
          lwc::Integer *val;
          params.get(i, val);
          Ruby2C<lwc::AT_INT>::ToValue(crv, *val);
          break;
        }
        case lwc::AT_REAL: {
          lwc::Real *val;
          params.get(i, val);
          Ruby2C<lwc::AT_REAL>::ToValue(crv, *val);
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


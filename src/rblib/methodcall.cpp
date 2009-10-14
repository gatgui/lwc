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

#include <lwc/ruby/convert.h>

// This needs to be a recursive call rather than a for loop inside
// because real parameters are created on the stack [important for return value]

namespace rb {

VALUE CallMethod(lwc::Object *o, const char *n, lwc::MethodParams &params, int cArg,
                 VALUE *args, size_t nargs, size_t rbArg, std::map<size_t,size_t> &arraySizes) {
  
  //std::cout << "rb::CallMethod(\"" << n << "\", " << cArg << ", " << nargs << ", " << rbArg << ")" << std::endl;
  const lwc::Method &m = params.getMethod();
  
  if (m.numArgs() == cArg) {
    
    if (rbArg != nargs) {
      rb_raise(rb_eRuntimeError, "Invalid arguments. None expected");
      return Qnil;
    }
    
    //std::cout << "  " << cArg << ": call C++ proxy" << std::endl;
    o->call(n, params);
    
    if (m.numArgs() == 0) {
      return Qnil;
    } else {
      return rb_ary_new();
    }
    
  } else {
    
    VALUE rv = Qnil;
    
    const lwc::Argument &ad = m[cArg];
    
    if (!ad.isArray() && ad.getDir() == lwc::AD_INOUT) {
      rb_raise(rb_eRuntimeError, "inout non array arguments not supported in ruby");
      return Qnil;
    }
    
    // PreCallArray will increment rbArg if necessarys
    size_t oldRbArg = rbArg;
    
    switch (ad.getType()) {
      case lwc::AT_BOOL: {
        if (ad.isArray()) {
          bool *ary=0;
          ParamConverter<lwc::AT_BOOL>::PreCallArray(ad, cArg, args, nargs, rbArg, arraySizes, ary);
          if (ad.getDir() == lwc::AD_IN) {
            params.set(cArg, ary);
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_BOOL>::PostCallArray(ad, cArg, args, nargs, oldRbArg, arraySizes, ary, rv);
          
        } else {
          bool val;
          ParamConverter<lwc::AT_BOOL>::PreCall(ad, cArg, args, nargs, rbArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            params.set(cArg, val);
          } else {
            params.set(cArg, &val);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_BOOL>::PostCall(ad, cArg, args, nargs, oldRbArg, arraySizes, val, rv);
        }
        break;
      }
      case lwc::AT_INT: {
        if (ad.isArray()) {
          lwc::Integer *ary=0;
          ParamConverter<lwc::AT_INT>::PreCallArray(ad, cArg, args, nargs, rbArg, arraySizes, ary);
          if (ad.getDir() == lwc::AD_IN) {
            params.set(cArg, ary);
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_INT>::PostCallArray(ad, cArg, args, nargs, oldRbArg, arraySizes, ary, rv);
          
        } else {
          lwc::Integer val;
          ParamConverter<lwc::AT_INT>::PreCall(ad, cArg, args, nargs, rbArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            params.set(cArg, val);
          } else {
            params.set(cArg, &val);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_INT>::PostCall(ad, cArg, args, nargs, oldRbArg, arraySizes, val, rv);
        }
        break;
      }
      case lwc::AT_REAL: {
        if (ad.isArray()) {
          lwc::Real *ary=0;
          ParamConverter<lwc::AT_REAL>::PreCallArray(ad, cArg, args, nargs, rbArg, arraySizes, ary);
          if (ad.getDir() == lwc::AD_IN) {
            params.set(cArg, ary);
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_REAL>::PostCallArray(ad, cArg, args, nargs, oldRbArg, arraySizes, ary, rv);
          
        } else {
          lwc::Real val;
          ParamConverter<lwc::AT_REAL>::PreCall(ad, cArg, args, nargs, rbArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            params.set(cArg, val);
          } else {
            params.set(cArg, &val);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_REAL>::PostCall(ad, cArg, args, nargs, oldRbArg, arraySizes, val, rv);
        }
        break;
      }
      case lwc::AT_STRING: {
        if (ad.isArray()) {
          char **ary=0;
          ParamConverter<lwc::AT_STRING>::PreCallArray(ad, cArg, args, nargs, rbArg, arraySizes, ary);
          if (ad.getDir() == lwc::AD_IN) {
            params.set(cArg, ary);
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_STRING>::PostCallArray(ad, cArg, args, nargs, oldRbArg, arraySizes, ary, rv);
          
        } else {
          char *val;
          ParamConverter<lwc::AT_STRING>::PreCall(ad, cArg, args, nargs, rbArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            params.set(cArg, val);
          } else {
            params.set(cArg, &val);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_STRING>::PostCall(ad, cArg, args, nargs, oldRbArg, arraySizes, val, rv);
        }
        break;
      }
      case lwc::AT_OBJECT: {
        if (ad.isArray()) {
          lwc::Object **ary=0;
          ParamConverter<lwc::AT_OBJECT>::PreCallArray(ad, cArg, args, nargs, rbArg, arraySizes, ary);
          if (ad.getDir() == lwc::AD_IN) {
            params.set(cArg, ary);
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_OBJECT>::PostCallArray(ad, cArg, args, nargs, oldRbArg, arraySizes, ary, rv);
          
        } else {
          lwc::Object *val;
          ParamConverter<lwc::AT_OBJECT>::PreCall(ad, cArg, args, nargs, rbArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            params.set(cArg, val);
          } else {
            params.set(cArg, &val);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_OBJECT>::PostCall(ad, cArg, args, nargs, oldRbArg, arraySizes, val, rv);
        }
        break;
      }
      default:
        rb_raise(rb_eRuntimeError, "Invalid argument type");
        return Qnil;
    }
    
    //if (rv != Qnil) {
    if (rv != Qnil && cArg == 0)  {
      size_t sz = RARRAY(rv)->len;
      if (sz == 0) {
        rv = Qnil;
        
      } else if (sz == 1) {
        rv = RARRAY(rv)->ptr[0];
      }
    }
    
    return rv;
  }
}

}


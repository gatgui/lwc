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

#include <lwc/ruby/convert.h>

// This needs to be a recursive call rather than a for loop inside
// because real parameters are created on the stack [important for return value]

namespace rb {

VALUE CallMethod(lwc::Object *o, const char *n, lwc::MethodParams &params, int cArg,
                 VALUE *args, VALUE kwargs, size_t nargs, size_t rbArg, std::map<size_t,size_t> &arraySizes) throw(std::runtime_error) {
  
  //std::cout << "rb::CallMethod(\"" << n << "\", " << cArg << ", " << nargs << ", " << rbArg << ")" << std::endl;
  const lwc::Method &m = params.getMethod();
  
  if (m.numArgs() == size_t(cArg)) {
    
    if (rbArg != nargs) {
      rb_raise(rb_eRuntimeError, "Invalid arguments. None expected");
      //return Qnil;
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
      //return Qnil;
    }
    
    // PreCallArray will increment rbArg if necessarys
    size_t oldRbArg = rbArg;
    std::string err = "";
    bool failed = false;
    
    switch (ad.getType()) {
      case lwc::AT_BOOL: {
        if (ad.isArray()) {
          bool *ary=0;
          const lwc::Argument &sad = m[ad.arraySizeArg()];
          if (ParamConverter<lwc::AT_BOOL>::PreCallArray(ad, cArg, sad, args, kwargs, nargs, rbArg, arraySizes, ary, err)) {
            try {
              if (ad.getDir() == lwc::AD_IN) {
                params.set(cArg, ary, false);
              } else {
                params.set(cArg, &ary, false);
              }
            } catch (std::exception &e) {
              err = e.what();
              failed = true;
            }
            if (!failed) {
              rv = CallMethod(o, n, params, cArg+1, args, kwargs, nargs, rbArg, arraySizes);
            }
            ParamConverter<lwc::AT_BOOL>::PostCallArray(ad, cArg, sad, args, kwargs, nargs, oldRbArg, arraySizes, ary, rv);
          } else {
            failed = true;
          }
          
        } else {
          bool val;
          if (ParamConverter<lwc::AT_BOOL>::PreCall(ad, cArg, args, kwargs, nargs, rbArg, arraySizes, val, err)) {
            try {
              if (ad.getDir() == lwc::AD_IN) {
                params.set(cArg, val, false);
              } else {
                params.set(cArg, &val, false);
              }
            } catch (std::exception &e) {
              err = e.what();
              failed = true;
            }
            if (!failed) {
              rv = CallMethod(o, n, params, cArg+1, args, kwargs, nargs, rbArg, arraySizes);
            }
            ParamConverter<lwc::AT_BOOL>::PostCall(ad, cArg, args, kwargs, nargs, oldRbArg, arraySizes, val, rv);
          } else {
            failed = true;
          }
        }
        break;
      }
      case lwc::AT_INT: {
        if (ad.isArray()) {
          lwc::Integer *ary=0;
          const lwc::Argument &sad = m[ad.arraySizeArg()];
          if (ParamConverter<lwc::AT_INT>::PreCallArray(ad, cArg, sad, args, kwargs, nargs, rbArg, arraySizes, ary, err)) {
            try {
              if (ad.getDir() == lwc::AD_IN) {
                params.set(cArg, ary, false);
              } else {
                params.set(cArg, &ary, false);
              }
            } catch (std::exception &e) {
              err = e.what();
              failed = true;
            }
            if (!failed) {
              rv = CallMethod(o, n, params, cArg+1, args, kwargs, nargs, rbArg, arraySizes);
            }
            ParamConverter<lwc::AT_INT>::PostCallArray(ad, cArg, sad, args, kwargs, nargs, oldRbArg, arraySizes, ary, rv);
          } else {
            failed = true;
          }
          
        } else {
          lwc::Integer val;
          if (ParamConverter<lwc::AT_INT>::PreCall(ad, cArg, args, kwargs, nargs, rbArg, arraySizes, val, err)) {
            try {
              if (ad.getDir() == lwc::AD_IN) {
                params.set(cArg, val, false);
              } else {
                params.set(cArg, &val, false);
              }
            } catch (std::exception &e) {
              err = e.what();
              failed = true;
            }
            if (!failed) {
              rv = CallMethod(o, n, params, cArg+1, args, kwargs, nargs, rbArg, arraySizes);
            }
            ParamConverter<lwc::AT_INT>::PostCall(ad, cArg, args, kwargs, nargs, oldRbArg, arraySizes, val, rv);
          } else {
            failed = true;
          }
        }
        break;
      }
      case lwc::AT_REAL: {
        if (ad.isArray()) {
          lwc::Real *ary=0;
          const lwc::Argument &sad = m[ad.arraySizeArg()];
          if (ParamConverter<lwc::AT_REAL>::PreCallArray(ad, cArg, sad, args, kwargs, nargs, rbArg, arraySizes, ary, err)) {
            try {
              if (ad.getDir() == lwc::AD_IN) {
                params.set(cArg, ary, false);
              } else {
                params.set(cArg, &ary, false);
              }
            } catch (std::exception &e) {
              err = e.what();
              failed = true;
            }
            if (!failed) {
              rv = CallMethod(o, n, params, cArg+1, args, kwargs, nargs, rbArg, arraySizes);
            }
            ParamConverter<lwc::AT_REAL>::PostCallArray(ad, cArg, sad, args, kwargs, nargs, oldRbArg, arraySizes, ary, rv);
          } else {
            failed = true;
          }
          
        } else {
          lwc::Real val;
          if (ParamConverter<lwc::AT_REAL>::PreCall(ad, cArg, args, kwargs, nargs, rbArg, arraySizes, val, err)) {
            try {
              if (ad.getDir() == lwc::AD_IN) {
                params.set(cArg, val, false);
              } else {
                params.set(cArg, &val, false);
              }
            } catch (std::exception &e) {
              err = e.what();
              failed = true;
            }
            if (!failed) {
              rv = CallMethod(o, n, params, cArg+1, args, kwargs, nargs, rbArg, arraySizes);
            }
            ParamConverter<lwc::AT_REAL>::PostCall(ad, cArg, args, kwargs, nargs, oldRbArg, arraySizes, val, rv);
          } else {
            failed = true;
          }
        }
        break;
      }
      case lwc::AT_STRING: {
        if (ad.isArray()) {
          char **ary=0;
          const lwc::Argument &sad = m[ad.arraySizeArg()];
          if (ParamConverter<lwc::AT_STRING>::PreCallArray(ad, cArg, sad, args, kwargs, nargs, rbArg, arraySizes, ary, err)) {
            try {
              if (ad.getDir() == lwc::AD_IN) {
                params.set(cArg, ary, false);
              } else {
                params.set(cArg, &ary, false);
              }
            } catch (std::exception &e) {
              err = e.what();
              failed = true;
            }
            if (!failed) {
              rv = CallMethod(o, n, params, cArg+1, args, kwargs, nargs, rbArg, arraySizes);
            }
            ParamConverter<lwc::AT_STRING>::PostCallArray(ad, cArg, sad, args, kwargs, nargs, oldRbArg, arraySizes, ary, rv);
          } else {
            failed = true;
          }
          
        } else {
          char *val;
          if (ParamConverter<lwc::AT_STRING>::PreCall(ad, cArg, args, kwargs, nargs, rbArg, arraySizes, val, err)) {
            try {
              if (ad.getDir() == lwc::AD_IN) {
                params.set(cArg, val, false);
              } else {
                params.set(cArg, &val, false);
              }
            } catch (std::exception &e) {
              err = e.what();
              failed = true;
            }
            if (!failed) {
              rv = CallMethod(o, n, params, cArg+1, args, kwargs, nargs, rbArg, arraySizes);
            }
            ParamConverter<lwc::AT_STRING>::PostCall(ad, cArg, args, kwargs, nargs, oldRbArg, arraySizes, val, rv);
          } else {
            failed = true;
          }
        }
        break;
      }
      case lwc::AT_OBJECT: {
        if (ad.isArray()) {
          lwc::Object **ary=0;
          const lwc::Argument &sad = m[ad.arraySizeArg()];
          if (ParamConverter<lwc::AT_OBJECT>::PreCallArray(ad, cArg, sad, args, kwargs, nargs, rbArg, arraySizes, ary, err)) {
            try {
              if (ad.getDir() == lwc::AD_IN) {
                params.set(cArg, ary, false);
              } else {
                params.set(cArg, &ary, false);
              }
            } catch (std::exception &e) {
              err = e.what();
              failed = true;
            }
            if (!failed) {
              rv = CallMethod(o, n, params, cArg+1, args, kwargs, nargs, rbArg, arraySizes);
            }
            ParamConverter<lwc::AT_OBJECT>::PostCallArray(ad, cArg, sad, args, kwargs, nargs, oldRbArg, arraySizes, ary, rv);
          } else {
            failed = true;
          }
          
        } else {
          lwc::Object *val=0;
          if (ParamConverter<lwc::AT_OBJECT>::PreCall(ad, cArg, args, kwargs, nargs, rbArg, arraySizes, val, err)) {
            try {
              if (ad.getDir() == lwc::AD_IN) {
                params.set(cArg, val, false);
              } else {
                params.set(cArg, &val, false);
              }
            } catch (std::exception &e) {
              err = e.what();
              failed = true;
            }
            if (!failed) {
              rv = CallMethod(o, n, params, cArg+1, args, kwargs, nargs, rbArg, arraySizes);
            }
            ParamConverter<lwc::AT_OBJECT>::PostCall(ad, cArg, args, kwargs, nargs, oldRbArg, arraySizes, val, rv);
          } else {
            failed = true;
          }
        }
        break;
      }
      default:
        rb_raise(rb_eRuntimeError, "Invalid argument type");
        return Qnil;
    }
    
    if (failed) {
      std::cout << "Return Qnil because of param convertion failure" << std::endl;
      rb_raise(rb_eRuntimeError, err.c_str());
      return Qnil;
    }
    
    //if (rv != Qnil) {
    if (rv != Qnil && cArg == 0)  {
      size_t sz = RARRAY(rv)->len;
      if (sz == 0) {
        rv = Qnil;
        
      } else if (sz == 1) {
        rv = RARRAY(rv)->ptr[0];
      
      } else {
        // reverse array
        size_t hl = sz / 2;
        for (size_t i=0; i<hl; ++i) {
          VALUE tmp = RARRAY(rv)->ptr[i];
          RARRAY(rv)->ptr[i] = RARRAY(rv)->ptr[sz-i-1];
          RARRAY(rv)->ptr[sz-i-1] = tmp;
        }
        
      }
    }
    
    return rv;
  }
}

}


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
    
    if (ad.isPtr()) {
      rb_raise(rb_eRuntimeError, "Pointer arguments not supported in ruby");
      return Qnil;
    }
    
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
            if (ad.isConst()) {
              params.set(cArg, (const bool*)ary);
            } else {
              params.set(cArg, ary);
            }
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_BOOL>::PostCallArray(ad, cArg, args, nargs, oldRbArg, arraySizes, ary, rv);
          
        } else {
          bool val;
          ParamConverter<lwc::AT_BOOL>::PreCall(ad, cArg, args, nargs, rbArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const bool)val);
            } else {
              params.set(cArg, val);
            }
          } else {
            params.set(cArg, &val);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_BOOL>::PostCall(ad, cArg, args, nargs, oldRbArg, arraySizes, val, rv);
        }
        break;
      }
      case lwc::AT_CHAR: {
        if (ad.isArray()) {
          char *ary=0;
          ParamConverter<lwc::AT_CHAR>::PreCallArray(ad, cArg, args, nargs, rbArg, arraySizes, ary);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const char*)ary);
            } else {
              params.set(cArg, ary);
            }
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_CHAR>::PostCallArray(ad, cArg, args, nargs, oldRbArg, arraySizes, ary, rv);
          
        } else {
          char val;
          ParamConverter<lwc::AT_CHAR>::PreCall(ad, cArg, args, nargs, rbArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const char)val);
            } else {
              params.set(cArg, val);
            }
          } else {
            params.set(cArg, &val);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_CHAR>::PostCall(ad, cArg, args, nargs, oldRbArg, arraySizes, val, rv);
        }
        break;
      }
      case lwc::AT_UCHAR: {
        if (ad.isArray()) {
          unsigned char *ary=0;
          ParamConverter<lwc::AT_UCHAR>::PreCallArray(ad, cArg, args, nargs, rbArg, arraySizes, ary);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const unsigned char*)ary);
            } else {
              params.set(cArg, ary);
            }
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_UCHAR>::PostCallArray(ad, cArg, args, nargs, oldRbArg, arraySizes, ary, rv);
          
        } else {
          unsigned char val;
          ParamConverter<lwc::AT_UCHAR>::PreCall(ad, cArg, args, nargs, rbArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const unsigned char)val);
            } else {
              params.set(cArg, val);
            }
          } else {
            params.set(cArg, &val);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_UCHAR>::PostCall(ad, cArg, args, nargs, oldRbArg, arraySizes, val, rv);
        }
        break;
      }
      case lwc::AT_SHORT: {
        if (ad.isArray()) {
          short *ary=0;
          ParamConverter<lwc::AT_SHORT>::PreCallArray(ad, cArg, args, nargs, rbArg, arraySizes, ary);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const short*)ary);
            } else {
              params.set(cArg, ary);
            }
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_SHORT>::PostCallArray(ad, cArg, args, nargs, oldRbArg, arraySizes, ary, rv);
          
        } else {
          short val;
          ParamConverter<lwc::AT_SHORT>::PreCall(ad, cArg, args, nargs, rbArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const short)val);
            } else {
              params.set(cArg, val);
            }
          } else {
            params.set(cArg, &val);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_SHORT>::PostCall(ad, cArg, args, nargs, oldRbArg, arraySizes, val, rv);
        }
        break;
      }
      case lwc::AT_USHORT: {
        if (ad.isArray()) {
          unsigned short *ary=0;
          ParamConverter<lwc::AT_USHORT>::PreCallArray(ad, cArg, args, nargs, rbArg, arraySizes, ary);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const unsigned short*)ary);
            } else {
              params.set(cArg, ary);
            }
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_USHORT>::PostCallArray(ad, cArg, args, nargs, oldRbArg, arraySizes, ary, rv);
          
        } else {
          unsigned short val;
          ParamConverter<lwc::AT_USHORT>::PreCall(ad, cArg, args, nargs, rbArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const unsigned short)val);
            } else {
              params.set(cArg, val);
            }
          } else {
            params.set(cArg, &val);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_USHORT>::PostCall(ad, cArg, args, nargs, oldRbArg, arraySizes, val, rv);
        }
        break;
      }
      case lwc::AT_INT: {
        if (ad.isArray()) {
          int *ary=0;
          ParamConverter<lwc::AT_INT>::PreCallArray(ad, cArg, args, nargs, rbArg, arraySizes, ary);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const int*)ary);
            } else {
              params.set(cArg, ary);
            }
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_INT>::PostCallArray(ad, cArg, args, nargs, oldRbArg, arraySizes, ary, rv);
          
        } else {
          int val;
          ParamConverter<lwc::AT_INT>::PreCall(ad, cArg, args, nargs, rbArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const int)val);
            } else {
              params.set(cArg, val);
            }
          } else {
            params.set(cArg, &val);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_INT>::PostCall(ad, cArg, args, nargs, oldRbArg, arraySizes, val, rv);
        }
        break;
      }
      case lwc::AT_UINT: {
        if (ad.isArray()) {
          unsigned int *ary=0;
          ParamConverter<lwc::AT_UINT>::PreCallArray(ad, cArg, args, nargs, rbArg, arraySizes, ary);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const unsigned int*)ary);
            } else {
              params.set(cArg, ary);
            }
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_UINT>::PostCallArray(ad, cArg, args, nargs, oldRbArg, arraySizes, ary, rv);
          
        } else {
          unsigned int val;
          ParamConverter<lwc::AT_UINT>::PreCall(ad, cArg, args, nargs, rbArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const unsigned int)val);
            } else {
              params.set(cArg, val);
            }
          } else {
            params.set(cArg, &val);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_UINT>::PostCall(ad, cArg, args, nargs, oldRbArg, arraySizes, val, rv);
        }
        break;
      }
      case lwc::AT_LONG: {
        if (ad.isArray()) {
          long *ary=0;
          ParamConverter<lwc::AT_LONG>::PreCallArray(ad, cArg, args, nargs, rbArg, arraySizes, ary);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const long*)ary);
            } else {
              params.set(cArg, ary);
            }
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_LONG>::PostCallArray(ad, cArg, args, nargs, oldRbArg, arraySizes, ary, rv);
          
        } else {
          long val;
          ParamConverter<lwc::AT_LONG>::PreCall(ad, cArg, args, nargs, rbArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const long)val);
            } else {
              params.set(cArg, val);
            }
          } else {
            params.set(cArg, &val);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_LONG>::PostCall(ad, cArg, args, nargs, oldRbArg, arraySizes, val, rv);
        }
        break;
      }
      case lwc::AT_ULONG: {
        if (ad.isArray()) {
          unsigned long *ary=0;
          ParamConverter<lwc::AT_ULONG>::PreCallArray(ad, cArg, args, nargs, rbArg, arraySizes, ary);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const unsigned long*)ary);
            } else {
              params.set(cArg, ary);
            }
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_ULONG>::PostCallArray(ad, cArg, args, nargs, oldRbArg, arraySizes, ary, rv);
          
        } else {
          unsigned long val;
          ParamConverter<lwc::AT_ULONG>::PreCall(ad, cArg, args, nargs, rbArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const unsigned long)val);
            } else {
              params.set(cArg, val);
            }
          } else {
            params.set(cArg, &val);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_ULONG>::PostCall(ad, cArg, args, nargs, oldRbArg, arraySizes, val, rv);
        }
        break;
      }
      case lwc::AT_FLOAT: {
        if (ad.isArray()) {
          float *ary=0;
          ParamConverter<lwc::AT_FLOAT>::PreCallArray(ad, cArg, args, nargs, rbArg, arraySizes, ary);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const float*)ary);
            } else {
              params.set(cArg, ary);
            }
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_FLOAT>::PostCallArray(ad, cArg, args, nargs, oldRbArg, arraySizes, ary, rv);
          
        } else {
          float val;
          ParamConverter<lwc::AT_FLOAT>::PreCall(ad, cArg, args, nargs, rbArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const float)val);
            } else {
              params.set(cArg, val);
            }
          } else {
            params.set(cArg, &val);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_FLOAT>::PostCall(ad, cArg, args, nargs, oldRbArg, arraySizes, val, rv);
        }
        break;
      }
      case lwc::AT_DOUBLE: {
        if (ad.isArray()) {
          double *ary=0;
          ParamConverter<lwc::AT_DOUBLE>::PreCallArray(ad, cArg, args, nargs, rbArg, arraySizes, ary);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const double*)ary);
            } else {
              params.set(cArg, ary);
            }
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_DOUBLE>::PostCallArray(ad, cArg, args, nargs, oldRbArg, arraySizes, ary, rv);
          
        } else {
          double val;
          ParamConverter<lwc::AT_DOUBLE>::PreCall(ad, cArg, args, nargs, rbArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const double)val);
            } else {
              params.set(cArg, val);
            }
          } else {
            params.set(cArg, &val);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_DOUBLE>::PostCall(ad, cArg, args, nargs, oldRbArg, arraySizes, val, rv);
        }
        break;
      }
      case lwc::AT_STRING: {
        if (ad.isArray()) {
          char **ary=0;
          ParamConverter<lwc::AT_STRING>::PreCallArray(ad, cArg, args, nargs, rbArg, arraySizes, ary);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const char**)ary);
            } else {
              params.set(cArg, ary);
            }
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_STRING>::PostCallArray(ad, cArg, args, nargs, oldRbArg, arraySizes, ary, rv);
          
        } else {
          char *val;
          ParamConverter<lwc::AT_STRING>::PreCall(ad, cArg, args, nargs, rbArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const char*)val);
            } else {
              params.set(cArg, val);
            }
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
            if (ad.isConst()) {
              params.set(cArg, (const lwc::Object**)ary);
            } else {
              params.set(cArg, ary);
            }
          } else {
            params.set(cArg, &ary);
          }
          rv = CallMethod(o, n, params, cArg+1, args, nargs, rbArg, arraySizes);
          ParamConverter<lwc::AT_OBJECT>::PostCallArray(ad, cArg, args, nargs, oldRbArg, arraySizes, ary, rv);
          
        } else {
          lwc::Object *val;
          ParamConverter<lwc::AT_OBJECT>::PreCall(ad, cArg, args, nargs, rbArg, arraySizes, val);
          if (ad.getDir() == lwc::AD_IN) {
            if (ad.isConst()) {
              params.set(cArg, (const lwc::Object*)val);
            } else {
              params.set(cArg, val);
            }
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


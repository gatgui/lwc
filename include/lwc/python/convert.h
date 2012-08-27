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

#ifndef __lwc_py_convert_h__
#define __lwc_py_convert_h__

#include <lwc/python/config.h>
#include <lwc/python/pobject.h>

namespace py {
  
  template <typename T> struct PythonType {
    static const char* Name() {return "unknown";}
    static bool Check(PyObject *obj) {return false;}
    static void ToC(PyObject *, T &) {}
    static void Dispose(T &) {}
  };
  template <> struct PythonType<bool> {
    static const char* Name() {return "boolean";}
    static bool Check(PyObject *obj) {return PyBool_Check(obj);}
    static void ToC(PyObject *obj, bool &val) {val = (obj == Py_True);}
    static void Dispose(bool &) {}
  };
  template <> struct PythonType<lwc::Integer> {
    static const char* Name() {return "integer";}
    static bool Check(PyObject *obj) {return PyInt_Check(obj);}
    static void ToC(PyObject *obj, lwc::Integer &val) {val = (lwc::Integer) PyInt_AsLong(obj);}
    static void Dispose(lwc::Integer &) {}
  };
  template <> struct PythonType<lwc::Real> {
    static const char* Name() {return "real";}
    static bool Check(PyObject *obj) {return (PyFloat_Check(obj) || PyInt_Check(obj));}
    static void ToC(PyObject *obj, lwc::Real &val) {
      val = (lwc::Real) (PyInt_Check(obj) ? PyInt_AsLong(obj) : PyFloat_AsDouble(obj));
    }
    static void Dispose(lwc::Real &) {}
  };
  template <> struct PythonType<char*> {
    static const char* Name() {return "string";}
    static bool Check(PyObject *obj) {return (obj == Py_None || PyString_Check(obj));}
    static void ToC(PyObject *obj, char* &val) {
      if (obj == Py_None) {
        val = 0;
      } else {
        char *str = PyString_AsString(obj);
#ifdef LWC_MEMTRACK
        val = (char*) lwc::memory::Alloc(strlen(str)+1, sizeof(char), 0, "PythonType<char*>::ToC");
#else
        val = (char*) lwc::memory::Alloc(strlen(str)+1, sizeof(char));
#endif
        strcpy(val, str);
      }
    }
    static void Dispose(char* &val) {
      if (val) {
        lwc::memory::Free((void*)val);
      }
    }
  };
  template <> struct PythonType<lwc::Object*> {
    static const char* Name() {return "LWC object";}
    static bool Check(PyObject *obj) {return (obj == Py_None || PyObject_TypeCheck(obj, &PyLWCObjectType));}
    static void ToC(PyObject *obj, lwc::Object* &val) {
      if (obj == Py_None) {
        val = 0;
      } else {
        PyLWCObject *ilo = (PyLWCObject*)obj;
        val = ilo->obj;
      }
    }
    static void Dispose(lwc::Object *&) {}
  };

  template <typename T> struct CType {
    static void ToPython(const T &, PyObject *&) {}
  };
  template <> struct CType<bool> {
    static void ToPython(const bool &val, PyObject *&obj) {
      obj = (val == true ? Py_True : Py_False);
      Py_INCREF(obj);
    }
  };
  template <> struct CType<lwc::Integer> {
    static void ToPython(const lwc::Integer &val, PyObject *&obj) {obj = PyInt_FromLong(long(val));}
  };
  template <> struct CType<lwc::Real> {
    static void ToPython(const lwc::Real &val, PyObject *&obj) {obj = PyFloat_FromDouble(val);}
  };
  template <> struct CType<char*> {
    static void ToPython(const char *val, PyObject *&obj) {
      if (!val) {
        obj = Py_None;
        Py_INCREF(obj);
      
      } else {
        obj = PyString_FromString(val);
      }
    }
  };
  template <> struct CType<lwc::Object*> {
    static void ToPython(const lwc::Object *val, PyObject *&obj) {
      if (!val) {
        obj = Py_None;
        Py_INCREF(obj);
        
      } else {
        if (!strcmp(val->getLoaderName(), "pyloader")) {
          obj = ((py::Object*)val)->self();
          Py_INCREF(obj);
        } else {
          obj = PyObject_CallObject((PyObject*)&PyLWCObjectType, NULL);
          SetObjectPointer((PyLWCObject*)obj, (lwc::Object*)val);
        }
      }
    }
  };

  template <lwc::Type T> struct Python2C {
    typedef typename lwc::Enum2Type<T>::Type Type;
    typedef typename lwc::Enum2Type<T>::Type* Array;
    static void ToValue(PyObject *obj, Type &val) {
      if (!PythonType<Type>::Check(obj)) {
        PyErr_Format(PyExc_RuntimeError, "Expected %s argument", PythonType<Type>::Name());
        return;
      }
      PythonType<Type>::ToC(obj, val);
    }
    static void DisposeValue(Type &val) {
      PythonType<Type>::Dispose(val);
    }
    static void ToArray(PyObject *obj, Array &ary, size_t &length) {
      if (!PyList_Check(obj)) {
        PyErr_SetString(PyExc_RuntimeError, "Expected list argument");
      }
      length = PyList_Size(obj);
      if (ary != 0 && length != 0) {
        // should dispose the values ? [not always though]
        // realloc should copy back the values though...
        for (size_t i=0; i<length; ++i) {
          PythonType<Type>::Dispose(ary[i]);
        }
      }
#ifdef LWC_MEMTRACK
      ary = (Array) lwc::memory::Alloc(length, sizeof(Type), (void*)ary, "Python2C::ToArray");
#else
      ary = (Array) lwc::memory::Alloc(length, sizeof(Type), (void*)ary);
#endif
      for (size_t i=0; i<length; ++i) {
        PyObject *item = PyList_GetItem(obj, i);
        if (!PythonType<Type>::Check(item)) {
          PyErr_Format(PyExc_RuntimeError, "Expected list of %ss argument", PythonType<Type>::Name());
          return;
        }
        PythonType<Type>::ToC(item, ary[i]);
      }
    }
    static void DisposeArray(Array &ary, size_t length) {
      for (size_t i=0; i<length; ++i) {
        PythonType<Type>::Dispose(ary[i]);
      }
      lwc::memory::Free((void*)ary);
    }
  };

  template <lwc::Type T> struct C2Python {
    typedef typename lwc::Enum2Type<T>::Type Type;
    typedef typename lwc::Enum2Type<T>::Type* Array;
    // beware: const Type & width Type == char* gives char * const &val
    static void ToValue(const Type &val, PyObject *&obj) {
      CType<Type>::ToPython(val, obj);
    }
    static void ToArray(const Array &ary, size_t length, PyObject *&obj) {
      if (obj != 0 && PyList_Check(obj)) {
        size_t sz = PyList_Size(obj);
        if (length > sz) {
          size_t d = length - sz;
          for (size_t i=0; i<d; ++i) {
            Py_INCREF(Py_None);
            PyList_Append(obj, Py_None);
          }
        } else if (length < sz) {
          PyList_SetSlice(obj, length, sz, NULL);
        }
      } else {
        if (obj) {
          Py_DECREF(obj);
        }
        obj = PyList_New(length);
      }
      for (size_t i=0; i<length; ++i) {
        PyObject *item = 0;
        CType<Type>::ToPython(ary[i], item);
        PyList_SetItem(obj, i, item);
      }
    }
  };

  template <lwc::Type T> struct ParamConverter {
    typedef typename lwc::Enum2Type<T>::Type Type;
    typedef typename lwc::Enum2Type<T>::Type* Array;
    template <typename TT>
    static bool GetDefaultValue(const lwc::Argument &desc, TT &val) {
      if (!desc.hasDefaultValue()) {
        return false;
      }
      
      const lwc::ArgumentValue &dv = desc.getRawDefaultValue();
      
      lwc::Type dt = desc.getType();
      
      if (desc.isArray()) {
        // if TT is a pointer, gcore::TypeTraits<TT>::Value will be the pointer type
        // that must match the argument type
        if (lwc::Type2Enum<typename gcore::TypeTraits<TT>::Value>::Enum != (int)dt) {
          std::cout << "Array type mismatch" << std::endl;
          return false;
        }
      } else {
        if (lwc::Type2Enum<TT>::Enum != (int)dt) {
          std::cout << "Type mismatch" << std::endl;
          return false;
        }
      }
      
      if (!desc.isArray()) {
        if (dt == lwc::AT_BOOL) {
          if (lwc::Convertion<bool, TT>::Possible()) {
            lwc::Convertion<bool, TT>::Do(dv.boolean, val);
            return true;
          }
        } else if (dt == lwc::AT_INT) {
          if (lwc::Convertion<lwc::Integer, TT>::Possible()) {
            lwc::Convertion<lwc::Integer, TT>::Do(dv.integer, val);
            return true;
          }
        } else if (dt == lwc::AT_REAL) {
          if (lwc::Convertion<lwc::Real, TT>::Possible()) {
            lwc::Convertion<lwc::Real, TT>::Do(dv.real, val);
            return true;
          }
        }
      } else {
        if (lwc::Convertion<void*, TT>::Possible()) {
          lwc::Convertion<void*, TT>::Do(dv.ptr, val);
          return true;
        }
      }
      
      return false;
    }
    static bool PreCall(const lwc::Argument &desc, size_t, PyObject *args, PyObject *kwargs, size_t &iarg, std::map<size_t,size_t> &arraySizes, Type &val) {
      if (desc.getDir() == lwc::AD_IN || desc.getDir() == lwc::AD_INOUT) {
        if (desc.arrayArg() >= 0) {
          unsigned long idx = (unsigned long) arraySizes[size_t(desc.arrayArg())];
          lwc::Convertion<unsigned long, Type>::Do(idx, val);
          
        } else {
          if (iarg >= size_t(PyTuple_Size(args))) {
            bool failed = true;
            if (desc.isNamed()) {
              PyObject *dv = (kwargs ? PyDict_GetItemString(kwargs, desc.getName().c_str()) : 0);
              if (dv != 0) {
                Python2C<T>::ToValue(dv, val);
                failed = false;
                
              } else if (desc.hasDefaultValue()) {
                failed = !GetDefaultValue(desc, val);
              }
            }
            if (failed) {
              std::cout << "PreCall: not enough argument, expected " << desc.toString() << std::endl;
              PyErr_SetString(PyExc_RuntimeError, "Not enough argument");
              return false;
            }
            
          } else {
            PyObject *pa = PyTuple_GetItem(args, iarg);
            Python2C<T>::ToValue(pa, val);
            ++iarg;
          }
        }
      }
      return true;
    }
    static void PostCall(const lwc::Argument &desc, size_t, PyObject *args, PyObject *kwargs, size_t &iarg, std::map<size_t,size_t> &arraySizes, Type &val, PyObject *&rv, bool callFailed) {
      
      bool dontDispose = false;
      if (iarg >= size_t(PyTuple_Size(args)) && desc.isNamed() &&
          (kwargs == 0 ||
           PyDict_GetItemString(kwargs, desc.getName().c_str()) == 0) &&
          desc.hasDefaultValue()) {
        dontDispose = true;
      }
      if (callFailed && desc.getDir() == lwc::AD_OUT) {
        dontDispose = true;
      }
       
      if (desc.getDir() == lwc::AD_IN) {
        if (!dontDispose) {
          Python2C<T>::DisposeValue(val);
        }
        
      } else {
        if (desc.arrayArg() >= 0) {
          arraySizes[size_t(desc.arrayArg())] = size_t(val);
          if (!dontDispose) {
            Python2C<T>::DisposeValue(val);
          }
          
        } else {
          if (rv == 0) {
            // rv == 0 when call failed
            if (!dontDispose) {
              Python2C<T>::DisposeValue(val);
            }
            
          } else {
            PyObject *obj = 0;
            if (!callFailed) {
              C2Python<T>::ToValue(val, obj);
            } else {
              obj = Py_None;
              Py_INCREF(obj);
            }
            if (!dontDispose) {
              Python2C<T>::DisposeValue(val);
            }
            Py_ssize_t ti = PyTuple_Size(rv);
            _PyTuple_Resize(&rv, ti+1);
            PyTuple_SetItem(rv, ti, obj);
          }
        }
      }
    }
    static bool PreCallArray(const lwc::Argument &desc, size_t idesc, const lwc::Argument &sdesc, PyObject *args, PyObject *kwargs, size_t &iarg, std::map<size_t,size_t> &arraySizes, Array &ary) {
      if (desc.getDir() == lwc::AD_IN || desc.getDir() == lwc::AD_INOUT) {
        size_t length;
        if (iarg >= size_t(PyTuple_Size(args))) {
          bool failed = true;
          if (desc.isNamed()) {
            PyObject *dv = (kwargs ? PyDict_GetItemString(kwargs, desc.getName().c_str()) : 0);
            if (dv != 0) {
              Python2C<T>::ToArray(dv, ary, length);
              arraySizes[idesc] = length;
              failed = false;
              
            } else if (desc.hasDefaultValue()) {
              failed = !GetDefaultValue(desc, ary);
              if (!failed) {
                lwc::Integer tmp;
                failed = !GetDefaultValue(sdesc, tmp);
                if (!failed) {
                  length = (size_t) tmp;
                }
              }
            }
          }
          if (failed) {
            std::cout << "PreCallArray: not enough argument, expected " << desc.toString() << std::endl;
            PyErr_SetString(PyExc_RuntimeError, "Not enough argument");
            return false;
          }
          
        } else {
          PyObject *pa = PyTuple_GetItem(args, iarg);
          Python2C<T>::ToArray(pa, ary, length);
          arraySizes[idesc] = length;
          ++iarg;
        }
      }
      return true;
    }
    static void PostCallArray(const lwc::Argument &desc, size_t idesc, const lwc::Argument &, PyObject *args, PyObject *kwargs, size_t &iarg, std::map<size_t,size_t> &arraySizes, Array &ary, PyObject *&rv, bool callFailed) {
      
      bool dontDispose = false;
      if (iarg >= size_t(PyTuple_Size(args)) && desc.isNamed() &&
          (kwargs == 0 ||
           PyDict_GetItemString(kwargs, desc.getName().c_str()) == 0) &&
          desc.hasDefaultValue()) {
        // only for ptr types
        dontDispose = true;
      }
      if (callFailed && desc.getDir() == lwc::AD_OUT) {
        dontDispose = true;
      }
      
      if (desc.getDir() == lwc::AD_IN) {
        if (!dontDispose) {
          Python2C<T>::DisposeArray(ary, arraySizes[idesc]);
        }
        
      } else {
        PyObject *obj = 0;
        if (!callFailed) {
          if (desc.getDir() == lwc::AD_INOUT) {
            obj = PyTuple_GetItem(args, iarg);
          }
          C2Python<T>::ToArray(ary, arraySizes[idesc], obj);
        } else {
          obj = Py_None;
          Py_INCREF(obj);
        }
        if (desc.getDir() != lwc::AD_INOUT) {
          if (!dontDispose) {
            Python2C<T>::DisposeArray(ary, arraySizes[idesc]);
          }
          if (rv != 0) {
            Py_ssize_t ti = PyTuple_Size(rv);
            _PyTuple_Resize(&rv, ti+1);
            PyTuple_SetItem(rv, ti, obj);
          }
        } else {
          if (!dontDispose) {
            Python2C<T>::DisposeArray(ary, arraySizes[idesc]);
          }
        }
      }
    }
  };

}

#endif



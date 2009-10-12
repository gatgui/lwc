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

#ifndef __lwc_py_convert_h__
#define __lwc_py_convert_h__

#include <lwc/python/config.h>

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
  template <> struct PythonType<char> {
    static const char* Name() {return "integer";}
    static bool Check(PyObject *obj) {return PyInt_Check(obj);}
    static void ToC(PyObject *obj, char &val) {val = (char) PyInt_AsLong(obj);}
    static void Dispose(char &) {}
  };
  template <> struct PythonType<unsigned char> {
    static const char* Name() {return "integer";}
    static bool Check(PyObject *obj) {return PyInt_Check(obj);}
    static void ToC(PyObject *obj, unsigned char &val) {val = (unsigned char) PyInt_AsLong(obj);}
    static void Dispose(unsigned char &) {}
  };
  template <> struct PythonType<short> {
    static const char* Name() {return "integer";}
    static bool Check(PyObject *obj) {return PyInt_Check(obj);}
    static void ToC(PyObject *obj, short &val) {val = (short) PyInt_AsLong(obj);}
    static void Dispose(short &) {}
  };
  template <> struct PythonType<unsigned short> {
    static const char* Name() {return "integer";}
    static bool Check(PyObject *obj) {return PyInt_Check(obj);}
    static void ToC(PyObject *obj, unsigned short &val) {val = (unsigned short) PyInt_AsLong(obj);}
    static void Dispose(unsigned short &) {}
  };
  template <> struct PythonType<int> {
    static const char* Name() {return "integer";}
    static bool Check(PyObject *obj) {return PyInt_Check(obj);}
    static void ToC(PyObject *obj, int &val) {val = (int) PyInt_AsLong(obj);}
    static void Dispose(int &) {}
  };
  template <> struct PythonType<unsigned int> {
    static const char* Name() {return "integer";}
    static bool Check(PyObject *obj) {return PyInt_Check(obj);}
    static void ToC(PyObject *obj, unsigned int &val) {val = (unsigned int) PyInt_AsLong(obj);}
    static void Dispose(unsigned int &) {}
  };
  template <> struct PythonType<long> {
    static const char* Name() {return "integer";}
    static bool Check(PyObject *obj) {return PyInt_Check(obj);}
    static void ToC(PyObject *obj, long &val) {val = (long) PyInt_AsLong(obj);}
    static void Dispose(long &) {}
  };
  template <> struct PythonType<unsigned long> {
    static const char* Name() {return "integer";}
    static bool Check(PyObject *obj) {return PyInt_Check(obj);}
    static void ToC(PyObject *obj, unsigned long &val) {val = (unsigned long) PyInt_AsLong(obj);}
    static void Dispose(unsigned long &) {}
  };
  template <> struct PythonType<float> {
    static const char* Name() {return "real";}
    static bool Check(PyObject *obj) {return (PyFloat_Check(obj) || PyInt_Check(obj));}
    static void ToC(PyObject *obj, float &val) {
      val = (float) (PyInt_Check(obj) ? PyInt_AsLong(obj) : PyFloat_AsDouble(obj));}
    static void Dispose(float &) {}
  };
  template <> struct PythonType<double> {
    static const char* Name() {return "real";}
    static bool Check(PyObject *obj) {return (PyFloat_Check(obj) || PyInt_Check(obj));}
    static void ToC(PyObject *obj, double &val) {
      val = (double) (PyInt_Check(obj) ? PyInt_AsLong(obj) : PyFloat_AsDouble(obj));}
    static void Dispose(double &) {}
  };
  template <> struct PythonType<char*> {
    static const char* Name() {return "string";}
    static bool Check(PyObject *obj) {return PyString_Check(obj);}
    static void ToC(PyObject *obj, char* &val) {
      char *str = PyString_AsString(obj);
      val = (char*) lwc::memory::Alloc(strlen(str)+1, sizeof(char));
      strcpy(val, str);
    }
    static void Dispose(char* &val) {
      lwc::memory::Free((void*)val);
    }
  };
  template <> struct PythonType<lwc::Object*> {
    static const char* Name() {return "LWC object";}
    static bool Check(PyObject *obj) {return PyObject_TypeCheck(obj, &PyLWCObjectType);}
    static void ToC(PyObject *obj, lwc::Object* &val) {
      PyLWCObject *ilo = (PyLWCObject*)obj;
      val = ilo->obj;
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
  template <> struct CType<char> {
    static void ToPython(const char &val, PyObject *&obj) {obj = PyInt_FromLong(val);}
  };
  template <> struct CType<unsigned char> {
    static void ToPython(const unsigned char &val, PyObject *&obj) {obj = PyInt_FromLong(val);}
  };
  template <> struct CType<short> {
    static void ToPython(const short &val, PyObject *&obj) {obj = PyInt_FromLong(val);}
  };
  template <> struct CType<unsigned short> {
    static void ToPython(const unsigned short &val, PyObject *&obj) {obj = PyInt_FromLong(val);}
  };
  template <> struct CType<int> {
    static void ToPython(const int &val, PyObject *&obj) {obj = PyInt_FromLong(val);}
  };
  template <> struct CType<unsigned int> {
    static void ToPython(const unsigned int &val, PyObject *&obj) {obj = PyInt_FromLong(val);}
  };
  template <> struct CType<long> {
    static void ToPython(const long &val, PyObject *&obj) {obj = PyInt_FromLong(val);}
  };
  template <> struct CType<unsigned long> {
    static void ToPython(const unsigned long &val, PyObject *&obj) {obj = PyInt_FromLong(val);}
  };
  template <> struct CType<float> {
    static void ToPython(const float &val, PyObject *&obj) {obj = PyFloat_FromDouble(val);}
  };
  template <> struct CType<double> {
    static void ToPython(const double &val, PyObject *&obj) {obj = PyFloat_FromDouble(val);}
  };
  template <> struct CType<char*> {
    static void ToPython(const char *val, PyObject *&obj) {obj = PyString_FromString(val);}
  };
  template <> struct CType<lwc::Object*> {
    static void ToPython(const lwc::Object *val, PyObject *&obj) {
      obj = PyObject_CallObject((PyObject*)&PyLWCObjectType, NULL);
      SetObjectPointer((PyLWCObject*)obj, (lwc::Object*)val);
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
      //ary = (Array) lwc::memory::Alloc(length, sizeof(Type));
      if (ary != 0 && length != 0) {
        // should dispose the values ? [not always though]
        // realloc should copy back the values though...
        for (size_t i=0; i<length; ++i) {
          PythonType<Type>::Dispose(ary[i]);
        }
      }
      ary = (Array) lwc::memory::Alloc(length, sizeof(Type), (void*)ary);
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
    static void PreCall(const lwc::Argument &desc, size_t idesc, PyObject *args, size_t &iarg, std::map<size_t,size_t> &arraySizes, Type &val) {
      if (desc.getDir() == lwc::AD_IN || desc.getDir() == lwc::AD_INOUT) {
        if (desc.arrayArg() != -1) {
          // this should only be executed for integer types
          val = (Type) arraySizes[desc.arrayArg()];
        } else {
          if (iarg >= PyTuple_Size(args)) {
            PyErr_SetString(PyExc_RuntimeError, "Not enough argument");
            return;
          }
          PyObject *pa = PyTuple_GetItem(args, iarg);
          Python2C<T>::ToValue(pa, val);
          ++iarg;
        }     
      }
    }
    static void PostCall(const lwc::Argument &desc, size_t idesc, PyObject *args, size_t &iarg, std::map<size_t,size_t> &arraySizes, Type &val, PyObject *&rv) {
      if (desc.getDir() == lwc::AD_IN) {
        Python2C<T>::DisposeValue(val);
        ++iarg;
      } else {
        if (desc.arrayArg() != -1) {
          arraySizes[desc.arrayArg()] = size_t(val);
          Python2C<T>::DisposeValue(val);
          
        } else {
          PyObject *obj = 0;
          C2Python<T>::ToValue(val, obj);
          Python2C<T>::DisposeValue(val);
          Py_ssize_t ti = PyTuple_Size(rv);
          _PyTuple_Resize(&rv, ti+1);
          PyTuple_SetItem(rv, ti, obj);
        }
      }
    }
    static void PreCallArray(const lwc::Argument &desc, size_t idesc, PyObject *args, size_t &iarg, std::map<size_t,size_t> &arraySizes, Array &ary) {
      if (desc.getDir() == lwc::AD_IN || desc.getDir() == lwc::AD_INOUT) {
        size_t length;
        if (iarg >= PyTuple_Size(args)) {
          PyErr_SetString(PyExc_RuntimeError, "Not enough argument");
          return;
        }
        PyObject *pa = PyTuple_GetItem(args, iarg);
        Python2C<T>::ToArray(pa, ary, length);
        arraySizes[idesc] = length;
        ++iarg;
      }
    }
    static void PostCallArray(const lwc::Argument &desc, size_t idesc, PyObject *args, size_t &iarg, std::map<size_t,size_t> &arraySizes, Array &ary, PyObject *&rv) {
      if (desc.getDir() == lwc::AD_IN) {
        Python2C<T>::DisposeArray(ary, arraySizes[idesc]);
      } else {
        PyObject *obj = 0;
        if (desc.getDir() == lwc::AD_INOUT) {
          obj = PyTuple_GetItem(args, iarg);
        }
        C2Python<T>::ToArray(ary, arraySizes[idesc], obj);
        if (desc.getDir() != lwc::AD_INOUT) {
          //if (desc.isAllocated()) {
          Python2C<T>::DisposeArray(ary, arraySizes[idesc]);
          //}
          Py_ssize_t ti = PyTuple_Size(rv);
          _PyTuple_Resize(&rv, ti+1);
          PyTuple_SetItem(rv, ti, obj);
        } else {
          Python2C<T>::DisposeArray(ary, arraySizes[idesc]);
        }
      }
    }
  };

}

#endif



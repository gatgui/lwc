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
#include <lwc/loader.h>
#include <lwc/factory.h>
#if !defined(_WIN32) && !defined(__APPLE__)
# include <dlfcn.h>
#endif

class PFactory : public lwc::Factory {
  public:
  
    PFactory() {
    }
    
    virtual ~PFactory() {
      std::map<std::string, TypeEntry>::iterator it = mTypes.begin();
      while (it != mTypes.end()) {
        Py_DECREF(it->second.klass);
        delete it->second.methods;
        if (it->second.desc) {
          delete[] it->second.desc;
        }
        ++it;
      }
    }
    
    virtual const lwc::MethodsTable* getMethods(const char *typeName) {
      std::map<std::string, TypeEntry>::iterator it = mTypes.find(typeName);
      if (it == mTypes.end()) {
        return 0;
      } else {
        return it->second.methods;
      }
    }
    
    virtual lwc::Object* create(const char *typeName) {
      std::map<std::string, TypeEntry>::iterator it = mTypes.find(typeName);
      if (it != mTypes.end()) {
        PyObject *pyObj = PyObject_CallObject(it->second.klass, NULL);
        py::Object *obj = new py::Object(pyObj);
        //SetObjectPointer((PyLWCObject*)pyObj, obj);
        ((py::PyLWCObject*)pyObj)->obj = obj;
        return obj;
      } else {
        return 0;
      }
    }
    
    virtual const char* getDescription(const char *typeName) {
      std::map<std::string, TypeEntry>::iterator it = mTypes.find(typeName);
      if (it != mTypes.end()) {
        return it->second.desc;
      } else {
        return 0;
      }
    }
    
    virtual std::string docString(const char *typeName, const std::string &indent="") {
      std::map<std::string, TypeEntry>::iterator it = mTypes.find(typeName);
      if (it != mTypes.end()) {
        std::ostringstream oss;
        oss << indent << typeName << ":" << std::endl;
        oss << indent << "  " << getDescription(typeName) << std::endl;
        oss << std::endl;
        oss << it->second.methods->docString(indent+"  ") << std::endl;
        return oss.str();
      } else {
        return "";
      }
    }
    
    virtual void destroy(lwc::Object *o) {
      if (o) {
        delete o;
      }
    }
    
    virtual bool isSingleton(const char *typeName) {
      std::map<std::string, TypeEntry>::iterator it = mTypes.find(typeName);
      if (it != mTypes.end()) {
        return it->second.singleton;
      } else {
        return false;
      }
    }
    
    bool addType(const char *name, PyObject *klass) {
      
      bool singleton = false;
      char *desc = NULL;
      
      PyObject *sg = PyObject_GetAttrString(klass, "Singleton");
      if (!sg) {
        PyErr_Clear();
      } else {
        if (PyBool_Check(sg)) {
          singleton = (sg == Py_True);
        }
        Py_DECREF(sg);
      }
      
      PyObject *de = PyObject_GetAttrString(klass, "Description");
      if (!de) {
        PyErr_Clear();
      } else {
        if (PyString_Check(de)) {
          char *tmp = PyString_AsString(de);
          desc = new char[strlen(tmp)+1];
          strcpy(desc, tmp);
        }
        Py_DECREF(de);
      }
      
      PyObject *mt = PyObject_GetAttrString(klass, "Methods");
      
      if (!mt || !PyDict_Check(mt)) {
        std::cout << "pyloader: Missing \"Methods\" class member" << std::endl;
        Py_XDECREF(mt);
        return false;
      }
      
      if (PyDict_Size(mt) == 0) {
        std::cout << "pyloader: No keys in \"Methods\" dict" << std::endl;
        Py_DECREF(mt);
        return false;
      }
      
      lwc::MethodsTable *typeMethods = 0;
      
      PyObject *key;
      PyObject *value;
      Py_ssize_t pos = 0;
      
      while (PyDict_Next(mt, &pos, &key, &value)) {
        
        if (!PyString_Check(key) || !PyTuple_Check(value)) {
          std::cout << "pyloader: \"Methods\" must be a dict of tuples" << std::endl;
          continue;
        }
        
        bool add = true;
        char *mname = PyString_AsString(key);
        char *mdesc = NULL;
        
        long n = PyTuple_Size(value);
        if (n < 1 || n > 2) {
          std::cout << "pyloader: \"Methods\" dict values must be tuples with 1 to 2 elements" << std::endl;
          continue;
        }
        
        PyObject *args = PyTuple_GetItem(value, 0);
        if (!PyList_Check(args)) {
          std::cout << "pyloader: \"Methods\" dict values' tuple first element must be a list" << std::endl;
          continue;
        }
        
        if (n == 2) {
          PyObject *pdesc = PyTuple_GetItem(value, 1);
          if (!PyString_Check(pdesc)) {
            std::cout << "pyloader: \"Methods\" dict values' tuple second element must be a string" << std::endl;
            continue;
          }
          mdesc = PyString_AsString(pdesc);
        }
        
        lwc::Method meth;
        
        if (mdesc) {
          meth.setDescription(mdesc);
        }
        
        n = PyList_Size(args);
        
        for (long i=0; i<n; ++i) {
          PyObject *arg = PyList_GetItem(args, i);
          Py_ssize_t ts = PyTuple_Size(arg);
          if (!PyTuple_Check(arg) || ts < 2 || ts > 6) {
            std::cout << "pyloader: Arguments must be tuples with 2 to 6 elements" << std::endl;
            add = false;
            break;
          }
          lwc::Argument a;
          a.setDir(lwc::Direction(PyInt_AsLong(PyTuple_GetItem(arg, 0))));
          a.setType(lwc::Type(PyInt_AsLong(PyTuple_GetItem(arg, 1))));
          if (ts >= 3) {
            a.setArraySizeArg(PyInt_AsLong(PyTuple_GetItem(arg, 2)));
          }
          if (ts >= 4) {
            PyObject *ahd = PyTuple_GetItem(arg, 3);
            if (!PyBool_Check(ahd)) {
              std::cout << "pyloader: Argument's tuple 4th element must be a boolean" << std::endl;
              add = false;
              break;
            }
            if (ahd == Py_True) {
              if (ts < 5) {
                std::cout << "pyloader: Argument is missing default value" << std::endl;
                add = false;
                break;
              } else {
                PyObject *pdv = PyTuple_GetItem(arg, 4);
                py::SetArgDefault(a, pdv);
              }
            }
            // hasDefault (yes, no)
            // if yes -> must have at least 5
          }
          if (ts >= 6) {
            PyObject *aname = PyTuple_GetItem(arg, 5);
            if (!PyString_Check(aname)) {
              std::cout << "pyloader: Arguments name must be a string" << std::endl;
              add = false;
              break;
            }
            a.setName(PyString_AsString(aname));
          }
          try {
            meth.addArg(a);
          } catch (std::exception &e) {
            std::cout << "pyloader: " << e.what() << std::endl;
            add = false;
            break;
          }
        }
        
        if (!add) {
          std::cout << "pyloader: Skipped method (1) \"" << mname << "\" for type \"" << name << "\"" << std::endl;
          continue;
        
        } else {
          if (!typeMethods) {
            PyTypeObject *to = (PyTypeObject*) klass;
            Py_ssize_t nbases = PyTuple_Size(to->tp_bases);
            if (nbases > 1) {
              std::cout << "pyloader: Skipped type \"" << name << "\" (Multiple inheritance not supported)" << std::endl;
              return false;
            }
            if (nbases == 1 && PyTuple_GetItem(to->tp_bases, 0) != (PyObject*)&py::PyLWCObjectType) {
              PyObject *pto = PyTuple_GetItem(to->tp_bases, 0);
              std::map<std::string, TypeEntry>::iterator it = mTypes.begin();
              while (it != mTypes.end()) {
                if (it->second.klass == pto) {
                  typeMethods = new lwc::MethodsTable(it->second.methods);
                  break;
                }
                ++it;
              }
              if (!typeMethods) {
                std::cout << "pyloader: Skipped type \"" << name << "\" (Super class not registered yet)" << std::endl;
                return false;
              }
            } else {
              typeMethods = new lwc::MethodsTable();
            }
          }
          try {
            typeMethods->addMethod(mname, meth);
          } catch (std::runtime_error &) {
            std::cout << "pyloader: Skipped method (2) \"" << mname << "\" for type \"" << name << "\"" << std::endl;
            continue;
          }
        }
      }
      
      Py_DECREF(mt);
      
      if (typeMethods) {
        if (typeMethods->numMethods() == 0) {
          std::cout << "pyloader: MethodsTable is empty for type \"" << name << "\"" << std::endl;
          delete typeMethods;
          return false;
        } else {
          TypeEntry te;
          Py_INCREF(klass);
          te.klass = klass;
          te.singleton = singleton;
          te.methods = typeMethods;
          te.desc = desc;
          mTypes[name] = te;
          return true;
        }
      } else {
        std::cout << "pyloader: MethodsTable not allocated for type \"" << name << "\"" << std::endl;
        return false;
      }
      
    }
    
  protected:
    
    struct TypeEntry {
      PyObject *klass;
      bool singleton;
      const char *desc;
      lwc::MethodsTable *methods;
    };
    
    std::map<std::string, TypeEntry> mTypes;
};

class PLoader : public lwc::Loader {
  public:
    
    PLoader() {
      mFactory = new PFactory();
    }
    
    virtual ~PLoader() {
      delete mFactory;
      for (size_t i=0; i<mPyModules.size(); ++i) {
        Py_DECREF(mPyModules[i]);
      }
    }
    
    void addToSysPath(const gcore::Path &dirname) {
      PyObject *modname = PyString_FromString("sys");
      PyObject *mod = PyImport_Import(modname);
      Py_DECREF(modname);
      if (mod) {
        PyObject *path = PyObject_GetAttrString(mod, "path");
        if (path) {
          long sz = PyList_Size(path);
          for (long i=0; i<sz; ++i) {
            PyObject *item = PyList_GetItem(path, i);
            char *n = PyString_AsString(item);
            if (dirname == n) {
              Py_DECREF(path);
              Py_DECREF(mod);
              return;
            }
          }
          PyList_Append(path, PyString_FromString(dirname.fullname('/').c_str()));
          Py_DECREF(path);
        }
        Py_DECREF(mod);
      }
    }
    
    virtual bool canLoad(const gcore::Path &path) {
      return path.checkExtension("py");
    }
    
    virtual void load(const gcore::Path &path, lwc::Registry *reg) {
      
      std::string modulename = path.basename();
      modulename = modulename.substr(0, modulename.length()-3);
      
      gcore::Path dirname = path;
      dirname.pop();
      dirname.makeAbsolute().normalize();
      
      addToSysPath(dirname);
      
      PyObject *pymodname;
      pymodname = PyString_FromString(modulename.c_str());
      PyObject *mod = PyImport_Import(pymodname);
      Py_DECREF(pymodname);
      
      if (!mod) {
        std::cout << "pyloader: Could not load python module" << std::endl;
        PyErr_Print();
        return;
      }
      
      PyObject *getCountFunc = PyObject_GetAttrString(mod, "LWC_ModuleGetTypeCount");
      if (!getCountFunc || !PyCallable_Check(getCountFunc)) {
        if (getCountFunc) {
          Py_DECREF(getCountFunc);
        }
        Py_DECREF(mod);
        return;
      }
      
      PyObject *getNameFunc = PyObject_GetAttrString(mod, "LWC_ModuleGetTypeName");
      if (!getNameFunc || !PyCallable_Check(getNameFunc)) {
        Py_DECREF(getCountFunc);
        if (getNameFunc) {
          Py_DECREF(getNameFunc);
        }
        Py_DECREF(mod);
        return;
      }
      
      PyObject *getClassFunc = PyObject_GetAttrString(mod, "LWC_ModuleGetTypeClass");
      if (!getClassFunc || !PyCallable_Check(getClassFunc)) {
        Py_DECREF(getCountFunc);
        Py_DECREF(getNameFunc);
        if (getClassFunc) {
          Py_DECREF(getClassFunc);
        }
        Py_DECREF(mod);
        return;
      }
      
      PyObject *count = PyObject_CallObject(getCountFunc, NULL);
      long n = PyInt_AsLong(count);
      Py_DECREF(count);
      
      PyObject *args = PyTuple_New(1);
      
      for (long i=0; i<n; ++i) {
        // will this DECREF the old index?
        PyTuple_SetItem(args, 0, PyInt_FromLong(i));
        
        PyObject *pname = PyObject_CallObject(getNameFunc, args);
        PyObject *klass = PyObject_CallObject(getClassFunc, args);
        
        if (!klass) {
          std::cout << "pyloader: No class object for type" << std::endl;
          continue;
        }
        
        char *tn = PyString_AsString(pname);
        
        if (!reg->hasType(tn)) {
          if (mFactory->addType(tn, klass)) {
            registerType(tn, mFactory, reg);
          } else {
            std::cout << "pyloader: Invalid type \"" << tn << "\"" << std::endl;
          }
        } else {
          std::cout << "pyloader: Type \"" << tn << "\" already registered" << std::endl;
        }
        
        Py_DECREF(pname);
        Py_DECREF(klass);
      }
      
      Py_DECREF(args);
      Py_DECREF(getCountFunc);
      Py_DECREF(getNameFunc);
      Py_DECREF(getClassFunc);
      
      mPyModules.push_back(mod);
    }
    
    virtual const char* getName() const {
      return "pyloader";
    }
  
  private:
  
    PFactory *mFactory;
    std::vector<PyObject*> mPyModules;
};

// ---

static bool WasInitialized = false;

extern "C" {

#ifdef _WIN32
__declspec(dllexport)
#else
__attribute__ ((visibility ("default")))
#endif
  lwc::Loader* LWC_CreateLoader(const char*, void*) {
    if (Py_IsInitialized()) {
      WasInitialized = true;
    } else {
      #if !defined(_WIN32) && !defined(__APPLE__)
      // On Ubunty (9.04 at least), without this hack, python binary modules fail to load
      // After a little search on the web, it seems that Ubuntu's python is compiled a weird way
      char ver[32];
      sprintf(ver, "%.1f", PY_VER);
      std::string pyso = "libpython";
      pyso += ver;
      pyso += ".so";
      dlopen((char*) pyso.c_str(), RTLD_LAZY|RTLD_GLOBAL);
      #endif
      WasInitialized = false;
      Py_SetProgramName("lwc_python");
      //PyEval_InitThreads();
      Py_Initialize();
    }
    
    lwc::Loader *l = new PLoader();
    return l;
  }

#ifdef _WIN32
__declspec(dllexport)
#else
__attribute__ ((visibility ("default")))
#endif
  void LWC_DestroyLoader(lwc::Loader *l) {
    if (l) {
      delete l;
    }
    if (!WasInitialized) {
      Py_Finalize();
    }
  }

}


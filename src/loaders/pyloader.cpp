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
#include <lwc/loader.h>
#include <lwc/factory.h>
#include <lwc/file.h>


class PFactory : public lwc::Factory {
  public:
  
    PFactory() {
    }
    
    virtual ~PFactory() {
      std::map<std::string, TypeEntry>::iterator it = mTypes.begin();
      while (it != mTypes.end()) {
        Py_DECREF(it->second.klass);
        delete it->second.methods;
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
    
    virtual void destroy(lwc::Object *o) {
      if (o) {
        delete o;
      }
    }
    
    bool addType(const char *name, PyObject *klass) {
      
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
        
        if (!PyString_Check(key) || !PyList_Check(value)) {
          std::cout << "pyloader: \"Methods\" must be a dict if lists" << std::endl;
          continue;
        }
        
        char *mname = PyString_AsString(key);
        long n = PyList_Size(value);
        bool add = true;
        lwc::Method meth;
        
        for (long i=0; i<n; ++i) {
          PyObject *arg = PyList_GetItem(value, i);
          Py_ssize_t ts = PyTuple_Size(arg);
          if (!PyTuple_Check(arg) || ts < 2 || ts > 5) {
            std::cout << "pyloader: Arguments must be tuples with 2 to 5 elements" << std::endl;
            add = false;
            break;
          }
          lwc::Argument a;
          a.setDir(lwc::Direction(PyInt_AsLong(PyTuple_GetItem(arg, 0))));
          a.setType(lwc::Type(PyInt_AsLong(PyTuple_GetItem(arg, 1))));
          //if (ts >= 3) {
          //  a.setAllocated(PyTuple_GetItem(arg, 2) == Py_True);
          //}
          if (ts >= 3) {
            a.setConst(PyTuple_GetItem(arg, 2) == Py_True);
          }
          if (ts >= 4) {
            a.setArray(PyTuple_GetItem(arg, 3) == Py_True);
          }
          if (ts >= 5) {
            a.setArraySizeArg(PyInt_AsLong(PyTuple_GetItem(arg, 4)));
          }
          meth.addArg(a);
        }
        
        if (!add) {
          std::cout << "pyloader: Skipped method \"" << mname << "\" for type \"" << name << "\"" << std::endl;
          continue;
        
        } else {
          if (!typeMethods) {
            typeMethods = new lwc::MethodsTable();
          }
          try {
            typeMethods->addMethod(mname, meth);
          } catch (std::runtime_error &e) {
            std::cout << "pyloader: Skipped method \"" << mname << "\" for type \"" << name << "\"" << std::endl;
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
          te.methods = typeMethods;
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
    
    void addToSysPath(const std::string &dirname) {
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
            if (lwc::file::IsSamePath(dirname, n)) {
              Py_DECREF(path);
              Py_DECREF(mod);
              return;
            }
          }
          PyList_Append(path, PyString_FromString(dirname.c_str()));
          Py_DECREF(path);
        }
        Py_DECREF(mod);
      }
    }
    
    virtual bool canLoad(const std::string &path) {
      return lwc::file::CheckFileExtension(path, ".py");
    }
    
    virtual void load(const std::string &path, lwc::Registry *reg) {
      
      std::string modulename = lwc::file::Basename(path);
      modulename = modulename.substr(0, modulename.length()-3);
      
      std::string dirname = lwc::file::NormalizePath(lwc::file::MakeAbsolutePath(lwc::file::Dirname(path)));
      
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
#endif
  lwc::Loader* LWC_CreateLoader(const char*, void*) {
    if (Py_IsInitialized()) {
      WasInitialized = true;
    } else {
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


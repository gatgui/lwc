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

#ifndef __lwc_method_h__
#define __lwc_method_h__

#include <lwc/argument.h>
#include <map>

namespace lwc {
  
  class LWC_API Object;
  class LWC_API MethodPointer;
  class LWC_API MethodParams;
  
  struct LWC_API MethodDecl {
    const char *name;
    Integer nargs;
    ArgumentDecl args[LWC_MAX_ARGS];
    MethodPointer *ptr;
    const char *desc;
  };
  
#define LWC_NUMMETHODS(Ary) (sizeof(Ary) / sizeof(lwc::MethodDecl))
#define LWC_METHOD(Class, Name) new lwc::TMethodPointer<Class>(&Class::Name)
  
  // Ugly work around to have things work on windows
  //   if pointer type is (void (Object::*)()) we cannot assign (void (Box::*)()) to it
  // Need a better scheme, this makes each objects do allocation for each of its method
  // and code for setting up methods is butt-ugly
  class LWC_API MethodPointer {
    public:
      MethodPointer();
      virtual ~MethodPointer();
  };
  
  template <class T> class TMethodPointer : public MethodPointer {
    public:
      TMethodPointer(void (T::*ptr)(MethodParams &))
        : MethodPointer(), mPtr(ptr) {
      }
      virtual ~TMethodPointer() {
      }
      inline void call(T *self, MethodParams &params) {
        (self->*mPtr)(params);
      }
    protected:
      void (T::*mPtr)(MethodParams &);
  };
  
  // IMPORTANT NODE:
  // Method DOES NOT OWN the MethodPointer object
  class LWC_API Method {
    
    public:
      
      friend class Object;
      
      Method();
      Method(const Method &rhs);
      ~Method();
      
      Method& operator=(const Method &rhs);
      
      std::string toString() const;
      std::string docString(const std::string &indent="") const;
      
      void addArg(const Argument &arg) throw(std::runtime_error);
      inline size_t numArgs() const {return mArgs.size();}
      inline size_t numPositionalArgs() const {return mNumPArgs;}
      size_t namedArgIndex(const char *name) const throw(std::runtime_error);
      
      inline void setPointer(MethodPointer *ptr) {mPtr = ptr;}
      inline MethodPointer* getPointer() const {return mPtr;}
      
      inline void setDescription(const char *desc) {mDesc = (desc == 0 ? "" : desc);}
      inline const char* getDescription() const {return mDesc.c_str();}
      
      inline const Argument& operator[](size_t idx) const {return mArgs[idx];}
      inline Argument& operator[](size_t idx) {return mArgs[idx];}
      
      //inline const Argument& operator[](const char *name) const {return mArgs[namedArgIndex(name)];}
      //inline Argument& operator[](const char *name) {return mArgs[namedArgIndex(name)];}
      
      void validateArgs() throw(std::runtime_error);
      
    private:
      
      std::vector<Argument> mArgs;
      MethodPointer *mPtr;
      std::string mDesc;
      size_t mNumPArgs;
      std::map<std::string, size_t> mNArgIndices;
  };
  
  class LWC_API MethodsTable {
    public:
      
      MethodsTable(const MethodsTable *parent=0);
      ~MethodsTable();
      
      inline const Method* findMethod(const char *name) const {
        std::map<std::string, Method>::const_iterator it = mTable.find(name);
        return (it == mTable.end() ? (mParent ? mParent->findMethod(name) : 0) : &(it->second));
      }
      
      size_t numMethods() const;
      
      std::string toString() const;
      std::string docString(const std::string &indent="") const;
      
      size_t availableMethods(std::vector<std::string> &methodNames) const;
      
      void addMethod(const char *name, const Method &m, bool override=false) throw(std::runtime_error);
      
      void fromDeclaration(const MethodDecl *decl, size_t n, bool override=false) throw(std::runtime_error);
      
    protected:
      
      std::map<std::string, Method> mTable;
      const MethodsTable *mParent;
  };
  
  class LWC_API MethodParams {
    public:
      
      MethodParams(const Method &m);
      MethodParams(const MethodParams &rhs);
      ~MethodParams();
      
      inline const Method& getMethod() const {
        return mMethod;
      }
      
      // get/set raw arguments
      
      inline const ArgumentValue& rawget(size_t i, bool positionalOnly=true) const throw(std::runtime_error) {
        if (( positionalOnly && i >= mMethod.numPositionalArgs()) ||
            (!positionalOnly && i >= mMethod.numArgs())) {
          std::ostringstream oss;
          oss << "MethodParams::rawget: Invalid method argument index " << i;
          throw std::runtime_error(oss.str());
        }
        return mParams[i];
      }
      
      inline void rawset(size_t i, const ArgumentValue &v, bool positionalOnly=true) throw(std::runtime_error) {
        if (( positionalOnly && i >= mMethod.numPositionalArgs()) ||
            (!positionalOnly && i >= mMethod.numArgs())) {
          std::ostringstream oss;
          oss << "MethodParams::rawget: Invalid method argument index " << i;
          throw std::runtime_error(oss.str());
        }
        mParams[i] = v;
      }
      
      inline const ArgumentValue& rawgetn(const char *name) const throw(std::runtime_error) {
        return mParams[mMethod.namedArgIndex(name)];
      }
      
      inline void rawsetn(const char *name, const ArgumentValue &v) throw(std::runtime_error) {
        mParams[mMethod.namedArgIndex(name)] = v;
      }
      
      // get/set positional arguments
      
      template <typename T>
      void set(size_t i, T value, bool positionalOnly=true) throw(std::runtime_error) {
        if (( positionalOnly && i >= mMethod.numPositionalArgs()) ||
            (!positionalOnly && i >= mMethod.numArgs())) {
          std::ostringstream oss;
          oss << "MethodParams::set: Invalid method argument index " << i;
          throw std::runtime_error(oss.str());
        }
        _set(i, value);
      }
      
      template <typename T>
      void get(size_t i, T &value, bool positionalOnly=true) throw(std::runtime_error)  {
        if (( positionalOnly && i >= mMethod.numPositionalArgs()) ||
            (!positionalOnly && i >= mMethod.numArgs())) {
          std::ostringstream oss;
          oss << "MethodParams::get: Invalid method argument index " << i;
          throw std::runtime_error(oss.str());
        }
        _get(i, value);
      }
      
      template <typename T>
      T get(size_t i, bool positionalOnly=true) throw(std::runtime_error)  {
        if (( positionalOnly && i >= mMethod.numPositionalArgs()) ||
            (!positionalOnly && i >= mMethod.numArgs())) {
          std::ostringstream oss;
          oss << "MethodParams::get: Invalid method argument index " << i;
          throw std::runtime_error(oss.str());
        }
        T value;
        _get(i, value);
        return value;
      }
      
      // get/set named arguments
      
      template <typename T>
      void setn(const char *name, T value) throw(std::runtime_error) {
        _set(mMethod.namedArgIndex(name), value);
      }
      
      template <typename T>
      void getn(const char *name, T &value) throw(std::runtime_error) {
        _get(mMethod.namedArgIndex(name), value);
      }
      
      template <typename T>
      T getn(const char *name) throw(std::runtime_error) {
        T value;
        _get(mMethod.namedArgIndex(name), value);
        return value;
      }
      
    private:
      
      MethodParams();
      MethodParams& operator=(const MethodParams&);
      
      template <typename T>
      void _set(size_t i, T value) throw(std::runtime_error) {
        std::string err;
        if (!details::GetSet<typename gcore::NoRefOrConst<T>::Type>::Set(mMethod[i], value, mParams[i], err)) {
          std::ostringstream oss;
          oss << "MethodParams::_set: Method argument " << i << ": " << err;
          throw std::runtime_error(oss.str());
        }
      }
      
      template <typename T>
      void _get(size_t i, T &value) throw(std::runtime_error)  {
        std::string err;
        if (!details::GetSet<typename gcore::NoRefOrConst<T>::Type>::Get(mMethod[i], mParams[i], value, err)) {
          std::ostringstream oss;
          oss << "MethodParams::_get: Method argument " << i << ": " << err;
          throw std::runtime_error(oss.str());
        }
      }
      
      template <typename T>
      T _get(size_t i) throw(std::runtime_error)  {
        T value;
        get(i, value);
        return value;
      }
      
    private:
      
      const Method &mMethod;
      ArgumentValue mParams[LWC_MAX_ARGS];
  };
  
}

#endif



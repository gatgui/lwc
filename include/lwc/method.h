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
    ArgumentDecl args[16];
    MethodPointer *ptr;
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
      
      void addArg(const Argument &arg) throw(std::runtime_error);
      inline size_t numArgs() const {return mArgs.size();}
      
      inline void setPointer(MethodPointer *ptr) {mPtr = ptr;}
      inline MethodPointer* getPointer() const {return mPtr;}
      
      inline const Argument& operator[](size_t idx) const {return mArgs[idx];}
      inline Argument& operator[](size_t idx) {return mArgs[idx];}
      
      void validateArgs() throw(std::runtime_error);
      
    private:
      
      std::vector<Argument> mArgs;
      MethodPointer *mPtr;
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
      
      template <typename T>
      void set(size_t i, T value) throw(std::runtime_error) {
        if (i >= mMethod.numArgs()) {
          std::ostringstream oss;
          oss << "Invalid method argument index " << i;
          throw std::runtime_error(oss.str());
        }
        std::string err;
        if (!details::GetSet<T>::Set(mMethod[i], value, mParams[i], err)) {
          std::ostringstream oss;
          oss << "Method argument " << i << ": " << err;
          throw std::runtime_error(oss.str());
        }
      }
      
      template <typename T>
      void get(size_t i, T &value) throw(std::runtime_error)  {
        if (i >= mMethod.numArgs()) {
          std::ostringstream oss;
          oss << "Invalid method argument index " << i;
          throw std::runtime_error(oss.str());
        }
        std::string err;
        if (!details::GetSet<T>::Get(mMethod[i], mParams[i], value, err)) {
          std::ostringstream oss;
          oss << "Method argument " << i << ": " << err;
          throw std::runtime_error(oss.str());
        }
      }
      
      template <typename T>
      T get(size_t i) throw(std::runtime_error)  {
        if (i >= mMethod.numArgs()) {
          std::ostringstream oss;
          oss << "Invalid method argument index " << i;
          throw std::runtime_error(oss.str());
        }
        T value;
        std::string err;
        if (!details::GetSet<T>::Get(mMethod[i], mParams[i], value, err)) {
          std::ostringstream oss;
          oss << "Method argument " << i << ": " << err;
          throw std::runtime_error(oss.str());
        }
        return value;
      }
      
    private:
      
      MethodParams();
      MethodParams& operator=(const MethodParams&);
      
    private:
      
      const Method &mMethod;
      ArgumentValue mParams[16];
  };
  
}

#endif



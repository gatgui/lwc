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
  
  
  namespace details {
    
    // general case (plain types)
    template <typename T> struct GetSet {
      static bool Get(const Argument &arg, ArgumentValue &src, T &dst, std::string &err) {
        err = "";
        if (arg.indirectionLevel() != 0) {
          err ="Expected 0 level of indirection";
          return false;
        }
        switch (arg.getType()) {
          case AT_BOOL: {
            if (!Convertion<bool, typename NoRefOrConst<T>::Type>::Possible()) {
              err = "Invalid argument type";
              return false;
            }
            Convertion<bool, typename NoRefOrConst<T>::Type>::Do(src.boolean, dst);
            return true;
          }
          case AT_INT: {
            if (!Convertion<Integer, typename NoRefOrConst<T>::Type>::Possible()) {
              err = "Invalid argument type";
              return false;
            }
            Convertion<Integer, typename NoRefOrConst<T>::Type>::Do(src.integer, dst);
            return true;
          }
          case AT_REAL: {
            if (!Convertion<Real, typename NoRefOrConst<T>::Type>::Possible()) {
              err = "Invalid argument type";
              return false;
            }
            Convertion<Real, typename NoRefOrConst<T>::Type>::Do(src.real, dst);
            return true;
          }
          case AT_STRING: {
            if (!Convertion<char*, typename NoRefOrConst<T>::Type>::Possible()) {
              err = "Invalid argument type";
              return false;
            }
            char *val = (char*) src.ptr;
            Convertion<char*, typename NoRefOrConst<T>::Type>::Do(val, dst);
            return true;
          }
          case AT_OBJECT: {
            if (!Convertion<Object*, typename NoRefOrConst<T>::Type>::Possible()) {
              err = "Invalid argument type";
              return false;
            }
            lwc::Object *val = (lwc::Object*) src.ptr;
            Convertion<Object*, typename NoRefOrConst<T>::Type>::Do(val, dst);
            return true;
          }
          default:
            err = "Invalid argument type";
            return false;
        }
      }
      static bool Set(const Argument &arg, T &src, ArgumentValue &dst, std::string &err) {
        err = "";
        if (arg.indirectionLevel() != 0) {
          err ="Expected 0 level of indirection";
          return false;
        }
        switch (arg.getType()) {
          case AT_BOOL: {
            if (!Convertion<typename NoRefOrConst<T>::Type, bool>::Possible()) {
              err = "Invalid argument type";
              return false;
            }
            Convertion<typename NoRefOrConst<T>::Type, bool>::Do(src, dst.boolean);
            return true;
          }
          case AT_INT: {
            if (!Convertion<typename NoRefOrConst<T>::Type, Integer>::Possible()) {
              err = "Invalid argument type";
              return false;
            }
            Convertion<typename NoRefOrConst<T>::Type, Integer>::Do(src, dst.integer);
            return true;
          }
          case AT_REAL: {
            if (!Convertion<typename NoRefOrConst<T>::Type, Real>::Possible()) {
              err = "Invalid argument type";
              return false;
            }
            Convertion<typename NoRefOrConst<T>::Type, Real>::Do(src, dst.real);
            return true;
          }
          case AT_STRING: {
            if (!Convertion<typename NoRefOrConst<T>::Type, char*>::Possible()) {
              err = "Invalid argument type";
              return false;
            }
            char *val = 0;
            Convertion<typename NoRefOrConst<T>::Type, char*>::Do(src, val);
            dst.ptr = (void*) val;
            return true;
          }
          case AT_OBJECT: {
            if (!Convertion<typename NoRefOrConst<T>::Type, Object*>::Possible()) {
              err = "Invalid argument type";
              return false;
            }
            lwc::Object *val = 0;
            Convertion<typename NoRefOrConst<T>::Type, Object*>::Do(src, val);
            dst.ptr = (void*) val;
            return true;
          }
          default:
            err = "Invalid argument type";
            return false;
        }
      }
    };
    template <typename T> struct GetSet<T*> {
      static bool Get(const Argument &arg, ArgumentValue &src, T* &dst, std::string &err) {
        err = "";
        // use typetraits?
        if (arg.indirectionLevel() != 1) {
          err ="Expected 1 level of indirection";
          return false;
        }
        if (Type2Enum<T>::Enum != int(arg.getType())) {
          err ="Invalid type";
          return false;
        }
        // check constness?
        dst = (T*) src.ptr;
        return true;
      }
      static bool Set(const Argument &arg, T* &src, ArgumentValue &dst, std::string &err) {
        err = "";
        // use typetraits?
        if (arg.indirectionLevel() != 1) {
          err ="Expected 1 level of indirection";
          return false;
        }
        if (Type2Enum<T>::Enum != int(arg.getType())) {
          err ="Invalid type";
          return false;
        }
        // check constness ?
        dst.ptr = (void*) src;
        return true;
      }
    };
    template <typename T> struct GetSet<T**> {
      static bool Get(const Argument &arg, ArgumentValue &src, T** &dst, std::string &err) {
        err = "";
        // use typetraits?
        if (arg.indirectionLevel() != 2) {
          err = "Expected 2 level of indirection";
          return false;
        }
        if (Type2Enum<T>::Enum != int(arg.getType())) {
          err = "Invalid type";
          return false;
        }
        // check constness?
        dst = (T**) src.ptr;
        return true;
      }
      static bool Set(const Argument &arg, T** &src, ArgumentValue &dst, std::string &err) {
        err = "";
        // use typetraits?
        if (arg.indirectionLevel() != 2) {
          err = "Expected 2 level of indirection";
          return false;
        }
        if (Type2Enum<T>::Enum != int(arg.getType())) {
          err = "Invalid type";
          return false;
        }
        // check constness?
        dst.ptr = (void*) src;
        return true;
      }
    };
    
    // string specual case
    template <> struct GetSet<char*> {
      static bool Get(const Argument &arg, ArgumentValue &src, char* &dst, std::string &err) {
        err = "";
        // use typetraits?
        if (arg.indirectionLevel() != 0) {
          err = "Expected 2 level of indirection";
          return false;
        }
        if (arg.getType () != AT_STRING) {
          err = "Invalid type";
          return false;
        }
        // check constness?
        dst = (char*) src.ptr;
        return true;
      }
      static bool Set(const Argument &arg, char* &src, ArgumentValue &dst, std::string &err) {
        err = "";
        // use typetraits?
        if (arg.indirectionLevel() != 0) {
          err = "Expected 2 level of indirection";
          return false;
        }
        if (arg.getType() != AT_STRING) {
          err = "Invalid type";
          return false;
        }
        // check constness?
        dst.ptr = (void*) src;
        return true;
      }
    };
    template <> struct GetSet<char**> {
      static bool Get(const Argument &arg, ArgumentValue &src, char** &dst, std::string &err) {
        err = "";
        // use typetraits?
        if (arg.indirectionLevel() != 1) {
          err = "Expected 2 level of indirection";
          return false;
        }
        if (arg.getType () != AT_STRING) {
          err = "Invalid type";
          return false;
        }
        // check constness?
        dst = (char**) src.ptr;
        return true;
      }
      static bool Set(const Argument &arg, char** &src, ArgumentValue &dst, std::string &err) {
        err = "";
        // use typetraits?
        if (arg.indirectionLevel() != 1) {
          err = "Expected 2 level of indirection";
          return false;
        }
        if (arg.getType() != AT_STRING) {
          err = "Invalid type";
          return false;
        }
        // check constness?
        dst.ptr = (void*) src;
        return true;
      }
    };
    template <> struct GetSet<char***> {
      static bool Get(const Argument &arg, ArgumentValue &src, char*** &dst, std::string &err) {
        err = "";
        // use typetraits?
        if (arg.indirectionLevel() != 2) {
          err = "Expected 2 level of indirection";
          return false;
        }
        if (arg.getType () != AT_STRING) {
          err = "Invalid type";
          return false;
        }
        // check constness?
        dst = (char***) src.ptr;
        return true;
      }
      static bool Set(const Argument &arg, char*** &src, ArgumentValue &dst, std::string &err) {
        err = "";
        // use typetraits?
        if (arg.indirectionLevel() != 2) {
          err = "Expected 2 level of indirection";
          return false;
        }
        if (arg.getType() != AT_STRING) {
          err = "Invalid type";
          return false;
        }
        // check constness?
        dst.ptr = (void*) src;
        return true;
      }
    };
    
    // object special case
    template <> struct GetSet<Object*> {
      static bool Get(const Argument &arg, ArgumentValue &src, Object* &dst, std::string &err) {
        err = "";
        // use typetraits?
        if (arg.indirectionLevel() != 0) {
          err = "Expected 1 level of indirection";
          return false;
        }
        if (arg.getType() != AT_OBJECT) {
          err = "Invalid type";
          return false;
        }
        // check constness?
        dst = (Object*) src.ptr;
        return true;
      }
      static bool Set(const Argument &arg, Object* &src, ArgumentValue &dst, std::string &err) {
        err = "";
        // use typetraits?
        if (arg.indirectionLevel() != 0) {
          err = "Expected 1 level of indirection";
          return false;
        }
        if (arg.getType() != AT_OBJECT) {
          err = "Invalid type";
          return false;
        }
        // check constness?
        dst.ptr = (void*) src;
        return true;
      }
    };
    template <> struct GetSet<Object**> {
      static bool Get(const Argument &arg, ArgumentValue &src, Object** &dst, std::string &err) {
        err = "";
        // use typetraits?
        if (arg.indirectionLevel() != 1) {
          err = "Expected 1 level of indirection";
          return false;
        }
        if (arg.getType() != AT_OBJECT) {
          err = "Invalid type";
          return false;
        }
        // check constness?
        dst = (Object**) src.ptr;
        return true;
      }
      static bool Set(const Argument &arg, Object** &src, ArgumentValue &dst, std::string &err) {
        err = "";
        // use typetraits?
        if (arg.indirectionLevel() != 1) {
          err = "Expected 1 level of indirection";
          return false;
        }
        if (arg.getType() != AT_OBJECT) {
          err = "Invalid type";
          return false;
        }
        // check constness?
        dst.ptr = (void*) src;
        return true;
      }
    };
    template <> struct GetSet<Object***> {
      static bool Get(const Argument &arg, ArgumentValue &src, Object*** &dst, std::string &err) {
        err = "";
        // use typetraits?
        if (arg.indirectionLevel() != 2) {
          err = "Expected 2 level of indirection";
          return false;
        }
        if (arg.getType() != AT_OBJECT) {
          err = "Invalid type";
          return false;
        }
        // check constness?
        dst = (Object***) src.ptr;
        return true;
      }
      static bool Set(const Argument &arg, Object*** &src, ArgumentValue &dst, std::string &err) {
        err = "";
        // use typetraits?
        if (arg.indirectionLevel() != 2) {
          err = "Expected 2 level of indirection";
          return false;
        }
        if (arg.getType() != AT_OBJECT) {
          err = "Invalid type";
          return false;
        }
        // check constness?
        dst.ptr = (void*) src;
        return true;
      }
    };
    
    // empty special case
    template <> struct GetSet<Empty> {
      static bool Get(const Argument &, ArgumentValue &, Empty &, std::string &err) {
        err = "Empty is not a valid argument type";
        return false;
      }
      static bool Set(const Argument &, Empty &, ArgumentValue &, std::string &err) {
        err = "Empty is not a valid argument type";
        return false;
      }
    };
  }
  
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
      
      MethodsTable();
      ~MethodsTable();
      
      inline const Method* findMethod(const char *name) const {
        std::map<std::string, Method>::const_iterator it = mTable.find(name);
        return (it == mTable.end() ? 0 : &(it->second));
      }
      
      inline Method* findMethod(const char *name) {
        std::map<std::string, Method>::iterator it = mTable.find(name);
        return (it == mTable.end() ? 0 : &(it->second));
      }
      
      inline size_t numMethods() const {
        return mTable.size();
      }
      
      std::string toString() const;
      
      size_t availableMethods(std::vector<std::string> &methodNames) const;
      
      void addMethod(const char *name, const Method &m, bool override=false) throw(std::runtime_error);
      
      void fromDeclaration(const MethodDecl *decl, size_t n, bool override=false) throw(std::runtime_error);
      
    protected:
      
      std::map<std::string, Method> mTable;
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



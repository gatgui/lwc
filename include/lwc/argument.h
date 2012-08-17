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

#ifndef __lwc_argument_h__
#define __lwc_argument_h__

#include <lwc/config.h>
#include <lwc/types.h>
#include <gcore/typetraits.h>

namespace lwc {
  
  class LWC_API Object;
  class LWC_API Method;
  
  enum Direction {
    AD_IN = 0,
    AD_OUT,
    AD_INOUT
  };
  
  struct LWC_API ArgumentDecl {
    Direction dir;
    Type type;
    Integer arylen;
    const char *name; // NULL by default
  };
  
  typedef union {
    bool boolean;
    Integer integer;
    Real real;
    void *ptr;
  } ArgumentValue;
  
  class LWC_API Argument {
    
    public:
      
      friend class Object;
      friend class Method;
      
      Argument();
      Argument(Direction d, Type t, Integer lenidx=-1, const char *name=0);
      Argument(const Argument &rhs);
      ~Argument();
      
      Argument& operator=(const Argument &rhs);
      
      void fromDeclaration(const ArgumentDecl &decl);
      
      std::string toString() const;
      
      Argument& setDir(Direction d);
      Argument& setType(Type t);
      Argument& setArraySizeArg(Integer idx);
      Argument& setArrayArg(Integer idx);
      Argument& setName(const char *name);
      template <typename T>
      Argument& setDefaultValue(T def) throw(std::runtime_error);
      
      inline Direction getDir() const {return mDir;}
      inline Type getType() const {return mType;}
      inline bool isArray() const {return mArray;}
      inline Integer arraySizeArg() const {return mArraySizeArg;}
      inline Integer arrayArg() const {return mArrayArg;}
      inline int indirectionLevel() const {return mIndirectionLevel;}
      inline const std::string& getName() const {return mName;}
      inline bool isNamed() const {return (mName.length() > 0);}
      inline bool hasDefaultValue() const {return mHasDefault;}
      template <typename T>
      void getDefaultValue(T &def) const throw(std::runtime_error);
      template <typename T>
      T getDefaultValue() const throw(std::runtime_error);
      
      // default value?
      
    private:
      
      Type mType;
      Direction mDir;
      bool mArray;
      Integer mArraySizeArg;
      Integer mArrayArg;
      
      int mIndirectionLevel;
      
      std::string mName;
      bool mHasDefault;
      ArgumentValue mDefaultValue;
  };
  
  // ---
  
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
            if (!Convertion<bool, typename gcore::NoRefOrConst<T>::Type>::Possible()) {
              err = "Invalid argument type";
              return false;
            }
            Convertion<bool, typename gcore::NoRefOrConst<T>::Type>::Do(src.boolean, dst);
            return true;
          }
          case AT_INT: {
            if (!Convertion<Integer, typename gcore::NoRefOrConst<T>::Type>::Possible()) {
              err = "Invalid argument type";
              return false;
            }
            Convertion<Integer, typename gcore::NoRefOrConst<T>::Type>::Do(src.integer, dst);
            return true;
          }
          case AT_REAL: {
            if (!Convertion<Real, typename gcore::NoRefOrConst<T>::Type>::Possible()) {
              err = "Invalid argument type";
              return false;
            }
            Convertion<Real, typename gcore::NoRefOrConst<T>::Type>::Do(src.real, dst);
            return true;
          }
          case AT_STRING: {
            if (!Convertion<char*, typename gcore::NoRefOrConst<T>::Type>::Possible()) {
              err = "Invalid argument type";
              return false;
            }
            char *val = (char*) src.ptr;
            Convertion<char*, typename gcore::NoRefOrConst<T>::Type>::Do(val, dst);
            return true;
          }
          case AT_OBJECT: {
            if (!Convertion<Object*, typename gcore::NoRefOrConst<T>::Type>::Possible()) {
              err = "Invalid argument type";
              return false;
            }
            lwc::Object *val = (lwc::Object*) src.ptr;
            Convertion<Object*, typename gcore::NoRefOrConst<T>::Type>::Do(val, dst);
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
            if (!Convertion<typename gcore::NoRefOrConst<T>::Type, bool>::Possible()) {
              err = "Invalid argument type";
              return false;
            }
            Convertion<typename gcore::NoRefOrConst<T>::Type, bool>::Do(src, dst.boolean);
            return true;
          }
          case AT_INT: {
            if (!Convertion<typename gcore::NoRefOrConst<T>::Type, Integer>::Possible()) {
              err = "Invalid argument type";
              return false;
            }
            Convertion<typename gcore::NoRefOrConst<T>::Type, Integer>::Do(src, dst.integer);
            return true;
          }
          case AT_REAL: {
            if (!Convertion<typename gcore::NoRefOrConst<T>::Type, Real>::Possible()) {
              err = "Invalid argument type";
              return false;
            }
            Convertion<typename gcore::NoRefOrConst<T>::Type, Real>::Do(src, dst.real);
            return true;
          }
          case AT_STRING: {
            if (!Convertion<typename gcore::NoRefOrConst<T>::Type, char*>::Possible()) {
              err = "Invalid argument type";
              return false;
            }
            char *val = 0;
            Convertion<typename gcore::NoRefOrConst<T>::Type, char*>::Do(src, val);
            dst.ptr = (void*) val;
            return true;
          }
          case AT_OBJECT: {
            if (!Convertion<typename gcore::NoRefOrConst<T>::Type, Object*>::Possible()) {
              err = "Invalid argument type";
              return false;
            }
            lwc::Object *val = 0;
            Convertion<typename gcore::NoRefOrConst<T>::Type, Object*>::Do(src, val);
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
    
    // string special case
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
  
  // ---
  
  template <typename T>
  Argument& Argument::setDefaultValue(T def) throw(std::runtime_error) {
    std::string err;
    if (!details::GetSet<T>::Get(*this, mDefaultValue, def, err)) {
      std::ostringstream oss;
      oss << "Argument::setDefaultValue: " << err;
      throw std::runtime_error(oss.str());
    }
    mHasDefault = true;
    return *this;
  }
  
  template <typename T>
  void Argument::getDefaultValue(T &def) const throw(std::runtime_error)  {
    if (!mHasDefault) {
      std::ostringstream oss;
      oss << "Argument::getDefaultValue: No default value set yet";
      throw std::runtime_error(oss.str());
    }
    std::string err;
    if (!details::GetSet<T>::Get(*this, mDefaultValue, def, err)) {
      std::ostringstream oss;
      oss << "Argument::getDefaultValue: " << err;
      throw std::runtime_error(oss.str());
    }
  }
  
  template <typename T>
  T Argument::getDefaultValue() const throw(std::runtime_error)  {
    T def;
    getDefaultValue(def);
    return def;
  }
}

#endif

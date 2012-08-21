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

#define LWC_MAX_POSITIONAL_ARGS 16
#define LWC_MAX_ARGS 32
#define LWC_DEFVAL(v) true, {(void*)(v)}
#define LWC_DEFVAL2(v) true, {*reinterpret_cast<void**>(&v)}
#define LWC_NODEF     false, {0}

namespace lwc {
  
  class LWC_API Object;
  class LWC_API Method;
  
  enum Direction {
    AD_IN = 0,
    AD_OUT,
    AD_INOUT
  };
  
  typedef union {
    void *ptr;
    Real real;
    Integer integer;
    bool boolean;
  } ArgumentValue;
  
  struct LWC_API ArgumentDecl {
    Direction dir;
    Type type;
    Integer arylen;
    bool hasdef;
    ArgumentValue defval;
    const char *name; // NULL by default
  };
  
  class LWC_API BaseArgument {
    public:
       
      friend class Object;
      friend class Method;
      friend class KeywordArgs;
      
      BaseArgument();
      BaseArgument(Direction d, Type t);
      BaseArgument(const BaseArgument &);
      virtual ~BaseArgument();
      
      BaseArgument& operator=(const BaseArgument &rhs);
      
      BaseArgument& setDir(Direction d);
      BaseArgument& setType(Type t);
      
      inline Direction getDir() const {return mDir;}
      inline Type getType() const {return mType;}
      inline bool isArray() const {return mArray;}
      inline int indirectionLevel() const {return mIndirectionLevel;}
      
      std::string toString() const;
      
      bool typeMatches(const BaseArgument &rhs) const;
      
      template <typename T>
      BaseArgument& setValue(T val) throw(std::runtime_error);
      
      template <typename T>
      void getValue(T &val) const throw(std::runtime_error);
      
      template <typename T>
      T getValue() const throw(std::runtime_error);
      
      inline const ArgumentValue& getRawValue() const {return mValue;}
      
    protected:
       
      Type mType;
      Direction mDir;
      bool mArray;
      int mIndirectionLevel;
      ArgumentValue mValue;
  };
  
  class LWC_API Argument : public BaseArgument {
    
    public:
      
      friend class Object;
      friend class Method;
      
      static const ArgumentValue _DefVal;
      
      Argument();
      Argument(Direction d, Type t, Integer lenidx=-1, bool hasdef=false, const ArgumentValue &def=_DefVal, const char *name=0);
      Argument(const Argument &rhs);
      virtual ~Argument();
      
      Argument& operator=(const Argument &rhs);
      
      void fromDeclaration(const ArgumentDecl &decl);
      
      std::string toString() const;
      std::string docString(const std::string &indent="") const;
      
      Argument& setDir(Direction d);
      Argument& setType(Type t);
      Argument& setArraySizeArg(Integer idx);
      Argument& setArrayArg(Integer idx);
      Argument& setName(const char *name);
      template <typename T>
      Argument& setDefaultValue(T def) throw(std::runtime_error);
      
      //inline Direction getDir() const {return mDir;}
      //inline Type getType() const {return mType;}
      //inline bool isArray() const {return mArray;}
      inline Integer arraySizeArg() const {return mArraySizeArg;}
      inline Integer arrayArg() const {return mArrayArg;}
      //inline int indirectionLevel() const {return mIndirectionLevel;}
      inline const std::string& getName() const {return mName;}
      inline bool isNamed() const {return (mName.length() > 0);}
      inline bool hasDefaultValue() const {return mHasDefault;}
      inline const ArgumentValue& getRawDefaultValue() const {return getRawValue();}
      template <typename T>
      void getDefaultValue(T &def) const throw(std::runtime_error);
      template <typename T>
      T getDefaultValue() const throw(std::runtime_error);
      
    private:
      
      //Type mType;
      //Direction mDir;
      //bool mArray;
      Integer mArraySizeArg;
      Integer mArrayArg;
      
      //int mIndirectionLevel;
      
      std::string mName;
      bool mHasDefault;
      //ArgumentValue mDefaultValue;
  };
  
  // ---
  
  namespace details {
    
    // general case (plain types)
    template <typename T> struct GetSet {
      static bool Get(const BaseArgument &arg, const ArgumentValue &src, T &dst, std::string &err) {
        err = "";
        if (arg.indirectionLevel() != 0) {
          err ="Expected 0 level of indirection";
          return false;
        }
        switch (arg.getType()) {
          case AT_BOOL: {
            if (!Convertion<bool, typename gcore::NoRefOrConst<T>::Type>::Possible()) {
              err = "Invalid argument type (excepted ";
              err += typeid(bool).name();
              err += " got ";
              err += typeid(typename gcore::NoRefOrConst<T>::Type).name();
              err += ")";
              return false;
            }
            Convertion<bool, typename gcore::NoRefOrConst<T>::Type>::Do(src.boolean, dst);
            return true;
          }
          case AT_INT: {
            if (!Convertion<Integer, typename gcore::NoRefOrConst<T>::Type>::Possible()) {
              err = "Invalid argument type (excepted ";
              err += typeid(Integer).name();
              err += " got ";
              err += typeid(typename gcore::NoRefOrConst<T>::Type).name();
              err += ")";
              return false;
            }
            Convertion<Integer, typename gcore::NoRefOrConst<T>::Type>::Do(src.integer, dst);
            return true;
          }
          case AT_REAL: {
            if (!Convertion<Real, typename gcore::NoRefOrConst<T>::Type>::Possible()) {
              err = "Invalid argument type (excepted ";
              err += typeid(Real).name();
              err += " got ";
              err += typeid(typename gcore::NoRefOrConst<T>::Type).name();
              err += ")";
              return false;
            }
            Convertion<Real, typename gcore::NoRefOrConst<T>::Type>::Do(src.real, dst);
            return true;
          }
          case AT_STRING: {
            if (!Convertion<char*, typename gcore::NoRefOrConst<T>::Type>::Possible()) {
              err = "Invalid argument type (excepted ";
              err += typeid(char*).name();
              err += " got ";
              err += typeid(typename gcore::NoRefOrConst<T>::Type).name();
              err += ")";
              return false;
            }
            char *val = (char*) src.ptr;
            Convertion<char*, typename gcore::NoRefOrConst<T>::Type>::Do(val, dst);
            return true;
          }
          case AT_OBJECT: {
            if (!Convertion<Object*, typename gcore::NoRefOrConst<T>::Type>::Possible()) {
              err = "Invalid argument type (excepted ";
              err += typeid(Object*).name();
              err += " got ";
              err += typeid(typename gcore::NoRefOrConst<T>::Type).name();
              err += ")";
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
      static bool Set(const BaseArgument &arg, T src, ArgumentValue &dst, std::string &err) {
        err = "";
        if (arg.indirectionLevel() != 0) {
          err ="Expected 0 level of indirection";
          return false;
        }
        switch (arg.getType()) {
          case AT_BOOL: {
            if (!Convertion<typename gcore::NoRefOrConst<T>::Type, bool>::Possible()) {
              err = "Invalid argument type (excepted ";
              err += typeid(bool).name();
              err += " got ";
              err += typeid(typename gcore::NoRefOrConst<T>::Type).name();
              err += ")";
              return false;
            }
            Convertion<typename gcore::NoRefOrConst<T>::Type, bool>::Do(src, dst.boolean);
            return true;
          }
          case AT_INT: {
            if (!Convertion<typename gcore::NoRefOrConst<T>::Type, Integer>::Possible()) {
              err = "Invalid argument type (excepted ";
              err += typeid(Integer).name();
              err += " got ";
              err += typeid(typename gcore::NoRefOrConst<T>::Type).name();
              err += ")";
              return false;
            }
            Convertion<typename gcore::NoRefOrConst<T>::Type, Integer>::Do(src, dst.integer);
            return true;
          }
          case AT_REAL: {
            if (!Convertion<typename gcore::NoRefOrConst<T>::Type, Real>::Possible()) {
              err = "Invalid argument type (excepted ";
              err += typeid(Real).name();
              err += " got ";
              err += typeid(typename gcore::NoRefOrConst<T>::Type).name();
              err += ")";
              return false;
            }
            Convertion<typename gcore::NoRefOrConst<T>::Type, Real>::Do(src, dst.real);
            return true;
          }
          case AT_STRING: {
            if (!Convertion<typename gcore::NoRefOrConst<T>::Type, char*>::Possible()) {
              err = "Invalid argument type (excepted ";
              err += typeid(char*).name();
              err += " got ";
              err += typeid(typename gcore::NoRefOrConst<T>::Type).name();
              err += ")";
              return false;
            }
            char *val = 0;
            Convertion<typename gcore::NoRefOrConst<T>::Type, char*>::Do(src, val);
            dst.ptr = (void*) val;
            return true;
          }
          case AT_OBJECT: {
            if (!Convertion<typename gcore::NoRefOrConst<T>::Type, Object*>::Possible()) {
              err = "Invalid argument type (excepted ";
              err += typeid(Object*).name();
              err += " got ";
              err += typeid(typename gcore::NoRefOrConst<T>::Type).name();
              err += ")";
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
      static bool Get(const BaseArgument &arg, const ArgumentValue &src, T* &dst, std::string &err) {
        err = "";
        // use typetraits?
        if (arg.indirectionLevel() != 1) {
          err ="Expected 1 level of indirection";
          return false;
        }
        if (Type2Enum<T>::Enum != int(arg.getType())) {
          err = "Invalid type";
          return false;
        }
        // check constness?
        dst = (T*) src.ptr;
        return true;
      }
      static bool Set(const BaseArgument &arg, const T* src, ArgumentValue &dst, std::string &err) {
        err = "";
        // use typetraits?
        if (arg.indirectionLevel() != 1) {
          err ="Expected 1 level of indirection";
          return false;
        }
        if (Type2Enum<T>::Enum != int(arg.getType())) {
          err = "Invalid type";
          return false;
        }
        // check constness ?
        dst.ptr = (void*) src;
        return true;
      }
      static bool Set(const BaseArgument &arg, T* src, ArgumentValue &dst, std::string &err) {
        return Set(arg, (const T*)src, dst, err);
      }
    };
    template <typename T> struct GetSet<T**> {
      static bool Get(const BaseArgument &arg, const ArgumentValue &src, T** &dst, std::string &err) {
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
      static bool Set(const BaseArgument &arg, const T** src, ArgumentValue &dst, std::string &err) {
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
      static bool Set(const BaseArgument &arg, T** src, ArgumentValue &dst, std::string &err) {
        return Set(arg, (const T**)src, dst, err);
      }
    };
    
    // string special case
    template <> struct GetSet<char*> {
      static bool Get(const BaseArgument &arg, const ArgumentValue &src, char* &dst, std::string &err) {
        err = "";
        // use typetraits?
        if (arg.indirectionLevel() != 0) {
          err = "Expected 0 level of indirection";
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
      static bool Set(const BaseArgument &arg, const char* src, ArgumentValue &dst, std::string &err) {
        err = "";
        // use typetraits?
        if (arg.indirectionLevel() != 0) {
          err = "Expected 0 level of indirection";
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
      static bool Set(const BaseArgument &arg, char* src, ArgumentValue &dst, std::string &err) {
        return Set(arg, (const char*)src, dst, err);
      }
    };
    template <> struct GetSet<char**> {
      static bool Get(const BaseArgument &arg, const ArgumentValue &src, char** &dst, std::string &err) {
        err = "";
        // use typetraits?
        if (arg.indirectionLevel() != 1) {
          err = "Expected 1 level of indirection";
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
      static bool Set(const BaseArgument &arg, const char** src, ArgumentValue &dst, std::string &err) {
        err = "";
        // use typetraits?
        if (arg.indirectionLevel() != 1) {
          err = "Expected 1 level of indirection";
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
      static bool Set(const BaseArgument &arg, char** src, ArgumentValue &dst, std::string &err) {
        return Set(arg, (const char**)src, dst, err);
      }
    };
    template <> struct GetSet<char***> {
      static bool Get(const BaseArgument &arg, const ArgumentValue &src, char*** &dst, std::string &err) {
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
      static bool Set(const BaseArgument &arg, const char*** src, ArgumentValue &dst, std::string &err) {
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
      static bool Set(const BaseArgument &arg, char*** src, ArgumentValue &dst, std::string &err) {
        return Set(arg, (const char***)src, dst, err);
      }
    };
    
    // object special case
    template <> struct GetSet<Object*> {
      static bool Get(const BaseArgument &arg, const ArgumentValue &src, Object* &dst, std::string &err) {
        err = "";
        // use typetraits?
        if (arg.indirectionLevel() != 0) {
          err = "Expected 0 level of indirection";
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
      static bool Set(const BaseArgument &arg, const Object* src, ArgumentValue &dst, std::string &err) {
        err = "";
        // use typetraits?
        if (arg.indirectionLevel() != 0) {
          err = "Expected 0 level of indirection";
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
      static bool Set(const BaseArgument &arg, Object* src, ArgumentValue &dst, std::string &err) {
        return Set(arg, (const Object*)src, dst, err);
      }
    };
    template <> struct GetSet<Object**> {
      static bool Get(const BaseArgument &arg, const ArgumentValue &src, Object** &dst, std::string &err) {
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
      static bool Set(const BaseArgument &arg, const Object** src, ArgumentValue &dst, std::string &err) {
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
      static bool Set(const BaseArgument &arg, Object** src, ArgumentValue &dst, std::string &err) {
        return Set(arg, (const Object**)src, dst, err);
      }
    };
    template <> struct GetSet<Object***> {
      static bool Get(const BaseArgument &arg, const ArgumentValue &src, Object*** &dst, std::string &err) {
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
      static bool Set(const BaseArgument &arg, const Object*** src, ArgumentValue &dst, std::string &err) {
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
      static bool Set(const BaseArgument &arg, Object*** src, ArgumentValue &dst, std::string &err) {
        return Set(arg, (const Object***)src, dst, err);
      }
    };
    
    // empty special case
    template <> struct GetSet<Empty> {
      static bool Get(const BaseArgument &, const ArgumentValue &, Empty &, std::string &err) {
        err = "Empty is not a valid argument type";
        return false;
      }
      static bool Set(const BaseArgument &, const Empty &, ArgumentValue &, std::string &err) {
        err = "Empty is not a valid argument type";
        return false;
      }
      static bool Set(const BaseArgument &, Empty &, ArgumentValue &, std::string &err) {
        err = "Empty is not a valid argument type";
        return false;
      }
    };
    
    // some more templates for fun (actually for keyword args
    
    template <bool Cond1, bool Cond2> struct Or {
      enum {Value = 1};
    };
    
    template <> struct Or<false, false> {
      enum {Value = 0};
    };
    
    template <bool StopCond, typename T> struct _BaseType {
      typedef typename gcore::TypeTraits<T>::Value BaseType;
      enum {EnumType = Type2Enum<BaseType>::Enum};
      typedef typename _BaseType<Or<EnumType != (int)AT_UNKNOWN, gcore::TypeTraits<BaseType>::IsPtr == 0>::Value == 1, BaseType>::Value Value;
    };
    
    template <typename T> struct _BaseType<true, T> {
      typedef T Value;
    };
    
    template <bool StopCond, typename T> struct _IndirectionLevel {
      typedef typename gcore::TypeTraits<T>::Value BaseType;
      enum {
        EnumType = Type2Enum<BaseType>::Enum,
        Value = 1 + _IndirectionLevel<Or<EnumType != (int)AT_UNKNOWN, gcore::TypeTraits<BaseType>::IsPtr == 0>::Value == 1, BaseType>::Value
      };
    };
    
    template <typename T> struct _IndirectionLevel<true, T> {
      enum {Value = 0};
    };
    
    // top level access
    
    template <typename T> struct IndirectionLevel {
      enum {
        Value = _IndirectionLevel<Or<Type2Enum<T>::Enum != (int)AT_UNKNOWN, gcore::TypeTraits<T>::IsPtr == 0>::Value == 1, T>::Value
      };
    };
    
    template <typename T> struct BaseType {
      typedef typename _BaseType<Or<Type2Enum<T>::Enum != (int)AT_UNKNOWN, gcore::TypeTraits<T>::IsPtr == 0>::Value == 1, T>::Value Value;
    };
  }
  
  // ---
  
  class LWC_API KeywordArgs : protected std::map<std::string, BaseArgument> {
    public:
      
      typedef std::map<std::string, BaseArgument> Super;
      
      typedef Super::iterator iterator;
      typedef Super::const_iterator const_iterator;
      
      using Super::begin;
      using Super::end;
      using Super::find;
      using Super::clear;
      
      KeywordArgs() {
      }
      
      KeywordArgs(const KeywordArgs &rhs)
        : Super(rhs) {
      }
      
      virtual ~KeywordArgs() {
      }
      
      KeywordArgs& operator=(const KeywordArgs &rhs) {
        Super::operator=(rhs);
        return *this;
      }
      
      template <typename T>
      void set(const char *n, T val) throw(std::runtime_error) {
        
        // Keyword arguments can only be IN or INOUT
        // We don't really care about the IN array and INOUT difference
        // What we're interested in when setting method params is having a matching indirection level
        
        BaseArgument &arg = (*this)[n];
        
        int il = details::IndirectionLevel<T>::Value;
        Type t = (Type) Type2Enum<typename details::BaseType<T>::Value>::Enum;
        
        if (t == -1) {
          bool doConvert = false;
          if (il == 0) {
            // allow a implicit conversion in plain types
            if (Convertion<typename gcore::NoRefOrConst<typename details::BaseType<T>::Value>::Type, Integer>::Possible()) {
              t = AT_INT;
              doConvert = true;
            } else if (Convertion<typename gcore::NoRefOrConst<typename details::BaseType<T>::Value>::Type, Real>::Possible()) {
              t = AT_REAL;
              doConvert = true;
            } else if (Convertion<typename gcore::NoRefOrConst<typename details::BaseType<T>::Value>::Type, char*>::Possible()) {
              t = AT_STRING;
              doConvert = true;
            } else if (Convertion<typename gcore::NoRefOrConst<typename details::BaseType<T>::Value>::Type, Object*>::Possible()) {
              t = AT_OBJECT;
              doConvert = true;
            }
          } else if (il == 1) {
            // could be a pointer to char or object, as the 
            if (Convertion<typename gcore::NoRefOrConst<typename details::BaseType<T>::Value*>::Type, char*>::Possible()) {
              il -= 1;
              t = AT_STRING;
              doConvert = true;
            } else if (Convertion<typename gcore::NoRefOrConst<typename details::BaseType<T>::Value*>::Type, Object*>::Possible()) {
              il -= 1;
              t = AT_OBJECT;
              doConvert = true;
            }
          }
          if (!doConvert) {
            std::ostringstream oss;
            oss << "Could not figure out Keyword argument type" << std::endl;
            throw std::runtime_error(oss.str());
          }
        }
        
        arg.setDir(AD_IN);
        arg.setType(t);
        
        if (arg.indirectionLevel() != il) {
          
          t = (Type) (AT_ARRAY_BASE + t);
          arg.setType(t);
          
          if (arg.indirectionLevel() != il) {
            
            arg.setDir(AD_INOUT);
            
            if (arg.indirectionLevel() != il) {
              std::ostringstream oss;
              oss << "Unsupported value type";
              throw std::runtime_error(oss.str());
            }
          }
        }
        
        arg.setValue(val);
      }
  };
  
  extern LWC_API const KeywordArgs NoKeywordArgs;
  
  // ---
  
  template <typename T>
  BaseArgument& BaseArgument::setValue(T val) throw(std::runtime_error) {
    std::string err;
    if (!details::GetSet<typename gcore::NoRefOrConst<T>::Type>::Set(*this, val, mValue, err)) {
      std::ostringstream oss;
      oss << "BaseArgument::setValue: " << err;
      throw std::runtime_error(oss.str());
    }
    return *this;
  }
  
  template <typename T>
  void BaseArgument::getValue(T &val) const throw(std::runtime_error)  {
    std::string err;
    if (!details::GetSet<typename gcore::NoRefOrConst<T>::Type>::Get(*this, mValue, val, err)) {
      std::ostringstream oss;
      oss << "BaseArgument::getValue: " << err;
      throw std::runtime_error(oss.str());
    }
  }
  
  template <typename T>
  T BaseArgument::getValue() const throw(std::runtime_error)  {
    T val;
    getValue(val);
    return val;
  }
  
  // ---
  
  template <typename T>
  Argument& Argument::setDefaultValue(T def) throw(std::runtime_error) {
    BaseArgument::setValue(def);
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
    BaseArgument::getValue(def);
  }
  
  template <typename T>
  T Argument::getDefaultValue() const throw(std::runtime_error)  {
    T def;
    getDefaultValue(def);
    return def;
  }
}

#endif

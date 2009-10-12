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

#ifndef __lwc_method_h__
#define __lwc_method_h__

#include <lwc/config.h>
#include <lwc/types.h>
#include <lwc/traits.h>
#include <map>

namespace lwc {
  
  class LWC_API Object;
  class LWC_API MethodParams;
  class LWC_API MethodPointer;
  
  enum Direction {
    AD_IN = 0,
    AD_OUT,
    AD_INOUT,
    AD_RETURN
  };
  
  struct LWC_API ArgumentDecl {
    Direction dir;
    Type type;
    bool cst;
    bool ptr;
    bool ary;
    size_t arylen;
  };

  struct LWC_API MethodDecl {
    const char *name;
    size_t nargs;
    ArgumentDecl args[16];
    MethodPointer *ptr;
  };
  
  namespace details {
    
    template <typename T> struct GetSet {
      inline static void Get(size_t i, void **p, T &val) {
        // This error in linux: complaining it cannot cast void* to int because or loss of data
        //val = (T) p[i];
        val = *((T*)&p[i]);
      }
      inline static void Set(size_t i, void **p, T val) {
        p[i] = (void*) val;
        // This doesn't work properly with int on linux
        //p[i] = *((void**)&val);
      }
    };
    
    template <> struct GetSet<Empty> {
      inline static void Get(size_t, void**, Empty&) {
      }
      inline static void Set(size_t, void**, Empty) {
      }
    };
    
    // might not be necessary
    template <> struct GetSet<float> {
      inline static void Get(size_t i, void **p, float &val) {
        val = *((float*)&p[i]);
      }
      inline static void Set(size_t i, void **p, float val) {
        // will this work?
        p[i] = *((void**)&val);
      }
    };
    
    // might not be necessary
    template <> struct GetSet<double> {
      inline static void Get(size_t i, void **p, double &val) {
        val = *((double*)&p[i]);
      }
      inline static void Set(size_t i, void **p, double val) {
        // will this work?
        p[i] = *((void**)&val);
      }
    };
  }
  
  class LWC_API Argument {
    
    public:
      
      friend class Object;
      
      Argument();
      Argument(Direction d, Type t, bool cst=false, bool ptr=false, bool array=false, int lenidx=-1);
      Argument(const Argument &rhs);
      ~Argument();
      
      Argument& operator=(const Argument &rhs);
      
      void fromDeclaration(const ArgumentDecl &decl);
      
      std::string toString() const;
      
      inline const std::string& getError() const {return mErr;}
      
      inline Argument& setConst(bool cst) {mConst=cst; return *this;}
      inline Argument& setPtr(bool ptr) {mPtr=ptr; return *this;}
      inline Argument& setArray(bool ary) {mArray=ary; return *this;}
      inline Argument& setArraySizeArg(int idx) {mArraySizeArg=idx; return *this;}
      inline Argument& setArrayArg(int idx) {mArrayArg=idx; return *this;}
      inline Argument& setType(Type t) {mType=t; return *this;}
      inline Argument& setDir(Direction d) {mDir=d; return *this;}
      //inline Argument& setAllocated(bool alloc) {mAllocated=alloc; return *this;}
      
      inline bool isConst() const {return mConst;}
      inline bool isPtr() const {return mPtr;}
      inline bool isArray() const {return mArray;}
      inline int arraySizeArg() const {return mArraySizeArg;}
      inline int arrayArg() const {return mArrayArg;}
      inline Type getType() const {return mType;}
      inline Direction getDir() const {return mDir;}
      //inline bool isAllocated() const {return mAllocated;}
      
      template <typename T> bool check() const {
        mErr = "";
        int ilevel = (mDir!=AD_IN?1:0) + (mPtr?1:0) + (mArray?1:0);
        switch (ilevel) {
        case 3:
          if (TypeTraits<T>::IsPtr == 0 ||
              TypeTraits<typename TypeTraits<T>::Value>::IsPtr == 0 ||
              TypeTraits<typename TypeTraits<typename TypeTraits<T>::Value>::Value>::IsPtr == 0) {
            mErr ="Expected 3 level of indirection";
            return false;
          }
          if (TypeTraits<typename TypeTraits<typename TypeTraits<T>::Value>::Value>::IsConst != int(mConst)) {
            mErr ="Expected " + std::string(mConst?"":"non-") + "const";
            return false;
          }
          if (Type2Enum<typename TypeTraits<typename TypeTraits<typename TypeTraits<T>::Value>::Value>::Value>::Enum != int(mType)) {
            mErr ="Invalid type";
            return false;
          }
          return true;
        case 2:
          if (TypeTraits<T>::IsPtr == 0 ||
              TypeTraits<typename TypeTraits<T>::Value>::IsPtr == 0) {
            mErr ="Expected 2 level of indirection";
            return false;
          }
          if (TypeTraits<typename TypeTraits<T>::Value>::IsConst != int(mConst)) {
            mErr ="Expected " + std::string(mConst?"":"non-") + "const";
            return false;
          }
          if (Type2Enum<typename TypeTraits<typename TypeTraits<T>::Value>::Value>::Enum != int(mType)) {
            mErr ="Invalid type";
            return false;
          }
          return true;
        case 1:
          if (TypeTraits<T>::IsPtr == 0) {
            mErr ="Expected 1 level of indirection";
            return false;
          }
          if (TypeTraits<T>::IsConst != int(mConst)) {
            mErr ="Expected " + std::string(mConst?"":"non-") + "const";
            return false;
          }
          if (Type2Enum<typename TypeTraits<T>::Value>::Enum != int(mType)) {
            mErr ="Invalid type";
            return false;
          }
          return true;
        case 0:
          if (TypeTraits<T>::IsConst != int(mConst)) {
            mErr ="Expected " + std::string(mConst?"":"non-") + "const";
            return false;
          }
          if (Type2Enum<T>::Enum != int(mType)) {
            mErr ="Invalid type";
            return false;
          }
          return true;
        default:
          mErr ="Invalid level of indirection";
          return false;
        }
      }
      
      
    private:
      
      Type mType;
      
      Direction mDir;
      
      bool mConst;
      
      bool mPtr;
      
      bool mArray;
      int mArraySizeArg;
      int mArrayArg;
      
      //bool mAllocated;
      
      mutable std::string mErr;
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
        if (!mMethod[i].check<T>()) {
          std::ostringstream oss;
          oss << "Method argument " << i << ": " << mMethod[i].getError();
          throw std::runtime_error(oss.str());
        }
        details::GetSet<T>::Set(i, mParams, value);
      }
      
      template <typename T>
      void get(size_t i, T &value) throw(std::runtime_error)  {
        if (i >= mMethod.numArgs()) {
          std::ostringstream oss;
          oss << "Invalid method argument index " << i;
          throw std::runtime_error(oss.str());
        }
        if (!mMethod[i].check<T>()) {
          std::ostringstream oss;
          oss << "Method argument " << i << ": " << mMethod[i].getError();
          throw std::runtime_error(oss.str());
        }
        details::GetSet<T>::Get(i, mParams, value);
      }
      
    private:
      
      MethodParams();
      MethodParams& operator=(const MethodParams&);
      
    private:
      
      const Method &mMethod;
      void *mParams[16];
  };
  
}

#endif



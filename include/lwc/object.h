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

#ifndef __lwc_object_h__
#define __lwc_object_h__

#include <lwc/config.h>
#include <lwc/method.h>

namespace lwc {
  
  class LWC_API Object {
    public:
      
      template <typename T0=Empty,  typename T1=Empty,  typename T2=Empty,  typename T3=Empty,
                typename T4=Empty,  typename T5=Empty,  typename T6=Empty,  typename T7=Empty,
                typename T8=Empty,  typename T9=Empty,  typename T10=Empty, typename T11=Empty,
                typename T12=Empty, typename T13=Empty, typename T14=Empty, typename T15=Empty>
      struct MethodCall {
        static void Call(Object *self, const char *name, const KeywordArgs &kwargs,
                         T0 arg0=T0(), T1 arg1=T1(), T2 arg2=T2(), T3 arg3=T3(),
                         T4 arg4=T4(), T5 arg5=T5(), T6 arg6=T6(), T7 arg7=T7(),
                         T8 arg8=T8(), T9 arg9=T9(), T10 arg10=T10(), T11 arg11=T11(),
                         T12 arg12=T12(), T13 arg13=T13(), T14 arg14=T14(), T15 arg15=T15()) throw(std::runtime_error) {
          
          if (!self->respondsTo(name)) {
            std::ostringstream oss;
            oss << "Object has not method \"" << name << "\"";
            throw std::runtime_error(oss.str());
          }
          
          const Method &meth = self->getMethod(name);
          
          static const std::type_info &emptytype = typeid(Empty);
          
          MethodParams params(meth);
          
          // Set keyworkds arguments
          // -> must be set before as SETPARAMORCALL may return
          
          KeywordArgs::const_iterator it = kwargs.begin();
          while (it != kwargs.end()) {
            size_t idx = meth.namedArgIndex(it->first.c_str());
            const Argument &ma = meth[idx];
            if (!it->second.typeMatches(ma)) {
              // at the time keyword arguments are set, we don't know yet which
              // method they will be applied to, so don't know the exact target type
              // for plain type, we should allow int to real and real to int conversions
              // as allow in Argument::setValue, MethodParams::set/setn
              if (ma.indirectionLevel() == 0 && // plain type
                  it->second.indirectionLevel() == ma.indirectionLevel() &&
                  ((it->second.getType() == AT_INT && // int -> real
                    ma.getType() == AT_REAL) ||
                   (it->second.getType() == AT_REAL && // real -> int
                    ma.getType() == AT_INT))) {
                ArgumentValue av;
                if (ma.getType() == AT_REAL) {
                  it->second.getValue(av.real);
                } else {
                  it->second.getValue(av.integer);
                }
                params.rawsetn(it->first.c_str(), av);
              } else {
                std::ostringstream oss;
                oss << "In method \"" << name << "\": Keyword argument type mismatch";
                throw std::runtime_error(oss.str());
              }
            } else {
              params.rawsetn(it->first.c_str(), it->second.getRawValue());
            }
            ++it;
          }
          
#define SETPARAMORCALL(n) \
          if (typeid(T##n) == emptytype) {\
            if (n < meth.numArgs()) {\
              size_t nn = n;\
              while (nn < meth.numArgs()) {\
                const Argument &ma = meth[nn];\
                bool ignoreKwarg = (ma.isNamed() && kwargs.find(ma.getName()) != kwargs.end());\
                if (ma.hasDefaultValue()) {\
                  if (!ignoreKwarg) {\
                    try {\
                      params.rawset(nn, ma.getRawDefaultValue(), false);\
                    } catch (std::exception &e) {\
                      std::ostringstream oss;\
                      oss << "In method \"" << name << "\": " << e.what();\
                      throw std::runtime_error(oss.str());\
                    }\
                  }\
                  ++nn;\
                } else {\
                  if (ignoreKwarg) {\
                    ++nn;\
                    continue;\
                  } else {\
                    break;\
                  }\
                }\
              }\
              if (meth.numArgs() != nn) {\
                std::ostringstream oss;\
                oss << "Method \"" << name << "\" expects " << meth.numArgs() << " arguments";\
                throw std::runtime_error(oss.str());\
              }\
            }\
            self->call(name, params);\
            return;\
          } else {\
            if (n >= meth.numPositionalArgs()) {\
              if (kwargs.find(meth[n].getName()) != kwargs.end()) {\
                std::ostringstream oss;\
                oss << "Method \"" << name << "\" keyword argument \"" << meth[n].getName() << "\" is set twice";\
                throw std::runtime_error(oss.str());\
              }\
            }\
            try {\
              params.set(size_t(n), arg##n, false);\
            } catch (std::exception &e) {\
              std::ostringstream oss;\
              oss << "In method \"" << name << "\": " << e.what();\
              throw std::runtime_error(oss.str());\
            }\
          }
          
          // Set positional arguments
          
          SETPARAMORCALL(0)
          SETPARAMORCALL(1)
          SETPARAMORCALL(2)
          SETPARAMORCALL(3)
          SETPARAMORCALL(4)
          SETPARAMORCALL(5)
          SETPARAMORCALL(6)
          SETPARAMORCALL(7)
          SETPARAMORCALL(8)
          SETPARAMORCALL(9)
          SETPARAMORCALL(10)
          SETPARAMORCALL(11)
          SETPARAMORCALL(12)
          SETPARAMORCALL(13)
          SETPARAMORCALL(14)
          SETPARAMORCALL(15)
          
#undef SETPARAMORCALL
          
          if (meth.numPositionalArgs() != LWC_MAX_POSITIONAL_ARGS) {
            std::ostringstream oss;
            oss << "Method \"" << name << "\" expects " << meth.numArgs() << " arguments";
            throw std::runtime_error(oss.str());
          }
          self->call(name, params);
        }
      };
          
    public:
      
      Object();
      virtual ~Object();
      
      
      inline const char* getLoaderName() const {
        return mLoaderName.c_str();
      }
      
      inline const char* getTypeName() const {
        return mTypeName.c_str();
      }
      
      inline const MethodsTable* getMethods() const {
        return mMethods;
      }
      
      inline bool respondsTo(const char *name) const {
        return (mMethods ? (mMethods->findMethod(name) != 0) : false);
      }
      
      inline size_t availableMethods(std::vector<std::string> &methds) const {
        methds.clear();
        if (mMethods) {
          return mMethods->availableMethods(methds);
        } else {
          return 0;
        }
      }
      
      inline const Method& getMethod(const char *name) const throw(std::runtime_error) {
        const Method *m = (mMethods ? mMethods->findMethod(name) : 0);
        if (!m) {
          std::ostringstream oss;
          oss << "Object has not method \"" << name << "\"";
          throw std::runtime_error(oss.str());
        }
        return *m;
      }
      
      virtual void call(const char *name, MethodParams &params) throw(std::runtime_error);
      
      void call(const char *name, const KeywordArgs &kwargs=NoKeywordArgs) throw(std::runtime_error) {
        MethodCall<Empty>::Call(this, name, kwargs);
      }
      
      template <typename T0>
      void call(const char *name, T0 arg0, const KeywordArgs &kwargs=NoKeywordArgs) throw(std::runtime_error) {
        MethodCall<T0>::Call(this, name, kwargs, arg0);
      }
      
      template <typename T0, typename T1>
      void call(const char *name, T0 arg0, T1 arg1, const KeywordArgs &kwargs=NoKeywordArgs) throw(std::runtime_error) {
        MethodCall<T0,T1>::Call(this, name, kwargs, arg0, arg1);
      }
      
      template <typename T0, typename T1, typename T2>
      void call(const char *name,
                T0 arg0, T1 arg1, T2 arg2, const KeywordArgs &kwargs=NoKeywordArgs) throw(std::runtime_error) {
        MethodCall<T0,T1,T2>::Call(this, name, kwargs, arg0, arg1, arg2);
      }
      
      template <typename T0, typename T1, typename T2, typename T3>
      void call(const char *name,
                T0 arg0, T1 arg1, T2 arg2, T3 arg3, const KeywordArgs &kwargs=NoKeywordArgs) throw(std::runtime_error) {
        MethodCall<T0,T1,T2,T3>::Call(this, name, kwargs, arg0, arg1, arg2, arg3);
      }
      
      template <typename T0, typename T1, typename T2, typename T3,
                typename T4>
      void call(const char *name,
                T0 arg0, T1 arg1, T2 arg2, T3 arg3,
                T4 arg4, const KeywordArgs &kwargs=NoKeywordArgs) throw(std::runtime_error) {
        MethodCall<T0,T1,T2,T3,T4>::Call(this, name, kwargs, arg0, arg1, arg2, arg3, arg4);
      }
      
      template <typename T0, typename T1, typename T2, typename T3,
                typename T4, typename T5>
      void call(const char *name,
                T0 arg0, T1 arg1, T2 arg2, T3 arg3,
                T4 arg4, T5 arg5, const KeywordArgs &kwargs=NoKeywordArgs) throw(std::runtime_error) {
        MethodCall<T0,T1,T2,T3,T4,T5>::Call(this, name, kwargs, arg0, arg1, arg2, arg3,
                                            arg4, arg5);
      }
      
      template <typename T0, typename T1, typename T2, typename T3,
                typename T4, typename T5, typename T6>
      void call(const char *name,
                T0 arg0, T1 arg1, T2 arg2, T3 arg3,
                T4 arg4, T5 arg5, T6 arg6, const KeywordArgs &kwargs=NoKeywordArgs) throw(std::runtime_error) {
        MethodCall<T0,T1,T2,T3,T4,T5,T6>::Call(this, name, kwargs, arg0, arg1, arg2, arg3,
                                               arg4, arg5, arg6);
      }
      
      template <typename T0, typename T1, typename T2, typename T3,
                typename T4, typename T5, typename T6, typename T7>
      void call(const char *name,
                T0 arg0, T1 arg1, T2 arg2, T3 arg3,
                T4 arg4, T5 arg5, T6 arg6, T7 arg7, const KeywordArgs &kwargs=NoKeywordArgs) throw(std::runtime_error) {
        MethodCall<T0,T1,T2,T3,T4,T5,T6,T7>::Call(this, name, kwargs, arg0, arg1, arg2,
                                                  arg3, arg4, arg5, arg6, arg7);
      }
      
      template <typename T0, typename T1, typename T2, typename T3,
                typename T4, typename T5, typename T6, typename T7,
                typename T8>
      void call(const char *name,
                T0 arg0, T1 arg1, T2 arg2, T3 arg3,
                T4 arg4, T5 arg5, T6 arg6, T7 arg7,
                T8 arg8, const KeywordArgs &kwargs=NoKeywordArgs) throw(std::runtime_error) {
        MethodCall<T0,T1,T2,T3,T4,T5,T6,T7,T8>::Call(this, name, kwargs, arg0, arg1, arg2,
                                                     arg3, arg4, arg5, arg6, arg7,
                                                     arg8);
      }
      
      template <typename T0, typename T1, typename T2, typename T3,
                typename T4, typename T5, typename T6, typename T7,
                typename T8, typename T9>
      void call(const char *name,
                T0 arg0, T1 arg1, T2 arg2, T3 arg3,
                T4 arg4, T5 arg5, T6 arg6, T7 arg7,
                T8 arg8, T9 arg9, const KeywordArgs &kwargs=NoKeywordArgs) throw(std::runtime_error) {
        MethodCall<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>::Call(this, name, kwargs, arg0, arg1, arg2,
                                                        arg3, arg4, arg5, arg6, arg7,
                                                        arg8, arg9);
      }
      
      template <typename T0, typename T1, typename T2, typename T3,
                typename T4, typename T5, typename T6, typename T7,
                typename T8, typename T9, typename T10>
      void call(const char *name,
                T0 arg0, T1 arg1, T2 arg2, T3 arg3,
                T4 arg4, T5 arg5, T6 arg6, T7 arg7,
                T8 arg8, T9 arg9, T10 arg10, const KeywordArgs &kwargs=NoKeywordArgs) throw(std::runtime_error) {
        MethodCall<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>::Call(this, name, kwargs, arg0, arg1, arg2,
                                                            arg3, arg4, arg5, arg6, arg7,
                                                            arg8, arg9, arg10);
      }
      
      template <typename T0, typename T1, typename T2, typename T3,
                typename T4, typename T5, typename T6, typename T7,
                typename T8, typename T9, typename T10, typename T11>
      void call(const char *name,
                T0 arg0, T1 arg1, T2 arg2, T3 arg3,
                T4 arg4, T5 arg5, T6 arg6, T7 arg7,
                T8 arg8, T9 arg9, T10 arg10, T11 arg11, const KeywordArgs &kwargs=NoKeywordArgs) throw(std::runtime_error) {
        MethodCall<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>::Call(this, name, kwargs, arg0, arg1, arg2,
                                                                arg3, arg4, arg5, arg6, arg7,
                                                                arg8, arg9, arg10, arg11);
      }
      
      template <typename T0, typename T1, typename T2, typename T3,
                typename T4, typename T5, typename T6, typename T7,
                typename T8, typename T9, typename T10, typename T11,
                typename T12>
      void call(const char *name,
                T0 arg0, T1 arg1, T2 arg2, T3 arg3,
                T4 arg4, T5 arg5, T6 arg6, T7 arg7,
                T8 arg8, T9 arg9, T10 arg10, T11 arg11,
                T12 arg12, const KeywordArgs &kwargs=NoKeywordArgs) throw(std::runtime_error) {
        MethodCall<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12>::Call(this, name, kwargs, arg0, arg1, arg2,
                                                                    arg3, arg4, arg5, arg6, arg7,
                                                                    arg8, arg9, arg10, arg11, arg12);
      }
      
      template <typename T0, typename T1, typename T2, typename T3,
                typename T4, typename T5, typename T6, typename T7,
                typename T8, typename T9, typename T10, typename T11,
                typename T12, typename T13>
      void call(const char *name,
                T0 arg0, T1 arg1, T2 arg2, T3 arg3,
                T4 arg4, T5 arg5, T6 arg6, T7 arg7,
                T8 arg8, T9 arg9, T10 arg10, T11 arg11,
                T12 arg12, T13 arg13, const KeywordArgs &kwargs=NoKeywordArgs) throw(std::runtime_error) {
        MethodCall<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13>::Call(this, name, kwargs, arg0, arg1, arg2,
                                                                        arg3, arg4, arg5, arg6, arg7,
                                                                        arg8, arg9, arg10, arg11, arg12,
                                                                        arg13);
      }
      
      template <typename T0, typename T1, typename T2, typename T3,
                typename T4, typename T5, typename T6, typename T7,
                typename T8, typename T9, typename T10, typename T11,
                typename T12, typename T13, typename T14>
      void call(const char *name,
                T0 arg0, T1 arg1, T2 arg2, T3 arg3,
                T4 arg4, T5 arg5, T6 arg6, T7 arg7,
                T8 arg8, T9 arg9, T10 arg10, T11 arg11,
                T12 arg12, T13 arg13, T14 arg14, const KeywordArgs &kwargs=NoKeywordArgs) throw(std::runtime_error) {
        MethodCall<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14>::Call(this, name, kwargs, arg0, arg1, arg2,
                                                                            arg3, arg4, arg5, arg6, arg7,
                                                                            arg8, arg9, arg10, arg11, arg12,
                                                                            arg13, arg14);
      }
      
      template <typename T0, typename T1, typename T2, typename T3,
                typename T4, typename T5, typename T6, typename T7,
                typename T8, typename T9, typename T10, typename T11,
                typename T12, typename T13, typename T14, typename T15>
      void call(const char *name,
                T0 arg0, T1 arg1, T2 arg2, T3 arg3,
                T4 arg4, T5 arg5, T6 arg6, T7 arg7,
                T8 arg8, T9 arg9, T10 arg10, T11 arg11,
                T12 arg12, T13 arg13, T14 arg14, T15 arg15, const KeywordArgs &kwargs=NoKeywordArgs) throw(std::runtime_error) {
        MethodCall<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15>::Call(this, name, kwargs, arg0, arg1, arg2,
                                                                                arg3, arg4, arg5, arg6, arg7,
                                                                                arg8, arg9, arg10, arg11, arg12,
                                                                                arg13, arg14, arg15);
      }
      
#ifdef LWC_MEMTRACK
      static unsigned long GetInstanceCount();
#endif
      
    private:
      
      inline void setMethodTable(const MethodsTable *methods) {
        mMethods = methods;
      }
      
      inline void setLoaderName(const char *name) {
        mLoaderName = name;
      }
      
      inline void setTypeName(const char *name) {
        mTypeName = name;
      }
      
    private:
      
      friend class Loader;
      friend class Registry;
      
      std::string mLoaderName;
      std::string mTypeName;
      const MethodsTable *mMethods;
  };

}

#endif

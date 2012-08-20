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

#ifndef __lwc_types_h__
#define __lwc_types_h__

#include <lwc/config.h>

namespace lwc {
  
  class LWC_API Object;
  
  enum Type {
    AT_UNKNOWN = -1,
    AT_BOOL,
    AT_INT,
    AT_REAL,
    AT_STRING,
    AT_OBJECT,
    AT_ARRAY_BASE,
    AT_BOOL_ARRAY = AT_ARRAY_BASE,
    AT_INT_ARRAY,
    AT_REAL_ARRAY,
    AT_STRING_ARRAY,
    AT_OBJECT_ARRAY
  };

  struct Empty {
  };

  template <Type T>
  struct Enum2Type {
    typedef Empty Type;
  };
  
  template <typename T>
  struct Type2Enum {
    enum {
      Enum = AT_UNKNOWN
    };
  };
  
  typedef double Real;
  
#if defined(__LP64__) || defined(_LP64) || (_MIPS_SZLONG == 64) || (__WORDSIZE == 64)
# define NATIVE_64BIT_LONG 1
  typedef long Integer;
#elif defined(_MSC_VER) || (defined(__BCPLUSPLUS__) && __BORLANDC__ > 0x500) || defined(__WATCOM_INT64__)
# define NATIVE_64BIT_LONG 0
  typedef __int64 Integer;
#elif defined(__GNUG__) || defined(__GNUC__) || defined(__SUNPRO_CC) || defined(__MWERKS__) || defined(__SC__) || defined(_LONGLONG)
# define NATIVE_64BIT_LONG 0
  typedef long long Integer;
#else
#error "Integer not defined for this architecture!"
#endif
  
  template <> struct Enum2Type<AT_BOOL> {typedef bool Type;};
  template <> struct Enum2Type<AT_INT> {typedef Integer Type;};
  template <> struct Enum2Type<AT_REAL> {typedef Real Type;};
  template <> struct Enum2Type<AT_STRING> {typedef char* Type;};
  template <> struct Enum2Type<AT_OBJECT> {typedef Object* Type;};
  
  template <> struct Type2Enum<bool> {enum {Enum = AT_BOOL};};
  template <> struct Type2Enum<Integer> {enum {Enum = AT_INT};};
  template <> struct Type2Enum<Real> {enum {Enum = AT_REAL};};
  template <> struct Type2Enum<char*> {enum {Enum = AT_STRING};};
  template <> struct Type2Enum<Object*> {enum {Enum = AT_OBJECT};};
  
  template <typename T, typename U> struct Convertion {
    static bool Possible() {return false;}
    static void Do(const T &, U &) {}
  };
  // same type
  template <typename T> struct Convertion<T, T> {
    static bool Possible() {return true;}
    static void Do(const T &from, T &to) {to = from;}
  };
  // c++ string to c string
  template <> struct Convertion<std::string, char*> {
    static bool Possible() {return true;}
    static void Do(const std::string &from, char *&to) {to = (char*) from.c_str();}
    static void DoRev(const char *&from, std::string &to) {to = from;}
  };
  template <> struct Convertion<char*, std::string> {
    static bool Possible() {return true;}
    static void Do(const char *&from, std::string &to) {to = from;}
  };
  // plain type to integer
  template <> struct Convertion<char, Integer> {
    static bool Possible() {return true;}
    static void Do(const char &from, Integer &to) {to = (Integer)from;}
  };
  template <> struct Convertion<Integer, char> {
    static bool Possible() {return true;}
    static void Do(const Integer &from, char &to) {to = (char)from;}
  };
  template <> struct Convertion<unsigned char, Integer> {
    static bool Possible() {return true;}
    static void Do(const unsigned char &from, Integer &to) {to = (Integer)from;}
  };
  template <> struct Convertion<Integer, unsigned char> {
    static bool Possible() {return true;}
    static void Do(const Integer &from, unsigned char &to) {to = (unsigned char)from;}
  };
  template <> struct Convertion<short, Integer> {
    static bool Possible() {return true;}
    static void Do(const short &from, Integer &to) {to = (Integer)from;}
  };
  template <> struct Convertion<Integer, short> {
    static bool Possible() {return true;}
    static void Do(const Integer &from, short &to) {to = (short)from;}
  };
  template <> struct Convertion<unsigned short, Integer> {
    static bool Possible() {return true;}
    static void Do(const unsigned short &from, Integer &to) {to = (Integer)from;}
  };
  template <> struct Convertion<Integer, unsigned short> {
    static bool Possible() {return true;}
    static void Do(const Integer &from, unsigned short &to) {to = (unsigned short)from;}
  };
  template <> struct Convertion<int, Integer> {
    static bool Possible() {return true;}
    static void Do(const int &from, Integer &to) {to = (Integer)from;}
  };
  template <> struct Convertion<Integer, int> {
    static bool Possible() {return true;}
    static void Do(const Integer &from, int &to) {to = (int)from;}
  };
  template <> struct Convertion<unsigned int, Integer> {
    static bool Possible() {return true;}
    static void Do(const unsigned int &from, Integer &to) {to = (Integer)from;}
  };
  template <> struct Convertion<Integer, unsigned int> {
    static bool Possible() {return true;}
    static void Do(const Integer &from, unsigned int &to) {to = (unsigned int)from;}
  };
#ifndef NATIVE_64BIT_LONG
  template <> struct Convertion<long, Integer> {
    static bool Possible() {return true;}
    static void Do(const long &from, Integer &to) {to = (Integer)from;}
  };
  template <> struct Convertion<Integer, long> {
    static bool Possible() {return true;}
    static void Do(const Integer &from, long &to) {to = (long)from;}
  };
#endif
  template <> struct Convertion<unsigned long, Integer> {
    static bool Possible() {return true;}
    static void Do(const unsigned long &from, Integer &to) {to = (Integer)from;}
  };
  template <> struct Convertion<Integer, unsigned long> {
    static bool Possible() {return true;}
    static void Do(const Integer &from, unsigned long &to) {to = (unsigned long)from;}
  };
  template <> struct Convertion<float, Integer> {
    static bool Possible() {return true;}
    static void Do(const float &from, Integer &to) {to = (Integer)from;}
  };
  template <> struct Convertion<Integer, float> {
    static bool Possible() {return true;}
    static void Do(const Integer &from, float &to) {to = (float)from;}
  };
  template <> struct Convertion<double, Integer> {
    static bool Possible() {return true;}
    static void Do(const double &from, Integer &to) {to = (Integer)from;}
  };
  template <> struct Convertion<Integer, double> {
    static bool Possible() {return true;}
    static void Do(const Integer &from, double &to) {to = (double)from;}
  };
  /*
  // plain type to bool
  template <> struct Convertion<char, bool> {
    static bool Possible() {return true;}
    static void Do(const char &from, bool &to) {to = (from != 0);}
  };
  template <> struct Convertion<bool, char> {
    static bool Possible() {return true;}
    static void Do(const bool &from, char &to) {to = (from ? 1 : 0);}
  };
  template <> struct Convertion<unsigned char, bool> {
    static bool Possible() {return true;}
    static void Do(const unsigned char &from, bool &to) {to = (from != 0);}
  };
  template <> struct Convertion<bool, unsigned char> {
    static bool Possible() {return true;}
    static void Do(const bool &from, unsigned char &to) {to = (from ? 1 : 0);}
  };
  template <> struct Convertion<short, bool> {
    static bool Possible() {return true;}
    static void Do(const short &from, bool &to) {to = (from != 0);}
  };
  template <> struct Convertion<bool, short> {
    static bool Possible() {return true;}
    static void Do(const bool &from, short &to) {to = (from ? 1 : 0);}
  };
  template <> struct Convertion<unsigned short, bool> {
    static bool Possible() {return true;}
    static void Do(const unsigned short &from, bool &to) {to = (from != 0);}
  };
  template <> struct Convertion<bool, unsigned short> {
    static bool Possible() {return true;}
    static void Do(const bool &from, unsigned short &to) {to = (from ? 1 : 0);}
  };
  template <> struct Convertion<int, bool> {
    static bool Possible() {return true;}
    static void Do(const int &from, bool &to) {to = (from != 0);}
  };
  template <> struct Convertion<bool, int> {
    static bool Possible() {return true;}
    static void Do(const bool &from, int &to) {to = (from ? 1 : 0);}
  };
  template <> struct Convertion<unsigned int, bool> {
    static bool Possible() {return true;}
    static void Do(const unsigned int &from, bool &to) {to = (from != 0);}
  };
  template <> struct Convertion<bool, unsigned int> {
    static bool Possible() {return true;}
    static void Do(const bool &from, unsigned int &to) {to = (from ? 1 : 0);}
  };
  template <> struct Convertion<long, bool> {
    static bool Possible() {return true;}
    static void Do(const long &from, bool &to) {to = (from != 0);}
  };
  template <> struct Convertion<bool, long> {
    static bool Possible() {return true;}
    static void Do(const bool &from, long &to) {to = (from ? 1 : 0);}
  };
  template <> struct Convertion<unsigned long, bool> {
    static bool Possible() {return true;}
    static void Do(const unsigned long &from, bool &to) {to = (from != 0);}
  };
  template <> struct Convertion<bool, unsigned long> {
    static bool Possible() {return true;}
    static void Do(const bool &from, unsigned long &to) {to = (from ? 1 : 0);}
  };
  template <> struct Convertion<float, bool> {
    static bool Possible() {return true;}
    static void Do(const float &from, bool &to) {to = (from != 0.0f);}
  };
  template <> struct Convertion<bool, float> {
    static bool Possible() {return true;}
    static void Do(const bool &from, float &to) {to = (from ? 1.0f : 0.0f);}
  };
  template <> struct Convertion<double, bool> {
    static bool Possible() {return true;}
    static void Do(const double &from, bool &to) {to = (from != 0.0);}
  };
  template <> struct Convertion<bool, double> {
    static bool Possible() {return true;}
    static void Do(const bool &from, double &to) {to = (from ? 1.0 : 0.0);}
  };
  */
  // plain type to real
  template <> struct Convertion<char, Real> {
    static bool Possible() {return true;}
    static void Do(const char &from, Real &to) {to = (Real)from;}
  };
  template <> struct Convertion<Real, char> {
    static bool Possible() {return true;}
    static void Do(const Real &from, char &to) {to = (char)from;}
  };
  template <> struct Convertion<unsigned char, Real> {
    static bool Possible() {return true;}
    static void Do(const unsigned char &from, Real &to) {to = (Real)from;}
  };
  template <> struct Convertion<Real, unsigned char> {
    static bool Possible() {return true;}
    static void Do(const Real &from, unsigned char &to) {to = (unsigned char)from;}
  };
  template <> struct Convertion<short, Real> {
    static bool Possible() {return true;}
    static void Do(const short &from, Real &to) {to = (Real)from;}
  };
  template <> struct Convertion<Real, short> {
    static bool Possible() {return true;}
    static void Do(const Real &from, short &to) {to = (short)from;}
  };
  template <> struct Convertion<unsigned short, Real> {
    static bool Possible() {return true;}
    static void Do(const unsigned short &from, Real &to) {to = (Real)from;}
  };
  template <> struct Convertion<Real, unsigned short> {
    static bool Possible() {return true;}
    static void Do(const Real &from, unsigned short &to) {to = (unsigned short)from;}
  };
  template <> struct Convertion<int, Real> {
    static bool Possible() {return true;}
    static void Do(const int &from, Real &to) {to = (Real)from;}
  };
  template <> struct Convertion<Real, int> {
    static bool Possible() {return true;}
    static void Do(const Real &from, int &to) {to = (int)from;}
  };
  template <> struct Convertion<unsigned int, Real> {
    static bool Possible() {return true;}
    static void Do(const unsigned int &from, Real &to) {to = (Real)from;}
  };
  template <> struct Convertion<Real, unsigned int> {
    static bool Possible() {return true;}
    static void Do(const Real &from, unsigned int &to) {to = (unsigned int)from;}
  };
#ifndef NATIVE_64BIT_LONG
  template <> struct Convertion<long, Real> {
    static bool Possible() {return true;}
    static void Do(const long &from, Real &to) {to = (Real)from;}
  };
  template <> struct Convertion<Real, long> {
    static bool Possible() {return true;}
    static void Do(const Real &from, long &to) {to = (long)from;}
  };
#endif
  template <> struct Convertion<unsigned long, Real> {
    static bool Possible() {return true;}
    static void Do(const unsigned long &from, Real &to) {to = (Real)from;}
  };
  template <> struct Convertion<Real, unsigned long> {
    static bool Possible() {return true;}
    static void Do(const Real &from, unsigned long &to) {to = (unsigned long)from;}
  };
  template <> struct Convertion<float, Real> {
    static bool Possible() {return true;}
    static void Do(const float &from, Real &to) {to = (Real)from;}
  };
  template <> struct Convertion<Real, float> {
    static bool Possible() {return true;}
    static void Do(const Real &from, float &to) {to = (float)from;}
  };
  
}

#endif



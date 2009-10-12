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

#ifndef __lwc_types_h__
#define __lwc_types_h__

#include <lwc/config.h>

namespace lwc {
  
  enum Type {
    AT_UNKNOWN = -1,
    AT_BOOL,
    AT_CHAR,
    AT_UCHAR,
    AT_SHORT,
    AT_USHORT,
    AT_INT,
    AT_UINT,
    AT_LONG,
    AT_ULONG,
    AT_FLOAT,
    AT_DOUBLE,
    AT_STRING,
    AT_OBJECT
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
  
  class LWC_API Object;
  
  template <> struct Enum2Type<AT_BOOL> {typedef bool Type;};
  template <> struct Enum2Type<AT_OBJECT> {typedef Object* Type;};
  template <> struct Enum2Type<AT_STRING> {typedef char* Type;};
  template <> struct Enum2Type<AT_CHAR> {typedef char Type;};
  template <> struct Enum2Type<AT_UCHAR> {typedef unsigned char Type;};
  template <> struct Enum2Type<AT_SHORT> {typedef short Type;};
  template <> struct Enum2Type<AT_USHORT> {typedef unsigned short Type;};
  template <> struct Enum2Type<AT_INT> {typedef int Type;};
  template <> struct Enum2Type<AT_UINT> {typedef unsigned int Type;};
  template <> struct Enum2Type<AT_LONG> {typedef long Type;};
  template <> struct Enum2Type<AT_ULONG> {typedef unsigned long Type;};
  template <> struct Enum2Type<AT_FLOAT> {typedef float Type;};
  template <> struct Enum2Type<AT_DOUBLE> {typedef double Type;};
  
  template <> struct Type2Enum<bool> {enum {Enum = AT_BOOL};};
  template <> struct Type2Enum<char> {enum {Enum = AT_CHAR};};
  template <> struct Type2Enum<unsigned char> {enum {Enum = AT_UCHAR};};
  template <> struct Type2Enum<short> {enum {Enum = AT_SHORT};};
  template <> struct Type2Enum<unsigned short> {enum {Enum = AT_USHORT};};
  template <> struct Type2Enum<int> {enum {Enum = AT_INT};};
  template <> struct Type2Enum<unsigned int> {enum {Enum = AT_UINT};};
  template <> struct Type2Enum<long> {enum {Enum = AT_LONG};};
  template <> struct Type2Enum<unsigned long> {enum {Enum = AT_ULONG};};
  template <> struct Type2Enum<float> {enum {Enum = AT_FLOAT};};
  template <> struct Type2Enum<double> {enum {Enum = AT_DOUBLE};};
  template <> struct Type2Enum<char*> {enum {Enum = AT_STRING};};
  template <> struct Type2Enum<Object*> {enum {Enum = AT_OBJECT};};
}

#endif



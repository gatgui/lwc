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

#ifndef __lwc_traits_h__
#define __lwc_traits_h__

#include <lwc/config.h>

namespace lwc {
  
  template <typename T> struct NoRef {
    typedef T Type;
  };
  template <typename T> struct NoRef<const T&> {
    typedef const T Type;
  };
  template <typename T> struct NoRef<T&> {
    typedef T Type;
  };
  
  template <typename T> struct NoPtr {
    typedef T Type;
  };
  template <typename T> struct NoPtr<const T*> {
    typedef const T Type;
  };
  template <typename T> struct NoPtr<T*> {
    typedef T Type;
  };
  
  template <typename T> struct TypeTraits {
    typedef const T& ConstRef;
    typedef const T* ConstPtr;
    typedef T& Ref;
    typedef T* Ptr;
    typedef T Value;
    typedef const T ConstValue;
    typedef T Self;
    enum {IsConst = 0};
    enum {IsPtr   = 0};
    enum {IsRef   = 0};
  };
  
  template <typename T> struct TypeTraits<const T> {
    typedef const T& ConstRef;
    typedef const T* ConstPtr;
    typedef T& Ref;
    typedef T* Ptr;
    typedef T Value;
    typedef const T ConstValue;
    typedef const T Self;
    enum {IsConst = 1};
    enum {IsPtr   = 0};
    enum {IsRef   = 0};
  };
  
  template <typename T> struct TypeTraits<const T&> {
    typedef const T& ConstRef;
    typedef const T* ConstPtr;
    typedef T& Ref;
    typedef T* Ptr;
    typedef T Value;
    typedef const T ConstValue;
    typedef const T& Self;
    enum {IsConst = 1};
    enum {IsPtr   = 0};
    enum {IsRef   = 1};
  };
  
  template <typename T> struct TypeTraits<const T*> {
    typedef const T& ConstRef;
    typedef const T* ConstPtr;
    typedef T& Ref;
    typedef T* Ptr;
    typedef T Value;
    typedef const T ConstValue;
    typedef const T* Self;
    enum {IsConst = 1};
    enum {IsPtr   = 1};
    enum {IsRef   = 0};
  };
  
  template <typename T> struct TypeTraits<T&> {
    typedef const T& ConstRef;
    typedef const T* ConstPtr;
    typedef T& Ref;
    typedef T* Ptr;
    typedef T Value;
    typedef const T ConstValue;
    typedef T& Self;
    enum {IsConst = 0};
    enum {IsPtr   = 0};
    enum {IsRef   = 1};
  };
  
  template <typename T> struct TypeTraits<T*> {
    typedef const T& ConstRef;
    typedef const T* ConstPtr;
    typedef T& Ref;
    typedef T* Ptr;
    typedef T Value;
    typedef const T ConstValue;
    typedef T* Self;
    enum {IsConst = 0};
    enum {IsPtr   = 1};
    enum {IsRef   = 0};
  };
  
}

#endif



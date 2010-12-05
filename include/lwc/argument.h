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
      Argument(Direction d, Type t, Integer lenidx=-1);
      Argument(const Argument &rhs);
      ~Argument();
      
      Argument& operator=(const Argument &rhs);
      
      void fromDeclaration(const ArgumentDecl &decl);
      
      std::string toString() const;
      
      Argument& setDir(Direction d);
      Argument& setType(Type t);
      Argument& setArraySizeArg(Integer idx);
      Argument& setArrayArg(Integer idx);
      
      inline Direction getDir() const {return mDir;}
      inline Type getType() const {return mType;}
      inline bool isArray() const {return mArray;}
      inline Integer arraySizeArg() const {return mArraySizeArg;}
      inline Integer arrayArg() const {return mArrayArg;}
      inline int indirectionLevel() const {return mIndirectionLevel;}
      
    private:
      
      Type mType;
      Direction mDir;
      bool mArray;
      Integer mArraySizeArg;
      Integer mArrayArg;
      
      int mIndirectionLevel;
  };
}

#endif

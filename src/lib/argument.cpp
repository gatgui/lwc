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

#include <lwc/argument.h>
#include <sstream>

namespace lwc {

Argument::Argument()
  : mType(AT_UNKNOWN), mDir(AD_IN), mArray(false),
    mArraySizeArg(-1), mArrayArg(-1), mIndirectionLevel(0),
    mName(""), mHasDefault(false) {
}

Argument::Argument(Direction d, Type t, Integer lenidx, const char *name)
  : mType(AT_UNKNOWN), mDir(AD_IN), mArray(false),
    mArraySizeArg(-1), mArrayArg(-1), mIndirectionLevel(0),
    mName((name == 0 ? "" : name)),
    mHasDefault(false) {
  setDir(d);
  setType(t);
  setArraySizeArg(lenidx);
}

Argument::Argument(const Argument &rhs)
  : mType(rhs.mType), mDir(rhs.mDir), mArray(rhs.mArray),
    mArraySizeArg(rhs.mArraySizeArg), mArrayArg(rhs.mArrayArg),
    mIndirectionLevel(rhs.mIndirectionLevel), mName(rhs.mName),
    mHasDefault(rhs.mHasDefault) {
  if (mHasDefault) {
    //rhs.getDefaultValue();
    //setDefaultValue();
  }
}

Argument& Argument::operator=(const Argument &rhs) {
  if (this != &rhs) {
    mType = rhs.mType;
    mDir = rhs.mDir;
    mArray = rhs.mArray;
    mArraySizeArg = rhs.mArraySizeArg;
    mArrayArg = rhs.mArrayArg;
    mIndirectionLevel = rhs.mIndirectionLevel;
    mName = rhs.mName;
    mHasDefault = rhs.mHasDefault;
    if (mHasDefault) {
      //rhs.getDefaultValue();
      //setDefaultValue();
    }
  }
  return *this;
}

Argument::~Argument() {
}

Argument& Argument::setDir(Direction d) {
  mDir = d;
  mIndirectionLevel = (mDir != AD_IN ? 1 : 0) + (mArray ? 1 : 0);
  return *this;
}

Argument& Argument::setType(Type t) {
  if (t >= AT_ARRAY_BASE) {
    mType = Type(t - AT_ARRAY_BASE);
    mArray = true;
  } else {
    mType = t;
    mArray = false;
    mArraySizeArg = -1;
  }
  mIndirectionLevel = (mDir != AD_IN ? 1 : 0) + (mArray ? 1 : 0);
  return *this;
}

Argument& Argument::setArraySizeArg(Integer idx) {
  if (mArray) {
    mArraySizeArg = idx;
  }
  return *this;
}

Argument& Argument::setArrayArg(Integer idx) {
  mArrayArg = idx;
  return *this;
}

Argument& Argument::setName(const char *name) {
  mName = name;
  return *this;
}

void Argument::fromDeclaration(const ArgumentDecl &decl) {
  setDir(decl.dir);
  setType(decl.type);
  setArraySizeArg(decl.arylen);
  if (decl.name != 0) {
    setName(decl.name);
  }
  mArrayArg = -1;
}

std::string Argument::toString() const {
  static const char* argdir[] = {
    "in", "out", "inout", "return"
  };

  static const char* argtype[] = {
    "bool", "integer", "real", "string", "object"
  };

  std::ostringstream oss;
  oss << "[";
  oss << argdir[mDir];
  oss << "] ";
  if (mType <= AT_UNKNOWN || mType > AT_OBJECT_ARRAY) {
    oss << "<unknown>";
  } else {
    oss << argtype[mType];
  }
  if (mArray) {
    oss << " [array, size arg @" << mArraySizeArg << "]";
  }
  return oss.str();
}


}


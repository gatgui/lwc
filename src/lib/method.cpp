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

#include <lwc/method.h>
#include <lwc/object.h>
#include <sstream>
#include <algorithm>
#include <set>
#include <string>

namespace lwc {

MethodPointer::MethodPointer() {
}

MethodPointer::~MethodPointer() {
}

// ---

Method::Method()
  : mPtr(0), mNumPArgs(0) {
}

Method::Method(const Method &rhs)
  : mArgs(rhs.mArgs), mPtr(rhs.mPtr), mDesc(rhs.mDesc),
    mNumPArgs(rhs.mNumPArgs), mNArgIndices(rhs.mNArgIndices) {
}

Method::~Method() {
}

Method& Method::operator=(const Method &rhs) {
  if (this != &rhs) {
    mArgs = rhs.mArgs;
    mPtr = rhs.mPtr;
    mDesc = rhs.mDesc;
    mNumPArgs = rhs.mNumPArgs;
    mNArgIndices = rhs.mNArgIndices;
  }
  return *this;
}

std::string Method::toString() const {
  std::ostringstream oss;
  oss << "(";
  if (mArgs.size() > 0) {
    size_t j = 0;
    for (j=0; j<mArgs.size() - 1; ++j) {
      oss << mArgs[j].toString() << ", ";
    }
    oss << mArgs[j].toString();
  }
  oss << ")";
  return oss.str();
}

std::string Method::docString(const std::string &indent) const {
  std::ostringstream oss;
  oss << indent << "Description: " << mDesc << std::endl;
  if (mArgs.size() > 0) {
    oss << indent << "Arguments:" << std::endl;
    size_t j = 0;
    for (j=0; j<mArgs.size() - 1; ++j) {
      oss << mArgs[j].docString(indent+"  ");
    }
    oss << mArgs[j].docString();
  }
  return oss.str();
}

void Method::addArg(const Argument &arg) throw(std::runtime_error) {
  //if (mArgs.size() >= 16) {
  //  throw std::runtime_error("Methods cannot have more than 16 argument");
  //}
  if (!arg.isNamed()) {
    ++mNumPArgs;
    if (mNumPArgs >= 16) {
      throw std::runtime_error("Methods cannot have more than 16 positional arguments");
    }
  } else {
    mNArgIndices[arg.getName()] = mArgs.size();
  }
  mArgs.push_back(arg);
}

size_t Method::namedArgIndex(const char *name) const throw(std::runtime_error) {
  std::map<std::string, size_t>::const_iterator it = mNArgIndices.find(name);
  if (it == mNArgIndices.end()) {
    std::ostringstream oss;
    oss << "Invalid argument name \"" << name << "\"";
    throw std::runtime_error(oss.str());
  }
  return it->second;
}

void Method::validateArgs() throw(std::runtime_error) {
  // argument names must be unique
  // all positional arguments (unnamed) must precede named ones
  // once a default value is set, all following args must have one too (positional or named)
  std::string name;
  std::set<std::string> names;
  std::set<std::string>::iterator nit;
  bool mustHaveName = false;
  bool mustHaveDefault = false;
  
  for (size_t i=0; i<mArgs.size(); ++i) {
    
    name = mArgs[i].getName();
    if (name.length() > 0) {
      mustHaveName = true;
      nit = names.find(name);
      if (nit != names.end()) {
        std::ostringstream oss;
        oss << "duplicate named argument " << i << ": \"" << name << "\" already declared";
        throw std::runtime_error(oss.str());
      }
    } else if (mustHaveName) {
      std::ostringstream oss;
      oss << "positional argument " << i << " must precede any named argument";
      throw std::runtime_error(oss.str());
    }
    
    if (mArgs[i].hasDefaultValue()) {
      mustHaveDefault = true;
    } else if (mustHaveDefault) {
      std::ostringstream oss;
      oss << "argument " << i << " must have a default value";
      throw std::runtime_error(oss.str());
    }
    
    if (mArgs[i].isArray()) {
      if (mArgs[i].arraySizeArg() <= 0) {
        std::ostringstream oss;
        oss << "argument " << i << " missing array size argument";
        throw std::runtime_error(oss.str());
      }
      size_t idx = (size_t) mArgs[i].arraySizeArg();
      if (idx >= mArgs.size()) {
        std::ostringstream oss;
        oss << "argument " << i << " has an invalid array size argument";
        throw std::runtime_error(oss.str());
      }
      if (mArgs[idx].getType() != AT_INT) {
        std::ostringstream oss;
        oss << "argument " << i << " has an invalid array size argument type";
        throw std::runtime_error(oss.str());
      }
      if (mArgs[i].getDir() != mArgs[idx].getDir()) {
        std::ostringstream oss;
        oss << "argument " << i << " has an invalid array size argument direction";
        throw std::runtime_error(oss.str());
      }
      mArgs[idx].setArrayArg(Integer(i));
    }
  }
}

// ---

MethodsTable::MethodsTable(const MethodsTable *parent)
  : mParent(parent) {
}

MethodsTable::~MethodsTable() {
  mTable.clear();
}

size_t MethodsTable::numMethods() const {
  // not of overridden methods
  
  if (mParent) {
    size_t n = mParent->numMethods();
    std::map<std::string, Method>::const_iterator it = mTable.begin();
    while (it != mTable.end()) {
      if (mParent->findMethod(it->first.c_str()) == 0) {
        ++n;
      }
      ++it;
    }
    return n;
    
  } else {
    return mTable.size();
  }
}

size_t MethodsTable::availableMethods(std::vector<std::string> &methodNames) const {
  
  std::map<std::string, Method>::const_iterator it = mTable.begin();
  
  if (mParent) {
    mParent->availableMethods(methodNames);
    while (it != mTable.end()) {
      if (mParent->findMethod(it->first.c_str()) == 0) {
        methodNames.push_back(it->first);
      }
      ++it;
    }
    
  } else {
    methodNames.clear();
    while (it != mTable.end()) {
      methodNames.push_back(it->first);
      ++it;
    }
  }
  return methodNames.size();
}

void MethodsTable::addMethod(const char *name, const Method &m, bool override) throw(std::runtime_error) {
  std::map<std::string, Method>::iterator it = mTable.find(name);
  if (override == false && it != mTable.end()) {
    std::ostringstream oss;
    oss << "Object::addMethod: Method already defined \"" << name << "\"";
    throw std::runtime_error(oss.str());
  }
  mTable[name] = m;
  try {
    mTable[name].validateArgs();
  } catch (std::runtime_error &e) {
    std::ostringstream oss;
    oss << "Object::addMethod: Invalid arguments spec for method \"" << name << "\"" << std::endl;
    oss << e.what();
    throw std::runtime_error(oss.str());
  }
}

void MethodsTable::fromDeclaration(const MethodDecl *decls, size_t n, bool override) throw(std::runtime_error) {
  for (size_t i=0; i<n; ++i) {
    
    const MethodDecl &decl = decls[i];
    
    Method m;
    Argument arg;
    
    m.setPointer(decl.ptr);
    m.setDescription(decl.desc);
    
    for (Integer j=0; j<decl.nargs; ++j) {
      const ArgumentDecl &a = decl.args[j];
      
      arg.fromDeclaration(a);
      m.addArg(arg);
    }
    
    addMethod(decl.name, m, override);
  }
}

std::string MethodsTable::toString() const {
  std::ostringstream oss;
  std::map<std::string, Method>::const_iterator it = mTable.begin();
  while (it != mTable.end()) {
    oss << it->first << it->second.toString() << std::endl;
    ++it;
  }
  if (mParent) {
    oss << "Inherited:" << std::endl;
    oss << mParent->toString();
  }
  return oss.str();
}

std::string MethodsTable::docString(const std::string &indent) const {
  std::ostringstream oss;
  std::map<std::string, Method>::const_iterator it = mTable.begin();
  while (it != mTable.end()) {
    oss << indent << it->first << std::endl;
    oss << it->second.docString(indent+"  ");
    ++it;
  }
  if (mParent) {
    oss << indent << "Inherited:" << std::endl;
    oss << mParent->docString(indent+"  ");
  }
  return oss.str();
}

// ---

MethodParams::MethodParams(const Method &m)
  : mMethod(m) {
}

MethodParams::MethodParams(const MethodParams &rhs)
  : mMethod(rhs.mMethod) {
  memcpy(mParams, rhs.mParams, 16*sizeof(void*));
}

MethodParams::~MethodParams() {
}

}

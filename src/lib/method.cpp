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

#include <lwc/method.h>
#include <lwc/object.h>
#include <sstream>

namespace lwc {

MethodPointer::MethodPointer() {
}

MethodPointer::~MethodPointer() {
}

// ---

Method::Method()
  : mPtr(0) {
}

Method::Method(const Method &rhs)
  : mArgs(rhs.mArgs), mPtr(rhs.mPtr) {
}

Method::~Method() {
}

Method& Method::operator=(const Method &rhs) {
  if (this != &rhs) {
    mArgs = rhs.mArgs;
    mPtr = rhs.mPtr;
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

void Method::addArg(const Argument &arg) throw(std::runtime_error) {
  if (mArgs.size() >= 16) {
    throw std::runtime_error("Methods cannot have more than 16 argument");
  }
  mArgs.push_back(arg);
}

void Method::validateArgs() throw(std::runtime_error) {
  for (size_t i=0; i<mArgs.size(); ++i) {
    if (mArgs[i].isArray()) {
      if (mArgs[i].arraySizeArg() == -1) {
        std::ostringstream oss;
        oss << "argument " << i << " missing array size argument";
        throw std::runtime_error(oss.str());
      }
      Integer idx = mArgs[i].arraySizeArg();
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

MethodsTable::MethodsTable() {
}

MethodsTable::~MethodsTable() {
  mTable.clear();
}

size_t MethodsTable::availableMethods(std::vector<std::string> &methodNames) const {
  methodNames.clear();
  std::map<std::string, Method>::const_iterator it = mTable.begin();
  while (it != mTable.end()) {
    methodNames.push_back(it->first);
    ++it;
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
    
    for (size_t j=0; j<decl.nargs; ++j) {
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

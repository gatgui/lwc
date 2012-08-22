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

#include <lwc/loader.h>
#include <lwc/registry.h>

namespace lwc {

Loader::Loader() {
}

Loader::~Loader() {
}

bool Loader::registerType(const char *name, Factory *f, Registry *reg) {
  if (!name || !f || !reg) {
    return false;
  }
  if (reg->registerType(name, this, f->isSingleton(name))) {
    mFactories[name] = f;
    return true;
  } else {
    return false;
  }
}

const char* Loader::getTypeName(size_t idx) const throw(std::runtime_error) {
  if (idx >= mFactories.size()) {
    std::ostringstream oss;
    oss << "Type index out of range: \"" << idx << "\"";
    throw std::runtime_error(oss.str());
  }
  std::map<std::string, Factory*>::const_iterator it = mFactories.begin();
  for (size_t i=0; i<idx; ++i, ++it) {}
  return it->first.c_str();
}

const MethodsTable* Loader::getMethods(const char *name) {
  if (!name) {
    return 0;
  }
  std::map<std::string, Factory*>::iterator it = mFactories.find(name);
  if (it != mFactories.end()) {
    return it->second->getMethods(name);
  } else {
    return 0;
  }
}

const char* Loader::getDescription(const char *name) {
  if (!name) {
    return 0;
  }
  std::map<std::string, Factory*>::iterator it = mFactories.find(name);
  if (it != mFactories.end()) {
    return it->second->getDescription(name);
  } else {
    return 0;
  }
}

std::string Loader::docString(const char *name, const std::string &indent) {
  if (!name) {
    return 0;
  }
  std::map<std::string, Factory*>::iterator it = mFactories.find(name);
  if (it != mFactories.end()) {
    return it->second->docString(name, indent);
  } else {
    return "";
  }
}

Object* Loader::create(const char *name) {
  if (!name) {
    return 0;
  }
  std::map<std::string, Factory*>::iterator it = mFactories.find(name);
  if (it != mFactories.end()) {
    Object *obj = it->second->create(name);
    obj->setMethodTable(it->second->getMethods(name));
    obj->setTypeName(name);
    obj->setLoaderName(getName());
    return obj;
  } else {
    return 0;
  }
}

void Loader::destroy(Object *o) {
  if (!o || o->mLoaderName != getName()) {
    return;
  }
  std::map<std::string, Factory*>::iterator it = mFactories.find(o->mTypeName.c_str());
  if (it != mFactories.end()) {
    it->second->destroy(o);
  }
}

}




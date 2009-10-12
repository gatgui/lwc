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

#include <lwc/object.h>
#include <lwc/registry.h>

int main(int, char**) {
  
  lwc::Registry *reg = lwc::Registry::Initialize();
  
  reg->addLoaderPath("./components/loaders");
  reg->addModulePath("./components/modules");
  
  std::cout << "Num types: " << reg->numTypes() << std::endl;
  
  // or, if already initialized
  //lwc::ObjectRegistry &reg = lwc::ObjectRegistry::Instance();
  std::cout << "=== test.DoubleBox methods" << std::endl;
  const lwc::MethodsTable *mt = reg->getMethods("test.DoubleBox");
  std::cout << mt->toString();
  
  lwc::Object *b = reg->create("test.DoubleBox");
  
  if (!b) {
    std::cout << "Object no registered" << std::endl;
    lwc::Registry::DeInitialize();
    return -1;
  }
  
  std::cout << "=== Available methods: " << std::endl;
  std::vector<std::string> names;
  b->availableMethods(names);
  for (size_t i=0; i<names.size(); ++i) {
    std::cout << names[i] << std::endl;
  }
  
  std::cout << "=== Fool around..." << std::endl;
  b->call("setX", 10);
  b->call("setY", 10);
  b->call("setWidth", 400);
  b->call("setHeight", 400);
  
  int val;
  
  b->call("getX", &val);
  std::cout << "getX = " << val << std::endl;
  
  val = 1;
  
  b->call("setY", val);
  b->call("setX", val);
  
  std::cout << "call toBox" << std::endl;
  lwc::Object *c;
  b->call("toBox", &c);
  
  c->call("getX", &val);
  std::cout << "getX = " << val << std::endl;
  c->call("getY", &val);
  std::cout << "getY = " << val << std::endl;
  c->call("getWidth", &val);
  std::cout << "getWidth = " << val << std::endl;
  c->call("getHeight", &val);
  std::cout << "getHeight = " << val << std::endl;
  
  reg->destroy(c);
  reg->destroy(b);
  
  lwc::Registry::DeInitialize();
  
  return 0;
}

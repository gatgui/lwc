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

#include <lwc/object.h>
#include <lwc/registry.h>

using lwc::Integer;

// ---

namespace details
{
   template <bool Cond1, bool Cond2>
   struct Or
   {
      enum
      {
         Value = 1
      };
   };
   
   template <>
   struct Or<false, false>
   {
      enum
      {
         Value = 0
      };
   };
   
   template <bool Cond1, bool Cond2>
   struct And
   {
      enum
      {
         Value = 0
      };
   };
   
   template <>
   struct And<true, true>
   {
      enum
      {
         Value = 1
      };
   };
   
   template <bool StopCond, typename T>
   struct _BaseType
   {
      typedef typename gcore::TypeTraits<T>::Value BaseType;
      
      enum
      {
         EnumType = lwc::Type2Enum<BaseType>::Enum
      };
      
      typedef typename _BaseType<Or<EnumType != (int)lwc::AT_UNKNOWN, gcore::TypeTraits<BaseType>::IsPtr == 0>::Value == 1, BaseType>::Value Value;
   };
   
   template <typename T>
   struct _BaseType<true, T>
   {
      typedef T Value;
   };
   
   template <bool StopCond, typename T>
   struct _IndirectionLevel
   {
      typedef typename gcore::TypeTraits<T>::Value BaseType;
      
      enum
      {
         EnumType = lwc::Type2Enum<BaseType>::Enum,
         Value = 1 + _IndirectionLevel<Or<EnumType != (int)lwc::AT_UNKNOWN, gcore::TypeTraits<BaseType>::IsPtr == 0>::Value == 1, BaseType>::Value
      };
   };
   
   template <typename T>
   struct _IndirectionLevel<true, T>
   {
      enum
      {
         Value = 0
      };
   };
}

template <typename T>
struct IndirectionLevel
{
   enum
   {
      Value = details::_IndirectionLevel<details::Or<lwc::Type2Enum<T>::Enum != (int)lwc::AT_UNKNOWN, gcore::TypeTraits<T>::IsPtr == 0>::Value == 1, T>::Value
   };
};

template <typename T>
struct BaseType
{
   typedef typename details::_BaseType<details::Or<lwc::Type2Enum<T>::Enum != (int)lwc::AT_UNKNOWN, gcore::TypeTraits<T>::IsPtr == 0>::Value == 1, T>::Value Value;
};

/*
template <typename T>
void printTypeInfo(T val)
{
   std::cout << "Indirection level: " << IndirectionLevel<T>::Count << std::endl;
}
*/

// ---

int main(int, char**) {
  
  /*
   * std::cout << IndirectionLevel<lwc::Object***>::Value << std::endl;
  std::cout << typeid(BaseType<lwc::Object***>::Value).name() << std::endl;
  int e = lwc::Type2Enum<BaseType<lwc::Object***>::Value>::Enum;
  std::cout << e << std::endl;
  return 0;
  */
  
  lwc::Registry *reg = lwc::Registry::Initialize();
  
  reg->addLoaderPath("./components/loaders");
  reg->addModulePath("./components/modules");
  
  std::cout << "Num types: " << reg->numTypes() << std::endl;
  
  std::cout << "doc string for test.DoubleBox" << std::endl << reg->docString("test.DoubleBox") << std::endl;
  
  // or, if already initialized
  //lwc::ObjectRegistry &reg = lwc::ObjectRegistry::Instance();
  std::cout << "=== test.DoubleBox methods" << std::endl;
  std::cout << "test.DoubleBox: " << reg->getDescription("test.DoubleBox") << std::endl;
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
  
  lwc::Object *c = 0;
  Integer val = 0;
  
  std::cout << "=== Fool around..." << std::endl;
  try {
    /*
    val = 10;
    b->call("setX", val);
    b->call("setY", val);
    val = 400;
    b->call("setWidth", val);
    b->call("setHeight", val);
    */
    //b->call("setX", 10);
    //b->call("setY", 10);
    
    //b->call("set");
    
    lwc::KeywordArgs kwargs;
    kwargs.set("normalize", false);
    kwargs.set("scale", 2.0);
    
    lwc::Integer pos[2] = {10, 10};
    
    b->call("set", pos, kwargs);
    b->call("set", pos);
    
    // fails
    //b->call("set", pos, false);
    //b->call("set", pos, 2, 2.0);
    
    kwargs.clear();
    kwargs.set("normalize", false);
    b->call("set", pos, kwargs);
    
    kwargs.set("scale", 3);
    b->call("set", pos, kwargs);
    
    b->call("setWidth", 400);
    b->call("setHeight", 400);
    
    b->call("getX", &val);
    std::cout << "getX = " << val << std::endl;
    b->call("getY", &val);
    std::cout << "getY = " << val << std::endl;
    b->call("getWidth", &val);
    std::cout << "getWidth = " << val << std::endl;
    b->call("getHeight", &val);
    std::cout << "getHeight = " << val << std::endl;
    
    
    val = 1;
    
    b->call("setY", val);
    b->call("setX", val);
    
    std::cout << "call toBox" << std::endl;
    
    b->call("toBox", &c);
    
    c->call("getX", &val);
    std::cout << "getX = " << val << std::endl;
    c->call("getY", &val);
    std::cout << "getY = " << val << std::endl;
    c->call("getWidth", &val);
    std::cout << "getWidth = " << val << std::endl;
    c->call("getHeight", &val);
    std::cout << "getHeight = " << val << std::endl;
    
  } catch (std::exception &e) {
    std::cout << "*** Caught exception: " << e.what() << std::endl;
  }
  
  if (c) reg->destroy(c);
  if (b) reg->destroy(b);
  
  lwc::Registry::DeInitialize();
  
  return 0;
}

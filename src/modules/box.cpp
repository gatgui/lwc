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
#include <lwc/factory.h>
#include <lwc/registry.h>
#include <lwc/moduleutils.h>

class Box : public lwc::Object {
  public:
    Box()
      : lwc::Object(), mX(0), mY(0), mW(1), mH(1) {
    }
    Box(const Box &rhs)
      : lwc::Object(rhs), mX(rhs.mX), mY(rhs.mY), mW(rhs.mW), mH(rhs.mH) {
    }
    virtual ~Box() {
    }
    
    Box& operator=(const Box &rhs) {
      mX = rhs.mX;
      mY = rhs.mY;
      mW = rhs.mW;
      mH = rhs.mH;
      return *this;
    }
    
    void set(lwc::MethodParams &p) {
      std::cout << "Box::set..." << std::endl;
      
      lwc::Integer *c;
      lwc::Integer n;
      lwc::Real s;
      bool normalize;
      
      p.get(0, c);
      p.get(1, n);
      p.getn("normalize", normalize);
      p.getn("scale", s);
      
      if (n != 2) {
         throw std::runtime_error("Box::set: Only accepts array of size 2");
      }
      
      std::cout << "  => {" << c[0] << ", " << c[1] << "}, " << n << ", normalize=" << normalize << ", scale=" << s << "}" << std::endl;
      mX = c[0];
      mY = c[1];
    }
    void setX(lwc::MethodParams &p) {lwc::Integer x; p.get(0, x); mX = x;}
    void setY(lwc::MethodParams &p) {lwc::Integer y; p.get(0, y); mY = y;}
    void setWidth(lwc::MethodParams &p) {lwc::Integer w; p.get(0, w); mW = w;}
    void setHeight(lwc::MethodParams &p) {lwc::Integer h; p.get(0, h); mH = h;}
    
    void getX(lwc::MethodParams &p) {lwc::Integer *x; p.get(0, x); *x = mX;}
    void getY(lwc::MethodParams &p) {lwc::Integer *y; p.get(0, y); *y = mY;}
    void getWidth(lwc::MethodParams &p) {lwc::Integer *w; p.get(0, w); *w =  mW;}
    void getHeight(lwc::MethodParams &p) {lwc::Integer *h; p.get(0, h); *h = mH;}
    //void getX(lwc::MethodParams &p) {p.set(0, mX);}
    //void getY(lwc::MethodParams &p) {p.set(0, mY);}
    //void getWidth(lwc::MethodParams &p) {p.set(0, mW);}
    //void getHeight(lwc::MethodParams &p) {p.set(0, mH);}
  
  protected:
    
    lwc::Integer mX, mY, mW, mH;
};

class DoubleBox : public Box {
  public:
    
    DoubleBox()
      : Box() {
    }
    DoubleBox(const DoubleBox &rhs)
      : Box(rhs) {
    }
    virtual ~DoubleBox() {
    }
    
    DoubleBox& operator=(const DoubleBox &rhs) {
      Box::operator=(rhs);
      return *this;
    }
    
    void setX(lwc::MethodParams &p) {lwc::Integer x; p.get(0, x); mX = 2*x;}
    
    void setY(lwc::MethodParams &p) {lwc::Integer y; p.get(0, y); mY = 2*y;}
    
    void setWidth(lwc::MethodParams &p) {lwc::Integer w; p.get(0, w); mW = 2*w;}
    
    void setHeight(lwc::MethodParams &p) {lwc::Integer h; p.get(0, h); mH = 2*h;}
    
    void toBox(lwc::MethodParams &p) {
      lwc::Object **b;
      p.get(0, b);
      lwc::Object *obj = lwc::Registry::Create("test.Box");
      obj->call("setX", mX);
      obj->call("setY", mY);
      obj->call("setWidth", mW);
      obj->call("setHeight", mH);
      *b = obj;
    }
};

static double defscl = 1.0;

static lwc::MethodDecl BoxMethods[] = {
  {"set", 4, {{lwc::AD_IN, lwc::AT_INT_ARRAY,  1, LWC_NODEF,            NULL},
              {lwc::AD_IN, lwc::AT_INT,       -1, LWC_DEFVAL(2),        NULL},
              {lwc::AD_IN, lwc::AT_BOOL,      -1, LWC_DEFVAL(true),     "normalize"},
              {lwc::AD_IN, lwc::AT_REAL,      -1, LWC_DEFVAL2(defscl),  "scale"}}, LWC_METHOD(Box, set), "Set box origin"},
  {"setX", 1,      {{lwc::AD_IN,  lwc::AT_INT, -1, LWC_NODEF, NULL}}, LWC_METHOD(Box, setX), "Set box origin x coord"},
  {"setY", 1,      {{lwc::AD_IN,  lwc::AT_INT, -1, LWC_NODEF, NULL}}, LWC_METHOD(Box, setY), "Set box origin y coord"},
  {"setWidth", 1,  {{lwc::AD_IN,  lwc::AT_INT, -1, LWC_NODEF, NULL}}, LWC_METHOD(Box, setWidth), "Set box width"},
  {"setHeight", 1, {{lwc::AD_IN,  lwc::AT_INT, -1, LWC_NODEF, NULL}}, LWC_METHOD(Box, setHeight), "Set box height"},
  {"getX", 1,      {{lwc::AD_OUT, lwc::AT_INT, -1, LWC_NODEF, NULL}}, LWC_METHOD(Box, getX), "Get box origin x coord"},
  {"getY", 1,      {{lwc::AD_OUT, lwc::AT_INT, -1, LWC_NODEF, NULL}}, LWC_METHOD(Box, getY), "Get box origin y coord"},
  {"getWidth", 1,  {{lwc::AD_OUT, lwc::AT_INT, -1, LWC_NODEF, NULL}}, LWC_METHOD(Box, getWidth), "Get box width"},
  {"getHeight", 1, {{lwc::AD_OUT, lwc::AT_INT, -1, LWC_NODEF, NULL}}, LWC_METHOD(Box, getHeight), "Get box height"},
};

static lwc::MethodDecl DoubleBoxMethods[] = {
  {"setX", 1,      {{lwc::AD_IN,  lwc::AT_INT,    -1, LWC_NODEF, NULL}}, LWC_METHOD(DoubleBox, setX), "Set box origin x coord"},
  {"setY", 1,      {{lwc::AD_IN,  lwc::AT_INT,    -1, LWC_NODEF, NULL}}, LWC_METHOD(DoubleBox, setY), "Set box origin y coord"},
  {"setWidth", 1,  {{lwc::AD_IN,  lwc::AT_INT,    -1, LWC_NODEF, NULL}}, LWC_METHOD(DoubleBox, setWidth), "Set box width"},
  {"setHeight", 1, {{lwc::AD_IN,  lwc::AT_INT,    -1, LWC_NODEF, NULL}}, LWC_METHOD(DoubleBox, setHeight), "Set box height"},
  {"toBox", 1,     {{lwc::AD_OUT, lwc::AT_OBJECT, -1, LWC_NODEF, NULL}}, LWC_METHOD(DoubleBox, toBox), "Convert to standard box"},
};

LWC_BEGIN_MODULE(2)
LWC_MODULE_TYPE(0, "test.Box", Box, BoxMethods, false, "Box primitive")
LWC_MODULE_DERIVED_TYPE(1, "test.DoubleBox", DoubleBox, DoubleBoxMethods, false, "Box primitive that doubles its origin and dimensions", 0)
LWC_END_MODULE()

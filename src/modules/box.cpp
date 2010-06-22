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
    
    void setX(lwc::MethodParams &p) {lwc::Integer x; p.get(0, x); mX = x;}
    void setY(lwc::MethodParams &p) {lwc::Integer y; p.get(0, y); mY = y;}
    void setWidth(lwc::MethodParams &p) {lwc::Integer w; p.get(0, w); mW = w;}
    void setHeight(lwc::MethodParams &p) {lwc::Integer h; p.get(0, h); mH = h;}
    
    void getX(lwc::MethodParams &p) {lwc::Integer *x; p.get(0, x); *x = mX;}
    void getY(lwc::MethodParams &p) {lwc::Integer *y; p.get(0, y); *y = mY;}
    void getWidth(lwc::MethodParams &p) {lwc::Integer *w; p.get(0, w); *w =  mW;}
    void getHeight(lwc::MethodParams &p) {lwc::Integer *h; p.get(0, h); *h = mH;}
  
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
      lwc::Object *obj = lwc::Registry::Instance()->create("test.Box");
      obj->call("setX", mX);
      obj->call("setY", mY);
      obj->call("setWidth", mW);
      obj->call("setHeight", mH);
      *b = obj;
    }
};

static lwc::MethodDecl BoxMethods[] = {
  {"setX", 1,      {{lwc::AD_IN,  lwc::AT_INT, -1}}, LWC_METHOD(Box, setX)},
  {"setY", 1,      {{lwc::AD_IN,  lwc::AT_INT, -1}}, LWC_METHOD(Box, setY)},
  {"setWidth", 1,  {{lwc::AD_IN,  lwc::AT_INT, -1}}, LWC_METHOD(Box, setWidth)},
  {"setHeight", 1, {{lwc::AD_IN,  lwc::AT_INT, -1}}, LWC_METHOD(Box, setHeight)},
  {"getX", 1,      {{lwc::AD_OUT, lwc::AT_INT, -1}}, LWC_METHOD(Box, getX)},
  {"getY", 1,      {{lwc::AD_OUT, lwc::AT_INT, -1}}, LWC_METHOD(Box, getY)},
  {"getWidth", 1,  {{lwc::AD_OUT, lwc::AT_INT, -1}}, LWC_METHOD(Box, getWidth)},
  {"getHeight", 1, {{lwc::AD_OUT, lwc::AT_INT, -1}}, LWC_METHOD(Box, getHeight)},
};

static lwc::MethodDecl DoubleBoxMethods[] = {
  {"setX", 1,      {{lwc::AD_IN,  lwc::AT_INT,    -1}}, LWC_METHOD(DoubleBox, setX)},
  {"setY", 1,      {{lwc::AD_IN,  lwc::AT_INT,    -1}}, LWC_METHOD(DoubleBox, setY)},
  {"setWidth", 1,  {{lwc::AD_IN,  lwc::AT_INT,    -1}}, LWC_METHOD(DoubleBox, setWidth)},
  {"setHeight", 1, {{lwc::AD_IN,  lwc::AT_INT,    -1}}, LWC_METHOD(DoubleBox, setHeight)},
  {"toBox", 1,     {{lwc::AD_OUT, lwc::AT_OBJECT, -1}}, LWC_METHOD(DoubleBox, toBox)},
};

LWC_BEGIN_MODULE(2)
LWC_MODULE_TYPE(0, "test.Box", Box, BoxMethods)
LWC_MODULE_DERIVED_TYPE(1, "test.DoubleBox", DoubleBox, DoubleBoxMethods, 0)
LWC_END_MODULE()

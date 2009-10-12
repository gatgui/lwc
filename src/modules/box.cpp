/*

This file is part of lwc.

lwc is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

lwc is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with lwc.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <lwc/object.h>
#include <lwc/factory.h>
#include <lwc/registry.h>


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
    
    void setX(lwc::MethodParams &p) {int x; p.get(0, x); mX = x;}
    void setY(lwc::MethodParams &p) {int y; p.get(0, y); mY = y;}
    void setWidth(lwc::MethodParams &p) {int w; p.get(0, w); mW = w;}
    void setHeight(lwc::MethodParams &p) {int h; p.get(0, h); mH = h;}
    
    void getX(lwc::MethodParams &p) {int *x; p.get(0, x); *x = mX;}
    void getY(lwc::MethodParams &p) {int *y; p.get(0, y); *y = mY;}
    void getWidth(lwc::MethodParams &p) {int *w; p.get(0, w); *w =  mW;}
    void getHeight(lwc::MethodParams &p) {int *h; p.get(0, h); *h = mH;}
  
  protected:
    
    int mX, mY, mW, mH;
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
    
    void setX(lwc::MethodParams &p) {int x; p.get(0, x); mX = 2*x;}
    
    void setY(lwc::MethodParams &p) {int y; p.get(0, y); mY = 2*y;}
    
    void setWidth(lwc::MethodParams &p) {int w; p.get(0, w); mW = 2*w;}
    
    void setHeight(lwc::MethodParams &p) {int h; p.get(0, h); mH = 2*h;}
    
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

#define NUMMETHODS(Ary) (sizeof(Ary)/sizeof(lwc::MethodDecl))
#define METHOD(Class,Name) new lwc::TMethodPointer<Class>(&Class::Name)

static lwc::MethodDecl BoxMethods[] = {
  {"setX", 1, {{lwc::AD_IN, lwc::AT_INT, 0, 0, 0, -1}}, METHOD(Box,setX)},
  {"setY", 1, {{lwc::AD_IN, lwc::AT_INT, 0, 0, 0, -1}}, METHOD(Box,setY)},
  {"setWidth", 1, {{lwc::AD_IN, lwc::AT_INT, 0, 0, 0, -1}}, METHOD(Box,setWidth)},
  {"setHeight", 1, {{lwc::AD_IN, lwc::AT_INT, 0, 0, 0, -1}}, METHOD(Box,setHeight)},
  {"getX", 1, {{lwc::AD_RETURN, lwc::AT_INT, 0, 0, 0, -1}}, METHOD(Box,getX)},
  {"getY", 1, {{lwc::AD_RETURN, lwc::AT_INT, 0, 0, 0, -1}}, METHOD(Box,getY)},
  {"getWidth", 1, {{lwc::AD_RETURN, lwc::AT_INT, 0, 0, false, -1}}, METHOD(Box,getWidth)},
  {"getHeight", 1, {{lwc::AD_RETURN, lwc::AT_INT, 0, 0, 0, -1}}, METHOD(Box,getHeight)},
};

class BoxFactory : public lwc::Factory {
  public:
    BoxFactory() {
      mMethods = new lwc::MethodsTable();
      mMethods->fromDeclaration(BoxMethods, NUMMETHODS(BoxMethods), false);
    }
    
    virtual ~BoxFactory() {
      size_t nm = NUMMETHODS(BoxMethods);
      for (size_t i=0; i<nm; ++i) {
        delete BoxMethods[i].ptr;
      }
      delete mMethods;
    }
    
    virtual const lwc::MethodsTable* getMethods(const char *) {
      return mMethods;
    }
    
    virtual lwc::Object* create(const char *) {
      return new Box();
    }
    
    virtual void destroy(lwc::Object *o) {
      if (o) {
        delete o;
      }
    }
  
  protected:
    
    lwc::MethodsTable *mMethods;
};

static lwc::MethodDecl DoubleBoxMethods[] = {
  {"setX", 1, {{lwc::AD_IN, lwc::AT_INT, 0, 0, 0, -1}}, METHOD(DoubleBox,setX)},
  {"setY", 1, {{lwc::AD_IN, lwc::AT_INT, 0, 0, 0, -1}}, METHOD(DoubleBox,setY)},
  {"setWidth", 1, {{lwc::AD_IN, lwc::AT_INT, 0, 0, 0, -1}}, METHOD(DoubleBox,setWidth)},
  {"setHeight", 1, {{lwc::AD_IN, lwc::AT_INT, 0, 0, 0, -1}}, METHOD(DoubleBox,setHeight)},
  {"getX", 1, {{lwc::AD_RETURN, lwc::AT_INT, 0, 0, 0, -1}}, METHOD(Box,getX)},
  {"getY", 1, {{lwc::AD_RETURN, lwc::AT_INT, 0, 0, 0, -1}}, METHOD(Box,getY)},
  {"getWidth", 1, {{lwc::AD_RETURN, lwc::AT_INT, 0, 0, false, -1}}, METHOD(Box,getWidth)},
  {"getHeight", 1, {{lwc::AD_RETURN, lwc::AT_INT, 0, 0, 0, -1}}, METHOD(Box,getHeight)},
  {"toBox", 1, {{lwc::AD_RETURN, lwc::AT_OBJECT, 0, 0, 0, -1}}, METHOD(DoubleBox,toBox)},
};

class DoubleBoxFactory : public lwc::Factory {
  public:
    DoubleBoxFactory() {
      mMethods = new lwc::MethodsTable();
      mMethods->fromDeclaration(DoubleBoxMethods, NUMMETHODS(DoubleBoxMethods));
    }
    
    virtual ~DoubleBoxFactory() {
      size_t nm = NUMMETHODS(DoubleBoxMethods);
      for (size_t i=0; i<nm; ++i) {
        delete DoubleBoxMethods[i].ptr;
      }
      delete mMethods;
    }
    
    virtual const lwc::MethodsTable* getMethods(const char *) {
      return mMethods;
    }
    
    virtual lwc::Object* create(const char *) {
      return new DoubleBox();
    }
    
    virtual void destroy(lwc::Object *o) {
      if (o) {
        delete o;
      }
    }
    
  protected:
    
    lwc::MethodsTable *mMethods;
};

static const size_t NumTypes = 2;

static const char* Types[] = {
  "test.Box",
  "test.DoubleBox"
};

static lwc::Factory* Factories[] = {0, 0};


extern "C" {

#ifdef _WIN32
# define EXPORT __declspec(dllexport)
#else
# define EXPORT
#endif

EXPORT void LWC_ModuleInit() {
  Factories[0] = new BoxFactory();
  Factories[1] = new DoubleBoxFactory();
}

EXPORT size_t LWC_ModuleGetTypeCount() {
  return NumTypes;
}

EXPORT const char* LWC_ModuleGetTypeName(size_t i) {
  if (i < NumTypes) {
    return Types[i];
  } else {
    return 0;
  }
}

EXPORT lwc::Factory* LWC_ModuleGetTypeFactory(size_t i) {
  if (i < NumTypes) {
    return Factories[i];
  } else {
    return 0;
  }
}

EXPORT void LWC_ModuleExit() {
  delete Factories[0];
  delete Factories[1];
} 

}


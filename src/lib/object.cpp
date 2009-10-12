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
#include <sstream>

namespace lwc {

#ifdef LWC_MEMTRACK
static unsigned long InstanceCount = 0;

unsigned long Object::GetInstanceCount() {
  return InstanceCount;
}
#endif

Object::Object() : mMethods(0) {
#ifdef LWC_MEMTRACK
  ++InstanceCount;
#endif
}

Object::~Object() {
#ifdef LWC_MEMTRACK
  --InstanceCount;
#endif
}

void Object::call(const char *name, MethodParams &params) throw(std::runtime_error) {
  //TMethodPointer<Object> *mptr = params.getMethod().getPointer<Object>();
  TMethodPointer<Object> *mptr = (TMethodPointer<Object>*) params.getMethod().getPointer();
  if (!mptr) {
    std::ostringstream oss;
    oss << "Object::call: Invalid pointer for method \"" << name << "\"";
    throw std::runtime_error(oss.str());
  }
  mptr->call(this, params);
}

} // namespace lwc




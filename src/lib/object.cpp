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




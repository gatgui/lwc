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

#ifndef __lwc_rb_methodcall_h__
#define __lwc_rb_methodcall_h__

#include <lwc/ruby/types.h>

namespace rb {

  LWCRB_API VALUE CallMethod(lwc::Object *o, const char *n,
                             lwc::MethodParams &params, int cArg,
                             VALUE *args, size_t nargs, size_t rbArg,
                             std::map<size_t,size_t> &arraySizes);

  }

#endif



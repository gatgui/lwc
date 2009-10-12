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

#ifndef __lwc_lua_methodcall_h__
#define __lwc_lua_methodcall_h__

#include <lwc/lua/types.h>

#define NO_RETVAL -1
#define BAD_METHOD_CALL -2

namespace lua {

  LWCLUA_API int CallMethod(lwc::Object *o, const char *n,
                            lwc::MethodParams &params, int cArg,
                            lua_State *L, int firstArg, size_t nargs, size_t luaArg, // firstArg, index of first arg on stack, luaArg relative to first arg
                            std::map<size_t,size_t> &arraySizes);

}

#endif



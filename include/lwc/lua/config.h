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

#ifndef __lwc_lua_config_h__
#define __lwc_lua_config_h__

#ifdef _WIN32
# ifdef LWCLUA_EXPORTS
#   define LWCLUA_API __declspec(dllexport)
# else
#   define LWCLUA_API __declspec(dllimport)
# endif
#else
# define LWCLUA_API
#endif

#define LWCLUA_DATA_API extern LWCLUA_API

#include <lwc/memory.h>
#include <lwc/object.h>
#include <lwc/registry.h>
#include <lua.hpp>

#endif


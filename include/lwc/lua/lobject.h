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

#ifndef __lwc_lua_lobject_h__
#define __lwc_lua_lobject_h__

#include <lwc/lua/config.h>

namespace lua {

  class LWCLUA_API Object : public lwc::Object {
    public:
      
      Object(lua_State *L, int inst);
      virtual ~Object();
      
      // put self on stack
      int self();
      
      virtual void call(const char *name, lwc::MethodParams &params) throw(std::runtime_error);
    
    protected:
      
      lua_State *mState;
  };

}

#endif



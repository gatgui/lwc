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

#ifndef __lwc_rb_methodcall_h__
#define __lwc_rb_methodcall_h__

#include <lwc/ruby/types.h>

namespace rb {

  LWCRB_API VALUE CallMethod(lwc::Object *o, const char *n,
                             lwc::MethodParams &params, int cArg,
                             VALUE *args, size_t nargs, size_t rbArg,
                             std::map<size_t,size_t> &arraySizes) throw(std::runtime_error);

  }

#endif



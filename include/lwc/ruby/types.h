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

#ifndef __lwc_rb_types_h__
#define __lwc_rb_types_h__

#include <lwc/ruby/config.h>

namespace rb {

  LWCRB_DATA_API VALUE mLWC;
  LWCRB_DATA_API VALUE cLWCArgument;
  LWCRB_DATA_API VALUE cLWCMethod;
  LWCRB_DATA_API VALUE cLWCMethodsTable;
  LWCRB_DATA_API VALUE cLWCObject;
  LWCRB_DATA_API VALUE cLWCRegistry;

  LWCRB_API void SetObjectPointer(VALUE robj, lwc::Object *obj);

  LWCRB_API bool InitArgument(VALUE mod);
  LWCRB_API bool InitMethod(VALUE mod);
  LWCRB_API bool InitMethodsTable(VALUE mod);
  LWCRB_API bool InitObject(VALUE mod);
  LWCRB_API bool InitRegistry(VALUE mod);
  LWCRB_API void CreateModule();

}

#endif




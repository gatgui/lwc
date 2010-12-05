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

#ifndef __lwc_memory_h__
#define __lwc_memory_h__

#include <lwc/config.h>

namespace lwc {
  namespace memory {
    
#ifdef LWC_MEMTRACK
    LWC_API void* Alloc(size_t count, size_t byteSize, void *ptr=0, const char *info=0);
#else
    LWC_API void* Alloc(size_t count, size_t byteSize, void *ptr=0);
#endif
    LWC_API void Free(void *ptr);
    
#ifdef LWC_MEMTRACK
    LWC_API size_t GetAllocatedMemorySize();
    LWC_API void PrintAllocationInfo();
#endif
  }
}

#endif

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

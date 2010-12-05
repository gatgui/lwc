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

#include <lwc/memory.h>
#include <cstdlib>

namespace lwc {
namespace memory {

#ifdef LWC_MEMTRACK

static std::map<void*, size_t> AllocatedSizeMap;
static std::map<void*, std::string> AllocationComments;

size_t GetAllocatedMemorySize() {
  size_t sz = 0;
  std::map<void*, size_t>::iterator it = AllocatedSizeMap.begin();
  while (it != AllocatedSizeMap.end()) {
    sz += it->second;
    ++it;
  }
  return sz;
}

void PrintAllocationInfo() {
  std::map<void*, size_t>::iterator mit = AllocatedSizeMap.begin();
  std::map<void*, std::string>::iterator cit;
  
  size_t sz = GetAllocatedMemorySize();
  std::cout << "Total allocated memory size: " << sz << std::endl;
  if (sz > 0) {
    std::cout << "Details:" << std::endl;
    while (mit != AllocatedSizeMap.end()) {
      std::cout << "  " << mit->second << " bytes allocated @" << mit->first;
      cit = AllocationComments.find(mit->first);
      if (cit != AllocationComments.end()) {
        std::cout << " for \"" << cit->second << "\"";
      }
      std::cout << std::endl;
      ++mit;
    }
  }
}

void* Alloc(size_t count, size_t byteSize, void *ptr, const char *info) {
  size_t sz = count * byteSize;
  void *p = 0;
  std::string oldInfo = "";
  
  std::map<void*, size_t>::iterator it = AllocatedSizeMap.find(ptr);
  std::map<void*, std::string>::iterator cit = AllocationComments.find(ptr);
  
  if (ptr) {
    if (it == AllocatedSizeMap.end()) {
      std::cout << "Memory @" << ptr << " was not allocated by lwc::memory::Alloc" << std::endl;
    } else {
      if (cit != AllocationComments.end()) {
        oldInfo = cit->second;
      }
    }
    p = realloc(ptr, sz);
  } else {
    p = malloc(sz);
  }
  
  if (p != ptr) {
    if (it != AllocatedSizeMap.end()) {
      AllocatedSizeMap.erase(it);
    }
    if (cit != AllocationComments.end()) {
      AllocationComments.erase(cit);
    }
  }
  
  AllocatedSizeMap[p] = sz;
  AllocationComments[p] = (info ? std::string(info) : oldInfo);
  
  return p;
}

void Free(void *ptr) {
  std::map<void*, size_t>::iterator it = AllocatedSizeMap.find(ptr);
  std::map<void*, std::string>::iterator cit = AllocationComments.find(ptr);
  if (it != AllocatedSizeMap.end()) {
    AllocatedSizeMap.erase(it);
    if (cit != AllocationComments.end()) {
      AllocationComments.erase(cit);
    }
  } else {
    std::cout << "Memory @" << ptr << " was not allocated by lwc::memory::Alloc" << std::endl;
  }
  free(ptr);
}

#else

void* Alloc(size_t count, size_t byteSize, void *ptr) {
  if (ptr) {
    return realloc(ptr, count * byteSize);
  } else {
    return malloc(count * byteSize);
  }
}

void Free(void *ptr) {
  free(ptr);
}

#endif

}
}

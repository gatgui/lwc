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

#include <lwc/dynlib.h>
#ifdef _WIN32
# include <windows.h>
#else
# include <dlfcn.h>
#endif

namespace lwc {

DynLib::DynLib()
  : mHandle(0), mName("") {
}

DynLib::DynLib(const std::string &name)
  : mHandle(0), mName(name) {
  open(name);
}

DynLib::~DynLib() {
  if (opened()) {
    close();
  }
}

bool DynLib::opened() const {
  return (mHandle != 0);
}

bool DynLib::open(const std::string &name) {
  if (opened()) {
    if (name == mName) {
      return true;
    }
    close();
  }
#ifndef _WIN32
  // RTLD_LAZY / RTLD_NOW (check all exported symbols at load time)
  // RTLD_GLOBAL / RTLD_LOCAL (symbol can be access by using RTLD_DEFAULT or RTLD_NEXT handle, or only through dlopen handle)
  mHandle = dlopen(name.c_str(), RTLD_LAZY|RTLD_LOCAL);
#else
  mHandle = LoadLibraryEx(name.c_str(),NULL,LOAD_WITH_ALTERED_SEARCH_PATH);
#endif
  return (mHandle != 0);
}

bool DynLib::close() {
  bool r;
#ifndef _WIN32
  r = (dlclose(mHandle) == 0);
#else
  r = (FreeLibrary((HMODULE)mHandle) == TRUE);
#endif
  mHandle = 0;
  mSymbolMap.clear();
  return r;
}

void* DynLib::getSymbol(const std::string &symbol) const {
  if (opened()) {
    std::map<std::string,void*>::iterator it = mSymbolMap.find(symbol);
    if (it != mSymbolMap.end()) {
      return (*it).second;
    }
    void *ptr = 0;
#ifndef _WIN32
    ptr = dlsym(mHandle, symbol.c_str());
#else
    ptr = (void*)GetProcAddress((HMODULE)mHandle, symbol.c_str());
#endif
    if (ptr) {
      mSymbolMap[symbol] = ptr;
    }
    return ptr;
  }
  return 0;
}

std::string DynLib::getError() const {
#ifndef _WIN32
  return dlerror();
#else
  DWORD dw = GetLastError();
  char buffer[512];
  FormatMessage(
    FORMAT_MESSAGE_FROM_SYSTEM,
    NULL, dw, MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
    (LPTSTR)&buffer, sizeof(buffer), NULL);
  return buffer;
#endif
}

}



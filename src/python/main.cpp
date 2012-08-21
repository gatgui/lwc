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

#include <lwc/python/types.h>

/*
#ifdef _WIN32
BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID) {
  switch (fdwReason) {
  case DLL_PROCESS_DETACH:
    py::CleanupModule();
  default:
    break;
  }
  return TRUE;
}
#else
#ifdef __GNUC__
__attribute__((destructor)) void _pylwcexit() {
#else
void fini() {
#endif
  py::CleanupModule();
}
#endif
*/

extern "C" {
#ifdef _WIN32
__declspec(dllexport)
#else
__attribute__ ((visibility ("default")))
#endif
  void initlwcpy(void) {
    py::CreateModule();
  }
  
}


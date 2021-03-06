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

#ifndef __lwc_py_config_h__
#define __lwc_py_config_h__

#ifdef _WIN32
# ifdef LWCPY_EXPORTS
#   define LWCPY_API __declspec(dllexport)
# else
#   define LWCPY_API __declspec(dllimport)
# endif
#else
# define LWCPY_API
#endif

#define LWCPY_DATA_API extern LWCPY_API

#include <lwc/memory.h>
#include <lwc/object.h>
#include <lwc/registry.h>

#if !defined(_WIN32) || !defined(__APPLE__)
#  undef _POSIX_C_SOURCE
#  undef _XOPEN_SOURCE
#endif

#include <Python.h>

#endif

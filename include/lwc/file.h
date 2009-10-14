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

#ifndef __lwc_file_h__
#define __lwc_file_h__

#include <lwc/config.h>

#ifdef _WIN32
# define DIR_SEP '\\'
# define PATH_SEP ';'
#else
# define DIR_SEP '/'
# define PATH_SEP ':'
#endif

namespace lwc {
  
  namespace file {
    
    enum Type {
      FT_FILE = 0,
      FT_DIR,
      FT_UNKNOWN,
      FT_MAX
    };
    
    LWC_API std::string JoinPath(const std::string &path0, const std::string &path1);
    
    LWC_API bool IsAbsolutePath(const std::string &path);
    
    LWC_API std::string MakeAbsolutePath(const std::string &path);
    
    // Normalize path will make path absolute only if necessary
    LWC_API std::string NormalizePath(const std::string &path);
    
    LWC_API bool IsSamePath(const std::string &path0, const std::string &path1);
    
    LWC_API std::string Basename(const std::string &path);
    
    LWC_API std::string Dirname(const std::string &path);
    
    LWC_API std::string FileExtension(const std::string &path);
    
    LWC_API bool CheckFileExtension(const std::string &path, const std::string &ext);
    
    LWC_API std::string GetCurrentDir();
    
    LWC_API bool FileExists(const std::string &path);
    
    LWC_API bool DirExists(const std::string &path);
  
    LWC_API size_t FileSize(const std::string &path);
  
    LWC_API bool CreateDir(const std::string &dir);
  
    // both callback should return false to stop iteration
    
    typedef bool (*EnumEnvCallback)(const std::string&, void*);
    LWC_API void ForEachInEnv(const std::string &e, EnumEnvCallback cb, void *ud=0);
    
    typedef bool (*EnumFileCallback)(const std::string&, const std::string&, Type, void*);
    LWC_API void ForEachInDir(const std::string &d, EnumFileCallback cb, bool recurse=false, void *ud=0);
  }
}

#endif



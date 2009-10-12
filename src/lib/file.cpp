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

#include <lwc/file.h>
#ifdef _WIN32
# include <windows.h>
#else
# include <dirent.h>
#endif
#include <sys/stat.h>

namespace lwc {
namespace file {

#ifdef _WIN32
static void NormalizeSeparators(std::string &path) {
  size_t p0 = 0;
  size_t p1 = path.find('/', p0);
  while (p1 != std::string::npos) {
    path[p1] = '\\';
    p0 = p1 + 1;
    p1 = path.find('/', p0);
  } 
}
#else
static void NormalizeSeparators(std::string &) {
}
#endif 

bool FileExists(const std::string &path) {
#ifdef _WIN32
  DWORD fa;
  fa = GetFileAttributes(path.c_str());
  if (fa != 0xFFFFFFFF) {
    return ((fa & FILE_ATTRIBUTE_DIRECTORY) == 0);
  }
#else
  struct stat st;
  if (stat(path.c_str(), &st) == 0) {
    return ((st.st_mode & S_IFREG) != 0);
  }
#endif
  return false; 
}

bool DirExists(const std::string &path) {
#ifdef _WIN32
  DWORD fa;
  fa = GetFileAttributes(path.c_str());
  if (fa != 0xFFFFFFFF) {
    return ((fa & FILE_ATTRIBUTE_DIRECTORY) != 0);
  }
#else
  struct stat st;
  if (stat(path.c_str(), &st) == 0) {
    return ((st.st_mode & S_IFDIR) != 0);
  }
#endif
  return false;
}

size_t FileSize(const std::string &path) {
	struct stat fileStat;
	if (stat(path.c_str(), &fileStat) == 0) {
		return fileStat.st_size;
	}
	return (size_t)-1;
}

bool CreateDir(const std::string &dir) {
	if (! DirExists(dir)) {
#ifdef _WIN32
		return (CreateDirectory(dir.c_str(), NULL) == TRUE);
#else
    return (mkdir(dir.c_str(), S_IRWXU) == 0);
#endif
	}
	return true;
}

std::string GetCurrentDir() {
#ifdef _WIN32
  DWORD cwdLen = GetCurrentDirectory(0, NULL);
  char *cwd = (char*)malloc(cwdLen * sizeof(char));
  GetCurrentDirectory(cwdLen, cwd);
#else
  char *cwd = getcwd((char*)NULL, 0);
#endif
  std::string rv(cwd);
  free(cwd);
  return rv;
}

std::string FileExtension(const std::string &path) {
  size_t p0 = path.rfind('.');
  if (p0 != std::string::npos) {
#ifdef _WIN32
    size_t p1 = path.find_last_of("\\/");
#else
    size_t p1 = path.rfind('/');
#endif
    if (p1 != std::string::npos && p0 < p1) {
      return "";
    }
    return path.substr(p0);
  } else {
    return "";
  }
}

bool CheckFileExtension(const std::string &path, const std::string &ext) {
  std::string pext = FileExtension(path);
#ifdef _WIN32
#	if _MSC_VER >= 1400
  return (_stricmp(pext.c_str(), ext.c_str()) == 0);
#	else
  return (stricmp(pext.c_str(), ext.c_str()) == 0);
#	endif
#else
  return (strcasecmp(pext.c_str(), ext.c_str()) == 0);
#endif
}

std::string JoinPath(const std::string &path0, const std::string &path1) {
  size_t l0 = path0.length();
  size_t l1 = path1.length();
  if (l0 == 0 || l1 == 0) {
    return "";
  }
  bool addSep = false;
  size_t lj = l0 + l1;
#ifdef _WIN32
  if (path0[l0-1] != '/' && path0[l0-1] != '\\') {
#else
  if (path0[l0-1] != '/') {
#endif
    addSep = true;
    lj += 1;
  }
#ifdef _WIN32
  if (path1[0] == '/' || path1[0] == '\\') {
#else
  if (path1[0] == '/') {
#endif
    addSep = false;
    lj -= 1;
  }
  std::string path = path0;
  if (addSep) {
    path.push_back(DIR_SEP);
  }
  path += path1;
  return path;
}

bool IsAbsolutePath(const std::string &path) {
  if (path.length() == 0) {
    return false;
  }
#ifdef _WIN32
  if (path.length() < 2) {
    return false;
  }
  return (path[0] != '\0' && path[1] == ':');
#else
  return (path[0] == '/');
#endif
}

std::string MakeAbsolutePath(const std::string &path) {
  if (IsAbsolutePath(path) == true) {
    return path;
  } else {
    return JoinPath(GetCurrentDir(), path);
  }
}

std::string NormalizePath(const std::string &path) {
  std::string npath;
  size_t p0 = 0;
#ifdef _WIN32
  size_t p1 = path.find_first_of("/\\");
#else
  size_t p1 = path.find('/');
#endif
  while (p1 != std::string::npos) {
    if (p0 != p1) {
      std::string tmp = path.substr(p0, p1-p0);
      if (tmp == "..") {
        if (npath.length() == 0) {
          npath = GetCurrentDir();
        }
#ifdef _WIN32
        size_t p2 = npath.find_last_of("/\\");
#else
        size_t p2 = npath.rfind('/');
#endif
        if (p2 != std::string::npos) {
          npath.erase(p2);
        } else {
          npath = "";
        }
      } else if (tmp != ".") {
        if (npath.length() > 1) {
          npath.push_back(DIR_SEP);
        }
        npath += tmp;
      }
    } else {
      npath.push_back(path[p0]);
    }
    p0 = p1 + 1;
#ifdef _WIN32
    p1 = path.find_first_of("/\\", p0);
#else
    p1 = path.find('/', p0);
#endif
  }
  if (p0 < path.length()) {
    // remaining chars
    std::string tmp = path.substr(p0);
    if (tmp == "..") {
      if (npath.length() == 0) {
        npath = GetCurrentDir();
      }
#ifdef _WIN32
      size_t p2 = npath.find_last_of("/\\");
#else
      size_t p2 = npath.rfind('/');
#endif
      if (p2 != std::string::npos) {
        npath.erase(p2);
      } else {
        npath = "";
      }
    } else if (tmp != ".") {
      if (npath.length() > 0) {
        npath.push_back(DIR_SEP);
      }
      npath += tmp;
    }
  }
  return npath;
}

std::string Basename(const std::string &path) {
  size_t p;
#ifdef _WIN32
  p = path.find_last_of("\\/");  
#else
  p = path.rfind('/');
#endif
  if (p == std::string::npos) {
    return "";
  }
  return path.substr(p+1);
}

std::string Dirname(const std::string &path) {
  size_t p;
#ifdef _WIN32
  p = path.find_last_of("\\/");
#else
  p = path.rfind('/');
#endif
  if (p == std::string::npos) {
    return "";
  }
  return path.substr(0, p);
}

bool IsSamePath(const std::string &path0, const std::string &path1) {
  size_t l0 = path0.length();
  size_t l1 = path1.length();
  if (l0 == 0 && l1 == 0) {
    return true;
  }
  std::string apath0, apath1;
  if (l0 == 0) {
    apath0 = GetCurrentDir();
  } else {
    apath0 = NormalizePath(MakeAbsolutePath(path0));
  }
  if (l1 == 0) {
    apath1 = GetCurrentDir();
  } else {
    apath1 = NormalizePath(MakeAbsolutePath(path1));
  }
  NormalizeSeparators(apath0);
  NormalizeSeparators(apath1);
#ifdef _WIN32
#	if _MSC_VER >= 1400
  return (_stricmp(apath0.c_str(), apath1.c_str()) == 0);
#	else
  return (stricmp(apath0.c_str(), apath1.c_str()) == 0);
#	endif
#else
  return (strcmp(apath0.c_str(), apath1.c_str()) == 0);
#endif
}

void ForEachInEnv(const std::string &e, EnumEnvCallback callback, void *ud) {
  if (e.length() == 0 || callback == 0) {
    return;
  }
  char *envVal = getenv(e.c_str());
  if (envVal) {
    std::string v = envVal;
    std::cout << e << " = \"" << v << "\"" << std::endl;
    size_t p0 = 0, p1 = v.find(PATH_SEP, p0);
    while (p1 != std::string::npos) {
      std::string tmp = v.substr(p0, p1 - p0);
      if (tmp.length() > 0) {
        if (!(*callback)(tmp, ud)) {
          return;
        }
      }
      p0 = p1 + 1;
      p1 = v.find(PATH_SEP, p0);
    }
    // and last one
    if (p0 < v.length()) {
      std::string tmp = v.substr(p0);
      if (tmp.length() > 0) {
        (*callback)(tmp, ud);
      }
    }
  }
}

void ForEachInDir(const std::string &path, EnumFileCallback callback, bool recurse, void *ud) {
  if (callback == 0) {
    return;
  }
#ifdef _WIN32
  WIN32_FIND_DATA fd;
  HANDLE hFile;
  std::string fffs; // find first file string 
  if (path.length() == 0) {
    fffs = ".\\*.*";
  } else {
    size_t p = path.find("*.*");
    if (p == std::string::npos) {
      if (path[path.length()-1] == '\\' || path[path.length()-1] == '/') {
        fffs = path + "*.*";
      } else {
        fffs = path + "\\*.*";
      }
    } else {
      fffs = path;
    }
  }
  hFile = FindFirstFile(fffs.c_str(), &fd);
  if (hFile != INVALID_HANDLE_VALUE) {
    do {
      std::string fname = fd.cFileName;
      if (fname == "." ||  fname == "..") {
        continue;
      }
      if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
        if (!(*callback)(path, fname, FT_DIR, ud)) {
          break;
        }
        if (recurse) {
          std::string tmp = JoinPath(path, fname);
          ForEachInDir(tmp, callback, true, ud);
        }
      } else {
        if (!(*callback)(path, fname, FT_FILE, ud)) {
          break;
        }
      }
    } while (FindNextFile(hFile, &fd));
    FindClose(hFile);
  }
#else
  DIR *d=0;
  if (path.length() == 0) {
    d = opendir(GetCurrentDir().c_str());
  } else {
    d = opendir(path.c_str());
  }
  if (d) {
    struct dirent *de;
    struct stat st;
    while ((de = readdir(d)) != 0) {
      std::string fname = de->d_name;
      if (fname == "." || fname == "..") {
        continue;
      }
      std::string tmp = JoinPath(path, fname);
      if (stat(tmp.c_str(), &st) == 0) {
        if ((st.st_mode & S_IFREG) != 0) {
          if (!(*callback)(path, fname, FT_FILE, ud)) {
            break;
          }
        } else if ((st.st_mode & S_IFDIR) != 0) {
          if (!(*callback)(path, fname, FT_DIR, ud)) {
            break;
          }
          if (recurse == true) {
            ForEachInDir(tmp, callback, true, ud);
          }
        }
      } else {
        std::cerr << "*** Could not stat \"" << tmp << "\"" << std::endl;
      }
    }
    closedir(d);
  }
#endif
}

}
}


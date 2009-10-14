import os, sys, glob, platform
from string import Template

UNKNOWN = -1
WINDOWS = 0
OSX     = 1
LINUX   = 2


plat = UNKNOWN
arch = "x86" if platform.architecture()[0] == '32bit' else "x64"

debug_build  = int(ARGUMENTS.get("debug", "0"))

if str(Platform()) in ["win32", "cygwin"]:
  plat = WINDOWS
  luaprefix = os.path.join(os.getcwd(), "luawin")
elif str(Platform()) == "darwin":
  plat = OSX
  luaprefix = "/usr/local"
else:
  plat = LINUX
  luaprefix = "/usr"

luaprefix = ARGUMENTS.get("luaprefix", luaprefix)

print("LUA prefix: \"%s\" (override using luaprefix=<prefix>" % luaprefix)
variant = "Debug" if debug_build else "Release"
variant += " %s build (override using debug=1|0)" % arch
print(variant)

def MakeBaseEnvironment():
  if plat == WINDOWS:
    msvs_version = ARGUMENTS.get("MSVS_VERSION", "8.0")
    env = Environment(MSVS_VERSION=msvs_version)
    env["IMPLIBFLAGS"] = ' -implib:$IMPLIB$LIBSUFFIX'
    env["NOIMPLIBFLAGS"] = ' -implib:$OBJDIR/dummy.lib'
    env.Append(CCFLAGS=" -GR -EHsc -FD")
    env.Append(CPPDEFINES=["_WIN32", "WIN32", "_CONSOLE", "_MBCS"])
    if float(msvs_version) > 7.1:
      env['LINKCOM'] = [env['LINKCOM'], 'mt.exe -nologo -manifest ${TARGET}.manifest -outputresource:$TARGET;1']
      env['SHLINKCOM'] = [env['SHLINKCOM'], 'mt.exe -nologo -manifest ${TARGET}.manifest -outputresource:$TARGET;2']
      env.Append(CPPDEFINES='_CRT_SECURE_NO_DEPRECATE')
    if "INCLUDE" in os.environ:
      env.Append(CPPPATH = os.environ["INCLUDE"].split(";"))
    if "LIB" in os.environ:
      env.Append(LIBPATH = os.environ["LIB"].split(";"))
    if debug_build:
      env.Append(CPPDEFINES=["_DEBUG"])
      env.Append(CCFLAGS=" -MDd -Gm -Zi")
      env.Append(LINKFLAGS=" -debug -incremental")
    else:
      env.Append(CPPDEFINES=["NDEBUG"])
      env.Append(CCFLAGS=" -O2 -MD")
      env.Append(LINKFLAGS=" -release -opt:ref -opt:icf -incremental:no")
    # Remove .pdb, .ilk, .manifest
  else:
    env = Environment()
    env.Append(CCFLAGS=" -pipe -fno-common -Wno-ctor-dtor-privacy")
    if plat == OSX:
      env.Append(CCFLAGS=" -flat_namespace")
    if debug_build:
      env.Append(CPPDEFINES=["_DEBUG"])
      env.Append(CCFLAGS=" -O0 -g -ggdb")
    else:
      env.Append(CPPDEFINES=["NDEBUG"])
      env.Append(CCFLAGS=" -O2")
      if plat == OSX:
        env.Append(LINKFLAGS=" -Wl,-dead_strip")
  return env

def LinkPython(e):
  from distutils import sysconfig
  e.Append(CPPPATH=[sysconfig.get_python_inc()])
  if sysconfig.get_config_var("PYTHONFRAMEWORK"):
    a = ' -F' + sysconfig.get_config_var("PYTHONFRAMEWORKPREFIX")
    b = ' -framework ' + sysconfig.get_config_var("PYTHONFRAMEWORK")
    e.Append(LINKFLAGS=a+b)
  else:
    if plat == WINDOWS:
      e.Append(LIBPATH=[sysconfig.PREFIX+'\\libs'])
      e.Append(LIBS=["python%s" % sysconfig.get_python_version().replace(".", "")])
    else:
      e.Append(LINKFLAGS=" %s" % sysconfig.get_config_var("LINKFORSHARED"))
      e.Append(LIBS=["python%s" % sysconfig.get_python_version()])

def LinkRuby(e):
  rb_conf = Template("ruby -e \"require 'rbconfig'; print Config::CONFIG['$flag']\"")
  e.Append(CPPPATH=[os.popen(rb_conf.substitute(flag='archdir')).read()])
  e.Append(LIBPATH=[os.popen(rb_conf.substitute(flag='libdir')).read()])
  e.Append(LIBS=[os.popen(rb_conf.substitute(flag='RUBY_SO_NAME')).read()])

def LinkLua(e, linkstatic=False):
  if plat == WINDOWS:
    e.Append(CPPDEFINES=["LUA_BUILD_AS_DLL"])
    e.Append(CPPPATH=[luaprefix+"/include"])
    e.Append(LIBPATH=[luaprefix+"/lib/"+arch]) # if 64 bit build?
    e.Append(LIBS = ["lua51"])
  else:
    # Requires a dynamic library build of lua
    #e.Append(LINKFLAGS=" -undefined dynamic_lookup")
    e.Append(CPPPATH=[luaprefix+"/include"])
    e.Append(LIBPATH=[luaprefix+"/lib"])
    e.Append(LIBS="lua")

def AsPythonExtension(e):
  from distutils import sysconfig
  e.Append(CPPPATH=[sysconfig.get_python_inc()])
  e['SHLIBPREFIX'] = ''
  e['SHLIBSUFFIX'] = sysconfig.get_config_var("SO")
  if plat == OSX:
    e["SHLINKFLAGS"] = "$LINKFLAGS -bundle"
  if sysconfig.get_config_var("PYTHONFRAMEWORK"):
    a = ' -F' + sysconfig.get_config_var("PYTHONFRAMEWORKPREFIX")
    b = ' -framework ' + sysconfig.get_config_var("PYTHONFRAMEWORK")
    e.Append(LINKFLAGS=a+b)
  else:
    if plat == WINDOWS:
      e.Append(LIBPATH=[sysconfig.PREFIX+'\\libs'])
      e.Append(LIBS=["python%s" % sysconfig.get_python_version().replace(".", "")])
    else:
      e.Append(LINKFLAGS=" %s" % sysconfig.get_config_var("LINKFORSHARED"))
      e.Append(LIBS=["python%s" % sysconfig.get_python_version()])

def AsRubyExtension(e):
  rb_conf = Template("ruby -e \"require 'rbconfig'; print Config::CONFIG['$flag']\"")
  e.Append(CPPPATH=[os.popen(rb_conf.substitute(flag='archdir')).read()])
  e.Append(LIBPATH=[os.popen(rb_conf.substitute(flag='libdir')).read()])
  e.Append(LIBS=[os.popen(rb_conf.substitute(flag='RUBY_SO_NAME')).read()])
  e['SHLIBPREFIX'] = ''
  e['SHLIBSUFFIX'] = '.' + os.popen(rb_conf.substitute(flag='DLEXT')).read()
  if plat == OSX:
    e['SHLINKFLAGS'] = '$LINKFLAGS -bundle'

def AsLuaExtension(e):
  LinkLua(e)
  e['SHLIBPREFIX'] = ''
  e['SHLIBSUFFIX'] = '.so' # On all platforms
  if plat == OSX:
    e['SHLINKFLAGS'] = '$LINKFLAGS -bundle'

def AsDynamicModule(e):
  e["SHLIBPREFIX"] = ""
  if str(Platform()) == "darwin":
    e["SHLIBSUFFIX"] = ".bundle"
    e["SHLINKFLAGS"] = "$LINKFLAGS -bundle"
  elif str(Platform()) == "win32":
    e["SHLIBSUFFIX"] = ".dll"
  else:
    e["SHLIBSUFFIX"] = ".so"

def SetImportLib(e, path):
  if plat == WINDOWS:
    e["IMPLIB"] = path
    e.Append(LINKFLAGS = e["IMPLIBFLAGS"])
    e["no_import_lib"] = 1

def CleanWindows(e, t):
  if plat == WINDOWS:
    e.Clean(t, glob.glob("*.manifest"))
    e.Clean(t, glob.glob("*.pdb"))
    e.Clean(t, glob.glob("*.idb"))
    e.Clean(t, glob.glob("*.ilk"))

def NoMSVCShit(e, t, exp=False, lib=False, extra=[]):
  if plat == WINDOWS:
    bn = str(t[0])
    manifest = bn + ".manifest"
    # if exist somefile somecommand
    e.AddPostAction(t, "if exist \"%s\" del /Q \"%s\"" % (manifest, manifest))
    if lib:
      libfile = os.path.splitext(bn)[0] + ".lib"
      e.AddPostAction(t, "if exist \"%s\" del /Q \"%s\"" % (libfile, libfile))
    if lib or exp:
      expfile = os.path.splitext(bn)[0] + ".exp"
      e.AddPostAction(t, "if exist \"%s\" del /Q \"%s\"" % (expfile, expfile))
    for ef in extra:
      e.AddPostAction(t, "if exist \"%s\" del /Q \"%s\"" % (ef, ef))

env = MakeBaseEnvironment()

libenv = env.Clone()
libenv.Append(CPPPATH=["include"])
libenv.Append(CPPDEFINES=["LWC_EXPORTS", "LWC_MEMTRACK"])
if str(Platform()) == "posix":
  libenv.Append(LIBS=["dl"])
lwc = libenv.SharedLibrary("lib/lwc", glob.glob("src/lib/*.cpp"))
Alias("lwc", lwc)
NoMSVCShit(libenv, lwc, exp=True)
CleanWindows(libenv, lwc)

pylibenv = env.Clone()
pylibenv.Append(CPPPATH=["include"])
pylibenv.Append(CPPDEFINES=["LWCPY_EXPORTS", "LWC_MEMTRACK"])
pylibenv.Append(LIBPATH=["lib"])
pylibenv.Append(LIBS=["lwc"])
lwcpylib = pylibenv.SharedLibrary("lib/lwcpy", glob.glob("src/pylib/*.cpp"))
LinkPython(pylibenv)
Depends(lwcpylib, lwc)
Alias("lwcpylib", lwcpylib)
NoMSVCShit(pylibenv, lwcpylib, exp=True)
CleanWindows(pylibenv, lwcpylib)

rblibenv = env.Clone()
rblibenv.Append(CPPPATH=["include"])
rblibenv.Append(CPPDEFINES=["LWCRB_EXPORTS", "LWC_MEMTRACK"])
rblibenv.Append(LIBPATH=["lib"])
rblibenv.Append(LIBS=["lwc"])
lwcrblib = rblibenv.SharedLibrary("lib/lwcrb", glob.glob("src/rblib/*.cpp"))
LinkRuby(rblibenv)
Depends(lwcrblib, lwc)
Alias("lwcrblib", lwcrblib)
NoMSVCShit(rblibenv, lwcrblib, exp=True)
CleanWindows(rblibenv, lwcrblib)

lualibenv = env.Clone()
lualibenv.Append(CPPPATH=["include"])
lualibenv.Append(CPPDEFINES=["LWCLUA_EXPORTS", "LWC_MEMTRACK"])
lualibenv.Append(LIBPATH=["lib"])
lualibenv.Append(LIBS=["lwc"])
lwclualib = lualibenv.SharedLibrary("lib/lwclua", glob.glob("src/lualib/*.cpp"))
LinkLua(lualibenv)
Depends(lwclualib, lwc)
Alias("lwclualib", lwclualib)
NoMSVCShit(lualibenv, lwclualib, exp=True)
CleanWindows(lualibenv, lwclualib)

cloaderenv = env.Clone()
cloaderenv.Append(CPPPATH=["include"])
cloaderenv.Append(CPPDEFINES=["LWC_MEMTRACK"])
cloaderenv.Append(LIBPATH=["lib"])
cloaderenv.Append(LIBS=["lwc"])
AsDynamicModule(cloaderenv)
cloader = cloaderenv.SharedLibrary("components/loaders/cloader", ["src/loaders/cloader.cpp"])
Depends(cloader, lwc)
Alias("cloader", cloader)
NoMSVCShit(cloaderenv, cloader, lib=True)
CleanWindows(cloaderenv, cloader)

pyloaderenv = env.Clone()
pyloaderenv.Append(CPPPATH=["include"])
pyloaderenv.Append(CPPDEFINES=["LWC_MEMTRACK"])
pyloaderenv.Append(LIBPATH=["lib"])
pyloaderenv.Append(LIBS=["lwc", "lwcpy"])
AsDynamicModule(pyloaderenv)
LinkPython(pyloaderenv)
pyloader = pyloaderenv.SharedLibrary("components/loaders/pyloader", ["src/loaders/pyloader.cpp"])
Depends(pyloader, lwc)
Depends(pyloader, lwcpylib)
Alias("pyloader", pyloader)
NoMSVCShit(pyloaderenv, pyloader, lib=True)
CleanWindows(pyloaderenv, pyloader)

rbloaderenv = env.Clone()
rbloaderenv.Append(CPPPATH=["include"])
rbloaderenv.Append(CPPDEFINES=["LWC_MEMTRACK"])
rbloaderenv.Append(LIBPATH=["lib"])
rbloaderenv.Append(LIBS=["lwc", "lwcrb"])
AsDynamicModule(rbloaderenv)
LinkRuby(rbloaderenv)
rbloader = rbloaderenv.SharedLibrary("components/loaders/rbloader", ["src/loaders/rbloader.cpp"])
Depends(rbloader, lwc)
Depends(rbloader, lwcrblib)
Alias("rbloader", rbloader)
NoMSVCShit(rbloaderenv, rbloader, lib=True)
CleanWindows(rbloaderenv, rbloader)

lualoaderenv = env.Clone()
lualoaderenv.Append(CPPPATH=["include"])
lualoaderenv.Append(CPPDEFINES=["LWC_MEMTRACK"])
lualoaderenv.Append(LIBPATH=["lib"])
lualoaderenv.Append(LIBS=["lwc", "lwclua"])
AsDynamicModule(lualoaderenv)
LinkLua(lualoaderenv, linkstatic=True)
lualoader = lualoaderenv.SharedLibrary("components/loaders/lualoader", ["src/loaders/lualoader.cpp"])
Depends(lualoader, lwc)
Depends(lualoader, lwclualib)
Alias("lualoader", lualoader)
NoMSVCShit(lualoaderenv, lualoader, lib=True)
CleanWindows(lualoaderenv, lualoader)

cmodenv = env.Clone()
cmodenv.Append(CPPPATH=["include"])
cmodenv.Append(CPPDEFINES=["LWC_MEMTRACK"])
cmodenv.Append(LIBPATH=["lib"])
cmodenv.Append(LIBS=["lwc"])
AsDynamicModule(cmodenv)
cmod = cmodenv.SharedLibrary("components/modules/cmod", ["src/modules/box.cpp"])
Depends(cmod, lwc)
Alias("cmod", cmod)
NoMSVCShit(cmodenv, cmod, lib=True)
CleanWindows(cmodenv, cmod)

pymodenv = env.Clone()
pymodenv.Append(CPPPATH=["include"])
pymodenv.Append(CPPDEFINES=["LWC_MEMTRACK"])
pymodenv.Append(LIBPATH=["lib"])
pymodenv.Append(LIBS=["lwc", "lwcpy"])
AsPythonExtension(pymodenv)
lwcpymod = pymodenv.SharedLibrary("python/lwcpy", ["src/python/main.cpp"])
Depends(lwcpymod, lwc)
Depends(lwcpymod, lwcpylib)
Alias("lwcpymod", lwcpymod)
NoMSVCShit(pymodenv, lwcpymod, lib=True)
CleanWindows(pymodenv, lwcpymod)

rbmodenv = env.Clone()
rbmodenv.Append(CPPPATH=["include"])
rbmodenv.Append(CPPDEFINES=["LWC_MEMTRACK"])
rbmodenv.Append(LIBPATH=["lib"])
rbmodenv.Append(LIBS=["lwc", "lwcrb"])
AsRubyExtension(rbmodenv)
lwcrbmod = rbmodenv.SharedLibrary("ruby/RLWC", ["src/ruby/main.cpp"])
Depends(lwcrbmod, lwc)
Depends(lwcrbmod, lwcrblib)
Alias("lwcrbmod", lwcrbmod)
NoMSVCShit(rbmodenv, lwcrbmod, lib=True)
CleanWindows(rbmodenv, lwcrbmod)

luamodenv = env.Clone()
luamodenv.Append(CPPPATH=["include"])
luamodenv.Append(CPPDEFINES=["LWC_MEMTRACK"])
luamodenv.Append(LIBPATH=["lib"])
luamodenv.Append(LIBS=["lwc", "lwclua"])
AsLuaExtension(luamodenv)
lwcluamod = luamodenv.SharedLibrary("lua/llwc", ["src/lua/main.cpp"])
Depends(lwcluamod, lwc)
Depends(lwcluamod, lwclualib)
Alias("lwcluamod", lwcluamod)
NoMSVCShit(luamodenv, lwcluamod, lib=True)
CleanWindows(luamodenv, lwcluamod)

testenv = env.Clone()
testenv.Append(CPPPATH=["include"])
testenv.Append(CPPDEFINES=["LWC_MEMTRACK"])
testenv.Append(LIBPATH=["lib"])
testenv.Append(LIBS=["lwc"])
test = testenv.Program("test", ["src/test/test.cpp"])
Depends(test, lwc)
Depends(test, cloader)
Depends(test, cmod)
Alias("test", test)
NoMSVCShit(testenv, test)
CleanWindows(testenv, test)






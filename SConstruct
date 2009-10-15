import os, sys, glob, platform
from string import Template

arch      = "x86" if platform.architecture()[0] == '32bit' else "x64"
debug     = int(ARGUMENTS.get("debug", "0"))
memtrack  = int(ARGUMENTS.get("memtrack", "0"))
luaprefix = ARGUMENTS.get("luaprefix", None)

variant = "Debug" if debug else "Release"
variant += " %s build (override using debug=1|0)" % arch

print("LUA prefix: \"%s\" (override using luaprefix=<prefix>" % luaprefix)
print(variant)
print("Memory tracker: ON" if memtrack else "Memory tracker: OFF")

class Env(Environment):
  
  UNKNOWN = -1
  WINDOWS = 0
  OSX     = 1
  LINUX   = 2
  
  def __init__(self, **kwargs):
    if str(Platform()) in ["win32", "cygwin"]:
      self.plat = Env.WINDOWS
      self.luaprefix = os.path.join(os.getcwd(), "luawin")
    elif str(Platform()) == "darwin":
      self.plat = Env.OSX
      self.luaprefix = "/usr/local"
    else:
      self.plat = Env.LINUX
      self.luaprefix = "/usr"
    
    self.debug_build = 0
    
    if self.plat == Env.WINDOWS:
      self.msvs_version = ARGUMENTS.get("MSVS_VERSION", "8.0")
      kwargs["MSVS_VERSION"] = self.msvs_version
    
    Environment.__init__(self, **kwargs)
    
    if "debug" in kwargs:
      self.debug_build = kwargs["debug"]
    
    if "arch" in kwargs and kwargs["arch"]:
      self.arch = kwargs["arch"]
    else:
      self.arch = "x86" if platform.architecture()[0] == '32bit' else "x64"
    
    if "luaprefix" in kwargs and kwargs["luaprefix"]:
      self.luaprefix = kwargs["luaprefix"]
    
    if self.plat == Env.WINDOWS:
      self.InitWindows()
    else:
      self.InitAnyOtherDecentSystem()
  
  def InitWindows(self):
    self["IMPLIBFLAGS"] = ' -implib:$IMPLIB$LIBSUFFIX'
    self["NOIMPLIBFLAGS"] = ' -implib:$OBJDIR/dummy.lib'
    self.Append(CCFLAGS=" -GR -EHsc -FD")
    self.Append(CPPDEFINES=["_WIN32", "WIN32", "_CONSOLE", "_MBCS"])
    
    if float(self.msvs_version) > 7.1:
      self['LINKCOM'] = [self['LINKCOM'], 'mt.exe -nologo -manifest ${TARGET}.manifest -outputresource:$TARGET;1']
      self['SHLINKCOM'] = [self['SHLINKCOM'], 'mt.exe -nologo -manifest ${TARGET}.manifest -outputresource:$TARGET;2']
      self.Append(CPPDEFINES='_CRT_SECURE_NO_DEPRECATE')
    
    if "INCLUDE" in os.environ:
      self.Append(CPPPATH = os.environ["INCLUDE"].split(";"))
    
    if "LIB" in os.environ:
      self.Append(LIBPATH = os.environ["LIB"].split(";"))
    
    if self.debug_build:
      self.Append(CPPDEFINES=["_DEBUG"])
      self.Append(CCFLAGS=" -MDd -Gm -Zi")
      self.Append(LINKFLAGS=" -debug -incremental")
    else:
      self.Append(CPPDEFINES=["NDEBUG"])
      self.Append(CCFLAGS=" -O2 -MD")
      self.Append(LINKFLAGS=" -release -opt:ref -opt:icf -incremental:no")
  
  def InitAnyOtherDecentSystem(self):
    self.Append(CCFLAGS=" -pipe -fno-common -Wno-ctor-dtor-privacy")
    
    if self.plat == Env.OSX:
      self.Append(CCFLAGS=" -flat_namespace")
    
    if self.debug_build:
      self.Append(CPPDEFINES=["_DEBUG"])
      self.Append(CCFLAGS=" -O0 -g -ggdb")
    else:
      self.Append(CPPDEFINES=["NDEBUG"])
      self.Append(CCFLAGS=" -O2")
      if self.plat == Env.OSX:
        self.Append(LINKFLAGS=" -Wl,-dead_strip")
  
  def Clone(e):
    c = Environment.Clone(e)
    c.__class__ = Env
    c.plat = e.plat
    c.arch = e.arch
    if e.plat == Env.WINDOWS:
      c.msvs_version = e.msvs_version
    c.debug_build = e.debug_build
    c.luaprefix = e.luaprefix
    return c
  
  def AsDynamicModule(e, ext=None):
    e["SHLIBPREFIX"] = ""
    if e.plat == Env.OSX:
      e["SHLIBSUFFIX"] = ".bundle" if ext == None else ext
      e["SHLINKFLAGS"] = "$LINKFLAGS -bundle"
    elif e.plat == Env.WINDOWS:
      e["SHLIBSUFFIX"] = ".dll" if ext == None else ext
    else:
      e["SHLIBSUFFIX"] = ".so" if ext == None else ext
  
  def LinkPython(e):
    from distutils import sysconfig
    e.Append(CPPPATH=[sysconfig.get_python_inc()])
    if sysconfig.get_config_var("PYTHONFRAMEWORK"):
      a = ' -F' + sysconfig.get_config_var("PYTHONFRAMEWORKPREFIX")
      b = ' -framework ' + sysconfig.get_config_var("PYTHONFRAMEWORK")
      e.Append(LINKFLAGS=a+b)
    else:
      if e.plat == Env.WINDOWS:
        e.Append(LIBPATH=[sysconfig.PREFIX+'\\libs'])
        e.Append(LIBS=["python%s" % sysconfig.get_python_version().replace(".", "")])
      else:
        e.Append(LINKFLAGS=" %s" % sysconfig.get_config_var("LINKFORSHARED"))
        e.Append(LIBS=["python%s" % sysconfig.get_python_version()])
  
  def AsPythonExtension(e):
    from distutils import sysconfig
    e.LinkPython()
    e.AsDynamicModule(ext=sysconfig.get_config_var("SO"))
  
  def LinkRuby(e):
    rb_conf = Template("ruby -e \"require 'rbconfig'; print Config::CONFIG['$flag']\"")
    e.Append(CPPPATH=[os.popen(rb_conf.substitute(flag='archdir')).read()])
    e.Append(LIBPATH=[os.popen(rb_conf.substitute(flag='libdir')).read()])
    e.Append(LIBS=[os.popen(rb_conf.substitute(flag='RUBY_SO_NAME')).read()])
  
  def AsRubyExtension(e):
    e.LinkRuby()
    rb_conf = Template("ruby -e \"require 'rbconfig'; print Config::CONFIG['$flag']\"")
    rmodext = '.' + os.popen(rb_conf.substitute(flag='DLEXT')).read()
    e.AsDynamicModule(ext=rmodext)
  
  def LinkLua(e):
    if e.plat == Env.WINDOWS:
      e.Append(CPPDEFINES=["LUA_BUILD_AS_DLL"])
      e.Append(CPPPATH=[e.luaprefix+"/include"])
      e.Append(LIBPATH=[e.luaprefix+"/lib/"+e.arch]) # if 64 bit build?
      e.Append(LIBS = ["lua51"])
    else:
      e.Append(CPPPATH=[e.luaprefix+"/include"])
      e.Append(LIBPATH=[e.luaprefix+"/lib"])
      e.Append(LIBS="lua")
  
  def AsLuaExtension(e):
    e.LinkLua()
    e.AsDynamicModule(ext=".so")
  
  def SetImportLib(e, path):
    if e.plat == Env.WINDOWS:
      e["IMPLIB"] = path
      e.Append(LINKFLAGS = e["IMPLIBFLAGS"])
      e["no_import_lib"] = 1
  
  def CleanWindows(e, t):
    if e.plat == Env.WINDOWS:
      e.Clean(t, glob.glob("*.manifest"))
      e.Clean(t, glob.glob("*.pdb"))
      e.Clean(t, glob.glob("*.idb"))
      e.Clean(t, glob.glob("*.ilk"))
  
  def NoMSVCShit(e, t, exp=False, lib=False, extra=[]):
    if e.plat == Env.WINDOWS:
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
  
  def SharedLibrary2(e, tname, objs):
    tgt = e.SharedLibrary(tname, objs)
    e.NoMSVCShit(tgt, exp=True)
    e.CleanWindows(tgt)
    return tgt
  
  def Program2(e, tname, objs):
    tgt = e.Program(tname, objs)
    e.NoMSVCShit(tgt)
    e.CleanWindows(tgt)
    return tgt
  
  def CModule(e, tname, objs):
    e.AsDynamicModule()
    tgt = e.SharedLibrary(tname, objs)
    e.NoMSVCShit(tgt, lib=True)
    e.CleanWindows(tgt)
    return tgt
  
  def PythonModule(e, tname, objs):
    e.AsPythonExtension()
    tgt = e.SharedLibrary(tname, objs)
    e.NoMSVCShit(tgt, lib=True)
    e.CleanWindows(tgt)
    return tgt
  
  def RubyModule(e, tname, objs):
    e.AsRubyExtension()
    tgt = e.SharedLibrary(tname, objs)
    e.NoMSVCShit(tgt, lib=True)
    e.CleanWindows(tgt)
    return tgt
  
  def LuaModule(e, tname, objs):
    e.AsLuaExtension()
    tgt = e.SharedLibrary(tname, objs)
    e.NoMSVCShit(tgt, lib=True)
    e.CleanWindows(tgt)
    return tgt


env = Env(debug=debug, arch=arch, luaprefix=luaprefix)
env.Append(CPPPATH=["include"])
env.Append(LIBPATH=["lib"])
if memtrack:
  env.Append(CPPDEFINES=["LWC_MEMTRACK"])

libenv = env.Clone()
libenv.Append(CPPDEFINES=["LWC_EXPORTS"])
if str(Platform()) == "posix":
  libenv.Append(LIBS=["dl"])
lwc = libenv.SharedLibrary2("lib/lwc", glob.glob("src/lib/*.cpp"))
Alias("lwc", lwc)

pylibenv = env.Clone()
pylibenv.Append(CPPDEFINES=["LWCPY_EXPORTS"])
pylibenv.Append(LIBS=["lwc"])
pylibenv.LinkPython()
lwcpylib = pylibenv.SharedLibrary2("lib/lwcpy", glob.glob("src/pylib/*.cpp"))
Depends(lwcpylib, lwc)
Alias("lwcpylib", lwcpylib)

rblibenv = env.Clone()
rblibenv.Append(CPPDEFINES=["LWCRB_EXPORTS"])
rblibenv.Append(LIBS=["lwc"])
rblibenv.LinkRuby()
lwcrblib = rblibenv.SharedLibrary2("lib/lwcrb", glob.glob("src/rblib/*.cpp"))
Depends(lwcrblib, lwc)
Alias("lwcrblib", lwcrblib)

lualibenv = env.Clone()
lualibenv.Append(CPPDEFINES=["LWCLUA_EXPORTS"])
lualibenv.Append(LIBS=["lwc"])
lualibenv.LinkLua()
lwclualib = lualibenv.SharedLibrary2("lib/lwclua", glob.glob("src/lualib/*.cpp"))
Depends(lwclualib, lwc)
Alias("lwclualib", lwclualib)

cloaderenv = env.Clone()
cloaderenv.Append(LIBS=["lwc"])
cloader = cloaderenv.CModule("components/loaders/cloader", ["src/loaders/cloader.cpp"])
Depends(cloader, lwc)
Alias("cloader", cloader)

pyloaderenv = env.Clone()
pyloaderenv.Append(LIBS=["lwc", "lwcpy"])
pyloaderenv.LinkPython()
pyloader = pyloaderenv.CModule("components/loaders/pyloader", ["src/loaders/pyloader.cpp"])
Depends(pyloader, lwc)
Depends(pyloader, lwcpylib)
Alias("pyloader", pyloader)

rbloaderenv = env.Clone()
rbloaderenv.Append(LIBS=["lwc", "lwcrb"])
rbloaderenv.LinkRuby()
rbloader = rbloaderenv.CModule("components/loaders/rbloader", ["src/loaders/rbloader.cpp"])
Depends(rbloader, lwc)
Depends(rbloader, lwcrblib)
Alias("rbloader", rbloader)

lualoaderenv = env.Clone()
lualoaderenv.Append(LIBS=["lwc", "lwclua"])
lualoaderenv.LinkLua()
lualoader = lualoaderenv.CModule("components/loaders/lualoader", ["src/loaders/lualoader.cpp"])
Depends(lualoader, lwc)
Depends(lualoader, lwclualib)
Alias("lualoader", lualoader)

cmodenv = env.Clone()
cmodenv.Append(LIBS=["lwc"])
cmod = cmodenv.CModule("components/modules/cmod", ["src/modules/box.cpp"])
Depends(cmod, lwc)
Alias("cmod", cmod)

pymodenv = env.Clone()
pymodenv.Append(LIBS=["lwc", "lwcpy"])
lwcpymod = pymodenv.PythonModule("python/lwcpy", ["src/python/main.cpp"])
Depends(lwcpymod, lwc)
Depends(lwcpymod, lwcpylib)
Alias("lwcpymod", lwcpymod)

rbmodenv = env.Clone()
rbmodenv.Append(LIBS=["lwc", "lwcrb"])
lwcrbmod = rbmodenv.RubyModule("ruby/RLWC", ["src/ruby/main.cpp"])
Depends(lwcrbmod, lwc)
Depends(lwcrbmod, lwcrblib)
Alias("lwcrbmod", lwcrbmod)

luamodenv = env.Clone()
luamodenv.Append(LIBS=["lwc", "lwclua"])
lwcluamod = luamodenv.LuaModule("lua/llwc", ["src/lua/main.cpp"])
Depends(lwcluamod, lwc)
Depends(lwcluamod, lwclualib)
Alias("lwcluamod", lwcluamod)

testenv = env.Clone()
testenv.Append(LIBS=["lwc"])
test = testenv.Program2("test", ["src/test/test.cpp"])
Depends(test, lwc)
Depends(test, cloader)
Depends(test, cmod)
Alias("test", test)





import os
import glob
import excons
import excons.tools
from excons.tools import lua
from excons.tools import ruby
from excons.tools import python
from excons.tools import dl

SConscript("gcore/SConstruct")

prjs = [
  { "name"    : "lwc",
    "type"    : "sharedlib",
    "srcs"    : glob.glob("src/lib/*.cpp"),
    "incdirs" : ["gcore/include"],
    "defs"    : ["LWC_EXPORTS"],
    "libs"    : ["gcore"],
    "custom"  : [dl.Require],
    "install" : {"include/lwc": glob.glob("include/lwc/*.h")}
  },
  { "name"    : "lwcpy",
    "type"    : "sharedlib",
    "srcs"    : glob.glob("src/pylib/*.cpp"),
    "defs"    : ["LWCPY_EXPORTS"],
    "incdirs" : ["gcore/include"],
    "libs"    : ["lwc", "gcore"],
    "custom"  : [python.Require],
    "deps"    : ["lwc"],
    "install" : {"include/lwc/python": glob.glob("include/lwc/python/*.h")}
  },
  { "name"    : "lwcrb",
    "type"    : "sharedlib",
    "srcs"    : glob.glob("src/rblib/*.cpp"),
    "defs"    : ["LWCRB_EXPORTS"],
    "incdirs" : ["gcore/include"],
    "libs"    : ["lwc", "gcore"],
    "custom"  : [ruby.Require],
    "deps"    : ["lwc"],
    "install" : {"include/lwc/ruby": glob.glob("include/lwc/ruby/*.h")}
  },
  { "name"    : "lwclua",
    "type"    : "sharedlib",
    "srcs"    : glob.glob("src/lualib/*.cpp"),
    "defs"    : ["LWCLUA_EXPORTS"],
    "incdirs" : ["gcore/include"],
    "libs"    : ["lwc", "gcore"],
    "custom"  : [lua.Require],
    "deps"    : ["lwc"],
    "install" : {"include/lwc/lua": glob.glob("include/lwc/lua/*.h")}
  },
  # Language bindings
  { "name"    : python.ModulePrefix()+"lwcpy",
    "alias"   : "pymod",
    "type"    : "dynamicmodule",
    "srcs"    : ["src/python/main.cpp"],
    "incdirs" : ["gcore/include"],
    "libs"    : ["lwc", "lwcpy", "gcore"],
    "custom"  : [python.Require],
    "deps"    : ["lwc", "lwcpy"],
    "ext"     : python.ModuleExtension()
  },
  { "name"    : ruby.ModulePrefix()+"RLWC",
    "alias"   : "rbmod",
    "type"    : "dynamicmodule",
    "srcs"    : ["src/ruby/main.cpp"],
    "incdirs" : ["gcore/include"],
    "libs"    : ["lwc", "lwcrb", "gcore"],
    "custom"  : [ruby.Require],
    "deps"    : ["lwc", "lwcrb"],
    "ext"     : ruby.ModuleExtension()
  },
  { "name"    : lua.ModulePrefix()+"llwc",
    "alias"   : "luamod",
    "type"    : "dynamicmodule",
    "srcs"    : ["src/lua/main.cpp"],
    "incdirs" : ["gcore/include"],
    "libs"    : ["lwc", "lwclua", "gcore"],
    "custom"  : [lua.Require],
    "deps"    : ["lwc", "lwclua"],
    "ext"     : lua.ModuleExtension()
  },
  # LWC Loaders
  { "name"    : "components/loaders/cloader",
    "alias"   : "cloader",
    "type"    : "dynamicmodule",
    "srcs"    : ["src/loaders/cloader.cpp"],
    "incdirs" : ["gcore/include"],
    "libs"    : ["lwc", "gcore"],
    "deps"    : ["lwc"]
  },
  { "name"    : "components/loaders/pyloader",
    "alias"   : "pyloader",
    "type"    : "dynamicmodule",
    "srcs"    : ["src/loaders/pyloader.cpp"],
    "incdirs" : ["gcore/include"],
    "libs"    : ["lwc", "lwcpy", "gcore"],
    "deps"    : ["lwc", "lwcpy"],
    "custom"  : [python.Require]
  },
  { "name"    : "components/loaders/rbloader",
    "alias"   : "rbloader",
    "type"    : "dynamicmodule",
    "srcs"    : ["src/loaders/rbloader.cpp"],
    "incdirs" : ["gcore/include"],
    "libs"    : ["lwc", "lwcrb", "gcore"],
    "deps"    : ["lwc", "lwcrb"],
    "custom"  : [ruby.Require]
  },
  { "name"    : "components/loaders/lualoader",
    "alias"   : "lualoader",
    "type"    : "dynamicmodule",
    "srcs"    : ["src/loaders/lualoader.cpp"],
    "incdirs" : ["gcore/include"],
    "libs"    : ["lwc", "lwclua", "gcore"],
    "deps"    : ["lwc", "lwclua"],
    "custom"  : [lua.Require]
  },
  # Test
  { "name"    : "components/modules/cmod",
    "alias"   : "modules",
    "type"    : "dynamicmodule",
    "srcs"    : ["src/modules/box.cpp"],
    "incdirs" : ["gcore/include"],
    "libs"    : ["lwc", "gcore"],
    "deps"    : ["lwc"],
    "install" : {"components/modules": ["src/modules/dict.lua",
                                        "src/modules/objlist.py",
                                        "src/modules/point.rb"]}
  },
  { "name"    : "test",
    "type"    : "program",
    "srcs"    : ["src/test/test.cpp"],
    "incdirs" : ["gcore/include"],
    "libs"    : ["lwc", "gcore"],
    "deps"    : ["lwc", "components/modules/cmod"],
    "install" : {"": ["src/test/test.lua",
                      "src/test/test.py",
                      "src/test/test.rb",
                      "src/test/test2.rb"]}
  }
]

memtrack   = int(ARGUMENTS.get("memtrack", "0"))

env = excons.MakeBaseEnv()
if memtrack == 1:
  env.Append(CPPDEFINES=["LWC_MEMTRACK"])

excons.DeclareTargets(env, prjs)

import glob
import excons
import excons.tools
from excons.tools import lua
from excons.tools import ruby
from excons.tools import python
from excons.tools import dl

prjs = [
  { "name"    : "lwc",
    "type"    : "sharedlib",
    "srcs"    : glob.glob("src/lib/*.cpp"),
    "defs"    : ["LWC_EXPORTS"],
    "custom"  : [dl.Require]
  },
  { "name"    : "lwcpy",
    "type"    : "sharedlib",
    "srcs"    : glob.glob("src/pylib/*.cpp"),
    "defs"    : ["LWCPY_EXPORTS"],
    "libs"    : ["lwc"],
    "custom"  : [python.Require],
    "deps"    : ["lwc"]
  },
  { "name"    : "lwcrb",
    "type"    : "sharedlib",
    "srcs"    : glob.glob("src/rblib/*.cpp"),
    "defs"    : ["LWCRB_EXPORTS"],
    "libs"    : ["lwc"],
    "custom"  : [ruby.Require],
    "deps"    : ["lwc"]
  },
  { "name"    : "lwclua",
    "type"    : "sharedlib",
    "srcs"    : glob.glob("src/lualib/*.cpp"),
    "defs"    : ["LWCLUA_EXPORTS"],
    "libs"    : ["lwc"],
    "custom"  : [lua.Require],
    "deps"    : ["lwc"]
  },
  # Language bindings
  { "name"    : "python/lwcpy",
    "alias"   : "pymod",
    "type"    : "dynamicmodule",
    "srcs"    : ["src/python/main.cpp"],
    "libs"    : ["lwc", "lwcpy"],
    "custom"  : [python.Require],
    "deps"    : ["lwc", "lwcpy"],
    "ext"     : python.ModuleExtension()
  },
  { "name"    : "ruby/RLWC",
    "alias"   : "rbmod",
    "type"    : "dynamicmodule",
    "srcs"    : ["src/ruby/main.cpp"],
    "libs"    : ["lwc", "lwcrb"],
    "custom"  : [ruby.Require],
    "deps"    : ["lwc", "lwcrb"],
    "ext"     : ruby.ModuleExtension()
  },
  { "name"    : "lua/llwc",
    "alias"   : "luamod",
    "type"    : "dynamicmodule",
    "srcs"    : ["src/lua/main.cpp"],
    "libs"    : ["lwc", "lwclua"],
    "custom"  : [lua.Require],
    "deps"    : ["lwc", "lwclua"],
    "ext"     : lua.ModuleExtension()
  },
  # LWC Loaders
  { "name"    : "components/loaders/cloader",
    "alias"   : "cloader",
    "type"    : "dynamicmodule",
    "srcs"    : ["src/loaders/cloader.cpp"],
    "libs"    : ["lwc"],
    "deps"    : ["lwc"]
  },
  { "name"    : "components/loaders/pyloader",
    "alias"   : "pyloader",
    "type"    : "dynamicmodule",
    "srcs"    : ["src/loaders/pyloader.cpp"],
    "libs"    : ["lwc", "lwcpy"],
    "deps"    : ["lwc", "lwcpy"],
    "custom"  : [python.Require]
  },
  { "name"    : "components/loaders/rbloader",
    "alias"   : "rbloader",
    "type"    : "dynamicmodule",
    "srcs"    : ["src/loaders/rbloader.cpp"],
    "libs"    : ["lwc", "lwcrb"],
    "deps"    : ["lwc", "lwcrb"],
    "custom"  : [ruby.Require]
  },
  { "name"    : "components/loaders/lualoader",
    "alias"   : "lualoader",
    "type"    : "dynamicmodule",
    "srcs"    : ["src/loaders/lualoader.cpp"],
    "libs"    : ["lwc", "lwclua"],
    "deps"    : ["lwc", "lwclua"],
    "custom"  : [lua.Require]
  },
  # Test
  { "name"    : "components/modules/cmod",
    "alias"   : "cmod",
    "type"    : "dynamicmodule",
    "srcs"    : ["src/modules/box.cpp"],
    "libs"    : ["lwc"],
    "deps"    : ["lwc"]
  },
  { "name"    : "test",
    "type"    : "program",
    "srcs"    : ["src/test/test.cpp"],
    "libs"    : ["lwc"],
    "deps"    : ["lwc", "components/modules/cmod"]
  }
]

memtrack   = int(ARGUMENTS.get("memtrack", "0"))

env = excons.MakeBaseEnv()
if memtrack == 1:
  env.Append(CPPDEFINES=["LWC_MEMTRACK"])
env.Append(CPPPATH=["include"])
env.Append(LIBPATH=["lib"])

excons.DeclareTargets(env, prjs)

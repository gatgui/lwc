export DYLD_LIBRARY_PATH=`pwd`/$1/$2/lib:$DYLD_LIBRARY_PATH
export PYTHONPATH=`pwd`/$1/$2/lib/python:$PYTHONPATH
export RUBYLIB=`pwd`/$1/$2/lib/ruby:$RUBYLIB
export LUA_CPATH=`pwd`$1/$2/lib/lua/?.so;$LUA_CPATH
export LWC_LOADER_PATH=`pwd`/$1/$2/components/loaders
export LWC_MODULE_PATH=`pwd`/$1/$2/components/modules

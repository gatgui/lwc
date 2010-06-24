-- Copyright (C) 2009  Gaetan Guidet
-- 
-- This file is part of lwc.
-- 
-- lwc is free software; you can redistribute it and/or modify it
-- under the terms of the GNU Lesser General Public License as published by
-- the Free Software Foundation; either version 2.1 of the License, or (at
-- your option) any later version.
-- 
-- lwc is distributed in the hope that it will be useful, but
-- WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
-- Lesser General Public License for more details.
-- 
-- You should have received a copy of the GNU Lesser General Public
-- License along with this library; if not, write to the Free Software
-- Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
-- USA.

require "llwc"

module(..., package.seeall)

-- Dict class
-- "lwcobj" member of instance is automatically set

local Dict          = {}
Dict.Methods        = {}
Dict.Methods.keys   = {{llwc.AD_OUT, llwc.AT_STRING_ARRAY, 1}, {llwc.AD_OUT, llwc.AT_INT}}
Dict.Methods.values = {{llwc.AD_OUT, llwc.AT_STRING_ARRAY, 1}, {llwc.AD_OUT, llwc.AT_INT}}
Dict.Methods.size   = {{llwc.AD_OUT, llwc.AT_INT}}
Dict.Methods.get    = {{llwc.AD_IN, llwc.AT_STRING}, {llwc.AD_OUT, llwc.AT_STRING}}
Dict.Methods.set    = {{llwc.AD_IN, llwc.AT_STRING}, {llwc.AD_IN, llwc.AT_STRING}}

Dict.new = function ()
  self = {}
  -- place holder for llwc native object, you don't have to set it, but it makes things more explicit
  self.lwcobj = nil
  self.dict = {}
  setmetatable(self, Dict)
  return self
end

Dict.keys = function (self)
  kl = {}
  i = 1
  for k, v in pairs(self.dict) do
    kl[i] = k
    i = i + 1
  end
  return kl
end

Dict.values = function (self)
  vl = {}
  i = 1
  for k, v in pairs(self.dict) do
    vl[i] = v
    i = i + 1
  end
  return vl
end

Dict.set = function (self, key, val)
  self.dict[key] = val
end

Dict.get = function (self, key)
  return self.dict[key]
end

Dict.size = function (self)
  n = 0
  for k, v in pairs(self.dict) do
    n = n + 1
  end
  return n
end

-- Type inheritance in a single component

local Dict2         = {}
Dict2.Methods       = {}
Dict2.Methods.clear = {}
setmetatable(Dict2, {__index = Dict})

Dict2.new = function ()
  self = Dict.new()
  setmetatable(self, Dict2)
  return self
end

Dict2.clear = function ()
  self.dict = {}
end

-- Module entry point

function LWC_ModuleGetTypeCount()
  return 2
end

function LWC_ModuleGetTypeName(idx)
  if idx == 1 then
    return "luatest.Dict"
  elseif idx == 2 then
    return "luatest.Dict2"
  else
    return nil
  end
end

function LWC_ModuleGetTypeClass(idx)
  if idx == 1 then
    return Dict
  elseif idx == 2 then
    return Dict2
  else
    return nil
  end
end


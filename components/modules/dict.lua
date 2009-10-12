require "llwc"

module(..., package.seeall)

-- Dict class
-- "lwcobj" member of instance is automatically set

local Dict = {}

-- for lwc to do automatic type mapping
Dict.Methods        = {}
Dict.Methods.keys   = {{llwc.AD_OUT, llwc.AT_STRING, false, true, 1}, {llwc.AD_OUT, llwc.AT_ULONG}}
Dict.Methods.values = {{llwc.AD_OUT, llwc.AT_STRING, false, true, 1}, {llwc.AD_OUT, llwc.AT_ULONG}}
Dict.Methods.size   = {{llwc.AD_RETURN, llwc.AT_ULONG}}
Dict.Methods.get    = {{llwc.AD_IN, llwc.AT_STRING}, {llwc.AD_RETURN, llwc.AT_STRING}}
Dict.Methods.set    = {{llwc.AD_IN, llwc.AT_STRING}, {llwc.AD_IN, llwc.AT_STRING}}

-- could this be automatically generated?
Dict.__index = function (self, member)
	val = Dict[member]
	if val == nil then
		val = self.lwcobj[member]
	end
	return val
end

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


-- Module entry point

function LWC_ModuleGetTypeCount()
  return 1
end

function LWC_ModuleGetTypeName(idx)
  if idx == 1 then
    return "luatest.Dict"
  else
    return nil
  end
end

function LWC_ModuleGetTypeClass(idx)
  if idx == 1 then
    return Dict
  else
    return nil
  end
end


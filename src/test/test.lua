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

reg = llwc.Initialize()

reg:addLoaderPath("./components/loaders")
reg:addModulePath("./components/modules")

print(reg:numTypes() .. " types in registry")
for i = 1, reg:numTypes() do
  name = reg:getTypeName(i)
  print("=== " .. name)
  methods = reg:getMethods(name)
  print(methods)
end

print("=== Create a \"test.DoubleBox\" object")
obj = reg:create("test.DoubleBox")

print(obj:getLoaderName())
names = obj:availableMethods()
print(#names .. " methods available")
for i = 1, #names do
  print("  " .. names[i])
end
obj:setWidth(200)
obj:setHeight(100)
obj:setX(10)
obj:setY(15)
print(tostring(obj:getWidth()) .. "x" .. tostring(obj:getHeight()) .. " @ (" .. tostring(obj:getX()) .. ", " .. tostring(obj:getY()) .. ")")

-- a = obj:computeArea()

print("=== Destroy the \"test.DoubleBox\" object")
reg:destroy(obj)

print("=== Create a luatest.Dict2")
obj = reg:create("luatest.Dict2")
if obj == nil then
	print("is nil")
else
	print("  set keys")
	if obj:respondsTo("set") then
		print("  has \"set\" method")
	end
	obj:set("key", "poo")
	obj:set("value", "hello")
	print("  check keys")
	print("  numkeys = " .. tostring(obj:size()))
	print("  " .. obj:get("key"))
	print("  " .. obj:get("value"))
	kl = " "
	vl = " "
	for idx,val in ipairs(obj:keys()) do
	  kl = kl .. " " .. tostring(val)
	end
	for idx,val in ipairs(obj:values()) do
	  vl = vl .. " " .. tostring(val)
	end
	print(kl)
	print(vl)
	print("  clear")
	obj:clear()
	print("  numkeys = " .. tostring(obj:size()))
	reg:destroy(obj)
end


llwc.DeInitialize()



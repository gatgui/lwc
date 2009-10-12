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

print("=== Create a luatest.Dict")
obj = reg:create("luatest.Dict")
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
	reg:destroy(obj)
end


llwc.DeInitialize()



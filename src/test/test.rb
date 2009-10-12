require 'RLWC'

reg = RLWC.Initialize()

reg.addLoaderPath("./components/loaders")
reg.addModulePath("./components/modules")

(0...reg.numTypes).each do |i|
  puts "=== "+reg.getTypeName(i)
  puts reg.getMethods(reg.getTypeName(i))
end

obj = reg.create("test.DoubleBox")
puts "Width = #{obj.getWidth()}"
obj.setWidth(2)
puts "=> #{obj.getWidth()}"

reg.destroy(obj)

puts "Create lua object"
obj = reg.create("luatest.Dict")
puts "size = #{obj.size()}"
obj.set("Jelly", "Fish")
obj.set("Poo", "San")
puts "Check keys"
keys = obj.keys()
puts keys.join(", ")
puts "Check values"
vals = obj.values()
puts vals.join(", ")
reg.destroy(obj)

RLWC.DeInitialize()



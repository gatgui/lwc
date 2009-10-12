require 'RLWC'

reg = RLWC.Initialize()

reg.addLoaderPath("./components/loaders")
reg.addModulePath("./components/modules")

ary = reg.create("pytest.ObjectList")
#puts ary
#puts ary.getMethods()

(0..10).each do |i|
  elt = reg.create("rbtest.Point")
  #puts elt
  #puts elt.getMethods()
  elt.set(i, i)
  ary.push(elt)
end

(0...ary.size()).each do |i|
  elt = ary.at(i)
  puts "Point#{i}: (#{elt.getX()}, #{elt.getY()})"
  reg.destroy(elt)
end

reg.destroy(ary)

RLWC.DeInitialize()



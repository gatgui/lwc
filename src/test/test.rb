=begin

Copyright (C) 2009, 2010  Gaetan Guidet

This file is part of lwc.

lwc is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at
your option) any later version.

lwc is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
USA.

=end

require 'RLWC'

reg = RLWC.Initialize()

reg.addLoaderPath("./components/loaders")
reg.addModulePath("./components/modules")

(0...reg.numTypes).each do |i|
  puts "=== "+reg.getTypeName(i)
  puts reg.getMethods(reg.getTypeName(i))
end

obj = reg.create("test.DoubleBox")
if not obj.nil? then
  puts "Width = #{obj.getWidth()}"
  obj.setWidth(2)
  puts "=> #{obj.getWidth()}"
  obj.set([2, 2], :scale => 3, :normalize => true)
  obj.set([4, 3], :scale => 2)
  puts "=> (#{obj.getX()}, #{obj.getY()})"
  reg.destroy(obj)
end

puts "Create lua object"
obj = reg.create("luatest.Dict")
if not obj.nil? then
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
end

RLWC.DeInitialize()



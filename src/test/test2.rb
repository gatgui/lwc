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

puts "=== Create object list"
ary = reg.create("pytest.ObjectList")
#puts ary
#puts ary.getMethods()

(0..10).each do |i|
  puts "=== Create new point"
  elt = reg.create("rbtest.Point2")
  #puts elt
  #puts elt.getMethods()
  puts "=== Set point value"
  elt.set(i, i)
  puts "=== Add to list"
  ary.push(elt)
end

pt0 = ary.at(1)
pt1 = ary.at(2)
puts "=== Dot product"
puts pt0.dot(pt1)

(0...ary.size()).each do |i|
  puts "=== Get point at #{i}"
  elt = ary.at(i)
  puts "=== => #{elt}"
  puts "Point#{i}: (#{elt.getX()}, #{elt.getY()})"
  puts "=== Destroy point"
  reg.destroy(elt)
end

puts "=== Destroy object list"
reg.destroy(ary)

RLWC.DeInitialize()



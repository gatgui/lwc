=begin

Copyright (C) 2009  Gaetan Guidet

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



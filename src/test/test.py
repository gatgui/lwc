# Copyright (C) 2009, 2010  Gaetan Guidet
#
# This file is part of lwc.
#
# lwc is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 2.1 of the License, or (at
# your option) any later version.
#
# lwc is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
# USA.

import lwcpy

print("### Initialize")
reg = lwcpy.Initialize()

print("### Add loader path")
reg.addLoaderPath("./components/loaders")

print("### Add module path")
reg.addModulePath("./components/modules")

print("### Available types")
for i in xrange(reg.numTypes()):
  print("  %s" % reg.getTypeName(i))

print("### Has type \"pytest.ObjectList2\"?")
if reg.hasType("pytest.ObjectList2"):
  print("true")
  ml = reg.getMethods("pytest.ObjectList2")
  print(ml)
  # This seems to do weird things
  #for n in ml.availableMethods():
  #  print(n)
  print("### Create a pytest.ObjectList2")
  lst = reg.create("pytest.ObjectList2")
  print(lst)
  print("### Create a test.Box")
  box = reg.create("test.Box")
  print(box)
  print("### Add box to list")
  lst.push(box)
  print(lst.size())
  print("### Clear list")
  lst.clear()
  print(lst.size())
  print("### Keyword arg test")
  lst.printInt(10, indent="=> ")
  print("### Destroy box & list")
  reg.destroy(box)
  reg.destroy(lst)
else:
  print("false")

#print("### Methods of test.DoubleBox")
#table = reg.getMethods("test.DoubleBox")
# DOING THAT, there were a strange behavior in python:
#  it was complaining 'lwcpy.Object' had no attribute 'size' [this is the last function
#                                                             called on an object]
#print(table.availableMethods)
#methods = table.availableMethods()
#print(type(methods))
#for n in methods:
#  print("  %s%s" % (n, table.findMethod(n)))

print("### Create object test.DoubleBox")
obj = reg.create("test.DoubleBox")
print(obj)

print("### list attr")
print(dir(obj))

print("### Check available methods")
for n in obj.availableMethods():
  print(n)

print("### getWidth attrib")
print(obj.getWidth)

print("### call getters")
print(obj.getX())
print(obj.getY())
print(obj.getWidth())
print(obj.getHeight())

print("### call setters")
obj.setX(10)
obj.setY(2)
obj.setWidth(300)
obj.setHeight(150)

print("### clone")
obj2 = obj.toBox()

print("### call getters on Box")
print(obj2.getX())
print(obj2.getY())
print(obj2.getWidth())
print(obj2.getHeight())

print("### call Box.set")
try:
  obj2.set([2, 2], scale=3, normalize=True)
  print("%s, %s" % (obj2.getX(), obj2.getY()))
except Exception, e:
  print("*** FAILED: %s" % e)

print("### Destroy objects")
reg.destroy(obj)
reg.destroy(obj2)

"""
print("### test lua object")
obj = reg.create("luatest.Dict")
print("  size = %d" % obj.size())
print("  add keys")
obj.set("poo", "hello")
obj.set("grrr", "goodbye")
print("  check keys")
print(obj.get("poo"))
print(obj.get("grrr"))
print(obj.keys())
print(obj.values())
reg.destroy(obj)
"""

print("### DeInitialize")
lwcpy.DeInitialize()


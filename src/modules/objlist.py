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

class ObjectList(lwcpy.Object):
  
  Methods = {"push"  : ([(lwcpy.AD_IN, lwcpy.AT_OBJECT)],
                        "Append object to list"),
             "at"    : ([(lwcpy.AD_IN, lwcpy.AT_INT),
                         (lwcpy.AD_OUT, lwcpy.AT_OBJECT)],
                        "Get object at given index"),
             "set"   : ([(lwcpy.AD_IN, lwcpy.AT_INT),
                         (lwcpy.AD_IN, lwcpy.AT_OBJECT)],
                        "Replace object at given index"),
             "first" : ([(lwcpy.AD_OUT, lwcpy.AT_OBJECT)],
                        "Get first object in list"),
             "last"  : ([(lwcpy.AD_OUT, lwcpy.AT_OBJECT)],
                        "Get last object in list"),
             "erase" : ([(lwcpy.AD_IN, lwcpy.AT_INT)],
                        "Remove object at given index"),
             "size"  : ([(lwcpy.AD_OUT, lwcpy.AT_INT)],
                        "Get number of object(s) in list"),
             "pop"   : ([],
                        "Remove last object in list"),
             "printInt": ([(lwcpy.AD_IN, lwcpy.AT_INT),
                           (lwcpy.AD_IN, lwcpy.AT_STRING, -1, True, "", "indent")],
                          "Print an integer number with optional indent")}
  
  Description = "List of lwc::Object instances."
  
  def __init__(self):
    lwcpy.Object.__init__(self)
    self.lst = []
  
  def size(self):
    return len(self.lst)
  
  def push(self, obj):
    self.lst.append(obj)
  
  def at(self, idx):
    if idx < 0 or idx >= len(self.lst):
      raise Exception("Invalid index %s in list" % idx)
    return self.lst[idx]
  
  def set(self, idx, obj):
    if idx < 0 or idx >= len(self.lst):
      raise Exception("Invalid index %s in list" % idx)
    self.lst[idx] = obj
  
  def first(self):
    if len(self.lst) > 0:
      return self.lst[0]
    else:
      return None
  
  def last(self):
    if len(self.lst) > 0:
      return self.lst[-1]
    else:
      return None
  
  def erase(self, idx):
    if idx < 0 or idx >= len(self.lst):
      raise Exception("Invalid index %s in list" % idx)
    self.lst.remove(self.lst[idx])
  
  def pop(self):
    if len(self.lst) > 0:
      self.lst = self.lst[:-1]
  
  def printInt(self, val, indent=""):
    print("%s%s" % (indent, val))
  

class ObjectList2(ObjectList):
  
  Methods = {"clear": ([],
                       "Remove all object(s) from list")}
  
  Description = "Clearable list of lwc::Object instances."
  
  def __init__(self):
    ObjectList.__init__(self)
  
  def clear(self):
    self.lst = []
  


# Module init

def LWC_ModuleGetTypeCount():
  return 2

def LWC_ModuleGetTypeName(idx):
  if idx == 0:
    return "pytest.ObjectList"
  elif idx == 1:
    return "pytest.ObjectList2"
  else:
    return None

def LWC_ModuleGetTypeClass(idx):
  if idx == 0:
    return ObjectList
  elif idx == 1:
    return ObjectList2
  else:
    return None



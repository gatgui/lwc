import lwcpy

class ObjectList(lwcpy.Object):
  
  Methods = {"push"  : [(lwcpy.AD_IN, lwcpy.AT_OBJECT)],
             "at"    : [(lwcpy.AD_IN, lwcpy.AT_LONG), (lwcpy.AD_RETURN, lwcpy.AT_OBJECT)],
             "set"   : [(lwcpy.AD_IN, lwcpy.AT_LONG), (lwcpy.AD_IN, lwcpy.AT_OBJECT)],
             "first" : [(lwcpy.AD_RETURN, lwcpy.AT_OBJECT)],
             "last"  : [(lwcpy.AD_RETURN, lwcpy.AT_OBJECT)],
             "erase" : [(lwcpy.AD_IN, lwcpy.AT_LONG)],
             "size"  : [(lwcpy.AD_RETURN, lwcpy.AT_LONG)],
             "pop"   : []}
  
  def __init__(self):
    lwcpy.Object.__init__(self)
    self.lst = []
  
  def size(self):
    return len(self.lst)
  
  def push(self, obj):
    self.lst.append(obj)
  
  def at(self, idx):
    if idx < 0 or idx > len(self.lst):
      raise Exception("Invalid index %s in list" % idx)
    return self.lst[idx]
  
  def set(self, idx, obj):
    if idx < 0 or idx > len(self.lst):
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
    if idx < o or idx > len(self.lst):
      raise Exception("Invalid index %s in list" % idx)
    self.lst.remove(self.lst[idx])
  
  def pop(self):
    if len(self.lst) > 0:
      self.lst = self.lst[:-1]

# Module init

def LWC_ModuleGetTypeCount():
  return 1

def LWC_ModuleGetTypeName(idx):
  return "pytest.ObjectList" if idx == 0 else None

def LWC_ModuleGetTypeClass(idx):
  return ObjectList if idx == 0 else None



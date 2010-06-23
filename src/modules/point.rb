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

module Point
  
  class Point < RLWC::Object
    
    Methods = {:getX => [[RLWC::AD_OUT, RLWC::AT_INT]],
               :getY => [[RLWC::AD_OUT, RLWC::AT_INT]],
               :set  => [[RLWC::AD_IN, RLWC::AT_INT], [RLWC::AD_IN, RLWC::AT_INT]],
               :setX => [[RLWC::AD_IN, RLWC::AT_INT]],
               :setY => [[RLWC::AD_IN, RLWC::AT_INT]]}
    
    def initialize()
      super()
      @x = 0
      @y = 0
    end
    
    def getX()
      return @x
    end
    
    def getY()
      return @y
    end
    
    def setX(val)
      @x = val
    end
    
    def setY(val)
      @y = val
    end
    
    def set(x, y)
      @x = x
      @y = y
    end
    
  end
  
  class Point2 < Point
    
    Methods = {:dot => [[RLWC::AD_IN, RLWC::AT_OBJECT], [RLWC::AD_OUT, RLWC::AT_REAL]]}
    
    def initialize()
      super()
    end
    
    def dot(rhs)
      return (@x*rhs.x + @y*rhs.y)
    end
    
  end
  
  # IL Module Entry points
  
  def self.LWC_ModuleGetTypeCount()
    #return 1
    return 2
  end
  
  def self.LWC_ModuleGetTypeName(idx)
    case idx
    when 0
      return "rbtest.Point"
    when 1
      return "rbtest.Point2"
    else
      return nil
    end
    #return (idx == 0 ? "rbtest.Point" : nil)
  end
  
  def self.LWC_ModuleGetTypeClass(idx)
    case idx
    when 0
      return Point
    when 1
      return Point2
    else
      return nil
    end
    #return (idx == 0 ? Point : nil)
  end
  
end



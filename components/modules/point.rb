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
    
    Methods = {:getX => [[RLWC::AD_RETURN, RLWC::AT_LONG]],
               :getY => [[RLWC::AD_RETURN, RLWC::AT_LONG]],
               :set  => [[RLWC::AD_IN, RLWC::AT_LONG], [RLWC::AD_IN, RLWC::AT_LONG]],
               :setX => [[RLWC::AD_IN, RLWC::AT_LONG]],
               :setY => [[RLWC::AD_IN, RLWC::AT_LONG]]}
    
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
  
  # IL Module Entry points
  
  def self.LWC_ModuleGetTypeCount()
    return 1
  end
  
  def self.LWC_ModuleGetTypeName(idx)
    return (idx == 0 ? "rbtest.Point" : nil)
  end
  
  def self.LWC_ModuleGetTypeClass(idx)
    return (idx == 0 ? Point : nil)
  end
  
end



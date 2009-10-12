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



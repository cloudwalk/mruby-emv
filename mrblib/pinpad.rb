class Platform
  class Pinpad
    LOADKEY_DES         = 0
    LOADKEY_3DES        = 1

    DUKPT_DEFAULT = {
      "KSN"         => "",
      "PINBLOCK"    => ""
    }

    class << self
      attr_accessor :pinpad
    end

    def self.dukpt_default
      DUKPT_DEFAULT.dup
    end
  end
end

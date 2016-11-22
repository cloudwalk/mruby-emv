class Platform
  class Pinpad
    class << self
      attr_reader :info
    end
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

    def self.smartcard_detect?
    end
  end
end

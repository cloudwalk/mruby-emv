class Platform
  class EMV
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
        attr_reader :manufacturer, :details, :ctls, :firmaware_version, :shared_version,
          :application_version, :serial_number
      end

      def self.init
        ret, buffer = self.info("00")
        if buffer
          @manufacturer        = buffer[0..19]
          @details             = buffer[20..38]
          @ctls                = buffer[39]
          @firmaware_version   = buffer[40..59]
          @shared_version      = buffer[60..63]
          @application_version = buffer[64..79]
          @serial_number       = buffer[80..99].to_s.strip
        end
      end

      def self.dukpt_default
        DUKPT_DEFAULT.dup
      end

      def self.smartcard_detect?
      end
    end
  end
end


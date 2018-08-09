class Platform
  class EMV
    class Pinpad
      class << self
        attr_reader :info
      end
      RET_OK              = 0
      LOADKEY_DES         = 0
      LOADKEY_3DES        = 1

      DUKPT_DEFAULT = {
        "KSN"         => "",
        "PINBLOCK"    => ""
      }

      #TODO Platform custom
      GEDI_KMS_KEYTYPE_DES        = 0
      GEDI_KMS_KEYTYPE_TDES       = 1
      GEDI_KMS_KEYTYPE_DUKPT_DES  = 2
      GEDI_KMS_KEYTYPE_DUKPT_TDES = 3
      GEDI_KMS_KEYTYPE_AES        = 4

      GEDI_KMS_KEYPURPOSE_PIN     = 0 # Keys used for PIN acquisition.
      GEDI_KMS_KEYPURPOSE_DATA    = 1 # Keys used for data management.
      GEDI_KMS_KEYPURPOSE_KEK     = 2 # Keys used for transport of other keys.
      GEDI_KMS_KEYPURPOSE_SRED    = 3 # Keys used for encrypting account data.
      GEDI_KMS_KEYPURPOSE_AUTH    = 4 # Keys used for encrypting authenticated messages.

      class << self
        attr_accessor :pinpad
        attr_reader :manufacturer, :details, :ctls, :firmaware_version, :shared_version,
          :application_version, :serial_number
        alias_method :orig_info, :info
        alias_method :orig_set_working_key, :set_working_key
        alias_method :orig_display, :display
        alias_method :orig_start_get_key, :start_get_key
        alias_method :orig_get_key, :get_key
        alias_method :orig_start_get_pin, :start_get_pin
        alias_method :orig_get_pin, :get_pin
        alias_method :orig_start_check_event, :start_check_event
        alias_method :orig_check_event, :check_event
        alias_method :orig__encrypt_buffer, :_encrypt_buffer
        alias_method :orig_get_dukpt, :get_dukpt
        alias_method :orig_start_chip_direct, :start_chip_direct
        alias_method :orig_chip_direct, :chip_direct
        alias_method :orig_start_generic_command, :start_generic_command
        alias_method :orig_generic_command, :generic_command
        alias_method :orig_key_test, :key_test
        alias_method :orig__key_kcv, :_key_kcv
        alias_method :orig__key_ksn, :_key_ksn
      end

      def self.change_working_directory(&block)
        Dir.chdir("/home/LEGACY_FS/SYSTEM")
        ret = block.call
        Dir.chdir("/home/APPS/ohyeah")
        ret
      end

      def self.info(input)
        change_working_directory do
          self.orig_info(input)
        end
      end

      def self.set_working_key(input)
        change_working_directory do
          self.orig_set_working_key(input)
        end
      end

      def self.display(msg)
        change_working_directory do
          self.orig_display(msg)
        end
      end

      def self.start_get_key
        change_working_directory do
          self.orig_start_get_key
        end
      end

      def self.get_key
        change_working_directory do
          self.orig_get_key
        end
      end

      def self.start_get_pin(input)
        change_working_directory do
          self.orig_start_get_pin(input)
        end
      end

      def self.get_pin
        change_working_directory do
          self.orig_get_pin
        end
      end

      def self.start_check_event(input)
        change_working_directory do
          self.orig_start_check_event(input)
        end
      end

      def self.check_event(input)
        change_working_directory do
          self.orig_check_event(input)
        end
      end

      def self._encrypt_buffer(input)
        change_working_directory do
          self.orig__encrypt_buffer(input)
        end
      end

      def self.get_dukpt(input)
        change_working_directory do
          self.orig_get_dukpt(input)
        end
      end

      def self.start_chip_direct(input)
        change_working_directory do
          self.orig_start_chip_direct(input)
        end
      end

      def self.chip_direct
        change_working_directory do
          self.orig_chip_direct
        end
      end

      def self.start_generic_command(input)
        change_working_directory do
          self.orig_start_generic_command(input)
        end
      end

      def self.generic_command
        change_working_directory do
          self.orig_generic_command
        end
      end

      def self.key_test(type, operation, index)
        change_working_directory do
          self.orig_key_test(type, operation, index)
        end
      end

      def self._key_kcv(type, operation, index)
        change_working_directory do
          self.orig__key_kcv(type, operation, index)
        end
      end

      def self._key_ksn(type, operation, index)
        change_working_directory do
          self.orig__key_ksn(type, operation, index)
        end
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

      def self.select_result(ret)
        case ret
        when EmvSharedLibrary::PPCOMP_PROCESSING
          true
        when EmvSharedLibrary::PPCOMP_MCDATAERR
          true
        when EmvSharedLibrary::PPCOMP_NOTIFY
          true
        else
          false
        end
      end

      def self.pin(options = {})
        index   = options[:index] || 0
        pan     = options[:pan]
        message = options[:message]

        include DaFunk::Helper
        response = {}

        options = {delimiter: ".", precision: 2, separator: "," }
        input = input_pin(index, pan, message)
        response["return"] = Device::EMV::Pinpad.start_get_pin(input)

        return response unless response["return"] == RET_OK

        timeout = Time.now + EmvTransaction.timeout
        response["return"] = EmvSharedLibrary::PPCOMP_PROCESSING

        while select_result(response["return"])
          response["return"], buf, notification = Device::EMV::Pinpad.get_pin

          if response["return"] == EmvSharedLibrary::PPCOMP_OK
            response["block"] = buf[0..15]
            response["ksn"] = buf[16..-1]
            break
          end

          #if getc(400) == Device::IO::CANCEL
          #ret = EmvSharedLibrary::PPCOMP_CANCEL
          #Device::EMV.abort
          #end

          if response["return"] == EmvSharedLibrary::PPCOMP_NOTIFY
            Device::Display.clear
            print(notification.to_s, 2, 4)
          end

          if timeout < Time.now
            response["return"] = EmvSharedLibrary::PPCOMP_TIMEOUT
            Device::EMV.abort
          end
        end

        response
      end

      def self.encrypt_buffer(msg)
        case msg.to_s[0]
        when "3"
          slot = msg[1..2].to_i
          message = msg[35..-1]
          Device::Crypto.dukpt_encrypt_buffer(slot, [message].pack("H*"))
        else
          self._encrypt_buffer(msg)
        end
      end

      def self.key_kcv(index)
        pin  = self._key_kcv(GEDI_KMS_KEYTYPE_DUKPT_TDES , GEDI_KMS_KEYPURPOSE_PIN  , index)
        data = self._key_kcv(GEDI_KMS_KEYTYPE_DUKPT_TDES , GEDI_KMS_KEYPURPOSE_DATA , index)
        {
          :pin  => convert_result_to_hex(pin),
          :data => convert_result_to_hex(data)
        }
      end

      def self.key_ksn(index)
        pin  = self._key_ksn(GEDI_KMS_KEYTYPE_DUKPT_TDES , GEDI_KMS_KEYPURPOSE_PIN  , index)
        data = self._key_ksn(GEDI_KMS_KEYTYPE_DUKPT_TDES , GEDI_KMS_KEYPURPOSE_DATA , index)
        {
          :pin  => convert_result_to_hex(pin),
          :data => convert_result_to_hex(data)
        }
      end

      private

      def self.convert_result_to_hex(result)
        ret, binary = result
        [ret, binary.to_s.unpack("H*").first.to_s.upcase]
      end

      def self.input_pin(index, pan, text)
        "3" +
          rjust(index.to_s, 2, "0") +
          "00000000000000000000000000000000" +
          rjust(pan.size.to_s, 2, "0") +
          rjust(pan, 13, "0").ljust(19, " ") +
          "1" +
          "04" +
          "12" +
          text.gsub("|", "").ljust(32, " ")
      end
    end
  end

  # TODO Scalone, organize responsibilities and interfaces
  Pinpad = EMV::Pinpad
end


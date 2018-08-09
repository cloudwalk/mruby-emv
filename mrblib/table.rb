class Platform
  class EMV
    class Table

      def self.change_working_directory(&block)
        Dir.chdir("/home/LEGACY_FS/SYSTEM")
        ret = block.call
        Dir.chdir("/home/APPS/ohyeah")
        ret
      end

      def self.init(acquirer)
        change_working_directory do
          self._init(acquirer)
        end
      end

      def self.rec(table)
        change_working_directory do
          self._rec(table)
        end
      end

      def self.fin
        change_working_directory do
          self._fin
        end
      end

      def self.change(table)
        change_working_directory do
          self._change(table)
        end
      end
    end
  end
end
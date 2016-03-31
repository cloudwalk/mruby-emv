class Plataform
  class EMV
    PPCOMP_OK            = 0
    PPCOMP_PROCESSING    = 1
    PPCOMP_NOTIFY        = 2

    PPCOMP_F1            = 4
    PPCOMP_F2            = 5
    PPCOMP_F3            = 6
    PPCOMP_F4            = 7
    PPCOMP_BACKSP        = 8

    PPCOMP_INVCALL       = 10
    PPCOMP_INVPARM       = 11
    PPCOMP_TIMEOUT       = 12
    PPCOMP_CANCEL        = 13
    PPCOMP_ALREADYOPEN   = 14
    PPCOMP_NOTOPEN       = 15
    PPCOMP_EXECERR       = 16
    PPCOMP_INVMODEL      = 17
    PPCOMP_NOFUNC        = 18
    PPCOMP_ERRMANDAT     = 19

    PPCOMP_TABEXP        = 20
    PPCOMP_TABERR        = 21
    PPCOMP_NOAPPLIC      = 22

    PPCOMP_PORTERR       = 30
    PPCOMP_COMMERR       = 31
    PPCOMP_UNKNOWNSTAT   = 32
    PPCOMP_RSPERR        = 33
    PPCOMP_COMMTOUT      = 34

    PPCOMP_INTERR        = 40
    PPCOMP_MCDATAERR     = 41
    PPCOMP_ERRPIN        = 42
    PPCOMP_NOCARD        = 43
    PPCOMP_PINBUSY       = 44

    PPCOMP_SAMERR        = 50
    PPCOMP_NOSAM         = 51
    PPCOMP_SAMINV        = 52

    PPCOMP_DUMBCARD      = 60
    PPCOMP_ERRCARD       = 61
    PPCOMP_CARDINV       = 62
    PPCOMP_CARDBLOCKED   = 63
    PPCOMP_CARDNAUTH     = 64
    PPCOMP_CARDEXPIRED   = 65
    PPCOMP_CARDERRSTRUCT = 66
    PPCOMP_CARDINVALIDAT = 67
    PPCOMP_CARDPROBLEMS  = 68
    PPCOMP_CARDINVDATA   = 69
    PPCOMP_CARDAPPNAV    = 70
    PPCOMP_CARDAPPNAUT   = 71
    PPCOMP_NOBALANCE     = 72
    PPCOMP_LIMITEXC      = 73
    PPCOMP_CARDNOTEFFECT = 74
    PPCOMP_VCINVCURR     = 75
    PPCOMP_ERRFALLBACK   = 76

    class << self
      attr_accessor :init, :process, :tags, :optional_tags,
      attr_reader :card, :message_notify, :process_info, :process_message_notify
    end
  end
end


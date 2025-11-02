set -Eeuo pipefail
IFS=$'\n\t'


#################################################################################
#   0.      TEXT...                                                             #
#################################################################################
TEXT="Hello, World!"



#################################################################################
#   1.      COLORS...                                                           #
#################################################################################
log()       { printf "%s\n"   "$1"; }


#   TEST...
test_0()   { printf "Black      : \e[30m %s \e[0m.      \e[90m %s \e[0m. \n"   "$1" "$1"; }
test_1()   { printf "Red        : \e[31m %s \e[0m.      \e[91m %s \e[0m. \n"   "$1" "$1"; }
test_2()   { printf "Green      : \e[32m %s \e[0m.      \e[92m %s \e[0m. \n"   "$1" "$1"; }
test_3()   { printf "Yellow     : \e[33m %s \e[0m.      \e[93m %s \e[0m. \n"   "$1" "$1"; }
test_4()   { printf "Blue       : \e[34m %s \e[0m.      \e[94m %s \e[0m. \n"   "$1" "$1"; }
test_5()   { printf "Purple     : \e[35m %s \e[0m.      \e[95m %s \e[0m. \n"   "$1" "$1"; }
test_6()   { printf "Cyan       : \e[36m %s \e[0m.      \e[96m %s \e[0m. \n"   "$1" "$1"; }
test_7()   { printf "White      : \e[37m %s \e[0m.      \e[97m %s \e[0m. \n"   "$1" "$1"; }



#   BATCH #1A       (NORMAL FOREGROUND)...
log_10A()   { printf "ESC[30m       Black       : \e[30m %s \e[0m \n"   "$1"; }
log_11A()   { printf "ESC[31m       Red         : \e[31m %s \e[0m \n"   "$1"; }
log_12A()   { printf "ESC[32m       Green       : \e[32m %s \e[0m \n"   "$1"; }
log_13A()   { printf "ESC[33m       Yellow      : \e[33m %s \e[0m \n"   "$1"; }
log_14A()   { printf "ESC[34m       Blue        : \e[34m %s \e[0m \n"   "$1"; }
log_15A()   { printf "ESC[35m       Purple      : \e[35m %s \e[0m \n"   "$1"; }
log_16A()   { printf "ESC[36m       Cyan        : \e[36m %s \e[0m \n"   "$1"; }
log_17A()   { printf "ESC[37m       White       : \e[37m %s \e[0m \n"   "$1"; }
log_18A()   { printf "ESC[38m                   : \e[38m %s \e[0m \n"   "$1"; }
log_19A()   { printf "ESC[39m                   : \e[39m %s \e[0m \n"   "$1"; }


#   BATCH #1B.      (NORMAL BACKGROUND)...
log_10B()   { printf "ESC[40m       : \e[40m %s \e[0m \n"   "$1"; }
log_11B()   { printf "ESC[41m       : \e[41m %s \e[0m \n"   "$1"; }
log_12B()   { printf "ESC[42m       : \e[42m %s \e[0m \n"   "$1"; }
log_13B()   { printf "ESC[43m       : \e[43m %s \e[0m \n"   "$1"; }
log_14B()   { printf "ESC[44m       : \e[44m %s \e[0m \n"   "$1"; }
log_15B()   { printf "ESC[45m       : \e[45m %s \e[0m \n"   "$1"; }
log_16B()   { printf "ESC[46m       : \e[46m %s \e[0m \n"   "$1"; }
log_17B()   { printf "ESC[47m       : \e[47m %s \e[0m \n"   "$1"; }
log_18B()   { printf "ESC[48m       : \e[48m %s \e[0m \n"   "$1"; }
log_19B()   { printf "ESC[49m       : \e[49m %s \e[0m \n"   "$1"; }


#   BATCH #2A.      (BRIGHT FOREGROUND)...
log_20A()   { printf "ESC[90m       Black   : \e[90m %s \e[0m \n"   "$1"; }
log_21A()   { printf "ESC[91m       Red     : \e[91m %s \e[0m \n"   "$1"; }
log_22A()   { printf "ESC[92m       Green   : \e[92m %s \e[0m \n"   "$1"; }
log_23A()   { printf "ESC[93m       Yellow  : \e[93m %s \e[0m \n"   "$1"; }
log_24A()   { printf "ESC[94m       Blue    : \e[94m %s \e[0m \n"   "$1"; }
log_25A()   { printf "ESC[95m       Purple  : \e[95m %s \e[0m \n"   "$1"; }
log_26A()   { printf "ESC[96m       Cyan    : \e[96m %s \e[0m \n"   "$1"; }
log_27A()   { printf "ESC[97m       White   : \e[97m %s \e[0m \n"   "$1"; }


#   BATCH #2B.      (BRIGHT BACKGROUND)...
log_20B()   { printf "ESC[100m       : \e[100m %s \e[0m \n"   "$1"; }
log_21B()   { printf "ESC[101m       : \e[101m %s \e[0m \n"   "$1"; }
log_22B()   { printf "ESC[102m       : \e[102m %s \e[0m \n"   "$1"; }
log_23B()   { printf "ESC[103m       : \e[103m %s \e[0m \n"   "$1"; }
log_24B()   { printf "ESC[104m       : \e[104m %s \e[0m \n"   "$1"; }
log_25B()   { printf "ESC[105m       : \e[105m %s \e[0m \n"   "$1"; }
log_26B()   { printf "ESC[106m       : \e[106m %s \e[0m \n"   "$1"; }
log_27B()   { printf "ESC[107m       : \e[107m %s \e[0m \n"   "$1"; }




#   BATCH #3A       (DIM/FAINT FOREGROUND)...
log_300A()  { printf "ESC[2m            : \e[2m %s \e[0m \n"            "$1"; }
log_30A()   { printf "ESC[2m ESC[30m    : \e[2m\e[30m %s \e[0m \n"      "$1"; }
log_31A()   { printf "ESC[2m ESC[31m    : \e[2m\e[31m %s \e[0m \n"      "$1"; }
log_32A()   { printf "ESC[2m ESC[32m    : \e[2m\e[32m %s \e[0m \n"      "$1"; }
log_33A()   { printf "ESC[2m ESC[33m    : \e[2m\e[33m %s \e[0m \n"      "$1"; }
log_34A()   { printf "ESC[2m ESC[34m    : \e[2m\e[34m %s \e[0m \n"      "$1"; }
log_35A()   { printf "ESC[2m ESC[35m    : \e[2m\e[35m %s \e[0m \n"      "$1"; }
log_36A()   { printf "ESC[2m ESC[36m    : \e[2m\e[36m %s \e[0m \n"      "$1"; }
log_37A()   { printf "ESC[2m ESC[37m    : \e[2m\e[37m %s \e[0m \n"      "$1"; }
log_38A()   { printf "ESC[2m ESC[38m    : \e[2m\e[38m %s \e[0m \n"      "$1"; }
log_39A()   { printf "ESC[2m ESC[39m    : \e[2m\e[39m %s \e[0m \n"      "$1"; }


#   BATCH #3B.      (DIM/FAINT BACKGROUND)...
log_30B()   { printf "ESC[2m ESC[40m    : \e[2m\e[40m %s \e[0m \n"      "$1"; }
log_31B()   { printf "ESC[2m ESC[41m    : \e[2m\e[41m %s \e[0m \n"      "$1"; }
log_32B()   { printf "ESC[2m ESC[42m    : \e[2m\e[42m %s \e[0m \n"      "$1"; }
log_33B()   { printf "ESC[2m ESC[43m    : \e[2m\e[43m %s \e[0m \n"      "$1"; }
log_34B()   { printf "ESC[2m ESC[44m    : \e[2m\e[44m %s \e[0m \n"      "$1"; }
log_35B()   { printf "ESC[2m ESC[45m    : \e[2m\e[45m %s \e[0m \n"      "$1"; }
log_36B()   { printf "ESC[2m ESC[46m    : \e[2m\e[46m %s \e[0m \n"      "$1"; }
log_37B()   { printf "ESC[2m ESC[47m    : \e[2m\e[47m %s \e[0m \n"      "$1"; }
log_38B()   { printf "ESC[2m ESC[48m    : \e[2m\e[48m %s \e[0m \n"      "$1"; }
log_39B()   { printf "ESC[2m ESC[49m    : \e[2m\e[49m %s \e[0m \n"      "$1"; }




#   BATCH #4A       (BOLD FOREGROUND)...
log_40A()   { printf "ESC[1m ESC[30m    : \e[1m\e[30m %s \e[0m \n"      "$1"; }
log_41A()   { printf "ESC[1m ESC[31m    : \e[1m\e[31m %s \e[0m \n"      "$1"; }
log_42A()   { printf "ESC[1m ESC[32m    : \e[1m\e[32m %s \e[0m \n"      "$1"; }
log_43A()   { printf "ESC[1m ESC[33m    : \e[1m\e[33m %s \e[0m \n"      "$1"; }
log_44A()   { printf "ESC[1m ESC[34m    : \e[1m\e[34m %s \e[0m \n"      "$1"; }
log_45A()   { printf "ESC[1m ESC[35m    : \e[1m\e[35m %s \e[0m \n"      "$1"; }
log_46A()   { printf "ESC[1m ESC[36m    : \e[1m\e[36m %s \e[0m \n"      "$1"; }
log_47A()   { printf "ESC[1m ESC[37m    : \e[1m\e[37m %s \e[0m \n"      "$1"; }
log_48A()   { printf "ESC[1m ESC[38m    : \e[1m\e[38m %s \e[0m \n"      "$1"; }
log_49A()   { printf "ESC[1m ESC[39m    : \e[1m\e[39m %s \e[0m \n"      "$1"; }


#   BATCH #4B.      (BOLD BACKGROUND)...
log_40B()   { printf "ESC[1m ESC[40m    : \e[1m\e[40m %s \e[0m \n"      "$1"; }
log_41B()   { printf "ESC[1m ESC[41m    : \e[1m\e[41m %s \e[0m \n"      "$1"; }
log_42B()   { printf "ESC[1m ESC[42m    : \e[1m\e[42m %s \e[0m \n"      "$1"; }
log_43B()   { printf "ESC[1m ESC[43m    : \e[1m\e[43m %s \e[0m \n"      "$1"; }
log_44B()   { printf "ESC[1m ESC[44m    : \e[1m\e[44m %s \e[0m \n"      "$1"; }
log_45B()   { printf "ESC[1m ESC[45m    : \e[1m\e[45m %s \e[0m \n"      "$1"; }
log_46B()   { printf "ESC[1m ESC[46m    : \e[1m\e[46m %s \e[0m \n"      "$1"; }
log_47B()   { printf "ESC[1m ESC[47m    : \e[1m\e[47m %s \e[0m \n"      "$1"; }
log_48B()   { printf "ESC[1m ESC[48m    : \e[1m\e[48m %s \e[0m \n"      "$1"; }
log_49B()   { printf "ESC[1m ESC[49m    : \e[1m\e[49m %s \e[0m \n"      "$1"; }











#################################################################################
#   2.      OUTPUTS...                                                          #
#################################################################################

log_ok()        { printf "      [\e[1m\e[32m  OK   \e[0m]   : \e[32m%s\e[0m\n"        "$1"; }
log_info()      { printf "      [\e[2m\e[39m INFO  \e[0m]   : \e[2m\e[39m%s\e[0m\n"   "$1"; }
log_err()       { printf "      [\e[1m\e[31m ERROR \e[0m]   : \e[1m\e[31m%s\e[0m\n"   "$1" >&2; }
log_notify()    { printf "\e[34m==>\e[0m \e[1m\e[39m%s\e[0m\n"   "$1"; }
abort()         { log_err "$1"; exit 1; }




log_ok      "OKAY logging message..."
log_info    "INFO logging message..."
log_err     "ERROR logging message..."
log_notify  "NOTICE logging message..."



echo ""
log "TESTING COLORS..."
test_0 "$TEXT"
test_1 "$TEXT"
test_2 "$TEXT"
test_3 "$TEXT"
test_4 "$TEXT"
test_5 "$TEXT"
test_6 "$TEXT"
test_7 "$TEXT"
echo ""
echo ""




if true; then
#
#
#
#   BATCH #1A...
    log "BATCH #1A.     NORMAL FOREGROUND COLORS..."
    log_10A "$TEXT"
    log_11A "$TEXT"
    log_12A "$TEXT"
    log_13A "$TEXT"
    log_14A "$TEXT"
    log_15A "$TEXT"
    log_16A "$TEXT"
    log_17A "$TEXT"
    log_18A "$TEXT"
    log_19A "$TEXT"
#
#
#   BATCH #1B...
    echo ""
    log "BATCH #1B.     NORMAL BACKGROUND COLORS..."
    log_10B "$TEXT"
    log_11B "$TEXT"
    log_12B "$TEXT"
    log_13B "$TEXT"
    log_14B "$TEXT"
    log_15B "$TEXT"
    log_16B "$TEXT"
    log_17B "$TEXT"
    log_18B "$TEXT"
    log_19B "$TEXT"
#
#
#   BATCH #2A...
    echo ""
    log "BATCH #2A.      BRIGHT FOREGROUND COLORS..."
    log_20A "$TEXT"
    log_21A "$TEXT"
    log_22A "$TEXT"
    log_23A "$TEXT"
    log_24A "$TEXT"
    log_25A "$TEXT"
    log_26A "$TEXT"
    log_27A "$TEXT"
#
#
#   BATCH #2B...
    echo ""
    log "BATCH #2B.      BRIGHT FOREGROUND COLORS..."
    log_20B "$TEXT"
    log_21B "$TEXT"
    log_22B "$TEXT"
    log_23B "$TEXT"
    log_24B "$TEXT"
    log_25B "$TEXT"
    log_26B "$TEXT"
    log_27B "$TEXT"
#
#
#
    echo ""
    echo ""
    echo ""
#
#   BATCH #3A...
    log         "BATCH #3A.     DIM/FAINT FOREGROUND COLORS..."
    log_300A    "$TEXT"
    log_30A     "$TEXT"
    log_31A     "$TEXT"
    log_32A     "$TEXT"
    log_33A     "$TEXT"
    log_34A     "$TEXT"
    log_35A     "$TEXT"
    log_36A     "$TEXT"
    log_37A     "$TEXT"
    log_38A     "$TEXT"
    log_39A     "$TEXT"
#
#
#   BATCH #3B...
    echo ""
    log         "BATCH #3B.     DIM/FAINT BACKGROUND COLORS..."
    log_30B     "$TEXT"
    log_31B     "$TEXT"
    log_32B     "$TEXT"
    log_33B     "$TEXT"
    log_34B     "$TEXT"
    log_35B     "$TEXT"
    log_36B     "$TEXT"
    log_37B     "$TEXT"
    log_38B     "$TEXT"
    log_39B     "$TEXT"
#
#
#
    echo ""
    echo ""
    echo ""
#
#   BATCH #4A...
    log         "BATCH #4A.     BOLD FOREGROUND COLORS..."
    log_40A     "$TEXT"
    log_41A     "$TEXT"
    log_42A     "$TEXT"
    log_43A     "$TEXT"
    log_44A     "$TEXT"
    log_45A     "$TEXT"
    log_46A     "$TEXT"
    log_47A     "$TEXT"
    log_48A     "$TEXT"
    log_49A     "$TEXT"
#
#
#   BATCH #4B...
    echo ""
    log         "BATCH #4B.     BOLD BACKGROUND COLORS..."
    log_40B     "$TEXT"
    log_41B     "$TEXT"
    log_42B     "$TEXT"
    log_43B     "$TEXT"
    log_44B     "$TEXT"
    log_45B     "$TEXT"
    log_46B     "$TEXT"
    log_47B     "$TEXT"
    log_48B     "$TEXT"
    log_49B     "$TEXT"
#
#
#
fi







#################################################################################
#################################################################################   END.

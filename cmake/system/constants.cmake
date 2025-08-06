####################################################################################
#
#       *********************************************************************
#       ****        C O N S T A N T S . C M A K E  _____  F I L E        ****
#       *********************************************************************
#
#              AUTHOR:      Collin A. Bond
#               DATED:      July 20, 2025.
#
#   PURPOSE:
#           Define misc. constants and variables for the build system.
#
#
####################################################################################
####################################################################################



#   1.      ANSI COLOR CODES...
####################################################################################
####################################################################################

#   1.1.    DEFINE ANSI COLORS FOR macOS BUILDS...
#
if(APPLE)
#
#   CASE 1 :    ASCII IS ENABLED ON APPLE BUILDS...
####################################################################################
    string(ASCII 27 ESC)                    #     ESC = '\x1B'
#
#
#       0.      MISC. ANSI CODES...
    set(        RESET                       "${ESC}[0m")
    set(        RESET_BOLD                  "${ESC}[22m")
    set(        RESET_DIM                   "${ESC}[22m")
    set(        RESET_UNDERLINE             "${ESC}[24m")
    set(        RESET_BLINK                 "${ESC}[25m")
    set(        RESET_INVERSE               "${ESC}[27m")
    #
    set(        BOLD                        "${ESC}[1m")
    set(        DIM                         "${ESC}[2m")
    set(        UNDERLINE                   "${ESC}[4m")
    set(        BLINK                       "${ESC}[5m")
    set(        INVERSE                     "${ESC}[7m")
#
#
#       1.      FOREGROUND COLORS...
    set(        BLACK                       "${ESC}[30m")
    set(        RED                         "${ESC}[31m")
    set(        GREEN                       "${ESC}[32m")
    set(        YELLOW                      "${ESC}[33m")
    set(        BLUE                        "${ESC}[34m")
    set(        MAGENTA                     "${ESC}[35m")
    set(        CYAN                        "${ESC}[36m")
    set(        WHITE                       "${ESC}[37m")
#
    set(        BLACK_BRIGHT                "${ESC}[90m")
    set(        RED_BRIGHT                  "${ESC}[91m")
    set(        GREEN_BRIGHT                "${ESC}[92m")
    set(        YELLOW_BRIGHT               "${ESC}[93m")
    set(        BLUE_BRIGHT                 "${ESC}[94m")
    set(        MAGENTA_BRIGHT              "${ESC}[95m")
    set(        CYAN_BRIGHT                 "${ESC}[96m")
    set(        WHITE_BRIGHT                "${ESC}[97m")
#
#
#
#       2.      BACKGROUND COLORS...
    set(        BLACK_BG                    "${ESC}[40m")
    set(        RED_BG                      "${ESC}[41m")
    set(        GREEN_BG                    "${ESC}[42m")
    set(        YELLOW_BG                   "${ESC}[43m")
    set(        BLUE_BG                     "${ESC}[44m")
    set(        MAGENTA_BG                  "${ESC}[45m")
    set(        CYAN_BG                     "${ESC}[46m")
    set(        WHITE_BG                    "${ESC}[47m")
#
    set(        BLACK_BRIGHT_BG             "${ESC}[100m")
    set(        RED_BRIGHT_BG               "${ESC}[101m")
    set(        GREEN_BRIGHT_BG             "${ESC}[102m")
    set(        YELLOW_BRIGHT_BG            "${ESC}[103m")
    set(        BLUE_BRIGHT_BG              "${ESC}[104m")
    set(        MAGENTA_BRIGHT_BG           "${ESC}[105m")
    set(        CYAN_BRIGHT_BG              "${ESC}[106m")
    set(        WHITE_BRIGHT_BG             "${ESC}[107m")
#
#
#
#   CASE 2 :    ASCII IS *DISABLED*...
#               WE DEFINE EMPTY VARIABLES FOR OTHER BUILDS TO PREVENT ERRORS.
####################################################################################
else()
    foreach(c RESET BOLD BLACK RED GREEN YELLOW BLUE MAGENTA CYAN WHITE)
        set(${c} "")
    endforeach()
endif()












####################################################################################
####################################################################################    # END.

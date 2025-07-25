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
if(APPLE)
    string(ASCII 27 ESC)                    #     ESC = '\x1B'
    
#
#
    set(        RESET                       "${ESC}[0m")
    set(        BOLD                        "${ESC}[1m")
#
    set(        BLACK                       "${ESC}[30m")
    set(        RED                         "${ESC}[31m")
    set(        GREEN                       "${ESC}[32m")
    set(        YELLOW                      "${ESC}[33m")
    set(        BLUE                        "${ESC}[34m")
    set(        MAGENTA                     "${ESC}[35m")
    set(        CYAN                        "${ESC}[36m")
    set(        WHITE                       "${ESC}[37m")
#
#
#
#   1.2.    OTHERWISE---WE DEFINE EMPTY VARIABLES FOR OTHER BUILDS TO PREVENT ERRORS...
else()
    foreach(c RESET BOLD BLACK RED GREEN YELLOW BLUE MAGENTA CYAN WHITE)
        set(${c} "")
    endforeach()
endif()












####################################################################################
####################################################################################    # END.

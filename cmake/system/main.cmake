####################################################################################
#
#       *********************************************************************
#       ****             M A I N . C M A K E  _____  F I L E             ****
#       *********************************************************************
#
#              AUTHOR:      Collin A. Bond
#               DATED:      July 02, 2025.
#
#   PURPOSE:
#           Exported "header"-esque file.  The "CMakeLists.txt" file needs only
#       to import this one, single file.  Then, this file handles the remaining
#       responsibilities of importing each additional file that will be used across
#       the entire build system of my appliation.
#
#
####################################################################################
####################################################################################



#   1.      IMPORTED FILES...
####################################################################################
####################################################################################

include(${CMAKE_CURRENT_LIST_DIR}/constants.cmake)              #   CONSTANTS, ETC...
include(${CMAKE_CURRENT_LIST_DIR}/project_metadata.cmake)       #   METADATA / INFO.
include(${CMAKE_CURRENT_LIST_DIR}/utility.cmake)                #   FUNCTIONS AND MISC.
#
include(${CMAKE_CURRENT_LIST_DIR}/build.cmake)                  #       COMPILER FLAGS, ETC...












####################################################################################
####################################################################################    # END.

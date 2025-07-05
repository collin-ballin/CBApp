####################################################################################
#
#       *********************************************************************
#       ****          U T I L I T Y . C M A K E  _____  F I L E          ****
#       *********************************************************************
#
#              AUTHOR:      Collin A. Bond
#               DATED:      July 04, 2025.
#
#   PURPOSE:
#           Define helper functions to be used inside the build system.
#
#
####################################################################################
####################################################################################



#   2.          HELPER FUNCTIONS / UTILITIES...
####################################################################################
####################################################################################

#   "print_list"
#
function(print_list list_var)
    # Guard: warn if the list is empty
    if(NOT ${list_var})
        message(WARNING "[print_list] '${list_var}' is empty")
        return()
    endif()

    message(STATUS "---- ${list_var} ----")
    set(idx 0)
    foreach(item IN LISTS ${list_var})
        message(STATUS "  #${idx}\t${item}")
        math(EXPR idx "${idx}+1")
    endforeach()
endfunction()



# ---------------------------------------------------------------------------
#  CB_Log(<LEVEL> <FORMAT_STRING> [fmt-args...])
#
#  LEVEL          One of CMake's message modes: STATUS, DEBUG, WARNING, etc.
#  FORMAT_STRING  Text containing ordinary ${var} expansions *or* @name@ tokens.
#  fmt-args…      Optional VAR=value pairs to inject via @name@ syntax.
#
#  Examples:
#      set(cnt 3)          # normal ${} expansion
#      CB_Log(STATUS "Found ${cnt} source files")
#
#      CB_Log(DEBUG  "File: @file@  (#@idx@ of @total@)"
#                    file=${current} idx=${i} total=${n})
# ---------------------------------------------------------------------------
function(CB_Log level fmt)
    # Collect any additional VAR=value arguments into local variables
    foreach(pair IN LISTS ARGN)
        string(REPLACE "=" ";" _kv "${pair}")   # split "key=value"
        list(GET _kv 0 _k)
        list(GET _kv 1 _v)
        set(${_k} "${_v}" PARENT_SCOPE)        # make visible to string(CONFIGURE)
        set(${_k} "${_v}")                     # also visible inside this function
    endforeach()

    # First pass: allow regular ${var} interpolation already done by CMake
    # Second pass: resolve @var@ tokens, if any
    string(CONFIGURE "${fmt}" _msg @ONLY)

    message(${level} "${_msg}")
endfunction()












####################################################################################
####################################################################################    # END.

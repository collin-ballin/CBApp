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




#   1.          PRIMATIVE LOGGING FUNCTIONS...
####################################################################################
####################################################################################

# ---------------------------------------------------------------------------
#  CB_newline([N])
#
#  Prints N blank lines with `message("")`.
#  - N must be a positive integer; defaults to 1 if omitted or invalid.
#
#  Examples:
#      CB_newline()     # one blank line
#      CB_newline(3)    # three blank lines
# ---------------------------------------------------------------------------
function(CB_newline)
    # -- determine how many lines to print ----------------------------------
    if(ARGC GREATER 0)
        set(_num "${ARGV0}")
    else()
        set(_num 1)
    endif()

    # -- validate (positive integer) ----------------------------------------
    if(NOT _num MATCHES "^[1-9][0-9]*$")
        set(_num 1)                         # fallback to a single newline
    endif()

    # -- emit blank lines ----------------------------------------------------
    while(_num GREATER 0)
        message("")                         # each call adds its own newline
        math(EXPR _num "${_num}-1")
    endwhile()
endfunction()






#
#   ERROR,          WARNING,        NOTICE,         STATUS,         VERBOSE,        DEBUG,          TRACE.
#
# ---------------------------------------------------------------------------
#  CB_Log(<LEVEL> <FORMAT_STRING> [key=value ...])
#
#  LEVEL          Any valid message mode (STATUS, WARNING, DEBUG, …).
#  FORMAT_STRING  May use ordinary ${var} or @key@ placeholders.
#  key=value      Extra placeholders for the @key@ syntax.
# ---------------------------------------------------------------------------
#  CB_Log(<LEVEL> <FORMAT_STRING> [key=value ...])
# ---------------------------------------------------------------------------
function(CB_Log level fmt)
    #   1.      NORMALIZE MESSAGE AND VALIDATE...
    string(TOUPPER "${level}" _lvl)
    if(NOT _lvl MATCHES
       "^(FATAL_ERROR|SEND_ERROR|NOTIFY|WARNING|AUTHOR_WARNING|STATUS|OUTFO|INFO|VERBOSE|DEBUG)$")
        message(FATAL_ERROR "CB_Log(): invalid message level '${level}'")
    endif()


    #   2.      PARSE  (KEY, VALUE)  PAIRS...
    foreach(_pair IN LISTS ARGN)
        if(NOT _pair MATCHES "^([^=]+)=(.+)$")
            message(FATAL_ERROR "CB_Log(): malformed argument '${_pair}' (expect key=value)")
        endif()
        string(REGEX REPLACE "^([^=]+)=(.+)$" "\\1" _k "${_pair}")
        string(REGEX REPLACE "^([^=]+)=(.+)$" "\\2" _v "${_pair}")
        set("${_k}" "${_v}")                 # local only
    endforeach()


    #   3.      "SWITCH" STATEMENT:     MAP (LEVEL)  ==>  (TAG)...
    #
    #
    #           3.1.        "DEBUG".
    if(_lvl STREQUAL "DEBUG")
        set(_tag    "${DIM}${BOLD}${BLACK_BG}${WHITE}DEBUG")
        string(PREPEND fmt "${DIM}${BLACK_BG}")
    #
    #
    #           3.2.        "VERBOSE".
    elseif(_lvl STREQUAL "VERBOSE")
        set(_tag    "${BOLD}VERBOSE")
        string(PREPEND fmt "${WHITE}")
    #
    #
    #           3.3.        "INFO".
    elseif(_lvl STREQUAL "INFO")
        set(_tag    "${BOLD}${GREEN}INFO")
        string(PREPEND fmt "${WHITE_BRIGHT}")
    #
    #
    #           3.4.        "OUTFO".
    elseif(_lvl STREQUAL "OUTFO")
        set(_tag    "${BOLD}${GREEN}INFO")
        string(PREPEND fmt "${GREEN}")
    #
    #
    #           3.5.        "STATUS".
    elseif(_lvl STREQUAL "STATUS")
        set(_tag    "${BOLD}${BLUE}STATUS")
        string(PREPEND fmt "${BLUE}")
    #
    #
    #           3.6.        "WARNING" / "AUTHOR WARNING".
    elseif(_lvl STREQUAL "WARNING" OR _lvl STREQUAL "AUTHOR_WARNING")
        set(_tag    "${BOLD}${YELLOW}WARN")
        string(PREPEND fmt "${YELLOW}")
    #
    #
    #           3.7.        "NOTIFY".
    elseif(_lvl STREQUAL "NOTIFY")
        set(_tag    "${BOLD}${UNDERLINE}${BLACK_BG}${CYAN_BRIGHT}NOTIFY")
        string(PREPEND fmt "${UNDERLINE}${BLACK_BG}${CYAN_BRIGHT}")
    #
    #
    #           3.8.        "FATAL ERROR".
    elseif(_lvl STREQUAL "FATAL_ERROR" OR _lvl STREQUAL "SEND_ERROR")
        set(_tag    "${BOLD}${RED}ERROR")
        string(PREPEND fmt "${RED}")
    #
    #
    #           3.?.        DEFAULT.
    else()
        set(_tag    "${BOLD}???")
        #   set(_tag "${_lvl}")                 # fallback: raw level text
    #
    #
    #
    endif()     #   END SWITCH.


    #   3.      PADD "TAG" LABEL SO  “[ <tag> ]”  IS ALWAYS 9 CHARACTERS WIDE...
    #
    string(LENGTH "${_tag}" _len)
    math(EXPR _pad "9 - ${_len}")           # 9 = max tag width
    set(_spaces "")
    while(_pad GREATER 0)
        string(APPEND _spaces " ")
        math(EXPR _pad "${_pad}-1")
    endwhile()
    set(_prefix "${RESET}[ ${_tag}${_spaces}${RESET} ]\t: ")


    #   4.      DISPLAY MESSAGE...
    string(CONFIGURE "${fmt}" _msg @ONLY)
    message(STATUS      "${_prefix}${_msg}${RESET}")
endfunction()







####################################################################################
#
#
#
#   2.          ADVANCED LOGGING FUNCTIONS...
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


#   "print_file_list"
#
function(print_file_list list_var)
    # ---------- choose root ----------
    set(_root "${CMAKE_SOURCE_DIR}")        # default
    cmake_parse_arguments(ARG "" "" "BASE_DIR" ${ARGN})
    if(ARG_BASE_DIR)
        set(_root "${ARG_BASE_DIR}")
    endif()

    # ---------- sanity ----------
    if(NOT ${list_var})
        message(WARNING "[print_file_list] '${list_var}' is empty")
        return()
    endif()

    message(STATUS "---- ${list_var} (relative to ${_root}) ----")
    set(_idx 0)

    foreach(_abs IN LISTS ${list_var})
        # file(RELATIVE_PATH) trims the prefix if _abs is under _root
        file(RELATIVE_PATH _rel "${_root}" "${_abs}")
        message(STATUS "  #${_idx}\t${_rel}")
        math(EXPR _idx "${_idx}+1")
    endforeach()
endfunction()






# ---------------------------------------------------------------------------
#  CB_file_list_info(<LIST_VAR> [BASE_DIR <path>])
#
#  • Counts how many files are in <LIST_VAR>.
#  • Sums the total number of text-lines across all existing files.
#  • Emits one STATUS message with both figures.
# ---------------------------------------------------------------------------
function(CB_file_list_info level list_var sep)
    set(_root "${CMAKE_SOURCE_DIR}")
    cmake_parse_arguments(ARG "" "" "BASE_DIR" ${ARGN})
    if(ARG_BASE_DIR)
        set(_root "${ARG_BASE_DIR}")
    endif()


    # -------- verify list ----------------------------------------------------
    if(NOT DEFINED ${list_var} OR NOT ${list_var})
        CB_Log(STATUS "'${list_var}' is empty (relative to ${_root})")
        return()
    endif()


    # -------- accumulate -----------------------------------------------------
    set(_file_count 0)
    set(_total_lines 0)
    set(_total_chars 0)

    foreach(_abs IN LISTS ${list_var})
        if(NOT EXISTS "${_abs}")
            CB_Log(WARNING "Missing file in '${list_var}': ${_abs}")
            continue()
        endif()

        # lines
        file(STRINGS "${_abs}" _lines)
        list(LENGTH _lines _n_lines)
        math(EXPR _total_lines "${_total_lines}+${_n_lines}")

        # characters
        file(READ "${_abs}" _content LIMIT_INPUT 0)        # read whole file
        string(LENGTH "${_content}" _n_chars)              # UTF-8 byte count
        math(EXPR _total_chars "${_total_chars}+${_n_chars}")

        math(EXPR _file_count "${_file_count}+1")
    endforeach()




    #   LOG MESSAGE...
    #
    if(ARGC GREATER 3)            # user provided a 4th positional argument
        set(_list_name "${ARGV3}")
    else()
        set(_list_name "'${list_var}'")
    endif()
    #
    set(_msg "${_list_name}${sep}: contains ${_file_count} file(s)"
             " (${_total_lines} lines"
             " ${_total_chars} characters")
    CB_Log(${level} "${_msg})")
    
endfunction()



# ---------------------------------------------------------------------------
#  CB_file_list_SLOC(<LEVEL> <LIST_VAR> <SEP> [<LABEL>]
#                    [BASE_DIR <path>]
#                    [OUT_FILES <var>] [OUT_LINES <var>]
#                    [OUT_BYTES <var>] [OUT_SLOC <var>])
#
#  • Counts how many files are in <LIST_VAR>.
#  • Sums total physical lines and bytes (characters) across existing files.
#  • Also computes SLOC = non-blank lines that are not full-line '//' comments.
#  • Emits one STATUS message with all figures.
#  • Optionally returns totals via OUT_* variables (PARENT_SCOPE).
# ---------------------------------------------------------------------------
function(CB_file_list_SLOC level list_var sep)
    set(_root "${CMAKE_SOURCE_DIR}")
    cmake_parse_arguments(ARG "" "" "BASE_DIR;OUT_FILES;OUT_LINES;OUT_BYTES;OUT_SLOC" ${ARGN})
    if(ARG_BASE_DIR)
        set(_root "${ARG_BASE_DIR}")
    endif()

    # -------- verify list ----------------------------------------------------
    if(NOT DEFINED ${list_var} OR NOT ${list_var})
        CB_Log(STATUS "'${list_var}' is empty (relative to ${_root})")
        return()
    endif()

    # -------- accumulate -----------------------------------------------------
    set(_file_count   0)
    set(_total_lines  0)
    set(_total_chars  0)
    set(_total_sloc   0)   # new: source lines of code

    foreach(_abs IN LISTS ${list_var})
        if(NOT EXISTS "${_abs}")
            CB_Log(WARNING "Missing file in '${list_var}': ${_abs}")
            continue()
        endif()

        # physical lines
        file(STRINGS "${_abs}" _lines)
        list(LENGTH _lines _n_lines)
        math(EXPR _total_lines "${_total_lines}+${_n_lines}")

        # SLOC: non-blank and not starting with '//' after trimming
        set(_n_sloc 0)
        foreach(_ln IN LISTS _lines)
            string(STRIP "${_ln}" _s)
            if(_s STREQUAL "")
                continue()
            endif()
            if(_s MATCHES "^//")
                continue()
            endif()
            math(EXPR _n_sloc "${_n_sloc}+1")
        endforeach()
        math(EXPR _total_sloc "${_total_sloc}+${_n_sloc}")

        # characters (bytes)
        file(READ "${_abs}" _content)        # read whole file
        string(LENGTH "${_content}" _n_chars)
        math(EXPR _total_chars "${_total_chars}+${_n_chars}")

        math(EXPR _file_count "${_file_count}+1")
    endforeach()



    # -------- log message ----------------------------------------------------
    if(ARGC GREATER 3)            # user provided a 4th positional argument
        set(_list_name "${ARGV3}")
    else()
        set(_list_name "'${list_var}'")
    endif()

    set( _msg
         "${_list_name}${sep}: ${_file_count} files"
         " \t ${_total_sloc} sloc"
    )
    CB_Log(${level} "${_msg}")



    # -------- optional outputs to caller -------------------------------------
    if(ARG_OUT_FILES)
        set(${ARG_OUT_FILES} "${_file_count}" PARENT_SCOPE)
    endif()
    if(ARG_OUT_LINES)
        set(${ARG_OUT_LINES} "${_total_lines}" PARENT_SCOPE)
    endif()
    if(ARG_OUT_BYTES)
        set(${ARG_OUT_BYTES} "${_total_chars}" PARENT_SCOPE)
    endif()
    if(ARG_OUT_SLOC)
        set(${ARG_OUT_SLOC} "${_total_sloc}" PARENT_SCOPE)
    endif()
    
endfunction()



#
#
####################################################################################
####################################################################################    #   END "ADVANCED LOGGING".







####################################################################################
#
#
#
#   3.          BUILDING / OPERATION FUNCTIONS...
####################################################################################
####################################################################################

# ---------------------------------------------------------------------------
#   CB_Apply_Preset(<TARGET> <LIBRARY>)
#
# ---------------------------------------------------------------------------
function(CB_Apply_Preset target preset_lib)
    #   Simple guard so typos fail fast
    if(NOT TARGET "${preset_lib}")
        message(FATAL_ERROR
            "cb_apply_preset(): '${preset_lib}' is not a known target")
    endif()

    #   Link the interface library → target inherits all its compile / link flags
    target_link_libraries("${target}" PRIVATE "${preset_lib}")
endfunction()








####################################################################################
#
#
#
#   3.          THROW-AWAY FUNCTIONS...
####################################################################################
####################################################################################

#   "CB_Test_Logging"
#
function(CB_Test_Logging)
    #   option(CB_SHOW_LOG_TESTS "Display CB_Log() level demo output" ON)
    CB_Log(     DEBUG               "DEBUG level diagnostic                     -- message #(1/8)")
    CB_Log(     VERBOSE             "VERBOSE level diagnostic                   -- message #(2/8)")
    CB_Log(     STATUS              "STATUS level informational                 -- message #(3/8)")
    CB_Log(     AUTHOR_WARNING      "AUTHOR_WARNING level script issue          -- message #(4/8)")
    CB_Log(     WARNING             "WARNING level alert                        -- message #(5/8)")
    CB_Log(     NOTIFY              "WARNING level alert                        -- message #(6/8)")
    CB_Log(     SEND_ERROR          "SEND_ERROR level (configure continues)     -- message #(7/8)")
    CB_Log(     FATAL_ERROR         "FATAL_ERROR level (aborts)                 -- message #(8/8)")  # uncomment to test abort
endfunction()












####################################################################################
####################################################################################    # END.

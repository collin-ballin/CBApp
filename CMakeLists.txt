########################################################################
#
#       *********************************************************************
#       ****          C E N T R A L _ C M A K E ____  F I L E           ****
#       *********************************************************************
#
#              AUTHOR:      Collin A. Bond
#               DATED:      July 02, 2025.
#
#       USAGE:
#           >:      cd root/build/
#           >:      cmake --log-level=DEBUG -G "Xcode" ..
#
########################################################################
########################################################################



#   0.      INCLUDING EXTERNAL CMAKE RESOURCES...
########################################################################
########################################################################
include(${CMAKE_CURRENT_LIST_DIR}/cmake/system/main.cmake)

#   CB_Test_Logging()






########################################################################
#
#
#
#   1.      DEFINE SUPER PROJECT...
########################################################################
########################################################################
cmake_minimum_required(VERSION ${CB_MINIMUM_CMAKE_VERSION} FATAL_ERROR)
project(${CBAPP_NAME} VERSION ${CBAPP_VERSION} LANGUAGES CXX)





########################################################################
#
#
#
#   2.      DECLARE VARIABLES TO DEFINE THE PRIMARY DIRECTORIES...
########################################################################
########################################################################

#           2A.1.   CORE DIRECTORIES:
set(        CB_ROOT_DIR                     "${CMAKE_SOURCE_DIR}"                   )
set(        CB_SRC_DIR                      "${CB_ROOT_DIR}/src"                    )
set(        CB_INCLUDE_DIR                  "${CB_ROOT_DIR}/include"                )
set(        CB_IMGUI_DIR                    "${CB_ROOT_DIR}/imgui"                  )
set(        CB_THIRD_PARTY_DIR              "${CB_ROOT_DIR}/libs"                   )
#
set(        CB_MISC_DIR                     "${CB_ROOT_DIR}/misc"                   )
set(        CB_MISC_CPP_DIR                 "${CB_MISC_DIR}/cpp"                    )
#
#
#
#           2A.2.   CMAKE / BUILD-SYSTEM DIRECTORIES:
set(        CB_CMAKE_DIR                    "${CB_ROOT_DIR}/cmake"                  )
set(        CB_CMAKE_LIB_DIR                "${CB_ROOT_DIR}/cmake/system"           )
set(        CB_CMAKE_MACOS_DIR              "${CB_ROOT_DIR}/cmake/system"           )
set(        CB_CMAKE_WINDOWS_DIR            "${CMAKE_SOURCE_DIR}/macos"             )
#
#
#
#           2A.3.   ADDITIONAL DIRECTORIES:
set(        CB_VERSION_HEADER_DIR           ${CMAKE_BINARY_DIR}/generated           )
#
#
#
#           2A.4.   CBLIB DIRECTORIES:
set(        CBLib_ROOT_DIR                  "${CB_ROOT_DIR}/libs/cblib"             )
set(        CBLib_SRC_DIR                   "${CBLib_ROOT_DIR}/src"                 )
set(        CBLib_TESTS_DIR                 "${CBLib_ROOT_DIR}/tests"               )
set(        CBLib_INCLUDE_DIR               "${CBLib_ROOT_DIR}/include"             )




#           2B.1.   ASSIGNING LIST-OF-FILES FOR THE CMAKE BUILD SYSTEM:
set(        CB_BUILD_SYSTEM_FILES
                ${CMAKE_SOURCE_DIR}/CMakeLists.txt
            #
                ${CB_CMAKE_LIB_DIR}/main.cmake
                ${CB_CMAKE_LIB_DIR}/constants.cmake
                ${CB_CMAKE_LIB_DIR}/project_metadata.cmake
                ${CB_CMAKE_LIB_DIR}/utility.cmake
                ${CB_CMAKE_LIB_DIR}/build.cmake
            #
                ${CB_CMAKE_LIB_DIR}/build.cmake
)

#
####################################################################
########################################################################    END "2".



    
    
    
########################################################################
#
#
#
#   3.      INITIAL LOGGING AND DEBUGGING MESSAGING...
########################################################################
########################################################################

#   CB_newline(2)
CB_Log(NOTIFY   "Building \"${CBAPP_NAME}\" version ${CBAPP_VERSION}...")
CB_newline(2)



CB_Log(STATUS   "INITIALIZING...")

#           3.1.    PROGRAM VERSIONING:
CB_Log(DEBUG    "Current CMake Version              : ${CMAKE_VERSION}  (${CMAKE_COMMAND})")
CB_Log(DEBUG    "Minimum CMake Version Required     : ${CB_MINIMUM_CMAKE_VERSION}")
#
#
#
#           3.2.    MAIN DIRECTORY VARIABLES:
CB_Log(OUTFO    "Main Directory Variables:")
CB_Log(VERBOSE  "CB_ROOT_DIR                        : ${CB_ROOT_DIR}")
CB_Log(VERBOSE  "CB_SRC_DIR                         : ${CB_SRC_DIR}")
CB_Log(VERBOSE  "CB_MISC_DIR                        : ${CB_MISC_DIR}")
CB_Log(VERBOSE  "CB_MISC_CPP_DIR                    : ${CB_MISC_CPP_DIR}")
#
#
#
#           3.3.    CBLIB DIRECTORIES:
CB_Log(OUTFO    "Other Directory Variables:")
CB_Log(VERBOSE  "CB_CMAKE_DIR                       : ${CB_CMAKE_DIR}")
#
#
#
#           3.4.    CBLIB DIRECTORIES:
CB_Log(OUTFO    "CBLib Directory Variables:")
CB_Log(VERBOSE  "CBLib_ROOT_DIR                     : ${CBLib_ROOT_DIR}")
CB_Log(VERBOSE  "CBLib_SRC_DIR                      : ${CBLib_SRC_DIR}")
CB_Log(VERBOSE  "CBLib_INCLUDE_DIR                  : ${CBLib_INCLUDE_DIR}")

#
####################################################################
########################################################################    END "3".
    
    




########################################################################
#
#
#
#   4.      TRANSFERING CONTROL TO SUBSIDIARY CMAKE FILES...
########################################################################
########################################################################

CB_newline(2)
CB_Log(STATUS   "TRANSFERING CONTROL TO SECONDARY CMAKELISTS FILE...")
#
#
#   add_subdirectory(macos)      # later: add_subdirectory(windows)
include(${CMAKE_SOURCE_DIR}/macos/CMakeLists.txt)
    
    
    
#           4.1.    MACOS BUILD.
#   if(CB_TARGET_OS STREQUAL "macos")
#       add_subdirectory(macos)        # native build
#
#           4.2.    WINDOWS/MINGW BUILD.
#   elseif(CB_TARGET_OS STREQUAL "windows")
#     add_subdirectory(win64)        # MinGW-w64 cross-build
#   endif()



#
####################################################################
########################################################################    END "4".
    
    

    


########################################################################
#
#
#
#   5.      BUILD RESULTS AND END-MESSAGING...
########################################################################
########################################################################

CB_newline(2)
CB_Log(STATUS   "POST-BUILD DIAGNOSTICS...")

#           5.1.    CBAPP SOURCE + HEADERS.
CB_file_list_SLOC( INFO             APP_HEADERS             "\t"            "CBApp Header Files"
                   OUT_FILES        APP_HEADERS_FILES       OUT_LINES       APP_HEADERS_LINES
                   OUT_BYTES        APP_HEADERS_BYTES       OUT_SLOC        APP_HEADERS_SLOC )

CB_file_list_SLOC( INFO             APP_SRCS                "\t"            "CBApp Source Files"
                   OUT_FILES        APP_SRCS_FILES          OUT_LINES       APP_SRCS_LINES
                   OUT_BYTES        APP_SRCS_BYTES          OUT_SLOC        APP_SRCS_SLOC )



#           5.2.    CBLib SOURCE + HEADERS.
CB_file_list_SLOC( VERBOSE          CBLib_HEADERS           "\t"            "CBLib Header Files"
                   OUT_FILES        CBLib_HEADERS_FILES     OUT_LINES       CBLib_HEADERS_LINES
                   OUT_BYTES        CBLib_HEADERS_BYTES     OUT_SLOC        CBLib_HEADERS_SLOC )

CB_file_list_SLOC( VERBOSE          CBLib_SRCS              "\t"            "CBLib Source Files"
                   OUT_FILES        CBLib_SRCS_FILES        OUT_LINES       CBLib_SRCS_LINES
                   OUT_BYTES        CBLib_SRCS_BYTES        OUT_SLOC        CBLib_SRCS_SLOC )
                   




#
#       CB_file_list_info(DEBUG     IMGUI_HEADERS   "\t\t")
#       #   print_file_list(IMGUI_HEADERS)
#
#       CB_file_list_info(DEBUG     IMGUI_SRCS      "\t\t")
#       #   print_file_list(IMGUI_SRCS)
#

#
#       print_file_list(CBLib_SRCS)
#       #   print_file_list(CBLib_SRCS)
#
#       print_file_list(CBLib_HEADERS)
#       #   print_file_list(CBLib_HEADERS)
#






#   FINAL PRINT STATEMENT...
#
#       Validate List Variables.
#   if(NOT DEFINED ${list_var} OR NOT ${list_var})
#       # Ensure OUT_* are defined even on early return
#       if(ARG_OUT_FILES) set(${ARG_OUT_FILES} 0 PARENT_SCOPE) endif()
#       if(ARG_OUT_LINES) set(${ARG_OUT_LINES} 0 PARENT_SCOPE) endif()
#       if(ARG_OUT_BYTES) set(${ARG_OUT_BYTES} 0 PARENT_SCOPE) endif()
#       if(ARG_OUT_SLOC)  set(${ARG_OUT_SLOC}  0 PARENT_SCOPE) endif()
#
#       CB_Log(STATUS "'${list_var}' is empty (relative to ${_root})")
#       return()
#   endif()
#
#       Aggregate Values Across Each List.
#   math(   EXPR    PROJ_FILES      "${APP_HEADERS_FILES}   + ${APP_SRCS_FILES} + ${CBLIB_HEADERS_FILES}    + ${CBLIB_SRCS_FILES}"      )
#   math(   EXPR    PROJ_LINES      "${APP_HEADERS_LINES}   + ${APP_SRCS_LINES} + ${CBLIB_HEADERS_LINES}    + ${CBLIB_SRCS_LINES}"      )
#   math(   EXPR    PROJ_BYTES      "${APP_HEADERS_BYTES}   + ${APP_SRCS_BYTES} + ${CBLIB_HEADERS_BYTES}    + ${CBLIB_SRCS_BYTES}"      )
#   math(   EXPR    PROJ_SLOC       "${APP_HEADERS_SLOC}    + ${APP_SRCS_SLOC}  + ${CBLIB_HEADERS_SLOC}     + ${CBLIB_SRCS_SLOC}"       )
#   CB_Log(INFO "Project Totals\t: contains ${PROJ_FILES} file(s); (${PROJ_LINES} lines; ${PROJ_BYTES} characters; ${PROJ_SLOC} sloc)")





#   FINAL PRINT STATEMENT...
#
CB_newline(2)
CB_Log(NOTIFY   "build complete")

#
####################################################################
########################################################################    END "5".












########################################################################
########################################################################   END...

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
project(PROJECT_NAME VERSION ${CBAPP_VERSION} LANGUAGES CXX)





########################################################################
#
#
#
#   2.      DECLARE VARIABLES TO DEFINE THE PRIMARY DIRECTORIES...
########################################################################
########################################################################

#           2A.1.   CORE DIRECTORIES:
set(        CB_ROOT_DIR                 "${CMAKE_SOURCE_DIR}"               CACHE PATH "")
set(        CB_SRC_DIR                  "${CB_ROOT_DIR}/src"                CACHE PATH "")
set(        CB_INCLUDE_DIR              "${CB_ROOT_DIR}/include"            CACHE PATH "")
set(        CB_IMGUI_DIR                "${CB_ROOT_DIR}/imgui"              CACHE PATH "")
set(        CB_THIRD_PARTY_DIR          "${CB_ROOT_DIR}/libs"               CACHE PATH "")
#
set(        CB_MISC_DIR                 "${CB_ROOT_DIR}/misc"               CACHE PATH "")
set(        CB_MISC_CPP_DIR             "${CB_MISC_DIR}/cpp"                CACHE PATH "")
#
#
#
#           2A.2.   CMAKE / BUILD-SYSTEM DIRECTORIES:
set(        CB_CMAKE_DIR                "${CB_ROOT_DIR}/cmake"              CACHE PATH  "Path to main CMake resource directory (version.hpp.in, version.rc.in)"             )
set(        CB_CMAKE_LIB_DIR            "${CB_ROOT_DIR}/cmake/system"       CACHE PATH  "Directory containing the \"*.cmake\" files used for the CBApp build system."       )
set(        CB_CMAKE_MACOS_DIR          "${CB_ROOT_DIR}/cmake/system"       CACHE PATH  "Directory containing *.cmake for MacOS build system."                              )
set(        CB_CMAKE_WINDOWS_DIR        "${CMAKE_SOURCE_DIR}/macos"         CACHE PATH  "Directory containing *.cmake for Windows/MinGW build system."                      )
#
#
#
#           2A.3.   ADDITIONAL DIRECTORIES:
set(        CB_VERSION_HEADER_DIR       ${CMAKE_BINARY_DIR}/generated       CACHE PATH  "Directory containing \"version.hpp\" file.")




#           2B.1.   ASSIGNING LIST-OF-FILES FOR THE CMAKE BUILD SYSTEM:
set(        CB_BUILD_SYSTEM_FILES
                ${CMAKE_SOURCE_DIR}/CMakeLists.txt
)




#           2.4.    SOURCE FILES:
#               APP_SRCS
#
#           2.5.    HEADER FILES:
#               APP_HEADERS
    
    
    
    
    
    
########################################################################
#
#
#
#   3.      INITIAL LOGGING AND DEBUGGING MESSAGING...
########################################################################
########################################################################

#   CB_newline(2)
CB_Log(NOTIFY   "BUILDING ${CBAPP_NAME} ${CBAPP_VERSION}...")
CB_newline(2)



CB_Log(NOTIFY   "INITIALIZING...")



CB_Log(DEBUG    "Current CMake Version              : ${CMAKE_VERSION}  (${CMAKE_COMMAND})")
CB_Log(DEBUG    "Minimum CMake Version Required     : ${CB_MINIMUM_CMAKE_VERSION}")
#
#
CB_Log(INFO     "Main Directory Variables:")
CB_Log(VERBOSE  "CB_ROOT_DIR                        : ${CB_ROOT_DIR}")
CB_Log(VERBOSE  "CB_SRC_DIR                         : ${CB_SRC_DIR}")
CB_Log(VERBOSE  "CB_MISC_DIR                        : ${CB_MISC_DIR}")
CB_Log(VERBOSE  "CB_MISC_CPP_DIR                    : ${CB_MISC_CPP_DIR}")
#
#
CB_Log(INFO     "Other Directory Variables:")
#
CB_Log(VERBOSE  "CB_CMAKE_DIR                       : ${CB_CMAKE_DIR}")

    
    
    


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
    
    
    
    
    
    
    
    
    
    
    
########################################################################
#
#
#   4.      ...
########################################################################
########################################################################

#           4.1.    MACOS BUILD.
#   if(CB_TARGET_OS STREQUAL "macos")
#       add_subdirectory(macos)        # native build
#
#           4.2.    WINDOWS/MINGW BUILD.
#   elseif(CB_TARGET_OS STREQUAL "windows")
#     add_subdirectory(win64)        # MinGW-w64 cross-build
#   endif()
    
    
    
    
    
    
    
    
    
    
    
########################################################################
#
#
#   5.      BUILD RESULTS AND END-MESSAGING...
########################################################################
########################################################################

CB_newline(2)
CB_Log(NOTIFY   "POST-BUILD DIAGNOSTICS...")

CB_file_list_info(STATUS    APP_SRCS        "\t"      "CBApp Header Files")
CB_file_list_info(STATUS    APP_HEADERS     "\t"      "CBApp Source Files")


#
#
#   CB_file_list_info(DEBUG     IMGUI_SRCS      "\t\t")
#   CB_file_list_info(DEBUG     IMGUI_SRCS      "\t\t")
#
#


print_file_list(CB_BUILD_SYSTEM_FILES)










CB_newline(2)
CB_Log(NOTIFY   "done")
########################################################################
########################################################################   END...

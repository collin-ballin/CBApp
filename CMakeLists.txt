########################################################################
#
#       *********************************************************************
#       ****          C E N T R A L _ C M A K E ____  F I L E           ****
#       *********************************************************************
#
#              AUTHOR:      Collin A. Bond
#               DATED:      July 02, 2025.
#
#
########################################################################
########################################################################



#   0.  INCLUDING EXTERNAL CMAKE RESOURCES...
########################################################################
########################################################################

include(${CMAKE_CURRENT_LIST_DIR}/cmake/system/cbapp_cmake.cmake)
#
# include(${CMAKE_CURRENT_LIST_DIR}/cmake/project_meta.cmake)
# include(${CMAKE_CURRENT_LIST_DIR}/cmake/utility.cmake)






########################################################################
#
#
#
#   1.  DEFINE SUPER PROJECT...
########################################################################
########################################################################
cmake_minimum_required(VERSION ${CB_MINIMUM_CMAKE_VERSION} FATAL_ERROR)
#   project(cbapp_superbuild)
project(CBApp VERSION ${CBAPP_VERSION} LANGUAGES CXX)





########################################################################
#
#
#
#   2.  DECLARE VARIABLES TO DEFINE THE PRIMARY DIRECTORIES...
########################################################################
########################################################################

#       2.1.    CORE DIRECTORIES:
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
#       2.2.    ADDITIONAL DIRECTORIES:
set(        CB_CMAKE_DIR                "${CB_ROOT_DIR}/cmake"
            CACHE PATH                  "Path to shared cmake templates (version.hpp.in, version.rc.in)")
#
set(        CB_VERSION_HEADER_DIR       ${CMAKE_BINARY_DIR}/generated)      #   Directory of the "version.hpp"
    
    
    
    
    
    
########################################################################
#
#
#
#   3.  INITIAL LOGGING AND DEBUGGING MESSAGING...
########################################################################
########################################################################

CB_Log(STATUS "\n\n\nCENTRAL CMAKE FILE.  BUILDING \"${CBAPP_NAME} VERSION ${CBAPP_VERSION}\"...")
#
CB_Log(STATUS "Current CMake Version                : ${CMAKE_VERSION}  (${CMAKE_COMMAND})")
CB_Log(STATUS "Minimum CMake Version Required       : ${CB_MINIMUM_CMAKE_VERSION}")
#
#
CB_Log(STATUS "\nMAIN DIRECTORIES...")
CB_Log(STATUS "CB_ROOT_DIR                          : ${CB_ROOT_DIR}")
CB_Log(STATUS "CB_SRC_DIR                           : ${CB_SRC_DIR}")
CB_Log(STATUS "CB_MISC_DIR                          : ${CB_MISC_DIR}")
CB_Log(STATUS "CB_MISC_CPP_DIR                      : ${CB_MISC_CPP_DIR}")
#
#
CB_Log(STATUS "\nOTHER DIRECTORIES...")
#
CB_Log(STATUS "CB_CMAKE_DIR                         : ${CB_CMAKE_DIR}")



CB_Log(STATUS "\n\nBEGINNING BUILD...\n")
    
    
    
    
    
########################################################################
#
#
#
#   4.  TRANSFERING CONTROL TO SUBSIDIARY CMAKE FILES...
########################################################################
########################################################################

#   add_subdirectory(macos)      # later: add_subdirectory(windows)
include(${CMAKE_SOURCE_DIR}/macos/CMakeLists.txt)
    
    
    
    
    
    
    
    
    
    
    
########################################################################
#
#
#   4.  ...
########################################################################
########################################################################

#       4.1.    MACOS BUILD.
#   if(CB_TARGET_OS STREQUAL "macos")
#       add_subdirectory(macos)        # native build
#
#       4.2.    WINDOWS/MINGW BUILD.
#   elseif(CB_TARGET_OS STREQUAL "windows")
#     add_subdirectory(win64)        # MinGW-w64 cross-build
#   endif()














########################################################################
########################################################################   END...

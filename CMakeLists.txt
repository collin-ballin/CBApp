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
cmake_minimum_required(VERSION 3.10)



#   1.  DEFINE SUPER PROJECT...
########################################################################
########################################################################
project(cbapp_superbuild)






########################################################################
#
#
#   2.  CREATE VARIABLES TO DEFINE THE PRIMARY DIRECTORIES...
########################################################################
########################################################################

set(CB_ROOT_DIR         "${CMAKE_SOURCE_DIR}"            CACHE PATH "")
set(CB_SRC_DIR          "${CB_ROOT_DIR}/src"             CACHE PATH "")
set(CB_INCLUDE_DIR      "${CB_ROOT_DIR}/include"         CACHE PATH "")
set(CB_IMGUI_DIR        "${CB_ROOT_DIR}/imgui"           CACHE PATH "")
set(CB_THIRD_PARTY_DIR  "${CB_ROOT_DIR}/libs"            CACHE PATH "")
set(CB_CMAKE_DIR        "${CB_ROOT_DIR}/cmake"           CACHE PATH "")



set(CB_CMAKE_DIR "${CMAKE_SOURCE_DIR}/cmake"        CACHE PATH
    "Path to shared cmake templates (version.hpp.in, version.rc.in)")
    
    
    
    
    
    
########################################################################
#
#
#   3.  INVOKE SUBSIDIARY CMAKE FILES...
########################################################################
########################################################################
add_subdirectory(macos)      # later: add_subdirectory(windows)
    
    
    
    
    
    
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
########################################################################
#   END...

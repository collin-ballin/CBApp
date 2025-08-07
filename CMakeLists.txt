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
#
########################################################################
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
#
########################################################################
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
#
########################################################################
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

CB_file_list_info(INFO      APP_SRCS        "\t"      "CBApp Header Files")
CB_file_list_info(INFO      APP_HEADERS     "\t"      "CBApp Source Files")


#
#
#   CB_file_list_info(DEBUG     IMGUI_SRCS      "\t\t")
#   CB_file_list_info(DEBUG     IMGUI_SRCS      "\t\t")
#
#


# print_file_list(CB_BUILD_SYSTEM_FILES)



CB_newline(2)
CB_Log(NOTIFY   "build complete")

#
#
########################################################################
########################################################################    END "5".












########################################################################
########################################################################   END...

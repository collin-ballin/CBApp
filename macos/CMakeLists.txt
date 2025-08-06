########################################################################
#
#       *********************************************************************
#       ****           M A C _ O S _ C M A K E _____  F I L E            ****
#       *********************************************************************
#
#              AUTHOR:      Collin A. Bond
#               DATED:      July 02, 2025.
#             VERSION:      V2.2
#
#
#       *********************************************************************
#       INFO:
#                                   BUILD FILE FOR:       "macOS"
#                           Project Root Directory:       imgui/imgui_V0/
#
#       INVOCATIONS:
#                                           cmake --build . --config Debug
#                                           cmake --build . --config Release
#
#       HOW-TO:
#                         "cd" change-directory to:       imgui/CBApp/build
#                            then, run the command:       cmake -G "Xcode" ..
#                                               OR:       cmake -G "Xcode" -DCBLIB_DEBUG=ON ..
#
########################################################################
########################################################################
cmake_minimum_required(VERSION ${CB_MINIMUM_CMAKE_VERSION} FATAL_ERROR)



#   0.  INITIAL PROJECT SETUP...
########################################################################
########################################################################
if(NOT PROJECT_NAME STREQUAL ${CBAPP_NAME})
    CB_Log(FATAL_ERROR "Project initiation \"project(${CBAPP_NAME}, ...)\" was NOT performed by the central CMakeLists.txt file.")
    project(${CBAPP_NAME} VERSION ${CBAPP_VERSION} LANGUAGES CXX)
endif()


#   Derive a monotonically increasing build number (Git commit count).
#       - Fallback to 1 if Git is unavailable (e.g. first CI checkout).
execute_process(
    COMMAND git rev-list --count HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_COUNT
    ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
if(GIT_COMMIT_COUNT STREQUAL "")
    set(GIT_COMMIT_COUNT 1)
endif()
set(BUILD_NUMBER ${GIT_COMMIT_COUNT})


if(NOT EXISTS "${CB_CMAKE_DIR}/version.hpp.in")
    message(FATAL_ERROR "Template not found: ${CB_CMAKE_DIR}/version.hpp.in")
endif()

#
#
#
########################################################################
########################################################################    END "INITIAL PROJECT SETUP".






########################################################################
#
#
#
#   2.  GATHER PROJECT FILES AND RESOURCES...
########################################################################
########################################################################

################################################################
#       2.1.    GATHER FILES FOR ALL USAGE (Common)...
################################################################
#               1.  Find required packages: OpenGL and GLFW.
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

#               2.  Aggregate each include directory path.
set(CBAPP_INCLUDE_DIRS
    ${CB_INCLUDE_DIR}
    ${CB_INCLUDE_DIR}/config
    ${CB_INCLUDE_DIR}/app
    ${CB_INCLUDE_DIR}/imgui_extensions
    ${CB_IMGUI_DIR}
    ${CB_IMGUI_DIR}/backends
    ${CB_MISC_CPP_DIR}
    ${THIRD_PARTY_INCLUDE_DIR}
    ${CB_VERSION_HEADER_DIR}
)


#
#
################################################################
################################################################    END 2.1.




################################################################
#       2.2.    FILES FOR LIB #1 (Dear ImGui)...
################################################################
#                   Collect ImGui core source files from the imgui/directory.
set(IMGUI_CORE_SRCS
    ${CB_IMGUI_DIR}/imgui.cpp                   #     ImGui...
    ${CB_IMGUI_DIR}/imgui_draw.cpp
    ${CB_IMGUI_DIR}/imgui_widgets.cpp
    ${CB_IMGUI_DIR}/imgui_tables.cpp
    ${CB_IMGUI_DIR}/implot.cpp                  #     ImPlot...
    ${CB_IMGUI_DIR}/implot_items.cpp
    ${CB_IMGUI_DIR}/implot_demo.cpp             #     Optional.
    ${CB_IMGUI_DIR}/imgui_demo.cpp              #     Optional.
)


#                   Collect Misc ImGui source files.
set(IMGUI_MISC_SRCS
    ${CB_MISC_CPP_DIR}/imgui_stdlib.cpp
)


#                   Collect only the backends you need (GLFW and OpenGL3).
set(IMGUI_BACKEND_SRCS
    ${CB_IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${CB_IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)


#                   Combine all ImGui source files.
set(IMGUI_SRCS ${IMGUI_CORE_SRCS} ${IMGUI_MISC_SRCS} ${IMGUI_BACKEND_SRCS})

#
#
################################################################
################################################################    END 2.2.




################################################################
#       2.3.    FILES FOR LIB #2 (CBLib)...
################################################################
#   #
#   #   ...
#   #

#
#
################################################################
################################################################    END 2.3.




################################################################
#       2.X.    FILES FOR MAIN EXECUTABLE (CBApp)...
################################################################
#               Recursively Gather CBApp Source Files.
file(GLOB_RECURSE APP_SRCS
    CONFIGURE_DEPENDS
        ${CB_SRC_DIR}/*.cpp
)


#               Recursively Gather CBApp Header Files.
file(GLOB_RECURSE APP_HEADERS
    ${CB_INCLUDE_DIR}/*.h
    ${CB_INCLUDE_DIR}/**/*.h
)


#               Third party included libraries...
set(THIRD_PARTY_INCLUDE_DIR
    ${CB_THIRD_PARTY_DIR}/cblib/include
    ${CB_THIRD_PARTY_DIR}/nlohmann/include
)

#
#
################################################################
################################################################    END 2.X.



#
#
#
########################################################################
########################################################################    END "GATHERING RESOURCES".






########################################################################
#
#
#
#   3.  LIBRARY #1.     DEAR IMGUI SOURCE CODE...
########################################################################
########################################################################

#       3.1.    Compile Dear ImGui Source-Code SEPERATELY.
add_library(ImGui STATIC ${IMGUI_SRCS})


#       3.2.    Treat ImGui’s own headers as “system” and silence warnings.
target_include_directories(ImGui PRIVATE ${CBAPP_INCLUDE_DIRS})     #   Add GLFW inside Dear ImGui build...


#       3.3.    Add GLFW inside Dear ImGui build.
#                   - Provide compiler flag for "my_imconfig.h" to ImGui Source code.
#                   - Set the "-w" compiler flag to disable all compiler flags (compile as "system").
target_link_libraries(ImGui
    PUBLIC glfw LIB_cbapp_config_defines -w
)


#       3.4.    Set MacOS Minimum Version for ImGui Source Code Build.
set_target_properties(ImGui PROPERTIES
    XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET                "${CBAPP_MIN_MACOS_VERSION}"
)

#
#
#
########################################################################
########################################################################    END "LIBRARY #1".






########################################################################
#
#
#
#   4.  LIBRARY #2.     CBLIB TEMPLATE LIBRARY...
########################################################################
########################################################################

#   NEED:
#       - CBLIB_SRCS
#       - CBLIB_HEADERS
#       - CBLIB_INCLUDE_DIRS
#


#       4.1.    Compile Dear ImGui Source-Code SEPERATELY.
#   #   add_library(CBLib STATIC ${CBLIB_SRCS})


#       4.2.    Treat ImGui’s own headers as “system” and silence warnings.
#   #   target_include_directories(ImGui PRIVATE ${CBAPP_INCLUDE_DIRS})     #   Add GLFW inside Dear ImGui build...


#       4.3.    Add GLFW inside Dear ImGui build.
#                   - Provide compiler flag for "my_imconfig.h" to ImGui Source code.
#                   - Set the "-w" compiler flag to disable all compiler flags (compile as "system").
#   #   target_link_libraries(ImGui
#   #       PUBLIC glfw LIB_cbapp_config_defines -w
#   #   )


#       4.4.    Set MacOS Minimum Version for ImGui Source Code Build.
#   #   set_target_properties(ImGui PROPERTIES
#   #       XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET                "${CBAPP_MIN_MACOS_VERSION}"
#   #   )

#
#
#
########################################################################
########################################################################    END "LIBRARY #2".






########################################################################
#
#
#
#   X.  CREATE MAIN EXECUTIBLE
#       (MAP ATTRIBUTES, ADD INCLUDE-DIRECTORES, SET PROPERTIES, ETC)...
########################################################################
########################################################################



#       X.4.    Create the executable from your application and ImGui source files
add_executable(${PROJECT_NAME}
    ${APP_SRCS}
    ${APP_HEADERS}
)


#       X.5.    Link necessary libraries:   OpenGL, GLFW, ImGui, Compiler-Flags.
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${OPENGL_gl_LIBRARY}
        glfw
        ImGui
    #
        LIB_preprocessor_defines
        LIB_cxx_error_flags
        LIB_cxx_warning_flags
        LIB_cxx_debug_flags
)


#       X.6.    Link necessary libraries:   OpenGL, GLFW, ImGui, Compiler-Flags.
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CB_INCLUDE_DIR}
        ${CB_INCLUDE_DIR}/config
        ${CB_INCLUDE_DIR}/app
        ${CB_INCLUDE_DIR}/imgui_extensions
        ${CB_IMGUI_DIR}
        ${CB_IMGUI_DIR}/backends
        ${CB_MISC_CPP_DIR}
        ${THIRD_PARTY_INCLUDE_DIR}
        ${CB_VERSION_HEADER_DIR}
)


#       X.7     Xcode‑specific attributes **including version info**
CB_apply_xcode_target_properties(${PROJECT_NAME})

#
#
#
########################################################################
########################################################################    END "MAIN EXECUTABLE".













########################################################################
#
#
#
#   I.  CONFIGURE THE GENERATED XCODE PROJECT FILE...
########################################################################
########################################################################

#       I.i.    Auto‑increment build number during local Xcode builds (optional).
option(USE_AGVTOOL_AUTO_BUMP "Run agvtool before each Xcode build" OFF)
#
if(CMAKE_GENERATOR STREQUAL "Xcode" AND USE_AGVTOOL_AUTO_BUMP)
    add_custom_command(TARGET ${PROJECT_NAME}
        PRE_BUILD
        COMMAND /usr/bin/xcrun agvtool new-version -all ${BUILD_NUMBER} || true
        WORKING_DIRECTORY ${CB_ROOT_DIR}
        COMMENT "Optional: bump CFBundleVersion via agvtool")
endif()


#       I.ii.   Generate `version.hpp` so code can query the version.
set(CB_VERSION_HEADER_DIR ${CMAKE_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${CB_VERSION_HEADER_DIR})
configure_file(
    ${CB_CMAKE_DIR}/version.hpp.in          # ← always correct
    ${CB_VERSION_HEADER_DIR}/version.hpp
    @ONLY)




#       II.i.   Create Folder for "CBApp Source Code" in Xcode.
source_group(
    TREE   ${CB_SRC_DIR}
    PREFIX "src"
    FILES  ${APP_SRCS}
)


#       II.ii.  Create Folder for "CBApp Included Code" in Xcode.
source_group(
    TREE   ${CB_INCLUDE_DIR}
    PREFIX "include"
    FILES  ${APP_HEADERS}
)


#       II.iii  Create "src" File Group in Xcode.
source_group(TREE ${CB_SRC_DIR} PREFIX "src" FILES ${APP_SRCS})
source_group(
    TREE   ${CB_IMGUI_DIR}
    PREFIX "imgui"
    FILES  ${IMGUI_CORE_SRCS} ${IMGUI_BACKEND_SRCS}
)
#
#               Extra group just for the misc file
source_group(
    TREE   ${CB_MISC_DIR}/          # its real location
    PREFIX "imgui/misc"             # how it should appear
    FILES  ${IMGUI_MISC_SRCS}
)






#
#
#
########################################################################
########################################################################    END "XCODE PROJECT SETUP".






















########################################################################
#
#
#
#   6.  BUILD EXECUTABLES FOR MY "CB_LIB" TEMPLATE LIBRARY...
#           --THIS BRANCH IS NEVER TAKEN.  IT IS A PLACEGOLDER ITEM.        - AUGUST 5th, 2025.
########################################################################
########################################################################
if (BUILD_CBLIB)
    
    CB_Log(STATUS "\n\nBUILDING \"CBLIB\"...")
    
    #   Gather all .cpp test drivers under libs/cblib/tests/
    file(GLOB_RECURSE LIBCBAPP_USE_VIEWPORT_SRCS ${CB_THIRD_PARTY_DIR}/cblib/tests/*.cpp)
        
    #   Define the test target
    add_executable(cblib_tests ${LIBCBAPP_USE_VIEWPORT_SRCS})

    #   Provide include access to cblib headers
    target_include_directories(cblib_tests PRIVATE
        ${THIRD_PARTY_INCLUDE_DIR}
    )

    #   Optional strict compiler flags for testing
    target_compile_options(cblib_tests PRIVATE -Wall -Wextra -pedantic)

    #   Group test sources in Xcode project navigator
    source_group(TREE ${CB_THIRD_PARTY_DIR}/cblib/tests PREFIX "cblib/tests" FILES ${LIBCBAPP_USE_VIEWPORT_SRCS})
endif()











########################################################################
########################################################################
#   END...

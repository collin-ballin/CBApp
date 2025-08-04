########################################################################
#
#       *********************************************************************
#       ****           M A C _ O S _ C M A K E _____  F I L E            ****
#       *********************************************************************
#
#              AUTHOR:      Collin A. Bond
#               DATED:      July 02, 2025.
#             VERSION:      V2.1
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
# project(CBApp)



#   0.  PROJECT + GLOBAL VERSION...
########################################################################
########################################################################

#   Bump the semantic version here when you cut a *feature* release.
#   project(CBApp VERSION ${CBAPP_VERSION} LANGUAGES CXX)

if(NOT PROJECT_NAME STREQUAL "CBApp")
    project(CBApp VERSION ${CBAPP_VERSION} LANGUAGES CXX)
endif()



#   Derive a monotonically increasing build number (Git commit count).
#   Fallback to 1 if Git is unavailable (e.g. first CI checkout).
execute_process(
    COMMAND git rev-list --count HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_COUNT
    ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
if(GIT_COMMIT_COUNT STREQUAL "")
    set(GIT_COMMIT_COUNT 1)
endif()
set(BUILD_NUMBER ${GIT_COMMIT_COUNT})






message(DEBUG "CMAKE_SOURCE_DIR                = ${CMAKE_SOURCE_DIR}")
message(DEBUG "CMAKE_CURRENT_SOURCE_DIR        = ${CMAKE_CURRENT_SOURCE_DIR}")
message(DEBUG "CMAKE_SOURCE_DIR                = ${CMAKE_SOURCE_DIR}")
message(DEBUG "CMAKE_BINARY_DIR                = ${CMAKE_BINARY_DIR}")
message(DEBUG "CMAKE_CURRENT_BINARY_DIR        = ${CMAKE_CURRENT_BINARY_DIR}")

if(NOT EXISTS "${CB_CMAKE_DIR}/version.hpp.in")
    message(FATAL_ERROR "Template not found: ${CB_CMAKE_DIR}/version.hpp.in")
endif()





########################################################################
#
#
#
#   1.  PREPROCESSOR MACRO DEFINITIONS PER CONFIGURATION...
########################################################################
########################################################################
#   add_library(cb_defines INTERFACE)
#   target_compile_definitions(cb_defines INTERFACE
#
#       ...



########################################################################
#
#
#
#   2.      COMPILER FLAGS, GLOBAL SETTINGS, VERSION REQUIREMENTS, ETC...
########################################################################
########################################################################
#           2.1     Set the C++ standard (adjust if needed)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
#
#           2.2     Set the Minimum MacOSX Version...
set(CMAKE_OSX_DEPLOYMENT_TARGET "13.3" CACHE STRING "Minimum macOS deployment version")

#           2.3     Warnings policy---apply only to our main executable
add_library(cb_warnings INTERFACE)
target_compile_options(cb_warnings INTERFACE
    -Wall -Wextra -Wpedantic -Wshadow
    -Wno-sign-compare
    #-Wconversion -Wsign-conversion
)



########################################################################
#
#
#
#   4.      GETTING ALL THE FILES / "DEAR IMGUI" - SPECIFIC STUFF...
########################################################################
########################################################################
#           4.1     Find required packages: OpenGL and GLFW.
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
#
#
#           4.2     Collect ImGui core source files from the imgui/directory.
set(IMGUI_CORE_SRCS
    ${CB_IMGUI_DIR}/imgui.cpp             #   ImGui...
    ${CB_IMGUI_DIR}/imgui_draw.cpp
    ${CB_IMGUI_DIR}/imgui_widgets.cpp
    ${CB_IMGUI_DIR}/imgui_tables.cpp
    ${CB_IMGUI_DIR}/implot.cpp            #   ImPlot...
    ${CB_IMGUI_DIR}/implot_items.cpp
    ${CB_IMGUI_DIR}/implot_demo.cpp       # Optional
    ${CB_IMGUI_DIR}/imgui_demo.cpp        # Optional
)
#
#       4.3     Collect Misc ImGui source files.
set(IMGUI_MISC_SRCS
    ${CB_MISC_CPP_DIR}/imgui_stdlib.cpp
)
#
#       4.4     Collect only the backends you need (GLFW and OpenGL3).
set(IMGUI_BACKEND_SRCS
    ${CB_IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${CB_IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)
#
#       4.5     Combine all ImGui source files.
set(IMGUI_SRCS ${IMGUI_CORE_SRCS} ${IMGUI_MISC_SRCS} ${IMGUI_BACKEND_SRCS})
#
#       4.6     Recursively gather your application source files.
file(GLOB_RECURSE APP_SRCS
    CONFIGURE_DEPENDS
    ${CB_SRC_DIR}/*.cpp
)
#
#       Optional:   add header globbing and grouping (for future use):
#   file(GLOB_RECURSE APP_HEADERS
#       ${CB_INCLUDE_DIR}/include/*.h
#       ${CB_INCLUDE_DIR}/include/**/*.h
#   )
#
file(GLOB_RECURSE APP_HEADERS
    ${CB_INCLUDE_DIR}/*.h
    ${CB_INCLUDE_DIR}/**/*.h
)
source_group(TREE ${CB_INCLUDE_DIR} PREFIX "include" FILES ${APP_HEADERS})




########################################################################
#
#
#
#   5.  FINAL OPERATIONS (DEFINE EXE, LINKING BINARIES, ETC)...
########################################################################
########################################################################


#   print_file_list(APP_SRCS)
#   print_file_list(APP_HEADERS)
#   print_file_list(IMGUI_SRCS)


#       5.2     Create the executable from your application and ImGui source files
add_executable(${PROJECT_NAME}
    ${APP_SRCS}
    ${IMGUI_SRCS}
    ${APP_HEADERS}
)

#       5.3     Link necessary libraries: OpenGL, GLFW, and warnings interface.
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${OPENGL_gl_LIBRARY}
        glfw
        cb_warnings
        LIB_preprocessor_defines
)



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

#       5.4     Xcode‑specific attributes **including version info**
set_target_properties(${PROJECT_NAME} PROPERTIES
    XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "13.3"
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.example.cbapp"
    XCODE_ATTRIBUTE_MARKETING_VERSION ${PROJECT_VERSION}
    XCODE_ATTRIBUTE_CURRENT_PROJECT_VERSION ${BUILD_NUMBER}
    XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "-"
    XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME "NO")

#       5.5     Auto‑increment build number during local Xcode builds (optional)
option(USE_AGVTOOL_AUTO_BUMP "Run agvtool before each Xcode build" OFF)

if(CMAKE_GENERATOR STREQUAL "Xcode" AND USE_AGVTOOL_AUTO_BUMP)
    add_custom_command(TARGET ${PROJECT_NAME}
        PRE_BUILD
        COMMAND /usr/bin/xcrun agvtool new-version -all ${BUILD_NUMBER} || true
        WORKING_DIRECTORY ${CB_ROOT_DIR}
        COMMENT "Optional: bump CFBundleVersion via agvtool")
endif()






########################################################################
#
#
#   TESTING...
########################################################################
########################################################################


# print_file_list(APP_HEADERS)



#   CB_Log(STATUS   "Variable CB_INCLUDE_DIR    = ${CB_INCLUDE_DIR}")
#   CB_Log(STATUS   "Variable APP_HEADERS       = ${APP_HEADERS}")






########################################################################
########################################################################









#       5.6     Generate `version.hpp` so code can query the version
#   set(VERSION_HEADER_DIR ${CMAKE_BINARY_DIR}/generated)
#   file(MAKE_DIRECTORY ${VERSION_HEADER_DIR})
#   configure_file(
#       ${CB_CMAKE_DIR}/version.hpp.in          # ← always correct
#       ${VERSION_HEADER_DIR}/version.hpp
#       @ONLY)
#
set(CB_VERSION_HEADER_DIR ${CMAKE_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${CB_VERSION_HEADER_DIR})
configure_file(
    ${CB_CMAKE_DIR}/version.hpp.in          # ← always correct
    ${CB_VERSION_HEADER_DIR}/version.hpp
    @ONLY)


#       5.7     Organize source files in Xcode
# source_group(TREE ${CB_SRC_DIR} PREFIX "src" FILES ${APP_SRCS})
# source_group(TREE ${CB_IMGUI_DIR} PREFIX "imgui" FILES ${IMGUI_SRCS})

source_group(TREE ${CB_SRC_DIR} PREFIX "src" FILES ${APP_SRCS})
source_group(
    TREE   ${CB_IMGUI_DIR}
    PREFIX "imgui"
    FILES  ${IMGUI_CORE_SRCS} ${IMGUI_BACKEND_SRCS}
)
#               Extra group just for the misc file
source_group(
    TREE   ${CB_MISC_DIR}/          # its real location
    PREFIX "imgui/misc"             # how it should appear
    FILES  ${IMGUI_MISC_SRCS}
)








########################################################################
#
#
#
#   6.  BUILD EXECUTABLES FOR MY "CB_LIB" TEMPLATE LIBRARY...
########################################################################
########################################################################
if (BUILD_CB_LIBCBAPP_USE_VIEWPORTS)
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
#
#
#
#   ?.  DEBUG MESSAGING AND INFO...
########################################################################
########################################################################

#   MY OWN SOURCE CODE...
#       message(STATUS      "APP_SRCS                       = ${APP_SRCS}")
#       message(STATUS      "APP_HEADERS                    = ${APP_HEADERS}")
#
#
#   IMGUI SOURCE CODE...
#       message(DEBUG       "IMGUI_CORE_SRCS                = ${IMGUI_CORE_SRCS}")
#       message(DEBUG       "IMGUI_MISC_SRCS                = ${IMGUI_MISC_SRCS}")






########################################################################
########################################################################
#   END...

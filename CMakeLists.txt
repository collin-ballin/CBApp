########################################################################
#
#   ||====================================================||
#   ||  CMAKE FILE FOR "Dear ImGui" APPLICATION --- V1.1  ||
#   ||====================================================||
#
#     INFO:
#                     BUILD FILE FOR:       "macOS"
#             Project Root Directory:       imgui/imgui_V0/
#
#
#     INVOCATIONS:
#                                           cmake --build . --config Debug
#                                           cmake --build . --config Release
#
#     HOW-TO:
#           "cd" change-directory to:       imgui/CBApp/build
#              then, run the command:       cmake -G "Xcode" ..
#                                 OR:       cmake -G "Xcode" -DCBLIB_DEBUG=ON ..
#
########################################################################
########################################################################
cmake_minimum_required(VERSION 3.10)
# project(CBApp)



#   0.  PROJECT + GLOBAL VERSION...
########################################################################
########################################################################
#   Bump the semantic version here when you cut a *feature* release.
project(CBApp VERSION 0.3.0 LANGUAGES CXX)

#   Derive a monotonically increasing build number (Git commit count).
#   Fallback to 1 if Git is unavailable (e.g. first CI checkout).
execute_process(
    COMMAND git rev-list --count HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_COUNT
    ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
if(GIT_COMMIT_COUNT STREQUAL "")
    set(GIT_COMMIT_COUNT 1)
endif()
set(BUILD_NUMBER ${GIT_COMMIT_COUNT})



########################################################################
#
#
#
#   1.  PREPROCESSOR MACRO DEFINITIONS PER CONFIGURATION...
########################################################################
########################################################################
add_library(cb_defines INTERFACE)
target_compile_definitions(cb_defines INTERFACE
    #
    #   1.1     CONFIG FILES...
    IMGUI_USER_CONFIG=\"my_imconfig.h\"     #   SET FILENAME OF THE CUSTOM IMGUI CONFIG FILE...
    CBAPP_USER_CONFIG=\"cbapp_config.h\"         #   SPECIFY CBAPP CONFIG FILE...
    #
    #   1.2     MACROS FOR  "ALL"  BUILD TYPES...UILD TYPES...
    _CBLIB_LOG
    # CBAPP_USE_VIEWPORT
    # CBAPP_USE_DOCKSPACE
        #   ...Add additional GENERIC macros here...


    #   1.A     MACROS FOR  "DEBUG"  BUILD TYPES...
    $<$<CONFIG:Debug>:__CBAPP_DEBUG__>
    $<$<CONFIG:Debug>:_CBAPP_LOG>
        #   ...Add additional Debug-only macros here...


    #   2.B     MACROS FOR  "RELEASE"  BUILD TYPES...
    $<$<CONFIG:Release>:NDEBUG>
    $<$<CONFIG:Release>:__CBAPP_RELEASE__>
        #   ...Add additional Release-only macros here...


    #   3.C     MACROS FOR  "RelWithDebInfo"  BUILD TYPES...
    $<$<CONFIG:RelWithDebInfo>:__CBLIB_RELEASE_WITH_DEBUG_INFO__>
        #   ...Add additional RelWithDebInfo-only macros here...


    #   4.D     MACROS FOR  "MinSizeRel"  BUILD TYPES...
    $<$<CONFIG:MinSizeRel>:__CBLIB_MIN_SIZE_RELEASE__>
        #   ...Add additional MinSizeRel-only macros here...
)
# Ensure include/ is on the interface include path so the custom config file is found
#target_include_directories(cb_defines INTERFACE
#    ${CMAKE_CURRENT_SOURCE_DIR}/include
#)

#   Add at the top or below the 'project(CBApp)' line
# option(BUILD_CB_LIBCBAPP_USE_VIEWPORTS "Build test executables for cblib" ON)

#   Third party included libraries...
set(THIRD_PARTY_INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/cblib/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/nlohmann/include
)



########################################################################
#
#
#
#   2.  COMPILER FLAGS, GLOBAL SETTINGS, VERSION REQUIREMENTS, ETC...
########################################################################
########################################################################
#       2.1     Set the C++ standard (adjust if needed)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
#
#       2.2     Set the Minimum MacOSX Version...
set(CMAKE_OSX_DEPLOYMENT_TARGET "13.3" CACHE STRING "Minimum macOS deployment version")

#       2.3     Warnings policy---apply only to our main executable
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
#   4.  GETTING ALL THE FILES / "DEAR IMGUI" - SPECIFIC STUFF...
########################################################################
########################################################################
#       4.1     Find required packages: OpenGL and GLFW.
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
#
#
#       4.2     Collect ImGui core source files from the imgui/directory.
set(IMGUI_CORE_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp             #   ImGui...
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/implot.cpp            #   ImPlot...
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/implot_items.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/implot_demo.cpp       # Optional
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_demo.cpp        # Optional
)
#
#       4.3     Collect Misc ImGui source files.
set(IMGUI_MISC_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
)
#
#       4.4     Collect only the backends you need (GLFW and OpenGL3).
set(IMGUI_BACKEND_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.cpp
)
#
#       4.5     Combine all ImGui source files.
set(IMGUI_SRCS ${IMGUI_CORE_SRCS} ${IMGUI_MISC_SRCS} ${IMGUI_BACKEND_SRCS})
#
#       4.6     Recursively gather your application source files.
file(GLOB_RECURSE APP_SRCS
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)
#
#       Optional:   add header globbing and grouping (for future use):
file(GLOB_RECURSE APP_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/**/*.h
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "include" FILES ${APP_HEADERS})




########################################################################
#
#
#
#   5.  FINAL OPERATIONS (DEFINE EXE, LINKING BINARIES, ETC)...
########################################################################
########################################################################

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
        cb_defines
)
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/config
        ${CMAKE_CURRENT_SOURCE_DIR}/include/app
        ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui_extensions
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
        ${CMAKE_CURRENT_SOURCE_DIR}/misc/cpp
        ${THIRD_PARTY_INCLUDE_DIR}
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
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Optional: bump CFBundleVersion via agvtool")
endif()

#       5.6     Generate `version.hpp` so code can query the version
set(VERSION_HEADER_DIR ${CMAKE_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${VERSION_HEADER_DIR})
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.hpp.in
    ${VERSION_HEADER_DIR}/version.hpp @ONLY)
 target_include_directories(${PROJECT_NAME} PRIVATE ${VERSION_HEADER_DIR})



#       5.7     Organize source files in Xcode
# source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "src" FILES ${APP_SRCS})
# source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/imgui PREFIX "imgui" FILES ${IMGUI_SRCS})
source_group(
    TREE   ${CMAKE_CURRENT_SOURCE_DIR}/imgui
    PREFIX "imgui"
    FILES  ${IMGUI_CORE_SRCS} ${IMGUI_BACKEND_SRCS}
)
#               Extra group just for the misc file
source_group(
    TREE   ${CMAKE_CURRENT_SOURCE_DIR}/misc        # its real location
    PREFIX "imgui/misc"                            # how it should appear
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
    file(GLOB_RECURSE LIBCBAPP_USE_VIEWPORT_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/libs/cblib/tests/*.cpp)
        
    #   Define the test target
    add_executable(cblib_tests ${LIBCBAPP_USE_VIEWPORT_SRCS})

    #   Provide include access to cblib headers
    target_include_directories(cblib_tests PRIVATE
        ${THIRD_PARTY_INCLUDE_DIR}
    )

    #   Optional strict compiler flags for testing
    target_compile_options(cblib_tests PRIVATE -Wall -Wextra -pedantic)

    #   Group test sources in Xcode project navigator
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/libs/cblib/tests PREFIX "cblib/tests" FILES ${LIBCBAPP_USE_VIEWPORT_SRCS})
endif()



########################################################################
########################################################################
#   END...

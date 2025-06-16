########################################################################
#  root/win64/CMakeLists.txt   —  Windows‑only build description
#
#  Why a separate file?
#  ‑ The macOS (Xcode/OpenGL) and Windows (MinGW‑w64) targets need very
#    different generators, toolchains and cache variables. Isolating the
#    Windows configuration inside **win64/** avoids an explosion of
#    `if(WIN32)` branches in the root CMakeLists.txt and keeps each
#    platform’s build system reproducible and clean.
#
#  Typical invocation (performed automatically by `./scripts/build/build_windows.sh`):
#     cmake -S win64 -B win64/build -G Ninja \
#           -DCMAKE_TOOLCHAIN_FILE=cmake/win64-toolchain.cmake \
#           -DCMAKE_BUILD_TYPE=Release
#     cmake --build win64/build -j N
########################################################################
cmake_minimum_required(VERSION 3.16)



# ---------------------------------------------------------------------
# 0.  PROJECT + VERSION (single source of truth)
# ---------------------------------------------------------------------
project(CBApp_Win64 VERSION 0.3.0 LANGUAGES CXX)

# Auto build number = total Git commits (fallback 1 when Git unavailable)
set(PROJECT_ROOT ${CMAKE_CURRENT_LIST_DIR}/..)
execute_process(
    COMMAND git -C "${PROJECT_ROOT}" rev-list --count HEAD
    OUTPUT_VARIABLE GIT_COMMIT_COUNT
    ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
if(GIT_COMMIT_COUNT STREQUAL "")
    set(GIT_COMMIT_COUNT 1)
endif()
set(BUILD_NUMBER ${GIT_COMMIT_COUNT})



# ---------------------------------------------------------------------
# 1.  PREPROCESSOR MACROS & WARNING FLAGS
# ---------------------------------------------------------------------
add_library(cb_defines INTERFACE)
target_compile_definitions(cb_defines INTERFACE
    IMGUI_USER_CONFIG="config/my_imconfig.h"
    CBAPP_USER_CONFIG="config/cbapp_config.h"
    _CBLIB_LOG
    $<$<CONFIG:Debug>:__CBAPP_DEBUG__>
    $<$<CONFIG:Debug>:_CBAPP_LOG>
    $<$<CONFIG:Release>:NDEBUG>
    $<$<CONFIG:Release>:__CBAPP_RELEASE__>
    $<$<CONFIG:RelWithDebInfo>:__CBLIB_RELEASE_WITH_DEBUG_INFO__>
    $<$<CONFIG:MinSizeRel>:__CBLIB_MIN_SIZE_RELEASE__>)

add_library(cb_warnings INTERFACE)
target_compile_options(cb_warnings INTERFACE
    -Wall -Wextra -Wpedantic -Wshadow
#
    -Werror=stringop-truncation
#
    -Wno-unused-but-set-variable
    -Wno-unused-variable
#
    -Wno-deprecated-enum-enum-conversion
    -Wno-misleading-indentation
    -Wno-sign-compare)



# ---------------------------------------------------------------------
# 2.  COMPILER / LINKER GLOBALS
# ---------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_link_options(-static -static-libgcc -static-libstdc++)
    add_compile_options(-static)
endif()



# ---------------------------------------------------------------------
# 3.  THIRD‑PARTY PATHS
# ---------------------------------------------------------------------
set(GLFW_ROOT    ${PROJECT_ROOT}/other/glfw-win64-install)
set(THIRD_PARTY_INCLUDE_DIR
    ${PROJECT_ROOT}/libs/cblib/include
    ${PROJECT_ROOT}/libs/nlohmann/include)



# ---------------------------------------------------------------------
# 4.  PRE‑BUILT GLFW IMPORT
# ---------------------------------------------------------------------
add_library(glfw STATIC IMPORTED)
set_target_properties(glfw PROPERTIES
    IMPORTED_LOCATION "${GLFW_ROOT}/lib/libglfw3.a"
    INTERFACE_INCLUDE_DIRECTORIES "${GLFW_ROOT}/include")



# ---------------------------------------------------------------------
# 5.  GATHER SOURCES
# ---------------------------------------------------------------------
file(GLOB_RECURSE APP_SRCS     CONFIGURE_DEPENDS ${PROJECT_ROOT}/src/*.cpp)
file(GLOB_RECURSE APP_HEADERS                    ${PROJECT_ROOT}/include/*.h)

set(IMGUI_SRCS
    ${PROJECT_ROOT}/misc/cpp/imgui_stdlib.cpp
    ${PROJECT_ROOT}/imgui/imgui.cpp
    ${PROJECT_ROOT}/imgui/imgui_draw.cpp
    ${PROJECT_ROOT}/imgui/imgui_widgets.cpp
    ${PROJECT_ROOT}/imgui/imgui_tables.cpp
    ${PROJECT_ROOT}/imgui/implot.cpp
    ${PROJECT_ROOT}/imgui/implot_items.cpp
    ${PROJECT_ROOT}/imgui/implot_demo.cpp
    ${PROJECT_ROOT}/imgui/imgui_demo.cpp
    ${PROJECT_ROOT}/imgui/backends/imgui_impl_glfw.cpp
    ${PROJECT_ROOT}/imgui/backends/imgui_impl_opengl3.cpp)



# ---------------------------------------------------------------------
# 6.  MAIN EXECUTABLE
# ---------------------------------------------------------------------
add_executable(${PROJECT_NAME} ${APP_SRCS} ${IMGUI_SRCS} ${APP_HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_ROOT}/include
    ${PROJECT_ROOT}/include/config
    ${PROJECT_ROOT}/include/app
    ${PROJECT_ROOT}/include/imgui_extensions
    ${PROJECT_ROOT}/imgui
    ${PROJECT_ROOT}/imgui/backends
    ${PROJECT_ROOT}/misc/cpp
    ${THIRD_PARTY_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE glfw cb_defines cb_warnings opengl32)



# ---------------------------------------------------------------------
# 6.1  Generated header with version/build info
# ---------------------------------------------------------------------
set(VERSION_HEADER_DIR ${CMAKE_BINARY_DIR}/generated)
set(COMPANY_NAME    "Collin Andrew Bond")
set(LEGAL_COPYRIGHT "© 2025 Collin A. Bond. All rights reserved.")
set(TRADEMARKS      "")            # leave empty or add your mark


# Short git hash for the Comments field
execute_process(COMMAND git -C "${PROJECT_ROOT}" rev-parse --short HEAD
                OUTPUT_VARIABLE GIT_HASH
                OUTPUT_STRIP_TRAILING_WHITESPACE)



# ---------------------------------------------------------------------
# 6.2  Generated header with version/build info
# ---------------------------------------------------------------------
file(MAKE_DIRECTORY ${VERSION_HEADER_DIR})
configure_file(
    ${PROJECT_ROOT}/cmake/version.hpp.in        # shared template
    ${VERSION_HEADER_DIR}/version.hpp @ONLY)

target_include_directories(${PROJECT_NAME} PRIVATE ${VERSION_HEADER_DIR})



# ---------------------------------------------------------------------
# 6.3  VERSIONINFO resource (Explorer ▸ Details)
# ---------------------------------------------------------------------
if(WIN32)
    configure_file(
        ${PROJECT_ROOT}/cmake/version.rc.in     # shared template
        ${CMAKE_BINARY_DIR}/version.rc @ONLY)
    target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/version.rc)
endif()



# ---------------------------------------------------------------------
# 7.  EMBED FONT EXAMPLE (unchanged logic; path fixed)
# ---------------------------------------------------------------------
set(FONT_PATH "${PROJECT_ROOT}/assets/Fonts/Roboto/static/Roboto-Regular.ttf")
if(NOT EXISTS "${FONT_PATH}")
    message(FATAL_ERROR "Embed-assets: font not found at ${FONT_PATH}. Update FONT_PATH.")
endif()

set(EMBED_FILES "${FONT_PATH}")
set(ASSET_RC "${CMAKE_CURRENT_BINARY_DIR}/assets.rc")

add_custom_command(
    OUTPUT  ${ASSET_RC}
    COMMAND ${CMAKE_COMMAND}
        -DASSETS_LIST="${EMBED_FILES}"
        -DOUT_FILE=${ASSET_RC}
        -P ${PROJECT_ROOT}/cmake/gen_rc.cmake
    DEPENDS ${EMBED_FILES}
    COMMENT "Generating resource script for embedded font → ${FONT_PATH}")

add_custom_target(embed_assets DEPENDS ${ASSET_RC})
add_library(assets_res OBJECT ${ASSET_RC})
add_dependencies(assets_res embed_assets)

target_link_libraries(${PROJECT_NAME} PRIVATE assets_res)
add_dependencies(${PROJECT_NAME} embed_assets)



# ---------------------------------------------------------------------
# 8.  INSTALL Fallback when running the exe under Wine/macOS
# ---------------------------------------------------------------------
if(NOT WIN32)
    if(EXISTS "${PROJECT_ROOT}/assets")
        install(DIRECTORY "${PROJECT_ROOT}/assets/" DESTINATION assets)
    endif()
endif()

########################################################################
# END

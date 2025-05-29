########################################################################
#
#  WINDOWS‑ONLY CMAKE FILE FOR "Dear ImGui" APPLICATION (cross‑compiled)
#  Target: 64‑bit Windows EXE built **on macOS** with MinGW‑w64
#
#------------------------------------------------------------------------
#  How to configure:
#     cmake -G Ninja \
#           -DCMAKE_TOOLCHAIN_FILE=cmake/win64-toolchain.cmake \
#           -DCMAKE_BUILD_TYPE=Release \
#           -B build-win64
#     cmake --build build-win64
#
#
#------------------------------------------------------------------------
#  Toolchain file example (cmake/win64-toolchain.cmake) is shown in docs.
#
#------------------------------------------------------------------------
#  WINDOWS‑ONLY CMAKE FILE FOR "Dear ImGui" APPLICATION (cross‑compiled)
#  Location: imgui_V0/win64/CMakeLists.txt
#
#
########################################################################
cmake_minimum_required(VERSION 3.16)
project(CBApp_Win64 LANGUAGES CXX)



#   0.  PREPROCESSOR MACRO DEFINITIONS PER CONFIGURATION...
########################################################################
########################################################################
add_library(cb_defines INTERFACE)
target_compile_definitions(cb_defines INTERFACE
    #
    #   0.1     CONFIG FILES...
    IMGUI_USER_CONFIG=\"config/my_imconfig.h\"          #   SET FILENAME OF THE CUSTOM IMGUI CONFIG FILE...
    CBAPP_USER_CONFIG=\"config/cbapp_config.h\"         #   SPECIFY CBAPP CONFIG FILE...
    #
    #   0.2     MACROS FOR  "ALL"  BUILD TYPES...UILD TYPES...
    _CBLIB_LOG
    # CBAPP_USE_VIEWPORT
    # CBAPP_USE_DOCKSPACE
        #   ...Add additional GENERIC macros here...


    #   1.    MACROS FOR  "DEBUG"  BUILD TYPES...
    $<$<CONFIG:Debug>:__CBAPP_DEBUG__>
    $<$<CONFIG:Debug>:_CBAPP_LOG>
        #   ...Add additional Debug-only macros here...


    #   2.    MACROS FOR  "RELEASE"  BUILD TYPES...
    $<$<CONFIG:Release>:NDEBUG>
    $<$<CONFIG:Release>:__CBAPP_RELEASE__>
        #   ...Add additional Release-only macros here...


    #   3.    MACROS FOR  "RelWithDebInfo"  BUILD TYPES...
    $<$<CONFIG:RelWithDebInfo>:__CBLIB_RELEASE_WITH_DEBUG_INFO__>
        #   ...Add additional RelWithDebInfo-only macros here...


    #   4.    MACROS FOR  "MinSizeRel"  BUILD TYPES...
    $<$<CONFIG:MinSizeRel>:__CBLIB_MIN_SIZE_RELEASE__>
        #   ...Add additional MinSizeRel-only macros here...
)



#   1.      HELPER INTERFACE LIBRARIES...
########################################################################
########################################################################
# add_library(cb_defines INTERFACE)
# generic + debug macros
# target_compile_definitions(cb_defines INTERFACE _CBLIB_LOG $<$<CONFIG:Debug>:__CBAPP_DEBUG__>)



add_library(cb_warnings INTERFACE)
target_compile_options(cb_warnings INTERFACE
    -Wall -Wextra -Wpedantic -Wshadow
    -Wno-unused-variable
    -Wno-sign-compare
    -Wno-template-body
    #-Wconversion -Wsign-conversion
)



########################################################################
#
#
#
#   2.      GLOBAL SETTINGS, VERSION REQUIREMENTS, ETC...
########################################################################
########################################################################

# ------------------------------------------------------------------ 2. global settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2b. static link flags (MinGW only)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_link_options(-static -static-libgcc -static-libstdc++)
    add_compile_options(-static)
endif()



########################################################################
#
#
#
#   3.      PATHS...
########################################################################
########################################################################
set(PROJECT_ROOT ${CMAKE_CURRENT_LIST_DIR}/..)
set(GLFW_ROOT    ${PROJECT_ROOT}/other/glfw-win64-install)

set(THIRD_PARTY_INCLUDE_DIR
    ${PROJECT_ROOT}/libs/cblib/include
    ${PROJECT_ROOT}/libs/nlohmann/include)

# ------------------------------------------------------------------ 4. import static GLFW
add_library(glfw STATIC IMPORTED)
set_target_properties(glfw PROPERTIES
    IMPORTED_LOCATION "${GLFW_ROOT}/lib/libglfw3.a"
    INTERFACE_INCLUDE_DIRECTORIES "${GLFW_ROOT}/include")

# ------------------------------------------------------------------ 5. sources
file(GLOB_RECURSE APP_SRCS     CONFIGURE_DEPENDS ${PROJECT_ROOT}/src/*.cpp)
file(GLOB_RECURSE APP_HEADERS                    ${PROJECT_ROOT}/include/*.h)

set(IMGUI_SRCS
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


# ------------------------------------------------------------------ 6. executable
add_executable(${PROJECT_NAME} ${APP_SRCS} ${IMGUI_SRCS} ${APP_HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_ROOT}/include
    ${PROJECT_ROOT}/include/app
    ${PROJECT_ROOT}/include/imgui_extensions
    ${PROJECT_ROOT}/imgui
    ${PROJECT_ROOT}/imgui/backends
    ${THIRD_PARTY_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw cb_defines cb_warnings opengl32)

if(WIN32)
    # Optional version resource
    target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/version.rc)



# ------------------------------------------------------------------ 7. embed **one specific font** as proof‑of‑concept
# ------------------------------------------------------------------
#  Adjust FONT_PATH to match the *actual* location of a .ttf you want embedded.
#  We abort the configure step early if it is missing to avoid confusing Make errors.

set(FONT_PATH "${PROJECT_ROOT}/assets/Fonts/Roboto/static/Roboto-Regular.ttf")

if(NOT EXISTS "${FONT_PATH}")
    message(FATAL_ERROR "Embed‑assets: font not found at ${FONT_PATH}. Please update FONT_PATH in win64/CMakeLists.txt to a valid .ttf.")
endif()

set(EMBED_FILES "${FONT_PATH}")

set(ASSET_RC "${CMAKE_CURRENT_BINARY_DIR}/assets.rc")

add_custom_command(
    OUTPUT  ${ASSET_RC}
    COMMAND ${CMAKE_COMMAND}
            -DASSETS_LIST="${EMBED_FILES}"
            -DOUT_FILE=${ASSET_RC}
            -P ${CMAKE_CURRENT_LIST_DIR}/cmake/gen_rc.cmake
    DEPENDS ${EMBED_FILES}
    COMMENT "Generating resource script for embedded font → ${FONT_PATH}")

add_custom_target(embed_assets DEPENDS ${ASSET_RC})

add_library(assets_res OBJECT ${ASSET_RC})
add_dependencies(assets_res embed_assets)

target_link_libraries(${PROJECT_NAME} PRIVATE assets_res)
add_dependencies(${PROJECT_NAME} embed_assets)
endif()



# ------------------------------------------------------------------ 8. asset folder install (non‑Windows or when testing externally)
if(NOT WIN32)
    if(EXISTS "${PROJECT_ROOT}/assets")
        install(DIRECTORY "${PROJECT_ROOT}/assets/" DESTINATION assets)
    endif()
endif()

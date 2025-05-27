########################################################################
#
#   ||===================================================||
#   ||  CMAKE FILE FOR "Dear ImGui" APPLICATION --- V1.  ||
#   ||===================================================||
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
project(CBApp)



#   0.  PREPROCESSOR MACRO DEFINITIONS PER CONFIGURATION...
########################################################################
########################################################################
add_library(cb_defines INTERFACE)
target_compile_definitions(cb_defines INTERFACE
    #
    #   0.1     CONFIG FILES...
    IMGUI_USER_CONFIG=\"my_imconfig.h\"     #   SET FILENAME OF THE CUSTOM IMGUI CONFIG FILE...
    CBAPP_USER_CONFIG=\"cbapp_config.h\"         #   SPECIFY CBAPP CONFIG FILE...
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
#   1.  COMPILER FLAGS, GLOBAL SETTINGS, VERSION REQUIREMENTS, ETC...
########################################################################
########################################################################

#       1.1     Set the C++ standard (adjust if needed)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
#
#       1.2     Set the Minimum MacOSX Version...
set(CMAKE_OSX_DEPLOYMENT_TARGET "13.3" CACHE STRING "Minimum macOS deployment version")

#       1.3     Warnings policy---apply only to our main executable
add_library(cb_warnings INTERFACE)
target_compile_options(cb_warnings INTERFACE
    -Wall -Wextra -Wpedantic -Wshadow
    -Wno-unused-variable
    -Wno-sign-compare
    #-Wconversion -Wsign-conversion
)


########################################################################
#
#
#
#   2.  INCLUDE DIRECTORIES, SOURCE-FILE DIRECTORIES, ETC...
########################################################################
########################################################################

#       2.1     Include directories for your own headers, ImGui, and its backends
# include_directories(
#     ${CMAKE_CURRENT_SOURCE_DIR}/include                     #   main.h and public headers
#     ${CMAKE_CURRENT_SOURCE_DIR}/include/app                 #   user-defined headers
#     ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui_extensions    #   optional imgui wrappers
#     ${CMAKE_CURRENT_SOURCE_DIR}/imgui                       #   ImGui core
#     ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends              #   ImGui backends
# )



########################################################################
#
#
#
#   3.  GETTING ALL THE FILES / "DEAR IMGUI" - SPECIFIC STUFF...
########################################################################
########################################################################

#       3.1     Find required packages: OpenGL and GLFW.
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
#
#
#       3.2     Collect ImGui core source files from the imgui/directory.
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
#       3.3     Collect only the backends you need (GLFW and OpenGL3).
set(IMGUI_BACKEND_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.cpp
)
#
#       3.4     Combine all ImGui source files.
set(IMGUI_SRCS ${IMGUI_CORE_SRCS} ${IMGUI_BACKEND_SRCS})
#
#       3.5     Recursively gather your application source files.
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
#   4.  FINAL OPERATIONS (DEFINE EXE, LINKING BINARIES, ETC)...
########################################################################
########################################################################

#       4.2     Create the executable from your application and ImGui source files
add_executable(${PROJECT_NAME}
    ${APP_SRCS}
    ${IMGUI_SRCS}
    ${APP_HEADERS}
)

#       4.3     Link necessary libraries: OpenGL, GLFW, and warnings interface.
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
        ${THIRD_PARTY_INCLUDE_DIR}
)

#               Configure Xcode-specific behavior
set_target_properties(${PROJECT_NAME} PROPERTIES
    XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "13.3"
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.example.cbapp"
    XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "-"
    XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME "NO"
)

#       4.4     Add a Pre-Processor Macro "DEBUG"...
# target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:DEBUG>)




#   Organize source files in Xcode
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "src" FILES ${APP_SRCS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/imgui PREFIX "imgui" FILES ${IMGUI_SRCS})



########################################################################
#
#
#
#   5.  BUILD EXECUTABLES FOR MY "CB_LIB" TEMPLATE LIBRARY...
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

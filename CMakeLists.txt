########################################################################
########################################################################
#
#   CMAKE FILE FOR "Dear ImGui" APPLICATION --- V0.
#
#       go to:          imgui/imgui_V0/build
#       then, run:      cmake -G "Xcode" ..
#
########################################################################
########################################################################

cmake_minimum_required(VERSION 3.10)
project(CBApp)

#   Add at the top or below the 'project(CBApp)' line
# option(BUILD_CB_LIB_TESTS "Build test executables for cblib" ON)



#   1.  GLOBAL SETTINGS, VERSION REQUIREMENTS, ETC...
########################################################################
########################################################################

#       1.1     Set the C++ standard (adjust if needed)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#
#       1.2     Set the Minimum MacOSX Version...
set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "Minimum macOS deployment version")


########################################################################
#
#
#
#   2.  INCLUDE DIRECTORIES, SOURCE-FILE DIRECTORIES, ETC...
########################################################################
########################################################################

#       2.1     Include directories for your own headers, ImGui, and its backends
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include                     #   main.h and public headers
    ${CMAKE_CURRENT_SOURCE_DIR}/include/app                 #   user-defined headers
    ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui_extensions    #   optional imgui wrappers
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui                       #   ImGui core
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends              #   ImGui backends
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/cblib/include          #   cblib template library
)



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
file(GLOB_RECURSE APP_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
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

#       4.3     Link necessary libraries: OpenGL and GLFW.
target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_gl_LIBRARY} glfw)

#               Configure Xcode-specific behavior
set_target_properties(${PROJECT_NAME} PROPERTIES
    XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "13.0"
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

if (BUILD_CB_LIB_TESTS)
    #   Gather all .cpp test drivers under libs/cblib/tests/
    file(GLOB_RECURSE LIB_TEST_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/libs/cblib/tests/*.cpp)
        
    #   Define the test target
    add_executable(cblib_tests ${LIB_TEST_SRCS})

    #   Provide include access to cblib headers
    target_include_directories(cblib_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/cblib/include
    )

    #   Optional strict compiler flags for testing
    target_compile_options(cblib_tests PRIVATE -Wall -Wextra -pedantic)

    #   Group test sources in Xcode project navigator
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/libs/cblib/tests PREFIX "cblib/tests" FILES ${LIB_TEST_SRCS})
endif()



########################################################################
########################################################################
#   END...

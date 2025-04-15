####****************************************************************####
####****************************************************************####
#   CMAKE FILE FOR "Dear ImGui" APPLICATION --- V0.
#
#
####****************************************************************####
####****************************************************************####

cmake_minimum_required(VERSION 3.10)
project(IMGUIAppV0)



#   1.  GLOBAL SETTINGS, VERSION REQUIREMENTS, ETC...
########################################################################
########################################################################

#       1.1     Set the C++ standard (adjust if needed)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#
#       1.2     Set the Minimum MacOSX Version...
set(MACOSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "Minimum macOS deployment version")






########################################################################
#
#
#
#   2.  INCLUDE DIRECTORIES, SOURCE-FILE DIRECTORIES, ETC...
########################################################################
########################################################################

#       2.1     Include directories for your own headers, ImGui, and its backends
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/libs  # Optional: include if libs/ contains headers you need.
)
#
#
#       2.2     Gather your application source files located in src/
file(GLOB APP_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
#
#
#       2.3     Create the executable from your application and ImGui source files
add_executable(${PROJECT_NAME}
    ${APP_SRCS}
    ${IMGUI_SRCS}
)





########################################################################
#
#
#
#   3.  "DEAR IMGUI" - SPECIFIC STUFF...
########################################################################
########################################################################

#       3.1     Find required packages: OpenGL and GLFW.
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
#
#
#       3.2     Collect ImGui core source files from the imgui/directory.
set(IMGUI_CORE_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_tables.cpp  # <-- add this line
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_demo.cpp     # optional
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









########################################################################
#
#
#
#   4.  ALL OTHER FUNCTIONS (LINKING, ETC)...
########################################################################
########################################################################

#       4.1     Link necessary libraries: OpenGL and GLFW.
target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_gl_LIBRARY} glfw)






########################################################################
########################################################################
#   END...

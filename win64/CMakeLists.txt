########################################################################
 #   WINDOWS-ONLY CMAKE FILE FOR "Dear ImGui" APPLICATION
 ########################################################################
 cmake_minimum_required(VERSION 3.10)
 project(CBApp_Win64)
 
 #   Toolchain must already be specified (win64-toolchain.cmake)
 
 # Third-party include directories
 set(THIRD_PARTY_INCLUDE_DIR
     ${CMAKE_SOURCE_DIR}/../libs/cblib/include
     ${CMAKE_SOURCE_DIR}/../libs/nlohmann/include
 )
 
 # Set C++ standard
 set(CMAKE_CXX_STANDARD 20)
 set(CMAKE_CXX_STANDARD_REQUIRED ON)
 
 # Include directories
 include_directories(
     ${CMAKE_SOURCE_DIR}/../other/glfw-win64-install/include
     ${CMAKE_SOURCE_DIR}/../include
     ${CMAKE_SOURCE_DIR}/../include/app
     ${CMAKE_SOURCE_DIR}/../include/imgui_extensions
     ${CMAKE_SOURCE_DIR}/../imgui
     ${CMAKE_SOURCE_DIR}/../imgui/backends
 )
 link_directories(${CMAKE_SOURCE_DIR}/../other/glfw-win64-install/lib)
 
 
 # Find packages
 find_package(OpenGL REQUIRED)
 
 # Collect ImGui sources
 set(IMGUI_CORE_SRCS
     ${CMAKE_SOURCE_DIR}/../imgui/imgui.cpp
     ${CMAKE_SOURCE_DIR}/../imgui/imgui_draw.cpp
     ${CMAKE_SOURCE_DIR}/../imgui/imgui_widgets.cpp
     ${CMAKE_SOURCE_DIR}/../imgui/imgui_tables.cpp
     ${CMAKE_SOURCE_DIR}/../imgui/implot.cpp
     ${CMAKE_SOURCE_DIR}/../imgui/implot_items.cpp
     ${CMAKE_SOURCE_DIR}/../imgui/implot_demo.cpp
     ${CMAKE_SOURCE_DIR}/../imgui/imgui_demo.cpp
 )
 
 set(IMGUI_BACKEND_SRCS
     ${CMAKE_SOURCE_DIR}/../imgui/backends/imgui_impl_glfw.cpp
     ${CMAKE_SOURCE_DIR}/../imgui/backends/imgui_impl_opengl3.cpp
 )
 
 set(IMGUI_SRCS ${IMGUI_CORE_SRCS} ${IMGUI_BACKEND_SRCS})
 
 # Collect application sources and headers
 file(GLOB_RECURSE APP_SRCS ${CMAKE_SOURCE_DIR}/../src/*.cpp)
 file(GLOB_RECURSE APP_HEADERS
     ${CMAKE_SOURCE_DIR}/../include/*.h
     ${CMAKE_SOURCE_DIR}/../include/**/*.h
 )
 
 # Define executable
 add_executable(${PROJECT_NAME}
     ${APP_SRCS}
     ${IMGUI_SRCS}
     ${APP_HEADERS}
 )
 
 # Add Windows resource file for version information
 if (WIN32)
     target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/version.rc)
 endif()
 
 # Force static linking of standard libraries (libgcc, libstdc++, pthread)
 target_compile_options(${PROJECT_NAME} PRIVATE -static -static-libgcc -static-libstdc++)
 target_link_options(${PROJECT_NAME} PRIVATE -static -static-libgcc -static-libstdc++)
 
 
 # Link libraries
 # On Windows: opengl32.lib and glfw3.lib
 # (glfw3 should already be built for Windows via MinGW)
 target_link_libraries(${PROJECT_NAME}
     PRIVATE
         ${OPENGL_gl_LIBRARY}
         /Users/collinbond/Desktop/HOME_DIRECTORY/10_PROJECTS/imgui/imgui_V0/other/glfw-win64-install/lib/libglfw3.a
 )
 
 target_include_directories(${PROJECT_NAME}
     PRIVATE
         ${THIRD_PARTY_INCLUDE_DIR}
 )
 
 
 
 ########################################################################
 # END OF FILE
 ########################################################################

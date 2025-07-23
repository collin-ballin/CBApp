/***********************************************************************************
*
*       ********************************************************************
*       ****               E D I T O R . H  ____  F I L E               ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      June 12, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBWIDGETS_FUNCTIONAL_TESTING_H
#define _CBWIDGETS_FUNCTIONAL_TESTING_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
#include "utility/utility.h"
#include "utility/pystream/pystream.h"
#include "widgets/widgets.h"
//  //  #include "app/_init.h"
#include "app/state/state.h"
//
#include "app/c_counter/c_counter.h"        //  Seperate Application Classes.
#include "app/editor_app/editor_app.h"
#include "app/graph_app/graph_app.h"
//
#include "app/delegators/_menubar.h"        //  Delegator Classes.
#include "app/delegators/_browser.h"
#include "app/delegators/_controlbar.h"
#include "app/delegators/_detail_view.h"
//#include "app/titlebar/_titlebar.h"



//  0.2     STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <cstdlib>          // C-Headers...
#include <stdio.h>
#include <unistd.h>


#include <string>           //  <======| std::string, ...
#include <string_view>
#include <format>
#include <vector>           //  <======| std::vector, ...
#include <stdexcept>        //  <======| ...
#include <limits.h>
#include <math.h>


#ifdef __CBAPP_DEBUG__      //  <======| Fix for issue wherein multiple instances of application
# include <thread>          //           are launched when DEBUG build is run inside Xcode IDE...
# include <chrono>
#endif     //  __CBAPP_DEBUG__  //



//  0.3     "DEAR IMGUI" HEADERS...
#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#if defined(IMGUI_IMPL_OPENGL_ES2)
# include <GLES2/gl2.h>
#endif      //  IMGUI_IMPL_OPENGL_ES2  //

#include <GLFW/glfw3.h>     //  <======| Will drag system OpenGL headers

//  [Win32]     Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
//                  - To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
//                  - Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
# pragma comment(lib, "legacy_stdio_definitions")
#endif

//              This example can also compile and run with Emscripten! See 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
# include "../../libs/emscripten/emscripten_mainloop_stub.h"
#endif



namespace cb { namespace ui { //     BEGINNING NAMESPACE "cb::ui"...
// *************************************************************************** //
// *************************************************************************** //

















// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} } //   END OF "cb::ui" NAMESPACE.






#endif      //  _CBWIDGETS_FUNCTIONAL_TESTING_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.

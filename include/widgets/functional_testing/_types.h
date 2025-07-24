/***********************************************************************************
*
*       **************************************************************************
*       ****                  _ T Y P E S . H  ____  F I L E                  ****
*       **************************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      July 23, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBWIDGETS_FUNCTIONAL_TESTING_TYPES_H
#define _CBWIDGETS_FUNCTIONAL_TESTING_TYPES_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
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
//      STATIC HELPER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //






    
// *************************************************************************** //
//
//
//      0.      "ActionComposer" TYPES...
// *************************************************************************** //
// *************************************************************************** //

// *************************************************************************** //
//      ACTION TYPE LAYER...
// *************************************************************************** //

//  "ComposerState"
//
enum class ComposerState : uint8_t {
    Idle = 0,
    Run,
    Capture,
    COUNT
};

static constexpr std::array<const char *, static_cast<size_t>(ComposerState::COUNT)>
DEF_COMPOSER_STATE_NAMES = {
    "Idle", "Running", "Capturing"
};



// *************************************************************************** //
//      ACTION TYPE LAYER...
// *************************************************************************** //

//  "ActionType"
//
enum class ActionType : uint8_t {
    CursorMove,
//
    MouseClick,      // press + release immediately
    MousePress,      // press and keep held
    MouseRelease,    // release previously held
    MouseDrag,       // full drag in one action (press–move–release)
//
    Hotkey,
//
    COUNT
};


//  "DEF_ACTION_TYPE_NAMES"
static constexpr std::array<const char *, static_cast<size_t>( ActionType::COUNT )>
DEF_ACTION_TYPE_NAMES = {
    "CursorMove",       "Mouse Click",      "Mouse Press",      "Mouse Release",        "Mouse Drag",       "Hotkey"
};



// *************************************************************************** //
//      ACTION DATA LAYER...
// *************************************************************************** //

//  "CursorMoveParams"
//
struct CursorMoveParams {
    ImVec2              first           {};
    ImVec2              last            {};
    float               duration        {1.f};
};


//  "ClickParams"
//
struct ClickParams { bool left_button{true};  };


//  "DragParams"
//
struct DragParams
{
    ImVec2  from{};
    ImVec2  to{};
    float   duration{1.f};
    bool    left_button{true};
};



//  "HotkeyParams"
//
struct HotkeyParams {
    ImGuiKey            key             {ImGuiKey_C};
    bool                ctrl            {true};
    bool                shift           {false};
    bool                alt             {false};
};


//  "Action"
//
struct Action {
    std::string         name            = "new action";
    std::string         descr           = "description...";
    ActionType          type            = ActionType::CursorMove;
//
    CursorMoveParams    cursor;
    HotkeyParams        hotkey;
//
    bool                enabled         = true;
};



// *************************************************************************** //
//      ACTION EXECUTION LAYER...
// *************************************************************************** //
    
//  "ExecutorState"
//
enum class ExecutionState : uint8_t {
    None = 0,
    Move,
    ButtonDown,
    ButtonUp,
    COUNT
};

static constexpr std::array<const char *, static_cast<size_t>(ExecutionState::COUNT)>
DEF_EXEC_STATE_NAMES = {
    "None", "Move", "Button Down", "Button Up"
};



// *************************************************************************** //
//      COMPOSITION TYPES...
// *************************************************************************** //

//  "Composition_t"
//
struct Composition_t {
//
    std::vector<Action>     actions;
    std::string             name            { "New Composition" };
    std::string             description     { "description..." };
};


















// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} } //   END OF "cb::ui" NAMESPACE.






#endif      //  _CBWIDGETS_FUNCTIONAL_TESTING_TYPES_H  //
// *************************************************************************** //
// *************************************************************************** //   END.

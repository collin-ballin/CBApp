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
#include "json.hpp"
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

inline void to_json (nlohmann::json & j, const CursorMoveParams & p)
{
    j = { {"first", p.first},
          {"last",  p.last},
          {"duration", p.duration} };
}

inline void from_json(const nlohmann::json & j, CursorMoveParams & p)
{
    j.at("first").get_to(p.first);
    j.at("last").get_to(p.last);
    j.at("duration").get_to(p.duration);
}






//  "ClickParams"
//
struct ClickParams { bool left_button{true};  };

inline void to_json(nlohmann::json& j, const ClickParams& p)
{ j = { {"left", p.left_button} }; }
inline void from_json(const nlohmann::json& j, ClickParams& p)
{ j.at("left").get_to(p.left_button); }


//  "DEF_CLICK_PARAM_NAMES"
static constexpr std::array<const char *, static_cast<size_t>( ActionType::COUNT )>
DEF_CLICK_PARAM_NAMES = {
    "Left Mouse",       "Right Mouse"
};






//  "DragParams"
//
struct DragParams
{
    ImVec2  from{};
    ImVec2  to{};
    float   duration{1.f};
    bool    left_button{true};
};

inline void to_json(nlohmann::json & j, const DragParams & p)
{
    j = {
        {"from",    p.from},
        {"to",      p.to},
        {"duration",p.duration},
        {"left",    p.left_button}
    };
}
inline void from_json(const nlohmann::json & j, DragParams & p)
{
    j.at("from").get_to(p.from);
    j.at("to"  ).get_to(p.to  );
    j.at("duration").get_to(p.duration);
    j.at("left").get_to(p.left_button);
}






//  "HotkeyParams"
//
struct HotkeyParams {
    ImGuiKey            key             {ImGuiKey_C};
    bool                ctrl            {true};
    bool                shift           {false};
    bool                alt             {false};
};

inline void to_json(nlohmann::json& j, const HotkeyParams& p)
{
    j = { {"key",   p.key},
          {"ctrl",  p.ctrl},
          {"shift", p.shift},
          {"alt",   p.alt} };
}
inline void from_json(const nlohmann::json& j, HotkeyParams& p)
{
    j.at("key").get_to(p.key);
    j.at("ctrl" ).get_to(p.ctrl );
    j.at("shift").get_to(p.shift);
    j.at("alt"  ).get_to(p.alt  );
}






//  "Action"
//
struct Action {
    std::string             name            = "new action";
    std::string             descr           = "description...";
    ActionType              type            = ActionType::CursorMove;
//
    CursorMoveParams        cursor;             //  <   for CursorMove
    ClickParams             click;              //  <   for MouseClick
    ClickParams             press;              //  <   for MousePress
    ClickParams             release;            //  <   for MouseRelease
    DragParams              drag;               //  <   for MouseDrag
    HotkeyParams            hotkey;             //  <   for Hotkey
//
    GLFWwindow *            target          = nullptr;
    bool                    enabled         = true;
};

inline void to_json(nlohmann::json& j, const Action& a)
{
    j = {
        {"name",    a.name},
        {"descr",   a.descr},
        {"type",    static_cast<uint8_t>(a.type)},
        {"cursor",  a.cursor},
        {"click",   a.click},
        {"press",   a.press},
        {"release", a.release},
        {"drag",    a.drag},
        {"hotkey",  a.hotkey},
        {"enabled", a.enabled}
    };
}

inline void from_json(const nlohmann::json& j, Action& a)
{
    j.at("name").get_to(a.name);
    j.at("descr").get_to(a.descr);
    uint8_t t; j.at("type").get_to(t); a.type = static_cast<ActionType>(t);
    j.at("cursor" ).get_to(a.cursor );
    j.at("click"  ).get_to(a.click  );
    j.at("press"  ).get_to(a.press  );
    j.at("release").get_to(a.release);
    j.at("drag"   ).get_to(a.drag   );
    j.at("hotkey" ).get_to(a.hotkey );
    j.at("enabled").get_to(a.enabled);
}






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

inline void to_json(nlohmann::json& j, const Composition_t& c)
{ j = { {"name", c.name}, {"actions", c.actions} }; }

inline void from_json(const nlohmann::json& j, Composition_t& c)
{
    j.at("name").get_to(c.name);
    j.at("actions").get_to(c.actions);
}















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

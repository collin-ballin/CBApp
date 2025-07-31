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
#include <unordered_set>
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
//      0.      "KeyHoldManager" TYPE...
// *************************************************************************** //
// *************************************************************************** //

//  "KeyHoldManager"
//
class KeyHoldManager {
public:

    //  "Begin"
    //      Call ONCE each frame, *before* ImGui::NewFrame()...
    //
    inline void             Begin               (void)
    {
        ImGuiIO & io = ImGui::GetIO();

        //      1.   Emit scheduled releases (up edge exactly once)
        for (ImGuiKey key : m_release)      { io.AddKeyEvent(key, false); }
        
        m_release.clear();                      // done with them

        //      2.  Re-emit down edge for currently held keys
        for ( ImGuiKey key : m_active )     { io.AddKeyEvent(key, true); }
    }


    //  "Push"
    //      Start holding a key (idempotent)
    //
    inline void             Push                (ImGuiKey key)
    {
        if (key == ImGuiKey_None) return;
        if (m_active.insert(key).second)        // newly inserted?
            ImGui::GetIO().AddKeyEvent(key, true);  // initial press (processed next frame)
    }


    //  "Pop"
    //      Stop holding a key: release happens on the next Begin()
    //
    inline void             Pop                 (ImGuiKey key)
    {
        if (key == ImGuiKey_None) return;
        if (m_active.erase(key))                // was it active?
            m_release.insert(key);              // queue one-time release
    }
    

    //  "PopAll"
    //      Convenience helper
    //
    inline void             PopAll              (void)
    {
        for (ImGuiKey key : m_active)
            m_release.insert(key);
        m_active.clear();
    }
    
    
    //  "IsActive"
    //
    inline bool             IsActive            (ImGuiKey key) const { return m_active.contains(key); }


// *************************************************************************** //
// *************************************************************************** //
private:
    std::unordered_set<ImGuiKey> m_active;   // keys we want held
    std::unordered_set<ImGuiKey> m_release;  // keys to release on next Begin()
    

// *************************************************************************** //
// *************************************************************************** //
};//	END "KeyHoldManager" INLINE CLASS DEFINITION.








    
// *************************************************************************** //
//
//
//
//      1.      "ActionComposer" TYPES...
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
    MouseCapture,
    KeyCapture,
    COUNT
};

static constexpr std::array<const char *, static_cast<size_t>(ComposerState::COUNT)>
DEF_COMPOSER_STATE_NAMES = {
    "Idle", "Running", "Mouse Capture", "Keyboard Capture"
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
    KeyPress,            // NEW : press & hold
    KeyRelease,          // NEW : release key
//
    COUNT
};
//
//  "DEF_ACTION_TYPE_NAMES"
static constexpr std::array<const char *, static_cast<size_t>( ActionType::COUNT )>
DEF_ACTION_TYPE_NAMES = {
    "Move Cursor",
    "Mouse Click",      "Mouse Press",      "Mouse Release",        "Mouse Drag",
    "Hotkey",           "Key Press",        "Key Release"
};




//  "ClickType"
//
enum class ClickType : uint8_t {
    Click=0, Press, Release, COUNT
};
//
//  "DEF_ACTION_TYPE_NAMES"
static constexpr std::array<const char *, static_cast<size_t>( ActionType::COUNT )>
DEF_CLICK_TYPE_NAMES = {
    "Click",    "Press",    "Release"
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
    j = {
            {"first",       p.first},
            {"last",        p.last},
            {"duration",    p.duration}
    };
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

inline void to_json(nlohmann::json & j, const ClickParams & p)
{
    j = {
            {"left",    p.left_button}
    };
}

inline void from_json(const nlohmann::json & j, ClickParams & p)
{
    j.at("left").get_to(p.left_button);
}


//  "MouseClickTypes"
//
enum class MouseClickTypes : uint8_t {
    Left, Right, COUNT
};
//
//  "DEF_CLICK_PARAM_NAMES"
static constexpr std::array<const char *, static_cast<size_t>( MouseClickTypes::COUNT )>
DEF_MOUSE_CLICK_TYPES_NAMES = {
    "Left Mouse",       "Right Mouse"
};






//  "DragParams"
//
struct DragParams
{
    ImVec2          from                    {};
    ImVec2          to                      {};
    float           duration                {1.f};
    bool            left_button             {true};
};

inline void to_json(nlohmann::json & j, const DragParams & p)
{
    j = {
            {"from",                p.from                          },
            {"to",                  p.to                            },
            {"duration",            p.duration                      },
            {"left",                p.left_button                   }
    };
}
inline void from_json(const nlohmann::json & j, DragParams & p)
{
    j.at("from"                     ).get_to(p.from                 );
    j.at("to"                       ).get_to(p.to                   );
    j.at("duration"                 ).get_to(p.duration             );
    j.at("left"                     ).get_to(p.left_button          );
}






//  "HotkeyParams"
//
struct HotkeyParams {
    ImGuiKey            key             {ImGuiKey_C};
    bool                ctrl            {true};
    bool                shift           {false};
    bool                alt             {false};
    bool                super           {false};
};

inline void to_json(nlohmann::json& j, const HotkeyParams& p)
{
    j = {
            {"key",                 p.key                           },
            {"ctrl",                p.ctrl                          },
            {"shift",               p.shift                         },
            {"alt",                 p.alt                           },
            {"super",               p.super                         }
    };
}


inline void from_json(const nlohmann::json& j, HotkeyParams& p)
{
    j.at("key"                      ).get_to(p.key                  );
    j.at("ctrl"                     ).get_to(p.ctrl                 );
    j.at("shift"                    ).get_to(p.shift                );
    j.at("alt"                      ).get_to(p.alt                  );
    j.at("super"                    ).get_to(p.super                );
}






//  "Action"
//
struct Action {
//
    inline void swap(void)
    { if ( type == ActionType::CursorMove )   { std::swap(cursor.first, cursor.last); } }
//
//
//
    std::string             name            = "new action";
    std::string             descr           = "";
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
    uint8_t     type    = static_cast<uint8_t>( a.type );
    
    j = {
            {"name",                a.name                          },
            {"descr",               a.descr                         },
            {"type",                type                            },
    //
            {"cursor",              a.cursor                        },
            {"click",               a.click                         },
            {"press",               a.press                         },
            {"release",             a.release                       },
            {"drag",                a.drag                          },
            {"hotkey",              a.hotkey                        },
    //
            {"enabled",             a.enabled                       }
    };
}


inline void from_json(const nlohmann::json & j, Action & a)
{
    uint8_t     type;
    
    j.at("name"                     ).get_to(a.name                 );
    j.at("descr"                    ).get_to(a.descr                );
    j.at("type"                     ).get_to(type                   );
    j.at("cursor"                   ).get_to(a.cursor               );
    j.at("click"                    ).get_to(a.click                );
    j.at("press"                    ).get_to(a.press                );
    j.at("release"                  ).get_to(a.release              );
    j.at("drag"                     ).get_to(a.drag                 );
    j.at("hotkey"                   ).get_to(a.hotkey               );
    j.at("enabled"                  ).get_to(a.enabled              );
    
    a.type      = static_cast<ActionType>(type);
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
    "None",     "Move",     "Button Down",      "Button Up"
};



// *************************************************************************** //
//      COMPOSITION TYPES...
// *************************************************************************** //

//  "Composition_t"
//
struct Composition_t {
//
    std::vector<Action>     actions;
    std::string             name            { "New Composition"     };
    std::string             description     { "description..."      };
};

inline void to_json(nlohmann::json & j, const Composition_t & c)
{
    j = {
            {"actions",             c.actions                       },
            {"name",                c.name                          },
            {"description",         c.description                   }
    };
}

inline void from_json(const nlohmann::json & j, Composition_t & c)
{
    j.at("actions"                  ).get_to(c.actions              );
    j.at("name"                     ).get_to(c.name                 );
    j.at("description"              ).get_to(c.description          );
}









// *************************************************************************** //
//      MORE STATE TYPES...
// *************************************************************************** //

//  "KeyCaptureState"
//
struct KeyCaptureState
{
    inline void reset(void) { *this = KeyCaptureState{}; }
//
//
    bool                active              { false };   ///< in capture mode?
    HotkeyParams *      dest                { nullptr }; ///< where to write on accept

    // Temporary/live data while user is pressing keys
    ImGuiKey            key_current         { ImGuiKey_None };
    bool                ctrl                { false };
    bool                shift               { false };
    bool                alt                 { false };
    bool                super               { false };

    // Previous binding (for cancel)
    HotkeyParams        backup              {  };
};



//  "MouseCaptureState"
//
struct MouseCaptureState {
    inline void reset(void) { *this = MouseCaptureState{}; }
//
//
    bool                active              { false };          //  <   capturing?
    ImVec2 *            dest                { nullptr };        //  <   where result is written
    ImVec2 *            alt_dest            { nullptr };        //  <   the “other” endpoint
//
    GLFWwindow *        target_window       { nullptr };        //  <   window the user hovered
//
    ImVec2              live_local          {  };               //  <   live coords (local win)
    ImVec2              backup              {  };               //  <   previous value (cancel)
//
    bool                snapped_to_grid     { false };          //  <   future: grid / step
    bool                double_clicked      { false };          //  <   future: detect dbl‑click
};



//  "OverlayCache"
//
struct OverlayCache
{
    //  "update_cache"
    inline bool     update_cache        (const int action, const int composition, Action * & current)
    {
        if ( !this->is_invalid(action, composition, current) )   { return false; }
    
        m_action_sel            = action;
        m_composition_sel       = composition;
        m_action                = current;
        return true;
    }
    
    
    //  "is_valid"
    inline bool     is_valid            (const int action, const int composition, const Action * current=nullptr)
    { return ( (this->m_action_sel == action) && (this->m_composition_sel == composition) && (this->m_action == current) ); }
    
    //  "is_invalid"
    inline bool     is_invalid          (const int action, const int composition, const Action * current=nullptr)
    { return ( (this->m_action_sel != action) || (this->m_composition_sel != composition) || (this->m_action != current) ); }
    
    
    //  "reset"
    inline void     reset               (void)
    { m_action_sel = -1;    m_composition_sel = -1;    m_action = nullptr;   return; }
//
//
//
    int             m_action_sel            = -1;
    int             m_composition_sel       = -1;
    Action *        m_action                = nullptr;
};









// *************************************************************************** //
//
//
//
// *************************************************************************** //
//      STATIC FUNCTIONS...
// *************************************************************************** //

//  "want_capture_mouse_next_frame"
//
[[maybe_unused]] static inline void want_capture_mouse_next_frame(const bool state=true)
{ ImGui::SetNextFrameWantCaptureMouse(state); }      // tell Dear ImGui backend


//  "want_capture_keyboard_next_frame"
//
[[maybe_unused]] static inline void want_capture_keyboard_next_frame(const bool state=true)
{ ImGui::SetNextFrameWantCaptureKeyboard(state); }


//  "set_mod"
//
[[maybe_unused]] static void set_mod(ImGuiIO& io, ImGuiKey key, bool down)
{
    io.AddKeyEvent(key, down);                 // physical LeftAlt etc.
    switch (key)
    {
        case ImGuiKey_LeftCtrl:   io.KeyCtrl  = down; break;
        case ImGuiKey_LeftShift:  io.KeyShift = down; break;
        case ImGuiKey_LeftAlt:    io.KeyAlt   = down; break;
        case ImGuiKey_LeftSuper:  io.KeySuper = down; break;
        default: break;
    }
}


//  "enqueue_key_event"
//
[[maybe_unused]] inline void enqueue_key_event(ImGuiKey key, bool down)
{
    ImGuiIO& io = ImGui::GetIO();
    const bool prev = io.AppAcceptingEvents;
    io.AppAcceptingEvents = true;          // allow queuing in mid-frame
    io.AddKeyEvent(key, down);
    io.AppAcceptingEvents = prev;
}


//  "enqueue_modifier"
//
[[maybe_unused]] inline void enqueue_modifier(ImGuiKey left_key, bool down)
{
    enqueue_key_event(left_key, down);        // physical key
    // io.KeyCtrl / KeyAlt … will be updated by ImGui internals next frame
}


//  "InjectKey"
//
[[maybe_unused]] inline void InjectKey(ImGuiKey key, bool down)
{
    ImGuiIO& io = ImGui::GetIO();

    // Allow AddKeyEvent() even though NewFrame() has already started
    const bool prev = io.AppAcceptingEvents;
    io.AppAcceptingEvents = true;
    io.AddKeyEvent(key, down);          // down == true  → press
                                        // down == false → release
    io.AppAcceptingEvents = prev;
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

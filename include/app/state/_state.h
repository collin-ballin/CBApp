/***********************************************************************************
*
*       ********************************************************************
*       ****                S T A T E . h  ____  F I L E                ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      May 01, 2025.
*
*       ********************************************************************
*                FILE:      [include/app/state/_state.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_APP_STATE_H
#define _CBAPP_APP_STATE_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  1.1.        ** MY **  HEADERS...
#include "_config.h"
#include "cblib.h"
#include "utility/utility.h"
#include "widgets/widgets.h"
#include "app/_init.h"
#include "app/_graphing_app.h"



//  1.2     STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <cstdlib>          // C-Headers...
#include <stdio.h>
#include <unistd.h>

#include <string>           //  <======| std::string, ...
#include <string_view>
#include <vector>           //  <======| std::vector, ...
#include <stdexcept>        //  <======| ...
#include <functional>
#include <limits.h>
#include <math.h>



//  1.3     "DEAR IMGUI" HEADERS...
#include "imgui.h"
#include "implot.h"






namespace cb { namespace app { //     BEGINNING NAMESPACE "cb" :: "app"...
// *************************************************************************** //
// *************************************************************************** //



//  1.1     [ CBAPP_ENABLE_MOVE_AND_RESIZE ]    IF WINDOW MOVEMENT AND RESIZING IS #DEFINED...
//      IF "NO_MOVE_RESIZE", SETif we’re *not* in move‑and‑resize mode, disable move & resize by default:
#ifdef CBAPP_ENABLE_MOVE_AND_RESIZE
    # define _CBAPP_NO_MOVE_RESIZE_FLAGS        0
#else
    # define _CBAPP_NO_MOVE_RESIZE_FLAGS        (ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)
#endif  //  CBAPP_ENABLE_MOVE_AND_RESIZE  //






//  1.2     "HOST", "SIDEBAR",  AND "CORE" WINDOW FLAGS...
//
#define     _CBAPP_HOST_WINDOW_FLAGS            ImGuiWindowFlags_None | _CBAPP_NO_MOVE_RESIZE_FLAGS | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground
#define     _CBAPP_SIDEBAR_WINDOW_FLAGS         ImGuiWindowFlags_None | _CBAPP_NO_MOVE_RESIZE_FLAGS | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus
#define     _CBAPP_CORE_WINDOW_FLAGS            ImGuiWindowFlags_None | _CBAPP_NO_MOVE_RESIZE_FLAGS | ImGuiWindowFlags_NoCollapse
#define     _CBAPP_DEFAULT_WINDOW_FLAGS         ImGuiWindowFlags_None






//  1.3     [ CBAPP_ENABLE_CB_DEMO ]            OPTIONAL / ADDITIONAL WINDOWS...
//
#ifdef CBAPP_ENABLE_CB_DEMO                                                 // ...When CBAPP_ENABLE_CB_DEMO is defined, this expands to one extra X(...)
    #define CBAPP_OPTIONAL_WINDOW(X)                                                                                                    \
        /*| NAME.               TITLE.                  DEFAULT OPEN.       FLAGS.                                                  */  \
        /*|========================================================================================================================|*/  \
        X(  CBDemo,             "CBDemo",               true,               _CBAPP_CORE_WINDOW_FLAGS)
# else                                                                      // ...Otherwise it expands to nothing
  #define CBAPP_OPTIONAL_WINDOW(X)
#endif  //  CBAPP_ENABLE_CB_DEMO  //



// *************************************************************************** //
//
//
//  1.  FUNCTIONAL MACRO TO DEFINE APPLICATION WINDOWS...
// *************************************************************************** //
// *************************************************************************** //

#define _CBAPP_WINDOW_LIST(X)                                                                                                               \
/*| NAME.               TITLE.                      DEFAULT OPEN.       FLAGS.                                                          */  \
/*|========================================================================================================================|            */  \
/*  1.  PRIMARY GUI STRUCTURE...                                                                                                        */  \
    X(Host,             "Dear ImGui App (V0)",      true,               _CBAPP_HOST_WINDOW_FLAGS                                )           \
    X(Sidebar,          "Sidebar",                  true,               _CBAPP_SIDEBAR_WINDOW_FLAGS                             )           \
    X(Menubar,          "MenuBar",                  true,               _CBAPP_DEFAULT_WINDOW_FLAGS                             )           \
    X(MainApp,          "My Application",           true,               _CBAPP_CORE_WINDOW_FLAGS                                )           \
/*                                                                                                                                      */  \
/*  2.  MAIN APPLICATION WINDOWS...                                                                                                     */  \
    X(GraphingApp,      "Graphing App",             true,               _CBAPP_CORE_WINDOW_FLAGS                                )           \
/*                                                                                                                                      */  \
/*  3.  TOOLS, DEMOS, MISC WINDOWS, ETC...                                                                                              */  \
    X(StyleEditor,      "Style Editor",             false,              _CBAPP_DEFAULT_WINDOW_FLAGS                             )           \
    X(Logs,             "Logs",                     false,              _CBAPP_DEFAULT_WINDOW_FLAGS                             )           \
    X(Console,          "Console",                  false,              _CBAPP_DEFAULT_WINDOW_FLAGS                             )           \
    X(Metrics,          "Metrics",                  false,              _CBAPP_DEFAULT_WINDOW_FLAGS                             )           \
    X(ImGuiDemo,        "ImGui Demo",               false,              _CBAPP_DEFAULT_WINDOW_FLAGS                             )           \
    X(ImPlotDemo,       "ImPlot Demo",              false,              _CBAPP_DEFAULT_WINDOW_FLAGS                             )
/*                                                                                                                                      */  \
/*|========================================================================================================================|            */  \






// *************************************************************************** //
//
//
//  2.  MORE WINDOW DEFINITIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "Window_t"
//      Struct to defined NAMED ENUMs to identify specific windows by INDEX inside an array.
//
enum class Window_t : int {
#define X(name, title, open, flags) name,
    _CBAPP_WINDOW_LIST(X)
#undef X
    Count
};


//  "WinInfo"
//      Plain‑old‑data (POD) to hold each window’s compile‑time defaults.
struct WinInfo {
    std::string_view                                                uuid;           //  Window title / unique ID.
    ImGuiWindowFlags                                                flags;          //  ImGui window flags.
    bool                                                            open;           //  Current visibility state.
    std::function<void(const char*, bool*, ImGuiWindowFlags)>       render_fn;      //  To be bound later...
};


//  COMPILE-TIME ARRAY CONTAINING ALL APPLICATION_FONTS
//
inline static const std::array<WinInfo, size_t(Window_t::Count)>    APPLICATION_WINDOW_INFOS    = {{
#define X(name, title, open, flags)  WinInfo{ title, flags, open, {} },
    _CBAPP_WINDOW_LIST(X)
#undef X
}};






// *************************************************************************** //
// *************************************************************************** //
//          3.    PRIMARY CLASS INTERFACE.
//  Class to define the state of the application.
// *************************************************************************** //
// *************************************************************************** //

struct AppState
{
// *************************************************************************** //
// *************************************************************************** //
    
    
    // *************************************************************************** //
    //  1.               CLASS-DEFINED, NESTED TYPENAME ALIASES...
    // *************************************************************************** //
    using               Window                          = app::Window_t;
    using               ImWindows                       = utl::EnumArray<Window, WinInfo>;
    using               Font                            = app::Font_t;
    using               ImFonts                         = utl::EnumArray<Font, ImFont *>;
    //using               ImFonts                         = utl::EnumArray< Font, ImFont *, static_cast<std::size_t>(Font::Count) >;
    
    
    // *************************************************************************** //
    //  1.               CLASS MEMBER FUNCTIONS...
    // *************************************************************************** //
    
    //  Default Constructor.
    //
    AppState(void)
    {
        size_t      i       = size_t(0);
    
        // 1.   INITIALIZE WINDOW INFOS...
        for (i = 0; i < static_cast<size_t>(Window_t::Count); ++i) {
            m_windows.data[i] = APPLICATION_WINDOW_INFOS[i];
        }

        // 2.   INITIALIZE APPLICATION FONTS...
        for (i = 0; i < static_cast<size_t>(Font::Count); ++i) {
            m_fonts.data[i] = nullptr;  // load later in your init()
        }
    }
    
    //  Default Destructor.
    //
    ~AppState(void) = default;



    // *************************************************************************** //
    //  2.               CLASS DATA MEMBERS...
    // *************************************************************************** //
    
    ImWindows               m_windows;              //  1.  APPLICATION WINDOW STATE...
    ImFonts                 m_fonts;                //  2.  APPLICATION FONTS...
    
    
    //  3.      I/O CONFIG FLAGS...
    ImGuiConfigFlags        m_io_flags              = ImGuiConfigFlags_None | ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad | ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
    
    //  4.      COLORS...
    ImVec4                  m_dock_bg               = cb::app::DEF_INVISIBLE_COLOR;
    ImVec4                  m_glfw_bg               = cb::app::DEF_ROOT_WIN_BG;
    ImVec4                  m_sidebar_bg            = cb::app::DEF_SIDEBAR_WIN_BG;
    ImVec4                  m_main_bg               = cb::app::DEF_MAIN_WIN_BG;
    
    //  5.      SIDEBAR STUFF...



    //  4.  ADD MORE SHARED STATE DATA MEMBERS HERE...
    bool                    m_enable_vsync      = true;
    float                   m_ui_scale          = 1.0f;



// *************************************************************************** //
// *************************************************************************** //
};//	END "Menubar" STRUCT INTERFACE.










// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" :: "app" NAMESPACE.






#endif      //  _CBAPP_APP_STATE_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.


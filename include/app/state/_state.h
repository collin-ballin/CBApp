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
#include CBAPP_USER_CONFIG
#include "cblib.h"
#include "utility/utility.h"
#include "widgets/widgets.h"
#include "app/_init.h"



//  1.2     STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <cstdlib>          // C-Headers...
#include <stdio.h>
#include <unistd.h>
#include <chrono>

#include <string>           //  <======| std::string, ...
#include <string_view>
#include <string_view>
#include <vector>           //  <======| std::vector, ...
#include <stdexcept>        //  <======| ...
#include <functional>
#include <limits.h>
#include <math.h>



//  1.3     "DEAR IMGUI" HEADERS...
#include "imgui.h"
#include "implot.h"
#include "imgui_internal.h"






namespace cb { namespace app { //     BEGINNING NAMESPACE "cb" :: "app"...
// *************************************************************************** //
// *************************************************************************** //



//  1.1     COMPILE-TIME SPECIFICATIONS OF "WinInfo" PARAMETERS...
// *************************************************************************** //
// *************************************************************************** //

//      1.0
inline constexpr const char *           _IMGUI_DEMO_UUID                    = "Dear ImGui Demo";
inline constexpr const char *           _IMPLOT_DEMO_UUID                   = "ImPlot Demo";


//      1.1     [ CBAPP_ENABLE_MOVE_AND_RESIZE ]    IF WINDOW MOVEMENT AND RESIZING IS #DEFINED...
//              IF "NO_MOVE_RESIZE", SETif we’re *not* in move‑and‑resize mode, disable move & resize by default:
#ifdef CBAPP_ENABLE_MOVE_AND_RESIZE
    # define _CBAPP_NO_MOVE_RESIZE_FLAGS        0
#else
    # define _CBAPP_NO_MOVE_RESIZE_FLAGS        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize
#endif  //  CBAPP_ENABLE_MOVE_AND_RESIZE  //


#ifdef CBAPP_DISABLE_SAVE_WINDOW_SIZE
    # define _CBAPP_NO_MOVE_RESIZE_FLAGS        ImGuiWindowFlags_NoSavedSettings
#else
    # define _CBAPP_NO_SAVE_WINDOW_SIZE         0
#endif  //  CBAPP_DISABLE_SAVE_WINDOW_SIZE  //






//      1.2     "HOST", "SIDEBAR",  AND "CORE" WINDOW FLAGS...
//
inline constexpr ImGuiWindowFlags       _CBAPP_HOST_WINDOW_FLAGS            = ImGuiWindowFlags_None | _CBAPP_NO_MOVE_RESIZE_FLAGS | _CBAPP_NO_SAVE_WINDOW_SIZE | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;
inline constexpr ImGuiWindowFlags       _CBAPP_DOCKSPACE_WINDOW_FLAGS       = ImGuiWindowFlags_None | _CBAPP_NO_MOVE_RESIZE_FLAGS | _CBAPP_NO_SAVE_WINDOW_SIZE | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;
inline constexpr ImGuiWindowFlags       _CBAPP_CONTROLBAR_WINDOW_FLAGS      = ImGuiWindowFlags_None | _CBAPP_NO_SAVE_WINDOW_SIZE | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus;
inline constexpr ImGuiWindowFlags       _CBAPP_BROWSER_WINDOW_FLAGS         = ImGuiWindowFlags_None | _CBAPP_NO_SAVE_WINDOW_SIZE | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus;
inline constexpr ImGuiWindowFlags       _CBAPP_DETVIEW_WINDOW_FLAGS         = ImGuiWindowFlags_None | _CBAPP_NO_SAVE_WINDOW_SIZE | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
inline constexpr ImGuiWindowFlags       _CBAPP_HOME_WINDOW_FLAGS            = ImGuiWindowFlags_None | _CBAPP_NO_MOVE_RESIZE_FLAGS | _CBAPP_NO_SAVE_WINDOW_SIZE | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

inline constexpr ImGuiWindowFlags       _CBAPP_CORE_WINDOW_FLAGS            = ImGuiWindowFlags_None | _CBAPP_NO_MOVE_RESIZE_FLAGS | _CBAPP_NO_SAVE_WINDOW_SIZE | ImGuiWindowFlags_NoCollapse;


inline constexpr ImGuiWindowFlags       _CBAPP_ABOUT_WINDOW_FLAGS           = ImGuiWindowFlags_None | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
inline constexpr ImGuiWindowFlags       _CBAPP_DEFAULT_WINDOW_FLAGS         = ImGuiWindowFlags_None | ImGuiWindowFlags_NoCollapse;



// *************************************************************************** //
//
//
//  1.3     OPTIONAL / ADDITIONAL WINDOWS...        | [ CBAPP_ENABLE_CB_DEMO ]
// *************************************************************************** //
// *************************************************************************** //

#ifdef CBAPP_ENABLE_CB_DEMO                         // IF [ CBAPP_ENABLE_CB_DEMO IS #DEFINED ]      --- this expands to one extra X(...).
    #define _CBAPP_OPTIONAL_WINDOWS(X)                                                                                                      \
/*| NAME.               TITLE.                      DEFAULT OPEN.       FLAGS.                                                          */  \
/*|========================================================================================================================|            */  \
    X(CBDemo,           "CBDemo",                   true,               _CBAPP_CORE_WINDOW_FLAGS                                )
# else                                              // OTHERWISE                                    --- it expands to nothing...
    #define _CBAPP_OPTIONAL_WINDOWS(X)
#endif  //  CBAPP_ENABLE_CB_DEMO  //



// *************************************************************************** //
//
//
//  1.  FUNCTIONAL MACRO TO DEFINE APPLICATION WINDOWS...
// *************************************************************************** //
// *************************************************************************** //

//      1.1     DEFINE DEFAULT VISIBILITY OF APPLICATION WINDOWS...
#if defined(__CBAPP_BUILD_CCOUNTER_APP__)       //  CASE A :    COINCIDENCE COUNTER BUILD...
        inline constexpr bool   DEF_CCOUNTER_APP_VIS            = true;
        inline constexpr bool   DEF_FDTD_APP_VIS                = false;
//
//
# elif defined (__CBAPP_BUILD_FDTD_APP__)       //  CASE B :    FDTD APP BUILD...
        inline constexpr bool   DEF_CCOUNTER_APP_VIS            = false;
        inline constexpr bool   DEF_FDTD_APP_VIS                = true;
//
//
# else                                          //  CASE C :    NO BUILD IS SPECIFIED...
        inline constexpr bool   DEF_CCOUNTER_APP_VIS            = false;
        inline constexpr bool   DEF_FDTD_APP_VIS                = false;
//
//
#endif  //  __CBAPP_BUILD_CCOUNTER_APP__  //



//  "_CBAPP_WINDOW_LIST"
//
/// @brief      Func-style Preprocessor macro to define window flags for main application windows at compile time.
/// @param      arg1        DESC
/// @return                 DESC
///
/// @todo       TODO
//
#define _CBAPP_WINDOW_LIST(X)                                                                                                                       \
/*| NAME.                   TITLE.                          DEFAULT OPEN.           FLAGS.                                                  */      \
/*|========================================================================================================================|                */      \
/*  1.  PRIMARY GUI STRUCTURE / "CORE WINDOWS"...                                                                                           */      \
    X(Host,                 "CBApp (V0)",                   true,                   _CBAPP_HOST_WINDOW_FLAGS                                )       \
    X(Dockspace,            "##RootDockspace",              true,                   _CBAPP_DOCKSPACE_WINDOW_FLAGS                           )       \
    X(MenuBar,              "##Menubar",                    true,                   _CBAPP_DEFAULT_WINDOW_FLAGS                             )       \
    X(ControlBar,           "##ControlBar",                 true,                   _CBAPP_CONTROLBAR_WINDOW_FLAGS                          )       \
    X(Browser,              "##Browser",                    true,                   _CBAPP_BROWSER_WINDOW_FLAGS                             )       \
    X(DetailView,           "##DetailView",                 true,                   _CBAPP_DETVIEW_WINDOW_FLAGS                             )       \
    X(MainApp,              "Home",                         true,                   _CBAPP_HOME_WINDOW_FLAGS                                )       \
/*                                                                                                                                          */      \
/*                                                                                                                                          */      \
/*  2.  MAIN APPLICATION WINDOWS...                                                                                                         */      \
/*                                                                                                                                          */      \
/*          COINCIDENCE COUNTER APP...                                                                                                      */      \
    X(CCounterApp,          "Coincidence Counter",          DEF_CCOUNTER_APP_VIS,   _CBAPP_CORE_WINDOW_FLAGS                                )       \
/*                                                                                                                                          */      \
/*          FDTD APP...                                                                                                                     */      \
    X(GraphApp,             "Graph App",                    DEF_FDTD_APP_VIS,       _CBAPP_CORE_WINDOW_FLAGS                                )       \
/*                                                                                                                                          */      \
/*                                                                                                                                          */      \
/*  3.  BASIC TOOLS...                                                                                                                      */      \
    X(ImGuiStyleEditor,     "Style Editor (ImGui)",         false,                  _CBAPP_DEFAULT_WINDOW_FLAGS                             )       \
    X(ImPlotStyleEditor,    "Style Editor (ImPlot)",        false,                  _CBAPP_DEFAULT_WINDOW_FLAGS                             )       \
    X(ImGuiMetrics,         "Dear ImGui Metrics/Debugger",  false,                  _CBAPP_DEFAULT_WINDOW_FLAGS                             )       \
    X(ImPlotMetrics,        "ImPlot Metrics",               false,                  _CBAPP_DEFAULT_WINDOW_FLAGS                             )       \
/*                                                                                                                                          */      \
    X(Logs,                 "Logs",                         false,                  _CBAPP_DEFAULT_WINDOW_FLAGS                             )       \
    X(Console,              "Console",                      false,                  _CBAPP_DEFAULT_WINDOW_FLAGS                             )       \
/*                                                                                                                                          */      \
/*                                                                                                                                          */      \
/*  4.  CUSTOM TOOLS, ETC...                                                                                                                */      \
    X(ColorTool,            "Color Tool",                   false,                  _CBAPP_DEFAULT_WINDOW_FLAGS                             )       \
    X(CustomRendering,      "Custom Rendering",             true,                   _CBAPP_DEFAULT_WINDOW_FLAGS                             )       \
/*                                                                                                                                          */      \
/*                                                                                                                                          */      \
/*  5.  DEMO WINDOWS...                                                                                                                     */      \
    X(ImGuiDemo,            _IMGUI_DEMO_UUID,               false,                  _CBAPP_DEFAULT_WINDOW_FLAGS                             )       \
    X(ImPlotDemo,           _IMPLOT_DEMO_UUID,              false,                  _CBAPP_DEFAULT_WINDOW_FLAGS                             )       \
    X(AboutMyApp,           "About This App",               false,                  _CBAPP_DEFAULT_WINDOW_FLAGS                             )   // END
/*                                                                                                                                          */
/*|========================================================================================================================|                */






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
    _CBAPP_OPTIONAL_WINDOWS(X)
#undef X
    Count
};


//  "WinInfo"
//      Plain‑old‑data (POD) to hold each window’s compile‑time defaults.
struct WinInfo {
    inline const char * get_uuid(void) const { return this->uuid.c_str(); }
    std::string                                                     uuid;           //  Window title / unique ID.
    ImGuiWindowFlags                                                flags;          //  ImGui window flags.
    bool                                                            open;           //  Current visibility state.
    std::function<void(const char*, bool*, ImGuiWindowFlags)>       render_fn;      //  To be bound later...
};


//  COMPILE-TIME ARRAY CONTAINING ALL APPLICATION_FONTS
//
inline static const std::array<WinInfo, size_t(Window_t::Count)>    APPLICATION_WINDOW_INFOS    = {{
#define X(name, title, open, flags)  WinInfo{ title, flags, open, {} },
    _CBAPP_WINDOW_LIST(X)
    _CBAPP_OPTIONAL_WINDOWS(X)
#undef X
}};






// *************************************************************************** //
// *************************************************************************** //
//          3.    PRIMARY CLASS INTERFACE.
//  Class to define the state of the application.
// *************************************************************************** //
// *************************************************************************** //


//      3.1     APPLICATION APIs AND INTERNAL TYPENAME ALIASES...
// *************************************************************************** //
// *************************************************************************** //

//  MEMBER VARIABLE NAME FOR "AppState" OBJECT IN EACH CLASS...
//      - Using this to consosolidate the lengthy member-accessor statements "this->m_state.my_object.my_data_member", etc ...
#define                 CBAPP_STATE_NAME                S


//  INTERNAL "API" USED **EXCLUSIVELY** INSIDE "AppState" CLASS.
//      - CLASS-NESTED, PUBLIC TYPENAME ALIASES FOR "CBApp" CLASSES THAT UTILIZE AN "AppState" OBJECT...
//
#define                 _CBAPP_APPSTATE_ALIAS_API                                                               \
public:                                                                                                         \
    using               Window                          = app::Window_t;                                        \
    using               Font                            = app::Font_t;                                          \
    using               Cmap                            = app::Colormap_t;                                      \
    using               Timestamp_t                     = std::chrono::time_point<std::chrono::system_clock>;   \
                                                                                                                \
    using               ImFonts                         = utl::EnumArray<Font, ImFont *>;                       \
    using               ImWindows                       = utl::EnumArray<Window, WinInfo>;                      \
    using               Anchor                          = utl::Anchor;                                          \
                                                                                                                \
    using               Logger                          = utl::Logger;                                          \
    using               LogLevel                        = Logger::Level;                                        \
    using               Tab_t                           = utl::Tab_t;


//  EXPORTED "API" USED BY "CBApp" DELEGATOR CLASSES THAT USE AN "AppState" OBJECT.
//      - CLASS-NESTED, PUBLIC TYPENAME ALIASES FOR "CBApp" CLASSES THAT UTILIZE AN "AppState" OBJECT...
//
#define                 CBAPP_APPSTATE_ALIAS_API                                                \
    using               AppState                        = app::AppState;                        \
    using               WinInfo                         = app::WinInfo;                         \
                                                        _CBAPP_APPSTATE_ALIAS_API

/*
DEF_COLORMAPS []   = {
//  1.  Perceptually Uniform Sequential Colormaps.
    Inferno,            Magma,                  Cividis,
//
//  2.  Sequential Colormaps.
    Blues,              YlOrRd,
//  3.  Sequential (2) Colormaps.
    Gist_heat,
//
//  4.  Diverging Colormaps.
    Berlin,             Managua,                Vanimo,
//
//  5.  Cyclic Colormaps.
    Csv,
//
//  6.  Qualitative Colormaps.
//
//  7.  Miscellaneous Colormaps.
    Ocean,              Gist_earth,             Gist_stern,             Gnuplot,                Gnuplot2,
    CMRmap,             Gist_rainbow,           Turbo,                  Nipy_spectral,
//
//  8.  Custom Colormaps.
    Perm_B,             Perm_E
};*/



/*
enum ImPlotColormap_ {
    ImPlotColormap_Deep     = 0,   // a.k.a. seaborn deep             (qual=true,  n=10) (default)
    ImPlotColormap_Dark     = 1,   // a.k.a. matplotlib "Set1"        (qual=true,  n=9 )
    ImPlotColormap_Pastel   = 2,   // a.k.a. matplotlib "Pastel1"     (qual=true,  n=9 )
    ImPlotColormap_Paired   = 3,   // a.k.a. matplotlib "Paired"      (qual=true,  n=12)
    ImPlotColormap_Viridis  = 4,   // a.k.a. matplotlib "viridis"     (qual=false, n=11)
    ImPlotColormap_Plasma   = 5,   // a.k.a. matplotlib "plasma"      (qual=false, n=11)
    ImPlotColormap_Hot      = 6,   // a.k.a. matplotlib/MATLAB "hot"  (qual=false, n=11)
    ImPlotColormap_Cool     = 7,   // a.k.a. matplotlib/MATLAB "cool" (qual=false, n=11)
    ImPlotColormap_Pink     = 8,   // a.k.a. matplotlib/MATLAB "pink" (qual=false, n=11)
    ImPlotColormap_Jet      = 9,   // a.k.a. MATLAB "jet"             (qual=false, n=11)
    ImPlotColormap_Twilight = 10,  // a.k.a. matplotlib "twilight"    (qual=false, n=11)
    ImPlotColormap_RdBu     = 11,  // red/blue, Color Brewer          (qual=false, n=11)
    ImPlotColormap_BrBG     = 12,  // brown/blue-green, Color Brewer  (qual=false, n=11)
    ImPlotColormap_PiYG     = 13,  // pink/yellow-green, Color Brewer (qual=false, n=11)
    ImPlotColormap_Spectral = 14,  // color spectrum, Color Brewer    (qual=false, n=11)
    ImPlotColormap_Greys    = 15,  // white/black                     (qual=false, n=2 )
};*/



enum Colormap_t : int {
    IMPLOT_END  = 16,
    //  1.  Perceptually Uniform Sequential Colormaps.
    Viridis                 = ImPlotColormap_Viridis,
    Plasma                  = ImPlotColormap_Plasma,
    Inferno                 = IMPLOT_END + 1,
    Magma,                  Cividis,
    //
    //  2.  Sequential Colormaps.
    Greys                   = ImPlotColormap_Greys,
    Blues                   = Cividis + 1,
    YlOrRd,
    //
    //  3.  Sequential (2) Colormaps.
    Pink                    = ImPlotColormap_Pink,
    Cool                    = ImPlotColormap_Cool,
    Hot                     = ImPlotColormap_Hot,
    Berlin                  = YlOrRd + 1,
    Managua,                Vanimo,                     Gist_heat,
    //
    //  4.  Diverging Colormaps.
    Pi_YG                   = ImPlotColormap_PiYG,
    BrBg                    = ImPlotColormap_BrBG,
    RdBu                    = ImPlotColormap_RdBu,
    Spectral                = ImPlotColormap_Spectral,
    //
    //  5.  Cyclic Colormaps.
    Csv                     = Gist_heat + 1,
    //
    //  6.  Qualitative Colormaps.
    Set1                    = ImPlotColormap_Dark,
    Paired                  = ImPlotColormap_Paired,
    Pastel1                 = ImPlotColormap_Pastel,
    //
    //  7.  Miscellaneous Colormaps.
    Ocean                   = Csv + 1,
    Gist_earth,             Gist_stern,                 Gnuplot,
    Gnuplot2,               CMRmap,                     Gist_rainbow,               Turbo,
    Jet                     = ImPlotColormap_Jet,
    
    Nipy_spectral           = Turbo,
    //
    //  8.  Custom Colormaps.
    Perm_E,                 Perm_B,
    //
    //
    Count
};



//  "AppState"
//      PLAIN-OLD-DATA (POD) ABSTRACTION/CONTAINER TO
//      FACILITATE MANUAL DEPENDENCY INJECTION DESIGN PATTERN...
//
struct AppState
{
    _CBAPP_APPSTATE_ALIAS_API       //  CLASS-DEFINED, NESTED TYPENAME ALIASES.
// *************************************************************************** //
// *************************************************************************** //
    
    // *************************************************************************** //
    //  1.1     STRUCT INITIALIZATION FUNCTIONS...
    // *************************************************************************** //
                                        AppState                    (void);         //  Default Constructor.
                                        ~AppState                   (void);         //  Default Destructor.
    
    
    // *************************************************************************** //
    //  1.2     STRUCT UTILITY FUNCTIONS...
    // *************************************************************************** //
    void                                SetDarkMode                 (void);
    void                                SetLightMode                (void);
    void                                LoadCustomColorMaps         (void);
    
    
    void                                DockAtHome                  (const Window &);
    void                                DockAtHome                  (const char *);
    void                                DockAtDetView               (const Window &);
    void                                DockAtDetView               (const char *);
    void                                PushFont                    ([[maybe_unused]] const Font & );
    void                                PopFont                     (void);



    // *************************************************************************** //
    //  2.               CLASS DATA MEMBERS...
    // *************************************************************************** //
    
    //  0.      GROUPS / SUB-CLASSES OF "APPSTATE"...
    utl::Logger &                       m_logger;                                                   //  1.      LOGGER...
    ImWindows                           m_windows;                                                  //  2.      APPLICATION WINDOW STATE...
    std::vector<WinInfo *>              m_detview_windows           = {};                           //  2.1     WINDOWS INSIDE DETAIL VIEW...
    ImFonts                             m_fonts;                                                    //  3.      APPLICATION FONTS...
    std::vector< std::pair<std::chrono::time_point<std::chrono::system_clock>, std::string> >
                                         m_notes                    = {};
    
    
    
    //  1.      STATIC AND CONSTEXPR VARIABLES...
    //
    //          1.  Window Variables:
    //              1.1     ALL Windows.
    static constexpr size_t             ms_WINDOWS_BEGIN            = static_cast<size_t>(Window::Dockspace);
    static constexpr size_t             ms_RHS_WINDOWS_BEGIN        = static_cast<size_t>(Window::MainApp);
    static constexpr size_t             ms_WINDOWS_END              = static_cast<size_t>(Window::Count);
    //
    //              1.2     Main Application Windows.
    static constexpr size_t             ms_APP_WINDOWS_BEGIN        = static_cast<size_t>(Window::CCounterApp);
    static constexpr size_t             ms_APP_WINDOWS_END          = static_cast<size_t>(Window::ImGuiStyleEditor);
    //
    //              1.3     Basic Tool Windows.
    static constexpr size_t             ms_TOOL_WINDOWS_BEGIN       = static_cast<size_t>(Window::ImGuiStyleEditor);
    static constexpr size_t             ms_TOOL_WINDOWS_END         = static_cast<size_t>(Window::ColorTool);
    //
    //              1.4     "MY Tools" / Custom Tools Windows.
    static constexpr size_t             ms_MY_TOOLS_WINDOWS_BEGIN   = static_cast<size_t>(Window::ColorTool);
    static constexpr size_t             ms_MY_TOOLS_WINDOWS_END     = static_cast<size_t>(Window::ImGuiDemo);
    //
    //              1.5     Demo Windows.
    static constexpr size_t             ms_DEMO_WINDOWS_BEGIN       = static_cast<size_t>(Window::ImGuiDemo);
    static constexpr size_t             ms_DEMO_WINDOWS_END         = static_cast<size_t>(Window::Count);
    
    
    //  2.      MISC. INFORMATION...
    bool                                m_enable_vsync              = true;
    const char *                        m_glsl_version              = nullptr;
    LogLevel                            m_LogLevel                  = LogLevel::Debug;
    
    
    
    //  3.      GLFW AND HOST WINDOW STUFF...
    ImGuiConfigFlags                    m_io_flags                  = ImGuiConfigFlags_None | ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad | ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
    ImGuiViewport *                     m_main_viewport             = nullptr;
    GLFWwindow *                        m_glfw_window               = nullptr;
    
    
    //  4.      COLORS...
    ImVec4                              m_glfw_bg                   = cb::app::DEF_ROOT_WIN_BG;
    ImVec4                              m_dock_bg                   = cb::app::DEF_INVISIBLE_COLOR;
    ImVec4                              m_main_bg                   = cb::app::DEF_MAIN_WIN_BG;
    //
    ImVec4                              m_controlbar_bg             = cb::app::DEF_CONTROLBAR_WIN_BG;
    ImVec4                              m_browser_bg                = cb::app::DEF_BROWSER_WIN_BG;
    ImVec4                              m_detview_bg                = cb::app::DEF_DETVIEW_WIN_BG; 
    
    
    //  5.      DIMENSIONS...
    //
    //              6.1     System.
    int                                 m_system_w                  = -1;       //  Sys. Display Dims.
    int                                 m_system_h                  = -1;
    float                               m_dpi_scale                 = 1.0f;
    float                               m_dpi_fontscale             = 1.0f;
    //
    //              6.2     Main UI.
    //                              Main Window Dims.
    int                                 m_window_w                  = 1280;
    int                                 m_window_h                  = 720;
    
    
    //  6.      BOOLEANS...
    //
    bool                                m_running                   = true;
    bool                                m_rebuild_dockspace         = false;
    //
    bool                                m_show_controlbar_window    = true;
    bool                                m_show_browser_window       = true;
    bool                                m_show_detview_window       = true;
    //
    bool                                m_show_system_preferences   = false;
    
    
    //  7.      SPECIFICS...
    //
    //              8.1     Main Dockingspace.
    const char *                        m_dock_name                 = "##RootDockspace";
    ImGuiID                             m_dockspace_id              = 0;
    //
    ImGuiID                             m_main_dock_id              = 0;
    ImGuiDockNodeFlags                  m_main_node_flags           = ImGuiDockNodeFlags_NoDockingOverOther | ImGuiDockNodeFlags_CentralNode; //ImGuiDockNodeFlags_NoDocking; ImGuiDockNodeFlags_NoDockingOverMe
    ImGuiDockNode *                     m_main_node                 = nullptr;
    //
    //              8.2     ControlBar.
    ImGuiID                             m_controlbar_dock_id        = 0;
    ImGuiDockNodeFlags                  m_controlbar_node_flags     = ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_HiddenTabBar | ImGuiDockNodeFlags_NoResize; //ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoCloseButton;      //  ImGuiDockNodeFlags_NoSplit
    ImGuiDockNode *                     m_controlbar_node           = nullptr;
    //
    float                               m_controlbar_ratio          = 0.02;
    //
    //              8.3     Sidebar.
    ImGuiID                             m_browser_dock_id           = 0;
    ImGuiDockNodeFlags                  m_browser_node_flags        = ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoCloseButton;      //  ImGuiDockNodeFlags_NoSplit
    ImGuiDockNode *                     m_browser_node              = nullptr;
    //
    float                               m_browser_ratio             = app::DEF_BROWSER_RATIO;
    //
    //              8.4     DetView.
    ImGuiID                             m_detview_dock_id           = 0;
    ImGuiDockNodeFlags                  m_detview_node_flags        = ImGuiDockNodeFlags_NoDockingOverOther | ImGuiDockNodeFlags_NoCloseButton;      //  ImGuiDockNodeFlags_NoSplit
    ImGuiDockNode *                     m_detview_node              = nullptr;
    //
    float                               m_detview_ratio             = 0.45f;
    //
    //
    //              9.1     DETVIEW DOCKSPACE.
    static constexpr const char *       m_detview_dockspace_uuid    = "##DetailViewDockspace";
    ImGuiID                             m_detview_dockspace_id      = 0;
    ImGuiDockNodeFlags                  m_detview_dockspace_flags   = ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_HiddenTabBar;      //  ImGuiDockNodeFlags_NoSplit
    ImGuiDockNodeFlags                  m_detview_window_flags      = ImGuiDockNodeFlags_NoTabBar;      //  ImGuiDockNodeFlags_NoSplit
    


    //  99.     ADD MORE SHARED STATE DATA MEMBERS HERE...
    //
    //              1.1     SubCategory.
    //                          ...
    //



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


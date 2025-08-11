/***********************************************************************************
*
*       ********************************************************************
*       ****               _ T Y P E S . h  ____  F I L E               ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      June 22, 2025.
*
*       ********************************************************************
*                FILE:      [include/app/state/_state.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_APP_STATE_TYPES_H
#define _CBAPP_APP_STATE_TYPES_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  1.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
#include "utility/utility.h"
#include "widgets/widgets.h"
#include "app/state/_config.h"
#include "app/state/_constants.h"
//  #include "app/_init.h"



//  1.2     STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <cstdlib>          // C-Headers...
#include <stdio.h>
#include <unistd.h>
#include <cstdint>
#include <chrono>

#include <string>           //  <======| std::string, ...
#include <string_view>
#include <string_view>
#include <vector>           //  <======| std::vector, ...
#include <stdexcept>        //  <======| ...
#include <functional>
#include <limits.h>
#include <math.h>
#include <atomic>



//  1.3     "DEAR IMGUI" HEADERS...
#include "imgui.h"
#include "implot.h"
#include "imgui_internal.h"



namespace cb { namespace app { //     BEGINNING NAMESPACE "cb" :: "app"...
// *************************************************************************** //
// *************************************************************************** //



//      1.      APPLICATION UTILITY STRUCTS...
// *************************************************************************** //
// *************************************************************************** //

//  "AppColorStyle_t"
//
enum class AppColorStyle_t : uint8_t {
    Default = 0,
    LightMode,
    DarkMode,
//
    Laser_410NM,
//
    ImGuiLight,
    ImGuiDark,
    ImGuiClassic,
//
    Count
};
//
//  "APPLICATION_COLOR_STYLE_NAMES"
//      COMPILE-TIME ARRAY CONTAINING THE NAME OF ALL STYLES.
inline static const std::array<const char *, static_cast<size_t>(AppColorStyle_t::Count)>
APPLICATION_COLOR_STYLE_NAMES           = {{
    "Default",                      "Light Mode",                   "Dark Mode",
    "410 nm",
//
    "ImGui Light Mode",             "ImGui Dark Mode",              "ImGui Classic"
}};



//  "PlotColorStyle_t"
//
enum class PlotColorStyle_t : uint8_t {
    Default = 0,
    LightMode,
    DarkMode,
//
    Count
};
//
//  "APPLICATION_PLOT_COLOR_STYLE_NAMES"
//      COMPILE-TIME ARRAY CONTAINING THE NAME OF ALL STYLES.
inline static const std::array<const char *, static_cast<size_t>(PlotColorStyle_t::Count)>
APPLICATION_PLOT_COLOR_STYLE_NAMES      = {{
    "Default",                      "Light Mode",                   "Dark Mode"
}};



//  "AppInfo"
//
// struct AppInfo {
//     std::vector< std::pair<Timestamp_t, std::string> >
//                                         m_notes                     = {};
//     Timestamp_t                         m_timestamp_spawn;
//     Timestamp_t                         m_timestamp_start;
// 
// };



//
//
// ******************************************************************* //
// *************************************************************************** //   END "App Utility".












// *************************************************************************** //
//
//
//
//      2.      MORE WINDOW DEFINITIONS...
// *************************************************************************** //
// *************************************************************************** //

// *************************************************************************** //
//      2A.  MORE WINDOW DEFINITIONS...
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
//
    std::string                                                     uuid;           //  Window title / unique ID.
    ImGuiWindowFlags                                                flags;          //  ImGui window flags.
    bool                                                            open;           //  Current visibility state.
    std::function<void(const char*, bool*, ImGuiWindowFlags)>       render_fn;      //  To be bound later...
};


//  COMPILE-TIME ARRAY CONTAINING ALL APPLICATION WINDOWS...
//
static inline const std::array<WinInfo, size_t(Window_t::Count)>
APPLICATION_WINDOW_INFOS            = {{
#define X(name, title, open, flags)  WinInfo{ title, flags, open, {} },
    _CBAPP_WINDOW_LIST(X)
    _CBAPP_OPTIONAL_WINDOWS(X)
#undef X
}};



// *************************************************************************** //
//      2B.  TASK IDENTIFIERS...
// *************************************************************************** //

//  "Applet_t"
//
enum class Applet_t : uint8_t {
    MainApp = 0,
    CCounterApp,
    EditorApp,
    GraphApp,
//
    COUNT
};



// *************************************************************************** //
//      2C.  SIGNAL IDENTIFIERS...
// *************************************************************************** //

//  "SignalBits"
//      One bit per “operation” we may queue.
//
enum CBSignalFlags_ {
    CBSignalFlags_None          = 0,
//
    CBSignalFlags_Shutdown      = 1u << 0,      // SIGINT / SIGTERM / CTRL_C_EVENT.
    CBSignalFlags_ReloadCfg     = 1u << 1,      // SIGHUP.
    CBSignalFlags_NewFailure    = 1u << 2,      // [OPERATOR NEW] / MALLOC FAILURE.
//
    CBSignalFlags_Custom1       = 1u << 3,      // SIGUSR1 demo.
//
    CBSignalFlags_COUNT
};

//  Bitwise helpers.
//
//      constexpr SignalBits        operator |     (SignalBits a, SignalBits b) noexcept
//      { return CBSignalFlags_(uint32_t(a) | uint32_t(b)); }
//
//      constexpr uint32_t to_uint(SignalBits v) noexcept
//      { return static_cast<uint32_t>(v); }

//
//
// ******************************************************************* //
// *************************************************************************** //   END "MORE WINDOW DEFINITIONS".












// *************************************************************************** //
//
//
//
//      3.      "MenuState" AUXILIARY TYPES...
// *************************************************************************** //
// *************************************************************************** //

//  "CBMenuCapabilityFlags_"
//
enum CBMenuCapabilityFlags_ : uint64_t {
    CBMenuCapabilityFlags_None			        = 0,
//
//
    CBMenuCapabilityFlags_Open                  = 1u <<  0,         //  "File" Menu.
    CBMenuCapabilityFlags_Save			        = 1u <<  1,
    CBMenuCapabilityFlags_SaveAs			    = 1u <<  2,
//
    CBMenuCapabilityFlags_Undo		            = 1u <<  3,         //  "Edit" Menu.
    CBMenuCapabilityFlags_Redo                  = 1u <<  4,
    CBMenuCapabilityFlags_Copy                  = 1u <<  5,
    CBMenuCapabilityFlags_Paste		            = 1u <<  6,
//
//
    CBMenuCapabilityFlags_CustomFile		    = 1u <<  7,         //  "Custom" Flags...
    CBMenuCapabilityFlags_CustomEdit		    = 1u <<  8,
    CBMenuCapabilityFlags_CustomView		    = 1u <<  9,
    CBMenuCapabilityFlags_CustomWindow	        = 1u << 10,
    CBMenuCapabilityFlags_CustomHelp		    = 1u << 11,
//
//
    CBMenuCapabilityFlags_COUNT
};


//  "MenuCallbacks"
//
struct MenuCallbacks {
	std::function<void()>	    file;		    //  Called inside   "File"      menu.
	std::function<void()>	    edit;		    //  Called inside   "Edit"      menu.
	std::function<void()>	    view;		    //  Called inside   "View"      menu.
    std::function<void()>       window;         //  Called inside   "Window"    menu.
	std::function<void()>	    tools;		    //  Called inside   "Tools"     menu.
	std::function<void()>	    help;		    //  Called inside   "Help"      menu.
};



//
//
// ******************************************************************* //
// *************************************************************************** //   END "MenuState Types".












// *************************************************************************** //
//
//
//
//      4.      COLOR STYLES AND TYPES...
// *************************************************************************** //
// *************************************************************************** //

//  "AppleSystemColors_"
/*
enum class AppleSystemColors_ : uint8_t {
    Gray = 0,       Pink,           Red,            Orange,         Yellow,
    Green,          Brown,          Blue,           Purple,         Count
};
//
//  "DEF_APPLE_SYSTEM_COLORS"
static constexpr cblib::EnumArray<AppleSystemColors_, ImU32>
DEF_APPLE_SYSTEM_COLORS = {
    0xFF98989D,     //  sys_gray
    0xFFFF375F,     //  sys_pink
    0xFFFF453A,     //  sys_red
    0xFFFF9F0A,     //  sys_orange
    0xFFFFD60A,     //  sys_yellow
    0xFF32D74B,     //  sys_green
    0xFFAC8E68,     //  sys_brown
    0xFF0A84FF,     //  sys_blue
    0xFFBF5AF2,     //  sys_purple
};*/


//  "AppleSystemColors_t"
//
struct AppleSystemColors_t {
    static constexpr ImVec4     Gray        = ImVec4(   0.596f,     0.596f,     0.616f,     1.000f      );
    static constexpr ImVec4     Pink        = ImVec4(   1.000f,     0.216f,     0.373f,     1.000f      );
    static constexpr ImVec4     Red         = ImVec4(   1.000f,     0.271f,     0.227f,     1.000f      );
    static constexpr ImVec4     Orange      = ImVec4(   1.000f,     0.624f,     0.039f,     1.000f      );
    static constexpr ImVec4     Yellow      = ImVec4(   1.000f,     0.839f,     0.039f,     1.000f      );
    static constexpr ImVec4     Green       = ImVec4(   0.196f,     0.843f,     0.294f,     1.000f      );
    static constexpr ImVec4     Brown       = ImVec4(   0.675f,     0.557f,     0.408f,     1.000f      );
    static constexpr ImVec4     Blue        = ImVec4(   0.039f,     0.518f,     1.000f,     1.000f      );
    static constexpr ImVec4     Purple      = ImVec4(   0.749f,     0.353f,     0.949f,     1.000f      );
};






// *************************************************************************** //
//      4.  COLORMAP TYPES...
// *************************************************************************** //

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



//
//
// ******************************************************************* //
// *************************************************************************** //   END "Color Types".












// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" :: "app" NAMESPACE.












#endif      //  _CBAPP_APP_STATE_TYPES_H  //
// *************************************************************************** //
// *************************************************************************** //  END.

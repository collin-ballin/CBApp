/***********************************************************************************
*
*       ********************************************************************
*       ****              _ C O N F I G . h  ____  F I L E              ****
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
#ifndef _CBAPP_APP_STATE_CONFIG_H
#define _CBAPP_APP_STATE_CONFIG_H  1



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

//  "get_app_title"
//
//  inline constexpr const std::string_view get_app_title(void) {
//      constexpr std::string_view  title       = "CBApp (";
//      constexpr std::string_view  version     = std::string_view(__CBAPP_VERSION__);
//      constexpr std::string_view  build       = std::string_view(__CBAPP_BUILD__);
//
//      //cblib::utl::strcat_string_view_cx<title, version, build, >;
//
//      constexpr std::string_view  result      = cblib::utl::strcat_string_view_cx< title, version, build >;
//      return result;
//      //return std::string_view("CBApp (") + std::string_view(version) + " [" + std::string(build) + "])";
//  }



//      1.0
//  inline constexpr const char *           _CBAPP_APP_NAME []                  = __CBAPP_VERSION__ + __CBAPP_BUILD__;
//  static inline constexpr std::string_view        _CBAPP_APP_TITLE_IMPL           =  get_app_title();
inline constexpr const char *                   _CBAPP_APP_TITLE                =  "CBApp (V0.3.0 WIP)";    //std::string( _CBAPP_APP_TITLE_IMPL );
//
//
//      1.1
inline constexpr const char *                   _IMGUI_DEMO_UUID                = "Dear ImGui Demo";
inline constexpr const char *                   _IMPLOT_DEMO_UUID               = "ImPlot Demo";
//
//
//      1.3     [ CBAPP_ENABLE_MOVE_AND_RESIZE ]    IF WINDOW MOVEMENT AND RESIZING IS #DEFINED...
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
inline constexpr ImGuiWindowFlags       _CBAPP_DETVIEW_WINDOW_FLAGS         = ImGuiWindowFlags_None | _CBAPP_NO_SAVE_WINDOW_SIZE | ImGuiWindowFlags_NoCollapse;

inline constexpr ImGuiWindowFlags       _CBAPP_HOME_WINDOW_FLAGS            = ImGuiWindowFlags_None | _CBAPP_NO_MOVE_RESIZE_FLAGS | _CBAPP_NO_SAVE_WINDOW_SIZE | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;


inline constexpr ImGuiWindowFlags       _CBAPP_EDITOR_WINDOW_FLAGS          = ImGuiWindowFlags_None | _CBAPP_NO_MOVE_RESIZE_FLAGS | _CBAPP_NO_SAVE_WINDOW_SIZE | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
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
        inline constexpr bool   DEF_EDITOR_APP_VIS              = false;
//
//
# elif defined (__CBAPP_BUILD_FDTD_APP__)       //  CASE B :    FDTD APP BUILD...
        inline constexpr bool   DEF_CCOUNTER_APP_VIS            = false;
        inline constexpr bool   DEF_FDTD_APP_VIS                = true;
        inline constexpr bool   DEF_EDITOR_APP_VIS              = false;
//
//
# elif defined (__CBAPP_BUILD_EDITOR_APP__)     //  CASE C :    EDITOR / "ILLUSTRATOR" APP BUILD...
        inline constexpr bool   DEF_CCOUNTER_APP_VIS            = false;
        inline constexpr bool   DEF_FDTD_APP_VIS                = false;
        inline constexpr bool   DEF_EDITOR_APP_VIS              = true;
//
//
# else                                          //  CASE D :    NO BUILD IS SPECIFIED...
        inline constexpr bool   DEF_CCOUNTER_APP_VIS            = false;
        inline constexpr bool   DEF_FDTD_APP_VIS                = false;
        inline constexpr bool   DEF_EDITOR_APP_VIS              = false;
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
    X(Host,                 _CBAPP_APP_TITLE,               true,                   _CBAPP_HOST_WINDOW_FLAGS                                )       \
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
/*          EDITOR APP...                                                                                                                   */      \
    X(EditorApp,            "Editor App",                   DEF_EDITOR_APP_VIS,     _CBAPP_EDITOR_WINDOW_FLAGS                              )       \
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
    X(CustomRendering,      "Custom Rendering",             false,                  _CBAPP_DEFAULT_WINDOW_FLAGS                             )       \
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
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" :: "app" NAMESPACE.






#endif      //  _CBAPP_APP_STATE_CONFIG_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.

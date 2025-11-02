/**
 * @file    _types.h
 * @brief   Place to put all the Enum types and structs that are needed throughout the application.
 * @author  Collin A. Bond
 * @date    2025-05-21
 *
 * @details     Only containing POD structs and Enum Types who's implementation
 *              is contained entirely within the header file only.
 */
/***********************************************************************************
*
*       ********************************************************************
*       ****               _ T Y P E S . H  ____  F I L E               ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      May 21, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_TYPES_H
#define _CBAPP_TYPES_H  1



//  0.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
//  #include "utility/_constants.h"
//  #include "utility/_templates.h"
//  #include "utility/_logger.h"
//  #ifdef _WIN32
//      # include "utility/resource_loader.h"
//  #endif  //  _WIN32  //
//  //#include "utility/pystream/pystream.h"
#include "json.hpp"


//  0.2     STANDARD LIBRARY HEADERS...
#include <stdio.h>
#include <vector>
#include <thread>
#include <fstream>
#include <filesystem>
#include <string>
#include <string_view>
#include <type_traits>
#include <math.h>


//  0.3     "DEAR IMGUI" HEADERS...
#include "imgui.h"                      //  0.3     "DEAR IMGUI" HEADERS...
#include "imgui_internal.h"
#include "implot.h"
#include "implot_internal.h"






namespace cb { namespace utl { //     BEGINNING NAMESPACE "cb" :: "utl"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//
//
//  1.      WIDGETS & WINDOW STRUCTS...
// *************************************************************************** //
// *************************************************************************** //



//          1A.     Widget-Table / Control-Table Stuff:
// *************************************************************************** //
// *************************************************************************** //

//  "WidgetRow"
//      Declarative per-row widget logic
struct WidgetRow {
    const char *            label               = nullptr;
    std::function<void()>   render              = nullptr;
};


//  "CellSpec"
//
struct CellSpec {
    const char*                                                         label {""};
    std::function<void(const ImVec2& widget_pos, const ImVec2& size)>   draw;
};


//  "ColumnCFG"
//
struct ColumnCFG {
    const char *            uuid                = nullptr;
    float                   width               = -1.0f;
    ImGuiTableColumnFlags   flags               = ImGuiTableColumnFlags_None;
};


//  "TableCFG"
//      - POD struct to define that STATE INFORMATION of a table in Dear ImGui.
template<auto NC>
struct TableCFG {
    const char *            uuid                = nullptr;
    ImGuiTableFlags         table_flags         = ImGuiTableFlags_None | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoKeepColumnsVisible | ImGuiTableFlags_SizingStretchProp; // ImGuiTableFlags_SizingStretchWeight
    ColumnCFG               columns [NC]        = {};
    bool                    header_row          = false;
};

template <>
struct TableCFG<2> {
    const char *            uuid                = nullptr;
    ImGuiTableFlags         table_flags         = ImGuiTableFlags_None | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoKeepColumnsVisible | ImGuiTableFlags_SizingStretchProp; // ImGuiTableFlags_SizingStretchWeight
    ColumnCFG               columns [2]         = {
                                { "Label",          200.0f,         ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize },
                                { "Widget",         -1.0f,          ImGuiTableColumnFlags_WidthStretch }
                            };
    bool                    header_row          = false;
};



//          1B.     Tab-Bar Stuff:
// *************************************************************************** //
// *************************************************************************** //

//  "Tab_t"
//
struct Tab_t    {
    using                   callback_t      = std::function<void(const char*, bool*, ImGuiWindowFlags)>;
    inline const char *     get_uuid        (void) const    { return this->uuid.c_str(); }
//
    std::string             uuid;
    bool                    open;
    bool                    no_close;
    ImGuiTabItemFlags       flags;
    callback_t              render_fn;
};


//          1C.     Window Information / Position Information:
// *************************************************************************** //
// *************************************************************************** //

//  "WindowLocation"
//
enum class WindowLocation : uint8_t {
    Center = 0,         //  default.
    LeftHalf,           //  left half of the monitor.
    RightHalf,          //  right half of the monitor.
    Fill,               //  Expand window to fill the entire screen.
    COUNT
};




//  "Anchor"
//
enum class Anchor : uint8_t {
      Center = 0
    , East, NorthEast, North, NorthWest, West, SouthWest, South, SouthEast
//
    , COUNT
};


//  "DEF_ANCHOR_NAMES"
//
static constexpr cblib::EnumArray< Anchor, const char * >
DEF_ANCHOR_NAMES = { {
    /*  Center / North      */    "Center"
    /*  East                */  , "East"
    /*  NorthEast           */  , "North-East"
    /*  North               */  , "North"
    /*  NorthWest           */  , "North-West"
    /*  West                */  , "West"
    /*  SouthWest           */  , "South-West"
    /*  South               */  , "South"
    /*  SouthEast           */  , "South-East"
} };


//  "DEF_ANCHOR_RADIAN_ANGLES"
//
static constexpr cblib::EnumArray< Anchor, double >
DEF_ANCHOR_RADIAN_ANGLES = { {
    /*  Center / North      */     0.0                   //  (0π)    [rad]   ==   0°   [deg]
    /*  East                */  ,  1.5707963267948966    //  (π/2)   [rad]   ==  90°   [deg]
    /*  NorthEast           */  ,  0.7853981633974483    //  (π/4)   [rad]   ==  45°   [deg]
    /*  North               */  ,  0.0                   //  (0π)    [rad]   ==   0°   [deg]
    /*  NorthWest           */  , -0.7853981633974483    //  (-π/4)  [rad]   == -45°   [deg]
    /*  West                */  , -1.5707963267948966    //  (-π/2)  [rad]   == -90°   [deg]
    /*  SouthWest           */  , -2.356194490192345     //  (-3π/4) [rad]   == -135°  [deg]
    /*  South               */  ,  3.141592653589793     //  (π)     [rad]   ==  180°  [deg]
    /*  SouthEast           */  ,  2.356194490192345     //  (3π/4)  [rad]   ==  135°  [deg]
} };





// *************************************************************************** //
//
//
//
//      2.      PLOTS AND GRAPHS...
// *************************************************************************** //
// *************************************************************************** //

//  "GraphCFG"
//
struct GraphCFG {
    const char *                uuid                = nullptr;
    ImVec2                      size                = ImVec2(-1.0f, -1.0f);
    ImPlotFlags                 flags               = ImPlotFlags_None | ImPlotFlags_NoTitle;
    
};


//  "AxisCFG"
//
struct AxisCFG {
    const char *                uuid                = nullptr;
    ImPlotAxisFlags             flags               = ImPlotAxisFlags_None | ImPlotAxisFlags_NoSideSwitch | ImPlotAxisFlags_NoHighlight;
};


//  "LegendCFG"
//
struct LegendCFG {
    ImPlotLocation              location            = ImPlotLocation_NorthWest;
    ImPlotLegendFlags           flags               = ImPlotLegendFlags_None;
};


//  "PlotCFG"
//
struct PlotCFG {
//
//          1.              MAIN PLOT PARAMETERS...
    GraphCFG                    graph               = { nullptr, ImVec2(-1.0f, -1.0f), ImPlotFlags_None | ImPlotFlags_NoTitle };
//
//          2.              AXIS PARAMETERS...
    AxisCFG                     axes [2]            = {
        {"X-axis    [Arb.]",    ImPlotAxisFlags_None | ImPlotAxisFlags_NoSideSwitch | ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_AutoFit},
        {"Y-axis    [Arb.]",    ImPlotAxisFlags_None | ImPlotAxisFlags_NoSideSwitch | ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_AutoFit}
    };
//
//          3.              LEGEND PARAMETERS...
    LegendCFG                   legend              = { ImPlotLocation_NorthWest, ImPlotLegendFlags_None};
};






// *************************************************************************** //
//
//
//
//      3.      MISC / SPECIFIC USE-CASES...
// *************************************************************************** //
// *************************************************************************** //



//          3B.     COINCIDENCE COUNTER:
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" :: "fdtd" NAMESPACE.












#endif      //  _CBAPP_TYPES_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.

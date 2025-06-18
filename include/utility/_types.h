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
*              AUTHOR:      Collin A. Bond
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
enum class WindowLocation {
    Center,             //  default.
    LeftHalf,           //  left half of the monitor.
    RightHalf,          //  right half of the monitor.
    Fill,               //  Expand window to fill the entire screen.
    COUNT
};


//  "Anchor"
//
enum class Anchor {
    Center,             //  default.
    East,
    NorthEast,
    North,
    NorthWest,
    West,
    SouthWest,
    South,
    SouthEast,
//
    COUNT
};


//  "AnchorAngles"
//
inline constexpr std::array<float, 9>   AnchorAngles = {
    0.0f,                // Center (same as North)
    IM_PI * 0.5f,        // East
    IM_PI * 0.25f,       // NorthEast
    0.0f,                // North
    -IM_PI * 0.25f,      // NorthWest
    -IM_PI * 0.5f,       // West
    -IM_PI * 0.75f,      // SouthWest
    IM_PI,               // South
    IM_PI * 0.75f        // SouthEast
};



// *************************************************************************** //
//
//
//  2.      PLOTS AND GRAPHS...
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
//  3.      MISC / SPECIFIC USE-CASES...
// *************************************************************************** //
// *************************************************************************** //



//          3B.     COINCIDENCE COUNTER:
// *************************************************************************** //
// *************************************************************************** //

//  "ChannelIdx"
//
enum ChannelIdx : std::size_t {
    UNUSED = 0,
    D,      C,      CD,
    B,      BD,     BC,     BCD,
    A,      AD,     AC,     ACD,
    AB,     ABD,    ABC,    ABCD,
    CHANNEL_COUNT   // = 16
};


//  "CoincidencePacket"
//      Holds a single FPGA “packet”
struct CoincidencePacket
{
    std::array<int, CHANNEL_COUNT> counts{};
    int cycles{};

    //  Convenience accessors
    int d           (void) const    { return counts[D];  }
    int c           (void) const    { return counts[C];  }
    int cd          (void) const    { return counts[CD]; }
    int b           (void) const    { return counts[B];  }
    int bd          (void) const    { return counts[BD]; }
    int bc          (void) const    { return counts[BC]; }
    int bcd         (void) const    { return counts[BCD];}
    int a           (void) const    { return counts[A];  }
    int ad          (void) const    { return counts[AD]; }
    int ac          (void) const    { return counts[AC]; }
    int acd         (void) const    { return counts[ACD];}
    int ab          (void) const    { return counts[AB]; }
    int abd         (void) const    { return counts[ABD];}
    int abc         (void) const    { return counts[ABC];}
    int abcd        (void) const    { return counts[ABCD];}
};


//  "parse_packet"
//      Parse one JSON‑line; returns nullopt on format errors
inline std::optional<CoincidencePacket>
parse_packet(std::string_view line)
{
    using json = nlohmann::json;
    CoincidencePacket pkt;

    try {
        json j = json::parse(line);
        const auto& arr = j.at("counts");
        if (arr.size() != CHANNEL_COUNT)         // sanity check
            return std::nullopt;

        for (std::size_t i = 0; i < CHANNEL_COUNT; ++i)
            pkt.counts[i] = arr[i].get<int>();

        pkt.cycles = j.at("cycles").get<int>();
        return pkt;

    } catch (const json::exception&) {
        return std::nullopt;                     // malformed JSON / keys
    }
}

inline std::optional<CoincidencePacket>
parse_packet(std::string_view line,
             bool mutual_exclusion)   // NEW ARG (default = previous behaviour)
{
    using json = nlohmann::json;
    CoincidencePacket pkt;

    constexpr std::size_t A_IDX = ChannelIdx::A;
    constexpr std::size_t B_IDX = ChannelIdx::B;
    constexpr std::size_t C_IDX = ChannelIdx::C;
    constexpr std::size_t D_IDX = ChannelIdx::D;

    try {
        json j   = json::parse(line);
        const auto & arr = j.at("counts");
        if (arr.size() != CHANNEL_COUNT)
            return std::nullopt;

        for (std::size_t i = 0; i < CHANNEL_COUNT; ++i)
            pkt.counts[i] = arr[i].get<int>();

        pkt.cycles = j.at("cycles").get<int>();

        // ------------------------------------------------------------------
        //  Optional reconciliation of mutually exclusive counts
        // ------------------------------------------------------------------
        if (!mutual_exclusion) {
            // The index value encodes which APD channels participated:
            // bit3=A, bit2=B, bit1=C, bit0=D  (e.g. 0b1100 == AB)
            for (std::size_t idx = 0; idx < CHANNEL_COUNT; ++idx) {
                const int n = pkt.counts[idx];
                if (n == 0) continue;
                const std::uint8_t mask = static_cast<std::uint8_t>(idx);

                // skip single channels or UNUSED (they already hold the count)
                if (mask == 0 || mask == 1 || mask == 2 || mask == 4 || mask == 8)
                    continue;

                if (mask & 0x8) pkt.counts[A_IDX] += n;   // A
                if (mask & 0x4) pkt.counts[B_IDX] += n;   // B
                if (mask & 0x2) pkt.counts[C_IDX] += n;   // C
                if (mask & 0x1) pkt.counts[D_IDX] += n;   // D
            }
        }

        return pkt;

    } catch (const json::exception&) {
        return std::nullopt;
    }
}



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

/***********************************************************************************
*
*       ********************************************************************
*       ****                 T E M P . H  ____  F I L  E                ****
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
#ifndef _CBAPP_GRAPH_APP_TEMP_H
#define _CBAPP_GRAPH_APP_TEMP_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
//#include "app/app.h"
#include "app/_init.h"
#include "utility/utility.h"

//  0.2     STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <cstdlib>          // C-Headers...
#include <stdio.h>
#include <unistd.h>
#include <random>
#include <tuple>
#include <utility>
#include <algorithm>

#include <string>           //  <======| std::string, ...
#include <string_view>
#include <vector>           //  <======| std::vector, ...
#include <stdexcept>        //  <======| ...
#include <limits.h>
#include <math.h>

//  0.3     "DEAR IMGUI" HEADERS...
#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_internal.h"
#include "implot.h"
#include "implot_internal.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //


//──────────── DATA LAYER ────────────────────────────────────────────────────
struct Pos          { uint32_t id; float x, y; };
struct PointStyle   { ImU32 color = IM_COL32(0,255,0,255); float radius = 4.f; bool visible = true; };
struct Point        { uint32_t v; PointStyle sty{}; };
struct Line         { uint32_t a, b; ImU32 color = IM_COL32(255,255,0,255); float thickness = 2.f; };

//──────────── WIDGET ────────────────────────────────────────────────────────
class Editor {
    enum class Mode : int { Default = 0, Line, Point, Count };
    static constexpr std::array<const char*, static_cast<size_t>(Mode::Count)> MODE_LABELS{ "Default","Line","Point" };

    struct Interaction {
        bool hovered{}, active{}, space{};
        ImVec2 canvas{}, origin{}, tl{};
        ImDrawList* dl{};
    };

public:
    void Begin(const char* id = "##EditorCanvas");

private:
    // mode handlers
    void _handle_default (const Interaction&);
    void _handle_line    (const Interaction&);
    void _handle_point   (const Interaction&);

    // helpers
    uint32_t _add_vertex(ImVec2 w);
    void     _add_point (ImVec2 w);
    int      _hit_point (const Interaction&) const;
    void     _draw_grid (ImDrawList*, const ImVec2&, const ImVec2&) const;
    void     _draw_lines(ImDrawList*, const ImVec2&) const;
    void     _draw_points(ImDrawList*,const ImVec2&) const;
    void     _draw_controls(const ImVec2&);
    void     _clear_all();

    // data
    std::vector<Pos>   m_vertices;
    std::vector<Point> m_points;
    std::vector<Line>  m_lines;

    Mode    m_mode      = Mode::Default;
    bool    m_show_grid = true;
    bool    m_drawing   = false;
    int     m_drag_point= -1;

    float   m_zoom      = 1.f;
    ImVec2  m_scroll    {0.f,0.f};
    uint32_t m_next_id  = 1;
};






// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.






#endif      //  _CBAPP_GRAPH_APP_TEMP_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.


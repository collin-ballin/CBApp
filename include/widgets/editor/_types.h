/***********************************************************************************
*
*       ********************************************************************
*       ****               _ T Y P E S . H  ____  F I L E               ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      June 13, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBWIDGETS_EDITOR_TYPES_H
#define _CBWIDGETS_EDITOR_TYPES_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
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
#include <array>
#include <unordered_set>
#include <optional>

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





//  1.  STATIC / CONSTEXPR / CONSTANTS...
// *************************************************************************** //
// *************************************************************************** //

// Inserted constants after GRID_STEP and HIT_THRESH_SQ
static constexpr float      GRID_STEP                   = 64.0f;
static constexpr float      HIT_THRESH_SQ               = 6.0f * 6.0f;


// ---- standardized style -----------------------------------------------
static constexpr ImU32      COL_POINT_DEFAULT           = IM_COL32(0,255,0,255);   // idle green
static constexpr ImU32      COL_POINT_HELD              = IM_COL32(255,100,0,255); // while dragging
static constexpr ImU32      COL_SELECTION_OUT           = IM_COL32(255,215,0,255); // gold outline
static constexpr float      DEFAULT_POINT_RADIUS        = 12.0f;                  // px

static constexpr ImU32      COL_LASSO_OUT               = IM_COL32(255,215,0,255); // gold outline
static constexpr ImU32      COL_LASSO_FILL              = IM_COL32(255,215,0,40);  // translucent fill


//  APPEARANCE FOR SELECTION STUFF...
static constexpr float      HANDLE_BOX_SIZE             = 4.f;






// *************************************************************************** //
//
//
//
//  2.  [DATA LAYER]    STRUCTS AND HELPER TYPE ABSTRACTIONS FOR THE "EDITOR" WIDGET...
// *************************************************************************** //
// *************************************************************************** //

//───────────────── VERTEX (anchor‑point) ────────────────────────────────
// A pure geometry node. `in_handle` and `out_handle` are offsets from
// `pos`; when they are both zero the segment attached to this vertex is
// straight. Handles live in world‑space units.
struct Vertex {
    uint32_t    id              = 0;
    float       x               = 0.0f,
                y               = 0.0f;
    //
    ImVec2      in_handle       = ImVec2(0.0f, 0.0f);   // incoming Bézier handle (from previous vertex)
    ImVec2      out_handle      = ImVec2(0.0f, 0.0f);   // outgoing Bézier handle (to next vertex)
};




struct PointStyle   { ImU32 color = IM_COL32(0,255,0,255); float radius = 4.f; bool visible = true; };
struct Point        { uint32_t v; PointStyle sty{}; };
struct Line         { uint32_t a, b; ImU32 color = IM_COL32(255,255,0,255); float thickness = 2.f; };


//───────────────── PATH (polyline / spline / area) ────────────────────────
// A sequence of vertex IDs. `closed == true` means the last vertex connects
// back to the first. Vertices carry their own Bézier handles; this struct
// only stores ordering and style.
struct PathStyle {
    ImU32 stroke_color = IM_COL32(255,255,0,255);
    ImU32 fill_color   = IM_COL32(255,255,255,0);   // default: transparent white
    float stroke_width = 2.0f;
};


//  "Path"
//
struct Path {
    inline bool is_area(void) const noexcept
    { return this->closed && this->verts.size() >= 3; }
//
    std::vector<uint32_t>   verts;   // ordered anchor IDs
    bool                    closed      = false;
    PathStyle               style       = PathStyle();
};


//  "Hit"
//
struct Hit {
  enum class    Type { Point, Line, Path };
  Type          type{};
  size_t        index{};
};


//  "PathHit"
//
struct PathHit {
    size_t  path_idx   = 0;   // which Path in m_paths
    size_t  seg_idx    = 0;   // segment i   (verts[i] → verts[i+1])
    float   t          = 0.f; // param along that segment
    ImVec2  pos_ws{};         // exact split position (world-space)
};




// *************************************************************************** //
//
//
//      WIDGET STUFF...
// *************************************************************************** //
// *************************************************************************** //

//  "Mode"
//      - Enum type for each "tool" that we have in the application.
//
enum class Mode : int {
    Default = 0,        //  "Select"                (KEY: "v").
    Line,
    Point,              //  "Point"                 (KEY: "n").
    Pen,                //  "Pen"                   (KEY: "p").
    Scissor,            //  "Scissors"              (KEY: "c").
    AddAnchor,          //  "Add Anchor"            (KEY: "+").
    RemoveAnchor,       //  "Remove Anchor"         (KEY: "-").
//
    Count
};


//  "DEF_EDITOR_STATE_NAMES"
//
constexpr std::array<const char*, static_cast<size_t>(Mode::Count)>
    DEF_EDITOR_STATE_NAMES  = { {
    "Default",              "Line",                     "Point",                    "Pen",
    "Scissor",              "Add Anchor Point",         "Remove Anchor Point"
} };


//  "GridSettings"
//
struct GridSettings {
    float   world_step   = 20.0f;   // grid pitch in world units
    bool    visible      = true;
    bool    snap_on      = false;
};



//  "Interaction"
//
struct Interaction {
    bool hovered{}, active{}, space{};
    ImVec2 canvas{}, origin{}, tl{};
    ImDrawList* dl{};
};


//  "Selection"
//
struct Selection {
    std::unordered_set<uint32_t>    vertices;
    std::unordered_set<size_t>      points;
    std::unordered_set<size_t>      lines;
    std::unordered_set<size_t>      paths;     // ← NEW
//
    void clear(void) {
        vertices.clear();
        points.clear();
        lines.clear();
        paths.clear();        // new
    }
    bool empty() const { return vertices.empty() && points.empty() && lines.empty(); }
};
    

// Global capability flags (extensible)
enum Capability : uint8_t { Cap_Select = 1 << 0 };






// *************************************************************************** //
//
//
//      ARRAYS...
// *************************************************************************** //
// *************************************************************************** //

//  "PenState"
//
struct PenState {
    bool        active          = false;
    size_t      path_index      = static_cast<size_t>(-1);
    uint32_t    last_vid        = 0;
    bool        dragging_handle = false;
    uint32_t    handle_vid      = 0;
    bool        prepend         = false;
};


// editor.h  (private)
struct EndpointInfo { size_t path_idx; bool prepend; };   // prepend==true ↔ first vertex



//  "BoxDrag" struct (add fields for handle_ws0, orig_w, orig_h after mouse_ws0)
struct BoxDrag
{
    bool      active      = false;
    uint8_t   handle_idx  = 0;
    ImVec2    anchor_ws   = {0,0};
    ImVec2    bbox_tl_ws  = {0,0};
    ImVec2    bbox_br_ws  = {0,0};
    std::vector<uint32_t> v_ids;
    std::vector<ImVec2>   v_orig;
    ImVec2    mouse_ws0   = {0,0};
    ImVec2    handle_ws0;            // initial world‑space position of the dragged handle
    float     orig_w = 1.f;          // original bbox width  (world units)
    float     orig_h = 1.f;          // original bbox height (world units)
    bool      first_frame = true;
};


//  "MoveDrag"
//
struct MoveDrag {
    bool                  active   = false;
    ImVec2                anchor_ws;
    std::vector<uint32_t> v_ids;
    std::vector<ImVec2>   v_orig;
};


// *************************************************************************** //
//
//
//      ARRAYS...
// *************************************************************************** //
// *************************************************************************** //

// Per‑mode capability mask
static constexpr std::array<uint8_t, static_cast<size_t>(Mode::Count)>
    MODE_CAPS{ Cap_Select,          // Default
               0,                   // Line    (selection disabled)
               Cap_Select,          // Point
               0                    // Pen     (selection disabled)
    };






// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.






#endif      //  _CBWIDGETS_EDITOR_TYPES_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.


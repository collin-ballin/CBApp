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


//      //  From the 2nd frame onward, test drag distance
//      //  if ( m_pen.pending_time >= PEN_DRAG_TIME_THRESHOLD && ImGui::IsMouseDragPastThreshold(ImGuiMouseButton_Left, PEN_DRAG_MOVEMENT_THRESHOLD) )
//      if ( m_pen.pending_time >= PEN_DRAG_TIME_THRESHOLD && ImGui::IsMouseDragPastThreshold(ImGuiMouseButton_Left, PEN_DRAG_MOVEMENT_THRESHOLD) )
//      {
//          _pen_begin_handle_drag( m_pen.pending_vid, /*out_handle=*/true, /*force_select=*/true );
//          if ( Vertex * v = find_vertex_mut(m_vertices, m_pen.pending_vid) )
//              v->in_handle = ImVec2(0,0);              // make handle visible
//
//
//          m_pen.pending_handle = false;                // hand-off to drag logic
//          m_pen.pending_time   = 0.0f;
//          _pen_update_handle_drag(it);                 // first update
//          return;
//      }
        


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
//  1.  [TYPE/ENUM LAYER]   TYPES AND ENUMERATIONS THAT WE USE FOR THE EDITOR...
// *************************************************************************** //
// *************************************************************************** //

//  "AnchorType"
//
enum class AnchorType : uint8_t { Corner, Smooth, Symmetric };
static constexpr const char *   ANCHOR_TYPE_NAMES[] = { "Corner", "Smooth", "Symmetric" };
//
static constexpr ImU32          ANCHOR_COLORS[]     = {
    IM_COL32(255,255,0,255),   // corner  – yellow
    IM_COL32(  0,255,0,255),   // smooth  – green
    IM_COL32(  0,200,255,255)  // symmetric – cyan
};



//  "Mode"
//      - Enum type for each "tool" that we have in the application.
//
enum class Mode : int {
    Default = 0,        //  "Select"                (KEY: "V").
    Line,
    Point,              //  "Point"                 (KEY: "N").
    Pen,                //  "Pen"                   (KEY: "P").
    Scissor,            //  "Scissors"              (KEY: "C").
    AddAnchor,          //  "Add Anchor"            (KEY: "+").
    RemoveAnchor,       //  "Remove Anchor"         (KEY: "-").
    EditAnchor,         //  "Edit Anchor"           (KEY: "SHIFT" + "C").
//
    Count
};
//
//  "DEF_EDITOR_STATE_NAMES"
constexpr std::array<const char*, static_cast<size_t>(Mode::Count)>
    DEF_EDITOR_STATE_NAMES  = { {
    "Default",              "Line",                     "Point",                    "Pen",
    "Scissor",              "Add Anchor Point",         "Remove Anchor Point",      "Edit Anchor"
} };



//  "Capability"
//      Global capability flags (extensible)
//
enum Capability : uint8_t
{
    Cap_Select      = 1 << 0,   // enables click/drag selection logic
    Cap_CursorHint  = 1 << 1    // enables hand / resize cursor hints
};
//
// Per‑mode capability mask
static constexpr std::array<uint8_t, static_cast<size_t>(Mode::Count)>
    MODE_CAPS               = {
        Cap_Select | Cap_CursorHint,        //      Default
        Cap_CursorHint,                     //      Line           (no selection)
        Cap_Select | Cap_CursorHint,        //      Point
        Cap_CursorHint,                     //      Pen            (selection off)
        0,                                  //      Scissor
        0,                                  //      AddAnchor      (no hint, no select)
        0,                                  //      RemoveAnchor
        Cap_CursorHint                      //      EditAnchor     (hint yes, selection no)
    };








// *************************************************************************** //
//
//
//
//  2.  [DATA LAYER]    STRUCTS AND HELPER TYPE ABSTRACTIONS FOR THE "EDITOR" WIDGET...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//      2.1.    VERTEX.
// *************************************************************************** //

//  "Vertex" / "Pos" / "Anchor Point"
//
//      A pure geometry node. `in_handle` and `out_handle` are offsets from
//      `pos`; when they are both zero the segment attached to this vertex is
//      straight. Handles live in world‑space units.
//
struct Vertex {
    uint32_t    id              = 0;
    float       x               = 0.0f,
                y               = 0.0f;
    //
    ImVec2      in_handle       = ImVec2(0.0f, 0.0f);   // incoming Bézier handle (from previous vertex)
    ImVec2      out_handle      = ImVec2(0.0f, 0.0f);   // outgoing Bézier handle (to next vertex)
    AnchorType  kind            = AnchorType::Corner;
};

struct PointStyle   { ImU32 color = IM_COL32(0,255,0,255); float radius = 4.0f; bool visible = true; };
struct Point        { uint32_t v; PointStyle sty{}; };
struct Line         { uint32_t a, b; ImU32 color = IM_COL32(255,255,0,255); float thickness = 2.f; };






// *************************************************************************** //
//      2.2.    PATH.
// *************************************************************************** //

//  "Path"  -- (polyline / spline / area).
//
//      A sequence of vertex IDs. `closed == true` means the last vertex connects
//      back to the first. Vertices carry their own Bézier handles; this struct
//      only stores ordering and style.
//
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


//  "EndpointInfo"
//
struct EndpointInfo { size_t path_idx; bool prepend; };   // prepend==true ↔ first vertex
  





// *************************************************************************** //
//      2.3.    HIT.
// *************************************************************************** //

//  "Hit"
//
struct Hit {
    enum class Type { Point, Line, Path, Handle };
    Type     type     = Type::Point;
    size_t   index    = 0;     // Point/Line/Path: original meaning
    bool     out      = false; // valid only when type == Handle
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
//      2.4.    INTERACTION INFORMATION/STATES.
// *************************************************************************** //

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
    bool            hovered{},      active{},       space{};
    ImVec2          canvas{},       origin{},       tl{};
    ImDrawList *    dl{};
};

//  "MoveDrag"
//
struct MoveDrag {
    bool                  active   = false;
    ImVec2                anchor_ws;
    std::vector<uint32_t> v_ids;
    std::vector<ImVec2>   v_orig;
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
    
    
    
// *************************************************************************** //
//      2.5.    APPLICATION TOOL STATE.
// *************************************************************************** //

//  "PenState"
//
struct PenState {
    bool        active          = false;
    size_t      path_index      = static_cast<size_t>(-1);
    uint32_t    last_vid        = 0;

    bool        dragging_handle = false;
    bool        dragging_out    = true;     // NEW: true → out_handle, false → in_handle
    uint32_t    handle_vid      = 0;

    bool        prepend         = false;
    bool        pending_handle  = false;        // waiting to see if user drags
    uint32_t    pending_vid     = 0;            // vertex that may get a handle
    float       pending_time    = 0.0f;
    //int         pending_frames  = 0;        // NEW – counts frames since click
};


//  "BoxDrag" struct (add fields for handle_ws0, orig_w, orig_h after mouse_ws0)
struct BoxDrag {
    bool                    active          = false;
    uint8_t                 handle_idx      = 0;
    ImVec2                  anchor_ws       = {0,0};
    ImVec2                  bbox_tl_ws      = {0,0};
    ImVec2                  bbox_br_ws      = {0,0};
    std::vector<uint32_t>   v_ids;
    std::vector<ImVec2>     v_orig;
    ImVec2                  mouse_ws0       = {0,0};
    ImVec2                  handle_ws0;                     // initial world‑space position of the dragged handle
    float                   orig_w          = 1.f;          // original bbox width  (world units)
    float                   orig_h          = 1.f;          // original bbox height (world units)
    bool                    first_frame     = true;
};


//  "Bounds"
//
struct Bounds {
    float min_x{0}, min_y{0}, max_x{0}, max_y{0};
};
  
  
//  "OverlayState"
//
struct OverlayState
{
    bool                open                = true;         // Editor toggles this; no “x” button
    bool                show_details        = true;         // extra diagnostics pane
    bool                verbose_detail      = false;        // extra diagnostics pane
    int                 location            = 3;            // 0-3 corners, −2 centre, −1 free
    float               alpha               = 0.65f;        // window translucency
    float               pad                 = 40.0f;        // margin from viewport edge
    ImGuiWindowFlags    base_flags          = ImGuiWindowFlags_NoDecoration       | ImGuiWindowFlags_NoDocking          | ImGuiWindowFlags_AlwaysAutoResize |
                                              ImGuiWindowFlags_NoSavedSettings    | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    ImGuiWindowFlags    flags               = 0;            // recomputed each frame
//
    std::string         log_msg;                            // last message (≤40 words)
    float               log_timer           = 0.0f;         // seconds until it disappears
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


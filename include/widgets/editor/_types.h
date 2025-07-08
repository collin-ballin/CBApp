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
// Updated declarations for selection overlay functions (no origin argument)
// void _draw_selection_highlight(ImDrawList* dl) const;
// void _draw_selection_bbox(ImDrawList* dl) const;
// void _draw_selected_handles(ImDrawList* dl) const;

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
#include "widgets/editor/_constants.h"

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
#include "json.hpp"
//
#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_internal.h"
#include "implot.h"
#include "implot_internal.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //






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
    Shape,              //  "Scissors"              (KEY: "C").
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
    "Scissor",              "Shape",                    "Add Anchor Point",         "Remove Anchor Point",
    "Edit Anchor"
} };



//  "Capability"
//      Global capability flags (extensible)
//      Camera / editor feature toggles, ImGui-style
//
enum CBCapabilityFlags_ : uint8_t {
    CBCapabilityFlags_None              = 0,            // no special behaviour
//
    CBCapabilityFlags_Pan               = 1 << 0,       // Space + drag navigation
    CBCapabilityFlags_Zoom              = 1 << 1,       // mouse-wheel magnification
    CBCapabilityFlags_Select            = 1 << 2,       // click / lasso / move-drag
    CBCapabilityFlags_CursorHint        = 1 << 3,       // hand or resize cursor
//
    CBCapabilityFlags_Navigation        = CBCapabilityFlags_Pan | CBCapabilityFlags_Zoom,
//
    CBCapabilityFlags_Count                             // helper: number of bits
};
//
//
//
using   CBCapabilityFlags     = CBCapabilityFlags_;
//
//  Per‑mode capability mask
static constexpr std::array<uint8_t, static_cast<size_t>(Mode::Count)>
MODE_CAPS               = {
/*  Default         */      CBCapabilityFlags_Navigation | CBCapabilityFlags_Select | CBCapabilityFlags_CursorHint,
/*  Line            */      CBCapabilityFlags_Navigation | CBCapabilityFlags_CursorHint,
/*  Point           */      CBCapabilityFlags_Navigation | CBCapabilityFlags_Select | CBCapabilityFlags_CursorHint,
/*  Pen             */      CBCapabilityFlags_Navigation | CBCapabilityFlags_CursorHint,
/*  Scissor         */      CBCapabilityFlags_Navigation | CBCapabilityFlags_None,
/*  Shape           */      CBCapabilityFlags_Navigation | CBCapabilityFlags_None,
/*  AddAnchor       */      CBCapabilityFlags_None,
/*  RemoveAnchor    */      CBCapabilityFlags_None,
/*  EditAnchor      */      CBCapabilityFlags_Navigation
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

//  "Vertex_t" / "Pos" / "Anchor Point"
//
//      A pure geometry node. `in_handle` and `out_handle` are offsets from
//      `pos`; when they are both zero the segment attached to this vertex is
//      straight. Handles live in world‑space units.
//
template<typename VID>
struct Vertex_t {
    static constexpr const char *   ms_DEF_VERTEX_LABEL_FMT_STRING      = "Vertex V%03d (id=%u)";
    static constexpr const char *   ms_DEF_VERTEX_FMT_STRING            = "V%03u";
//
    uint32_t    id              = 0;
    float       x               = 0.0f,
                y               = 0.0f;
    //
    ImVec2      in_handle       = ImVec2(0.0f, 0.0f);   // incoming Bézier handle (from previous vertex)
    ImVec2      out_handle      = ImVec2(0.0f, 0.0f);   // outgoing Bézier handle (to next vertex)
    AnchorType  kind            = AnchorType::Corner;
};
//
//  "to_json"
template <typename IdT>
inline void to_json(nlohmann::json& j, const Vertex_t<IdT>& v)
{
    j = {
        { "id",          v.id },
        { "x",           v.x  },
        { "y",           v.y  },
        { "in_handle",   { v.in_handle.x,  v.in_handle.y } },
        { "out_handle",  { v.out_handle.x, v.out_handle.y } },
        { "kind",        v.kind }
    };
}
//
//  "from_json"
template <typename IdT>
inline void from_json(const nlohmann::json& j, Vertex_t<IdT>& v)
{
    j.at("id")  .get_to(v.id);
    j.at("x")   .get_to(v.x);
    j.at("y")   .get_to(v.y);
    auto ih = j.at("in_handle");  v.in_handle  = { ih[0], ih[1] };
    auto oh = j.at("out_handle"); v.out_handle = { oh[0], oh[1] };
    j.at("kind").get_to(v.kind);
}

// *************************************************************************** //
// *************************************************************************** //






// *************************************************************************** //
//
//
//      POINT:
// *************************************************************************** //
// *************************************************************************** //

//  "PointStyle"
//
struct PointStyle   {
    ImU32       color       = IM_COL32(0,255,0,255);
    float       radius      = 4.0f;
    bool        visible     = true;
};
//
//  "to_json"
inline void to_json(nlohmann::json& j, const PointStyle& s)
{
    j = nlohmann::json{
        { "color",  s.color   },
        { "radius", s.radius  },
        { "visible",s.visible }
    };
}
//
//  "from_json"
inline void from_json(const nlohmann::json& j, PointStyle& s)
{
    j.at("color" ).get_to(s.color );
    j.at("radius").get_to(s.radius);
    j.at("visible").get_to(s.visible);
}




//  "Point_t"
//
template <typename PtID>
struct Point_t        {
    PtID            v;
    PointStyle      sty{};
};
//
//  "to_json"
template <typename PtID>
inline void to_json(nlohmann::json& j, const Point_t<PtID>& p)
{
    j = { { "v", p.v },
          { "sty", p.sty } };
}
//
//  "from_json"
template <typename PtID>
inline void from_json(const nlohmann::json& j, Point_t<PtID>& p)
{
    j.at("v"  ).get_to(p.v  );
    j.at("sty").get_to(p.sty);
}




//  "Line_t"
//
template <typename LID, typename ZID>
struct Line_t         {
    [[nodiscard]] inline bool is_mutable(void) const noexcept
    { return visible && !locked; }
//
//
    LID         a, b;
    ImU32       color       = IM_COL32(255,255,0,255);
    float       thickness   = 2.0f;
//
    ZID         z_index     = Z_FLOOR_USER;
    bool        locked      = false;
    bool        visible     = true;
};
//
//  "to_json"
template <typename LID, typename ZID>
inline void to_json(nlohmann::json  j, const Line_t<LID, ZID> & l)
{
    j = { { "a",            l.a             },
          { "b",            l.b             },
          { "color",        l.color         },
          { "thickness",    l.thickness     },
          { "z_index",      l.z_index       },
          { "locked",       l.locked        },
          { "visible",      l.visible       }
    };
}
//
//  "from_json"
template <typename LID, typename ZID>
inline void from_json(const nlohmann::json & j, Line_t<LID, ZID>  & l)
{
    j.at("a"                ).get_to(l.a);
    j.at("b"                ).get_to(l.b);
    j.at("color"            ).get_to(l.color);
    j.at("z_index"          ).get_to(l.z_index);
    j.at("locked"           ).get_to(l.locked);
    j.at("visible"          ).get_to(l.visible);
}

// *************************************************************************** //
// *************************************************************************** //






// *************************************************************************** //
//      2.2.    PATH.
// *************************************************************************** //








// *************************************************************************** //
//
//
//      "Path":     -- (polyline / spline / area).
// *************************************************************************** //
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
//
//  "to_json"
inline void to_json(nlohmann::json& j, const PathStyle& s)
{
    j = nlohmann::json{
        { "stroke_color", s.stroke_color },
        { "fill_color",   s.fill_color   },
        { "stroke_width", s.stroke_width }
    };
}
//
//  "from_json"
inline void from_json(const nlohmann::json& j, PathStyle& s)
{
    j.at("stroke_color").get_to(s.stroke_color);
    j.at("fill_color"  ).get_to(s.fill_color  );
    j.at("stroke_width").get_to(s.stroke_width);
}



//  "Path_t"
//
template<typename PID, typename VID, typename ZID>
struct Path_t {
    static constexpr size_t         ms_MAX_PATH_LABEL_LENGTH    = 64;
//
//
    [[nodiscard]] inline bool       is_area             (void) const noexcept
    { return this->closed && this->verts.size() >= 3; }
//
    [[nodiscard]] inline bool       is_mutable          (void) const noexcept
    { return visible && !locked; }
//
    inline void                     set_label           (const char * src) noexcept
    { this->label = std::string(src); this->_truncate(); }
//
    inline void                     set_default_label   (const PID id_) noexcept
    { this->id = id_;   this->label = std::format("Path {:03}", id_);   this->_truncate(); }
//
    inline void                     _truncate           (void)
    { if (this->label.size() > ms_MAX_PATH_LABEL_LENGTH) { this->label.resize( ms_MAX_PATH_LABEL_LENGTH ); } }
//
//
//
    std::vector<VID>    verts;   // ordered anchor IDs
    PID                 id                                      = 0;
    bool                closed                                  = false;
    PathStyle           style                                   = PathStyle();
//
// ─────────── NEW ───────────
    ZID                 z_index                                 = Z_FLOOR_USER;
    bool                locked                                  = false;
    bool                visible                                 = true;
    std::string         label                                   = "";
};
//
//  "to_json"
template<typename PID, typename VID, typename ZID>
inline void to_json(nlohmann::json & j, const Path_t<PID, VID, ZID> & p)
{
    j = { { "verts",        p.verts         },
          { "id",           p.id            },
          { "closed",       p.closed        },
          { "style",        p.style         },
          { "z_index",      p.z_index       },
          { "locked",       p.locked        },
          { "visible",      p.visible       },
          { "label",        p.label         } };
    return;
}
//
//  "from_json"
template<typename PID, typename VID, typename ZID>
inline void from_json(const nlohmann::json & j, Path_t<PID, VID, ZID> & p)
{
    if ( j.contains("id") )         { j.at("id").get_to(p.id);      }
    else                            { p.id = 0;                     }
    
    j.at("verts"    ).get_to(p.verts    );
    j.at("closed"   ).get_to(p.closed   );
    j.at("style"    ).get_to(p.style    );
    j.at("z_index"  ).get_to(p.z_index  );
    j.at("locked"   ).get_to(p.locked   );
    j.at("visible"  ).get_to(p.visible  );
    
    //  Handle old files that may lack "label"
    if ( j.contains("label") )      { j.at("label").get_to(p.label); }
    else                            { p.set_label("?"); }
        
    return;
}



// *************************************************************************** //
// *************************************************************************** //




//  "EndpointInfo"
//
template<typename PID>
struct EndpointInfo_t { PID path_idx; bool prepend; };   // prepend==true ↔ first vertex
  





// *************************************************************************** //
//      2.3.    HIT.
// *************************************************************************** //

//  "Hit_t"
//
template <typename HID>
struct Hit_t {
    enum class Type { Point, Line, Path, Handle };
    Type            type            = Type::Point;
    size_t          index           = 0;     // Point/Line/Path: original meaning
    bool            out             = false; // valid only when type == Handle
};

//  "PathHit_t"
//
template <typename PID, typename VID>
struct PathHit_t {
    size_t          path_idx        = 0;    // which Path in m_paths
    size_t          seg_idx         = 0;    // segment i   (verts[i] → verts[i+1])
    float           t               = 0.f;  // param along that segment
    ImVec2          pos_ws          {};     // exact split position (world-space)
};



// *************************************************************************** //
//      2.4.    INTERACTION INFORMATION/STATES.
// *************************************************************************** //

//  "GridSettings"
//
struct GridSettings {
    float   snap_step       = 20.0f;   // quantize condition for grid-snapping.
    bool    visible         = true;
    bool    snap_on         = false;
};


//  "Interaction"
//
struct Interaction {
    bool            hovered{},      active{},       space{};
    ImVec2          canvas{},       origin{},       tl{};
    ImDrawList *    dl{};
};

// *************************************************************************** //
// *************************************************************************** //





// *************************************************************************** //
//
//
//      SELECTION:
// *************************************************************************** //
// *************************************************************************** //

//  "Selection_t"
//
template<typename VID, typename PtID, typename LID, typename PID>
struct Selection_t {
    std::unordered_set<uint32_t>    vertices;
    std::unordered_set<size_t>      points;
    std::unordered_set<size_t>      lines;
    std::unordered_set<size_t>      paths;     // ← NEW
//
    inline void clear(void) {
        vertices.clear();
        points.clear();
        lines.clear();
        paths.clear();        // new
    }
    inline bool empty() const { return vertices.empty() && points.empty() && lines.empty(); }
};
//
//  "to_json"
template<typename VID, typename PtID, typename LID, typename PID>
inline void to_json(nlohmann::json& j,
                    const Selection_t<VID,PtID,LID,PID>& s)
{
    j = { { "vertices",  std::vector<VID>  (s.vertices.begin(), s.vertices.end()) },
          { "points",    std::vector<PtID> (s.points  .begin(), s.points  .end()) },
          { "lines",     std::vector<LID>  (s.lines   .begin(), s.lines   .end()) },
          { "paths",     std::vector<PID>  (s.paths   .begin(), s.paths   .end()) } };
}
//
//  "from_json"
template<typename VID, typename PtID, typename LID, typename PID>
inline void from_json(const nlohmann::json& j,
                      Selection_t<VID,PtID,LID,PID>& s)
{
    std::vector<VID >  vs;  j.at("vertices").get_to(vs);
    std::vector<PtID>  ps;  j.at("points"  ).get_to(ps);
    std::vector<LID >  ls;  j.at("lines"   ).get_to(ls);
    std::vector<PID >  pa;  j.at("paths"   ).get_to(pa);

    s.vertices.clear();  s.vertices.insert(vs.begin(), vs.end());
    s.points  .clear();  s.points  .insert(ps.begin(), ps.end());
    s.lines   .clear();  s.lines   .insert(ls.begin(), ls.end());
    s.paths   .clear();  s.paths   .insert(pa.begin(), pa.end());
}

// *************************************************************************** //
// *************************************************************************** //
    
    
    
    
    
    
    
    
    
    
    
    
// *************************************************************************** //
//      2.5.    APPLICATION TOOL STATE.
// *************************************************************************** //

//  "PenState_t"
//
template<typename VID>
struct PenState_t {
    bool            active                  = false;
    size_t          path_index              = static_cast<size_t>(-1);
    VID             last_vid                = 0;

    bool            dragging_handle         = false;
    bool            dragging_out            = true;     // NEW: true → out_handle, false → in_handle
    VID             handle_vid              = 0;

    bool            prepend                 = false;
    bool            pending_handle          = false;        // waiting to see if user drags
    VID             pending_vid             = 0;            // vertex that may get a handle
    float           pending_time            = 0.0f;
    //int         pending_frames  = 0;        // NEW – counts frames since click
};



//  "ShapeKind"
//
enum class ShapeKind : uint32_t {
    Square,             Rectangle,
    Circle,             Oval,               Ellipse, /*, Polygon, Star, …*/
//
    Count
};
//
//  "APPLICATION_PLOT_COLOR_STYLE_NAMES"
//      COMPILE-TIME ARRAY CONTAINING THE NAME OF ALL STYLES.
inline static const std::array<const char *, static_cast<size_t>(ShapeKind::Count)>
EDITOR_SHAPE_NAMES = {{
    "Square",           "Rectangle",
    "Circle",           "Oval",             "Ellipse"
}};



//  "ShapeState_t"
//
template<typename OID>
struct ShapeState_t {
    bool                active                  = false;            // true while user is click-dragging a preview
    OID                 overlay_id              = OID(0);           // contextual UI (0 ⇒ none)
    ShapeKind           kind                    = ShapeKind::Square;
    float               radius                  = 25.0f;            // corner- or major-radius (placeholder)
    float               params[5]               = {0.0f};           // Array to hold multiple geometric descriptors for more complex shapes.
    //
    //
    //  Live-preview drag info (world-space)
    bool                dragging                = false;
    ImVec2              press_ws{};                  // anchor at mouse-down
    ImVec2              cur_ws{};                    // current mouse pos each frame
    float               start_y                 = 0.0f;
    float               start_rad               = 0.0f;
};






    
// *************************************************************************** //
//      2.6.    OTHER STATE STRUCTURES.
// *************************************************************************** //

//  "Clipboard_t"
//
template<typename Vertex, typename Point, typename Line, typename Path>
struct Clipboard_t {
    inline bool             empty() const   { return vertices.empty() && points.empty() && lines.empty() && paths.empty(); }
//
    ImVec2                  ref_ws      {};     // reference origin (top-left of bbox)
    std::vector<Vertex>     vertices;           // geometry snapshots (id-field unused)
    std::vector<Point>      points;
    std::vector<Line>       lines;
    std::vector<Path>       paths;              // verts[] hold vertex indices into vertices[]
};


//  "MoveDrag"
//
struct MoveDrag {
    bool                        active          = false;
    ImVec2                      anchor_ws       {0,0};   // top-left of selection at mouse-press
    ImVec2                      press_ws        {0,0};
    ImVec2                      cum_delta       {0,0};   // accumulated world-space translation
    std::vector<uint32_t>       v_ids;               // selected vertex IDs
    std::vector<ImVec2>         v_orig;              // original positions (same order)
};


//  "BoxDrag"
//      struct (add fields for handle_ws0, orig_w, orig_h after mouse_ws0)
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


//  "Camera"
//
struct Camera {
    ImVec2  pan             {0,0};      // world coords of viewport top-left
    float   zoom_mag        = 1.0f;     // 1 = fit, >1 zooms in, <1 zooms out

    // helpers --------------------------------------------------------
    inline float pixels_per_world(const ImVec2& view_sz, const Bounds& world) const {
        float w = world.max_x - world.min_x;
        float h = world.max_y - world.min_y;
        if (w <= 0) w = 1;              // avoid div-by-zero
        if (h <= 0) h = 1;
        float fit = std::min(view_sz.x / w, view_sz.y / h);
        return fit * zoom_mag;
    }
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
//  0.  OVERLAY UTILITY CLASS STUFF...
// *************************************************************************** //
// *************************************************************************** //

//  "BBoxAnchor"
//      Defiled in the order of unit circle angles (0deg = +x-axis) and DEFAULT = 0 = CENTER.
//
enum class BBoxAnchor : uint8_t {
    Center, East, NorthEast, North, NorthWest, West, SouthWest, South, SouthEast
};


enum class OverlayAnchor : uint8_t {
    World,      // anchor_ws interpreted in world-space → converts via world_to_pixels()
    Screen,     // anchor_ws is absolute screen coords (pixels)
    Cursor      // anchor_ws is Δ offset from current cursor (pixels)
};

enum class OffscreenPolicy : uint8_t {
    Hide,                // overlay vanishes when anchor is outside canvas
    Clamp                // overlay clamps to nearest edge (old behaviour)
};



enum class OverlayPlacement : uint8_t {
    ScreenXY,           // anchor_px    = screen position (px)
    Cursor,             // anchor_px    = offset  (px)
    World,              // anchor_ws    = world‑space point
    CanvasTL,           // anchor_padpx = inset from top‑left  corner
    CanvasTR,           // anchor_padpx = inset from top‑right corner
    CanvasBL,           // anchor_padpx = inset from bot‑left  corner
    CanvasBR,           // anchor_padpx = inset from bot‑right corner
//
    CanvasPoint
};


struct OverlayCFG {
    OverlayPlacement            placement       {OverlayPlacement::ScreenXY};
    ImVec2                      anchor_px       {0,0};     // pixel inset / offset
    ImVec2                      anchor_ws       {0,0};     // world-space anchor
    float                       alpha           {0.65f};
    OffscreenPolicy             offscreen       {OffscreenPolicy::Clamp}; // NEW
//
    std::function<void()>       draw_fn         {};                    // widgets callback
};


//  "Overlay_t"
//
template<typename T = uint32_t>
struct Overlay_t {
    using                       OverlayID       = T;
//
    OverlayID                   id              = 0;
    bool                        visible         = false;                 // owner sets false to retire
    ImGuiWindowFlags            flags           = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    OverlayCFG                  cfg{};
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

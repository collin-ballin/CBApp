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
#include "widgets/editor/_constants.h"
#include "widgets/editor/_objects.h"

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

//  "IoResult"
//
enum class IoResult {
    Ok, IoError, ParseError, VersionMismatch
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
    COUNT
};
//
//  "DEF_EDITOR_STATE_NAMES"
static constexpr std::array<const char *, static_cast<size_t>(Mode::COUNT)>
    DEF_EDITOR_STATE_NAMES  = { {
    "Default",              "Line",                     "Point",                    "Pen",
    "Scissor",              "Shape",                    "Add Anchor Point",         "Remove Anchor Point",
    "Edit Anchor"
} };



//  "Capability"
//      Global capability flags (extensible)
//      Camera / editor feature toggles, ImGui-style
//
enum CBCapabilityFlags_ : uint16_t {
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
//
//  Per‑mode capability mask
static constexpr std::array<uint16_t, static_cast<size_t>(Mode::COUNT)>
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
//      2.3.    HIT DETECTION.
// *************************************************************************** //
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
// *************************************************************************** //   END "HIT DETECTION"






// *************************************************************************** //
//
//
//      2.4.    INTERACTION INFORMATION/STATES.
// *************************************************************************** //
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
// *************************************************************************** //   END "INTERACTION STATES"






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
    inline void                     clear           (void)          { vertices.clear(); points.clear(); lines.clear(); paths.clear();               }        // new
    inline bool                     empty           (void) const    { return vertices.empty() && points.empty() && lines.empty() && paths.empty();  }
    inline bool                     is_empty        (void) const    { return paths.empty();                                                         }
//
//
//
    std::unordered_set<uint32_t>    vertices                {};
    std::unordered_set<size_t>      points                  {};
    std::unordered_set<size_t>      lines                   {};
    std::unordered_set<size_t>      paths                   {};     // ← NEW
//
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
// *************************************************************************** //   END "SELECTION".
    
    
    
    
    
    
    
    
    
    
    
    
// *************************************************************************** //
//
//
//
//      2.5.        TOOL STATE.
// *************************************************************************** //
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
    COUNT
};
//
//  "APPLICATION_PLOT_COLOR_STYLE_NAMES"
//      COMPILE-TIME ARRAY CONTAINING THE NAME OF ALL STYLES.
static constexpr std::array<const char *, static_cast<size_t>(ShapeKind::COUNT)>
DEF_EDITOR_SHAPE_NAMES = {{
    "Square",           "Rectangle",
    "Circle",           "Oval",             "Ellipse"
}};


//  "ShapeState_t"
//
template<typename OID>
struct ShapeState_t {
    bool                active                  = false;            // true while user is click-dragging a preview
    OID                 overlay_id              = OID(0);           // contextual UI (0 ⇒ none)
    ShapeKind           kind                    = ShapeKind::Rectangle;
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
//
//  "to_json"
template<typename OID>
inline void to_json(nlohmann::json & j, const ShapeState_t<OID> & obj)
{
    j = {
            { "kind",               obj.kind            },
            { "radius",             obj.radius          }
    };
    return;
}
//
//  "from_json"
template<typename OID>
inline void from_json(const nlohmann::json & j, ShapeState_t<OID> & o)
{
    j.at("kind"         ).get_to(o.kind         );
    j.at("radius"       ).get_to(o.radius       );
    return;
}




//  "BrowserState_t"
//
template<typename VID, typename PtID, typename LID, typename PID>
struct BrowserState_t {
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      1.      CONSTEXPR AND ALIASES...
    // *************************************************************************** //
    static constexpr size_t     ms_PATH_TITLE_BUFFER_SIZE               = 128ULL;
    static constexpr size_t     ms_VERTEX_TITLE_BUFFER_SIZE             = 64ULL;

    // *************************************************************************** //
    //      2.      MEMBER FUNCTIONS...
    // *************************************************************************** //
    
    //  "reset"
    inline void                 reset(void) {
        m_browser_anchor            = -1;
        m_inspector_vertex_idx      = -1;
        rename_idx                  = -1;
        renaming                    = false;
        return;
    }
    
    // *************************************************************************** //


    // *************************************************************************** //
    //      3.      DATA MEMBERS...
    // *************************************************************************** //
    int                         m_browser_anchor                        = -1;       //  anchor index for Shift‑range select
    int                         m_inspector_vertex_idx                  = -1;       //  anchor index for Shift‑range select
//
    int                         rename_idx                              = -1;       //  row currently in rename-mode (–1 = none)
    bool                        renaming                                = false;    //  true while an InputText for rename is active
    char                        rename_buf[ms_PATH_TITLE_BUFFER_SIZE]   = {};       //  scratch text
    
    // *************************************************************************** //


// *************************************************************************** //
// *************************************************************************** //   END "BrowserState_t".
};






//  "IndexState_t"
//
template<typename VID, typename PtID, typename LID, typename PID>
struct IndexState {
    VID                         next_vid                                = 0;    //  original:   "m_next_id"...
    PID                         next_pid                                = 0;    //  original:   "m_next_pid"...
};






// *************************************************************************** //
// *************************************************************************** //   END "TOOL STATE"







// *************************************************************************** //
//
//
//
//      2.6.    SERIALIZATION STUFF...
// *************************************************************************** //
// *************************************************************************** //

//  "Clipboard_t"
//
template<typename Vertex, typename Point, typename Line, typename Path>
struct Clipboard_t {
    inline void             clear()         { ref_ws = ImVec2(); vertices.clear(); points.clear(); lines.clear(); paths.clear(); return; }
    inline bool             empty() const   { return vertices.empty() && points.empty() && lines.empty() && paths.empty(); }
//
    ImVec2                  ref_ws      {};     // reference origin (top-left of bbox)
    std::vector<Vertex>     vertices;           // geometry snapshots (id-field unused)
    std::vector<Point>      points;
    std::vector<Line>       lines;
    std::vector<Path>       paths;              // verts[] hold vertex indices into vertices[]
};

// *************************************************************************** //
// *************************************************************************** //   END "SERIALIZATION"
  
  
  



// *************************************************************************** //
//
//
//
//      2.7.    OVERLAY STUFF...
// *************************************************************************** //
// *************************************************************************** //

//  "Resident"
//      Enums for each RESIDENT that will be default-assigned in the Editor class.
//
enum Resident: uint8_t {
    Debugger, Selection, Shape, COUNT
};


//  "BBoxAnchor"
//      Defiled in the order of unit circle angles (0deg = +x-axis) and DEFAULT = 0 = CENTER.
//
enum class BBoxAnchor : uint8_t {
    Center, East, NorthEast, North, NorthWest, West, SouthWest, South, SouthEast, COUNT
};


//  "OverlayAnchor"
//
enum class OverlayAnchor : uint8_t {
    World,      // anchor_ws interpreted in world-space → converts via world_to_pixels()
    Screen,     // anchor_ws is absolute screen coords (pixels)
    Cursor      // anchor_ws is Δ offset from current cursor (pixels)
};


//  "OffscreenPolicy"
//
enum class OffscreenPolicy : uint8_t {
    Hide,                // Overlay vanishes when anchor is outside canvas
    Clamp,              //  Overlay clamps to nearest edge (old behaviour)
    COUNT
};


//  "OverlayPlacement"
//
enum class OverlayPlacement : uint8_t {
    Custom,             // anchor_px    = screen position (px)
    Cursor,             // anchor_px    = offset  (px)
    World,              // anchor_ws    = world‑space point
//
    CanvasPoint,
    CanvasTL,           // anchor_padpx = inset from top‑left  corner
    CanvasTR,           // anchor_padpx = inset from top‑right corner
    CanvasBL,           // anchor_padpx = inset from bot‑left  corner
    CanvasBR,           // anchor_padpx = inset from bot‑right corner
//
    COUNT
};


//  "OverlayCFG"
//
struct OverlayCFG {
    bool                        locked          = false;
    OverlayPlacement            placement       {OverlayPlacement::Custom};
    BBoxAnchor                  src_anchor      = BBoxAnchor::NorthWest;        // top-centre of the window
    OffscreenPolicy             offscreen       {OffscreenPolicy::Clamp};       // NEW
//
//
    ImVec2                      anchor_px       {0,0};     // pixel inset / offset
    ImVec2                      anchor_ws       {0,0};     // world-space anchor
//
    std::function<void()>       draw_fn         {};                    // widgets callback
};


//  "OverlayStyle"
//
struct OverlayStyle {
    float                       alpha           = 0.65f;
    ImU32                       bg              = 0x000000FF;
};


//  "Overlay_t"
//      MAIN "STATE" / "DATA" PACKET FOR EACH OVERLAY WINDOW...
//
template<typename T = uint32_t>
struct Overlay_t {
    using                       OverlayID       = T;
//
    OverlayID                   id              = 0;
    bool                        visible         = false;                 // owner sets false to retire
    ImGuiWindowFlags            flags           = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    OverlayCFG                  cfg             {};
    OverlayStyle                style           {};
};


//  "OverlayState"
//      **DEPRECATED** > State for the OLD "HELPER" OVERLAY MENU / DEBUGGER MENU...
struct OverlayState {
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
// *************************************************************************** //   END "OVERLAY"





    
// *************************************************************************** //
//
//
//
//      3.      OTHER STATE STRUCTURES.
// *************************************************************************** //
// *************************************************************************** //

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
  
// *************************************************************************** //
// *************************************************************************** //   END "OTHER STATES"











// *************************************************************************** //
//
//
//
//      99.     EDITOR SETTINGS / CONFIGS...
// *************************************************************************** //
// *************************************************************************** //

//  "EditorState"
//
template<typename VertexID, typename PathID>
struct EditorState
{
// *************************************************************************** //
//                                  OVERALL STATE...
// *************************************************************************** //
    Mode                                m_mode                          = Mode::Default;
    ImPlotInputMap                      m_backup;
// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                                  TRANSIENT STATE...
// *************************************************************************** //
    //
    //                              BEHAVIORS:
    bool                                m_show_grid                     = true;
    bool                                m_dragging                      = false;
    bool                                m_lasso_active                  = false;
    bool                                m_show_sel_overlay              = false;
    bool                                m_show_debug_overlay            = true;
    //
    //                              OTHER:
    bool                                m_pending_clear                 = false;    //  pending click selection state ---
    //
    //                              UTILITY:
    float                               m_bar_h                         = 0.0f;
    ImVec2                              m_avail                         = ImVec2(0.0f, 0.0f);
    ImVec2                              m_p0                            = ImVec2(0.0f, 0.0f);
    ImVec2                              m_p1                            = ImVec2(0.0f, 0.0f);
// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                                      TOOL STATE (TODO: RE-HOME THESE INTO TOOL STATE OBJs)...
// *************************************************************************** //
    //
    //                              LASSO TOOL / SELECTION:
    ImVec2                              m_lasso_start                   = ImVec2(0.f, 0.f);
    ImVec2                              m_lasso_end                     = ImVec2(0.f, 0.f);
    VertexID                            m_next_id                       = 1;
    PathID                              m_next_pid                      = 1;        // counter for new path IDs
    //
    //                              PEN-TOOL STATE:
    bool                                m_drawing                       = false;
    bool                                m_dragging_handle               = false;
    bool                                m_dragging_out                  = true;
    VertexID                            m_drag_vid                      = 0;
// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                                      MISC STATE...
// *************************************************************************** //

                                        //
                                        //  ...
                                        //

// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                                      BROWSER / SELECTION STATE...
// *************************************************************************** //
    //
    //                              MISC:
    //
    //                              SELECTION / BROWSER STATE:
    ImGuiTextFilter                     m_browser_filter;                           // search box text filter
    int                                 m_browser_anchor                = -1;       // anchor index for Shift‑range select
    int                                 m_inspector_vertex_idx          = -1;       // anchor index for Shift‑range select
// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                                      SERIALIZATION STUFF...
// *************************************************************************** //
    std::mutex                          m_task_mtx;
    std::vector<std::function<void()>>  m_main_tasks;
    std::atomic<bool>                   m_io_busy                       {false};
    IoResult                            m_io_last                       {IoResult::Ok};
    std::string                         m_io_msg                        {  };
    //
    std::atomic<bool>                   m_sdialog_open                  = {false};
    std::atomic<bool>                   m_odialog_open                  = {false};
// *************************************************************************** //



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "EditorState"
};






//  "EditorStyle"
struct EditorStyle
{
    static constexpr size_t     DEF_HISTORY_CAP                 = 64;
// *************************************************************************** //
// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                              INTERACTION / RESPONSIVENESS CONSTANTS...
// *************************************************************************** //
    float                       GRID_STEP                       = 64.0f;
    float                       HIT_THRESH_SQ                   = 6.0f * 6.0f;
//
    int                         PEN_DRAG_TIME_THRESHOLD         = 0.05;                             //  PEN_DRAG_TIME_THRESHOLD         // seconds.
    float                       PEN_DRAG_MOVEMENT_THRESHOLD     = 4.0f;                             //  PEN_DRAG_MOVEMENT_THRESHOLD     // px  (was 2)
// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                              STANDARDIZED STYLE...
// *************************************************************************** //
    ImU32                       COL_POINT_DEFAULT               = IM_COL32(0,255,0,255);        // idle green
    ImU32                       COL_POINT_HELD                  = IM_COL32(255,100,0,255);      // while dragging
    ImU32                       COL_SELECTION_OUT               = IM_COL32(255,215,0,255);      // gold outline
    float                       DEFAULT_POINT_RADIUS            = 12.0f;                        // px
// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                              CANVAS  | USER-INTERFACE CONSTANTS...
// *************************************************************************** //
//                          HANDLES:
    ImU32                       ms_HANDLE_COLOR                 = IM_COL32(255, 215, 0, 255);       //  ms_HANDLE_COLOR             //  gold
    ImU32                       ms_HANDLE_HOVER_COLOR           = IM_COL32(255, 255, 0, 255);       //  ms_HANDLE_HOVER_COLOR       //  yellow
    float                       ms_HANDLE_SIZE                  = 3.0f;
    float                       HANDLE_BOX_SIZE                 = 4.f;                              //  ms_HANDLE_SIZE              //  px half-side
// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                              CANVAS  | PREVIEW APPEARANCE CONSTANTS...
// *************************************************************************** //
//                          VERTICES:
    float                       VERTEX_PREVIEW_RADIUS           = 4.0f;
    ImU32                       VERTEX_PREVIEW_COLOR            = IM_COL32(255, 255, 0, 160);
// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                              TOOL CONSTANTS...
// *************************************************************************** //
//
//                          STANDARDIZED STYLE:
//
//                          PEN-TOOL ANCHORS:
//                              //  ...
//
//                          LASSO TOOL:
    ImU32                       COL_LASSO_OUT                   = IM_COL32(255,215,0,255); // gold outline
    ImU32                       COL_LASSO_FILL                  = IM_COL32(255,215,0,40);  // translucent fill
//
//                          BOUNDING BOX:
    ImU32                       SELECTION_BBOX_COL              = IM_COL32(0, 180, 255, 153);   //  cyan-blue
    float                       SELECTION_BBOX_TH               = 1.5f;
// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                              CURSOR CONSTANTS...
// *************************************************************************** //
//                          PEN-TOOL CURSOR STUFF:
    float                       PEN_RING_RADIUS                 = 6.0f;                             //  PEN_RING_RADIUS     [px]
    float                       PEN_RING_THICK                  = 1.5f;                             //  PEN_RING_THICK      [px]
    int                         PEN_RING_SEGMENTS               = 32;                               //  NUMBER OF SEGMENTS TO DRAW OUTER RING.
    float                       PEN_DOT_RADIUS                  = 2.0f;                             //  PEN_DOT_RADIUS      // px
    int                         PEN_DOT_SEGMENTS                = 16;                               //  NUMBER OF SEGMENTS TO DRAW INNER DOT.
    //
    //
    ImU32                       PEN_COL_NORMAL                  = IM_COL32(255,255,0,255);          //  PEN_COL_NORMAL      // yellow
    ImU32                       PEN_COL_EXTEND                  = IM_COL32(  0,255,0,255);          //  PEN_COL_EXTEND      // green
// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                              WIDGETS / UI-CONSTANTS / APP-APPEARANCE...
// *************************************************************************** //
    //
    //                      BROWSER CHILD-WINDOW COLORS:
    ImVec4                      ms_CHILD_FRAME_BG1              = ImVec4(0.205f,    0.223f,     0.268f,     1.000f);//      ms_CHILD_FRAME_BG1      //   BASE = #343944
    ImVec4                      ms_CHILD_FRAME_BG1L             = ImVec4(0.091f,    0.099f,     0.119f,     0.800f);//      ms_CHILD_FRAME_BG1L     //   #17191E
    ImVec4                      ms_CHILD_FRAME_BG1R             = ImVec4(0.129f,    0.140f,     0.168f,     0.800f);//      ms_CHILD_FRAME_BG1R     //   #21242B
    
    ImVec4                      ms_CHILD_FRAME_BG2              = ImVec4(0.149f,    0.161f,     0.192f,     1.000f);//      ms_CHILD_FRAME_BG2      // BASE = #52596B
    ImVec4                      ms_CHILD_FRAME_BG2L             = ImVec4(0.188f,    0.203f,     0.242f,     0.750f);//      ms_CHILD_FRAME_BG2L     // ##353A46
    ImVec4                      ms_CHILD_FRAME_BG2R             = ImVec4(0.250f,    0.271f,     0.326f,     0.750f);//      ms_CHILD_FRAME_BG2R     // #5B6377
    //
    //                      BROWSER CHILD-WINDOW STYLE:
    float                       ms_VERTEX_SUBBROWSER_HEIGHT     = 0.85f;    //  ms_VERTEX_SUBBROWSER_HEIGHT
    float                       ms_CHILD_BORDER1                = 2.0f;     //  ms_CHILD_BORDER1
    float                       ms_CHILD_BORDER2                = 1.0f;     //  ms_CHILD_BORDER2
    float                       ms_CHILD_ROUND1                 = 8.0f;     //  ms_CHILD_ROUND1
    float                       ms_CHILD_ROUND2                 = 4.0f;     //  ms_CHILD_ROUND2
    //
    //                      BROWSER CHILD-WINDOW DIMENSIONS:
    float                       OBJ_PROPERTIES_REL_WIDTH        = 0.5f;     // Relative width of OBJECT PROPERTIES PANEL.
    float                       VERTEX_SELECTOR_REL_WIDTH       = 0.075f;   // Rel. width of Vertex SELECTOR COLUMN.
    float                       VERTEX_INSPECTOR_REL_WIDTH      = 0.0f;     // Rel. width of Vertex INSPECTOR COLUMN.
    //
    //                      BROWSER **ALL** WIDGET STUFF:
    float                       ms_BROWSER_BUTTON_SEP           = 8.0f;
    float                       ms_BROWSER_SELECTABLE_SEP       = 16.0f;
    //
    //                      BROWSER PATH WIDGET STUFF:
    float                       ms_BROWSER_OBJ_LABEL_WIDTH      = 55.0f;
    float                       ms_BROWSER_OBJ_WIDGET_WIDTH     = 256.0f;
    //
    //                      BROWSER VERTEX WIDGET STUFF:
    float                       ms_BROWSER_VERTEX_LABEL_WIDTH   = 55.0f;
    float                       ms_BROWSER_VERTEX_WIDGET_WIDTH  = 196.0f;
    //
    //
    //
    //                      SYSTEM PREFERENCES WIDGETS:
    float                       ms_SETTINGS_LABEL_WIDTH         = 196.0f;
    float                       ms_SETTINGS_WIDGET_WIDTH        = 256.0f;
    //
// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                              RENDERING CONSTANTS...
// *************************************************************************** //
    //                      VERTEX RENDERING:
    int                         ms_VERTEX_NUM_SEGMENTS          = 12;
    //
    //                      BEZIER RENDERING:
    int                         ms_BEZIER_SEGMENTS              = 0;        //  ms_BEZIER_SEGMENTS
    int                         ms_BEZIER_HIT_STEPS             = 20;       //  ms_BEZIER_HIT_STEPS
    int                         ms_BEZIER_FILL_STEPS            = 24;       //  ms_BEZIER_FILL_STEPS
// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                              UTILITY...
// *************************************************************************** //
                                //
                                //  ...
                                //
// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                              MISC. / UNKNOWN CONSTANTS (RELOCATED FROM CODE)...
// *************************************************************************** //
    float                       TARGET_PX                       = 20.0f;                    //     desired screen grid pitch"  | found in "_update_grid"
    float                       PICK_PX                         = 6.0f;                     //     desired screen grid pitch"  | found in "_update_grid"

// *************************************************************************** //



// *************************************************************************** //
// *************************************************************************** //   END "EditorStyle"
};






//  "ZOrderCFG_t"
//
template<typename ZID>
struct ZOrderCFG_t {
    ZID         Z_EDITOR_BACK               = 1;                        //  Grid / background
    ZID         Z_FLOOR_USER                = 255;                      //  First user layer
    ZID         Z_EDITOR_FRONT              = UINT32_MAX - 2;           //  Overlays, guides
    ZID         Z_CEIL_USER                 = Z_EDITOR_FRONT - 1;       //  Max allowed for user items
    ZID         RENORM_THRESHOLD            = 10'000;                   //  Span triggering re-pack
};
//
//  "to_json"
template <typename ZID>
inline void to_json(nlohmann::json & j, const ZOrderCFG_t<ZID> & obj) {
    j = {
        { "Z_EDITOR_BACK",          obj.Z_EDITOR_BACK       },
        { "Z_FLOOR_USER",           obj.Z_FLOOR_USER        },
        { "Z_EDITOR_FRONT",         obj.Z_EDITOR_FRONT      },
        { "Z_CEIL_USER",            obj.Z_CEIL_USER         },
        { "RENORM_THRESHOLD",       obj.RENORM_THRESHOLD    }
    };
}
//
//  "from_json"
template <typename ZID>
inline void from_json(nlohmann::json & j, ZOrderCFG_t<ZID> & obj) {
    j.at("Z_EDITOR_BACK")           .get_to(obj.Z_EDITOR_BACK);
    j.at("Z_FLOOR_USER")            .get_to(obj.Z_FLOOR_USER);
    j.at("Z_EDITOR_FRONT")          .get_to(obj.Z_EDITOR_FRONT);
    j.at("Z_CEIL_USER")             .get_to(obj.Z_CEIL_USER);
    j.at("RENORM_THRESHOLD")        .get_to(obj.RENORM_THRESHOLD);
}
















// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.












#endif      //  _CBWIDGETS_EDITOR_TYPES_H  //
// *************************************************************************** //
// *************************************************************************** //   END FILE.

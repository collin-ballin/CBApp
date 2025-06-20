/***********************************************************************************
*
*       ********************************************************************
*       ****               E D I T O R . H  ____  F I L E               ****
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
#ifndef _CBWIDGETS_EDITOR_H
#define _CBWIDGETS_EDITOR_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
#include "app/_init.h"
#include "app/state/_state.h"
#include "utility/utility.h"
#include "widgets/editor/_types.h"

//  0.2     STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <iomanip>          //           for std::setw / std::setfill (ID labels)
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



//  0.  STATIC INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

static inline bool is_curved(const Vertex* a, const Vertex* b)
{
    return (a->out_handle.x || a->out_handle.y ||
            b->in_handle.x  || b->in_handle.y);
}

static inline ImVec2 cubic_eval(const Vertex * a, const Vertex* b, float t)
{
    const float u  = 1.0f - t;
    const float w0 = u*u*u;
    const float w1 = 3*u*u*t;
    const float w2 = 3*u*t*t;
    const float w3 = t*t*t;

    ImVec2 P0{ a->x, a->y };
    ImVec2 P1{ a->x + a->out_handle.x, a->y + a->out_handle.y };
    ImVec2 P2{ b->x + b->in_handle.x,  b->y + b->in_handle.y  };
    ImVec2 P3{ b->x, b->y };

    return { w0*P0.x + w1*P1.x + w2*P2.x + w3*P3.x,
             w0*P0.y + w1*P1.y + w2*P2.y + w3*P3.y };
}

static bool point_in_polygon(const std::vector<ImVec2>& poly, ImVec2 p)
{
    bool inside = false;
    const size_t n = poly.size();
    for (size_t i = 0, j = n - 1; i < n; j = i++)
    {
        bool intersect = ((poly[i].y > p.y) != (poly[j].y > p.y)) &&
                         (p.x < (poly[j].x - poly[i].x) * (p.y - poly[i].y) /
                                 (poly[j].y - poly[i].y + 1e-6f) + poly[i].x);
        if (intersect) inside = !inside;
    }
    return inside;
}

static inline ImVec4 u32_to_f4(ImU32 c)     { return ImGui::ColorConvertU32ToFloat4(c); }

static inline ImU32  f4_to_u32(ImVec4 f)    { return ImGui::ColorConvertFloat4ToU32(f); }

//  Returns length of a 2-D vector
static inline float vec_len(const ImVec2& v) { return sqrtf(v.x*v.x + v.y*v.y); }

//  Normalises `v` unless zero; returns {0,0} if zero
static inline ImVec2 vec_norm(const ImVec2& v)
{
    float l = vec_len(v);
    return (l > 0.f) ? ImVec2{ v.x / l, v.y / l } : ImVec2{0,0};
}

//  Mirrors the opposite handle according to anchor kind
static inline void mirror_handles(Vertex& v, bool dragged_out_handle)
{
    ImVec2& h_dragged = dragged_out_handle ? v.out_handle : v.in_handle;
    ImVec2& h_other   = dragged_out_handle ? v.in_handle  : v.out_handle;

    switch (v.kind)
    {
        case AnchorType::Corner:
            /* no coupling */ break;

        case AnchorType::Smooth: {
            float len = vec_len(h_other);           // keep original length
            ImVec2 dir = vec_norm(h_dragged);
            h_other = ImVec2{ -dir.x * len, -dir.y * len };
            break;
        }
        case AnchorType::Symmetric:
            h_other = ImVec2{ -h_dragged.x, -h_dragged.y };
            break;
    }
}


//  "find_vertex_mut"
//
static inline Vertex* find_vertex_mut(std::vector<Vertex>& arr, uint32_t id)
{
    for (Vertex& v : arr)
        if (v.id == id) return &v;
    return nullptr;
}


static inline bool alt_down()
{
    ImGuiIO& io = ImGui::GetIO();
#if defined(ImGuiMod_Alt)          // 1.90+
    if (io.KeyMods & ImGuiMod_Alt) return true;
#endif
#if defined(ImGuiKey_ModAlt)       // some back-ends set this explicitly
    if (ImGui::IsKeyDown(ImGuiKey_ModAlt)) return true;
#endif
    if (io.KeyAlt)                 // 1.89 back-ends set this bool
        return true;
#if defined(ImGuiKey_LeftAlt)
    if (ImGui::IsKeyDown(ImGuiKey_LeftAlt)  ||
        ImGui::IsKeyDown(ImGuiKey_RightAlt))
        return true;
#endif
    return false;
}



// *************************************************************************** //
// *************************************************************************** //
//                         Editor:
//                 Editor Widget for Dear ImGui.
// *************************************************************************** //
// *************************************************************************** //
       
//  "Editor"
//
class Editor {
public:
        friend class            App;
        using                   Pos                             = Vertex;
        using                   EndpointInfo                    = EndpointInfo;
//      CBAPP_APPSTATE_ALIAS_API        //  CLASS-DEFINED, NESTED TYPENAME ALIASES.
//
//
// *************************************************************************** //
// *************************************************************************** //
    static constexpr std::array<const char*, static_cast<size_t>(Mode::Count)>
                                ms_MODE_LABELS                  = DEF_EDITOR_STATE_NAMES;
    static constexpr float      ms_LIST_COLUMN_WIDTH            = 240.0f;   // px width of point‑list column



public:
    //  1.              INITIALIZATION METHODS...
    // *************************************************************************** //
                                Editor                          (void);
                                ~Editor                         (void);


    //  2.              PUBLIC MEMBER FUNCTIONS...
    // *************************************************************************** //
    void                        Begin                           (const char* id = "##EditorCanvas");
    void                        DrawBrowser                     (void);



// *************************************************************************** //
// *************************************************************************** //
private:
    //
    //  2.A             PROTECTED OPERATION MEMBER FUNCTIONS...
    // *************************************************************************** //
    // *************************************************************************** //
    
    

    // *************************************************************************** //
    //      PRIMARY STATE HANDLERS.         |   "editor.cpp" ...
    // *************************************************************************** //
    void                        _handle_default                 (const Interaction & );
    void                        _handle_line                    (const Interaction & );
    void                        _handle_point                   (const Interaction & );
    void                        _handle_pen                     (const Interaction & );
    void                        _handle_scissor                 (const Interaction & );
    void                        _handle_add_anchor              ([[maybe_unused]] const Interaction & );
    void                        _handle_remove_anchor           ([[maybe_unused]] const Interaction & );
    void                        _handle_edit_anchor             ([[maybe_unused]] const Interaction & );
    //
    //
    // *************************************************************************** //
    //      BROWSER STUFF.                  |   "browser.cpp" ...
    // *************************************************************************** //
    void                        _draw_path_inspector_column         (void);
    void                        _draw_path_list_column              (void);
    void                        _draw_vertex_list_subcolumn         (Path & );          // left part
    void                        _draw_vertex_inspector_subcolumn    (Path & );
    //
    void                        _draw_multi_path_inspector          (void);
    void                        _draw_single_path_inspector         (void);
    //
    //
    // *************************************************************************** //
    //      PEN-TOOL STUFF.                 |   "pen_tool.cpp" ...
    // *************************************************************************** //
    bool                        _pen_cancel_if_escape           ([[maybe_unused]] const Interaction & );
    void                        _pen_begin_handle_drag          (uint32_t vid, bool out_handle, const bool force_select=false);
    bool                        _pen_try_begin_handle_drag      (const Interaction & );
    void                        _pen_update_handle_drag         ([[maybe_unused]] const Interaction & );
    void                        _pen_begin_path_if_click_empty  (const Interaction & );
    inline bool                 _pen_click_hits_first_vertex    (const Interaction &, const Path &) const;
    void                        _pen_append_or_close_live_path  (const Interaction & );
    //
    std::optional<size_t>       _path_idx_if_last_vertex        (uint32_t vid) const;
    inline bool                 _can_join_selected_path         (void) const;
    void                        _join_selected_open_path        (void);
    void                        _draw_pen_cursor                (const ImVec2 &, ImU32);
    //
    //
    // *************************************************************************** //
    //      RENDERING FUNCTIONS.            |   "render.cpp" ...
    // *************************************************************************** //
    //
    //                      GRID:
    void                        _draw_grid                      (ImDrawList *, const ImVec2 &, const ImVec2 &) const;
    ImVec2                      _grid_snap                      (ImVec2 ) const;
    void                        _grid_draw                      (ImDrawList *, const ImVec2 &, const ImVec2 & ) const;
    void                        _grid_handle_shortcuts          (void);
    //
    //                      RENDERING INTERACTIBLES:
    void                        _draw_paths                     (ImDrawList *,   const ImVec2 & ) const;
    void                        _draw_lines                     (ImDrawList *,   const ImVec2 & ) const;
    void                        _draw_points                    (ImDrawList *,   const ImVec2 & ) const;
    //
    //
    // *************************************************************************** //
    //      SELECTION MECHANICS.            |   "selection.cpp" ...
    // *************************************************************************** //
    int                         _hit_point                      (const Interaction & ) const;
    std::optional<Hit>          _hit_any                        (const Interaction & ) const;
    std::optional<PathHit>      _hit_path_segment               (const Interaction & ) const;
    //
    void                        _process_selection              (const Interaction & );
    //
    void                        show_selection_context_menu     (const Interaction & it);
    void                        resolve_pending_selection       (const Interaction & it);
    void                        update_move_drag_state          (const Interaction & it);
    void                        start_move_drag                 (const ImVec2 & anchor_ws);
    void                        add_hit_to_selection            (const Hit & hit);
    //
    //
    void                        _update_cursor_select           (const Interaction & ) const;
    void                        _rebuild_vertex_selection       (void);   // decl
    //
    void                        _draw_selection_overlay         (ImDrawList *, const ImVec2 & ) const;
    //
    bool                        _selection_bounds               (ImVec2 & tl, ImVec2 & br) const;
    void                        _draw_selected_handles          (ImDrawList *, const ImVec2 & ) const;
    void                        _draw_selection_bbox            (ImDrawList *, const ImVec2 & ) const;
    //
    //                      BOUNDING BOX MECHANICS:
    void                        _start_bbox_drag                (uint8_t handle_idx, const ImVec2 & tl, const ImVec2 & br);
    void                        _update_bbox                    (void);
    //
    //
    // *************************************************************************** //
    //      OVERLAY.                        |   "tools.cpp" ...
    // *************************************************************************** //
    void                        _handle_overlay                 ([[maybe_unused]] const Interaction & );
    bool                        _overlay_begin_window           (void);
    void                        _overlay_end_window             (void);
    //
    void                        _overlay_draw_context_menu      (void);
    void                        _overlay_update_position        (void);
    void                        overlay_log                     (std::string msg, float secs = 2.0f);
    //
    void                        _overlay_draw_content           ([[maybe_unused]]const Interaction &);
    void                        _overlay_display_main_content   ([[maybe_unused]]const Interaction &);
    void                        _overlay_display_extra_content  ([[maybe_unused]]const Interaction &);
    //
    //
    // *************************************************************************** //
    //      UTILITIES.                      |   "utility.cpp" ...
    // *************************************************************************** //
    Pos *                       find_vertex                     (std::vector<Pos> & , uint32_t);
    const Pos *                 find_vertex                     (const std::vector<Pos> & , uint32_t) const;
    std::optional<EndpointInfo> _endpoint_if_open               (uint32_t vid) const;
    //
    //                      DATA MODIFIER UTILITIES:
    void                        _add_point_glyph                (uint32_t vid);
    uint32_t                    _add_vertex                     (ImVec2 w);
    void                        _add_point                      (ImVec2 w);
    void                        _erase_vertex_and_fix_paths     (uint32_t vid);
    void                        _erase_path_and_orphans         (size_t vid);
    //
    //                      APP UTILITY OPERATIONS:
    bool                        _try_begin_handle_drag          (const Interaction & );
    void                        _scissor_cut                    (const PathHit & );
    void                        _start_lasso_tool               (void);
    void                        _update_lasso                   (const Interaction & );
    //
    //                      LOCAMOTION UTILITIES:
    inline ImVec2               _world_from_screen              (ImVec2 scr) const  { return { (scr.x + m_scroll.x) / m_zoom, (scr.y + m_scroll.y) / m_zoom }; }
    void                        _update_world_extent            (void);
    void                        _zoom_canvas                    (const Interaction & );
    void                        _clamp_scroll                   (void);
    void                        _clamp_zoom                     (float & );
    //
    //                      MISC. UTILITIES:
    void                        _draw_controls                  (void);
    void                        _display_canvas_settings        (void);
    void                        _clear_all                      (void);



    // *************************************************************************** //
    //
    //
    //  2.C             INLINE FUNCTIONS...
    // *************************************************************************** //
    // *************************************************************************** //
    
    //  "_mode_has"
    inline bool                 _mode_has                       (Capability cap) const
    { return MODE_CAPS[static_cast<size_t>(m_mode)] & cap; }
    
    
    //  "maybe_snap"
    inline ImVec2               maybe_snap                      (ImVec2 w) const
    { return m_grid.snap_on ? _grid_snap(w) : w; }
        
        
    //  "want_snap"
    inline bool                 want_snap                       (void) const
    { return m_grid.snap_on || ImGui::GetIO().KeyShift; }
        
        
    //  "_grid_step_px"
    inline float                _grid_step_px                   (void) const
    { return m_grid.world_step * m_zoom; }
    
    
    //
    //
    //
    // *************************************************************************** //
    // *************************************************************************** //   END INLINE FUNCITONS...



    // *************************************************************************** //
    //
    //
    //  3.              PROTECTED DATA-MEMBERS...
    // *************************************************************************** //
    // *************************************************************************** //
    
    
    // *************************************************************************** //
    //      IMPORTANT DATA...
    // *************************************************************************** //
    std::vector<Pos>            m_vertices;
    std::vector<Point>          m_points;
    std::vector<Line>           m_lines;
    std::vector<Path>           m_paths;                       // new path container
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      BROWSER STUFF...
    // *************************************************************************** //
    std::string                 WinInfo_uuid                    = "Editor Browser";
    ImGuiWindowFlags            WinInfo_flags                   = ImGuiWindowFlags_None | ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY;
    bool                        WinInfo_open                    = true;
    ImGuiWindowClass            m_window_class;
    //
    ImGuiTextFilter             m_browser_filter;                           // search box text filter
    int                         m_browser_anchor                = -1;       // anchor index for Shift‑range select
    int                         m_inspector_vertex_idx          = -1;       // anchor index for Shift‑range select
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      APPLICATION STATE...
    // *************************************************************************** //
    //                      OVERALL STATE:
    Mode                        m_mode                          = Mode::Default;
    bool                        m_show_grid                     = true;
    //
    //                      MISC. STATE:
    bool                        m_dragging                      = false;
    bool                        m_lasso_active                  = false;
    bool                        m_pending_clear                 = false;    //  pending click selection state ---
    //
    //                      PEN-TOOL STATE:
    bool                        m_drawing                       = false;
    bool                        m_dragging_handle               = false;
    bool                        m_dragging_out                  = true;
    uint32_t                    m_drag_vid                      = 0;
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      OBJECTS...
    // *************************************************************************** //
    Bounds                      m_world_bounds                  = {
        /*min_x=*/0.0f,         /*min_y=*/0.0f,
        /*max_x=*/1000.0f,      /*max_y=*/1000.0f
    };
    PenState                    m_pen;
    GridSettings                m_grid                          = { 100.0f,  true,  false };
    OverlayState                m_overlay;
    //
    std::optional<Hit>          m_pending_hit;   // candidate under mouse when button pressed   | //  pending click selection state ---
    Selection                   m_sel;
    mutable BoxDrag             m_boxdrag;
    MoveDrag                    m_movedrag;
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      VARIABLES FOR SPECIFIC MECHANICS...
    // *************************************************************************** //
    //                      GRID / CANVAS:
    float                       m_zoom                          = 1.0f;
    ImVec2                      m_scroll                        = ImVec2( 0.0f,     0.0f    );
    ImVec2                      m_world_extent                  = ImVec2( 1000.f,   1000.f  );
    //
    static constexpr float      ms_GRID_STEP_MIN                = 2.0f;   // world-units
    static constexpr float      ms_GRID_LABEL_PAD               = 2.0f;
    //
    //                      LASSO TOOL / SELECTION:
    ImVec2                      m_lasso_start                   = ImVec2(0.f, 0.f);
    ImVec2                      m_lasso_end                     = ImVec2(0.f, 0.f);
    uint32_t                    m_next_id                       = 1;
    //
    //                      BBOX SCALING:
    mutable int                 m_hover_handle                  = -1;
    mutable ImVec2              m_origin_scr                    = {0.f, 0.f};   // screen-space canvas origin                       //  -1 = none, 0-7 otherwise (corners+edges)
    //
    //                      UTILITY:
    float                       m_bar_h                         = 0.0f;
    ImVec2                      m_avail                         = ImVec2(0.0f, 0.0f);
    ImVec2                      m_p0                            = ImVec2(0.0f, 0.0f);
    ImVec2                      m_p1                            = ImVec2(0.0f, 0.0f);
    // *************************************************************************** //






    // *************************************************************************** //
    //      STATIC / CONSTANTS...
    // *************************************************************************** //
    //
    //                      INTERACTION / RESPONSIVENESS CONSTANTS:
    static constexpr int        PEN_DRAG_TIME_THRESHOLD         = 0.05;     // seconds.
    static constexpr float      PEN_DRAG_MOVEMENT_THRESHOLD     = 4.0f;     // px  (was 2)
    //
    //
    //                      USER INTERFACE CONSTANTS:
    //                          Handles.
    static constexpr ImU32      ms_HANDLE_COLOR                 = IM_COL32(255, 215, 0, 255);   //  gold
    static constexpr float      ms_HANDLE_SIZE                  = 3.0f;                         //  px half-side
    static constexpr ImU32      ms_HANDLE_HOVER_COLOR           = IM_COL32(255, 255, 0, 255);   //  yellow
    //
    //                          Pen-Tool Anchors.
    static constexpr ImU32      PEN_ANCHOR_COLOR                = IM_COL32(255, 200, 0, 255);
    static constexpr float      PEN_ANCHOR_RADIUS               = 5.0f;
    //
    //                          Lasso.
    //                          //  ...
    //
    //                          Bounding Box.
    static constexpr ImU32      SELECTION_BBOX_COL              = IM_COL32(0, 180, 255, 255);   //  cyan-blue
    static constexpr float      SELECTION_BBOX_TH               = 1.5f;
    //
    //
    //                      CURSOR CONSTANTS:
    //                          Pen-Tool Cursor Stuff.
    static constexpr float      PEN_RING_RADIUS                 = 6.0f;                         // px
    static constexpr float      PEN_RING_THICK                  = 1.5f;                         // px
    static constexpr float      PEN_DOT_RADIUS                  = 2.0f;                         // px
    static constexpr ImU32      PEN_COL_NORMAL                  = IM_COL32(255,255,0,255);      // yellow
    static constexpr ImU32      PEN_COL_EXTEND                  = IM_COL32(  0,255,0,255);      // green
    //
    //
    //
    //                      UTILITY:
    // *************************************************************************** //
    //
    //                      RENDERING CONSTANTS:
    static constexpr int        ms_BEZIER_SEGMENTS              = 0;
    static constexpr int        ms_BEZIER_HIT_STEPS             = 20;
    static constexpr int        ms_BEZIER_FILL_STEPS            = 24;
    //
    //                      APPEARANCE / WIDGETS / UI CONSTANTS:
    static constexpr float      ms_VERTEX_SUBBROWSER_HEIGHT     = 0.85f;
    static constexpr ImVec4     ms_CHILD_FRAME_BG1              = ImVec4(0.205f,    0.223f,     0.268f,     1.000f);//  BASE = #343944
    static constexpr ImVec4     ms_CHILD_FRAME_BG1L             = ImVec4(0.091f,    0.099f,     0.119f,     0.800f);//  #17191E
    static constexpr ImVec4     ms_CHILD_FRAME_BG1R             = ImVec4(0.129f,    0.140f,     0.168f,     0.800f);//  #21242B
    
    static constexpr ImVec4     ms_CHILD_FRAME_BG2              = ImVec4(0.149f,    0.161f,     0.192f,     1.000f);//  BASE = #52596B
    static constexpr ImVec4     ms_CHILD_FRAME_BG2L             = ImVec4(0.188f,    0.203f,     0.242f,     0.750f);//  ##353A46
    static constexpr ImVec4     ms_CHILD_FRAME_BG2R             = ImVec4(0.250f,    0.271f,     0.326f,     0.750f);//  #5B6377
    //
    //                          Browser Child-Window Sizes.
    static constexpr float      ms_CHILD_BORDER1                = 2.0f;
    static constexpr float      ms_CHILD_BORDER2                = 1.0f;
    static constexpr float      ms_CHILD_ROUND1                 = 8.0f;
    static constexpr float      ms_CHILD_ROUND2                 = 4.0f;




// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
};//    END "EDITOR" CLASS DEFINITION.




//  MORE STATIC HELPERS...
// *************************************************************************** //
// *************************************************************************** //

//  "mode_label"
//
static inline const char * mode_label(Mode m)
{ return Editor::ms_MODE_LABELS[ static_cast<size_t>(m) ]; }







// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.






#endif      //  _CBWIDGETS_EDITOR_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.


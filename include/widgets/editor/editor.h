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
//  #include "app/_init.h"
#include "app/state/_init.h"
#include "app/state/state.h"

#include "utility/utility.h"
#include "widgets/editor/_icon.h"
#include "widgets/editor/_editor.h"
//  #include "widgets/editor/_types.h"

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



//      FORWARD DECLARATIONS...
// *************************************************************************** //
// *************************************************************************** //

namespace                       app                         { class AppState; }
struct                          EditorSnapshot;
class                           History;
struct                          SnapshotCmd;
struct                          Command;
//
//
struct                          Vertex_Tag                  {};
struct                          Point_Tag                   {};
struct                          Line_Tag                    {};
struct                          Path_Tag                    {};
struct                          Overlay_Tag                 {};
struct                          Hit_Tag                     {};



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
        friend class                    App;
        friend struct                   EditorSnapshot;
        friend class                    History;
        friend struct                   SnapshotCmd;
        friend struct                   Command;
        //
        using                           Font                            = EditorIMPL::Font                      ;
        using                           Logger                          = EditorIMPL::Logger                    ;
        using                           LogLevel                        = EditorIMPL::LogLevel                  ;
        using                           CBCapabilityFlags               = EditorIMPL::CBCapabilityFlags         ;
        using                           Anchor                          = EditorIMPL::Anchor                    ;
    //
    //                              ID / INDEX TYPES:
    //  using                           ID                              = Editor::ID                        ;
        using                           VertexID                        = EditorIMPL::VertexID                  ;
        using                           HandleID                        = EditorIMPL::HandleID                  ;
        using                           PointID                         = EditorIMPL::PointID                   ;
        using                           LineID                          = EditorIMPL::LineID                    ;
        using                           PathID                          = EditorIMPL::PathID                    ;
        using                           ZID                             = EditorIMPL::ZID                       ;
        using                           OverlayID                       = EditorIMPL::OverlayID                 ;
        using                           HitID                           = EditorIMPL::HitID                     ;
    //
    //                              TYPENAME ALIASES (BASED ON INDEX TYPES):
        using                           Vertex                          = EditorIMPL::Vertex                    ;
    //  using                           Handle                          = EditorIMPL::Handle                    ;
        using                           Point                           = EditorIMPL::Point                     ;
        using                           Line                            = EditorIMPL::Line                      ;
        using                           Path                            = EditorIMPL::Path                      ;
        using                           PathKind                        = Path::PathKind                        ;
        using                           Payload                         = Path::Payload                         ;
    //
        using                           Overlay                         = EditorIMPL::Overlay                   ;
        using                           Hit                             = EditorIMPL::Hit                       ;
        using                           PathHit                         = EditorIMPL::PathHit                   ;
        using                           EndpointInfo                    = EditorIMPL::EndpointInfo              ;
    //
    //                              SUB-STATE ABSTRACTIONS:
        using                           EditorState                     = EditorIMPL::EditorState               ;
        using                           Selection                       = EditorIMPL::Selection                 ;
        using                           PenState                        = EditorIMPL::PenState                  ;
        using                           ShapeState                      = EditorIMPL::ShapeState                ;
        using                           BrowserState                    = EditorIMPL::BrowserState              ;
    //
        using                           Clipboard                       = EditorIMPL::Clipboard                 ;
//
//      CBAPP_APPSTATE_ALIAS_API        //  CLASS-DEFINED, NESTED TYPENAME ALIASES.
//
//
// *************************************************************************** //
// *************************************************************************** //
    static constexpr float              ms_LIST_COLUMN_WIDTH            = 340.0f;   // px width of point‑list column
    static constexpr float              ms_LABEL_WIDTH                  = 90.0f;
    static constexpr float              ms_WIDGET_WIDTH                 = 250.0f;
    //
    //                              SETTINGS TAB:
    static constexpr float              ms_SETTINGS_LABEL_WIDTH         = 196.0f;
    static constexpr float              ms_SETTINGS_WIDGET_WIDTH        = 300.0f;
    static constexpr const char *       ms_NO_ASSIGNED_FILE_STRING      = "UNASSIGNED";      // Used when there is no "File > Save As..." assigned to app...
    static constexpr ImVec2             ms_SETTINGS_BUTTON_SIZE         = ImVec2( 80,   25 );
    




    
// *************************************************************************** //
//
//
//  1.              TEMPORARY PUBLIC STUFF...
// *************************************************************************** //
// *************************************************************************** //
public:

    // *************************************************************************** //
    //  1.              INITIALIZATION METHODS...
    // *************************************************************************** //
                                        Editor                          (app::AppState & src);
                                        ~Editor                         (void);
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //                              RESIDENT OVERLAY DATA:
    // *************************************************************************** //
    struct ResidentEntry {
        OverlayID                   id;         //  runtime ID (filled in ctor)
        Overlay *                   ptr;        //  Reference.
        OverlayCFG                  cfg;        //  compile-time defaults
        OverlayStyle                style;
    };
    //
    //
    std::array<ResidentEntry, Resident::COUNT>      m_residents { {
        //
        //  Debugger:
        {   0,                                  //  ID.
            nullptr,                            //  Reference.
            {   /*  locked      */  false,
                /*  placement   */  OverlayPlacement::CanvasBL,
                /*  src_anchor  */  Anchor::North,
                /*  offscreen   */  OffscreenPolicy::Clamp,
                /*  anchor_px   */  ImVec2{0,8},                        //  nudge below bbox
                /*  anchor_ws   */  ImVec2{0,0},                        //  ws anchor filled each frame
                /*  draw_fn     */  {}                                  // draw_fn patched in ctor
            },
            {//     STYLE...
                /*  alpha       */  0.80f,
                /*  background  */  0x00000000
            }
        },
        //
        //  Selection:
        {   0,                                  //  ID.
            nullptr,                            //  Reference.
            {   /*  locked      */  true,
                /*  placement   */  OverlayPlacement::CanvasPoint,
                /*  src_anchor  */  Anchor::North,
                /*  offscreen   */  OffscreenPolicy::Hide,
                /*  anchor_px   */  ImVec2{0,8},                        //  nudge below bbox
                /*  anchor_ws   */  ImVec2{0,0},                        //  ws anchor filled each frame
                /*  draw_fn     */  {}                                  // draw_fn patched in ctor
            },
            {//     STYLE...
                /*  alpha       */  0.95f,
                /*  background  */  0x00000000
            }
        },
        //
        //  Shape:
        {   0,                                  //  ID.
            nullptr,                            //  Reference.
            {//     CONFIGURE...
                /*  locked      */  false,
                /*  placement   */  OverlayPlacement::CanvasBR,
                /*  src_anchor  */  Anchor::SouthEast,
                /*  offscreen   */  OffscreenPolicy::Clamp,
                /*  anchor_px   */  ImVec2{-30.0f,   45.0f},                 //  nudge below bbox
                /*  anchor_ws   */  ImVec2{0.0f,    0.0f},                  //  ws anchor filled each frame
                /*  draw_fn     */  {}                                  // draw_fn patched in ctor
            },
            {//     STYLE...
                /*  alpha       */  0.65f,
                /*  background  */  0xFF000000
            }
        }
    } };
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END TEMPORARY STUFF.


    
// *************************************************************************** //
//
//
//  2.              PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
public:
    
    // *************************************************************************** //
    //      MAIN API.                       |   "common.cpp" ...
    // *************************************************************************** //
    void                                save                                (void);
    void                                open                                (void);
    void                                undo                                (void);
    void                                redo                                (void);
    //
    void                                _draw_io_overlay                    (void);
    void                                save_async                          (std::filesystem::path path);
    void                                load_async                          (std::filesystem::path path);
    //
    void                                Begin                               (const char * id = "##EditorCanvas");
    void                                DrawBrowser                         (void);
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END PUBLIC MEMBER FUNCTIONS.


    
// *************************************************************************** //
//
//
//  2.A             PROTECTED OPERATION MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
protected:
    
    // *************************************************************************** //
    //      "Begin" HELPERS.                |   "editor.cpp" ...
    // *************************************************************************** //
    inline void                         _mode_switch_hotkeys                ([[maybe_unused]] const Interaction & );
    inline void                         _dispatch_mode_handler              ([[maybe_unused]] const Interaction & );
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      PRIMARY STATE HANDLERS.         |   "editor.cpp" ...
    // *************************************************************************** //
    inline void                         _handle_default                     (const Interaction & );
    inline void                         _handle_hand                        (const Interaction & );
    inline void                         _handle_line                        (const Interaction & );
    inline void                         _handle_point                       (const Interaction & );
    inline void                         _handle_pen                         (const Interaction & );
    inline void                         _handle_scissor                     (const Interaction & );
    inline void                         _handle_add_anchor                  ([[maybe_unused]] const Interaction & );
    inline void                         _handle_remove_anchor               ([[maybe_unused]] const Interaction & );
    inline void                         _handle_edit_anchor                 ([[maybe_unused]] const Interaction & );
    inline void                         _handle_overlays                    ([[maybe_unused]] const Interaction & );
    //
    inline void                         _handle_io                          (void);
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      BROWSER STUFF.                  |   "browser.cpp" ...
    // *************************************************************************** //
    //                              OBJECT BROWSER:
    void                                _dispatch_obj_inspector_column          (void);     //  PREVIOUSLY:     _draw_path_inspector_column
    //
    void                                _draw_obj_selector_column               (void);     //  PREVIOUSLY:     _draw_path_list_column
    void                                _draw_obj_selector_table                (void);
    //
    void                                _draw_single_obj_inspector              (void);     //  PREVIOUSLY:     _draw_single_path_inspector
    void                                _draw_multi_obj_inspector               (void);     //  PREVIOUSLY:     _draw_multi_path_inspector
    //
    //                              VERTEX BROWSER:
    void                                _draw_vertex_selector_column            (Path & );  //  PREVIOUSLY:     _draw_vertex_list_subcolumn
    void                                _draw_vertex_inspector_column           (Path & );  //  PREVIOUSLY:     _draw_vertex_inspector_subcolumn
    //
    //                              PANELS FOR EACH OBJECT TYPE:
    void                                _draw_obj_properties_panel              (Path & , const size_t );
    void                                _draw_vertex_properties_panel           (void);
    //
    //                              BROWSER HELPERS:
    void                                _handle_selection_click                 (int row_idx, bool mutable_path);
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      PEN-TOOL STUFF.                 |   "pen_tool.cpp" ...
    // *************************************************************************** //
    void                                _pen_begin_handle_drag              (VertexID vid, bool out_handle, const bool force_select=false);
    //bool                                _pen_try_begin_handle_drag          (const Interaction & );
    void                                _pen_update_handle_drag             ([[maybe_unused]] const Interaction & );
    void                                _pen_begin_path_if_click_empty      (const Interaction & );
    void                                _pen_append_or_close_live_path      (const Interaction & );
    //
    std::optional<size_t>               _path_idx_if_last_vertex            (VertexID vid) const;
    inline bool                         _pen_click_hits_first_vertex        (const Interaction &, const Path &) const;
    inline bool                         _can_join_selected_path             (void) const;
    void                                _join_selected_open_path            (void);
    void                                _draw_pen_cursor                    (const ImVec2 &, ImU32);
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      SHAPE TOOL STUFF.               |   "tools.cpp" ...
    // *************************************************************************** //
    //                              MAIN SHAPE-TOOL FUNCTIONS:
    void                                _handle_shape                       ([[maybe_unused]] const Interaction & );
    void                                _shape_begin_anchor                 ([[maybe_unused]] const Interaction & );
    void                                _shape_update_radius                ([[maybe_unused]] const Interaction & );
    //
    void                                _shape_commit                       (void);
    void                                _shape_reset                        (void);
    //
    //                              SPECIFIC SHAPE FUNCTIONS:
    VertexID                            _shape_add_vertex                   (const ImVec2 & ws);
    size_t                              _shape_build_rectangle              (const ImVec2 & cen, float r);
    size_t                              _shape_build_ellipse                (const ImVec2 & cen, float r);
    //
    //                              UTILITIES:
    //                                  ...
    //
    //                              DEPRECATED:
    void                                _shape_preview_draw                 (ImDrawList * dl) const;
    void                                _draw_shape_cursor                  (const Interaction &) const;
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      RESIDENT STUFF.                 |   "resident.cpp" ...
    // *************************************************************************** //
    //                              UTILITY:
    void                                _dispatch_resident_draw_fn          (Resident idx);
    //
    //                              DEBUGGER RESIDENT:
    void                                _draw_debugger_resident             (void);
    //
    //                              SELECTION RESIDENT:
    void                                _draw_selection_resident            (void);
    //
    //                              SHAPE-TOOL RESIDENT:
    void                                _draw_shape_resident                (void);
    void                                _draw_shape_resident_custom         (void);
    void                                _draw_shape_resident_multi          (void);
    void                                _draw_shape_resident_default        (void);
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      OVERLAY TOOL STUFF.             |   "tools.cpp" ...
    // *************************************************************************** //
    void                                _handle_overlay                     ([[maybe_unused]] const Interaction & );
    bool                                _overlay_begin_window               (void);
    void                                _overlay_end_window                 (void);
    //
    void                                _overlay_draw_context_menu          (void);
    void                                _overlay_update_position            (void);
    void                                overlay_log                         (std::string msg, float secs = 2.0f);
    //
    void                                _overlay_draw_content               ([[maybe_unused]]const Interaction &);
    void                                _overlay_display_main_content       ([[maybe_unused]]const Interaction &);
    void                                _overlay_display_extra_content      ([[maybe_unused]]const Interaction &);
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      RENDERING FUNCTIONS.            |   "render.cpp" ...
    // *************************************************************************** //
    //                              GRID:
    void                                _grid_handle_shortcuts              (void);
    //
    //                              PRIMARY RENDERING:
    void                                _render_paths                       (ImDrawList * dl) const;
    void                                _render_lines                       (ImDrawList *, const ImVec2 & ) const;
    void                                _render_points                      (ImDrawList *) const;
    //
    //                              ADDITIONAL RENDERING:
    void                                _render_selection_highlight         (ImDrawList *) const;
    inline void                         _render_selected_handles            (ImDrawList *) const;   //  Helper for "_render_selection_highlight"
    inline void                         _render_selection_bbox              (ImDrawList *) const;   //  Helper for "_render_selection_highlight"
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      SELECTION MECHANICS.            |   "selection.cpp" ...
    // *************************************************************************** //
    int                                 _hit_point                          ([[maybe_unused]] const Interaction & ) const;
    std::optional<Hit>                  _hit_any                            ([[maybe_unused]] const Interaction & ) const;
    std::optional<PathHit>              _hit_path_segment                   ([[maybe_unused]] const Interaction & ) const;
    //
    //                              PRIMARY SELECTION OPERATION:
    void                                _process_selection                  (const Interaction & );
    //
    void                                resolve_pending_selection           (const Interaction & it);
    void                                update_move_drag_state              (const Interaction & it);
    void                                start_move_drag                     (const ImVec2 & anchor_ws);
    void                                add_hit_to_selection                (const Hit & hit);
    void                                _rebuild_vertex_selection           (void);   // decl
    //
    //                              SELECTION HIGHLIGHT / USER-INTERACTION / APPEARANCE:
    void                                _update_cursor_select               (const Interaction & ) const;
    //void                                _render_selection_highlight         (ImDrawList *) const;
    bool                                _selection_bounds                   (ImVec2 & tl, ImVec2 & br) const;
    //
    //                              LASSO TOOL MECHANICS:
    void                                _start_lasso_tool                   (void);
    void                                _update_lasso                       (const Interaction & );
    //
    //                              BOUNDING BOX MECHANICS:
    void                                _start_bbox_drag                    (uint8_t handle_idx, const ImVec2 & tl, const ImVec2 & br);
    void                                _update_bbox                        (void);
    //
    //                              SELECTION BEHAVIOR STUFF:
    void                                _selection_handle_shortcuts         ([[maybe_unused]] const Interaction & );
    //
    //                              SELECTION CONTEXT MENU STUFF:
    void                                dispatch_selection_context_menus    ([[maybe_unused]] const Interaction & it);
    inline void                         _show_selection_context_menu        ([[maybe_unused]] const Interaction & it, const char * );
    inline void                         _show_canvas_context_menu           ([[maybe_unused]] const Interaction & it, const char * );
    //
    inline void                         _selection_context_primative        ([[maybe_unused]] const Interaction & );
    inline void                         _selection_context_single           ([[maybe_unused]] const Interaction & );
    inline void                         _selection_context_multi            ([[maybe_unused]] const Interaction & );
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      UTILITIES.                      |   "utility.cpp" ...
    // *************************************************************************** //
    Vertex *                            find_vertex                         (std::vector<Vertex> & , VertexID);
    const Vertex *                      find_vertex                         (const std::vector<Vertex> & , VertexID) const;
    std::optional<EndpointInfo>         _endpoint_if_open                   (PathID vid) const;
    //
    //                              DATA MODIFIER UTILITIES:
    void                                _add_point_glyph                    (VertexID vid);
    VertexID                            _add_vertex                         (ImVec2 w);
    void                                _add_point                          (ImVec2 w);
    void                                _erase_vertex_and_fix_paths         (VertexID vid);
    void                                _erase_path_and_orphans             (VertexID vid);
    //
    //                              APP UTILITY OPERATIONS:
    bool                                _try_begin_handle_drag              (const Interaction & );
    void                                _scissor_cut                        (const PathHit & );
    //
    //                              LOCAMOTION UTILITIES:
    void                                _update_world_extent                (void);
    //
    //                              MISC. UTILITIES:
    void                                _draw_controls                      (void);
    void                                _draw_editor_settings               ([[maybe_unused]] popup::Context & ctx);
    //
    //                              HELPER FUNCTIONS FOR EDITOR SETTINGS:
    void                                _draw_settings_mechanics            (void);
    void                                _draw_settings_user_preferences     (void);
    void                                _draw_settings_serialize            (void);
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      COMMON.                         |   "common.cpp" ...
    // *************************************************************************** //
    //                              NEW SELECTION FUNCTIONS:
    void                                bring_selection_to_front            (void);
    void                                bring_selection_forward             (void);
    void                                send_selection_backward             (void);
    void                                send_selection_to_back              (void);
    //
    //                              SELECTION FUNCTIONS:
    void                                move_selection                      (const float dx, const float dy);
    void                                copy_to_clipboard                   (void);
    void                                paste_from_clipboard                (ImVec2);
    void                                delete_selection                    (void);
    //
    //                              GLOBAL OPERATIONS:
    void                                _clear_all                          (void);
    //
    //                              SERIALIZATION:
    inline EditorSnapshot               make_snapshot                       (void) const;
    void                                load_from_snapshot                  (EditorSnapshot && );
    void                                pump_main_tasks                     (void);
    void                                save_worker                         (EditorSnapshot snap, std::filesystem::path path);
    void                                load_worker                         (std::filesystem::path );
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END PROTECTED MEMBER FUNCTIONS.


   
// *************************************************************************** //
//
//
//  2.C                 INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      INLINE GRID/WORLD FUNCTIONS...
    // *************************************************************************** //
    
    //  "world_to_pixels"
    //      ImPlot works in double precision; promote, convert back to float ImVec2
    inline ImVec2                       world_to_pixels                     (ImVec2 w) const {
        ImPlotPoint p = ImPlot::PlotToPixels(ImPlotPoint(w.x, w.y));
        return { static_cast<float>(p.x), static_cast<float>(p.y) };
    }
    
    //  "pixels_to_world"
    inline ImVec2                       pixels_to_world                     (ImVec2 scr) const {
        ImPlotPoint p = ImPlot::PixelsToPlot(scr);// ImPlot uses double; convert back to float for our structs
        return { static_cast<float>(p.x), static_cast<float>(p.y) };
    }
    
    //  "snap_to_grid"
    inline ImVec2                       snap_to_grid                        (ImVec2 ws) const
    {
        if ( this->want_snap() ) {
            float s = m_grid.snap_step;
            if (s <= 0.0f) return ws;                     // safety
            const float inv = 1.0f / s;
            ws.x = std::round(ws.x * inv) * s;
            ws.y = std::round(ws.y * inv) * s;
        }
        return ws;
    }
    
    //  "want_snap"
    inline bool                         want_snap                           (void) const
    { return m_grid.snap_on || ImGui::GetIO().KeyShift; }
       
       
    //  "_update_grid"
    inline void                         _update_grid_info                   (void)
    {
        ImPlotRect      lim         = ImPlot::GetPlotLimits();          // world extent
        ImVec2          size        = ImPlot::GetPlotSize();           // in pixels

        float           range_x     = static_cast<float>(lim.X.Max - lim.X.Min);
        float           ppw         = size.x / range_x;                  // pixels‑per‑world‑unit
    
        float           raw_step    = m_style.TARGET_PX / ppw;                  // world units per 20 px

        //  Quantize to     1·10^n, 2·10^n, or 5·10^n
        float           exp10       = std::pow(10.0f, std::floor(std::log10(raw_step)));
        float           mant        = raw_step / exp10;
        if      ( mant < 1.5f )     { mant = 1.0f;    }
        else if ( mant < 3.5f )     { mant = 2.0f;    }
        else if ( mant < 7.5f )     { mant = 5.0f;    }
        else                        { mant = 1.0f; exp10 *= 10.0f; }

        m_grid.snap_step = mant * exp10;                        // store for the frame
        return;
    }
       
    //  "_clamp_plot_axes"
    inline void                         _clamp_plot_axes                    (void) const
    {
        return;
    }
    
    // *************************************************************************** //
    
    
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      INLINE BROWSER FUNCTIONS...
    // *************************************************************************** //
    
    //  "_reorder_paths"
    inline void                         _reorder_paths                      (int src, int dst)
    {
        BrowserState &      BS          = m_browser_S;
        const bool          invalid     = ( (src == dst) || (src < 0) || (dst < 0) || ( src >= static_cast<int>(m_paths.size()) ) || ( dst >= static_cast<int>(m_paths.size()) ) );
        
        if ( invalid )      { return; }



        /*──────────────── reorder the vector ────────────────*/
        Path temp      = std::move(m_paths[src]);
        m_paths.erase(m_paths.begin() + src);
        m_paths.insert(m_paths.begin() + (src < dst ? dst - 1 : dst),
                       std::move(temp));

        /*──────────────── rebuild z-index so
            top row (index 0) = highest Z
          ────────────────────────────────────────────────────*/
        const int n = static_cast<int>(m_paths.size());
        for (int i = 0; i < n; ++i)
            m_paths[i].z_index = Z_FLOOR_USER + (n - 1 - i);

        /*──────────────── adjust Editor-wide indices ────────*/
        auto remap = [&](int & idx) {
            if (idx < 0) return;
            if (idx == src)                idx = dst;
            else if (src < dst && idx > src && idx <= dst) idx--;
            else if (dst < src && idx >= dst && idx <  src) idx++;
        };
        
        remap(BS.m_browser_anchor);
        remap(BS.m_layer_browser_anchor);
        remap(BS.m_obj_rename_idx);
        remap(BS.m_layer_rename_idx);

        /*──────────────── remap path-selection set ──────────*/
        std::unordered_set<size_t> new_sel;
        for (size_t old_idx : m_sel.paths) {
            int idx = static_cast<int>(old_idx);
            remap(idx);
            new_sel.insert(static_cast<size_t>(idx));
        }
        m_sel.paths.swap(new_sel);
    }
    
    // *************************************************************************** //
    
    
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      NEW BOOK-KEEPING FUNCTIONS...
    // *************************************************************************** //
    
    //  "next_z_index"
    [[nodiscard]] inline ZID            next_z_index                        (void)
    {
        ZID max_z = Z_FLOOR_USER;       // find current maximum among user objects
        for (const Path& p : m_paths)
            if (p.z_index > max_z) max_z = p.z_index;

        if (max_z >= Z_CEIL_USER - 1)   // bump; renormalise if we’re at the ceiling
            renormalise_z();

        return std::min(max_z + 1, Z_CEIL_USER - 1);
    }

    //  "renormalise_z"
    void                                renormalise_z                       (void)
    {
        std::vector<Path*> items;// collect visible+hidden paths in stable draw order
        items.reserve(m_paths.size());
        for (Path& p : m_paths) items.push_back(&p);

        std::stable_sort(items.begin(), items.end(),
            [](const Path* a, const Path* b){ return a->z_index < b->z_index; });

        ZID     z   = Z_FLOOR_USER;
        for (Path* p : items) p->z_index = z++;
        return;
    }
    
    //  "parent_path_of_vertex"
    [[nodiscard]] inline const Path *   parent_path_of_vertex               (VertexID vid) const noexcept {
        for (const Path & p : m_paths) {
            for (VertexID v : p.verts)      { if (v == vid) return &p; }
        }
        return nullptr;                     // not found
    }
    
    //  "parent_path_of_vertex_mut"
    //      Mutable variant – returns nullptr if not found
    [[nodiscard]] inline Path *         parent_path_of_vertex_mut           (VertexID vid) {
        for (Path& p : m_paths) {
            for (VertexID v : p.verts) {
                if (v == vid)   { return &p; }
            }
        }
        return nullptr;
    }
    
    //  "_erase_vertex_record_only"
    inline void                         _erase_vertex_record_only           (VertexID vid) {
        m_vertices.erase(std::remove_if(m_vertices.begin(), m_vertices.end(), [vid](const Vertex& v){ return v.id == vid; }), m_vertices.end());
    }
    
    //  "_prune_selection_mutability"
    inline void                         _prune_selection_mutability         (void)
    {
        //  1.  PATHS...
        for ( auto it = m_sel.paths.begin(); it != m_sel.paths.end(); )
        {
            PathID  pid     = static_cast<PathID>(*it);
            if ( pid >= m_paths.size() || !m_paths[pid].is_mutable() )      { it = m_sel.paths.erase(it); }
            else                                                            { ++it; }
        }

        //  2.  POINTS & VERTICES...
        for ( auto it = m_sel.points.begin(); it != m_sel.points.end(); )
        {
            PointID         idx     = static_cast<PointID>(*it);
            if ( idx >= m_points.size() )                                   { it = m_sel.points.erase(it); continue; }

            const Path *    pp      = parent_path_of_vertex(m_points[idx].v);
            if ( !pp || !pp->is_mutable() )                                 { it = m_sel.points.erase(it); }
            else                                                            { ++it; }
        }
        for ( auto it = m_sel.vertices.begin(); it != m_sel.vertices.end(); )
        {
            VertexID        vid     = static_cast<VertexID>(*it);
            const Path *    pp      = parent_path_of_vertex(vid);
            if ( !pp || !pp->is_mutable() )                                 { it = m_sel.vertices.erase(it); }
            else                                                            { ++it; }
        }

        //  3.  STAND-ALONE LINES (Deprecated)...
        for ( auto it = m_sel.lines.begin(); it != m_sel.lines.end(); )
        {
            LineID lid = static_cast<LineID>(*it);
            if ( lid >= m_lines.size() || !m_lines[lid].is_mutable() )      { it = m_sel.lines.erase(it); }
            else                                                            { ++it; }
        }

        _rebuild_vertex_selection();     // sync vertices ↔ points
        return;
    }
    
    //  "_new_path_from_prototype"
    [[nodiscard]] inline Path           _new_path_from_prototype            (const Path & proto) {
        Path    p   = proto;                    //  Copy style, verts cleared by caller
        p.set_default_label(m_next_pid++);      //  "Path N"
        return p;
    }
    
    //  "_clone_path_proto"
    [[nodiscard]] inline Path           _clone_path_proto                   (const Path& src) {
        Path p = src;                     // copy style, z_index, locked, visible
        p.id = m_next_pid++;              // fresh PathID
        p.set_default_label(p.id);        // "Path %u"
        p.verts.clear();                  // caller will fill verts
        return p;
    }
    
    //  "_recompute_next_ids"
    inline void                         _recompute_next_ids                 (void) {
        m_next_id   = 1;
        m_next_pid  = 1;
        for (const Vertex& v : m_vertices)  { if (v.id >= m_next_id)    { m_next_id  = v.id + 1; }      }
        for (const Path& p : m_paths)       { if (p.id >= m_next_pid)   { m_next_pid = p.id + 1; }      }
    }
    
    // *************************************************************************** //
    
    
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      STANDARDIZED MECHANICS FOR TOOLS...
    // *************************************************************************** //
    
    //  "reset_pen"
    inline void                         reset_pen                           (void) {
        this->m_show_handles.erase( m_pen.handle_vid );
        this->m_sel.vertices.erase( m_pen.last_vid );
        m_pen = {};
        return;
    }
    
    //  "reset_selection"
    inline void                         reset_selection                     (void) {
        this->m_sel.clear();                this->m_show_handles.clear();
        this->m_show_sel_overlay = false;   this->m_browser_S.reset();
        return;
    }
    
    //  "RESET_ALL"
    inline void                         RESET_ALL                           (void) {
        this->reset_pen();
        this->reset_selection();
        this->m_clipboard.clear();
        this->m_shape = {};
        
        this->_clear_all();
    
        return;
    }
    
    //  "_make_path"
    //      Always call this to materialise a new Path.
    //      • Assigns fresh PathID & label
    //      • Puts it on the top Z-layer (unless caller overrides)
    //      • Copies style / flags from an optional prototype
    //
    inline Path &                       _make_path                          (const std::vector<VertexID> & verts, const Path * proto = nullptr) {
        Path        p       = proto ? *proto : Path{};      // copy style/flags if given
        p.id                = m_next_pid++;
        p.set_default_label(p.id);
        p.closed            = false;                        // caller may flip later
        p.z_index           = next_z_index();

        p.verts             = verts;                        // freshly-built vertex list
        m_paths.push_back(std::move(p));
        return m_paths.back();                              // reference for caller tweaks
    }
    
    //  "_make_shape"
    inline Path &                       _make_shape                         (const std::vector<VertexID> & verts, const Path * proto = nullptr) {
        Path &  p               = _make_path(verts, proto);   // assigns id, label, z_index, flags
        p.closed                = true;
        p.style.fill_color      = Path::ms_DEF_PATH_FILL_COLOR;
        return p;
    }
    
    //  "_clone_path"
    //      Duplicate an existing path with a vid-remap (copy/paste, boolean ops …).
    //
    inline Path &                       _clone_path                         (const Path& src, const std::vector<VertexID> & vid_map) {
        Path dup = src;                // copy style / flags
        dup.id   = m_next_pid++;
        dup.set_default_label(dup.id);
        dup.z_index = next_z_index();

        dup.verts.clear();
        for (VertexID old : src.verts)
            dup.verts.push_back(vid_map[old]); // map to duplicated verts

        m_paths.push_back(std::move(dup));
        return m_paths.back();
    }
    
    // *************************************************************************** //
    
    
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      INLINE MISC. FUNCTIONS...
    // *************************************************************************** //

    //  "left_label"
    inline void                         left_label                          (const char * label, const float label_w, const float widget_w) const
    { utl::LeftLabel(label, label_w, widget_w); ImGui::SameLine(); return; };
    
    //  "label"
    inline void                         label                               (const char * text, const float l_width=ms_LABEL_WIDTH, const float w_width=ms_WIDGET_WIDTH)
    { utl::LeftLabel(text, l_width, w_width); ImGui::SameLine(); };
    
    inline bool                         has_file                        (void) const    { return ( std::filesystem::exists( m_filepath ) ); }
    
    
    
    
    //  "maybe_snap"
    inline ImVec2                       maybe_snap                          (ImVec2 w) const
    { return m_grid.snap_on ? snap_to_grid(w) : w; }
    
    //  "path_is_mutable"
    inline bool                         path_is_mutable                     (const Path * p) const noexcept
    { return p && p->visible && !p->locked; }

    //  "_mode_has"
    inline bool                         _mode_has                           (CBCapabilityFlags flag) const
    { return (MODE_CAPS[static_cast<size_t>(m_mode)] & flag) != 0; }

    //  "_toggle_resident_overlay"
    inline void                         _toggle_resident_overlay            (const Resident idx) {
        auto & entry = m_residents[idx];    Overlay * ov = entry.ptr;
        ov->visible = !ov->visible;         return;
    }

    //  "_set_resident_visibility"
    inline void                         _set_resident_visibility            (const Resident idx, const bool vis) {
        auto & entry = m_residents[idx];    Overlay * ov = entry.ptr;
        ov->visible = vis;                  return;
    }
    
    
    
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
    app::AppState &                     CBAPP_STATE_NAME;
    EditorStyle                         m_style                         {  };
    //
    std::vector<Vertex>                 m_vertices;
    std::vector<Point>                  m_points;
    std::vector<Line>                   m_lines;
    std::vector<Path>                   m_paths;                //  New path container
    std::unordered_set<HandleID>        m_show_handles;         //  List of which glyphs we WANT to display Bezier points for.
    //
    OverlayManager                      m_overlays;
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      SERIALIZATION STUFF...
    // *************************************************************************** //
    std::mutex                          m_task_mtx;
    std::vector<std::function<void()>>  m_main_tasks;
    std::atomic<bool>                   m_io_busy                       {false};
    IoResult                            m_io_last                       {IoResult::Ok};
    std::string                         m_io_msg                        {  };
    //
    //
    std::atomic<bool>                   m_sdialog_open                  = {false};
    std::atomic<bool>                   m_odialog_open                  = {false};
    //
    cb::FileDialog::Initializer         m_SAVE_DIALOG_DATA              = {
        /* type               = */  cb::FileDialogType::Save,
        /* window_name        = */  "Save Editor Session",
        /* default_filename   = */  "canvas_settings.json",
        /* required_extension = */  "json",
        /* valid_extensions   = */  {".json", ".txt"},
        /* starting_dir       = */  std::filesystem::current_path()
    };
    cb::FileDialog::Initializer         m_OPEN_DIALOG_DATA              = {
        /* type               = */  cb::FileDialogType::Open,
        /* window_name        = */  "Save Editor Session",
        /* default_filename   = */  "",
        /* required_extension = */  "",
        /* valid_extensions   = */  {".json", ".cbjson", ".txt"},
        /* starting_dir       = */  std::filesystem::current_path()
    };
    cb::FileDialog                      m_save_dialog;
    cb::FileDialog                      m_open_dialog;
    std::filesystem::path               m_filepath                      = {"../../assets/.cbapp/debug/fdtd_testing_v0.json"};
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      BROWSER STUFF...
    // *************************************************************************** //
    std::string                         WinInfo_uuid                    = "Editor Browser";
    ImGuiWindowFlags                    WinInfo_flags                   = ImGuiWindowFlags_None | ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY;
    bool                                WinInfo_open                    = true;
    ImGuiWindowClass                    m_window_class;
    //
    //  BrowserState                        m_browser_state                 {  };
    //  ImGuiTextFilter                     m_browser_filter;                           // search box text filter
    //  int                                 m_browser_anchor                = -1;       // anchor index for Shift‑range select
    //  int                                 m_inspector_vertex_idx          = -1;       // anchor index for Shift‑range select
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      APPLICATION STATE...
    // *************************************************************************** //
    //                              OVERALL STATE:
    //  EditorState
    Mode                                m_mode                          = Mode::Default;
    bool                                m_show_grid                     = true;
    //
    //                              MISC. STATE:
    bool                                m_dragging                      = false;
    bool                                m_lasso_active                  = false;
    bool                                m_pending_clear                 = false;    //  pending click selection state ---
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
    //      OBJECTS...
    // *************************************************************************** //
    //                              TOOL STATES:
    PenState                            m_pen;
    ShapeState                          m_shape;
    OverlayState                        m_overlay;
    BrowserState                        m_browser_S;        //  <======|    NEW CONVENTION.  Let's use "m_name_S" to denote a STATE variable...
    //
    //                              OTHER FACILITIES:
    Selection                           m_sel;
    mutable BoxDrag                     m_boxdrag;
    MoveDrag                            m_movedrag;
    Clipboard                           m_clipboard;
    //
    //
    //                              NEED TO RE-HOME:
    std::optional<Hit>                  m_pending_hit;   // candidate under mouse when button pressed   | //  pending click selection state ---
    //
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      CAMERA SYSTEM...
    // *************************************************************************** //
    ImPlotFlags                         m_plot_flags                    = ImPlotFlags_Equal | ImPlotFlags_NoFrame | ImPlotFlags_NoBoxSelect | ImPlotFlags_NoMenus | ImPlotFlags_NoLegend | ImPlotFlags_NoTitle;
    utl::AxisCFG                        m_axes [2]                      = {
        {"##x-axis",    ImPlotAxisFlags_None | ImPlotAxisFlags_NoSideSwitch | ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_NoInitialFit | ImPlotAxisFlags_Opposite },
        {"##y-axis",    ImPlotAxisFlags_None | ImPlotAxisFlags_NoSideSwitch | ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_NoInitialFit }
    };
    utl::LegendCFG                      m_legend                        = { ImPlotLocation_NorthWest, ImPlotLegendFlags_None };
    //
    //
    //
    Camera                              m_cam;
    GridSettings                        m_grid                          = { 100.0f,  true,  false };
    float                               m_ppw                           = 1.0f;
    Bounds                              m_world_bounds                  = {
        /*min_x=*/0.0f,        /*min_y=*/0.0f,
        /*max_x=*/500.0f,      /*max_y=*/500.0f
    };
    //
    //
    static constexpr float              ms_GRID_STEP_MIN                = 2.0f;   // world-units
    static constexpr float              ms_GRID_LABEL_PAD               = 2.0f;
    //
    //                              FUNCTIONS:
    //
    //
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      VARIABLES FOR SPECIFIC MECHANICS...
    // *************************************************************************** //
    //                              LASSO TOOL / SELECTION:
    bool                                m_show_debug_overlay            = true;
    bool                                m_show_sel_overlay              = false;
    ImVec2                              m_lasso_start                   = ImVec2(0.f, 0.f);
    ImVec2                              m_lasso_end                     = ImVec2(0.f, 0.f);
    VertexID                            m_next_id                       = 1;
    PathID                              m_next_pid                      = 1;        // counter for new path IDs
    //
    //                              BBOX SCALING:
    mutable int                         m_hover_handle                  = -1;
    mutable ImVec2                      m_origin_scr                    = {0.f, 0.f};   // screen-space canvas origin                       //  -1 = none, 0-7 otherwise (corners+edges)
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
    //      STATIC / CONSTANTS...
    // *************************************************************************** //
    //                      ARRAYS:
    static constexpr auto &             ms_EDITOR_STATE_NAMES           = DEF_EDITOR_STATE_NAMES;
    static constexpr auto &             ms_SHAPE_NAMES                  = DEF_EDITOR_SHAPE_NAMES;
    static constexpr auto &             ms_ANCHOR_TYPE_NAMES            = DEF_ANCHOR_TYPE_NAMES;
    static constexpr auto &             ms_PATH_KIND_NAMES              = path::DEF_PATH_KIND_NAMES;
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END PROTECTED DATA-MEMBERS...



// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
};//    END "EDITOR" CLASS DEFINITION.






// *************************************************************************** //
//
//
//      SERIALIZATION...
// *************************************************************************** //
// *************************************************************************** //

inline static constexpr uint32_t        kSaveFormatVersion              = 4;

// Extend similarly for Point, Line, GridSettings, ViewState …
//
struct EditorSnapshot {
    // *************************************************************************** //
    //
    //
    //      1.          NESTED PUBLIC TYPENAME ALIASES...
    // *************************************************************************** //
    // *************************************************************************** //
        using                           Font                            = Editor::Font                      ;
        using                           Logger                          = Editor::Logger                    ;
        using                           LogLevel                        = Editor::LogLevel                  ;
        using                           CBCapabilityFlags               = Editor::CBCapabilityFlags         ;
        using                           Anchor                          = Editor::Anchor                    ;
    //
    //                              ID / INDEX TYPES:
    //  using                           ID                              = Editor::ID                        ;
        using                           VertexID                        = Editor::VertexID                  ;
        using                           HandleID                        = Editor::HandleID                  ;
        using                           PointID                         = Editor::PointID                   ;
        using                           LineID                          = Editor::LineID                    ;
        using                           PathID                          = Editor::PathID                    ;
        using                           ZID                             = Editor::ZID                       ;
        using                           OverlayID                       = Editor::OverlayID                 ;
        using                           HitID                           = Editor::HitID                     ;
    //
    //                              TYPENAME ALIASES (BASED ON INDEX TYPES):
        using                           Vertex                          = Editor::Vertex                    ;
    //  using                           Handle                          = Editor::Handle                    ;
        using                           Point                           = Editor::Point                     ;
        using                           Line                            = Editor::Line                      ;
        using                           Path                            = Editor::Path                      ;
        using                           Overlay                         = Editor::Overlay                   ;
        using                           Hit                             = Editor::Hit                       ;
        using                           PathHit                         = Editor::PathHit                   ;
        using                           Selection                       = Editor::Selection                 ;
    //
        using                           EndpointInfo                    = Editor::EndpointInfo              ;
        using                           PenState                        = Editor::PenState                  ;
        using                           ShapeState                      = Editor::ShapeState                ;
        using                           Clipboard                       = Editor::Clipboard                 ;
//
// *************************************************************************** //
// *************************************************************************** //
public:
    // *************************************************************************** //
    //
    //
    //      2.          MEMBER FUNCTIONS...
    // *************************************************************************** //
    // *************************************************************************** //
    
    // *************************************************************************** //
    //                                  RULE OF 7...
    // *************************************************************************** //
    //
    //                              DEFAULTED SPECIAL MEMBERS:
    inline                              EditorSnapshot          (const Editor & src);
    
    inline                              EditorSnapshot          (void)                              = default;
    inline                              EditorSnapshot          (const EditorSnapshot & )           = default;
    inline                              EditorSnapshot          (EditorSnapshot && ) noexcept       = default;
    //
    //                              DEFAULTED SPECIAL MEMBERS:
    EditorSnapshot &                    operator =              (const EditorSnapshot&)             = default;
    EditorSnapshot &                    operator =              (EditorSnapshot&&) noexcept         = default;
    
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //                                  CENTRALIZED FUNCTIONS...
    // *************************************************************************** //
        
    //  "copy_from"
    //      centralised copy function.
    //
    inline void                         copy_from               (const Editor & e)
    {
        //  1.  CORE DATA...
        this->vertices                  = e.m_vertices              ;
        this->paths                     = e.m_paths                 ;
        this->points                    = e.m_points                ;
        this->lines                     = e.m_lines                 ;
        //
        //  2.  SUB-STATE OBJECTS...
        this->selection                 = e.m_sel                   ;
        //
        //  3.  ADDITIONAL MEMBERS...
        this->next_vid                  = e.m_next_id               ;       // << new
        this->next_pid                  = e.m_next_pid              ;       // << new
    
        return;
    }
    
    
    //  "assign_to"
    //      centralised "apply" function.
    //
    inline void                         assign_to               ([[maybe_unused]] const Editor & e)
    {
        //  1.  CORE DATA...
        //      e.m_vertices                    = std::vector<Vertex>   ( this->vertices  )     ;
        //      e.m_paths                       = std::vector<Point>    ( this->paths     )     ;
        //      e.m_points                      = std::vector<Line>     ( this->points    )     ;
        //      e.m_lines                       = std::vector<Path>     ( this->lines     )     ;
        //
        //
        //  2.  SUB-STATE OBJECTS...
        //      e.m_sel                         = Selection(this->selection)    ;
        //
        //
        //  3.  ADDITIONAL MEMBERS...
        //  this->next_vid                  = m_next_id;    // << new
        //  this->next_pid                  = m_next_pid;   // << new
    
        return;
    }
    
    
    // *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
public:
    // *************************************************************************** //
    //
    //
    //      3.          DATA MEMBERS...
    // *************************************************************************** //
    // *************************************************************************** //
    
    // *************************************************************************** //
    //                                  MISC DATA...
    // *************************************************************************** //
    static constexpr uint32_t           version                 { kSaveFormatVersion };
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      CORE DATA...
    // *************************************************************************** //
    std::vector<Vertex>                 vertices;
    std::vector<Path>                   paths;
    std::vector<Point>                  points;
    std::vector<Line>                   lines;
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //                                  APPLICATION SUB-STATES...
    // *************************************************************************** //
    Selection                           selection;
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //                                  NEW STUFF...
    // *************************************************************************** //
    VertexID                            next_vid                        {0};            // first free vertex ID
    PathID                              next_pid                        {0};            // first free path  ID
    // *************************************************************************** //



// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
};//    END "EditorSnapshot" Struct Definition.

//  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(EditorSnapshot,
//                                     version,
//                                     next_vid,
//                                     next_pid,
//                                     vertices,
//                                     paths,
//                                     points,
//                                     lines,
//                                     selection)




//  "to_json"
//
inline void to_json(nlohmann::json & j, const EditorSnapshot & s)
{
    j   = nlohmann::json{
        { "vertices",               s.vertices              },
        { "paths",                  s.paths                 },
        { "points",                 s.points                },
        { "lines",                  s.lines                 },
        { "selection",              s.selection             }
        //
        //  add grid / view / mode when you serialize them
        //
    };
    
    return;
}


//  "from_json"
//
inline void from_json(const nlohmann::json & j, EditorSnapshot & s)
{
    j.at(   "vertices"              )       .get_to (   s.vertices             );
    j.at(   "paths"                 )       .get_to (   s.paths                );
    j.at(   "points"                )       .get_to (   s.points               );
    j.at(   "lines"                 )       .get_to (   s.lines                );
    j.at(   "selection"             )       .get_to (   s.selection            );
    
    return;
}

        

// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** // END "EditorSnapshot"












//  MORE STATIC HELPERS...
// *************************************************************************** //
// *************************************************************************** //

//  "mode_label"
//
// static inline const char * mode_label(Mode m)
// { return Editor::ms_EDITOR_STATE_NAMES[ static_cast<size_t>(m) ]; }






//  NEW STATIC HELPERS...
// *************************************************************************** //
// *************************************************************************** //




//  0.  STATIC INLINE       | DRAWING / CURVATURE / RENDERING...
// *************************************************************************** //
// *************************************************************************** //

//  "is_curved"
//
template< std::integral ID, typename Vertex = Vertex_t<ID> >
[[maybe_unused]] inline bool is_curved(const Vertex* a, const Vertex* b) noexcept
{
    return (a->out_handle.x || a->out_handle.y ||
            b->in_handle.x  || b->in_handle.y);
}


//  "cubic_eval"
//
template< std::integral ID, typename Vertex = Vertex_t<ID> >
[[maybe_unused]] static inline ImVec2 cubic_eval(const Vertex * a, const Vertex* b, float t)
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


//  "point_in_polygon"
//
[[maybe_unused]]
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


//  "is_convex_poly"
//
[[maybe_unused]] static bool is_convex_poly(const std::vector<ImVec2>& poly)
{
    const size_t n = poly.size();
    if (n < 4) return true;
    float sign = 0.f;
    for (size_t i = 0; i < n; ++i) {
        const ImVec2& a = poly[i];
        const ImVec2& b = poly[(i + 1) % n];
        const ImVec2& c = poly[(i + 2) % n];
        float cross = (b.x - a.x) * (c.y - a.y) -
                      (b.y - a.y) * (c.x - a.x);
        if (cross != 0.f) {
            if (sign == 0.f) sign = cross;
            else if ((sign > 0.f) != (cross > 0.f)) return false;
        }
    }
    return true;
}
    
    




// *************************************************************************** //
//
//
//
//  1.  STATIC INLINE       | VECTOR OPERATIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "vec_len"
//      Returns length of a 2-D vector.
//
static inline float vec_len(const ImVec2& v) { return sqrtf(v.x*v.x + v.y*v.y); }


//  "vec_norm"
//      Normalises `v` unless zero; returns {0,0} if zero.
//
static inline ImVec2 vec_norm(const ImVec2& v)
{
    float l = vec_len(v);
    return (l > 0.f) ? ImVec2{ v.x / l, v.y / l } : ImVec2{0,0};
}






// *************************************************************************** //
//
//
//
//  2.  STATIC INLINE       | MISC / OTHER...
// *************************************************************************** //
// *************************************************************************** //

//  "u32_to_f4"
//
[[maybe_unused]] static inline ImVec4 u32_to_f4(ImU32 c)        { return ImGui::ColorConvertU32ToFloat4(c); }


//  "f4_to_u32"
//
[[maybe_unused]] static inline ImU32  f4_to_u32(ImVec4 f)       { return ImGui::ColorConvertFloat4ToU32(f); }


//  "mirror_handles"
//      Mirrors the opposite handle according to anchor kind.
//
template< typename ID, typename Vertex = Vertex_t<ID> >
inline void mirror_handles(Vertex & v, bool dragged_out_handle) noexcept
{
    ImVec2 &    h_dragged   = dragged_out_handle ? v.out_handle : v.in_handle;
    ImVec2 &    h_other     = dragged_out_handle ? v.in_handle  : v.out_handle;

    switch (v.kind)
    {
        case AnchorType::Corner: {
            /* no coupling */ break;
        }

        case AnchorType::Smooth: {
            const float  len    = vec_len(h_other);          // preserve length
            const ImVec2 dir    = vec_norm(h_dragged);
            h_other             = ImVec2{ -dir.x * len, -dir.y * len };
            break;
        }

        case AnchorType::Symmetric:     {
            h_other = ImVec2{ -h_dragged.x, -h_dragged.y };
            break;
        }
        
        default :                       { IM_ASSERT( false && "Function __func__ called with unknown AnchorType" ); }   //  Quiet the warning.
    }
}


//  "find_vertex_mut"
//
template< typename ID, typename Vertex = Vertex_t<ID> >
inline Vertex * find_vertex_mut( std::vector< Vertex > & arr, ID id ) noexcept {
    for (Vertex & v : arr)
        if (v.id == id) return &v;
        
    return nullptr;
}


//  "alt_down"
//
static inline bool alt_down(void)
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

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
//      CBAPP_APPSTATE_ALIAS_API        //  CLASS-DEFINED, NESTED TYPENAME ALIASES.
//
//
// *************************************************************************** //
// *************************************************************************** //
    
    static constexpr std::array<const char*, static_cast<size_t>(Mode::Count)>
                                ms_MODE_LABELS                  = { "Default", "Line", "Point", "Pen" };
    static constexpr float      ms_LIST_COLUMN_WIDTH            = 240.0f;   // px width of point‑list column



public:
    //  1.              INITIALIZATION METHODS...
    // *************************************************************************** //
                                Editor                          (void);
                                ~Editor                         (void);


    //  2.              PUBLIC MEMBER FUNCTIONS...
    // *************************************************************************** //
    void                        Begin                           (const char* id = "##EditorCanvas");
    void                        DrawPointBrowser                ([[maybe_unused]] const char *, [[maybe_unused]] bool *, [[maybe_unused]] ImGuiWindowFlags);



// *************************************************************************** //
// *************************************************************************** //
private:
    //
    //  2.A             PROTECTED OPERATION MEMBER FUNCTIONS...
    // *************************************************************************** //

    //  mode handlers
    void                        _handle_default                 (const Interaction & );
    void                        _handle_line                    (const Interaction & );
    void                        _handle_point                   (const Interaction & );
    
    //  pen tool stuff...
    void                        _handle_pen                     (const Interaction & );
    //
    // ───────── Pen–tool helpers ───────────────────────────────
    bool                        _pen_cancel_if_escape           (const Interaction & );
    bool                        _pen_try_begin_handle_drag      (const Interaction & );
    void                        _pen_update_handle_drag         (const Interaction & );
    void                        _pen_begin_path_if_click_empty  (const Interaction & );
    void                        _pen_append_or_close_live_path  (const Interaction & );
    //
    static constexpr ImU32      PEN_ANCHOR_COL                  = IM_COL32(255,200,0,255);
    static constexpr float      PEN_ANCHOR_RADIUS               = 5.0f;
    static constexpr int        ms_BEZIER_SEGMENTS              = 0;
    static constexpr int        ms_BEZIER_HIT_STEPS             = 20;
    
    
    
    
    //  browser functions
    void                        _draw_point_list_column         (void);
    void                        _draw_point_inspector_column    (void);
    
    //
    //  2.B             NEW STUFF...
    // *************************************************************************** //
    void                        _draw_paths                     (ImDrawList*, const ImVec2&) const;
    
    

    //  2.C             INLINE FUNCTIONS...
    // *************************************************************************** //
    
    //  capability query
    inline bool                 _mode_has                       (Capability cap) const {
        return MODE_CAPS[static_cast<size_t>(m_mode)] & cap;
    }
    
    Pos *                       find_vertex                     (std::vector<Pos> & , uint32_t);
    const Pos *                 find_vertex                     (const std::vector<Pos> & , uint32_t) const;
    


    //  2.D             PROTECTED UTILITY FUNCTIONS...
    // *************************************************************************** //

    //  helpers
    uint32_t                    _add_vertex                     (ImVec2 w);
    void                        _add_point                      (ImVec2 w);

    int                         _hit_point                      (const Interaction&) const;
    std::optional<Hit>          _hit_any                        (const Interaction&) const;

    void                        _process_selection              (const Interaction&);
    void                        _update_cursor_select           (const Interaction&) const;

    void                        _draw_selection_overlay         (ImDrawList*, const ImVec2&) const;
    void                        _draw_grid                      (ImDrawList*, const ImVec2&, const ImVec2&) const;
    void                        _draw_lines                     (ImDrawList*, const ImVec2&) const;
    void                        _draw_points                    (ImDrawList*,const ImVec2&) const;
    void                        _draw_controls                  (const ImVec2&);
    void                        _clear_all                      (void);
    //
    bool                        _selection_bounds               (ImVec2& tl, ImVec2& br) const;
    void                        _rebuild_vertex_selection       (void);   // decl


    // *************************************************************************** //
    //
    //
    //  3.              PROTECTED DATA-MEMBERS...
    // *************************************************************************** //
    // *************************************************************************** //
    
    //  browser stuff
    std::string                 WinInfo_uuid                    = "Editor Browser";
    ImGuiWindowFlags            WinInfo_flags                   = ImGuiWindowFlags_None | ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY;
    bool                        WinInfo_open                    = true;
    ImGuiWindowClass            m_window_class; 
    //
    ImGuiTextFilter             m_browser_filter;                           // search box text filter
    int                         m_browser_anchor                = -1;       // anchor index for Shift‑range select

    //  data
    std::vector<Pos>            m_vertices;
    std::vector<Point>          m_points;
    std::vector<Line>           m_lines;
    std::vector<Path>           m_paths;                       // new path container
    PenState                    m_pen;


    Selection                   m_sel;
    bool                        m_dragging                      = false;

    bool                        m_lasso_active                  = false;
    ImVec2                      m_lasso_start                   = ImVec2(0.f, 0.f);
    ImVec2                      m_lasso_end                     = ImVec2(0.f, 0.f);

    Mode                        m_mode                          = Mode::Default;
    bool                        m_show_grid                     = true;
    bool                        m_drawing                       = false;

    float                       m_zoom                          = 1.f;
    ImVec2                      m_scroll                        = ImVec2(0.f, 0.f);
    uint32_t                    m_next_id                       = 1;


    // --- pending click selection state ---
    std::optional<Hit>          m_pending_hit;   // candidate under mouse when button pressed
    bool                        m_pending_clear                 = false;
//
//
//
// *************************************************************************** //
// *************************************************************************** //
};//    END "EDITOR" CLASS DEFINITION.









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


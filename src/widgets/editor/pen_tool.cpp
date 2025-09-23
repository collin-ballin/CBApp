/***********************************************************************************
*
*       *********************************************************************
*       ****           P E N _ T O O L . C P P  ____  F I L E            ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      June 14, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "widgets/editor/editor.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //






//  0.      STATIC VARIABLES...
// *************************************************************************** //
// *************************************************************************** //


//  0.      STATIC FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//
//
//  2.      SUBSIDIARY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_pen_begin_handle_drag"
//
void Editor::_pen_begin_handle_drag(VertexID vid, bool out_handle, const bool force_select)
{
    //  existing flag setup …
    m_pen.dragging_handle  = true;
    m_pen.handle_vid       = vid;
    m_pen.dragging_out     = out_handle;

    m_dragging_handle      = true;
    m_drag_vid             = vid;
    m_dragging_out         = out_handle;


    // ── NEW: set vertex kind = Symmetric so mirror_handles links both arms
    if ( Vertex * v = find_vertex_mut(m_vertices, vid) )
        {  v->SetCurvatureType( BezierCurvatureType::Cubic );  }


    // --- NEW: make the vertex temporarily selected --------------------
    if (force_select)
    {
        this->reset_selection();    // m_sel.clear();                               //     optional: keep if you want solo-select
        //  m_show_handles.clear();
        //
        m_sel.vertices.insert(vid);
        m_show_handles.insert(vid);

        for (size_t i = 0; i < m_points.size(); ++i) // select its Point glyph
        {
            if ( m_points[i].v == vid )
            {
                m_sel.points.insert(i);
                break;
            }
        }
    }
    
    return;
}


//  "_pen_update_handle_drag"
//
void Editor::_pen_update_handle_drag(const Interaction & /*it*/)
{
    ImGuiIO &       io              = ImGui::GetIO();
    Vertex *        v_ptr           = find_vertex_mut(m_vertices, m_drag_vid);
    
    
    
    //      CASE 0 :    NULL VERTEX...
    if ( !v_ptr ) {
        m_dragging_handle       = false;
        m_pen.dragging_handle   = false;
        return;
    }

    Vertex &        v               = *v_ptr;
    ImVec2          ws_anchor       { v.x, v.y };
    ImVec2          ws_mouse        = pixels_to_world(io.MousePos);          // NEW
    
    
    m_show_handles.insert( v.id );


    //      Apply grid‑snap if desired
    if ( want_snap() )              { ws_mouse = snap_to_grid(ws_mouse); }


    //      Optional inversion for in‑handle while using Pen tool
    if ( m_mode == Mode::Pen  &&  !m_dragging_out )
    {
        ws_mouse.x  = ws_anchor.x - (ws_mouse.x - ws_anchor.x);
        ws_mouse.y  = ws_anchor.y - (ws_mouse.y - ws_anchor.y);
    }

    ImVec2          offset          { ws_mouse.x - ws_anchor.x,  ws_mouse.y - ws_anchor.y };

    //  if (m_dragging_out)             { v.m_bezier.out_handle     = offset; }
    //  else                            { v.m_bezier.in_handle      = offset; }
    if (m_dragging_out)             { v.SetOutHandle(offset);   }
    else                            { v.SetInHandle(offset);    }


    mirror_handles<VertexID>(v, m_dragging_out);


    if ( !io.MouseDown[ImGuiMouseButton_Left] )
    {
        m_dragging_handle       = false;
        m_pen.dragging_handle   = false;
    }
    
    
    
    return;
}


//  "_pen_begin_path_if_click_empty"
//
void Editor::_pen_begin_path_if_click_empty(const Interaction & /*it*/ )
{
/*
    // 1. Ignore if the click actually hit something
    if (_hit_any(it)) return;

    // 2. Add first vertex at mouse position (world-space)
    ImVec2   ws  = pixels_to_world(ImGui::GetIO().MousePos);
    VertexID vid = _add_vertex(ws);

    // 3. Add glyph for that vertex
    m_points.push_back({ vid,
                         { PEN_ANCHOR_COLOR,
                           PEN_ANCHOR_RADIUS,
                           true } });

    // 4. Create a brand-new Path object
    Path p;
    p.id = m_next_pid++;                         // unique ID
    p.set_default_label(p.id);                   // default label: "Path %02u"

    p.verts.push_back(vid);
    p.closed = false;                            // open path

    // 5. Push to container and update Pen state
    m_paths.push_back(std::move(p));

    m_pen = { true,                    // active
              m_paths.size() - 1,      // index of the new path
              vid,                     // last_vid
              false, 0, false };       // other PenState fields
*/
    return;
}


//  "_pen_click_hits_first_vertex"
//
inline bool Editor::_pen_click_hits_first_vertex(const Interaction& it,
                                                 const Path& p) const
{
    int pi = _hit_point(it);                    // index in m_points, or -1
    if (pi < 0) return false;
    return m_points[pi].v == p.verts.front();   // hit == first anchor
}


//  "_pen_append_or_close_live_path"
//
void Editor::_pen_append_or_close_live_path(const Interaction& it)
{
    Path &          p           = m_paths[m_pen.path_index];


    //  1.  Click on first vertex closes path
    if ( _pen_click_hits_first_vertex(it, p) )
    {
        p.closed = true;
        if ( p.closed && (p.style.fill_color & 0xFF000000) == 0 ) {
            ImVec4 stroke_f = ImGui::ColorConvertU32ToFloat4(p.style.stroke_color);
            stroke_f.w = 0.4f;
            p.style.fill_color = ImGui::ColorConvertFloat4ToU32(stroke_f);
        }
        this->reset_pen();
        //  m_pen       = {};
        //  m_mode      = Mode::Default;
        return;
    }


    //  2.  Fallback: pixel‑distance to first vertex.
    ImVec2          ms          = ImGui::GetIO().MousePos;
    if ( !p.verts.empty() )
    {
        const Vertex *  first = find_vertex(m_vertices, p.verts.front());
        if (first)
        {
            ImVec2  first_px = world_to_pixels({ first->x, first->y });
            float   dx = first_px.x - ms.x, dy = first_px.y - ms.y;
            
            if ( dx*dx + dy*dy <= m_style.HIT_THRESH_SQ ) {
                p.closed = true;
                
                this->reset_pen();
                //  m_mode = Mode::Default;
                //  m_pen       = {};
                return;
            }
        }
    }

    //  3.  Add a new vertex at mouse position.
    ImVec2          ws          = pixels_to_world(ms);           // NEW
    VertexID        new_vid     = _add_vertex(ws);
    _add_point_glyph(new_vid);


    if (m_pen.prepend)      { p.verts.insert(p.verts.begin(), new_vid); }
    else                    { p.verts.push_back(new_vid); }


    //  NEW...
    if (m_pen.handle_vid) {
        m_show_handles.erase(m_pen.handle_vid);   // hide them
        m_pen.handle_vid = 0;                     // no active handle now
    }
    
    m_pen.last_vid = new_vid;
    return;
}







// *************************************************************************** //
//
//
//  PEN TOOL UTILITIES...
// *************************************************************************** //
// *************************************************************************** //

//  "_path_idx_if_last_vertex"
//
std::optional<size_t> Editor::_path_idx_if_last_vertex(VertexID vid) const
{
    for (size_t i = 0; i < m_paths.size(); ++i)
        if (!m_paths[i].closed && !m_paths[i].verts.empty() &&
            m_paths[i].verts.back() == vid)
            return i;
    return std::nullopt;
}


//  "_can_join_selected_path"
//
inline bool Editor::_can_join_selected_path(void) const
{
    return m_sel.paths.size() == 1
        && *m_sel.paths.begin() < m_paths.size()
        && !m_paths[*m_sel.paths.begin()].closed;
}


//  "_join_selected_open_path"
//      Join the single selected path back onto itself (Ctrl+J command)
//
void Editor::_join_selected_open_path(void)
{
    if (!_can_join_selected_path()) return;

    size_t idx = *m_sel.paths.begin();
    Path&  p   = m_paths[idx];

    if (p.verts.size() < 2) return;        // need at least a segment

    p.closed = true;

    // optional: give a faint default fill so the user sees area immediately
    if ((p.style.fill_color & 0xFF000000) == 0) {
        ImVec4 f = ImGui::ColorConvertU32ToFloat4(p.style.stroke_color);
        f.w = 0.25f;                       // 25 % opacity
        p.style.fill_color = ImGui::ColorConvertFloat4ToU32(f);
    }
}


//  "_draw_pen_cursor"
//
void Editor::_draw_pen_cursor(const ImVec2 & p, ImU32 col)
{
    ImGui::SetMouseCursor(ImGuiMouseCursor_None);
    ImDrawList* dl = ImGui::GetForegroundDrawList();

    dl->AddCircle      (p, m_style.PEN_RING_RADIUS, col, m_style.PEN_RING_SEGMENTS, m_style.PEN_RING_THICK);
    dl->AddCircleFilled(p, m_style.PEN_DOT_RADIUS,  col, m_style.PEN_DOT_SEGMENTS);
}










// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.











// *************************************************************************** //
// *************************************************************************** //
//
//  END.

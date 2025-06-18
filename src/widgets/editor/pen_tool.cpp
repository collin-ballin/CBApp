/***********************************************************************************
*
*       *********************************************************************
*       ****           P E N _ T O O L . C P P  ____  F I L E            ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond
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

//  "_pen_cancel_if_escape"
//
bool Editor::_pen_cancel_if_escape(const Interaction & it)
{
    if (m_pen.active && ImGui::IsKeyPressed(ImGuiKey_Escape))
    {
        if (m_pen.path_index < m_paths.size() &&
            m_paths[m_pen.path_index].verts.size() < 2)
            m_paths.erase(m_paths.begin() + static_cast<long>(m_pen.path_index));

        m_pen = {};
        m_mode = Mode::Default;
        return true;
    }
    return false;
}


//  "_pen_try_begin_handle_drag"
//
bool Editor::_pen_try_begin_handle_drag(const Interaction & it)
{
    ImGuiIO& io = ImGui::GetIO();
    if (!io.KeyAlt || !ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        return false;

    int pi = _hit_point(it);
    if (pi < 0) return false;

    m_pen.handle_vid      = m_points[pi].v;
    m_pen.dragging_handle = true;

    if (Pos* v = find_vertex(m_vertices, m_pen.handle_vid))
        v->out_handle = { 0,0 };                    // reset for first motion

    return true;                                    // consume the click
}


//  "_pen_update_handle_drag"
//
void Editor::_pen_update_handle_drag(const Interaction & it)
{
    if (Pos* v = find_vertex(m_vertices, m_pen.handle_vid))
    {
        ImVec2 w{ it.canvas.x / m_zoom, it.canvas.y / m_zoom };
        v->out_handle = { w.x - v->x, w.y - v->y };

        // visualiser (amber line + square)
        ImVec2 a_scr{ it.origin.x + v->x * m_zoom,
                      it.origin.y + v->y * m_zoom };
        ImVec2 h_scr{ a_scr.x + v->out_handle.x * m_zoom,
                      a_scr.y + v->out_handle.y * m_zoom };

        it.dl->AddLine(a_scr, h_scr, PEN_ANCHOR_COL);
        it.dl->AddRectFilled({ h_scr.x-3, h_scr.y-3 },
                             { h_scr.x+3, h_scr.y+3 },
                             PEN_ANCHOR_COL);
    }
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        m_pen.dragging_handle = false;
}


//  "_pen_begin_path_if_click_empty"
//
void Editor::_pen_begin_path_if_click_empty(const Interaction & it)
{
    if (_hit_any(it)) return;                       // clicked on an object

    ImVec2 w{ it.canvas.x / m_zoom, it.canvas.y / m_zoom };
    uint32_t vid = _add_vertex(w);
    m_points.push_back({ vid, { PEN_ANCHOR_COL, PEN_ANCHOR_RADIUS, true } });

    Path p; p.verts = { vid }; p.closed = false;
    m_paths.push_back(std::move(p));

    m_pen = { true, m_paths.size()-1, vid, false, 0 };
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
void Editor::_pen_append_or_close_live_path(const Interaction & it)
{
    Path &  p   = m_paths[m_pen.path_index];

    //  1.  Did we click the starting anchor?  (new fast test)
    if ( _pen_click_hits_first_vertex(it, p) )
    {
        p.closed    = true;
        if (p.closed && (p.style.fill_color & 0xFF000000) == 0)
        {
            ImVec4 stroke_f = ImGui::ColorConvertU32ToFloat4(p.style.stroke_color);
            stroke_f.w = 0.4f;                                      // 40 % opacity
            p.style.fill_color = ImGui::ColorConvertFloat4ToU32(stroke_f);
        }

        m_pen       = {};
        m_mode      = Mode::Default;
        return;
    }
    

    //  2.  Fallback: distance test (unchanged)
    const float thresh = HIT_THRESH_SQ / (m_zoom * m_zoom);
    ImVec2 w{ it.canvas.x / m_zoom, it.canvas.y / m_zoom };
    if (!p.verts.empty()) {
        const Pos* first = find_vertex(m_vertices, p.verts.front());
        if (first) {
            float dx = first->x - w.x, dy = first->y - w.y;
            if (dx*dx + dy*dy <= thresh) {
                p.closed = true;
                m_pen = {};
                m_mode = Mode::Default;
                return;
            }
        }
    }

    // 3. add a new vertex (as before)
    uint32_t vid = _add_vertex(w);
    m_points.push_back({ vid, { PEN_ANCHOR_COL, PEN_ANCHOR_RADIUS, true } });
    p.verts.push_back(vid);
    m_pen.last_vid = vid;
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

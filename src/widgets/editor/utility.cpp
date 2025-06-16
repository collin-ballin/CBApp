/***********************************************************************************
*
*       *********************************************************************
*       ****            U T I L I T Y . C P P  ____  F I L E             ****
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



//  1.  HELPERS...
// *************************************************************************** //
// *************************************************************************** //

//  "find_vertex"
//
Editor::Pos * Editor::find_vertex(std::vector<Pos>& verts, uint32_t id) {
    for (auto & v : verts) if (v.id == id) return &v; return nullptr;
}


//  "find_vertex"
//
const Editor::Pos * Editor::find_vertex(const std::vector<Pos>& verts, uint32_t id) const {
    for (auto & v : verts) if (v.id == id) return &v; return nullptr;
}












// *************************************************************************** //
//
//
//  2.  DATA MODIFIER UTILITIES...
// *************************************************************************** //
// *************************************************************************** //

//  "_add_vertex"
//
uint32_t Editor::_add_vertex(ImVec2 w)
{
    m_vertices.push_back({ m_next_id++, w.x, w.y });
    return m_vertices.back().id;
}


//  "_add_point"
//
void Editor::_add_point(ImVec2 w)
{
    uint32_t vid = _add_vertex(w);
    m_points.push_back({ vid, { COL_POINT_DEFAULT, DEFAULT_POINT_RADIUS, true } });
}


//  "_erase_vertex_and_fix_paths"
//
void Editor::_erase_vertex_and_fix_paths(uint32_t vid)
{
    /* a) erase vertex record ------------------------------------------- */
    m_vertices.erase(std::remove_if(m_vertices.begin(), m_vertices.end(),
                   [vid](const Pos& v){ return v.id == vid; }),
                   m_vertices.end());

    /* b) remove this ID from every path; drop paths < 2 verts ---------- */
    for (size_t i = 0; i < m_paths.size(); /*++i done inside*/)
    {
        Path& p = m_paths[i];
        p.verts.erase(std::remove(p.verts.begin(), p.verts.end(), vid),
                      p.verts.end());

        if (p.verts.size() < 2)
            m_paths.erase(m_paths.begin() + static_cast<long>(i));
        else
            ++i;
    }

    /* c) remove any glyph referencing the vertex ----------------------- */
    m_points.erase(std::remove_if(m_points.begin(), m_points.end(),
                  [vid](const Point& pt){ return pt.v == vid; }),
                  m_points.end());
}






// *************************************************************************** //
//
//
//
//  3.  APP UTILITY OPERATIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_start_lasso_tool"
//
//  LASSO START
//      Only begins if the user clicks on empty canvas (no object hit).
//      Unless Shift or Ctrl is held, any prior selection is cleared.
void Editor::_start_lasso_tool(void)
{
    ImGuiIO &       io      = ImGui::GetIO();
    
    m_lasso_active          = true;
    m_lasso_start           = io.MousePos;                      // in screen coords
    m_lasso_end             = m_lasso_start;
    
    if ( !io.KeyShift && !io.KeyCtrl )
        m_sel.clear();  // fresh selection

    return;
}



//  "_update_lasso"
//
//  LASSO UPDATE
//      While the mouse button is held, draw the translucent rectangle.
//      When the button is released, convert the rect to world space
//      and add all intersecting objects to the selection set.
void Editor::_update_lasso(const Interaction & it)
{
    ImGuiIO &       io      = ImGui::GetIO();
    m_lasso_end             = io.MousePos;



    // Visual feedback
    it.dl->AddRectFilled(m_lasso_start, m_lasso_end, COL_LASSO_FILL);
    it.dl->AddRect      (m_lasso_start, m_lasso_end, COL_LASSO_OUT);

    // On mouse-up: finalise selection
    if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
    {
        // Screen-space rect → TL/BR order
        ImVec2 tl_scr{ std::min(m_lasso_start.x, m_lasso_end.x),
                       std::min(m_lasso_start.y, m_lasso_end.y) };
        ImVec2 br_scr{ std::max(m_lasso_start.x, m_lasso_end.x),
                       std::max(m_lasso_start.y, m_lasso_end.y) };

        // Convert to world space (inverse pan + zoom)
        ImVec2 tl_w{ (tl_scr.x - it.origin.x) / m_zoom,
                     (tl_scr.y - it.origin.y) / m_zoom };
        ImVec2 br_w{ (br_scr.x - it.origin.x) / m_zoom,
                     (br_scr.y - it.origin.y) / m_zoom };

        bool additive = io.KeyShift || io.KeyCtrl;
        if (!additive)                       // fresh selection if no modifier
            m_sel.clear();

        /* ---------- Points ---------- */
        for (size_t i = 0; i < m_points.size(); ++i)
        {
            const Pos * v = find_vertex(m_vertices, m_points[i].v);
            if (!v) continue;
            bool inside = (v->x >= tl_w.x && v->x <= br_w.x &&
                           v->y >= tl_w.y && v->y <= br_w.y);
            if (!inside) continue;

            if (additive) {                  // ⇧ / Ctrl  ⇒ toggle
                if (!m_sel.points.erase(i))   // erase returns 0 if not present
                    m_sel.points.insert(i);
            } else {
                m_sel.points.insert(i);
            }
        }

        // ---------- Lines (segment–rect test) ----------
        auto seg_rect_intersect = [](ImVec2 a, ImVec2 b, ImVec2 tl, ImVec2 br)->bool
        {
            auto inside = [&](ImVec2 p){
                return p.x >= tl.x && p.x <= br.x && p.y >= tl.y && p.y <= br.y;
            };
            if (inside(a) || inside(b))
                return true;

            // quick reject: both points on same outside side
            if ((a.x < tl.x && b.x < tl.x) || (a.x > br.x && b.x > br.x) ||
                (a.y < tl.y && b.y < tl.y) || (a.y > br.y && b.y > br.y))
                return false;

            // helper for segment–segment intersection
            auto ccw = [](ImVec2 p1, ImVec2 p2, ImVec2 p3){
                return (p3.y - p1.y)*(p2.x - p1.x) > (p2.y - p1.y)*(p3.x - p1.x);
            };
            auto intersect = [&](ImVec2 p1, ImVec2 p2, ImVec2 p3, ImVec2 p4){
                return ccw(p1,p3,p4) != ccw(p2,p3,p4) && ccw(p1,p2,p3) != ccw(p1,p2,p4);
            };

            ImVec2 tr{ br.x, tl.y }, bl{ tl.x, br.y };
            return intersect(a,b, tl,tr) || intersect(a,b,tr,br) ||
                   intersect(a,b, br,bl) || intersect(a,b, bl,tl);
        };
        
        
        for (size_t i = 0; i < m_lines.size(); ++i)
        {
            const Pos* a = find_vertex(m_vertices, m_lines[i].a);
            const Pos* b = find_vertex(m_vertices, m_lines[i].b);
            if (!a || !b) continue;

            if (!seg_rect_intersect({a->x,a->y}, {b->x,b->y}, tl_w, br_w))
                continue;

            if (additive) {
                if (!m_sel.lines.erase(i))
                    m_sel.lines.insert(i);    // toggle
            } else {
                m_sel.lines.insert(i);
            }
        }
        
        
        // ---------- Paths (segment–rect test, straight segments only) ----------
        for (size_t pi = 0; pi < m_paths.size(); ++pi)
        {
            const Path& p = m_paths[pi];
            const size_t N = p.verts.size();
            if (N < 2) continue;

            bool intersects = false;
            for (size_t si = 0; si < N - 1 + (p.closed ? 1 : 0); ++si)
            {
                const Pos* a = find_vertex(m_vertices, p.verts[si]);
                const Pos* b = find_vertex(m_vertices, p.verts[(si+1)%N]);
                if (!a || !b) continue;
                if (seg_rect_intersect({a->x,a->y}, {b->x,b->y}, tl_w, br_w))
                { intersects = true; break; }
            }
            if (!intersects) continue;

            if (additive) {
                if (!m_sel.paths.erase(pi))
                    m_sel.paths.insert(pi);    // toggle
            } else {
                m_sel.paths.insert(pi);
            }
        }
        

        /* ---------- Sync vertex list ---------- */
        _rebuild_vertex_selection();

        m_lasso_active = false;                   // reset
    }
    
    
    return;
}
    
    
    
    
    
    
    
    
//  "_zoom_canvas"
//
void Editor::_zoom_canvas(const Interaction & it)
{
    ImGuiIO &   io          = ImGui::GetIO();
    float       new_zoom    = std::clamp( m_zoom * (1.0f + io.MouseWheel * 0.1f), 0.25f, 4.0f );
        
        
    if (std::fabs(new_zoom - m_zoom) > 1e-6f)   //  Preserve cursor-centred zoom
    {
        ImVec2 world_before{ (io.MousePos.x - it.origin.x) / m_zoom,
                             (io.MousePos.y - it.origin.y) / m_zoom };
        m_zoom = new_zoom;
        ImVec2 new_origin{ io.MousePos.x - world_before.x * m_zoom,
                           io.MousePos.y - world_before.y * m_zoom };
        m_scroll.x = new_origin.x - it.tl.x;
        m_scroll.y = new_origin.y - it.tl.y;
    }
    
    return;
}









// *************************************************************************** //
//
//
//
//      MISC. UTILITIES...
// *************************************************************************** //
// *************************************************************************** //

//  "_draw_controls"
//
void Editor::_draw_controls(const ImVec2& pos)
{
    ImGui::SetCursorScreenPos(pos);
    ImGui::BeginGroup();

    // Mode selector
    int mode_i = static_cast<int>(m_mode);
    ImGui::SetNextItemWidth(110.0f);
    ImGui::Combo("Mode", &mode_i, ms_MODE_LABELS.data(),
                 static_cast<int>(Mode::Count));
    m_mode = static_cast<Mode>(mode_i);

    // Grid toggle
    ImGui::SameLine(0.0f, 15.0f);
    ImGui::Checkbox("Grid", &m_show_grid);

    // Clear button
    ImGui::SameLine(0.0f, 15.0f);
    if (ImGui::Button("Clear"))
        _clear_all();

    ImGui::EndGroup();
}


//  "_clear_all"
//
void Editor::_clear_all()
{
    m_vertices.clear();
    m_points.clear();
    m_lines.clear();
    m_sel.clear();
    m_lasso_active  = false;
    m_dragging      = false;
    m_drawing       = false;
    m_next_id       = 1;
    m_pen = {};
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

/***********************************************************************************
*
*       *********************************************************************
*       ****             R E N D E R . C P P  ____  F I L E              ****
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

//
//      ...
//












// *************************************************************************** //
//
//
//
//  1.  RENDERING CORE MECHANICS OF THE WINDOW...
// *************************************************************************** //
// *************************************************************************** //

//  "_draw_grid"
//
void Editor::_draw_grid(ImDrawList* dl, const ImVec2& p0, const ImVec2& sz) const
{
    float step = GRID_STEP * m_zoom;
    float x0 = std::fmod(m_scroll.x, step);
    for (float x = x0; x < sz.x; x += step)
        dl->AddLine({ p0.x + x, p0.y }, { p0.x + x, p0.y + sz.y }, IM_COL32(200,200,200,40));
    float y0 = std::fmod(m_scroll.y, step);
    for (float y = y0; y < sz.y; y += step)
        dl->AddLine({ p0.x, p0.y + y }, { p0.x + sz.x, p0.y + y }, IM_COL32(200,200,200,40));
}












// *************************************************************************** //
//
//
//
//  2.  RENDERING THE INTERACTIBLE OBJECTS...
// *************************************************************************** //
// *************************************************************************** //

//  "_draw_lines"
//
void Editor::_draw_lines(ImDrawList* dl, const ImVec2& origin) const
{
#ifdef LEGACY_LINES
    for (const auto& ln : m_lines)
    {
        const Pos* a = find_vertex(m_vertices, ln.a);
        const Pos* b = find_vertex(m_vertices, ln.b);
        if (!a || !b) continue;
        dl->AddLine({ origin.x + a->x * m_zoom, origin.y + a->y * m_zoom },
                    { origin.x + b->x * m_zoom, origin.y + b->y * m_zoom },
                    ln.color, ln.thickness);
    }
#else
    (void)dl; (void)origin; // suppress unused‑param warnings
#endif
}


//  "_draw_paths"
//
void Editor::_draw_paths(ImDrawList* dl, const ImVec2& origin) const
{
    for (const Path& p : m_paths)
    {
        const size_t N = p.verts.size();
        if (N < 2) continue;

        auto draw_seg = [&](const Pos* a, const Pos* b){
            bool curved = (a->out_handle.x || a->out_handle.y ||
                           b->in_handle.x  || b->in_handle.y);

            if (!curved)    // straight line
            {
                dl->AddLine({ origin.x + a->x * m_zoom, origin.y + a->y * m_zoom },
                            { origin.x + b->x * m_zoom, origin.y + b->y * m_zoom },
                            p.style.stroke_color, p.style.stroke_width);
            }
            else            // cubic Bézier
            {
                ImVec2 P0{ origin.x + a->x * m_zoom,                  origin.y + a->y * m_zoom };
                ImVec2 P1{ origin.x + (a->x + a->out_handle.x) * m_zoom,
                           origin.y + (a->y + a->out_handle.y) * m_zoom };
                ImVec2 P2{ origin.x + (b->x + b->in_handle.x)  * m_zoom,
                           origin.y + (b->y + b->in_handle.y)  * m_zoom };
                ImVec2 P3{ origin.x + b->x * m_zoom,                  origin.y + b->y * m_zoom };

                dl->AddBezierCubic(P0, P1, P2, P3,
                                   p.style.stroke_color,
                                   p.style.stroke_width,
                                   ms_BEZIER_SEGMENTS);  // 0 ⇒ ImGui default tessellation
            }
        };

        for (size_t i = 0; i < N - 1; ++i)
            if (const Pos* a = find_vertex(m_vertices, p.verts[i]))
                if (const Pos* b = find_vertex(m_vertices, p.verts[i+1]))
                    draw_seg(a, b);

        if (p.closed)
            if (const Pos* a = find_vertex(m_vertices, p.verts.back()))
                if (const Pos* b = find_vertex(m_vertices, p.verts.front()))
                    draw_seg(a, b);
    }
}


//  "_draw_points"
//
void Editor::_draw_points(ImDrawList* dl, const ImVec2& origin) const
{
    for (size_t i = 0; i < m_points.size(); ++i)
    {
        const Point& pt = m_points[i]; if (!pt.sty.visible) continue;
        const Pos* v = find_vertex(m_vertices, pt.v); if (!v) continue;
        ImU32 col = (m_dragging && m_sel.points.count(i)) ? COL_POINT_HELD : pt.sty.color;
        dl->AddCircleFilled({ origin.x + v->x * m_zoom, origin.y + v->y * m_zoom }, pt.sty.radius, col);
    }
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

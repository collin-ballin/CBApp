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





// -----------------------------------------------------------------------------
// GRID HELPERS
// -----------------------------------------------------------------------------
ImVec2 Editor::_grid_snap(ImVec2 w) const
{
    if ( !this->want_snap() ) return w;
    
    const float s = m_grid.world_step;
    w.x = std::round(w.x / s) * s;
    w.y = std::round(w.y / s) * s;
    return w;
}


//  "_grid_handle_shortcuts"
//
//      I think our policy should be to RETURN OUT after implementing a SINGLE hotkey.
//      In other words, only allow ONE hotkey activation per frame.  It seems problematic to allow
//      the user to perform both a CTRL+ AND CTRL- (inverse operations) in the same frame.
//
void Editor::_grid_handle_shortcuts(void)
{
    ImGuiIO &   io      = ImGui::GetIO();
    
    
    //  1.  TOGGLE SNAP-TO-GRID.            [ SHIFT G ]
    if ( io.KeyShift && ImGui::IsKeyPressed(ImGuiKey_G) )
    { m_grid.snap_on = !m_grid.snap_on; return; }
   
   
    //  Exit early if CTRL key is not pressed.
    if ( !io.KeyCtrl )          return;


    //  2.  INCREASE GRID SPACING.          [ CTRL + ]
    if ( ImGui::IsKeyPressed(ImGuiKey_Equal) )
    { m_grid.world_step = std::max(ms_GRID_STEP_MIN,  m_grid.world_step * 0.5f); return; }
       
    //  3.  DECREASE GRID SPACING.          [ CTRL – ]
    if ( ImGui::IsKeyPressed(ImGuiKey_Minus) )
    { m_grid.world_step *= 2.f; return; }
        
    return;
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
        dl->AddLine({ origin.x + a->x * m_cam.zoom_mag, origin.y + a->y * m_cam.zoom_mag },
                    { origin.x + b->x * m_cam.zoom_mag, origin.y + b->y * m_cam.zoom_mag },
                    ln.color, ln.thickness);
    }
#else
    (void)dl; (void)origin; // suppress unused‑param warnings
#endif
}


//  "_draw_paths"
//
//void Editor::_draw_paths(ImDrawList* dl, const ImVec2& origin) const
void Editor::_draw_paths(ImDrawList* dl) const
{
    for (const Path& p : m_paths)
    {
        const size_t N = p.verts.size();
        if (N < 2) continue;

        // ───── Filled‑area pass (only for closed paths with non‑transparent fill)
        if (p.is_area() && (p.style.fill_color & 0xFF000000))
        {
            dl->PathClear();

            for (size_t i = 0; i < N; ++i)
            {
                const Pos* a = find_vertex(m_vertices, p.verts[i]);
                const Pos* b = find_vertex(m_vertices, p.verts[(i + 1) % N]);
                if (!a || !b) continue;

                if (!is_curved(a, b)) {
                    ImVec2 pa = world_to_pixels({ a->x, a->y });
                    dl->PathLineTo(pa);
                }
                else {
                    for (int step = 0; step <= ms_BEZIER_FILL_STEPS; ++step) {
                        float  t  = static_cast<float>(step) / ms_BEZIER_FILL_STEPS;
                        ImVec2 wp = cubic_eval(a, b, t);
                        dl->PathLineTo(world_to_pixels(wp));
                    }
                }
            }
            dl->PathFillConvex(p.style.fill_color);
        }

        // ───── Lambda to draw one segment (straight or cubic)
        auto draw_seg = [&](const Pos* a, const Pos* b)
        {
            const bool curved = is_curved(a, b);

            if (!curved) {
                dl->AddLine(world_to_pixels({ a->x, a->y }),
                            world_to_pixels({ b->x, b->y }),
                            p.style.stroke_color,
                            p.style.stroke_width);
            }
            else {
                ImVec2 P0 = world_to_pixels({ a->x,                         a->y });
                ImVec2 P1 = world_to_pixels({ a->x + a->out_handle.x,       a->y + a->out_handle.y });
                ImVec2 P2 = world_to_pixels({ b->x + b->in_handle.x,        b->y + b->in_handle.y  });
                ImVec2 P3 = world_to_pixels({ b->x,                         b->y });

                dl->AddBezierCubic(P0, P1, P2, P3,
                                   p.style.stroke_color,
                                   p.style.stroke_width,
                                   ms_BEZIER_SEGMENTS);   // 0 ⇒ default tessellation
            }
        };

        // ───── Stroke all contiguous segments
        for (size_t i = 0; i < N - 1; ++i)
            if (const Pos* a = find_vertex(m_vertices, p.verts[i]))
                if (const Pos* b = find_vertex(m_vertices, p.verts[i + 1]))
                    draw_seg(a, b);

        // Close the loop if required
        if (p.closed)
            if (const Pos* a = find_vertex(m_vertices, p.verts.back()))
                if (const Pos* b = find_vertex(m_vertices, p.verts.front()))
                    draw_seg(a, b);
    }
}


//  "_draw_points"
//
void Editor::_draw_points(ImDrawList* dl) const
{
    for (size_t i = 0; i < m_points.size(); ++i)
    {
        const Point& pt = m_points[i];
        if (!pt.sty.visible) continue;

        const Pos* v = find_vertex(m_vertices, pt.v);
        if (!v) continue;

        ImU32 col = (m_dragging && m_sel.points.count(i))
                    ? COL_POINT_HELD
                    : pt.sty.color;

        ImVec2 pix = world_to_pixels({ v->x, v->y });   // NEW transform
        dl->AddCircleFilled(pix, pt.sty.radius, col, 12);
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

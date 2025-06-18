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
float Editor::_grid_step_px() const            { return m_grid.world_step * m_zoom; }

ImVec2 Editor::_grid_snap(ImVec2 w) const
{
    if (!m_grid.snap_on) return w;
    const float s = m_grid.world_step;
    w.x = std::round(w.x / s) * s;
    w.y = std::round(w.y / s) * s;
    return w;
}


void Editor::_grid_draw(ImDrawList* dl, const ImVec2& p0, const ImVec2& sz) const
{
    if (!m_grid.visible) return;

    const float step_px = _grid_step_px();
    const ImU32 lineCol = IM_COL32(200,200,200, 64);
    const ImU32 textCol = ImGui::GetColorU32(ImGuiCol_Text);   // respect current theme

    // Label skip: ensure ≥50 px between labels
    const int skip = std::max(1, int(std::ceil(50.f / step_px)));
    const int maxLabels = 150;     // safety hard cap per edge

    ImGui::PushClipRect(p0, {p0.x + sz.x, p0.y + sz.y}, true);

    float first_wx = -m_scroll.x / m_zoom;
    float first_wy = -m_scroll.y / m_zoom;

    int labelCountX = 0, labelCountY = 0;
    int gridIndex   = 0;

    // X lines + labels
    for (float x = std::fmod(m_scroll.x, step_px); x < sz.x; x += step_px, ++gridIndex)
    {
        dl->AddLine({ p0.x + x, p0.y }, { p0.x + x, p0.y + sz.y }, lineCol);

        if (gridIndex % skip == 0 && labelCountX < maxLabels)
        {
            float wx = first_wx + x / m_zoom;
            char buf[16]; std::snprintf(buf, sizeof(buf), "%.0f", wx);
            dl->AddText({ p0.x + x + ms_GRID_LABEL_PAD, p0.y + ms_GRID_LABEL_PAD }, textCol, buf);
            ++labelCountX;
        }
    }

    // Y lines + labels
    gridIndex = 0;
    for (float y = std::fmod(m_scroll.y, step_px); y < sz.y; y += step_px, ++gridIndex)
    {
        dl->AddLine({ p0.x, p0.y + y }, { p0.x + sz.x, p0.y + y }, lineCol);

        if (gridIndex % skip == 0 && labelCountY < maxLabels)
        {
            float wy = first_wy + y / m_zoom;
            char buf[16]; std::snprintf(buf, sizeof(buf), "%.0f", wy);
            dl->AddText({ p0.x + ms_GRID_LABEL_PAD, p0.y + y + ms_GRID_LABEL_PAD }, textCol, buf);
            ++labelCountY;
        }
    }
    ImGui::PopClipRect();
}


// ---------------------------------------------------------------------------
// Editor::Begin – (partial, fit-to-canvas initialization)
// ---------------------------------------------------------------------------
// (NOTE: The following should be inserted in Editor::Begin after m_p1 is set)
// (Implementation provided per instructions)

// ... inside Editor::Begin, after:
//     m_p1 = { m_p0.x + m_avail.x,     m_p0.y + m_avail.y };
//
// Insert:
//
// ------------------------------------------------------------------
// One‑time “fit to canvas” initialisation
// ------------------------------------------------------------------
// (This code should be in Editor::Begin, not here, but placed here for patch context)
/*
    if (m_first_frame_init)
    {
        // Fit‑zoom so the entire world rectangle is visible with ~10 % margin
        const float fitX = m_avail.x / m_world_extent.x;
        const float fitY = m_avail.y / m_world_extent.y;
        m_zoom = 0.9f * std::min(fitX, fitY);
        _clamp_zoom(m_zoom);          // respect min/max bounds

        // Centre the view
        m_scroll = ImVec2((m_world_extent.x * m_zoom - m_avail.x) * 0.5f,
                          (m_world_extent.y * m_zoom - m_avail.y) * 0.5f);
        _clamp_scroll();

        // Choose a sensible initial grid pitch: 10 cells across the shorter axis
        m_grid.world_step = std::max(ms_GRID_STEP_MIN,
                                     std::min(m_world_extent.x, m_world_extent.y) / 10.0f);

        m_first_frame_init = false;
    }
*/


// optional: call this once per frame to handle CTRL + / – hot-keys
void Editor::_grid_handle_shortcuts()
{
    ImGuiIO& io = ImGui::GetIO();
    if (!io.KeyCtrl) return;

    if (ImGui::IsKeyPressed(ImGuiKey_Equal))            // CTRL +
        m_grid.world_step = std::max(ms_GRID_STEP_MIN,  m_grid.world_step * 0.5f);
    if (ImGui::IsKeyPressed(ImGuiKey_Minus))            // CTRL –
        m_grid.world_step *= 2.f;
        
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


        //  FILLED AREA PASS...
        //      If the path is closed *and* has a visible fill alpha, draw the
        //      convex (straight‑segment) polygon before stroking.
        if (p.is_area() && (p.style.fill_color & 0xFF000000))
        {
            dl->PathClear();

            for (size_t i = 0; i < N; ++i)
            {
                const Pos* a = find_vertex(m_vertices, p.verts[i]);
                const Pos* b = find_vertex(m_vertices, p.verts[(i + 1) % N]);
                if (!a || !b) continue;

                if (!is_curved(a, b))
                {
                    ImVec2 P{ origin.x + a->x * m_zoom, origin.y + a->y * m_zoom };
                    dl->PathLineTo(P);
                }
                else
                {
                    for (int step = 0; step <= ms_BEZIER_FILL_STEPS; ++step)
                    {
                        float t = static_cast<float>(step) / ms_BEZIER_FILL_STEPS;
                        ImVec2 w = cubic_eval(a, b, t);
                        dl->PathLineTo({ origin.x + w.x * m_zoom, origin.y + w.y * m_zoom });
                    }
                }
            }
            dl->PathFillConvex(p.style.fill_color);
        }
        
        
        //  "draw_seg"
        //
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
    
    return;
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
    
    return;
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

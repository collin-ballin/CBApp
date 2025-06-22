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


//  "_grid_draw"
//
void Editor::_grid_draw(ImDrawList* dl, const ImVec2& p0, const ImVec2& sz) const
{
    if (!m_grid.visible) return;

    const float     step_px         = _grid_step_px();
    const ImU32     lineCol         = IM_COL32(200,200,200, 64);
    const ImU32     textCol         = ImGui::GetColorU32(ImGuiCol_Text);   // respect current theme

    // Label skip: ensure ≥50 px between labels
    const int       skip            = std::max(1, int(std::ceil(50.f / step_px)));
    const int       maxLabels       = 150;     // safety hard cap per edge

    ImGui::PushClipRect(p0, {p0.x + sz.x, p0.y + sz.y}, true);

    float           first_wx        = -m_cam.pan.x / m_cam.zoom_mag;
    float           first_wy        = -m_cam.pan.y / m_cam.zoom_mag;
    int             labelCountX     = 0, labelCountY = 0;
    int             gridIndex       = 0;
    
    //  1.  X lines + labels
    float           start_x         = std::fmod(m_cam.pan.x, step_px);
    
    if (start_x < 0.0f)     { start_x += step_px; }          // ensure start_x ∈ [0, step_px)
    gridIndex = 0;
    
    for (float x = start_x; x < sz.x; x += step_px, ++gridIndex) {
        dl->AddLine({ p0.x + x, p0.y }, { p0.x + x, p0.y + sz.y }, lineCol);

        if (gridIndex % skip == 0 && labelCountX < maxLabels) {
            float wx = first_wx + x / m_cam.zoom_mag;
            char buf[16]; std::snprintf(buf, sizeof(buf), "%.0f", wx);
            dl->AddText({ p0.x + x + ms_GRID_LABEL_PAD, p0.y + ms_GRID_LABEL_PAD }, textCol, buf);
            ++labelCountX;
        }
    }


    //  2.  Y lines + labels
    float           start_y         = std::fmod(m_cam.pan.y, step_px);
    if (start_y < 0.0f)     { start_y += step_px; }
    gridIndex = 0;
    
    for (float y = start_y; y < sz.y; y += step_px, ++gridIndex) {
        dl->AddLine({ p0.x, p0.y + y }, { p0.x + sz.x, p0.y + y }, lineCol);

        if (gridIndex % skip == 0 && labelCountY < maxLabels)
        {
            float wy = first_wy + y / m_cam.zoom_mag;
            char buf[16]; std::snprintf(buf, sizeof(buf), "%.0f", wy);
            dl->AddText({ p0.x + ms_GRID_LABEL_PAD, p0.y + y + ms_GRID_LABEL_PAD }, textCol, buf);
            ++labelCountY;
        }
    }
    
    ImGui::PopClipRect();
    return;
}
/*
{
    if (!m_grid.visible) return;

    const float     step_px         = _grid_step_px();
    const ImU32     lineCol         = IM_COL32(200,200,200, 64);
    const ImU32     textCol         = ImGui::GetColorU32(ImGuiCol_Text);   // respect current theme

    // Label skip: ensure ≥50 px between labels
    const int       skip            = std::max(1, int(std::ceil(50.f / step_px)));
    const int       maxLabels       = 150;     // safety hard cap per edge

    ImGui::PushClipRect(p0, {p0.x + sz.x, p0.y + sz.y}, true);

    float           first_wx        = -m_cam.pan.x / m_cam.zoom_mag;
    float           first_wy        = -m_cam.pan.y / m_cam.zoom_mag;
    int             labelCountX     = 0, labelCountY = 0;
    int             gridIndex       = 0;
    
    //  1.  X lines + labels
    float           start_x         = std::fmod(m_cam.pan.x, step_px);
    
    if (start_x < 0.0f)     { start_x += step_px; }          // ensure start_x ∈ [0, step_px)
    gridIndex = 0;
    
    for (float x = start_x; x < sz.x; x += step_px, ++gridIndex) {
        dl->AddLine({ p0.x + x, p0.y }, { p0.x + x, p0.y + sz.y }, lineCol);

        if (gridIndex % skip == 0 && labelCountX < maxLabels) {
            float wx = first_wx + x / m_cam.zoom_mag;
            char buf[16]; std::snprintf(buf, sizeof(buf), "%.0f", wx);
            dl->AddText({ p0.x + x + ms_GRID_LABEL_PAD, p0.y + ms_GRID_LABEL_PAD }, textCol, buf);
            ++labelCountX;
        }
    }


    //  2.  Y lines + labels
    float           start_y         = std::fmod(m_cam.pan.y, step_px);
    if (start_y < 0.0f)     { start_y += step_px; }
    gridIndex = 0;
    
    for (float y = start_y; y < sz.y; y += step_px, ++gridIndex) {
        dl->AddLine({ p0.x, p0.y + y }, { p0.x + sz.x, p0.y + y }, lineCol);

        if (gridIndex % skip == 0 && labelCountY < maxLabels)
        {
            float wy = first_wy + y / m_cam.zoom_mag;
            char buf[16]; std::snprintf(buf, sizeof(buf), "%.0f", wy);
            dl->AddText({ p0.x + ms_GRID_LABEL_PAD, p0.y + y + ms_GRID_LABEL_PAD }, textCol, buf);
            ++labelCountY;
        }
    }
    
    ImGui::PopClipRect();
    return;
}
*/


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
void Editor::_draw_paths(ImDrawList* dl, const ImVec2& origin) const
{
    for (const Path & p : m_paths)
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
                    ImVec2 P{ origin.x + a->x * m_cam.zoom_mag, origin.y + a->y * m_cam.zoom_mag };
                    dl->PathLineTo(P);
                }
                else
                {
                    for (int step = 0; step <= ms_BEZIER_FILL_STEPS; ++step)
                    {
                        float t = static_cast<float>(step) / ms_BEZIER_FILL_STEPS;
                        ImVec2 w = cubic_eval(a, b, t);
                        dl->PathLineTo({ origin.x + w.x * m_cam.zoom_mag, origin.y + w.y * m_cam.zoom_mag });
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
                dl->AddLine({ origin.x + a->x * m_cam.zoom_mag, origin.y + a->y * m_cam.zoom_mag },
                            { origin.x + b->x * m_cam.zoom_mag, origin.y + b->y * m_cam.zoom_mag },
                            p.style.stroke_color, p.style.stroke_width);
            }
            else            // cubic Bézier
            {
                ImVec2 P0{ origin.x + a->x * m_cam.zoom_mag,                  origin.y + a->y * m_cam.zoom_mag };
                ImVec2 P1{ origin.x + (a->x + a->out_handle.x) * m_cam.zoom_mag,
                           origin.y + (a->y + a->out_handle.y) * m_cam.zoom_mag };
                ImVec2 P2{ origin.x + (b->x + b->in_handle.x)  * m_cam.zoom_mag,
                           origin.y + (b->y + b->in_handle.y)  * m_cam.zoom_mag };
                ImVec2 P3{ origin.x + b->x * m_cam.zoom_mag,                  origin.y + b->y * m_cam.zoom_mag };

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
        dl->AddCircleFilled({ origin.x + v->x * m_cam.zoom_mag, origin.y + v->y * m_cam.zoom_mag }, pt.sty.radius, col);
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

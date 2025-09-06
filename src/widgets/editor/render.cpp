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
    { m_grid.snap_step = std::max(ms_GRID_STEP_MIN,  m_grid.snap_step * 0.5f); return; }
       
    //  3.  DECREASE GRID SPACING.          [ CTRL – ]
    if ( ImGui::IsKeyPressed(ImGuiKey_Minus) )
    { m_grid.snap_step *= 2.f; return; }
        
    return;
}








// *************************************************************************** //
//
//
//
//  2.  PRIMARY RENDERING OPERATIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_render_lines"
//
void Editor::_render_lines(ImDrawList* dl, const ImVec2& origin) const
{
#ifdef LEGACY_LINES
    for (const auto& ln : m_lines)
    {
        const Vertex * a = find_vertex(m_vertices, ln.a);
        const Vertex * b = find_vertex(m_vertices, ln.b);
        if (!a || !b) continue;
        dl->AddLine({ origin.x + a->x * m_cam.zoom_mag, origin.y + a->y * m_cam.zoom_mag },
                    { origin.x + b->x * m_cam.zoom_mag, origin.y + b->y * m_cam.zoom_mag },
                    ln.color, ln.thickness);
    }
#else
    (void)dl; (void)origin; // suppress unused‑param warnings
#endif
}


//  "_render_paths"
//
//void Editor::_draw_paths(ImDrawList* dl, const ImVec2& origin) const
void Editor::_render_paths(ImDrawList * dl) const
{
    //  1.  BUILD THE DRAW-LIST (USE VISIBLE PATHS ONLY)...
    std::vector<const Path*>    draw_vec;
    draw_vec.reserve(m_paths.size());


    for (const Path & p : m_paths) {
        if (p.visible)              { draw_vec.push_back(&p); }// NEW visibility filter
    }


    //  2.  STABLE-SORT BY Z-INDEX (Low Z: Background → High Z: Foreground)...
    std::stable_sort( draw_vec.begin(), draw_vec.end(),
                      [](const Path* a, const Path* b) { return a->z_index < b->z_index; } );


    //  3.  DRAW EACH PATH IN SORTED ORDER...
    for (const Path * pp : draw_vec)
    {
        const Path &    p   = *pp;
        const size_t    N   = p.verts.size();
        if (N < 2) continue;

        // ───── Filled-area pass (only for closed paths with non-transparent fill)
        if (p.is_area() && (p.style.fill_color & 0xFF000000))
        {
            dl->PathClear();

            for (size_t i = 0; i < N; ++i)
            {
                const Vertex* a = find_vertex(m_vertices, p.verts[i]);
                const Vertex* b = find_vertex(m_vertices, p.verts[(i + 1) % N]);
                if (!a || !b) continue;

                if (!is_curved<VertexID>(a, b)) {
                    ImVec2 pa = world_to_pixels({ a->x, a->y });
                    dl->PathLineTo(pa);
                }
                else {
                    for (int step = 0; step <= m_style.ms_BEZIER_FILL_STEPS; ++step) {
                        float  t  = static_cast<float>(step) / m_style.ms_BEZIER_FILL_STEPS;
                        ImVec2 wp = cubic_eval<VertexID>(a, b, t);
                        dl->PathLineTo(world_to_pixels(wp));
                    }
                }
            }
            dl->PathFillConvex(p.style.fill_color);
        }

        // ───── Lambda to draw one segment (straight or cubic)
        auto draw_seg = [&](const Vertex * a, const Vertex * b)
        {
            const bool curved = is_curved<VertexID>(a, b);

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
                                   m_style.ms_BEZIER_SEGMENTS);   // 0 ⇒ default tessellation
            }
        };

        // ───── Stroke contiguous segments
        for ( size_t i = 0; i < N - 1; ++i ) {
            if ( const Vertex* a = find_vertex(m_vertices, p.verts[i]) ) {
                if ( const Vertex* b = find_vertex(m_vertices, p.verts[i + 1]) )    { draw_seg(a, b); }
            }
        }

        // Close the loop if required
        if (p.closed) {
            if ( const Vertex* a = find_vertex(m_vertices, p.verts.back()) ) {
                if ( const Vertex* b = find_vertex(m_vertices, p.verts.front()) )   { draw_seg(a, b); }
            }
        }
    }
    
    return;
}


//  "_render_points"
//
void Editor::_render_points(ImDrawList* dl) const
{
    for (size_t i = 0; i < m_points.size(); ++i)
    {
        const Point& pt = m_points[i];
        if (!pt.sty.visible) continue;

        const Vertex* v = find_vertex(m_vertices, pt.v);
        if (!v) continue;

        ImU32 col = (m_dragging && m_sel.points.count(i))
                    ? m_style.COL_POINT_HELD
                    : pt.sty.color;

        ImVec2 pix = world_to_pixels({ v->x, v->y });   // NEW transform
        dl->AddCircleFilled(pix, pt.sty.radius, col, 12);
    }
    
    return;
}

//
//
// *************************************************************************** //
// *************************************************************************** //   END "?".






// *************************************************************************** //
//
//
//
//      3.      "SELECTION HIGHLIGHT" RENDERING / INTERACTIBLES...
// *************************************************************************** //
// *************************************************************************** //

//  "_render_selection_highlight"
//      Draw outlines for selected primitives plus bbox/handles.
//
void Editor::_render_selection_highlight(ImDrawList * dl) const
{
    const ImU32 &               col         = m_style.COL_SELECTION_OUT;
    auto                        ws2px       = [this](ImVec2 w){ return world_to_pixels(w); };
    const BrowserState &        BS          = this->m_browser_S;


    // ───── Highlight selected points
    for (size_t idx : m_sel.points)
    {
        if ( idx >= m_points.size() )       { continue; }
        const Point &           pt  = m_points[idx];
        if (const Vertex *      v   = find_vertex(m_vertices, pt.v))
        {
            ImVec2  scr     = ws2px({ v->x, v->y });
            dl->AddCircle( scr,
                           pt.sty.radius + 2.f,        // small outset
                           col, 0, 2.f);               // thickness 2 px
        }
    }

    // ───── Highlight selected standalone lines
    for (size_t idx : m_sel.lines)
    {
        if ( idx >= m_lines.size() )        { continue; }
        const Line &        ln      = m_lines[idx];
        const Vertex *      a       = find_vertex(m_vertices, ln.a);
        const Vertex *      b       = find_vertex(m_vertices, ln.b);
        
        if ( a && b ) {
            dl->AddLine(ws2px({ a->x, a->y }),
                        ws2px({ b->x, b->y }),
                        col, ln.thickness + 2.f);
        }
    }

    // ───── Highlight selected paths
    for (size_t idx : m_sel.paths)
    {
        if ( idx >= m_paths.size() )            { continue; }
        const Path &        p           = m_paths[idx];
        const size_t        N           = p.verts.size();
        if (N < 2)                              { continue; }

        auto                draw_seg    = [&](const Vertex * a, const Vertex * b)
        {
            const float w = p.style.stroke_width + 2.0f;
            if ( is_curved<VertexID>(a,b) )
            {
                ImVec2 P0 = ws2px({ a->x,                         a->y });
                ImVec2 P1 = ws2px({ a->x + a->out_handle.x,       a->y + a->out_handle.y });
                ImVec2 P2 = ws2px({ b->x + b->in_handle.x,        b->y + b->in_handle.y  });
                ImVec2 P3 = ws2px({ b->x,                         b->y });
                dl->AddBezierCubic(P0, P1, P2, P3, col, w, m_style.ms_BEZIER_SEGMENTS);
            }
            else
            {
                dl->AddLine(ws2px({ a->x, a->y }),
                            ws2px({ b->x, b->y }),
                            col, w);
            }
        };

        for (size_t i = 0; i < N - 1; ++i)
        {
            if ( const Vertex* a = find_vertex(m_vertices, p.verts[i]) )
            {
                if (const Vertex * b = find_vertex(m_vertices, p.verts[i+1]))
                    draw_seg(a, b);
            }
        }

        if ( p.closed ) {
            if ( const Vertex * a = find_vertex(m_vertices, p.verts.back()) )
            {
                if ( const Vertex* b = find_vertex(m_vertices, p.verts.front()) )
                    draw_seg(a, b);
            }
        }
    }



    this->_render_selection_bbox      (dl);
    this->_render_selected_handles    (dl);
    this->_render_selected_handles    (dl);
    
    if ( BS.HasAuxiliarySelection() )
    {
        _render_auxiliary_highlights(dl);
    }
    
    

    return;
}

//  "_render_selected_handles"
//
inline void Editor::_render_selected_handles(ImDrawList * dl) const
{
    auto ws2px = [this](ImVec2 w){ return world_to_pixels(w); };

    for (const Vertex & v : m_vertices)
    {
        if ( !m_show_handles.count(v.id) )      { continue; }   // ← NEW visibility mask

        ImVec2 a = ws2px({ v.x, v.y });

        auto draw_handle = [&](const ImVec2 & off)
        {
            if ( (off.x == 0.f)  &&  (off.y == 0.f) )   { return; }
            
            ImVec2 h = ws2px({ v.x + off.x, v.y + off.y });
            dl->AddLine(a, h, m_style.ms_HANDLE_COLOR, 1.0f);
            dl->AddRectFilled({ h.x - m_style.ms_HANDLE_SIZE, h.y - m_style.ms_HANDLE_SIZE },
                              { h.x + m_style.ms_HANDLE_SIZE, h.y + m_style.ms_HANDLE_SIZE },
                              m_style.ms_HANDLE_COLOR);
        };

        draw_handle(v.out_handle);
        draw_handle(v.in_handle);
    }
    
    return;
}


//  "_render_selection_bbox"
//
inline void Editor::_render_selection_bbox(ImDrawList * dl) const
{
    const auto& V = m_boxdrag.view;
    if (!V.visible) { return; }                       // nothing to draw

    // Draw expanded bbox (from cache)
    const ImVec2 p0 = world_to_pixels(V.tl_ws);
    const ImVec2 p1 = world_to_pixels(V.br_ws);
    dl->AddRect(p0, p1,
                m_style.SELECTION_BBOX_COL, 0.0f,
                ImDrawFlags_None, m_style.SELECTION_BBOX_TH);

    // Draw the 8 handles using cached rectangles and cached hover index
    for (int i = 0; i < 8; ++i)
    {
        const bool          hovered     = (i == V.hover_idx);      // ← read, don’t compute
        const ImRect &      r           = V.handle_rect_px[i];
        dl->AddRectFilled(r.Min, r.Max,
                          hovered ? m_style.ms_HANDLE_HOVER_COLOR
                                  : m_style.ms_HANDLE_COLOR);
    }
    
    return;
}

/*{
    const bool has_paths_or_lines = !m_sel.paths.empty() || !m_sel.lines.empty();
    const bool single_vertex_only = (m_sel.vertices.size() <= 1) && !has_paths_or_lines;
    if (single_vertex_only) return;

    ImVec2 tl_tight, br_tight;
    if (!_selection_bounds(tl_tight, br_tight)) { m_hover_handle = -1; return; }

    // NEW: robust expansion (pixel-space min/max)
    const auto [tl, br] = _expand_bbox_by_pixels(tl_tight, br_tight, this->m_style.SELECTION_BBOX_MARGIN_PX);

    auto ws2px = [this](ImVec2 w){ return world_to_pixels(w); };
    ImVec2 p0 = ws2px(tl);
    ImVec2 p1 = ws2px(br);

    dl->AddRect(p0, p1, m_style.SELECTION_BBOX_COL, 0.0f,
                ImDrawFlags_None, m_style.SELECTION_BBOX_TH);

    // Handle positions from expanded WS box
    ImVec2 hw{ (tl.x + br.x) * 0.5f, (tl.y + br.y) * 0.5f };
    const ImVec2 ws[8] = {
        tl, { hw.x, tl.y }, { br.x, tl.y }, { br.x, hw.y },
        br, { hw.x, br.y }, { tl.x, br.y }, { tl.x, hw.y }
    };

    m_hover_handle = -1;
    for (int i = 0; i < 8; ++i)
    {
        ImVec2 s = ws2px(ws[i]);
        ImVec2 min{ s.x - m_style.HANDLE_BOX_SIZE, s.y - m_style.HANDLE_BOX_SIZE };
        ImVec2 max{ s.x + m_style.HANDLE_BOX_SIZE, s.y + m_style.HANDLE_BOX_SIZE };

        bool hovered = ImGui::IsMouseHoveringRect(min, max);
        if (hovered) m_hover_handle = i;

        dl->AddRectFilled(min, max, hovered ? m_style.ms_HANDLE_HOVER_COLOR
                                            : m_style.ms_HANDLE_COLOR);
    }
    
    return;
}*/


/*{
    const bool      has_paths_or_lines      = !m_sel.paths.empty() || !m_sel.lines.empty();
    const bool      single_vertex_only      = (m_sel.vertices.size() <= 1) && !has_paths_or_lines;
    
    if ( single_vertex_only )               { return; }

    ImVec2      tl, br;
    if ( !_selection_bounds(tl, br) )       { m_hover_handle = -1; return; }

    auto        ws2px       = [this](ImVec2 w){ return world_to_pixels(w); };
    ImVec2      p0          = ws2px(tl);
    ImVec2      p1          = ws2px(br);

    dl->AddRect(p0, p1, m_style.SELECTION_BBOX_COL, 0.0f,
                ImDrawFlags_None, m_style.SELECTION_BBOX_TH);

    // handle positions
    ImVec2 hw{ (tl.x + br.x) * 0.5f, (tl.y + br.y) * 0.5f };
    const ImVec2 ws[8] = {
        tl,
        { hw.x, tl.y },
        { br.x, tl.y },
        { br.x, hw.y },
        br,
        { hw.x, br.y },
        { tl.x, br.y },
        { tl.x, hw.y }
    };

    m_hover_handle = -1;
    for (int i = 0; i < 8; ++i)
    {
        ImVec2 s = ws2px(ws[i]);
        ImVec2 min{ s.x - m_style.HANDLE_BOX_SIZE, s.y - m_style.HANDLE_BOX_SIZE };
        ImVec2 max{ s.x + m_style.HANDLE_BOX_SIZE, s.y + m_style.HANDLE_BOX_SIZE };

        bool hovered = ImGui::IsMouseHoveringRect(min, max);
        if (hovered) m_hover_handle = i;

        dl->AddRectFilled(min, max, hovered ? m_style.ms_HANDLE_HOVER_COLOR
                                            : m_style.ms_HANDLE_COLOR);
    }
    
    return;
}*/



//
//
// *************************************************************************** //
// *************************************************************************** //   END "MAIN HIGHLIGHT".






// *************************************************************************** //
//
//
//
//      4.      "AUXILIARY" HIGHLIGHT RENDERING...
// *************************************************************************** //
// *************************************************************************** //

//  "_render_auxiliary_highlights"
//
inline void Editor::_render_auxiliary_highlights(ImDrawList * dl) const
{
    const BrowserState &            BS              = this->m_browser_S;
    const int &                     pidx            = BS.m_hovered_obj;
    const std::pair<int,int> &      vidx            = BS.m_hovered_vertex;
    //
    const bool                      draw_object     = ( pidx > 0 );
    const bool                      draw_handle     = ( (vidx.first > 0)  &&  (vidx.second > 0) );
    
    
    //      1.      RENDER THE OBJECT...
    if ( draw_object )
    {
        const Path & path   = this->m_paths[ static_cast<size_t>( pidx ) ];
        _auxiliary_highlight_object(path, dl);
    }
    
    
    //      2.      RENDER THE HANDLE...
    if ( draw_handle )
    {
        const Path &        v_path      = this->m_paths[ static_cast<size_t>(vidx.first) ];
        const VertexID      vid         = v_path.verts[ static_cast<size_t>(vidx.second) ];
        const Vertex *      v           = find_vertex_mut( m_vertices, vid );     IM_ASSERT( v != nullptr );
        
        _auxiliary_highlight_handle( *v, dl );
    }
    
    
    
    
    
    
    
    
    BS.ClearAuxiliarySelection();
    
    return;
}


//  "_auxiliary_highlight_object"
//
inline void Editor::_auxiliary_highlight_object(const Path & p, ImDrawList * dl) const
{
    const ImU32 &               col             = ImGui::GetColorU32(ImGuiCol_FrameBgHovered); //  .AUX_HIGHLIGHT_COLOR;
    const float &               w               = this->m_style.AUX_HIGHLIGHT_WIDTH;
    //  const ImU32 &               col         = m_style.AUX_HIGHLIGHT_COLOR;
    //  const float                 w           = p.style.stroke_width + 2.0f;
    //
    //
    //
    const size_t                N               = p.verts.size();
    //
    //
    //  "ws2px"
    auto                        ws2px           = [](ImVec2 w) {
        ImPlotPoint pp = ImPlot::PlotToPixels(ImPlotPoint(w.x, w.y));
        return ImVec2{ static_cast<float>(pp.x), static_cast<float>(pp.y) };
    };
    //
    //  "draw_seg"
    auto                        draw_seg        = [&](const Vertex * a, const Vertex * b)
    {
        if ( !a || !b )         { return; }
        if ( is_curved<VertexID>(a, b) ) {
            ImVec2 P0 = ws2px({ a->x,                       a->y });
            ImVec2 P1 = ws2px({ a->x + a->out_handle.x,     a->y + a->out_handle.y });
            ImVec2 P2 = ws2px({ b->x + b->in_handle.x,      b->y + b->in_handle.y  });
            ImVec2 P3 = ws2px({ b->x,                       b->y });
            dl->AddBezierCubic(P0, P1, P2, P3, col, w, m_style.ms_BEZIER_SEGMENTS);
        }
        else
        {
            dl->AddLine( ws2px({ a->x, a->y }), ws2px({ b->x, b->y }), col, w );
        }
    };



    for (size_t i = 0; i + 1 < N; ++i)
    {
        const Vertex *      a   = find_vertex(m_vertices, p.verts[i]);
        const Vertex *      b   = find_vertex(m_vertices, p.verts[i + 1]);
        draw_seg(a, b);
    }
    if ( p.closed )
    {
        const Vertex *      a   = find_vertex(m_vertices, p.verts.back());
        const Vertex *      b   = find_vertex(m_vertices, p.verts.front());
        draw_seg(a, b);
    }
    
    
    
    return;
}


//  "_auxiliary_highlight_handle"
//
inline void Editor::_auxiliary_highlight_handle(const Vertex & v, ImDrawList * dl) const
{
    const ImU32 &               col             = ImGui::GetColorU32(ImGuiCol_FrameBgHovered);  //  m_style.AUX_HIGHLIGHT_COLOR;
    const float &               w               = this->m_style.AUX_HIGHLIGHT_WIDTH;            //  p.style.stroke_width + 2.0f;
    //
    //  "ws2px"
    auto                        ws2px           = [](ImVec2 w) {
        ImPlotPoint pp = ImPlot::PlotToPixels(ImPlotPoint(w.x, w.y));
        return ImVec2{ static_cast<float>(pp.x), static_cast<float>(pp.y) };
    };
    //
    //  "draw_handle"
    auto                        draw_handle     = [&](const Vertex & v, const ImVec2 & off, const ImVec2 & a)
    {
        if ( (off.x == 0.f)  &&  (off.y == 0.f) )   { return; }
        
        ImVec2 h = ws2px({ v.x + off.x, v.y + off.y });
        
        dl->AddLine( a, h, m_style.ms_HANDLE_COLOR, 1.0f );
        dl->AddRectFilled(
            { h.x - m_style.ms_HANDLE_SIZE, h.y - m_style.ms_HANDLE_SIZE },
            { h.x + m_style.ms_HANDLE_SIZE, h.y + m_style.ms_HANDLE_SIZE },
            m_style.ms_HANDLE_COLOR
        );
    };
    
    
    
    //      2.      DRAW HOVERED VERTEX...
    v.draw_handle( dl, &world_to_pixels );
    
    
    
    //  const ImVec2    a       = ws2px({ v.x, v.y });
    //  draw_handle             ( v, v.out_handle, a );
    //  draw_handle             ( v, v.in_handle,  a );
    
    
    return;
}



//
//
// *************************************************************************** //
// *************************************************************************** //   END "AUXILIARY HIGHLIGHT".

























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

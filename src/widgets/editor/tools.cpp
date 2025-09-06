/***********************************************************************************
*
*       *********************************************************************
*       ****              T O O L S . C P P  ____  F I L E               ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      June 17, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "widgets/editor/editor.h"



namespace cb {  //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//  1.  STATIC CONSTEXPR VALUES...
// *************************************************************************** //
// *************************************************************************** //

// ── Shape-cursor tuning ─────────────────────────────────────
static constexpr ImU32  ms_SHAPE_CURSOR_LINE_COLOR   = IM_COL32(255,255,255,230);
static constexpr ImU32  ms_SHAPE_CURSOR_FILL_COLOR   = IM_COL32(  0,  0,  0,100);
static constexpr float  ms_SHAPE_CURSOR_LINE_WIDTH   = 1.5f;     // px
// ── Shape-cursor fixed size ────────────────────────────────
static constexpr float ms_SHAPE_CURSOR_HALF_SIZE_PX     = 8.0f;   // half-size in pixels
//
//
//
// ── Shape-tool drag → radius tuning ─────────────────────────
static constexpr float ms_SHAPE_RADIUS_DRAG_SCALE       = 1.0f;
static constexpr float ms_SHAPE_RADIUS_MIN              = 1.0f;
static constexpr float ms_SHAPE_RADIUS_MAX              = 250.0f;
//
static constexpr ImU32  ms_SHAPE_PREVIEW_FILL_COLOR    = IM_COL32(255,255,255, 40);
static constexpr ImU32  ms_SHAPE_PREVIEW_LINE_COLOR    = IM_COL32(255,255,255,180);
static constexpr float  ms_SHAPE_PREVIEW_LINE_WIDTH    = 1.6f;

static constexpr float  ms_SHAPE_MIN_DRAG_EPSILON      = 2.0f;      // ignore tiny drags
static constexpr float  ms_SHAPE_ELLIPSE_KAPPA         = 0.5522847498f; // cubic-circle coeff





// *************************************************************************** //
//
//
//
//  2.  SHAPE TOOLL...
// *************************************************************************** //
// *************************************************************************** //

//  "_handle_shape"
//
void Editor::_handle_shape([[maybe_unused]] const Interaction & it)
{
    auto &              shape_entry     = m_residents[Resident::Shape];
    static Overlay *    resident        = m_ov_manager.lookup_resident(shape_entry.id);
    
    
    // ── 0. Spawn overlay the first frame Shape mode is active ───────────
    resident->info.visible              = true;
    

    _draw_shape_cursor(it);        // custom cursor hint

    _shape_begin_anchor(it);
    _shape_update_radius(it);
    _shape_preview_draw(it.dl);

    
    


    if ( m_shape.dragging && ImGui::IsMouseReleased(ImGuiMouseButton_Left) )
        { _shape_commit(); }
 

    return;
}



//      2.1     SHAPE TOOL UTILITIES.
// *************************************************************************** //
// *************************************************************************** //

//  "_shape_begin_anchor"
//
void Editor::_shape_begin_anchor(const Interaction& it)
{
    if (!it.hovered) return;
    if ( !ImGui::IsMouseClicked(ImGuiMouseButton_Left) ) return;

    ImVec2 ws = pixels_to_world(ImGui::GetIO().MousePos);
    if ( want_snap() )      { ws = snap_to_grid(ws); }

    m_shape.press_ws   = ws;
    m_shape.cur_ws     = ws;
    m_shape.dragging   = true;
    m_shape.active     = true;
    m_shape.start_y    = ImGui::GetIO().MousePos.y;      // add this float in ShapeState
    m_shape.start_rad  = m_shape.radius;                 // add this float in ShapeState
}


//  "_shape_update_radius"
//
void Editor::_shape_update_radius([[maybe_unused]] const Interaction & it)
{
    if (!m_shape.dragging) return;

    // current mouse position in pixels
    ImGuiIO &   io              = ImGui::GetIO();
    float       cur_y           = io.MousePos.y;

    // pixels moved since press (positive when dragged downward)
    float       dy_px           = cur_y - m_shape.start_y;

    // convert 1 world-unit (y) to pixels at anchor -> gives px per world
    ImVec2      cen_ws          = m_shape.press_ws;
    ImVec2      cen_px          = world_to_pixels(cen_ws);
    ImVec2      unit_px         = world_to_pixels({cen_ws.x, cen_ws.y + 1.0f});
    float       px_per_world    = std::fabs(unit_px.y - cen_px.y);
    if (px_per_world <= 0.0f) return;                 // safety

    // world units per pixel
    float       world_per_px    = 1.0f / px_per_world;

    // drag up (dy_px < 0) should *increase* radius  →  subtract dy
    float       r_new           = m_shape.start_rad - dy_px * world_per_px * ms_SHAPE_RADIUS_DRAG_SCALE;
    r_new                       = std::clamp(r_new, ms_SHAPE_RADIUS_MIN, ms_SHAPE_RADIUS_MAX);
    m_shape.radius              = r_new;

    // keep current world cursor for preview positioning
    m_shape.cur_ws              = pixels_to_world(io.MousePos);
    
    return;
}


//  "_shape_commit"
//
void Editor::_shape_commit(void)
{
    if (!m_shape.active) return;

    float r = m_shape.radius;
    if (r < ms_SHAPE_MIN_DRAG_EPSILON) { _shape_reset(); return; }

    size_t new_idx = (m_shape.kind == ShapeKind::Rectangle)
                   ? _shape_build_rectangle(m_shape.press_ws, r)
                   : _shape_build_ellipse   (m_shape.press_ws, r);

    this->reset_selection(); //m_sel.clear();
    m_sel.paths.insert(new_idx);
    _rebuild_vertex_selection();
    _shape_reset();
    return;
}


//  "_shape_reset"
//
void Editor::_shape_reset(void)
{
    m_shape.dragging = false;
    m_shape.active   = false;
}





// *************************************************************************** //
//
//
//
//      ?.  SHAPE BUILDER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_shape_add_vertex"
//
Editor::VertexID Editor::_shape_add_vertex(const ImVec2 & ws) {
    uint32_t vid = _add_vertex(ws);
    _add_point_glyph(vid);
    return vid;
}


//  "_shape_build_rectangle"
//
size_t Editor::_shape_build_rectangle(const ImVec2 & cen, float r)
{
    ImVec2 v0{cen.x - r, cen.y - r};
    ImVec2 v1{cen.x + r, cen.y - r};
    ImVec2 v2{cen.x + r, cen.y + r};
    ImVec2 v3{cen.x - r, cen.y + r};

    std::vector<VertexID> ids{
        _shape_add_vertex(v0),
        _shape_add_vertex(v1),
        _shape_add_vertex(v2),
        _shape_add_vertex(v3)
    };

    // ─── 3. Build the Path via central factory ──────────────────────
    [[maybe_unused]] Path &  p   = _make_shape(ids);   // assigns id, label, z_index, flags

    //  Return the index of the newly added path
    return m_paths.size() - 1;
}


//  "_shape_build_ellipse"
//
size_t Editor::_shape_build_ellipse(const ImVec2 & cen, float r)
{
    float       k               = ms_SHAPE_ELLIPSE_KAPPA * r;

    ImVec2      p0              {cen.x - r, cen.y};
    ImVec2      p1              {cen.x, cen.y - r};
    ImVec2      p2              {cen.x + r, cen.y};
    ImVec2      p3              {cen.x, cen.y + r};

    VertexID    v0              = _shape_add_vertex(p0);
    VertexID    v1              = _shape_add_vertex(p1);
    VertexID    v2              = _shape_add_vertex(p2);
    VertexID    v3              = _shape_add_vertex(p3);

    Vertex *    a               = find_vertex_mut(m_vertices, v0);
    Vertex *    b               = find_vertex_mut(m_vertices, v1);
    Vertex *    c               = find_vertex_mut(m_vertices, v2);
    Vertex *    d               = find_vertex_mut(m_vertices, v3);

    a->m_bezier.out_handle      = { 0, -k };
    b->m_bezier.in_handle       = {-k,  0};
    b->m_bezier.out_handle      = { k,  0};
    c->m_bezier.in_handle       = { 0, -k};
    c->m_bezier.out_handle      = { 0,  k};
    d->m_bezier.in_handle       = { k,  0};
    d->m_bezier.out_handle      = {-k,  0};
    a->m_bezier.in_handle       = { 0,  k};



    Path &      p       = _make_path({v0, v1, v2, v3});   // assigns id, label, z_index, flags
    p.closed            = true;
    
    
    return m_paths.size() - 1;
}


























// *************************************************************************** //
//
//
//
//      DEPRECATED STUFF...
// *************************************************************************** //
// *************************************************************************** //

//  "_shape_preview_draw"
//
void Editor::_shape_preview_draw(ImDrawList* dl) const
{
    if ( !m_shape.dragging )    { return; }

    ImVec2      cen_ws          = m_shape.press_ws;
    ImVec2      cen_px          = world_to_pixels(cen_ws);

    //  convert one world-unit in X to pixels → gives px / world
    ImVec2      unit_px         = world_to_pixels({cen_ws.x + 1.0f, cen_ws.y});
    float       px_per_world    = std::fabs(unit_px.x - cen_px.x);

    float       C               = m_shape.radius * px_per_world;                      // half-size in px
    ImU32       fc              = ms_SHAPE_PREVIEW_FILL_COLOR;
    ImU32       lc              = ms_SHAPE_PREVIEW_LINE_COLOR;


    if ( m_shape.kind == ShapeKind::Rectangle )
    {
        dl->AddRectFilled({cen_px.x - C, cen_px.y - C},
                          {cen_px.x + C, cen_px.y + C},
                          fc, 0.0f);
        dl->AddRect({cen_px.x - C, cen_px.y - C},
                    {cen_px.x + C, cen_px.y + C},
                    lc, 0.0f, 0, ms_SHAPE_PREVIEW_LINE_WIDTH);
    }
    else if ( m_shape.kind == ShapeKind::Ellipse )
    {
        dl->AddCircleFilled(cen_px, C, fc, 32);
        dl->AddCircle(cen_px, C, lc, 32, ms_SHAPE_PREVIEW_LINE_WIDTH);
    }
    
    return;
}



//  "_draw_shape_cursor"
//
//  Draws a custom cursor glyph that previews the shape kind.
//  Call every frame from _handle_shape().
void Editor::_draw_shape_cursor(const Interaction & it) const
{
    if ( !it.hovered || it.space )      { return; }            //   skip when panning


    ImGuiIO &       io      = ImGui::GetIO();
    ImGui::SetMouseCursor(ImGuiMouseCursor_None);               // hide OS cursor


    const ImVec2    P       = io.MousePos;
    const float     SZ      = ms_SHAPE_CURSOR_HALF_SIZE_PX;     // constant half-size
    ImDrawList *    dl      = ImGui::GetForegroundDrawList();


    switch ( m_shape.kind )
    {
        case ShapeKind::Rectangle :     {
            dl->AddRectFilled({P.x - SZ, P.y - SZ}, {P.x + SZ, P.y + SZ},
                              ms_SHAPE_CURSOR_FILL_COLOR, 2.0f);
            dl->AddRect({P.x - SZ, P.y - SZ}, {P.x + SZ, P.y + SZ},
                        ms_SHAPE_CURSOR_LINE_COLOR, 2.0f, 0, ms_SHAPE_CURSOR_LINE_WIDTH);
            break;
        }

        case ShapeKind::Ellipse :       {
            dl->AddCircleFilled(P, SZ, ms_SHAPE_CURSOR_FILL_COLOR, 16);
            dl->AddCircle(P, SZ, ms_SHAPE_CURSOR_LINE_COLOR, 16, ms_SHAPE_CURSOR_LINE_WIDTH);
            break;
        }
        
        //  fallback glyph
        default :                       {
            dl->AddLine({P.x - SZ, P.y - SZ}, {P.x + SZ, P.y + SZ},
                        ms_SHAPE_CURSOR_LINE_COLOR, ms_SHAPE_CURSOR_LINE_WIDTH);
            dl->AddLine({P.x - SZ, P.y + SZ}, {P.x + SZ, P.y - SZ},
                        ms_SHAPE_CURSOR_LINE_COLOR, ms_SHAPE_CURSOR_LINE_WIDTH);
            break;
        }
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

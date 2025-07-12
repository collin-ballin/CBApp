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
    static Overlay *    resident        = m_overlays.lookup_resident(shape_entry.id);
    
    
    // ── 0. Spawn overlay the first frame Shape mode is active ───────────
    resident->visible                   = true;
    

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
    Path &  p   = _make_path(ids);   // assigns id, label, z_index, flags
    p.closed    = true;

    //  Return the index of the newly added path
    return m_paths.size() - 1;
}


//  "_shape_build_ellipse"
//
size_t Editor::_shape_build_ellipse(const ImVec2 & cen, float r)
{
    float       k       = ms_SHAPE_ELLIPSE_KAPPA * r;

    ImVec2      p0      {cen.x - r, cen.y};
    ImVec2      p1      {cen.x, cen.y - r};
    ImVec2      p2      {cen.x + r, cen.y};
    ImVec2      p3      {cen.x, cen.y + r};

    VertexID    v0      = _shape_add_vertex(p0);
    VertexID    v1      = _shape_add_vertex(p1);
    VertexID    v2      = _shape_add_vertex(p2);
    VertexID    v3      = _shape_add_vertex(p3);

    Vertex *    a       = find_vertex_mut(m_vertices, v0);
    Vertex *    b       = find_vertex_mut(m_vertices, v1);
    Vertex *    c       = find_vertex_mut(m_vertices, v2);
    Vertex *    d       = find_vertex_mut(m_vertices, v3);

    a->out_handle       = { 0, -k };
    b->in_handle        = {-k,  0};
    b->out_handle       = { k,  0};
    c->in_handle        = { 0, -k};
    c->out_handle       = { 0,  k};
    d->in_handle        = { k,  0};
    d->out_handle       = {-k,  0};
    a->in_handle        = { 0,  k};



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
    if (!m_shape.dragging) return;

    ImVec2      cen_ws          = m_shape.press_ws;
    ImVec2      cen_px          = world_to_pixels(cen_ws);

    //  convert one world-unit in X to pixels → gives px / world
    ImVec2      unit_px         = world_to_pixels({cen_ws.x + 1.0f, cen_ws.y});
    float       px_per_world    = std::fabs(unit_px.x - cen_px.x);

    float       C               = m_shape.radius * px_per_world;                      // half-size in px
    ImU32       fc              = ms_SHAPE_PREVIEW_FILL_COLOR;
    ImU32       lc              = ms_SHAPE_PREVIEW_LINE_COLOR;

    if (m_shape.kind == ShapeKind::Rectangle) {
        dl->AddRectFilled({cen_px.x - C, cen_px.y - C},
                          {cen_px.x + C, cen_px.y + C},
                          fc, 0.0f);
        dl->AddRect({cen_px.x - C, cen_px.y - C},
                    {cen_px.x + C, cen_px.y + C},
                    lc, 0.0f, 0, ms_SHAPE_PREVIEW_LINE_WIDTH);
    } else if (m_shape.kind == ShapeKind::Ellipse) {
        dl->AddCircleFilled(cen_px, C, fc, 32);
        dl->AddCircle(cen_px, C, lc, 32, ms_SHAPE_PREVIEW_LINE_WIDTH);
    }
    
    return;
}



//  "_draw_shape_cursor"
//
//  Draws a custom cursor glyph that previews the shape kind.
//  Call every frame from _handle_shape().
void Editor::_draw_shape_cursor(const Interaction& it) const
{
    if (!it.hovered || it.space) return;            // skip when panning

    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetMouseCursor(ImGuiMouseCursor_None);   // hide OS cursor

    const ImVec2  P = io.MousePos;
    const float   S = ms_SHAPE_CURSOR_HALF_SIZE_PX; // constant half-size

    ImDrawList* dl = ImGui::GetForegroundDrawList();

    switch (m_shape.kind)
    {
        case ShapeKind::Rectangle:
            dl->AddRectFilled({P.x - S, P.y - S}, {P.x + S, P.y + S},
                              ms_SHAPE_CURSOR_FILL_COLOR, 2.0f);
            dl->AddRect({P.x - S, P.y - S}, {P.x + S, P.y + S},
                        ms_SHAPE_CURSOR_LINE_COLOR, 2.0f, 0, ms_SHAPE_CURSOR_LINE_WIDTH);
            break;

        case ShapeKind::Ellipse:
            dl->AddCircleFilled(P, S, ms_SHAPE_CURSOR_FILL_COLOR, 16);
            dl->AddCircle(P, S, ms_SHAPE_CURSOR_LINE_COLOR, 16, ms_SHAPE_CURSOR_LINE_WIDTH);
            break;

        default: // fallback glyph
            dl->AddLine({P.x - S, P.y - S}, {P.x + S, P.y + S},
                        ms_SHAPE_CURSOR_LINE_COLOR, ms_SHAPE_CURSOR_LINE_WIDTH);
            dl->AddLine({P.x - S, P.y + S}, {P.x + S, P.y - S},
                        ms_SHAPE_CURSOR_LINE_COLOR, ms_SHAPE_CURSOR_LINE_WIDTH);
            break;
    }
}






























































// *************************************************************************** //
//
//
//
//  3.  OVERLAY TOOL...
// *************************************************************************** //
// *************************************************************************** //

// *************************************************************************** //
//      3.0.    STATIC OVERLAY HELPERS.
// *************************************************************************** //

namespace overlay {
// *************************************************************************** //

    //  3.0A.   DEFINE COLORS.
    static constexpr ImVec4 INFO_COL {1.00f, 1.00f, 1.00f, 1.0f};
    static constexpr ImVec4 HL_COL   {0.00f, 0.85f, 0.00f, 1.0f};
    static constexpr ImVec4 ACT_COL  {0.20f, 0.60f, 1.00f, 1.0f};
    static constexpr ImVec4 WARN_COL {1.00f, 0.85f, 0.00f, 1.0f};
    static constexpr ImVec4 DNG_COL  {0.90f, 0.15f, 0.15f, 1.0f};
    static constexpr ImVec4 COL_INFO {1.00f, 1.00f, 1.00f, 1.0f};
    static constexpr ImVec4 COL_HL   {0.00f, 0.85f, 0.00f, 1.0f};
    static constexpr ImVec4 COL_ACT  {0.20f, 0.60f, 1.00f, 1.0f};
    static constexpr ImVec4 COL_WARN {1.00f, 0.85f, 0.00f, 1.0f};
    static constexpr ImVec4 COL_DNG  {0.90f, 0.15f, 0.15f, 1.0f};

    //  "overlay_text"
    inline void overlay_text(ImVec4 col, const char * fmt, ...)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        va_list args; va_start(args, fmt);
        ImGui::TextV(fmt, args);
        va_end(args);
        ImGui::PopStyleColor();
    }

    //  Shorthand wrappers
    inline void info     (const char* f, ...) { va_list v; va_start(v,f); overlay_text(INFO_COL, f, v);  va_end(v); }
    inline void highlight(const char* f, ...) { va_list v; va_start(v,f); overlay_text(HL_COL,   f, v); va_end(v); }
    inline void action   (const char* f, ...) { va_list v; va_start(v,f); overlay_text(ACT_COL,  f, v); va_end(v); }
    inline void warn     (const char* f, ...) { va_list v; va_start(v,f); overlay_text(WARN_COL, f, v); va_end(v); }
    inline void danger   (const char* f, ...) { va_list v; va_start(v,f); overlay_text(DNG_COL,  f, v); va_end(v); }

// *************************************************************************** //
}// END "overlay" NAMESPACE.



// *************************************************************************** //
//      3.1.    MAIN OVERLAY API.
// *************************************************************************** //

//  "_handle_overlay"
//
void Editor::_handle_overlay([[maybe_unused]] const Interaction & it)
{
    if (!m_overlay.open)          // master visibility
        return;

    _overlay_update_position();   // sets flags + (alpha,pos)
    if (!_overlay_begin_window()) // Begin failed? bail
        return;

    _overlay_display_main_content(it);
    if (m_overlay.show_details)
        _overlay_display_extra_content(it);

    _overlay_draw_context_menu(); // ↔ toggles show_details, location, alpha
    _overlay_end_window();
}


//  "_overlay_begin_window"
//
bool Editor::_overlay_begin_window()
{
    return ImGui::Begin("Editor Overlay", nullptr, m_overlay.flags);
}


//  "_overlay_end_window"
//
void Editor::_overlay_end_window()
{
    ImGui::End();
}




// *************************************************************************** //
//      3.1.    SUBSIDIARY OVERLAY FUNCTIONS.
// *************************************************************************** //

//  "_overlay_draw_context_menu"
//
void Editor::_overlay_draw_context_menu(void)
{
    if ( ImGui::BeginPopupContextWindow() )
    {
        ImGui::SeparatorText("Settings");
        ImGui::MenuItem("Show details",         nullptr,    &m_overlay.show_details);
        ImGui::MenuItem("Verbose details",      nullptr,    &m_overlay.verbose_detail);
    
        if ( ImGui::BeginMenu("Menu Location") ) {
        //
            if (ImGui::MenuItem("Custom",       nullptr, m_overlay.location == -1))     m_overlay.location  = -1;
            if (ImGui::MenuItem("Center",       nullptr, m_overlay.location == -2))     m_overlay.location  = -2;
            if (ImGui::MenuItem("Top-left",     nullptr, m_overlay.location == 0))      m_overlay.location  = 0;
            if (ImGui::MenuItem("Top-right",    nullptr, m_overlay.location == 1))      m_overlay.location  = 1;
            if (ImGui::MenuItem("Bottom-left",  nullptr, m_overlay.location == 2))      m_overlay.location  = 2;
            if (ImGui::MenuItem("Bottom-right", nullptr, m_overlay.location == 3))      m_overlay.location  = 3;
            ImGui::EndMenu();
        //
        }
        ImGui::SliderFloat("Alpha", &m_overlay.alpha, 0.10f, 1.00f, "Alpha %.2f");
        
        
        ImGui::EndPopup();
    }
    return;
}


//  "_overlay_update_position"
//
void Editor::_overlay_update_position()
{
    m_overlay.flags             = m_overlay.base_flags;
    const ImGuiViewport * vp    = ImGui::GetMainViewport();

    if (m_overlay.location >= 0)
    {
        ImVec2 wp   = vp->WorkPos, ws = vp->WorkSize;
        ImVec2 pos, pivot;
        pos.x       = (m_overlay.location & 1) ? (wp.x + ws.x - m_overlay.pad) : (wp.x + m_overlay.pad);
        pos.y       = (m_overlay.location & 2) ? (wp.y + ws.y - m_overlay.pad) : (wp.y + m_overlay.pad);
        pivot.x     = (m_overlay.location & 1) ? 1.0f : 0.0f;
        pivot.y     = (m_overlay.location & 2) ? 1.0f : 0.0f;
        ImGui::SetNextWindowPos(pos, ImGuiCond_Always, pivot);
        ImGui::SetNextWindowViewport(vp->ID);
        m_overlay.flags |= ImGuiWindowFlags_NoMove;
    }
    else if (m_overlay.location == -2)
    {
        ImGui::SetNextWindowPos(vp->GetCenter(), ImGuiCond_Always, ImVec2(0.5f,0.5f));
        m_overlay.flags |= ImGuiWindowFlags_NoMove;
    }

    ImGui::SetNextWindowBgAlpha(m_overlay.alpha);
}



//  "overlay_log"
//
void Editor::overlay_log(std::string msg, float secs)
{
    m_overlay.log_msg       = std::move(msg);
    m_overlay.log_timer     = secs;
}




// *************************************************************************** //
//      3.2.    MAIN OVERLAY CONTENT.
// *************************************************************************** //

//  "_overlay_display_main_content"
//
void Editor::_overlay_display_main_content([[maybe_unused]] const Interaction& it)
{
    using namespace overlay;
    
    // 1. Pointer position ------------------------------------------------
    ImVec2 origin{ m_p0.x + m_cam.pan.x, m_p0.y + m_cam.pan.y };
    ImVec2 canvas{ ImGui::GetIO().MousePos.x - origin.x,
                   ImGui::GetIO().MousePos.y - origin.y };
    ImVec2 world { canvas.x / m_cam.zoom_mag, canvas.y / m_cam.zoom_mag };

    overlay_text(COL_INFO,
                 "Canvas: (%.0f, %.0f)    World: (%.2f, %.2f)",
                 canvas.x, canvas.y, world.x, world.y);

    // 2. Hover line ------------------------------------------------------
    ImVec4  hov_col = COL_INFO;
    char    hov_buf[128] = "none";

    if (auto hit = _hit_any(it))
    {
        switch (hit->type)
        {
            case Hit::Type::Point:
                std::snprintf(hov_buf,sizeof(hov_buf),
                              "Point: #%zu", hit->index);
                hov_col = COL_HL; break;

            case Hit::Type::Handle:
                std::snprintf(hov_buf,sizeof(hov_buf),
                              "%s  –  v%zu",
                              hit->out ? "Handle-out" : "Handle-in",
                              hit->index);
                hov_col = COL_HL; break;

            case Hit::Type::Path:
                std::snprintf(hov_buf,sizeof(hov_buf),
                              "Path: #%zu", hit->index);
                hov_col = COL_ACT; break;

            case Hit::Type::Line:
                std::snprintf(hov_buf,sizeof(hov_buf),
                              "Line: #%zu", hit->index);
                hov_col = COL_DNG; break;

            default: break;
        }
    }
    overlay_text(hov_col, "Hover: %s", hov_buf);

    // 3. Pen-tool extend cue --------------------------------------------
    if (m_mode == Mode::Pen)
    {
        int glyph_idx = _hit_point(it);
        if (glyph_idx >= 0)
        {
            uint32_t vid = m_points[glyph_idx].v;
            if (auto ep = _endpoint_if_open(vid))
            {
                overlay_text(COL_HL,
                             "Extend: Path #%zu    %s endpoint",
                             ep->path_idx, ep->prepend ? "prepend" : "append");
            }
        }
    }

    // 4. Scissor cut cue -------------------------------------------------
    if (m_mode == Mode::Scissor)
    {
        if (auto h = _hit_any(it);
            h && (h->type == Hit::Type::Path || h->type == Hit::Type::Line))
        {
            overlay_text(COL_DNG,
                         "Cut: %s #%zu    click to split",
                         h->type == Hit::Type::Path ? "Path" : "Line",
                         h->index);
        }
    }

    // 5. Current action --------------------------------------------------
    const char* act = "Idle";
    if (it.space && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f)) act = "Panning";
    else if (it.space)                                                   act = "Pan-ready";
    else if (m_dragging_handle)                                          act = "Handle-drag";
    else if (m_lasso_active)                                             act = "Lasso";
    else if (m_boxdrag.active)                                           act = "BBox-scale";
    else if (m_dragging)                                                 act = "Move-drag";

    overlay_text(COL_ACT, "Action: %s", act);

    // 6. Transient log ---------------------------------------------------
    if (m_overlay.log_timer > 0.0f)
        overlay_text(COL_WARN, "Log: %s", m_overlay.log_msg.c_str());
}


//  "_overlay_display_extra_content"
//
void Editor::_overlay_display_extra_content([[maybe_unused]] const Interaction& it)
{
    using namespace overlay;
    ImGuiIO &           io              = ImGui::GetIO();
    size_t              n_pts           = m_sel.points.size();
    size_t              n_pths          = m_sel.paths.size();
    size_t              n_lns           = m_sel.lines.size();
    size_t              total           = n_pts + n_pths + n_lns;

    // Selection summary --------------------------------------------------
    if (total == 0)
    {
        overlay_text(COL_INFO, "Selection: none");
    }
    else if (total == 1)
    {
        if (n_pts == 1)
        {
            size_t idx = *m_sel.points.begin();
            uint32_t vid = m_points[idx].v;
            overlay_text(COL_INFO, "Point: #%zu    Vertex: %u", idx, vid);
        }
        else if (n_pths == 1)
        {
            size_t idx = *m_sel.paths.begin();
            const Path& p = m_paths[idx];
            overlay_text(COL_INFO, "Path: #%zu", idx);
            overlay_text(COL_INFO, "Vertices: %zu    Closed: %s",
                         p.verts.size(), p.closed ? "yes" : "no");
        }
        else
        {
            size_t idx = *m_sel.lines.begin();
            const Line& ln = m_lines[idx];
            overlay_text(COL_INFO, "Line: #%zu    vA: %u    vB: %u", idx, ln.a, ln.b);
        }
    }
    else
    {
        static constexpr size_t     SIZE            = 128;
        char                        buf[SIZE]       = "";
        
        if (n_pts)      { std::snprintf(buf + std::strlen(buf), sizeof(buf) - std::strlen(buf), "%zu Point%s    ",  n_pts,  n_pts>  1 ? "s" : "");      }
        if (n_pths)     { std::snprintf(buf + std::strlen(buf), sizeof(buf) - std::strlen(buf), "%zu Path%s    ",   n_pths, n_pths> 1 ? "s" : "");      }
        if (n_lns)      { std::snprintf(buf + std::strlen(buf), sizeof(buf) - std::strlen(buf), "%zu Line%s",       n_lns,  n_lns>  1 ? "s" : "");      }
        
        overlay_text(COL_INFO, "Selection: %s", buf);
    }

    // Verbose diagnostics -------------------------------------------------
    if (!m_overlay.show_details) return;

    ImGui::Separator();
    overlay_text(COL_INFO, "Zoom: %.2f", m_cam.zoom_mag);
    overlay_text(COL_INFO, "Scroll: (%.1f, %.1f)", m_cam.pan.x, m_cam.pan.y);
    overlay_text(COL_INFO, "Grid step: %.2f    Snap: %s", m_grid.snap_step, m_grid.snap_on ? "on" : "off");


    {
        char        mods[32]        = "none";
        int         len             = 0;

        if (io.KeyShift) {
            int written = std::snprintf(mods + len, sizeof(mods) - len, "Shift ");
            if (written > 0) len += written;
        }
        if (io.KeyCtrl) {
            int written = std::snprintf(mods + len, sizeof(mods) - len, "Ctrl ");
            if (written > 0) len += written;
        }
        if (io.KeyAlt) {
            int written = std::snprintf(mods + len, sizeof(mods) - len, "Alt ");
            if (written > 0) len += written;
        }
        if (io.KeySuper) {
            int written = std::snprintf(mods + len, sizeof(mods) - len, "Super ");
            if (written > 0) len += written;
        }
        overlay_text(COL_INFO, "Modifiers: %s", mods);
    }
    
    return;
}

// *************************************************************************** //
// *************************************************************************** //   END OVERLAY CONTENT.











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

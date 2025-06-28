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




// *************************************************************************** //
//
//
//
//  2.  SHAPE TOOLL...
// *************************************************************************** //
// *************************************************************************** //

//  "_draw_shape_controls"
//
void Editor::_draw_shape_controls(void)
{
    static const char* SHAPES[] = { "Rectangle", "Ellipse" };

    int kind_idx = static_cast<int>(m_shape.kind);

    ImGui::TextUnformatted("Shape Tool");
    ImGui::Separator();

    if (ImGui::Combo("##shape_kind",
                     &kind_idx,
                     SHAPES,
                     IM_ARRAYSIZE(SHAPES)))
        m_shape.kind = static_cast<ShapeKind>(kind_idx);

    ImGui::SliderFloat("Radius",
                       &m_shape.radius,
                       0.0f, 100.0f, "%.1f");
           
           
           
    return;
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
    
    size_t n_pts  = m_sel.points.size();
    size_t n_pths = m_sel.paths.size();
    size_t n_lns  = m_sel.lines.size();
    size_t total  = n_pts + n_pths + n_lns;

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
        char buf[128] = "";
        if (n_pts)  std::snprintf(buf + std::strlen(buf), sizeof(buf) - std::strlen(buf),
                                  "%zu Point%s    ", n_pts, n_pts>1?"s":"");
        if (n_pths) std::snprintf(buf + std::strlen(buf), sizeof(buf) - std::strlen(buf),
                                  "%zu Path%s    ", n_pths, n_pths>1?"s":"");
        if (n_lns)  std::snprintf(buf + std::strlen(buf), sizeof(buf) - std::strlen(buf),
                                  "%zu Line%s",  n_lns, n_lns>1?"s":"");
        overlay_text(COL_INFO, "Selection: %s", buf);
    }

    // Verbose diagnostics -------------------------------------------------
    if (!m_overlay.show_details) return;

    ImGui::Separator();
    overlay_text(COL_INFO, "Zoom: %.2f", m_cam.zoom_mag);
    overlay_text(COL_INFO, "Scroll: (%.1f, %.1f)", m_cam.pan.x, m_cam.pan.y);
    overlay_text(COL_INFO, "Grid step: %.2f    Snap: %s",
                 m_grid.snap_step, m_grid.snap_on ? "on" : "off");

    ImGuiIO& io = ImGui::GetIO();
    char mods[32] = "none";
    int len = 0;
    if (io.KeyShift) len += std::sprintf(mods + len, "Shift ");
    if (io.KeyCtrl)  len += std::sprintf(mods + len, "Ctrl ");
    if (io.KeyAlt)   len += std::sprintf(mods + len, "Alt ");
    if (io.KeySuper) len += std::sprintf(mods + len, "Super ");
    overlay_text(COL_INFO, "Modifiers: %s", mods);
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

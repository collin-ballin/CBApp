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

    //  "text"
    inline void text(ImVec4 col, const char * fmt, ...) {
        va_list args; va_start(args, fmt);
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        ImGui::TextV(fmt, args);
        ImGui::PopStyleColor();
        va_end(args);
    }

    //  Shorthand wrappers
    inline void info     (const char* f, ...) { va_list v; va_start(v,f); text(INFO_COL, f, v);  va_end(v); }
    inline void highlight(const char* f, ...) { va_list v; va_start(v,f); text(HL_COL,   f, v); va_end(v); }
    inline void action   (const char* f, ...) { va_list v; va_start(v,f); text(ACT_COL,  f, v); va_end(v); }
    inline void warn     (const char* f, ...) { va_list v; va_start(v,f); text(WARN_COL, f, v); va_end(v); }
    inline void danger   (const char* f, ...) { va_list v; va_start(v,f); text(DNG_COL,  f, v); va_end(v); }

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
    // Canvas / World ----------------------------------------------------
    ImVec2 w{ it.canvas.x / m_zoom, it.canvas.y / m_zoom };
    ImGui::Text("Canvas: (%.0f, %.0f)    World: (%.0f, %.0f)",
                it.canvas.x, it.canvas.y, w.x, w.y);

    // Hovered object ----------------------------------------------------
    std::string hover = "none";
    if (auto h = _hit_any(it)) {
        switch (h->type) {
            case Hit::Type::Point:  hover = "Point: #"  + std::to_string(h->index); break;
            case Hit::Type::Handle: hover = (h->out ? "Handle-out: v" : "Handle-in: v")
                                           + std::to_string(h->index);              break;
            case Hit::Type::Path:   hover = "Path: #"   + std::to_string(h->index); break;
            case Hit::Type::Line:   hover = "Line: #"   + std::to_string(h->index); break;
            default: break;
        }
    }
    ImGui::Text("Hover: %s", hover.c_str());

    // Pen “extend” cue (green) -----------------------------------------
    if (m_mode == Mode::Pen) {
        int pt_idx = _hit_point(it);
        if (pt_idx >= 0) {
            uint32_t vid = m_points[pt_idx].v;
            if (auto ep = _endpoint_if_open(vid)) {
                const char* dir = ep->prepend ? "prepend" : "append";
                ImGui::TextColored(ImVec4(0.0f, 0.9f, 0.0f, 1.0f),
                                   "Extend: Path #%zu    %s endpoint",
                                   ep->path_idx, dir);
            }
        }
    }

    // Scissor cut preview (red) ----------------------------------------
    if (m_mode == Mode::Scissor)
    {
        if ( auto h = _hit_any(it) )
        {
            if ( h->type == Hit::Type::Path )
            {
                ImGui::TextColored(ImVec4(0.9f, 0.0f, 0.0f, 1.0f),
                                   "Cut: Path #%zu    click to split",
                                   h->index);
            }
            else if ( h->type == Hit::Type::Line )
            {
                ImGui::TextColored(ImVec4(0.9f, 0.0f, 0.0f, 1.0f),
                                   "Cut: Line #%zu    click to split",
                                   h->index);
            }
        }
    }

    // Current high-level action ----------------------------------------
    std::string act = "Idle";
    if (it.space && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f))  act = "Panning";
    else if (it.space)                                                    act = "Pan-ready";
    else if (m_dragging_handle)                                           act = "Handle-drag";
    else if (m_lasso_active)                                              act = "Lasso";
    else if (m_boxdrag.active)                                            act = "BBox-scale";
    else if (m_dragging)                                                  act = "Move-drag";

    ImGui::Text("Action: %s", act.c_str());
}


//  "_overlay_display_extra_content"
//
void Editor::_overlay_display_extra_content([[maybe_unused]] const Interaction& it)
{
    // --- selection summary (as before) --------------------------------
    size_t      n_pts       = m_sel.points.size();
    size_t      n_pths      = m_sel.paths.size();
    size_t      n_lns       = m_sel.lines.size();
    size_t      total       = n_pts + n_pths + n_lns;

    if (total == 0)
    {
        overlay::info("Selection: none");
    }
    else if (total == 1)
    {
        if (n_pts == 1)
        {
            size_t idx = *m_sel.points.begin();
            uint32_t vid = m_points[idx].v;
            overlay::info("Point: #%zu    Vertex ID: %u", idx, vid);
        }
        else if (n_pths == 1)
        {
            size_t idx = *m_sel.paths.begin();
            const Path& p = m_paths[idx];
            overlay::info("Path: #%zu", idx);
            overlay::info("Vertices: %zu    Closed: %s",
                          p.verts.size(), p.closed ? "yes" : "no");
        }
        else
        {
            size_t idx = *m_sel.lines.begin();
            const Line& ln = m_lines[idx];
            overlay::info("Line: #%zu    vA: %u    vB: %u", idx, ln.a, ln.b);
        }
    }
    else
    {
        std::string summary;
        if (n_pts)  summary += std::to_string(n_pts)  + (n_pts  > 1 ? " Points" : " Point");
        if (n_pths) summary += (summary.empty() ? "" : "    ") +
                               std::to_string(n_pths) + (n_pths > 1 ? " Paths"  : " Path");
        if (n_lns)  summary += (summary.empty() ? "" : "    ") +
                               std::to_string(n_lns)  + (n_lns  > 1 ? " Lines"  : " Line");

        overlay::info("Selection: %s", summary.c_str());
    }


    // --- verbose diagnostics -----------------------------------------
    if (!m_overlay.show_details || !m_overlay.verbose_detail)
        return;


    ImGui::SeparatorText("Diagnostics");
    ImGui::Text("Zoom: %.2f", m_zoom);
    ImGui::Text("Scroll: (%.1f, %.1f)", m_scroll.x, m_scroll.y);
    ImGui::Text("Grid step: %.2f    Snap: %s",
                m_grid.world_step, this->want_snap() ? "on" : "off");

    //  Modifier keys
    ImGuiIO& io = ImGui::GetIO();
    std::string mods;
    if (io.KeyShift)  mods += "Shift ";
    if (io.KeyCtrl)   mods += "Ctrl ";
    if (io.KeyAlt)    mods += "Alt ";
    if (io.KeySuper)  mods += "Super ";
    if (mods.empty()) mods = "none";
    ImGui::Text("Modifiers: %s", mods.c_str());
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

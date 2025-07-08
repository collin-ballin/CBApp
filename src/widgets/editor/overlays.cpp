/***********************************************************************************
*
*       *********************************************************************
*       ****           O V E R L A Y S . C P P  ____  F I L E            ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      June 25, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "widgets/editor/_overlays.h"



namespace cb {  //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//  CREATION AND DESTRUCTION...
// *************************************************************************** //
// *************************************************************************** //

//  "create_overlay"
//
OverlayManager::OverlayID OverlayManager::create_overlay(const OverlayCFG & cfg)
{
    if (!cfg.draw_fn) return 0;             // guard against empty callback
    Overlay ov;
    ov.id  = m_next_id++;
    ov.cfg = std::move(cfg);                // move preserves draw_fn
    m_windows.push_back(std::move(ov));
    return m_windows.back().id;
}


//  "destroy_overlay"
//
void OverlayManager::destroy_overlay(OverlayID id)
{
    m_windows.erase(std::remove_if(m_windows.begin(), m_windows.end(),
                                   [id](const Overlay& o){ return o.id == id; }),
                    m_windows.end());
}


//  "add_resident"
//
OverlayManager::OverlayID OverlayManager::add_resident(const OverlayCFG& cfg)
{
    Overlay ov;
    ov.id  = m_next_id++;
    ov.cfg = cfg;
    m_residents.push_back(std::move(ov));
    return m_residents.back().id;
}


//  "resident"
//
OverlayManager::Overlay * OverlayManager::resident(OverlayID id)
{
    for (auto& ov : m_residents)
        if (ov.id == id) return &ov;
    return nullptr;
}



// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //


//  "Begin"
//
void OverlayManager::Begin(const std::function<ImVec2(ImVec2)> & world_to_px, ImVec2 cursor_px, const ImRect & plot_rect)
{
    // ---------- render always-present overlays ----------
    for (auto& ov : m_residents)
        if (ov.visible && ov.cfg.draw_fn)
            _render_overlay(ov, world_to_px, cursor_px, plot_rect);

    // ---------- render & purge dynamic overlays ----------
    for (auto& ov : m_windows)
        if (ov.visible && ov.cfg.draw_fn)
            _render_overlay(ov, world_to_px, cursor_px, plot_rect);

    m_windows.erase(std::remove_if(m_windows.begin(), m_windows.end(),
                    [](const Overlay& o){ return !o.visible; }),
                    m_windows.end());
}










// *************************************************************************** //
//
//
//      PROTECTED FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_draw_context_menu"
//
void OverlayManager::_draw_context_menu(Overlay& ov)
{
    auto emit_item = [&](const char* label, OverlayPlacement p)
    {
        bool sel = (ov.cfg.placement == p);
        if (ImGui::MenuItem(label, nullptr, sel))
            ov.cfg.placement = p;
    };



    // Open when user right–clicks anywhere on the overlay window
    if (ImGui::BeginPopupContextWindow())
    {
        ImGui::SeparatorText("Overlay Settings");

        // ── placement submenu ---------------------------------------------------
        if (ImGui::BeginMenu("Placement"))
        {
            emit_item("Screen - Custom",        OverlayPlacement::ScreenXY);
            emit_item("Follow Cursor",          OverlayPlacement::Cursor);
            emit_item("World-Space Pin",        OverlayPlacement::World);
            ImGui::Separator();
            emit_item("Top-Left",               OverlayPlacement::CanvasTL);
            emit_item("Top-Right",              OverlayPlacement::CanvasTR);
            emit_item("Bottom-Left",            OverlayPlacement::CanvasBL);
            emit_item("Bottom-Right",           OverlayPlacement::CanvasBR);
            ImGui::Separator();
            emit_item("Canvas Point",           OverlayPlacement::CanvasPoint);

            ImGui::EndMenu();
        }

        // ── alpha slider --------------------------------------------------------
        ImGui::SliderFloat("Alpha", &ov.cfg.alpha, 0.10f, 1.00f, "%.2f");
        
        // World-space point tweaker (only when placement == CanvasPoint)
        if (ov.cfg.placement == OverlayPlacement::CanvasPoint) {
            ImGui::SeparatorText("Anchor (world-space)");
            ImGui::DragFloat2("X/Y", &ov.cfg.anchor_ws.x,
                              /*speed*/ 1.0f,
                              /*min*/   -10000.0f,
                              /*max*/    10000.0f,
                              "(%.1f, %.1f)");
        }

        ImGui::EndPopup();
    }
}



//  "_render_overlay"
//
void OverlayManager::_render_overlay(
        Overlay &                       ov,
        const std::function<ImVec2(ImVec2)>& world_to_px,
        ImVec2                         cursor_px,
        const ImRect&                  plot_rect)
{
#ifdef __CBAPP_DEBUG__
    IM_ASSERT( ov.cfg.draw_fn != nullptr && "Render function \"draw_fn\" for Overlay cannot be NULL" );
#endif  //  __CBAPP_DEBUG__  //


    //  1.  Resolve anchor position ------------------------------------------------
    auto    [pos, pivot]    = this->_anchor_to_pos(ov, world_to_px, cursor_px, plot_rect);



    //  4.  Spawn ImGui window ------------------------------------------------------
    const ImGuiViewport* vp = ImGui::GetMainViewport();     // ① main OS window
    ImGui::SetNextWindowViewport(vp->ID);                   // ② attach overlay to it

    ImGui::SetNextWindowBgAlpha(ov.cfg.alpha);
    ImGui::SetNextWindowPos(pos, ImGuiCond_Always, pivot);
    ImGui::Begin( ("##EditorOverlay_" + std::to_string(ov.id)).c_str(), nullptr, ov.flags );

        ov.cfg.draw_fn();               // tool-supplied widgets

        //  Post-clamp: ensure whole window stays in canvas
        if (ov.cfg.placement != OverlayPlacement::Cursor) {
            ImVec2      win_pos         = ImGui::GetWindowPos();
            ImVec2      win_size        = ImGui::GetWindowSize();

            ImVec2      min_allowed     = plot_rect.Min;
            ImVec2      max_allowed     = { plot_rect.Max.x - win_size.x, plot_rect.Max.y - win_size.y };

            ImVec2 clamped {
                std::clamp(win_pos.x, min_allowed.x, max_allowed.x),
                std::clamp(win_pos.y, min_allowed.y, max_allowed.y)
            };

            if (clamped.x != win_pos.x || clamped.y != win_pos.y)
                ImGui::SetWindowPos(clamped, ImGuiCond_Always);
        }

    this->_draw_context_menu(ov);      // ← add this line
        
    ImGui::End();
}










// *************************************************************************** //
//
//
//
//  HELPERS...
// *************************************************************************** //
// *************************************************************************** //

//  "_anchor_to_pos"
//
[[nodiscard]]
std::pair<ImVec2, ImVec2> OverlayManager::_anchor_to_pos(
        Overlay &                               ov,
        const std::function<ImVec2(ImVec2)> &   world_to_px,
        ImVec2                                  cursor_px,
        const ImRect &                          plot_rect)
{
    ImVec2          pos         = ImVec2(-1, -1);
    ImVec2          pivot       = ImVec2(0.0f, 0.0f);           // TL default
    //  const ImVec2 &  tl          = plot_rect.Min;   // convenience
    //  const ImVec2 &  br          = plot_rect.Max;
    
    //  NEW: treat anchor_px as an absolute inset (always ≥ 0)
    const float     ax          = std::fabs(ov.cfg.anchor_px.x);
    const float     ay          = std::fabs(ov.cfg.anchor_px.y);
    
    
    
    //  1.  Resolve anchor position ------------------------------------------------
    switch (ov.cfg.placement)
    {
        case OverlayPlacement::ScreenXY:        {
            pos     = ov.cfg.anchor_px;
            break;
        }
        
        case OverlayPlacement::Cursor:          {
            pos     = { cursor_px.x + ov.cfg.anchor_px.x, cursor_px.y + ov.cfg.anchor_px.y };
            break;
        }
        case OverlayPlacement::World:           {
            pos     = world_to_px(ov.cfg.anchor_ws);
            break;
        }
        
        case OverlayPlacement::CanvasTL:        {
            pos     = { plot_rect.Min.x + ax, plot_rect.Min.y + ay };
            break;
        }
        
        case OverlayPlacement::CanvasTR:        {
            pos     = { plot_rect.Max.x - ax, plot_rect.Min.y + ay };
            break;
        }
        
        case OverlayPlacement::CanvasBL:        {
            pos     = { plot_rect.Min.x + ax, plot_rect.Max.y - ay };
            break;
        }
        
        case OverlayPlacement::CanvasBR:        {
            pos     = { plot_rect.Max.x - ax, plot_rect.Max.y - ay };
            break;
        }
    //
    //
    //
        case OverlayPlacement::CanvasPoint:     {
            pos = world_to_px(ov.cfg.anchor_ws) + ov.cfg.anchor_px;   // pixel offset
            // pivot stays {0,0} so pos is TL unless we add a pivot field later
            break;
        }
    }
    

    //  2.  Pick pivot so <pos> corresponds to requested window-corner --------------
    switch (ov.cfg.placement) {
        case OverlayPlacement::CanvasTR: pivot = {1.0f, 0.0f}; break;
        case OverlayPlacement::CanvasBL: pivot = {0.0f, 1.0f}; break;
        case OverlayPlacement::CanvasBR: pivot = {1.0f, 1.0f}; break;
        default: break; // keep (0,0)
    }

    //  3.  Rough pre-clamp (keeps pivot inside canvas) -----------------------------
    if (ov.cfg.placement != OverlayPlacement::Cursor) {
        pos.x = std::clamp(pos.x, plot_rect.Min.x, plot_rect.Max.x);
        pos.y = std::clamp(pos.y, plot_rect.Min.y, plot_rect.Max.y);
    }
    
    return { pos, pivot };
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

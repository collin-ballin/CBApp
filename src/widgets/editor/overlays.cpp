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





// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //


//  "Begin"
//
void OverlayManager::Begin(const std::function<ImVec2(ImVec2)> & world_to_px, ImVec2 cursor_px, const ImRect& canvas_rect)
{
    for (auto& ov : m_windows) {
        if (!ov.visible || !ov.cfg.draw_fn) continue;
        _render_overlay(ov, world_to_px, cursor_px, canvas_rect);
    }

    // purge retired overlays
    m_windows.erase(std::remove_if(m_windows.begin(),
                                   m_windows.end(),
                                   [](const Overlay& o){ return !o.visible; }),
                    m_windows.end());
}










// *************************************************************************** //
//
//
//      PROTECTED FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //


//  "_anchor_to_pos"
//
[[nodiscard]]
std::pair<ImVec2, ImVec2> OverlayManager::_anchor_to_pos(
        Overlay &                               ov,
        const std::function<ImVec2(ImVec2)> &   world_to_px,
        ImVec2                                  cursor_px,
        const ImRect &                          canvas_rect)
{
    ImVec2          pos         = ImVec2(-1, -1);
    ImVec2          pivot       = ImVec2(0.0f, 0.0f);           // TL default
    
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
            pos     = { canvas_rect.Min.x + ax, canvas_rect.Min.y + ay };
            break;
        }
        
        case OverlayPlacement::CanvasTR:        {
            pos     = { canvas_rect.Max.x - ax, canvas_rect.Min.y + ay };
            break;
        }
        
        case OverlayPlacement::CanvasBL:        {
            pos     = { canvas_rect.Min.x + ax, canvas_rect.Max.y - ay };
            break;
        }
        
        case OverlayPlacement::CanvasBR:        {
            pos     = { canvas_rect.Max.x - ax, canvas_rect.Max.y - ay };
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
        pos.x = std::clamp(pos.x, canvas_rect.Min.x, canvas_rect.Max.x);
        pos.y = std::clamp(pos.y, canvas_rect.Min.y, canvas_rect.Max.y);
    }
    
    return { pos, pivot };
}




//  "_render_overlay"
//
void OverlayManager::_render_overlay(
        Overlay &                       ov,
        const std::function<ImVec2(ImVec2)>& world_to_px,
        ImVec2                         cursor_px,
        const ImRect&                  canvas_rect)
{
    //  1.  Resolve anchor position ------------------------------------------------
    auto    [pos, pivot]    = this->_anchor_to_pos(ov, world_to_px, cursor_px, canvas_rect);



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

            ImVec2      min_allowed     = canvas_rect.Min;
            ImVec2      max_allowed     = { canvas_rect.Max.x - win_size.x, canvas_rect.Max.y - win_size.y };

            ImVec2 clamped {
                std::clamp(win_pos.x, min_allowed.x, max_allowed.x),
                std::clamp(win_pos.y, min_allowed.y, max_allowed.y)
            };

            if (clamped.x != win_pos.x || clamped.y != win_pos.y)
                ImGui::SetWindowPos(clamped, ImGuiCond_Always);
        }

    ImGui::End();
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

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
    const bool  canvas_anchor       = ov.cfg.placement != OverlayPlacement::Custom &&
                                      ov.cfg.placement != OverlayPlacement::Cursor &&
                                      ov.cfg.placement != OverlayPlacement::World;
             
    //  "emit_item"
    auto        emit_item           = [&](const char * label, OverlayPlacement p) {
        bool sel = (ov.cfg.placement == p);
        if (ImGui::MenuItem(label, nullptr, sel))
            ov.cfg.placement = p;
    };
    
    //  "emit_anchor"
    auto        emit_anchor         = [&](const char * label, BBoxAnchor a) {
        bool sel = (ov.cfg.src_anchor == a);
        if ( ImGui::MenuItem(label, nullptr, sel) )     { ov.cfg.src_anchor = a; }
    };
    
    //  "emit_offscreen"
    auto        emit_offscreen      = [&](const char * label, OffscreenPolicy policy) {
        bool sel = (ov.cfg.offscreen == policy);
        if ( ImGui::MenuItem(label, nullptr, sel) )     { ov.cfg.offscreen = policy; }
    };




    // Open when user right–clicks anywhere on the overlay window
    if ( ImGui::BeginPopupContextWindow() )
    {
        ImGui::SeparatorText("Overlay Window Settings");
        
        
        
        
        ImGui::TextDisabled("State");

        //  1.  DESTINATION ANCHOR...
        if ( ImGui::BeginMenu("Destination Anchor") )
        {
            ImGui::TextDisabled("World");
            emit_item("Custom",                 OverlayPlacement::Custom);
            emit_item("World-Space Pin",        OverlayPlacement::World);
            emit_item("Follow Cursor",          OverlayPlacement::Cursor);
            //
            ImGui::Separator();
            //
            ImGui::TextDisabled("Canvas");
            emit_item("Fixed Point",            OverlayPlacement::CanvasPoint);
            emit_item("Top-Left",               OverlayPlacement::CanvasTL);
            emit_item("Top-Right",              OverlayPlacement::CanvasTR);
            emit_item("Bottom-Left",            OverlayPlacement::CanvasBL);
            emit_item("Bottom-Right",           OverlayPlacement::CanvasBR);

            ImGui::EndMenu();
        }
        
        //  2.  SOURCE ANCHOR...
        if ( ImGui::BeginMenu("Source Anchor") )
        {
            emit_anchor("Center",               Anchor::Center);
            emit_anchor("North",                Anchor::North);
            emit_anchor("North-East",           Anchor::NorthEast);
            emit_anchor("East",                 Anchor::East);
            emit_anchor("South-East",           Anchor::SouthEast);
            emit_anchor("South",                Anchor::South);
            emit_anchor("South-West",           Anchor::SouthWest);
            emit_anchor("West",                 Anchor::West);
            emit_anchor("North-West",           Anchor::NorthWest);
            ImGui::EndMenu();
        }
        
        //  3.  OFFSCREEN POLICY...
        if ( ImGui::BeginMenu("Offscreen Policy") )
        {
            emit_offscreen("Hide",              OffscreenPolicy::Hide);
            emit_offscreen("Clamp",             OffscreenPolicy::Clamp);
            ImGui::EndMenu();
        }
        
        
        //  4.  OFFSET POINT...
        ImGui::Separator();
        ImGui::TextDisabled("Position");
        
        
        if ( canvas_anchor) {
            if ( ImGui::BeginMenu("Offset") ) {
                utl::LeftLabelSimple("X:");
                ImGui::SliderFloat("##OverlayManager_CanvasOffsetX",     &ov.cfg.anchor_px.x,    -100,   100,    "%.0f");
                //
                utl::LeftLabelSimple("Y:");
                ImGui::SliderFloat("##OverlayManager_CanvasOffsetY",     &ov.cfg.anchor_px.y,    -100,   100,    "%.0f");
                ImGui::EndMenu();
            }
        }
        
        
        //  5.  CANVAS POINT ADJUSTMENT... (only when placement == CanvasPoint)...
        if (ov.cfg.placement == OverlayPlacement::CanvasPoint) {
        
            if ( ImGui::BeginMenu("Canvas Point") ) {
                ImGui::SeparatorText("Canvas Position");
                utl::LeftLabelSimple("X:");
                ImGui::SliderFloat("##OverlayManager_CanvasCoordX",     &ov.cfg.anchor_ws.x,    -25e2,   5e3,    "%.1f");
                //
                utl::LeftLabelSimple("Y:");
                ImGui::SliderFloat("##OverlayManager_CanvasCoordY",     &ov.cfg.anchor_ws.y,    -25e2,   5e3,    "%.1f");
            ImGui::EndMenu();
            }
        }


        //  6.  TRANSPARENCY / ALPHA SLIDER...
        utl::LeftLabelSimple("Alpha:");
        ImGui::SliderFloat("##OverlayManager_Alpha", &ov.cfg.alpha, 0.10f, 1.00f, "%.2f");
        
        

        ImGui::EndPopup();
    }
    
    return;
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
    IM_ASSERT(ov.cfg.draw_fn && "Overlay draw_fn must not be null");
#endif

    // ─────────────────────────────────────────────────────────────── 1. anchor → pos,pivot
    ImVec2 pos, pivot;
    std::tie(pos, pivot) = _anchor_to_pos(ov, world_to_px, cursor_px, plot_rect);

    // keep raw anchor-px for off-screen test when CanvasPoint
    const ImVec2 anchor_px =
        (ov.cfg.placement == OverlayPlacement::CanvasPoint)
        ? world_to_px(ov.cfg.anchor_ws)
        : ImVec2{0,0};

    // ─────────────────────────────────────────────────────────────── 2. Hide / Clamp policy
    if (ov.cfg.placement == OverlayPlacement::CanvasPoint)
    {
        bool inside =
            (anchor_px.x >= plot_rect.Min.x && anchor_px.x <= plot_rect.Max.x &&
             anchor_px.y >= plot_rect.Min.y && anchor_px.y <= plot_rect.Max.y);

        if (!inside && ov.cfg.offscreen == OffscreenPolicy::Hide)
            return;                                     // skip draw this frame
        // Clamp handled after window size is known (section 6)
    }

    // ─────────────────────────────────────────────────────────────── 3. window flags + pos
    ImGuiWindowFlags win_flags = ov.flags;
    if (ov.cfg.placement == OverlayPlacement::Custom)
        win_flags &= ~ImGuiWindowFlags_NoMove;
    else
        win_flags |=  ImGuiWindowFlags_NoMove;

    ImGuiCond cond = (ov.cfg.placement == OverlayPlacement::Custom)
                     ? ImGuiCond_Once
                     : ImGuiCond_Always;

    const ImGuiViewport* vp = ImGui::GetMainViewport();
    if (ov.cfg.placement != OverlayPlacement::Custom)
        ImGui::SetNextWindowViewport(vp->ID);

    ImGui::SetNextWindowBgAlpha(ov.cfg.alpha);
    ImGui::SetNextWindowPos(pos, cond, pivot);

    const std::string win_name = "##EditorOverlay_" + std::to_string(ov.id);

    // ─────────────────────────────────────────────────────────────── 4. begin, draw
    ImGui::Begin(win_name.c_str(), nullptr, win_flags);
        ov.cfg.draw_fn();
        _draw_context_menu(ov);
    ImGui::End();

    ImGuiWindow* win = ImGui::FindWindowByName(win_name.c_str());
    if (!win) return;

    // ─────────────────────────────────────────────────────────────── 5. persist drag (Custom)
    if (ov.cfg.placement == OverlayPlacement::Custom &&
        !ImGui::IsMouseDown(ImGuiMouseButton_Left))
        ov.cfg.anchor_px = win->Pos;

    // ─────────────────────────────────────────────────────────────── 6. Clamp window rect
    if (ov.cfg.placement == OverlayPlacement::CanvasPoint &&
        ov.cfg.offscreen  == OffscreenPolicy::Clamp)
    {
        ImVec2 min_allowed = plot_rect.Min;
        ImVec2 max_allowed = plot_rect.Max;

        ImVec2 win_min  = win->Pos;
        ImVec2 win_max  = { win->Pos.x + win->Size.x,
                            win->Pos.y + win->Size.y };

        ImVec2 delta{0.0f, 0.0f};

        if (win_min.x < min_allowed.x)                 // left face
            delta.x = min_allowed.x - win_min.x;
        else if (win_max.x > max_allowed.x)            // right face
            delta.x = max_allowed.x - win_max.x;

        if (win_min.y < min_allowed.y)                 // top face
            delta.y = min_allowed.y - win_min.y;
        else if (win_max.y > max_allowed.y)            // bottom face
            delta.y = max_allowed.y - win_max.y;

        if (delta.x != 0.f || delta.y != 0.f)
            win->Pos = { win->Pos.x + delta.x, win->Pos.y + delta.y };
    }
    else if (ov.cfg.placement != OverlayPlacement::Cursor &&
             ov.cfg.placement != OverlayPlacement::Custom)
    {
        // original clamp for other placements (top-left clamp)
        ImVec2 max_pos{ plot_rect.Max.x - win->Size.x,
                        plot_rect.Max.y - win->Size.y };
        win->Pos.x = std::clamp(win->Pos.x, plot_rect.Min.x, max_pos.x);
        win->Pos.y = std::clamp(win->Pos.y, plot_rect.Min.y, max_pos.y);
    }
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
    ImVec2          pos{};
    ImVec2          pivot = anchor_to_pivot(ov.cfg.src_anchor);
    
    //  ImVec2          pos         = ImVec2(-1, -1);
    //  ImVec2          pivot       = ImVec2(0.0f, 0.0f);           // TL default
    //  const ImVec2 &  tl          = plot_rect.Min;   // convenience
    //  const ImVec2 &  br          = plot_rect.Max;
    
    //  NEW: treat anchor_px as an absolute inset (always ≥ 0)
    const float     ax          = std::fabs(ov.cfg.anchor_px.x);
    const float     ay          = std::fabs(ov.cfg.anchor_px.y);
    
    
    
    //  1.  Resolve anchor position ------------------------------------------------
    switch (ov.cfg.placement)
    {
        case OverlayPlacement::Custom:          { pos = ov.cfg.anchor_px; break; }
        case OverlayPlacement::Cursor:          { pos = { cursor_px.x + ov.cfg.anchor_px.x, cursor_px.y + ov.cfg.anchor_px.y }; break; }
        case OverlayPlacement::World:           { pos = world_to_px(ov.cfg.anchor_ws); break; }
    //
    //
    //
        case OverlayPlacement::CanvasPoint:     { pos = world_to_px(ov.cfg.anchor_ws) + ov.cfg.anchor_px; break; }   // pixel offset // pivot stays {0,0} so pos is TL unless we add a pivot field later
        case OverlayPlacement::CanvasTL:        { pos = { plot_rect.Min.x + ax, plot_rect.Min.y + ay }; break; }
        case OverlayPlacement::CanvasTR:        { pos = { plot_rect.Max.x - ax, plot_rect.Min.y + ay }; break; }
        case OverlayPlacement::CanvasBL:        { pos = { plot_rect.Min.x + ax, plot_rect.Max.y - ay }; break; }
        case OverlayPlacement::CanvasBR:        { pos = { plot_rect.Max.x - ax, plot_rect.Max.y - ay }; break; }
    //
    }
    

    //  2.  Pick pivot so <pos> corresponds to requested window-corner --------------
    //  switch (ov.cfg.placement) {
    //      case OverlayPlacement::CanvasTR: pivot = {1.0f, 0.0f}; break;
    //      case OverlayPlacement::CanvasBL: pivot = {0.0f, 1.0f}; break;
    //      case OverlayPlacement::CanvasBR: pivot = {1.0f, 1.0f}; break;
    //      default: break; // keep (0,0)
    //  }

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

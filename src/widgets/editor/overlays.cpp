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
#include "widgets/editor/_types.h"



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
    if ( !cfg.draw_fn )     { return 0; }             //    guard against empty callback
    Overlay     ov;
    
    
    //  ov.info.id      = m_next_id++;
    ov.cfg          = std::move( cfg );                //  move preserves draw_fn
    this->_init_resident_overlay(ov);
    
    
    m_windows.push_back( std::move(ov) );
    
    return m_windows.back().info.id;
}


//  "destroy_overlay"
//
void OverlayManager::destroy_overlay(OverlayID id)
{
    m_windows.erase(std::remove_if(m_windows.begin(), m_windows.end(),
                                   [id](const Overlay& o){ return o.info.id == id; }),
                    m_windows.end());
}


//  "add_resident"
//
OverlayManager::OverlayID OverlayManager::add_resident(const OverlayCFG & cfg)
{
    Overlay             ov;
    //  ov.info.id          = m_next_id++;
    ov.cfg              = cfg;
    this->_init_resident_overlay(ov);
    
    m_residents.push_back(std::move(ov));
    return m_residents.back().info.id;
}

//  "add_resident"
OverlayManager::OverlayID OverlayManager::add_resident(const OverlayCFG & cfg, const OverlayStyle & style) {
    Overlay             ov;
    
    //  ov.info.id              = m_next_id++;
    //  ov.info.window_name     = "##EditorOverlay_" + std::to_string(ov.info.id);
    ov.cfg                  = cfg;
    ov.style                = style;
    this->_init_resident_overlay(ov);
    
    m_residents.push_back( std::move(ov) );
    return m_residents.back().info.id;
}


//  "add_resident"
//  OverlayManager::OverlayID OverlayManager::add_resident(const OverlayCFG & cfg, const OverlayStyle & style, const OverlayInfo & info) {
//      Overlay             ov;
//
//      ov.info             = info;
//      ov.info.id          = m_next_id++;
//
//      ov.cfg              = cfg;
//      ov.style            = style;
//      m_residents.push_back( std::move(ov) );
//
//      return m_residents.back().info.id;
//  }


    


//  "resident"
//
OverlayManager::Overlay * OverlayManager::resident(OverlayID id)
{
    for ( auto & ov : m_residents ) {
        if (ov.info.id == id)   { return &ov; }
    }
    
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
    for (auto & ov : m_residents)
    {
        if (ov.info.visible && ov.cfg.draw_fn) {
            _render_overlay(ov, world_to_px, cursor_px, plot_rect);
        }
    }

    // ---------- render & purge dynamic overlays ----------
    for (auto & ov : m_windows)
    {
        if (ov.info.visible && ov.cfg.draw_fn) {
            _render_overlay(ov, world_to_px, cursor_px, plot_rect);
        }
    }

    m_windows.erase(std::remove_if(m_windows.begin(), m_windows.end(),
                    [](const Overlay& o){ return !o.info.visible; }),
                    m_windows.end());
                    
    return;
}










// *************************************************************************** //
//
//
//      PROTECTED FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_draw_context_menu"
//
void OverlayManager::_draw_context_menu(Overlay & ov)
{
    const bool  canvas_anchor       = ov.cfg.placement != OverlayPlacement::Custom &&
                                      ov.cfg.placement != OverlayPlacement::Cursor &&
                                      ov.cfg.placement != OverlayPlacement::World;
    const bool  canvas_point        = (ov.cfg.placement == OverlayPlacement::CanvasPoint);
    const bool  can_resize          = ov.style.window_size.has_value();
    
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
        ImGui::BeginDisabled(true);
            ImGui::SeparatorText("Overlay Window Settings");
        ImGui::EndDisabled();
        
        
        
        
        ImGui::TextDisabled("State");

        //      1.      DESTINATION ANCHOR...
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
            //
            //
                emit_item("Fixed Point",            OverlayPlacement::CanvasPoint);
                //
                ImGui::NewLine();
                emit_item("Top-Left",               OverlayPlacement::CanvasTL);
                emit_item("Top-Right",              OverlayPlacement::CanvasTR);
                emit_item("Bottom-Left",            OverlayPlacement::CanvasBL);
                emit_item("Bottom-Right",           OverlayPlacement::CanvasBR);
                //
                ImGui::NewLine();
                emit_item("Left",                   OverlayPlacement::CanvasLeft);
                emit_item("Right",                  OverlayPlacement::CanvasRight);
                emit_item("Top",                    OverlayPlacement::CanvasTop);
                emit_item("Bottom",                 OverlayPlacement::CanvasBottom);
            //
            //
            ImGui::EndMenu();
        }
        
        //      2.      SOURCE ANCHOR...
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
        
        //      3.      OFFSCREEN POLICY...
        if ( ImGui::BeginMenu("Offscreen Policy") )
        {
            emit_offscreen("Hide",              OffscreenPolicy::Hide);
            emit_offscreen("Clamp",             OffscreenPolicy::Clamp);
            ImGui::EndMenu();
        }
        
        
        //      4.      WINDOW POSITIONING...
        ImGui::Separator();
        if ( canvas_anchor || canvas_point )
        {
        //
        //
            ImGui::TextDisabled("Position");
            
            
            //      4A.     ...
            if ( canvas_anchor)
            {
                if ( ImGui::BeginMenu("Offset") ) {
                    utl::LeftLabelSimple("X:");
                    ImGui::SliderFloat("##OverlayManager_CanvasOffsetX",     &ov.cfg.anchor_px.x,    -100,   100,    "%.0f");
                    //
                    utl::LeftLabelSimple("Y:");
                    ImGui::SliderFloat("##OverlayManager_CanvasOffsetY",     &ov.cfg.anchor_px.y,    -100,   100,    "%.0f");
                    ImGui::EndMenu();
                }
            }
            
            
            //      4B.     CANVAS POINT ADJUSTMENT... (only when placement == CanvasPoint)...
            if ( canvas_point )
            {
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
        //
        //
        }


        //      5.      WINDOW SIZE...
        ImGui::BeginDisabled( !can_resize );
        //
            if ( can_resize && ImGui::BeginMenu("Window Size") )
            {
                auto &  size    = *ov.style.window_size;
                
                
                //  Size.
                ImGui::SeparatorText("Size");
                utl::LeftLabelSimple("X:");
                ImGui::SliderFloat("##OverlayManager_WindowSizeX",      &size.Value().x,            size.Min().x,       size.Max().x,    "%.1f");
                //
                utl::LeftLabelSimple("Y:");
                ImGui::SliderFloat("##OverlayManager_WindowSizeY",      &size.Value().y,            size.Min().y,       size.Max().y,    "%.1f");
                
                
                //  Minimum.
                //      ImGui::SeparatorText("Minimum Size");
                //      utl::LeftLabelSimple("X:");
                //      ImGui::SliderFloat("##OverlayManager_WindowSizeMinX",     &size.Value().x,          size.Min().x,       size.Max().x,    "%.1f");
                //      //
                //      utl::LeftLabelSimple("Y:");
                //      ImGui::SliderFloat("##OverlayManager_WindowSizeMinY",     &size.Value().y,          size.Min().y,       size.Max().y,    "%.1f");
                
                
                //  Maximum.
                //      ImGui::SeparatorText("Maximum Size");
                //      utl::LeftLabelSimple("X:");
                //      ImGui::SliderFloat("##OverlayManager_WindowSizeMaxX",     &size.Value().x,          size.Min().x,       size.Max().x,    "%.1f");
                //      //
                //      utl::LeftLabelSimple("Y:");
                //      ImGui::SliderFloat("##OverlayManager_WindowSizeMaxY",     &size.Value().y,          size.Min().y,       size.Max().y,    "%.1f");
                
                
            ImGui::EndMenu();
            }
        //
        ImGui::EndDisabled();



        //      6.      TRANSPARENCY / ALPHA SLIDER...
        utl::LeftLabelSimple("Alpha:");
        ImGui::SliderFloat("##OverlayManager_Alpha", &ov.style.alpha, 0.10f, 1.00f, "%.2f");
        
        

        ImGui::EndPopup();
    }
    
    return;
}






//  "_render_default_overlay"
//
void OverlayManager::_render_default_overlay(
        Overlay &                               ov,
        const std::function<ImVec2(ImVec2)> &   world_to_px,
        ImVec2                                  cursor_px,
        const ImRect &                          plot_rect)
{
    // ───────────────────────────────────────────────────────────── 1. anchor
    auto            [pos, pivot]    = _anchor_to_pos(ov, world_to_px, cursor_px, plot_rect);
    const bool      is_custom       = (ov.cfg.placement == OverlayPlacement::Custom);
    const ImVec2    anchor_px       = (ov.cfg.placement == OverlayPlacement::CanvasPoint)// pixel coords of anchor itself (CanvasPoint only)
                                        ? world_to_px(ov.cfg.anchor_ws) : ImVec2{0, 0};
    //
    //
    ImGuiWindow *                       win                     = nullptr;
    ImGuiWindowFlags                    win_flags               = OverlayInfo::ms_DEF_WINDOW_FLAGS;
    //
    auto &                              style                   = ov.style;


    // ───────────────────────────────────────────────────────────── 2. off‑screen
    if ( ov.cfg.placement == OverlayPlacement::CanvasPoint )
    {
        const bool inside =
              (anchor_px.x >= plot_rect.Min.x)  &&  (anchor_px.x <= plot_rect.Max.x)  &&
              (anchor_px.y >= plot_rect.Min.y)  &&  (anchor_px.y <= plot_rect.Max.y);

        if (!inside)
        {
            if (ov.cfg.offscreen == OffscreenPolicy::Hide)      { return; }  // skip draw

            if (ov.cfg.offscreen == OffscreenPolicy::Clamp)
            {
                ImVec2 clamped_anchor{
                    std::clamp(anchor_px.x, plot_rect.Min.x, plot_rect.Max.x),
                    std::clamp(anchor_px.y, plot_rect.Min.y, plot_rect.Max.y)
                };
                pos = clamped_anchor + ov.cfg.anchor_px;          // new pos
            }
        }
    }


    // ───────────────────────────────────────────────────────────── 3. flags/pos
    //
    //
    if (is_custom)              { win_flags &= ~ImGuiWindowFlags_NoMove; }
    else                        { win_flags |=  ImGuiWindowFlags_NoMove; }
    //
    ImGuiCond                   cond            = (is_custom)   ? ImGuiCond_Once : ImGuiCond_Always;
    const ImGuiViewport *       vp              = ImGui::GetMainViewport();
    //  const std::string           win_name        = "##EditorOverlay_" + std::to_string(ov.info.id);
    
    if (!is_custom)                 { ImGui::SetNextWindowViewport(vp->ID); }



    //      OVERLAY STYLE PUSH:
    //
    ImGui::PushStyleColor           ( ImGuiCol_WindowBg,                style.bg                    );
    ImGui::PushStyleVar             ( ImGuiStyleVar_WindowRounding,     style.window_rounding       );
    //
    //
    //      NEXT WINDOW:
    ImGui::SetNextWindowPos         (   pos, cond, pivot                                            );
    ImGui::SetNextWindowBgAlpha     ( style.alpha                                                   );
    

    // ───────────────────────────────────────────────────────────── 4. draw
    ImGui::Begin( ov.info.window_name.c_str(), nullptr, win_flags );
    //
    //  1.  OVERLAY STYLE POP:
        ImGui::PopStyleVar();       //  ImGuiStyleVar_WindowRounding
        ImGui::PopStyleColor();     //  ImGuiCol_WindowBg
        
        ov.cfg.draw_fn();
        if ( !ov.cfg.locked )   { _draw_context_menu(ov);                                   }
    //
    //
    ImGui::End();


    //  ImGuiWindow * win = ImGui::FindWindowByName( win_name.c_str() );
    win = ImGui::FindWindowByName( ov.info.window_name.c_str() );
    if ( !win )                 { return; }



    // ───────────────────────────────────────────────────────────── 5. clamp rect
    const bool want_rect_clamp =
        (ov.cfg.placement == OverlayPlacement::CanvasPoint &&
         ov.cfg.offscreen == OffscreenPolicy::Clamp) ||
        (ov.cfg.placement != OverlayPlacement::Cursor &&
         ov.cfg.placement != OverlayPlacement::Custom &&
         ov.cfg.placement != OverlayPlacement::CanvasPoint);

    if (want_rect_clamp)
    {
        ImVec2 win_min = win->Pos;
        ImVec2 win_max = win_min + win->Size;
        ImVec2 shift   {0,0};

        // left vs right
        if ( win_min.x < plot_rect.Min.x )          { shift.x = plot_rect.Min.x - win_min.x; }
        else if ( win_max.x > plot_rect.Max.x )     { shift.x = plot_rect.Max.x - win_max.x; }

        // top vs bottom
        if ( win_min.y < plot_rect.Min.y )          { shift.y = plot_rect.Min.y - win_min.y; }
        else if ( win_max.y > plot_rect.Max.y )     { shift.y = plot_rect.Max.y - win_max.y; }

        if ( shift.x || shift.y )                   { win->Pos = win_min + shift; }
    }

    // ───────────────────────────────────────────────────────────── 6. persist drag
    if (is_custom && !ImGui::IsMouseDown(ImGuiMouseButton_Left))
        ov.cfg.anchor_px = win->Pos;        // remember new TL


    return;
}










//  "_render_custom_overlay"
//
void OverlayManager::_render_custom_overlay(
        Overlay &                               ov,
        const std::function<ImVec2(ImVec2)> &   world_to_px,
        ImVec2                                  cursor_px,
        const ImRect &                          plot_rect)
{
    IM_ASSERT(ov.cfg.draw_fn  &&  "Overlay draw_fn must not be null");
    
    if ( !ov.info.visible )         { return; }


    // ───────────────────────────────────────────────────────────── 1. anchor
    auto            [pos, pivot]    = _anchor_to_pos(ov, world_to_px, cursor_px, plot_rect);
    const bool      is_custom       = (ov.cfg.placement == OverlayPlacement::Custom);
    const bool      custom_size     = ( ov.style.window_size.has_value() );
    const ImVec2    anchor_px       = (ov.cfg.placement == OverlayPlacement::CanvasPoint)// pixel coords of anchor itself (CanvasPoint only)
                                        ? world_to_px(ov.cfg.anchor_ws) : ImVec2{0, 0};
    //
    //
    ImGuiWindow *                       win                     = nullptr;
    ImGuiWindowFlags                    win_flags               = ov.info.flags;
    //
    static std::optional<OverlayID>     resize_ID               = std::nullopt;
    static bool                         resizing                = false;
    //
    const OverlayID &                   current_ID              = ov.info.id;
    const bool                          resizing_this_window    = ( (resize_ID)  &&  (*resize_ID == current_ID) );
    const bool                          set_pos                 = ( (!resizing)  ||  (!resizing_this_window) );


    // ───────────────────────────────────────────────────────────── 2. off‑screen
    if ( ov.cfg.placement == OverlayPlacement::CanvasPoint )
    {
        const bool inside =
              (anchor_px.x >= plot_rect.Min.x)  &&  (anchor_px.x <= plot_rect.Max.x)  &&
              (anchor_px.y >= plot_rect.Min.y)  &&  (anchor_px.y <= plot_rect.Max.y);

        if (!inside)
        {
            if (ov.cfg.offscreen == OffscreenPolicy::Hide)      { return; }  // skip draw

            if (ov.cfg.offscreen == OffscreenPolicy::Clamp)
            {
                ImVec2 clamped_anchor{
                    std::clamp(anchor_px.x, plot_rect.Min.x, plot_rect.Max.x),
                    std::clamp(anchor_px.y, plot_rect.Min.y, plot_rect.Max.y)
                };
                pos = clamped_anchor + ov.cfg.anchor_px;          // new pos
            }
        }
    }


    // ───────────────────────────────────────────────────────────── 3. flags/pos
    //
    //
    if (is_custom)              { win_flags &= ~ImGuiWindowFlags_NoMove; }
    else                        { win_flags |=  ImGuiWindowFlags_NoMove; }
    //
    //
    ImGuiCond                   cond            = (is_custom)   ? ImGuiCond_Once : ImGuiCond_Always;
    const ImGuiViewport *       vp              = ImGui::GetMainViewport();
    //  const std::string           win_name        = "##EditorOverlay_" + std::to_string(ov.info.id);
    
    if (!is_custom)             { ImGui::SetNextWindowViewport(vp->ID); }



    //      OVERLAY STYLE PUSH:
    //  win_flags                                      ^= ov.cfg.override_flags;
    //
    auto &                          style           = ov.style;
    ImGui::PushStyleColor           ( ImGuiCol_WindowBg,                style.bg                    );
    ImGui::PushStyleVar             ( ImGuiStyleVar_WindowRounding,     style.window_rounding       );
    //
    //
    //      NEXT WINDOW:
    if ( set_pos )                  { ImGui::SetNextWindowPos(pos, cond, pivot); }
    else                            { ImGui::SetNextWindowPos(this->m_pos_cache, ImGuiCond_Once, this->m_pivot_cache); }
    ImGui::SetNextWindowBgAlpha     ( style.alpha                                                   );

    if (custom_size) {
        win_flags      &= ~(ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::SetNextWindowSize                (   (*style.window_size).Value(),       ImGuiCond_Appearing             );
        ImGui::SetNextWindowSizeConstraints     (   (*style.window_size).Min(),         (*style.window_size).Max()      );
    }
    

    // ───────────────────────────────────────────────────────────── 4. draw
    ImGui::Begin( ov.info.window_name.c_str(), nullptr, win_flags );
    //
    //  1.  OVERLAY STYLE POP:
        ImGui::PopStyleVar();       //  ImGuiStyleVar_WindowRounding
        ImGui::PopStyleColor();     //  ImGuiCol_WindowBg
        
        win         = ImGui::GetCurrentWindowRead();
        
        ov.cfg.draw_fn();
        if ( !ov.cfg.locked )   { _draw_context_menu(ov);                                   }
        if ( custom_size )      { (*style.window_size).value = ImGui::GetItemRectSize();    }
        
        
        //      CASE 1 :    CHECK IF *THIS* WINDOW HAS STARTED RE-SIZING...
        if ( !resizing ) {
        
        
            resizing    = ( (win->ResizeBorderHeld == ImGuiDir_None)  &&  ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f) );
            
            if ( win->ResizeBorderHeld == ImGuiDir_None )
            {
                //  ...
            }
            
            if (resizing)
            {
                //  BEGAN RESIZING
            }
            resize_ID   = current_ID;
        }
        //
        //      CASE 2 :    IF *THIS* WINDOW IS ALREADY RESIZING, UPDATE ITS STATUS...
        else
        {
            resizing    = ( ImGui::IsMouseDown(ImGuiMouseButton_Left) );
            if ( !resizing )
            {
                resize_ID               .reset();
                this->m_pos_cache       = {   };
                this->m_pivot_cache     = {   };
            }
        }
    //
    //
    ImGui::End();


    if ( resizing )             { return; }


    //  ImGuiWindow * win = ImGui::FindWindowByName( win_name.c_str() );
    //  ImGuiWindow * win = ImGui::FindWindowByName( ov.info.window_name.c_str() );
    if ( !win )                 { return; }



    // ───────────────────────────────────────────────────────────── 5. clamp rect
    const bool want_rect_clamp =
        (ov.cfg.placement == OverlayPlacement::CanvasPoint &&
         ov.cfg.offscreen == OffscreenPolicy::Clamp) ||
        (ov.cfg.placement != OverlayPlacement::Cursor &&
         ov.cfg.placement != OverlayPlacement::Custom &&
         ov.cfg.placement != OverlayPlacement::CanvasPoint);

    if (want_rect_clamp)
    {
        ImVec2 win_min = win->Pos;
        ImVec2 win_max = win_min + win->Size;
        ImVec2 shift   {0,0};

        // left vs right
        if ( win_min.x < plot_rect.Min.x )          { shift.x = plot_rect.Min.x - win_min.x; }
        else if ( win_max.x > plot_rect.Max.x )     { shift.x = plot_rect.Max.x - win_max.x; }

        // top vs bottom
        if ( win_min.y < plot_rect.Min.y )          { shift.y = plot_rect.Min.y - win_min.y; }
        else if ( win_max.y > plot_rect.Max.y )     { shift.y = plot_rect.Max.y - win_max.y; }

        if ( shift.x || shift.y )                   { win->Pos = win_min + shift; }
    }

    // ───────────────────────────────────────────────────────────── 6. persist drag
    if (is_custom && !ImGui::IsMouseDown(ImGuiMouseButton_Left))
        ov.cfg.anchor_px = win->Pos;        // remember new TL


    return;
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
        case OverlayPlacement::Custom:          { pos = ov.cfg.anchor_px;                                                                   break; }
        case OverlayPlacement::Cursor:          { pos = { cursor_px.x + ov.cfg.anchor_px.x, cursor_px.y + ov.cfg.anchor_px.y };             break; }
        case OverlayPlacement::World:           { pos = world_to_px(ov.cfg.anchor_ws);                                                      break; }
    //
    //
    //
        case OverlayPlacement::CanvasPoint:     { pos = world_to_px(ov.cfg.anchor_ws) + ov.cfg.anchor_px;                                   break; }   // pixel offset // pivot stays {0,0} so pos is TL unless we add a pivot field later
        case OverlayPlacement::CanvasTL:        { pos = { plot_rect.Min.x + ax, plot_rect.Min.y + ay };                                     break; }
        case OverlayPlacement::CanvasTR:        { pos = { plot_rect.Max.x - ax, plot_rect.Min.y + ay };                                     break; }
        case OverlayPlacement::CanvasBL:        { pos = { plot_rect.Min.x + ax, plot_rect.Max.y - ay };                                     break; }
        case OverlayPlacement::CanvasBR:        { pos = { plot_rect.Max.x - ax, plot_rect.Max.y - ay };                                     break; }
    //
        case OverlayPlacement::CanvasLeft:      { pos = { plot_rect.Min.x + ax, (plot_rect.Min.y + plot_rect.Max.y) * 0.5f + ay };          break; }
        case OverlayPlacement::CanvasRight:     { pos = { plot_rect.Max.x - ax, (plot_rect.Min.y + plot_rect.Max.y) * 0.5f + ay };          break; }
        case OverlayPlacement::CanvasTop:       { pos = { (plot_rect.Min.x + plot_rect.Max.x) * 0.5f + ax, plot_rect.Min.y + ay };          break; }
        case OverlayPlacement::CanvasBottom:    { pos = { (plot_rect.Min.x + plot_rect.Max.x) * 0.5f + ax, plot_rect.Max.y - ay };          break; }
    //
        default :                               { break; }
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

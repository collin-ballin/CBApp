/***********************************************************************************
*
*       ********************************************************************
*       ****          G R A P H _ A P P . C P P  ____  F I L E          ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond
*               DATED:      May 18, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/graph_app/graph_app.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//  1B.     PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "Begin"
//
void GraphApp::Begin([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
{
    static bool                     init                    = true;
    static constexpr const char *   controls_uuid           = "Controls";


    //  1.  CREATING THE HOST WINDOW...
    ImGui::Begin(uuid, p_open, flags);
        //  Generate a persistent dockspace ID
        m_dockspace_id          = ImGui::GetID(m_dockspace_name);
        
        ImGui::DockSpace(m_dockspace_id,    ImVec2(0.0f, 0.0f),     m_dockspace_flags);
        if (init) [[unlikely]] {
            init = false;
            ImGui::DockBuilderRemoveNode    (m_dockspace_id); // clear any previous layout
            ImGui::DockBuilderAddNode       (m_dockspace_id, ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize   (m_dockspace_id, ImVec2(800, 600));

            //  Split the dock UP and DOWN...
            m_dock_ids[1]   = ImGui::DockBuilderSplitNode( m_dockspace_id, ImGuiDir_Down, m_dockspace_ratio,
                                                           nullptr, &m_dock_ids[0] );

            ImGui::DockBuilderDockWindow(m_win_uuids[0],    m_dock_ids[0]);
            ImGui::DockBuilderDockWindow(m_win_uuids[1],    m_dock_ids[1]);
            ImGui::DockBuilderFinish(m_dockspace_id);
        }
        if (this->m_rebuild_dockspace) [[unlikely]] {
            this->m_rebuild_dockspace   = false;
            this->RebuildDockspace();
        }
    ImGui::End();
    
    
    //  2.  CREATE TOP WINDOW FOR PLOTS...
    ImGui::SetNextWindowClass(&this->m_window_class[0]);
    ImGui::Begin(m_win_uuids[0], nullptr, m_docked_win_flags[0]);
        this->display_plots();
    ImGui::End();
    
    
    //  3.  CREATE BOTTOM WINDOW FOR CONTROLS...
    if (m_child_open[1]) {
        ImGui::SetNextWindowClass(&this->m_window_class[1]);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, app::DEF_LIVE_DARKBLUE);
        ImGui::Begin(m_win_uuids[1], nullptr, m_docked_win_flags[1]);
            this->display_controls();
        ImGui::End();
        ImGui::PopStyleColor();
    }
    
    
    //  4.  DEFINE COLLAPSE/EXPAND TOOLBAR BUTTON...
    static toolbar::config  ctrl_toolbar{
    //  PARENT UUID.      PARENT ANCHOR.        HOST UUID.      HOST ANCHOR.                PARENT OFFSET.      HOST OFFSET.
        m_win_uuids[1], utl::Anchor::NorthWest, uuid,           utl::Anchor::SouthWest,     {ms_COLLAPSE_BUTTON_SIZE.x, 0.0f},       {0.0f, -45.0f}
    };
    
    if ( toolbar::begin("##sidebar_toolbar", ctrl_toolbar) )
    {
        if ( utl::DirectionalButton("##toggle", m_child_open[1] ? Anchor::South : Anchor::North, ms_COLLAPSE_BUTTON_SIZE) )
        {
            m_child_open[1]     = !m_child_open[1];
            //sidebar_ratio       = show_sidebar ? 0.60 : 0.05f;
        }
    }
    toolbar::end();
    
    
    
    return;
}




// *************************************************************************** //
//
//
//  2.  PRIMARY "PLOT" AND "CONTROL" FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "display_plots"
//
void GraphApp::display_plots(void)
{
    //  DEFINE MISC. VARIABLES...
    static size_t                   N                           = 0;
    static bool                     SHOW_HELP_TABS              = true;
    static bool                     ENABLE_ADDING_TABS          = true;
    

    //  BEGIN THE TAB BAR...
    if ( ImGui::BeginTabBar(m_tabbar_uuids[0], m_tabbar_flags[0]) )
    {
        //  1.      DRAW HELP-MENU TAB-BUTTON ITEM ("?")...
        if (SHOW_HELP_TABS) {
            if (ImGui::TabItemButton("?", ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_NoTooltip))
                ImGui::OpenPopup("MyHelpMenu");
        }
        if (ImGui::BeginPopup("MyHelpMenu")) {
            ImGui::Selectable("Here's a help message!  The PLOTS go here, on the top.");
            ImGui::EndPopup();
        }


        if (ENABLE_ADDING_TABS)     //  2.      DRAW THE "ADD-TAB" BUTTON ("+")...
        {
            if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip))
            {   ImGui::OpenPopup("add_plot_tab_popup");     }
            //
            if (ImGui::BeginPopup("add_plot_tab_popup")) {  //          - NEW PLOTTING TAB POP-UP...
                //
                AddNewPlotTab();
                //
                ImGui::EndPopup();
            }// END "Popup".
        }



        //      2.3     DRAW EACH OF THE TAB ITEMS...
        for (auto & tab : this->ms_PLOT_TABS)
        {
            if ( ImGui::BeginTabItem( tab.get_uuid(), (tab.no_close) ? nullptr : &tab.open, tab.flags ) )
            {
                if (tab.render_fn) {
                    tab.render_fn( tab.get_uuid(), &tab.open, tab.flags );
                }
                else {
                    this->DefaultPlotTabRenderFunc(tab.get_uuid(), &tab.open, tab.flags);
                }
                
            ImGui::EndTabItem();
            }// END "BeginTabItem".
        
        } // END "for auto & tab".


    ImGui::EndTabBar();
        
    } // END "BeginTabBar".

    return;
}


//  "display_controls"
//
void GraphApp::display_controls(void)
{
    //  DEFINE MISC. VARIABLES...
    static  ImGuiTabItemFlags       SPACING_FLAGS   = ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_NoTooltip | ImGuiTabItemFlags_NoReorder;
    
    
    //  BEGIN THE TAB BAR...
    if ( ImGui::BeginTabBar(m_tabbar_uuids[1], m_tabbar_flags[1]) )
    {
        //      2.3     DRAW EACH OF THE TAB ITEMS...
        for (auto & tab : this->ms_CTRL_TABS)
        {
            //  PLACE TAB-BAR ITEM SPACING...
            ImGui::PushID("##CtrlTabbarSpacing");
            ImGui::TabItemSpacing("##CtrlTabbarSpacing", SPACING_FLAGS, ms_SPACING);
            ImGui::PopID();
        
            //  PLACING EACH TAB...
            if ( ImGui::BeginTabItem( tab.get_uuid(), (tab.no_close) ? nullptr : &tab.open, tab.flags ) )
            {
                if (tab.render_fn) {
                    tab.render_fn( tab.get_uuid(), &tab.open, tab.flags );
                }
                else {
                    this->DefaultCtrlTabRenderFunc(tab.get_uuid(), &tab.open, tab.flags);
                }
                
            ImGui::EndTabItem();
            }// END "BeginTabItem".
        
        } // END "for auto & tab".


    ImGui::EndTabBar();
    } // END "BeginTabBar".

    return;
}






// *************************************************************************** //
//
//
//  ?.      UTILITY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "RebuildDockspace"
//
void GraphApp::RebuildDockspace(void)
{
    //ImGui::DockBuilderRemoveNode    (this->S.m_dockspace_id);
    //m_dockspace_id          = ImGui::GetID(m_dockspace_name);
    
    //  ImGui::DockSpace(m_dockspace_id,    ImVec2(0.0f, 0.0f),     m_dockspace_flags);
    //  ImGui::DockBuilderRemoveNode    (m_dockspace_id); // clear any previous layout
    //  ImGui::DockBuilderAddNode       (m_dockspace_id, ImGuiDockNodeFlags_DockSpace);
    
    ImGui::DockBuilderSetNodeSize   (m_dockspace_id, ImVec2(800, 600));

        // Example split: left and right
        m_dock_ids[1]   = ImGui::DockBuilderSplitNode( m_dockspace_id, ImGuiDir_Down, m_dockspace_ratio,
                                                       nullptr, &m_dock_ids[0] );

        ImGui::DockBuilderDockWindow(m_win_uuids[0],    m_dock_ids[0]);
        ImGui::DockBuilderDockWindow(m_win_uuids[1],    m_dock_ids[1]);
        ImGui::DockBuilderFinish(m_dockspace_id);

    return;
}


//  "AddNewPlotTab"
//
void GraphApp::AddNewPlotTab(void)
{
    constexpr size_t            N           = 3;
    const static std::string    OPTIONS[]   = {
        "1D Plot", "Heatmap", "Other"
    };
    Tab_t               new_tab     = { "",     true,   false,      0U,     nullptr };
    
    bool                selected    = false;
    short               plot_type   = -1;
    const size_t        N_tabs      = this->ms_PLOT_TABS.size() + 1;
    std::string         uuid        = "Tab #" + std::to_string(N);
    
    
    ImGui::SeparatorText("Add New Plot...");
    ImGui::Separator();
    
    
    
    //  2.  DEFINE PLOT TYPE SELECTIONS...
    for (size_t i = 0; i < N; ++i)
    {
        if ( ImGui::Selectable( OPTIONS[i].c_str() ) ) {
            selected        = true;
            plot_type       = static_cast<short>(i);
            new_tab.uuid    = OPTIONS[i] + " #" + std::to_string(N_tabs);
        }
    }
         

    //  3.  ADD THE NEW PLOT TYPE...
    if (selected)
    {
        switch (plot_type) {
            case 0 : {          //  CASE "0" :      ???
                //
                break;
            }
            case 1 : {          //  CASE "1" :      ???
                //
                break;
            }
            //
            //
            default : {         //  DEFAULT :       GENERIC TAB.
                //
                break;
            }
        }
        this->ms_PLOT_TABS.push_back(new_tab);
    }
        
        
        
    return;
}


//  "AddNewCtrlTab"
//
void GraphApp::AddNewCtrlTab(void)      { return; }


//  "DefaultPlotTabRenderFunc"
//
void GraphApp::DefaultPlotTabRenderFunc([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags) {
    if (!p_open)
        return;
        
    ImGui::Text("Window Tab \"%s\".  Here is some default text dispatched by \"DefaultPlotTabRenderFunc()\".", uuid);
    return;
}



//  "DefaultCtrlTabRenderFunc"
//
void GraphApp::DefaultCtrlTabRenderFunc([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags) {
    if (!p_open)
        return;
        
    ImGui::BulletText("Tab UUID: \"%s\".", uuid);
        
    ImGui::Text("Window Tab \"%s\".  Here is some default text dispatched by \"DefaultCtrlTabRenderFunc()\".", uuid);
    return;
}



//  "get_tab"
//
utl::Tab_t * GraphApp::get_ctrl_tab(const std::string & uuid, std::vector<Tab_t> & tabs) {
    bool            match       = false;
    const size_t    N           = tabs.size();
    size_t          idx         = N + 1;       //   Default:    if (NO MATCH):  "N < idx"
    
    //  1.  FIND THE INDEX AT WHICH THE TAB WITH NAME "uuid" IS FOUND...
    for (size_t i = 0; i < N && !match; ++i) {
        match = ( uuid == tabs[i].uuid );
        if (match) idx = i;
    }
    
    if (!match)
        return nullptr;
    
    return std::addressof( tabs[idx] );
}



// *************************************************************************** //
//
//
//  ?.      NEW SKETCH / EDITOR STUFF...
// *************************************************************************** //
// *************************************************************************** //






// *************************************************************************** //
//
//
//  ?.      OLD SKETCH STUFF...
// *************************************************************************** //
// *************************************************************************** //

namespace dragpoint {
// *************************************************************************** //
static constexpr size_t         BUFFER_SIZE             = 64;
struct DragPoint {
    int                         id                      = -1;
    std::string                 uuid                    = "";
    ImPlotPoint                 pos                     = ImPlotPoint(0.0f, 0.0f);
    ImPlotDragToolFlags         flags                   = ImPlotDragToolFlags_None;
    bool                        clicked                 = false;
    bool                        hovered                 = false;
    bool                        held                    = false;
};
// *************************************************************************** //
}

static std::vector<float>           eps_data;



// *************************************************************************** //
// *************************************************************************** //

//  plot CFG...
static utl::PlotCFG             plot_cfg        = {
    {   "##EditorPlot",             ImVec2(-1, -1),         ImPlotFlags_None  }, // ImPlotFlags_CanvasOnly
    {
        { "##X", ImPlotAxisFlags_None | ImPlotAxisFlags_NoSideSwitch | ImPlotAxisFlags_NoHighlight }, // ImPlotAxisFlags_Lock
        { "##Y", ImPlotAxisFlags_None | ImPlotAxisFlags_NoSideSwitch | ImPlotAxisFlags_NoHighlight }
    },
    {   ImPlotLocation_NorthWest,
        ImPlotLegendFlags_None | ImPlotLegendFlags_Outside
    }
};



// *************************************************************************** //
// *************************************************************************** //

struct SketchState {
    // dynamic data buffer (row-major): size = res_x * res_y
    std::vector<float> data;

    // grid dimensions
    int res_x = 0; // columns (X)
    int res_y = 0; // rows    (Y)

    // interaction / brush settings
    int   brush_size   = 3;   // radius in grid units
    int   brush_shape  = 0;   // 0 = square, 1 = circle
    float paint_value  = 1.0f;

    // colormap + value range
    float vmin         = 0.0f;
    float vmax         = 1.0f;
    int   cmap         = 0;   // ImPlot colormap index

    // run-time interaction flags
    bool  drawing      = false;
    int   prev_x       = -1;  // previous cell (for Bresenham)
    int   prev_y       = -1;

    // ImPlot ID for the heatmap (unique per instance)
    ImGuiID heatmap_uuid = ImHashStr("SketchHeatmap");

    // ---------------------------------------------------------------------
    void resize(int new_x, int new_y) {
        if (new_x == res_x && new_y == res_y) return;
        std::vector<float> new_buf(static_cast<size_t>(new_x) * new_y, 0.0f);
        if (res_x > 0 && res_y > 0) {
            const int copy_x = std::min(res_x, new_x);
            const int copy_y = std::min(res_y, new_y);
            for (int y = 0; y < copy_y; ++y)
                std::copy_n(data.data() + static_cast<size_t>(y) * res_x,
                            copy_x,
                            new_buf.data() + static_cast<size_t>(y) * new_x);
        }
        data.swap(new_buf);
        res_x = new_x;
        res_y = new_y;
    }

    void clear() { std::fill(data.begin(), data.end(), 0.0f); }

    void reset_prev() { prev_x = prev_y = -1; }
};// end struct






//  Global persistent state (local to this translation unit)
// single persistent instance --------------------------------------------------------
static SketchState sketch;

static constexpr float      LABEL_WIDTH         = 150.0f;
static constexpr float      WIDGET_WIDTH        = 250.0f;

//  "EditorUtility"
//
void GraphApp::EditorUtility(void)
{
    // ---------------------------------------------------------------------
    const int res_x = m_steps.NX.value;   // external grid width
    const int res_y = m_steps.NY.value;   // external grid height
    sketch.resize(res_x, res_y);

    ImPlotHeatmapFlags hm_flags   = 0;
    ImPlotAxisFlags    axes_flags = ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoGridLines |
                                     ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoDecorations;
    ImPlotFlags        plot_flags = ImPlotFlags_NoLegend;

    ImVec2 avail      = ImGui::GetContentRegionAvail();
    ImVec2 scale_dims = ImVec2(80.0f, avail.y);
    ImVec2 plot_dims  = ImVec2(avail.x - scale_dims.x - ImGui::GetStyle().ItemSpacing.x, avail.y);


    // ----------------------- Begin Plot ----------------------------------
        // lambda: stamp brush at grid cell (gx,gy)
        auto stamp = [&](int gx, int gy) {
            for (int dy = -sketch.brush_size + 1; dy < sketch.brush_size; ++dy) {
                for (int dx = -sketch.brush_size + 1; dx < sketch.brush_size; ++dx) {
                    int x = gx + dx;
                    int y = gy + dy;
                    if (x < 0 || x >= res_x || y < 0 || y >= res_y) continue;
                    if (sketch.brush_shape == 0 || (dx*dx + dy*dy) < sketch.brush_size*sketch.brush_size)
                        sketch.data[static_cast<size_t>(y) * res_x + x] = sketch.paint_value;
                }
            }
        };

        // lambda: Bresenham line
        auto draw_line = [&](int x0,int y0,int x1,int y1){
            int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
            int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
            int err = (dx > dy ? dx : -dy) / 2;
            for (;;) {
                stamp(x0,y0);
                if (x0 == x1 && y0 == y1) break;
                int e2 = err;
                if (e2 > -dx) { err -= dy; x0 += sx; }
                if (e2 <  dy) { err += dx; y0 += sy; }
            }
        };

        // --------------------- Input Handling -----------------------------
        if (ImPlot::IsPlotHovered() && ImGui::IsMouseDown(0)) {
            ImPlotPoint mp = ImPlot::GetPlotMousePos();
            int cx = static_cast<int>(mp.x);
            int cy = static_cast<int>(mp.y);
            if (sketch.prev_x < 0)
                draw_line(cx, cy, cx, cy);
            else
                draw_line(sketch.prev_x, sketch.prev_y, cx, cy);
            sketch.drawing = true;
            sketch.prev_x = cx;
            sketch.prev_y = cy;
        } else {
            sketch.drawing = false;
            sketch.reset_prev();
        }

        // --------------------- Heatmap Render -----------------------------
        ImPlot::PlotHeatmap("heat", sketch.data.data(), res_y, res_x,
                            sketch.vmin, sketch.vmax, nullptr,
                            ImPlotPoint(0,0), ImPlotPoint((double)res_x,(double)res_y), hm_flags);

        // --------------------- Brush Cursor -------------------------------
        if (ImPlot::IsPlotHovered()) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_None);
            ImPlotPoint mp = ImPlot::GetPlotMousePos();
            ImVec2 center = ImPlot::PlotToPixels(mp);
            float unit_px = ImPlot::PlotToPixels(ImPlotPoint(1,0)).x - ImPlot::PlotToPixels(ImPlotPoint(0,0)).x;
            float half    = sketch.brush_size * unit_px * 0.5f;
            ImDrawList* dl = ImGui::GetForegroundDrawList();
            if (sketch.brush_shape == 0)
                dl->AddRect({center.x-half,center.y-half},{center.x+half,center.y+half},IM_COL32(255,0,0,255),0,0,2.0f);
            else
                dl->AddCircle(center, half, IM_COL32(255,0,0,255), 0, 2.0f);
        }

        
    return;
}


    
    
//  "ShowEditorOLD"
//
void GraphApp::ShowEditorOLD(void)
{
    using                           DragPoint       = dragpoint::DragPoint;
    //  CONSTANTS...
    static ImVec4                   C1              = app::DEF_APPLE_BLUE;
    static ImVec4                   C2              = app::DEF_APPLE_RED;
    static float                    R1              = 10.0;
    static float                    R2              = 1.25 * R1;
    static float                    XLIMS [2]       = {0.0f, 1.0f};
    static float                    YLIMS [2]       = {0.0f, 1.0f};
    static int                      N               = 0;
    static int                      MAX_POINTS      = 20;
    static bool                     once            = true;
    
    //  VARIABLES...
    static std::vector<DragPoint>   POINTS;
    int                             cache_N         = static_cast<int>( POINTS.size() );
    static ImPlotDragToolFlags      drag_flags      = ImPlotDragToolFlags_None;
    
    
    //  1A.     INITIALIZE VECTOR...
    if (once) {
        POINTS.reserve(MAX_POINTS);
    }
    
    
    //  1B.     SHRINKING...
    if ( N < POINTS.size() ) {
        //S.m_logger.infof("SHRINKING | (Pre) \t Size={}, \t Capacity={}", POINTS.size(), POINTS.capacity() );
        POINTS.resize(N);              // destroys the last elements
    }
    
    
    //  1C.     GROWING...
    if ( POINTS.size() < N ) {
        const int diff = N - static_cast<int>( POINTS.size() );
        
        for (size_t i = cache_N; i < N; ++i) {
            POINTS.push_back( DragPoint{ static_cast<int>(i), "",
                                         ImPlotPoint( utl::RandomRange(XLIMS[0], XLIMS[1]), utl::RandomRange(YLIMS[0], YLIMS[1]) ),
                                         drag_flags } );
            
            //std::snprintf(popup_id, sizeof(popup_id), "PointPopup_%d", point.uuid);
        }
        cache_N = static_cast<int>( POINTS.size() );
    }
    
    
    //  2.  PLACE CONTROLS...
    //  ImGui::SliderInt("Number of Points",    &N,     0,      MAX_POINTS,     "%d points");

    ImGui::CheckboxFlags("NoCursors",   (unsigned int*)&drag_flags,     ImPlotDragToolFlags_NoCursors);     ImGui::SameLine();
    ImGui::CheckboxFlags("NoFit",       (unsigned int*)&drag_flags,     ImPlotDragToolFlags_NoFit);         ImGui::SameLine();
    ImGui::CheckboxFlags("NoInput",     (unsigned int*)&drag_flags,     ImPlotDragToolFlags_NoInputs);
    
    
    //  3.  BEGIN THE PLOT...
    if ( utl::MakePlotCFG(plot_cfg) )
    {
        ImPlot::SetupAxesLimits(0, this->m_steps.NX.value,  0, this->m_steps.NY.value, ImPlotCond_Always);
    
    
        
        
        
        ImPlot::PushColormap( ImPlot::GetColormapIndex("Perm_E") );

    
        this->EditorUtility();
    
              
        
        //  1.  ALLOW USER TO CLICK...
        if ( ImPlot::IsPlotHovered() && ImGui::IsMouseClicked(0) && utl::GetIO_KeyCtrl() )
        {
            ImPlotPoint pos = ImPlot::GetPlotMousePos();
            
            if (N < MAX_POINTS) {
                S.m_logger.infof("Adding point from mouse-click (x={:.2}, y={:.2})", pos.x, pos.y);
                
                POINTS.push_back( DragPoint{static_cast<int>(++N), "", pos, drag_flags} );
            }
        }


        //  2.  PLACE THE DRAG-POINTS...
        for (size_t i = 0; i < POINTS.size(); ++i)
        {
            //      2.1     Create the point:
            auto & point = POINTS[i];
            ImPlot::DragPoint( point.id, &point.pos.x, &point.pos.y,
                               (point.held) ? C2 : C1,
                               (point.held) ? R2 : R1,
                               drag_flags,
                               &point.clicked, &point.hovered, &point.held );
                               
                     
            //      2.2     Allow user to RIGHT-CLICK on a point:
            if ( point.hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right) )
            {
                ImVec2 popup_pos = ImPlot::PlotToPixels(point.pos.x, point.pos.y);
                ImGui::SetNextWindowPos(popup_pos, ImGuiCond_Appearing, ImVec2(0, 1));
                char popup_id[32];
                std::snprintf(popup_id, sizeof(popup_id), "PointPopup_%d", point.id);
                ImGui::OpenPopup(popup_id);
            }
            //
            //      2.3     Draw a "Popup Context Menu" over the point.
            char popup_id[32];
            std::snprintf(popup_id, sizeof(popup_id), "PointPopup_%d", point.id);
            if (ImGui::BeginPopup(popup_id))
            {
                ImGui::Text("Point %d Menu", point.id);
                ImGui::Separator();
                // Future menu items go here
                ImGui::EndPopup();
            }
            
        }// END "for-loop" DRAG POINTS.
        
        

        ImVec2                  avail           = ImGui::GetContentRegionAvail();
        ImVec2                  scale_dims      = ImVec2(80.0f, avail.y);
                            
        ImGui::SameLine();
        ImPlot::ColormapScale("##Perm_E_Scale", 1.0f, 5.0f, scale_dims);
        
        
        
        ImPlot::PopColormap();
        ImPlot::EndPlot();  //  END PLOT.
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

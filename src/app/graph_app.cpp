/***********************************************************************************
*
*       ********************************************************************
*       ****       _ C O I N C I D E N C E . C P P  ____  F I L E       ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond
*               DATED:      May 10, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/_graph_app.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//  0.      STATIC VARIABLES...
// *************************************************************************** //
// *************************************************************************** //

namespace sketch {
    //  MISC STUFF...
    static float                        w                       = 200.0f;
    
    //  SKETCH STUFF...
    static const char *                 heatmap_uuid            = "##EtchASketch";
    static int                          res                     = 128;
    static int                          last_res                = -1;
    static float                        vmin                    = 0.0f,     vmax = 1.0f;
    static ImPlotColormap               cmap                    = ImPlotColormap_Viridis;
    static std::vector<float>           data;
    
    //  BRUSH STUFF...
    static const char *                 brush_shapes[]          = {"Square", "Circle"};
    static int                          brush_size              = 1;
    static int                          brush_shape             = 0; // 0 = square, 1 = circle
    static float                        paint_value             = 1.0f; // value to paint into the grid
    static bool                         drawing                 = false;
}



// *************************************************************************** //
//
//
//  1.      INITIALIZATION  | DEFAULT CONSTRUCTOR, DESTRUCTOR, ETC...
// *************************************************************************** //
// *************************************************************************** //

//  Parametric Constructor 1.
//
GraphApp::GraphApp(app::AppState & src)
    : S(src)
{
    this->m_heatmap     = cb::HeatMap(256, 256);
}


//  Destructor.
//
GraphApp::~GraphApp(void)     { this->destroy(); }


//  "destroy"
//
void GraphApp::destroy(void)
{
    return;
}






// *************************************************************************** //
//
//
//  1B.     SECONDARY INITIALIZATION...
// *************************************************************************** //
// *************************************************************************** //

//  "initialize"
//
void GraphApp::initialize(void)
{
    if (this->m_initialized)
        return;
        
    this->init();
    return;
}


//  "init"
//
void GraphApp::init(void)
{
    ms_I_PLOT_COL_WIDTH                        *= S.m_dpi_scale;
    
    
    //  Assign "Window Class" Properties...
    m_window_class[0].DockNodeFlagsOverrideSet      = 0;    //  ImGuiDockNodeFlags_HiddenTabBar; //ImGuiDockNodeFlags_NoTabBar;
    m_window_class[1].DockNodeFlagsOverrideSet      = ImGuiDockNodeFlags_NoTabBar; //ImGuiDockNodeFlags_HiddenTabBar; //ImGuiDockNodeFlags_NoTabBar;
    
    
    //  1.  TABS FOR PLOT WINDOW...
    ms_PLOT_TABS                                = {
        Tab_t(  "Tab 1",    true,   true,   ImGuiTabItemFlags_None,     nullptr),
        Tab_t(  "Tab 2",    true,   true,   ImGuiTabItemFlags_None,     nullptr)
    };
    
    //  for (std::size_t i = 0; i < ms_PLOT_TABS.size(); ++i)
    //  {
    //      Tab_t &       tinfo         = S.m_windows[ static_cast<Window>(i) ];
    //      if (tinfo.open) {
    //          this->S.m_primary_windows.push_back( std::string(winfo.uuid) );
    //      }
    //  }
    
    
    //  2.  TABS FOR CONTROL WINDOW...
    ms_CTRL_TABS                                = {
        Tab_t(  "Tab 1",    true,   true,   ImGuiTabItemFlags_None,     nullptr),
        Tab_t(  "Tab 2",    true,   true,   ImGuiTabItemFlags_None,     nullptr)
    };
    
    
    this->m_initialized                         = true;
    return;
}


//  "dispatch_plot_function"
//
void GraphApp::dispatch_plot_function(const size_t idx)
{
    auto &          T       = ms_PLOT_TABS[idx];
    
    
    //  DISPATCH EACH RENDER FUNCTION FOR EACH WINDOW OF THE APPLICATION...
    switch (idx)
    {
        //
        //      1.  ...
        // case 0:         {
        //     this->ShowDockspace(         w.uuid.c_str(),     nullptr,        w.flags);
        //     break;
        // }
        default: {
            break;
        }
    }
    
    return;
}






// *************************************************************************** //
//
//
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
        //
        //  Generate a persistent dockspace ID
        m_dockspace_id          = ImGui::GetID(m_dockspace_name);
        
        ImGui::DockSpace(m_dockspace_id,    ImVec2(0.0f, 0.0f),     m_dockspace_flags);
        if (init) [[unlikely]] {
            init = false;
            ImGui::DockBuilderRemoveNode    (m_dockspace_id); // clear any previous layout
            ImGui::DockBuilderAddNode       (m_dockspace_id, ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize   (m_dockspace_id, ImVec2(800, 600));

            // Example split: left and right
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
    ImGui::PushStyleColor(ImGuiCol_WindowBg, app::DEF_LIVE_LIGHTBLUE);
    ImGui::Begin(m_win_uuids[0], nullptr, m_docked_win_flags[0]);
        this->display_plots();
    ImGui::End();
    ImGui::PopStyleColor();
    
    
    //  3.  CREATE BOTTOM WINDOW FOR CONTROLS...
    ImGui::SetNextWindowClass(&this->m_window_class[1]);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, app::DEF_LIVE_DARKBLUE);
    ImGui::Begin(m_win_uuids[1], nullptr, m_docked_win_flags[1]);
        this->display_controls();
    ImGui::End();
    ImGui::PopStyleColor();
    
    return;
}


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
        //      1.      DRAW HELP-MENU TAB-BUTTON ITEM ("?")...
        if (SHOW_HELP_TABS) {
            if (ImGui::TabItemButton("?", ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_NoTooltip))
                ImGui::OpenPopup("MyHelpMenu");
        }
        if (ImGui::BeginPopup("MyHelpMenu")) {
            ImGui::Selectable("Hello!");
            ImGui::EndPopup();
        }


        //      2.      DRAW THE "ADD-TAB" BUTTON ("+")...
        //  if (ENABLE_ADDING_TABS) {
        //      if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip))
        //          { AddNewTabFunc(); /* Add new tab */ }
        //  }



        //      2.3     DRAW EACH OF THE TAB ITEMS...
        for (auto & tab : this->ms_PLOT_TABS)
        {
            if ( ImGui::BeginTabItem( tab.uuid, (tab.no_close) ? nullptr : &tab.open, tab.flags ) )
            {
                if (tab.render_fn) {
                    tab.render_fn( tab.uuid, &tab.open, tab.flags );
                }
                else {
                    this->DefaultTabRenderFunc(tab.uuid, &tab.open, tab.flags);
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
    static bool                     ENABLE_ADDING_TABS          = true;
    

    //  BEGIN THE TAB BAR...
    if ( ImGui::BeginTabBar(m_tabbar_uuids[1], m_tabbar_flags[1]) )
    {
        //      1.      DRAW THE CLOSE-CONTROL-BAR BUTTON...
        if ( ImGui::TabItemButton( (this->m_child_open[1]) ? ms_TABBAR_OPEN_TEXT : ms_TABBAR_CLOSED_TEXT,
             ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_NoTooltip) )
        {
            this->m_child_open[1]       = !this->m_child_open[1];
            m_HEIGHT_LIMITS[1].first    = (this->m_child_open[1]) ? 0 : 30 * S.m_dpi_scale ;
            
            //m_docked_win_flags[1]  |= ImGuiWindowFlags_AlwaysAutoResize;
            
            
            m_dockspace_ratio           = (this->m_child_open[1]) ?  0.60f : 0.05f;
            this->m_rebuild_dockspace   = true;
        }
        
        
        //      2.      QUIT EARLY IF CONTROL TAB-BAR IS NOT OPEN...
        if (!this->m_child_open[1])
        {
            ImGui::EndTabBar();
            return;
        }
        

        //      2.3     DRAW EACH OF THE TAB ITEMS...
        for (auto & tab : this->ms_CTRL_TABS)
        {
            if ( ImGui::BeginTabItem( tab.uuid, (tab.no_close) ? nullptr : &tab.open, tab.flags ) )
            {
                if (tab.render_fn) {
                    tab.render_fn( tab.uuid, &tab.open, tab.flags );
                }
                else {
                    this->DefaultTabRenderFunc(tab.uuid, &tab.open, tab.flags);
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
//  2.      PROTECTED MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "etch_a_sketch"
//
void GraphApp::etch_a_sketch(void)
{
    // 1. CONSTANTS AND VARIABLES
    static constexpr float LABEL_WIDTH  = 150.0f;
    static constexpr float WIDGET_WIDTH = 250.0f;

    // 1.1 Layout flags
    ImPlotHeatmapFlags hm_flags   = 0;
    ImPlotAxisFlags    axes_flags = ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoGridLines |
                                   ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoDecorations;
    ImPlotFlags        plot_flags = ImPlotFlags_NoLegend;
    ImVec2             avail      = ImGui::GetContentRegionAvail();
    ImVec2             scale_dims = ImVec2(80.0f, avail.y);
    ImVec2             plot_dims  = ImVec2(avail.x - scale_dims.x - ImGui::GetStyle().ItemSpacing.x, avail.y);

    // Resize buffer if resolution changed
    if (sketch::res != sketch::last_res) {
        sketch::data.assign(sketch::res * sketch::res, 0.0f);
        sketch::last_res = sketch::res;
    }

    // Track last drawn grid cell for Bresenham
    static int prev_x = -1, prev_y = -1;

    ImGui::BeginGroup();
    if (ImPlot::BeginPlot(sketch::heatmap_uuid, plot_dims, plot_flags)) {
        ImPlot::SetupAxes(nullptr, nullptr, axes_flags, axes_flags);
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, sketch::res, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, sketch::res, ImGuiCond_Always);

        // Bresenham line rasterization lambda
        auto draw_line = [&](int x0, int y0, int x1, int y1) {
            int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
            int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
            int err = (dx > dy ? dx : -dy) / 2;
            int x = x0, y = y0;
            while (true) {
                // Stamp brush at (x,y)
                for (int dyb = -sketch::brush_size + 1; dyb < sketch::brush_size; ++dyb) {
                    for (int dxb = -sketch::brush_size + 1; dxb < sketch::brush_size; ++dxb) {
                        int sxp = x + dxb;
                        int syp = y + dyb;
                        if (sxp >= 0 && sxp < sketch::res && syp >= 0 && syp < sketch::res) {
                            if (sketch::brush_shape == 0 || (dxb*dxb + dyb*dyb) < sketch::brush_size*sketch::brush_size) {
                                sketch::data[(sketch::res - 1 - syp) * sketch::res + sxp] = sketch::paint_value;
                            }
                        }
                    }
                }
                if (x == x1 && y == y1) break;
                int e2 = err;
                if (e2 > -dx) { err -= dy; x += sx; }
                if (e2 <  dy) { err += dx; y += sy; }
            }
        };

        // Paint or erase on drag
        if (ImPlot::IsPlotHovered() && ImGui::IsMouseDown(0)) {
            ImPlotPoint mp = ImPlot::GetPlotMousePos();
            int cx = static_cast<int>(mp.x);
            int cy = static_cast<int>(mp.y);
            if (prev_x < 0) {
                // first sample: just draw single point
                draw_line(cx, cy, cx, cy);
            } else {
                // connect from previous to current
                draw_line(prev_x, prev_y, cx, cy);
            }
            sketch::drawing = true;
            prev_x = cx; prev_y = cy;
        }
        else {
            sketch::drawing = false;
            prev_x = prev_y = -1;
        }

        // Render heatmap
        ImPlot::PlotHeatmap("heat", sketch::data.data(), sketch::res, sketch::res,
                            sketch::vmin, sketch::vmax, nullptr,
                            ImPlotPoint(0,0), ImPlotPoint((double)sketch::res, (double)sketch::res), hm_flags);

        // Brush cursor preview
        if (ImPlot::IsPlotHovered()) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_None);
            ImPlotPoint mp = ImPlot::GetPlotMousePos();
            ImVec2 center = ImPlot::PlotToPixels(mp);
            float scale  = ImPlot::PlotToPixels(ImPlotPoint(1,0)).x - ImPlot::PlotToPixels(ImPlotPoint(0,0)).x;
            float half   = sketch::brush_size * scale * 0.5f;
            ImDrawList* dl = ImGui::GetForegroundDrawList();
            if (sketch::brush_shape == 0) {
                dl->AddRect({center.x-half, center.y-half}, {center.x+half, center.y+half}, IM_COL32(255,0,0,255), 0, 0, 2.0f);
            } else {
                dl->AddCircle(center, half, IM_COL32(255,0,0,255), 0, 2.0f);
            }
        }

        ImPlot::EndPlot();
    }
    ImGui::SameLine();
    ImPlot::ColormapScale("##HeatScale", sketch::vmin, sketch::vmax, scale_dims);
    ImGui::EndGroup();
}






// *************************************************************************** //
//
//
//  ?.      UTILITY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "DefaultTabRenderFunc"
//
void GraphApp::DefaultTabRenderFunc([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags) {
    if (!p_open)
        return;
        
    ImGui::Text("Here is some text on a window with no render function.");
    return;
}






// *************************************************************************** //
//
//
//  3.      PRIVATE MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

















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

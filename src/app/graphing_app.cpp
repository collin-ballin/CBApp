//
//  graphing_app.cpp
//  CBApp
//
//  Created by Collin Bond on 4/29/25.
//
// *************************************************************************** //
// *************************************************************************** //
#include "app/_graphing_app.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//  0.      STATIC VARIABLES...
// *************************************************************************** //
// *************************************************************************** //

//  STATIC MEMBERS...
using dim_type      = GraphingApp::dim_type;
using value_type    = GraphingApp::value_type;

static constexpr dim_type                               ms_control_width    = dim_type(225);
static constexpr dim_type                               ms_scale_width      = dim_type(90);
static constexpr std::pair<dim_type, dim_type>          ms_width_range      = { dim_type(500),          dim_type(1200) };
static constexpr std::pair<dim_type, dim_type>          ms_height_range     = { dim_type(500),          dim_type(1200) };
static constexpr std::pair<value_type, value_type>      ms_vlimits          = { value_type(-10),        value_type(10) };






// *************************************************************************** //
//
//
//  1.      INITIALIZATION  | DEFAULT CONSTRUCTOR, DESTRUCTOR, ETC...
// *************************************************************************** //
// *************************************************************************** //

//  Parametric Constructor 1.
//
GraphingApp::GraphingApp(const size_type rows_, const size_type cols_)
    : m_heatmap(rows_, cols_)
{
    //this->m_heatmap     = cb::HeatMap(rows_, cols_);
    //this->init();
}


//  "initialize"
//
void GraphingApp::initialize(void)
{
    if (this->m_initialized)
        return;
        
    this->init();
    return;
}


//  "init"
//
void GraphingApp::init(void)
{
    CONTROL_WIDTH                               = ImGui::GetFontSize() * 8;
    m_window_class.DockNodeFlagsOverrideSet     = ImGuiDockNodeFlags_NoTabBar;
    
    this->m_initialized                         = true;
    return;
}



//  Destructor.
//
GraphingApp::~GraphingApp(void)     { this->destroy(); }


//  "destroy"
//
void GraphingApp::destroy(void)
{
    return;
}





// *************************************************************************** //
//
//
//  1B.     PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "RebuildDockspace"
//
void GraphingApp::RebuildDockspace(void)
{
    IM_ASSERT(m_initialized == true && "IMGUI ASSERTION FAILURE:\n\tAttempt to call \"GraphingApp::RebuildDockspace()\" before \"GraphingApp::initialize()\" was initialized.\n");
    
        
    m_dockspace_id          = ImGui::GetID(m_dockspace_name);
    
    
    
    ImGui::DockSpace(m_dockspace_id,    ImVec2(0.0f, 0.0f),     m_dockspace_flags);


        ImGui::DockBuilderRemoveNode    (m_dockspace_id); // clear any previous layout
        ImGui::DockBuilderAddNode       (m_dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize   (m_dockspace_id, ImVec2(800, 600));

        // Example split: left and right
        m_ctrl_dock_id= ImGui::DockBuilderSplitNode( m_dockspace_id, ImGuiDir_Down, m_dockspace_ratio,
                                                      nullptr, &m_plot_dock_id );

        ImGui::DockBuilderDockWindow(m_ctrl_win_uuid,       m_ctrl_dock_id);
        ImGui::DockBuilderDockWindow(m_plot_win_uuid,       m_plot_dock_id);

        ImGui::DockBuilderFinish(m_dockspace_id);

    return;
}



//  "Begin"
//
void GraphingApp::Begin([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
{
    static bool                     init                    = true;
    static constexpr const char *   controls_uuid           = "Controls";

    //  1.  CREATING THE HOST WINDOW...
    ImGui::Begin(uuid, p_open, flags);

        //  Generate a persistent dockspace ID
        m_dockspace_id          = ImGui::GetID(m_dockspace_name);
        
        
        
        ImGui::DockSpace(m_dockspace_id,    ImVec2(0.0f, 0.0f),     m_dockspace_flags);

        if (init) {
            init = false;
            ImGui::DockBuilderRemoveNode    (m_dockspace_id); // clear any previous layout
            ImGui::DockBuilderAddNode       (m_dockspace_id, ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize   (m_dockspace_id, ImVec2(800, 600));

            // Example split: left and right
            m_ctrl_dock_id= ImGui::DockBuilderSplitNode( m_dockspace_id, ImGuiDir_Down, m_dockspace_ratio,
                                                          nullptr, &m_plot_dock_id );

            ImGui::DockBuilderDockWindow(m_ctrl_win_uuid,       m_ctrl_dock_id);
            ImGui::DockBuilderDockWindow(m_plot_win_uuid,       m_plot_dock_id);

            ImGui::DockBuilderFinish(m_dockspace_id);
        }

    ImGui::End();
    
    
    //  2.  CREATE BOTTOM WINDOW FOR CONTROLS...
    ImGui::SetNextWindowClass(&this->m_window_class);
    ImGui::Begin(m_ctrl_win_uuid, p_open, m_docked_win_flags);
        //this->sketch_controls();
        this->display_control_panel();
    ImGui::End();
    
    
    //  3.  CREATE BOTTOM WINDOW FOR CONTROLS...
    //ImGui::SetNextWindowClass(&this->m_window_class);
    ImGui::Begin(m_plot_win_uuid, p_open, m_docked_win_flags);
        //this->etch_a_sketch();
        this->test_table();
        //this->display_plot_panel();
    ImGui::End();
    
    return;
}

/*{
    ImGuiIO &                       io                      = ImGui::GetIO(); (void)io;
    ImGuiStyle &                    style                   = ImGui::GetStyle();
    static float                    SPACING                 = 0.0f;
    static float                    DEF_SPACING             = 1.5 * ImGui::GetTextLineHeightWithSpacing();
    static float                    TEXT_SPACING            = 8.0 * ImGui::GetTextLineHeightWithSpacing();
    static constexpr const char *   controls_uuid           = "Controls";
        //ImGui::Dummy( ImVec2(0.0f, ImGui::GetContentRegionAvail().y - SPACING) );
    
    
    
    //  1.  CREATE BOTTOM WINDOW FOR CONTROLS...
    ImGui::Begin(controls_uuid, p_open, flags);
        this->display_control_panel();
    ImGui::End();
    
    //  2.  CREATE TOP WINDOW FOR PLOTS...
    ImGui::Begin(uuid, p_open, flags);
        this->display_plot_panel();
    ImGui::End();
    
    
    
    return;
}*/



// *************************************************************************** //
//
//
//  2.      PROTECTED MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //



//  2A.     CHILD WINDOW FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "display_plot_panel"
//
void GraphingApp::display_plot_panel(void)
{
    static bool                 show_help_tab           = true;     //  Enable / Disable the "?" Tab Button.
    static bool                 enable_adding_tabs      = true;     //  Enable / Disable the "+" Add-Tab Button.
    static constexpr size_t     NO_CLOSE_IDX            = 3;
    
    
    //  CHILD WINDOW WIDGET VARIABLES...
    static int                  draw_lines              = 3;
    static ImVec2               win_dims_min, win_dims_max;
    static ImVec2               size_arg                = ImVec2(-FLT_MIN,      0.0f);
      
      
    win_dims_min    = ImVec2(0.0f,          ImGui::GetTextLineHeightWithSpacing() * MC_PLOT_HEIGHT_LIMIT.first);
    win_dims_max    = ImVec2(FLT_MAX,       ImGui::GetTextLineHeightWithSpacing() * MC_PLOT_HEIGHT_LIMIT.second);
    
    
    
    
      
    //  1.  DRAWING CHILD-WINDOW...
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding,            mc_plot_corner_radius);
    ImGui::SetNextWindowSizeConstraints(    win_dims_min,       win_dims_max );
    ImGui::BeginChild(mc_plot_child_id,     size_arg,           mc_plot_child_flags);
    
    
        //  2.  DRAWING THE TAB-BAR INSIDE THE CHILD WINDOW...
        if (ImGui::BeginTabBar(mc_plot_tab_bar_id, mc_plot_tabbar_flags))
        {
            //      2.3     DRAW THE MAIN TAB ITEMS.
            std::size_t     n   = 0ULL;
            for (auto & tab : MC_PLOT_TABS) {
            
                //  DRAW THE TAB...                               (n<2) ? nullptr : &std::get<1>(tab)   ===> Prevent first 2 tabs from being closed.
                if (ImGui::BeginTabItem( std::get<0>(tab).c_str(),
                                         (n < NO_CLOSE_IDX) ? nullptr : &std::get<1>(tab),
                                         std::get<2>(tab)) )
                {
                    switch (n)
                    {
                        case 0 : {      //      TAB 0 :     MAIN PLOT...
                        #ifdef CBAPP_ETCH_A_SKETCH
                            this->etch_a_sketch();
                        # else
                            this->m_heatmap.draw_plots();
                        #endif  //  CBAPP_ETCH_A_SKETCH  //
                            break;
                        }
                    //
                        case 1 : {      //      TAB 1 :     SECOND CONTROLS...
                        #ifdef CBAPP_ETCH_A_SKETCH
                            this->m_heatmap.draw_controls();
                        # else
                            this->etch_a_sketch();
                        #endif  //  CBAPP_ETCH_A_SKETCH  //
                            break;
                        }
                    //
                        case 2 : {      //      TAB 2 :     TABLE TESTING...
                            this->test_table();
                            break;
                        }
                    //
                        default : {     //      DEFAULT :   ALL REMAINING MC_PLOT_TABS...
                            ImGui::Text("This is the %zu tab.", n);
                            break;
                        }
                        
                    }// END "switch".
                    ImGui::EndTabItem();
                }// END "BeginTabItem".
                    
            
                //  2.4     IF THE USER DELETED A TAB...
                if (!std::get<1>(tab))      MC_PLOT_TABS.erase(MC_PLOT_TABS.begin() + n);
                else                        ++n;
                    
            }// END "for (auto & tab : MC_PLOT_TABS).


            ImGui::EndTabBar();
        }// END "BeginTabBar".
    
    //  END "Child Window".
    ImGui::EndChild();
    ImGui::PopStyleVar();


    return;
}







//  "display_control_panel"
//
void GraphingApp::display_control_panel(void)
{
    //  TAB-BAR VARIABLES...
    static bool                 show_help_tab           = true;     //  Enable / Disable the "?" Tab Button.
    static bool                 enable_adding_tabs      = true;     //  Enable / Disable the "+" Add-Tab Button.
    static constexpr size_t     NO_CLOSE_IDX            = 2;
    
    //  CHILD WINDOW WIDGET VARIABLES...
    static int                  draw_lines              = 3;
    static ImVec2               win_dims_min, win_dims_max;
    static ImVec2               size_arg                = ImVec2(-FLT_MIN,      0.0f);
      
      
    win_dims_min    = ImVec2(0.0f,          ImGui::GetTextLineHeightWithSpacing() * MC_CTRL_HEIGHT_LIMIT.first);
    win_dims_max    = ImVec2(FLT_MAX,       ImGui::GetTextLineHeightWithSpacing() * MC_CTRL_HEIGHT_LIMIT.second);
    
    
    
    
      
    //  1.  DRAWING CHILD-WINDOW...
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding,            mc_ctrl_corner_radius);
    ImGui::SetNextWindowSizeConstraints(    win_dims_min,       win_dims_max );
    ImGui::BeginChild("ControlChild",       size_arg,           mc_ctrl_child_flags);
    
    
    
        //  2.  DRAWING THE TAB-BAR INSIDE THE CHILD WINDOW...
        if (ImGui::BeginTabBar("ControlTabBar", mc_ctrl_tabbar_flags))
        {
            //      2.1     DRAW HELP-MENU TAB ITEM "?".
            if (show_help_tab) {
                if (ImGui::TabItemButton("?", ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_NoTooltip))
                    ImGui::OpenPopup("MyHelpMenu");
            }
            if (ImGui::BeginPopup("MyHelpMenu")) {
                ImGui::Selectable("Hello!");
                ImGui::EndPopup();
            }


            //      2.2     DRAW TRAILING "+" ADD-TAB BUTTON.
            if (enable_adding_tabs) {
                if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip))
                    {   MC_CTRL_TABS.push_back({std::string("Tab #") + std::to_string(MC_CTRL_TABS.size()+1), true, m_def_tab_flags});    }// Add new tab
            }


            //      2.3     DRAW THE MAIN TAB ITEMS.
            std::size_t     n   = 0ULL;
            for (auto & tab : MC_CTRL_TABS)
            {
                //  DRAW THE TAB...                               (n<2) ? nullptr : &std::get<1>(tab)   ===> Prevent first 2 tabs from being closed.
                if (ImGui::BeginTabItem( std::get<0>(tab).c_str(),
                                         (n < NO_CLOSE_IDX) ? nullptr : &std::get<1>(tab),
                                         std::get<2>(tab)) )
                {
                    switch (n)
                    {
                        case 0 : {      //      TAB 0 :     MAIN CONTROLS...
                        #ifdef CBAPP_ETCH_A_SKETCH
                            this->sketch_controls();
                        # else
                            this->m_heatmap.draw_controls();
                        #endif  //  CBAPP_ETCH_A_SKETCH  //
                            break;
                        }
                    //
                        case 1 : {      //      TAB 1 :     SECOND CONTROLS...
                        #ifdef CBAPP_ETCH_A_SKETCH
                            this->m_heatmap.draw_controls();
                        # else
                            this->sketch_controls();
                        #endif  //  CBAPP_ETCH_A_SKETCH  //
                            break;
                        }
                    //
                        default : {     //      DEFAULT :   ALL REMAINING MC_PLOT_TABS...
                            ImGui::Text("This is the %zu tab.", n);
                            break;
                        }
                    //
                    }// END "switch".
                    ImGui::EndTabItem();
                }
                    
            
                //  2.4     IF THE USER DELETED A TAB...
                if (!std::get<1>(tab))      MC_CTRL_TABS.erase(MC_CTRL_TABS.begin() + n);
                else                        ++n;
                    
            }// END "for (auto & tab : MC_CTRL_TABS).


            ImGui::EndTabBar();
        }// END "BeginTabBar".
    
    
    //  END "Child Window".
    ImGui::EndChild();
    ImGui::PopStyleVar();

    return;
}



// *************************************************************************** //
//
//
//  ?.      "ETCH - A - SKETCH"  FUNCTIONS...
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


//  "etch_a_sketch"
//
void GraphingApp::etch_a_sketch(void)
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





//  "sketch_controls"
//
void GraphingApp::sketch_controls(void)
{
    //  CONSTANTS...
    static constexpr float              LABEL_COLUMN_WIDTH      = 200.0f;
    static constexpr float              WIDGET_COLUMN_WIDTH     = 250.0f;

    //  INTERACTIVE VARIABLES...
    static int                          param_a                 = 3;
    static int                          param_b                 = 7;
    static bool                         toggle                  = true;
    static ImVec4                       color                   = {0.5f, 0.5f, 1.0f, 1.0f};

    //  TABLE GLOBAL FLAGS...
    static bool                         freeze_header           = false;
    static bool                         freeze_column           = false;
    static bool                         stretch_column_1        = true;

    //  COLUMN-SPECIFIC FLAGS...
    
    static ImGuiTableColumnFlags        col0_flags              = ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize;
    static ImGuiTableColumnFlags        col1_flags              = stretch_column_1 ? ImGuiTableColumnFlags_WidthStretch : ImGuiTableColumnFlags_WidthFixed;
    static ImGuiTableFlags              flags                   = ImGuiTableFlags_None | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoKeepColumnsVisible; //| ImGuiTableFlags_ScrollX;
        
    static const utl::WidgetRow rows[]                          = {
        {"Clear Sketch",        []{ if (ImGui::Button("Clear Canvas")) std::fill(sketch::data.begin(), sketch::data.end(), 0.0f); }                                                                                             },
        {"Sketch Resolution",   []{ float w = ImGui::GetColumnWidth(); ImGui::SetNextItemWidth(w); ImGui::SliderInt("##SketchResolution",&sketch::res,16, 256); }                                                               },
    //
        {"Brush Size",          []{ float w = ImGui::GetColumnWidth(); ImGui::SetNextItemWidth(w); ImGui::SliderInt("##BrushSize", &sketch::brush_size, 1, 8);      }                                                           },
        {"Brush Shape",         []{ float w = ImGui::GetColumnWidth(); ImGui::SetNextItemWidth(w); ImGui::Combo("##BrushShape",        &sketch::brush_shape, sketch::brush_shapes, IM_ARRAYSIZE(sketch::brush_shapes));  }      },
        {"Paint Value",         []{ float w = ImGui::GetColumnWidth(); ImGui::SetNextItemWidth(w); ImGui::SliderFloat("##BrushValue",  &sketch::paint_value, sketch::vmin, sketch::vmax); }                                     },
    //
        {"Colormap",            []{
            float w = ImGui::GetColumnWidth();
            if (ImPlot::ColormapButton(ImPlot::GetColormapName(sketch::cmap), ImVec2(w, 0), sketch::cmap))
            {
                sketch::cmap = (sketch::cmap + 1) % ImPlot::GetColormapCount();
                ImPlot::BustColorCache(sketch::heatmap_uuid);
            }
            ImPlot::PushColormap(sketch::cmap);
        }}
    };



    //  1.  PRIMARY TABLE ENTRY...
    //
    if (ImGui::BeginTable("SketchControls", 2, flags))
    {
        if (freeze_column || freeze_header)
            ImGui::TableSetupScrollFreeze(freeze_column ? 1 : 0, freeze_header ? 1 : 0);


        ImGui::TableSetupColumn("Label",    col0_flags,     LABEL_COLUMN_WIDTH);
        ImGui::TableSetupColumn("Widget",   col1_flags,     stretch_column_1 ? 1.0f : WIDGET_COLUMN_WIDTH);
        ImGui::TableHeadersRow();

        for (const auto & row : rows) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted(row.label);
            ImGui::TableSetColumnIndex(1);
            row.render();
        }

        ImGui::EndTable();
    }
    
    
    return;
}



// *************************************************************************** //
//
//
//  ?.      TESTING TABLE APPEARANCE...
// *************************************************************************** //
// *************************************************************************** //

//  "test_table"
//
void GraphingApp::test_table(void)
{
    // Constants
    static constexpr float LABEL_COLUMN_WIDTH  = 150.0f;
    static constexpr float WIDGET_COLUMN_WIDTH = 250.0f;

    // Actual controlled variables
    static int    param_a = 3;
    static int    param_b = 7;
    static bool   toggle  = true;
    static ImVec4 color   = {0.5f, 0.5f, 1.0f, 1.0f};

    // Table flags (interactive toggles)
    static bool show_borders     = true;
    static bool show_row_bg      = true;
    static bool resizable        = false;
    static bool no_keep_cols     = true;
    static bool enable_scroll_x  = false;
    static bool enable_scroll_y  = false;
    static bool freeze_header    = false;
    static bool freeze_column    = false;
    static bool enable_sorting   = false;
    static bool enable_hiding    = false;
    static bool stretch_column_1 = false;

    // Per-column flags
    static bool col0_no_resize = false;
    static bool col0_no_sort   = false;
    static bool col0_default_hide = false;

    static bool col1_no_resize = false;
    static bool col1_no_sort   = false;
    static bool col1_default_hide = false;

    if ( ImGui::TreeNode("Table Options") )
    {
        ImGui::Checkbox("Show Borders", &show_borders);
        ImGui::Checkbox("Alternate Row Background", &show_row_bg);
        ImGui::Checkbox("Resizable Columns", &resizable);
        ImGui::Checkbox("No Keep Columns Visible", &no_keep_cols);
        ImGui::Checkbox("Scroll X", &enable_scroll_x);
        ImGui::Checkbox("Scroll Y", &enable_scroll_y);
        ImGui::Checkbox("Freeze Header Row", &freeze_header);
        ImGui::Checkbox("Freeze Label Column", &freeze_column);
        ImGui::Checkbox("Enable Sorting", &enable_sorting);
        ImGui::Checkbox("Allow Hiding Columns", &enable_hiding);
        ImGui::Checkbox("Stretch Widget Column", &stretch_column_1);
        ImGui::Spacing();
        
        ImGui::TreePop();
    }


    if ( ImGui::TreeNode("Column Settings") )
    {
        if (ImGui::BeginTable("ColumnSettings", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("Label Column");
            ImGui::TableSetupColumn("Widget Column");
            ImGui::TableHeadersRow();

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Checkbox("NoResize##0", &col0_no_resize);
            ImGui::TableSetColumnIndex(1); ImGui::Checkbox("NoResize##1", &col1_no_resize);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Checkbox("NoSort##0", &col0_no_sort);
            ImGui::TableSetColumnIndex(1); ImGui::Checkbox("NoSort##1", &col1_no_sort);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Checkbox("DefaultHide##0", &col0_default_hide);
            ImGui::TableSetColumnIndex(1); ImGui::Checkbox("DefaultHide##1", &col1_default_hide);

            ImGui::EndTable();
        }
        
        ImGui::TreePop();
    }



    // Combine global table flags
    ImGuiTableFlags flags = 0;
    if (show_borders)    flags |= ImGuiTableFlags_Borders;
    if (show_row_bg)     flags |= ImGuiTableFlags_RowBg;
    if (resizable)       flags |= ImGuiTableFlags_Resizable;
    if (no_keep_cols)    flags |= ImGuiTableFlags_NoKeepColumnsVisible;
    if (enable_scroll_x) flags |= ImGuiTableFlags_ScrollX;
    if (enable_scroll_y) flags |= ImGuiTableFlags_ScrollY;
    if (enable_sorting)  flags |= ImGuiTableFlags_Sortable;
    if (enable_hiding)   flags |= ImGuiTableFlags_Hideable;

    // Declarative per-row widget logic
    struct WidgetRow {
        const char* label;
        std::function<void()> render;
    };

    static const WidgetRow rows[] = {
        {"Param A", []{ float w = ImGui::GetColumnWidth(); ImGui::SetNextItemWidth(w); ImGui::SliderInt("##A", &param_a, 0, 10); }},
        {"Param B", []{ float w = ImGui::GetColumnWidth(); ImGui::SetNextItemWidth(w); ImGui::SliderInt("##B", &param_b, 0, 10); }},
        {"Enabled", []{ float w = ImGui::GetColumnWidth(); ImGui::SetNextItemWidth(w); ImGui::Checkbox("##Toggle", &toggle); }},
        {"Color",   []{ float w = ImGui::GetColumnWidth(); ImGui::SetNextItemWidth(w); ImGui::ColorEdit4("##Color", (float*)&color); }},
    };

    if (ImGui::BeginTable("WidgetTable", 2, flags)) {
        if (freeze_column || freeze_header)
            ImGui::TableSetupScrollFreeze(freeze_column ? 1 : 0, freeze_header ? 1 : 0);

        ImGuiTableColumnFlags col0_flags = ImGuiTableColumnFlags_WidthFixed;
        if (col0_no_resize)     col0_flags |= ImGuiTableColumnFlags_NoResize;
        if (col0_no_sort)       col0_flags |= ImGuiTableColumnFlags_NoSort;
        if (col0_default_hide)  col0_flags |= ImGuiTableColumnFlags_DefaultHide;

        ImGuiTableColumnFlags col1_flags = stretch_column_1 ? ImGuiTableColumnFlags_WidthStretch : ImGuiTableColumnFlags_WidthFixed;
        if (col1_no_resize)     col1_flags |= ImGuiTableColumnFlags_NoResize;
        if (col1_no_sort)       col1_flags |= ImGuiTableColumnFlags_NoSort;
        if (col1_default_hide)  col1_flags |= ImGuiTableColumnFlags_DefaultHide;

        ImGui::TableSetupColumn("Label", col0_flags, LABEL_COLUMN_WIDTH);
        ImGui::TableSetupColumn("Widget", col1_flags, stretch_column_1 ? 1.0f : WIDGET_COLUMN_WIDTH);
        ImGui::TableHeadersRow();

        for (const auto& row : rows) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted(row.label);
            ImGui::TableSetColumnIndex(1);
            row.render();
        }

        ImGui::EndTable();
    }
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

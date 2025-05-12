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
#include "app/c_counter/c_counter.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//  0.      STATIC VARIABLES...
// *************************************************************************** //
// *************************************************************************** //

struct ChannelSpec { int idx; const char * name; };


//  STATIC VARIABLES FOR COINCIDENCE COUNTER...
namespace cc {
    //  MISC STUFF...
    static float                        w                       = 200.0f;
    
    //  SKETCH STUFF...
    static const char *                 heatmap_uuid            = "##EtchASketch";
    static int                          res                     = 128;
    static int                          last_res                = -1;
    static float                        vmin                    = 0.0f,     vmax = 1.0f;
    static ImPlotColormap               cmap                    = ImPlotColormap_Cool;
    static std::vector<float>           data;
    
    //  BRUSH STUFF...
    static const char *                 brush_shapes[]          = {"Square", "Circle"};
    static int                          brush_size              = 1;
    static int                          brush_shape             = 0; // 0 = square, 1 = circle
    static float                        paint_value             = 1.0f; // value to paint into the grid
    static bool                         drawing                 = false;


    //  COPIED...
    constexpr int                   NUM                         = 15;                 // skip index 0 (UNUSED)
    constexpr float                 CENTER                      = 0.75f;
    static constexpr ChannelSpec    channels[NUM]               = {
        { 8,  "A"   }, { 4,  "B"   }, { 2,  "C"   }, { 1,  "D"   },
        {12,  "AB"  }, {10,  "AC"  }, { 9,  "AD"  }, { 6,  "BC"  },
        { 5,  "BD"  }, { 3,  "CD"  }, {14,  "ABC" }, {13,  "ABD" },
        {11,  "ACD" }, { 7,  "BCD" }, {15,  "ABCD"}
    };
    
/*
    //  Per‑channel ring buffers for ImPlot
    static std::array<utl::ScrollingBuffer, NUM_CHANNELS>   buffers;
    static float                    max_counts[NUM_CHANNELS]    = {};

    //  Python process wrapper
    static utl::PyStream            proc(app::PYTHON_DUMMY_FPGA_FILEPATH);      //  Python process wrapper
    static float                    delay_s                     = 1.0f;         //  Command slider
    static bool                     started                     = false;
    static float                    history_s                   = 30.0f;        //  Seconds visible in sparklines
    static float                    row_height_px               = 60.0f;
    static char                     line_buf[256]{};                            //  Manual send box
    static ImPlotAxisFlags          m_plot_flags                  = ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoDecorations;
*/

    static ImVec2                   HEADER_SEP_TOP              = ImVec2();
    static ImVec2                   HEADER_SEP_BOTTOM           = ImVec2();
}


//  Per‑channel ring buffers for ImPlot
static std::array<utl::ScrollingBuffer, cc::NUM>   buffers;
static float                    max_counts[cc::NUM]         = {};

//  Python process wrapper
static utl::PyStream            proc(app::PYTHON_DUMMY_FPGA_FILEPATH);
static bool                     started                     = false;
static float                    history_s                   = 30.0f;                // seconds visible in sparklines
static float                    row_height_px               = 60.0f;
static float                    master_row_height_px        = 400.f;                // independent of per‑row height
static float                    delay_s                     = 1.0f;                 // command slider
static char                     line_buf[256]{};                                    // manual send box





// *************************************************************************** //
//
//
//  1.      INITIALIZATION  | DEFAULT CONSTRUCTOR, DESTRUCTOR, ETC...
// *************************************************************************** //
// *************************************************************************** //

//  "initialize"
//
void CCounterApp::initialize(void)
{
    if (this->m_initialized)
        return;
        
    this->init();
    return;
}


//  "init"
//
void CCounterApp::init(void)
{

    COUNTER_COL_WIDTH           = 80.0f; //ImGui::CalcTextSize("999999").x;
    CONTROL_WIDTH               = ImGui::GetFontSize() * 8;
    cc::HEADER_SEP_TOP          = ImVec2( 0.0f, 0.5 * ImGui::GetTextLineHeightWithSpacing() );
    cc::HEADER_SEP_BOTTOM       = ImVec2( 0.0f, 0.0 * ImGui::GetTextLineHeightWithSpacing() );
    
    
    
    
    //  Assign "Window Class" Properties...
    m_window_class.DockNodeFlagsOverrideSet     = ImGuiDockNodeFlags_NoTabBar;
    
    
    this->m_initialized                         = true;
    return;
}



//  Destructor.
//
CCounterApp::~CCounterApp(void)     { this->destroy(); }


//  "destroy"
//
void CCounterApp::destroy(void)
{
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
void CCounterApp::Begin([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
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
            m_dock_ids[1]   = ImGui::DockBuilderSplitNode( m_dockspace_id, ImGuiDir_Down, m_dockspace_ratio,
                                                           nullptr, &m_dock_ids[0] );


            ImGui::DockBuilderDockWindow(m_win_uuids[0],    m_dock_ids[0]);
            ImGui::DockBuilderDockWindow(m_win_uuids[1],    m_dock_ids[1]);
            ImGui::DockBuilderFinish(m_dockspace_id);
        }
    ImGui::End();
    
    
    //  2.  CREATE TOP WINDOW FOR PLOTS...
    ImGui::SetNextWindowClass(&this->m_window_class);
    ImGui::Begin(m_win_uuids[0], p_open, m_docked_win_flags[0]);
        this->display_plots();
    ImGui::End();
    
    
    //  3.  CREATE BOTTOM WINDOW FOR CONTROLS...
    ImGui::SetNextWindowClass(&this->m_window_class);
    ImGui::Begin(m_win_uuids[1], p_open, m_docked_win_flags[1]);
        this->display_controls();
    ImGui::End();
    
    return;
}




// *************************************************************************** //
//
//
//  2.      PROTECTED MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //





// *************************************************************************** //
//
//
//  2.  COINCIDENCE COUNTER TESTING FUNC...
// *************************************************************************** //
// *************************************************************************** //

//  "display_plots"
//
void CCounterApp::display_plots(void)
{


/*
    //  2.  CONTROL PANEL...
    //
    //
    ImGui::Dummy( cc::HEADER_SEP_TOP );
    ImGui::SeparatorText("Control Panel");
    ImGui::Dummy( cc::HEADER_SEP_BOTTOM );
    //
    if (ImGui::SliderFloat("Acquisition delay (s)", &delay_s, 0.05f, 5.0f, "%.2f")) {
        if (started) {
            char cmd[48];
            std::snprintf(cmd, sizeof(cmd), "delay %.3f\n", delay_s);
            proc.send(cmd);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Apply") && started) {
        char cmd[48];
        std::snprintf(cmd, sizeof(cmd), "delay %.3f\n", delay_s);
        proc.send(cmd);
    }

    // Launch/terminate Python child
    if (!started) {
        if (ImGui::Button("Start process")) {
            started = proc.start();
            if (!started) ImGui::OpenPopup("launch_error");
            else          max_counts[0] = 0.f; // reset stats
        }
    } else {
        if (ImGui::Button("Stop process")) {
            proc.stop();
            started = false;
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(running)");
    }

    if (ImGui::BeginPopupModal("launch_error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Failed to launch Python process!");
        if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }


    ImGui::Separator();

    // Manual raw line send (developer aid)
    ImGui::InputText("send", line_buf, IM_ARRAYSIZE(line_buf), ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::SameLine();
    if (ImGui::Button("Send") || ImGui::IsItemDeactivatedAfterEdit()) {
        if (started && line_buf[0]) {
            proc.send(line_buf);   // passthrough; must include \n if desired
            line_buf[0] = '\0';
        }
    }

    ImGui::Separator();
*/



    //---------------------------------------------
    // 3)  Poll child stdout and update buffers
    //---------------------------------------------
    std::string         raw;
    const float         now         = static_cast<float>(ImGui::GetTime());
    while (proc.try_receive(raw))
    {
        if (auto pkt = utl::parse_packet(raw))
        {
            const auto &    counts = pkt->counts; // std::array<int,16>
            
            for (int i = 0; i < cc::NUM; ++i)
            {
                int     channel_idx     = cc::channels[i].idx;
                float   v               = static_cast<float>(counts[channel_idx]);
                buffers[i].AddPoint(now, v);
                max_counts[i]           = std::max(max_counts[i], v);
            }
        }
    }








    //  5.      "MASTER" PLOT FOR MASTER PLOT...
    //
    //ImGui::Dummy( cc::HEADER_SEP_TOP );
    //ImGui::SeparatorText("Master Plot");
    //ImGui::Dummy( cc::HEADER_SEP_BOTTOM );
    //
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::CollapsingHeader("Master Plot") )
    {
        //
        ImGui::PushID("master");
        if (ImPlot::BeginPlot("##master", ImVec2(-1, master_row_height_px), m_mst_plot_flags))
        {
        
            //  SPARKLINE PROTOTYPE:
            //
            //      void ScrollingSparkline(const float time,               const float window,     ScrollingBuffer & data,
            //                  const ImPlotAxisFlags flags,    const ImVec4 & color,   const ImVec2 & size,
            //                  const float center)
                   
            //  SPARKLINE CALLS:
            //                                                    |---  ScrollingBuffer
            //                                                    |
            //      ScrollingSparkline( now,     history_s,      buf,    m_plot_flags,      ImPlot::GetColormapColor(row),  ImVec2(-1, row_height_px),  cc::CENTER);
            //                           |        |               |       |                  |                                |                          |
            //                          time     window          data    flags              color (ImVec4)                   size                       center
            //
            //
            //      ImPlot::PlotLine(   "Mouse X",  &data.Data[0].x,    &data.Data[0].y,    data.Data.size(),   ImPlotLineFlags_Shaded,     data.Offset,    2 * sizeof(float)   );
            //
            //
        
        
            //  1.  SETUP THE PLOT...
            //          - Enable grid on both axes, keep no decorations.
            //
            ImPlot::SetupAxes(nullptr, nullptr,     m_mst_xaxis_flags,  m_mst_yaxis_flags);
            ImPlot::SetupLegend(m_mst_legend_loc,   m_mst_legend_flags);
            
            //      1.1.    X-Limits.
            float   xmin    = now - cc::CENTER * history_s;
            float   xmax    = xmin + history_s;
            ImPlot::SetupAxisLimits(ImAxis_X1, xmin, xmax, ImGuiCond_Always);



            //  2.  ADD EACH COUNTER TO THE SAME PLOT...
            //
            for (int k = 0; k < cc::NUM; ++k)
            {
                const auto &    data        = buffers[k];
                const auto &    color       = ImPlot::GetColormapColor(k);
                
                
                //  3.  ADDING A PLOT...
                ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0,0));
                {
                    ImPlot::SetNextLineStyle(   color,      3.0);
                    ImPlot::SetNextFillStyle(   color,      0.0);
                    if (!data.Data.empty()) {
                        ImPlot::PlotLine( cc::channels[k].name,     &data.Data[0].x,     &data.Data[0].y,     data.Data.size(),        ImPlotLineFlags_Shaded, data.Offset, 2 * sizeof(float));
                    }
                    
                    
                }// END "Adding A Plot".
                ImPlot::PopStyleVar();
                
                
            }   //  END "For-Loop" THRU EACH PLOT.
            ImPlot::EndPlot();
        }
        ImGui::PopID();
    //
    //
    //
    //ImGui::TreePop();
    }// END TREE NODE.
                
        

    //  6.  DRAW THE TABLE OF EACH INDIVIDUAL COUNTER...
    //
    //  ImGui::Dummy( cc::HEADER_SEP_TOP );
    //  ImGui::SeparatorText("Individual Counters");
    //  ImGui::Dummy( cc::HEADER_SEP_BOTTOM );
    //
    ImGui::NewLine();
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::CollapsingHeader("Individual Counters") )
    {
        if (ImGui::BeginTable("##coinc_table",      5,      ImGuiTableFlags_BordersOuter | ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupColumn("Counter(s)",       ImGuiTableColumnFlags_WidthFixed,   COUNTER_COL_WIDTH);
            ImGui::TableSetupColumn("Max",              ImGuiTableColumnFlags_WidthFixed,   COUNTER_COL_WIDTH);
            ImGui::TableSetupColumn("Avg.",             ImGuiTableColumnFlags_WidthFixed,   COUNTER_COL_WIDTH);
            ImGui::TableSetupColumn("Current",          ImGuiTableColumnFlags_WidthFixed,   COUNTER_COL_WIDTH);
            ImGui::TableSetupColumn("Plot");
            ImGui::TableHeadersRow();


            //  6.2     PLOT FOR EACH CHANNEL...
            for (int row = 0; row < cc::NUM; ++row)
            {
                const int           idx         = cc::channels[row].idx;
                auto &              buf         = buffers[row];
                const auto &        vec         = buf.Data;

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted( cc::channels[row].name );
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.0f", max_counts[row]);
                ImGui::TableSetColumnIndex(2);
                float avg = 0.f;
                if (!vec.empty()) {
                    for (const auto &pt : vec) avg += pt.y;
                    avg /= static_cast<float>(vec.size());
                }
                ImGui::Text("%.1f", avg);
                ImGui::TableSetColumnIndex(3);
                float curr = vec.empty() ? 0.f : vec.back().y;
                ImGui::Text("%.0f", curr);
                ImGui::TableSetColumnIndex(4);

                ImGui::PushID(row);
                if (!vec.empty()) {
                    ScrollingSparkline(now, history_s, buf, m_plot_flags,
                                       ImPlot::GetColormapColor(row),
                                       ImVec2(-1, row_height_px), cc::CENTER);
                }
                ImGui::PopID();
            }// END "plot for each channel".
            
            
            
            ImGui::EndTable();
        }// END "table".
    //
    //
    //
    }// END TREE NODE.


    //ImPlot::PopColormap();
    //ImGui::End();
}




























// *************************************************************************** //
//
//
//  2.  PRIMARY "PLOT" AND "CONTROL" FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "display_plots"
//
/*
void CCounterApp::display_plots(void)
{
    // 1. CONSTANTS AND VARIABLES
    static constexpr float LABEL_WIDTH  = 150.0f;
    static constexpr float WIDGET_WIDTH = 250.0f;

    // 1.1 Layout flags
    ImPlotHeatmapFlags hm_flags   = 0;
    ImPlotAxisFlags    axes_flags = ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoGridLines |
                                   ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoDecorations;
    ImPlotFlags        m_plot_flags = ImPlotFlags_NoLegend;
    ImVec2             avail      = ImGui::GetContentRegionAvail();
    ImVec2             scale_dims = ImVec2(80.0f, avail.y);
    ImVec2             plot_dims  = ImVec2(avail.x - scale_dims.x - ImGui::GetStyle().ItemSpacing.x, avail.y);

    // Resize buffer if resolution changed
    if (cc::res != cc::last_res) {
        cc::data.assign(cc::res * cc::res, 0.0f);
        cc::last_res = cc::res;
    }

    // Track last drawn grid cell for Bresenham
    static int prev_x = -1, prev_y = -1;

    ImGui::BeginGroup();
    if (ImPlot::BeginPlot(cc::heatmap_uuid, plot_dims, m_plot_flags)) {
        ImPlot::SetupAxes(nullptr, nullptr, axes_flags, axes_flags);
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, cc::res, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, cc::res, ImGuiCond_Always);

        // Bresenham line rasterization lambda
        auto draw_line = [&](int x0, int y0, int x1, int y1) {
            int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
            int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
            int err = (dx > dy ? dx : -dy) / 2;
            int x = x0, y = y0;
            while (true) {
                // Stamp brush at (x,y)
                for (int dyb = -cc::brush_size + 1; dyb < cc::brush_size; ++dyb) {
                    for (int dxb = -cc::brush_size + 1; dxb < cc::brush_size; ++dxb) {
                        int sxp = x + dxb;
                        int syp = y + dyb;
                        if (sxp >= 0 && sxp < cc::res && syp >= 0 && syp < cc::res) {
                            if (cc::brush_shape == 0 || (dxb*dxb + dyb*dyb) < cc::brush_size*cc::brush_size) {
                                cc::data[(cc::res - 1 - syp) * cc::res + sxp] = cc::paint_value;
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
            cc::drawing = true;
            prev_x = cx; prev_y = cy;
        }
        else {
            cc::drawing = false;
            prev_x = prev_y = -1;
        }

        // Render heatmap
        ImPlot::PlotHeatmap("heat", cc::data.data(), cc::res, cc::res,
                            cc::vmin, cc::vmax, nullptr,
                            ImPlotPoint(0,0), ImPlotPoint((double)cc::res, (double)cc::res), hm_flags);

        // Brush cursor preview
        if (ImPlot::IsPlotHovered()) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_None);
            ImPlotPoint mp = ImPlot::GetPlotMousePos();
            ImVec2 center = ImPlot::PlotToPixels(mp);
            float scale  = ImPlot::PlotToPixels(ImPlotPoint(1,0)).x - ImPlot::PlotToPixels(ImPlotPoint(0,0)).x;
            float half   = cc::brush_size * scale * 0.5f;
            ImDrawList* dl = ImGui::GetForegroundDrawList();
            if (cc::brush_shape == 0) {
                dl->AddRect({center.x-half, center.y-half}, {center.x+half, center.y+half}, IM_COL32(255,0,0,255), 0, 0, 2.0f);
            } else {
                dl->AddCircle(center, half, IM_COL32(255,0,0,255), 0, 2.0f);
            }
        }

        ImPlot::EndPlot();
    }
    ImGui::SameLine();
    ImPlot::ColormapScale("##HeatScale", cc::vmin, cc::vmax, scale_dims);
    ImGui::EndGroup();
}*/





//  "display_controls"
//
void CCounterApp::display_controls(void)
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
        
        
        
    //  DEFINE EACH WIDGET IN CONTROL PANEL...
    //
    constexpr float                 margin                  = 0.75f;
    constexpr float                 pad                     = 10.0f;
    static const utl::WidgetRow     rows[]                  = {
    //
    //  1.  CONTROL PARAMETERS...
        {"Record",                                  []
            {// BEGIN.
                ImGui::SetNextItemWidth( margin * ImGui::GetColumnWidth() );
                ImGui::InputText("##file", line_buf, IM_ARRAYSIZE(line_buf),  ImGuiInputTextFlags_EnterReturnsTrue);
                ImGui::SameLine(0.0f, pad);
                //
                    if (!started)
                    {
                        if (ImGui::Button("Start Process", ImVec2(ImGui::GetContentRegionAvail().x - pad, 0)) ) {
                            started = proc.start();
                            if (!started)   ImGui::OpenPopup("launch_error");
                            else            max_counts[0] = 0.f; // reset stats
                        }
                    }
                    else
                    {
                        if (ImGui::Button("Stop Process", ImVec2(ImGui::GetContentRegionAvail().x - pad, 0)) ) {
                            proc.stop();
                            started = false;
                        }
                        ImGui::SameLine();
                        ImGui::TextDisabled("(running)");
                    }
                //
                ImGui::Dummy( ImVec2(pad, 0.0f) );



                if (ImGui::BeginPopupModal("launch_error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                    ImGui::Text("Failed to launch Python process!");
                    if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
                }
                
                if (started)    {
                    char cmd[48];
                    std::snprintf(cmd, sizeof(cmd), "delay %.3f\n", delay_s);
                    proc.send(cmd);
                }
                
            //
            //
            //
            } // END.
        },
        {"Communicate",                             []
            {// BEGIN.
                ImGui::BeginGroup();
                //
                //
                ImGui::SetNextItemWidth( margin * ImGui::GetColumnWidth() );
                if ( ImGui::InputText("##send", line_buf, IM_ARRAYSIZE(line_buf),  ImGuiInputTextFlags_EnterReturnsTrue) )   {
                    if (started)    {
                        char cmd[48];
                        std::snprintf(cmd, sizeof(cmd), "delay %.3f\n", delay_s);
                        proc.send(cmd);
                    }
                }
                ImGui::SameLine(0.0f, pad);
                if ( ImGui::Button("Send", ImVec2(ImGui::GetContentRegionAvail().x - pad, 0)) || ImGui::IsItemDeactivatedAfterEdit() )
                {
                    if (started && line_buf[0]) {
                        proc.send(line_buf);        // passthrough; must include \n if desired
                        line_buf[0]     = '\0';
                    }
                }
                ImGui::Dummy( ImVec2(pad, 0.0f) );
                //
                //
                ImGui::EndGroup();//  TOOL-TIP FOR "COMMUNICATION...
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_NoSharedDelay))
                    ImGui::SetTooltip( "Manual communication to the child process.  DO NOT TOUCH THIS UNLESS YOU KNOW WHAT YOU'RE DOING." );
                
            }// END.
        },
    //
        {"Integration Window",                      []
            {// BEGIN.
                ImGui::SetNextItemWidth( margin * ImGui::GetColumnWidth() );
                if (ImGui::SliderFloat("##IntegrationWindow",       &delay_s,   0.001f,    2.50f,   "%.3f sec") )   {
                    if (started)    {
                        char cmd[48];
                        std::snprintf(cmd, sizeof(cmd), "delay %.3f\n", delay_s);
                        proc.send(cmd);
                    }
                }
                ImGui::SameLine(0.0f, pad);
                if (ImGui::Button("Apply", ImVec2(ImGui::GetContentRegionAvail().x - pad, 0)) ) {
                    char cmd[48];
                    std::snprintf(cmd, sizeof(cmd), "delay %.3f\n", delay_s);
                    proc.send(cmd);
                }
                ImGui::Dummy( ImVec2(pad, 0.0f) );
            }// END.
        },
    //
        {"History Length",                          []
            {// BEGIN.
                ImGui::SetNextItemWidth( margin * ImGui::GetColumnWidth() );
                ImGui::SliderFloat("##HistoryLength",       &history_s,                 5.f,       120.0f,      "%.1f sec");
                ImGui::SameLine(0.0f, pad);
                if ( ImGui::Button("Clear Plot", ImVec2(ImGui::GetContentRegionAvail().x - pad, 0)) ) {
                    for (auto & b : buffers) b.Data.clear();
                    std::fill(std::begin(max_counts), std::end(max_counts), 0.f);
                }
                ImGui::Dummy( ImVec2(pad, 0.0f) );
            }// END.
        },
    //
    //
    //
    //  2.  MISC. / APPEARANCE CONTROLS...
        {"Appearance Settings",                     []
            {// BEGIN.
            }// END.
        },
    //
        {"Master Plot Height",                      []
            {
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::SliderFloat("##MasterPlotHeight",    &master_row_height_px,      180.f,       500.f,      "%.0f px");
            }
        },
    //
        {"Single Plot Height",                      []
            {
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::SliderFloat("##SinglePlotHeight",    &row_height_px,             30.f,       120.f,      "%.0f px");
            }
        },
    //
        {"Colormap",            []
            {
                float w = ImGui::GetColumnWidth();
                if (ImPlot::ColormapButton(ImPlot::GetColormapName(cc::cmap), ImVec2(w, 0), cc::cmap))
                {
                    cc::cmap = (cc::cmap + 1) % ImPlot::GetColormapCount();
                    ImPlot::BustColorCache(cc::heatmap_uuid);
                }
                ImPlot::PushColormap(cc::cmap);
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

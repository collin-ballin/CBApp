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



//  STATIC VARIABLES FOR COINCIDENCE COUNTER...
namespace cc {
    //  SKETCH STUFF...
    static ImPlotColormap               cmap                    = ImPlotColormap_Cool;


    //  COPIED...
    constexpr int                   NUM                         = 15;                 // skip index 0 (UNUSED)
    constexpr float                 CENTER                      = 0.75f;
    //  static constexpr ChannelSpec    channels[NUM]               = {
    //      { 8,  "A"   }, { 4,  "B"   }, { 2,  "C"   }, { 1,  "D"   },
    //      {12,  "AB"  }, {10,  "AC"  }, { 9,  "AD"  }, { 6,  "BC"  },
    //      { 5,  "BD"  }, { 3,  "CD"  }, {14,  "ABC" }, {13,  "ABD" },
    //      {11,  "ACD" }, { 7,  "BCD" }, {15,  "ABCD"}
    //  };
    


    static ImVec2                   HEADER_SEP_TOP              = ImVec2();
    static ImVec2                   HEADER_SEP_BOTTOM           = ImVec2();
}


//  Per‑channel ring buffers for ImPlot
//static std::array<utl::ScrollingBuffer, ms_NUM>   buffers;
//static float                    max_counts[ms_NUM]         = {};

//  Python process wrapper
static utl::PyStream            proc(app::PYTHON_DUMMY_FPGA_FILEPATH);
static constexpr size_t         BUFFER_SIZE                 = 256;
static ImGuiInputTextFlags      write_file_flags            = ImGuiInputTextFlags_None | ImGuiInputTextFlags_ElideLeft | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue;
static char                     filepath[BUFFER_SIZE];

static bool                     started                     = false;
static bool                     enter                       = false;
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
    COUNTER_COL_WIDTH          *= S.m_dpi_scale;
    CONTROL_WIDTH              *= ImGui::GetFontSize();
    
    std::snprintf(filepath, BUFFER_SIZE, "%s", app::PYTHON_DUMMY_FPGA_FILEPATH);
    
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
    //ImGui::SetNextWindowClass(&this->m_window_class);
    ImGui::Begin(m_win_uuids[0], p_open, m_docked_win_flags[0]);
        this->display_plots();
    ImGui::End();
    
    
    //  3.  CREATE BOTTOM WINDOW FOR CONTROLS...
    //ImGui::SetNextWindowClass(&this->m_window_class);
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
            
            for (int i = 0; i < ms_NUM; ++i)
            {
                int     channel_idx     = ms_channels[i].idx;
                float   v               = static_cast<float>(counts[channel_idx]);
                m_buffers[i].AddPoint(now, v);
                m_max_counts[i]           = std::max(m_max_counts[i], v);
            }
        }
    }



    //  5.      "MASTER" PLOT FOR MASTER PLOT...
    //
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::CollapsingHeader("Master Plot") )
    {
        //
        ImGui::PushID("master");
        if (ImPlot::BeginPlot("##master", ImVec2(-1, master_row_height_px), m_mst_plot_flags))
        {
            //  1.  SETUP THE PLOT...
            //          - Enable grid on both axes, keep no decorations.
            //
            ImPlot::SetupAxes(ms_mst_axis_labels[0],    ms_mst_axis_labels[1],  m_mst_xaxis_flags,  m_mst_yaxis_flags);
            ImPlot::SetupLegend(m_mst_legend_loc,   m_mst_legend_flags);
            
            //      1.1.    X-Limits.
            float   xmin    = now - cc::CENTER * history_s;
            float   xmax    = xmin + history_s;
            ImPlot::SetupAxisLimits(ImAxis_X1, xmin, xmax, ImGuiCond_Always);



            //  2.  ADD EACH COUNTER TO THE SAME PLOT...
            //
            for (int k = 0; k < ms_NUM; ++k)
            {
                const auto &    data        = m_buffers[k];
                const auto &    color       = ImPlot::GetColormapColor(k);
                
                
                //  3.  ADDING A PLOT...
                ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0,0));
                {
                    ImPlot::SetNextLineStyle(   color,      3.0);
                    ImPlot::SetNextFillStyle(   color,      0.0);
                    if (!data.Data.empty()) {
                        ImPlot::PlotLine( ms_channels[k].name,     &data.Data[0].x,     &data.Data[0].y,     data.Data.size(),        ImPlotLineFlags_Shaded, data.Offset, 2 * sizeof(float));
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
            for (int row = 0; row < ms_NUM; ++row)
            {
                const int           idx         = ms_channels[row].idx;
                auto &              buf         = m_buffers[row];
                const auto &        vec         = buf.Data;

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted( ms_channels[row].name );
                ImGui::TableSetColumnIndex(1); ImGui::Text("%.0f", m_max_counts[row]);
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

//  "display_controls"
//
void CCounterApp::display_controls(void)
{
    //  CONSTANTS...
    static constexpr float              LABEL_COLUMN_WIDTH      = 200.0f;
    static constexpr float              WIDGET_COLUMN_WIDTH     = 250.0f;

    //  INTERACTIVE VARIABLES...
    static ImGuiSliderFlags             SLIDER_FLAGS            = ImGuiSliderFlags_AlwaysClamp;

    //  TABLE GLOBAL FLAGS...
    static bool                         freeze_header           = false;
    static bool                         freeze_column           = false;
    static bool                         stretch_column_1        = true;

    //  COLUMN-SPECIFIC FLAGS...
    static ImGuiTableColumnFlags        col0_flags              = ImGuiTableColumnFlags_WidthFixed;
    static ImGuiTableColumnFlags        col1_flags              = stretch_column_1 ? ImGuiTableColumnFlags_WidthStretch : ImGuiTableColumnFlags_WidthFixed;
    static ImGuiTableFlags              flags                   = ImGuiTableFlags_None | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoKeepColumnsVisible; //| ImGuiTableFlags_ScrollX;
        
        

    //  DEFINE EACH WIDGET IN CONTROL PANEL...
    //
    constexpr float                     margin                  = 0.75f;
    constexpr float                     pad                     = 10.0f;
    static const utl::WidgetRow         rows[]                  = {
    //
    //  1.  CONTROL PARAMETERS...
        {"Record",                                  [this]
            {// BEGIN.
            //
            //  1.  PYTHON-SCRIPT FILEPATH FIELD...
                ImGui::SetNextItemWidth( margin * ImGui::GetColumnWidth() );
                enter   = ImGui::InputText("##PyFilepath", filepath, BUFFER_SIZE, write_file_flags);
                ImGui::SameLine(0.0f, pad);
                //
                if (enter)
                {
                    if ( utl::file_exists(filepath) ) {
                        std::snprintf(filepath, BUFFER_SIZE, "%s", "INVALID FILEPATH");
                        enter = false;
                    }
                    else {
                        proc.set_filepath(filepath);
                    }
                }
                //
                //
                //
                //  2.  START/STOP PYTHON SCRIPT BUTTON...
                //
                //      CASE 1 :    SCRIPT IS  **NOT**  RUNNING...
                    if (!started)
                    {
                        if (ImGui::Button("Start Process", ImVec2(ImGui::GetContentRegionAvail().x - pad, 0)) )
                        {
                            started         = proc.start();
                            if (!started) {
                                ImGui::OpenPopup("launch_error");
                            }
                            else {
                                m_max_counts[0] = 0.f; // reset stats
                            }
                        }
                    }
                    //
                    //  CASE 2 :    SCRIPT  **IS**  RUNNING...
                    else
                    {
                        ImGui::PushStyleColor(ImGuiCol_Button, app::DEF_APPLE_RED );
                        if (ImGui::Button("Stop Process", ImVec2(ImGui::GetContentRegionAvail().x - pad, 0)) ) {
                            proc.stop();
                            started = false;
                        }
                        ImGui::SameLine();
                        ImGui::TextDisabled("(running)");
                        ImGui::PopStyleColor();
                    }
                //
                ImGui::Dummy( ImVec2(pad, 0.0f) );
                //
                //
                //
                //  2B.     HANDLING ANY ERRORS WITH PROCESS...
                if (ImGui::BeginPopupModal("launch_error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                    ImGui::Text("Failed to launch Python process!");
                    if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
                }
                //
                if (started)    {
                    char cmd[48];
                    std::snprintf(cmd, sizeof(cmd), "delay %.3f\n", delay_s);
                    proc.send(cmd);
                }
            //
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
                if (ImGui::SliderFloat("##IntegrationWindow",       &delay_s,   0.001f,    2.50f,   "%.3f sec", SLIDER_FLAGS) )   {
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
        {"History Length",                          [this]
            {// BEGIN.
                ImGui::SetNextItemWidth( margin * ImGui::GetColumnWidth() );
                ImGui::SliderFloat("##HistoryLength",       &history_s,         5.f,       120.0f,      "%.1f sec", SLIDER_FLAGS);
                ImGui::SameLine(0.0f, pad);
                if ( ImGui::Button("Clear Plot", ImVec2(ImGui::GetContentRegionAvail().x - pad, 0)) ) {
                    for (auto & b : m_buffers) b.Data.clear();
                    std::fill(std::begin(m_max_counts), std::end(m_max_counts), 0.f);
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
                ImGui::SliderFloat("##MasterPlotHeight",    &master_row_height_px,  180.f,  500.f,  "%.0f px",  SLIDER_FLAGS);
            }
        },
    //
        {"Single Plot Height",                      []
            {
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::SliderFloat("##SinglePlotHeight",    &row_height_px,         30.f,   120.f,  "%.0f px",  SLIDER_FLAGS);
            }
        },
    //
        {"Colormap",            []
            {
                float w = ImGui::GetColumnWidth();
                if (ImPlot::ColormapButton(ImPlot::GetColormapName(cc::cmap), ImVec2(w, 0), cc::cmap))
                {
                    cc::cmap = (cc::cmap + 1) % ImPlot::GetColormapCount();
                    //ImPlot::BustColorCache(cc::heatmap_uuid);
                }
                ImPlot::PushColormap(cc::cmap);
            }}
    };



    //  1.  PRIMARY TABLE ENTRY...
    //
    if (ImGui::BeginTable("CBCCounterControls",     2,  flags))
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
    
    
    
    //  2.  LOGGER...
    /*
    ImGui::BeginChild("log", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
    
        for (auto const & s : log)
            ImGui::TextUnformatted(s.c_str());
            
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);       // auto‑scroll
            
            
    ImGui::EndChild();
    */
    
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

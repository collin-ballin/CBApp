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
    //  COPIED...
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

CCounterApp::CCounterApp(app::AppState & src)
    : S(src)                        { }

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
    ms_I_PLOT_COL_WIDTH       *= S.m_dpi_scale;
    std::snprintf(filepath, BUFFER_SIZE, "%s", app::PYTHON_DUMMY_FPGA_FILEPATH);
    
    
    //  Assign "Window Class" Properties...
    m_window_class.DockNodeFlagsOverrideSet     = 0; //ImGuiDockNodeFlags_HiddenTabBar; //ImGuiDockNodeFlags_NoTabBar;
    
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
                size_t      channel_idx     = ms_channels[i].idx;
                float       v               = static_cast<float>(counts[channel_idx]);
                m_buffers[i].AddPoint(now, v);
                m_max_counts[i]             = std::max(m_max_counts[i], v);
            }
        }
    }



    //  5.      "MASTER" PLOT FOR MASTER PLOT...
    //
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::CollapsingHeader("Master Plot") )
    {
        //
        ImGui::PushID(ms_PLOT_UUIDs[0]);
        if (ImPlot::BeginPlot(ms_PLOT_UUIDs[0], ImVec2(-1, master_row_height_px), m_mst_PLOT_flags))    //  m_mst_plot_flags
        {
            //  1.  SETUP THE PLOT...
            //          - Enable grid on both axes, keep no decorations.
            //
            ImPlot::SetupAxes(ms_mst_axis_labels[0],    ms_mst_axis_labels[1],  m_mst_xaxis_flags,  m_mst_yaxis_flags);
            ImPlot::SetupLegend(m_mst_legend_loc,   m_mst_legend_flags);
            
            //      1.1.    X-Limits.
            float   xmin    = now - ms_CENTER * history_s;
            float   xmax    = xmin + history_s;
            ImPlot::SetupAxisLimits(ImAxis_X1, xmin, xmax, ImGuiCond_Always);



            //  2.  ADD EACH COUNTER TO THE SAME PLOT...
            //
            for (int k = 0; k < ms_NUM; ++k)
            {
                const auto &    data        = m_buffers[k];
                static float    frequency   = 0;
                //const auto &    color       = ImPlot::GetColormapColor(k);
                
                //  3.  ADDING A PLOT...
                ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0,0));
                {
                    ImPlot::SetNextLineStyle(   m_plot_colors[k],      3.0);
                    ImPlot::SetNextFillStyle(   m_plot_colors[k],      0.0);
                    if (!data.Data.empty()) {
                        ImPlot::PlotLine( ms_channels[k].name,     &data.Data[0].x,     &data.Data[0].y,     data.Data.size(),        ImPlotLineFlags_Shaded, data.Offset, 2 * sizeof(float));
                    }
                }// END "Adding A Plot".
                ImPlot::PopStyleVar();
            
                
                //  CUSTOM LEGEND ENTRY...
                if (ImPlot::BeginLegendPopup(ms_channels[k].name))
                {
                    ImGui::SliderFloat("Frequency", &frequency, 0,  1,  "%0.2f");
                    // ImGui::SliderFloat("Amplitude", &amplitude, 0,  1,  "%0.2f");
                    // ImGui::Separator();
                    // ImGui::ColorEdit3("Color",&color.x);
                    // ImGui::SliderFloat("Transparency",&alpha,0,1,"%.2f");
                    // ImGui::Checkbox("Line Plot", &line);
                    //  if (line) {
                    //      ImGui::SliderFloat("Thickness", &thickness, 0, 5);
                    //      ImGui::Checkbox("Markers", &markers);
                    //      ImGui::Checkbox("Shaded",&shaded);
                    //  }
                    ImPlot::EndLegendPopup();
                }
                if (m_toggle_mst_plots)
                    this->ToggleAllPlots( ms_PLOT_UUIDs[0] );
                
                
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
        if (ImGui::BeginTable(ms_PLOT_UUIDs[1],    5,      ms_i_plot_table_flags)) //  ImGuiTableFlags_BordersOuter | ImGuiTableFlags_RowBg
        {
            ImGui::TableSetupColumn("Counter(s)",       ms_i_plot_column_flags,     ms_I_PLOT_COL_WIDTH);
            ImGui::TableSetupColumn("Max",              ms_i_plot_column_flags,     ms_I_PLOT_COL_WIDTH);
            ImGui::TableSetupColumn("Avg.",             ms_i_plot_column_flags,     ms_I_PLOT_COL_WIDTH);
            ImGui::TableSetupColumn("Current",          ms_i_plot_column_flags,     ms_I_PLOT_COL_WIDTH);
            ImGui::TableSetupColumn("Plot",             ms_i_plot_plot_flags,       ms_I_PLOT_PLOT_WIDTH);
            ImGui::TableHeadersRow();


            //  6.2     PLOT FOR EACH CHANNEL...
            for (size_t row = 0; row < ms_NUM; ++row)
            {
                const size_t        idx         = ms_channels[row].idx;
                auto &              buf         = m_buffers[row];
                const auto &        vec         = buf.Data;


                ImGui::TableNextRow();              //  ROW 0 :     COUNTER(S)...
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted( ms_channels[row].name );
                
                
                ImGui::TableSetColumnIndex(1);      //  ROW 1 :     MAX...
                //ImGui::BulletText("%.0f",           m_max_counts[row]);
                //
                ImGui::Text("%.0f", m_max_counts[row]);
                
                
                ImGui::TableSetColumnIndex(2);      //  ROW 2 :     AVERAGE...
                float avg = 0.f;
                if (!vec.empty()) {
                    for (const auto & pt : vec) avg += pt.y;
                        avg /= static_cast<float>(vec.size());
                }
                ImGui::Text("%.2f", avg);
                // ImGui::NewLine();
                // ImGui::Text("%.2f", avg);
                
                
                ImGui::TableSetColumnIndex(3);      //  ROW 3 :     CURRENT...
                float curr = vec.empty() ? 0.f : vec.back().y;
                ImGui::Text("%.0f", curr);
                
                
                
                ImGui::TableSetColumnIndex(4);      //  ROW 4 :     PLOT...
                ImGui::PushID(static_cast<int>(row));
                if (!vec.empty()) {
                    ScrollingSparkline(now, history_s, buf, m_plot_flags,
                                       m_plot_colors[row],
                                       ImVec2(-1, row_height_px), ms_CENTER);
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


//  "ToggleAllPlots"
//
void CCounterApp::ToggleAllPlots(const char * title) {
    ImPlotContext &     gp          = *GImPlot;
    ImPlotPlot *        plot        = nullptr;
    
    // First try current plot
    if (gp.CurrentPlot && strcmp(gp.CurrentPlot->GetTitle(), title) == 0) {
        plot = gp.CurrentPlot;
    }
    else {
        // Fallback: lookup by ID in the active window
        ImGuiWindow* Window = GImGui->CurrentWindow;
        ImGuiID id = Window->GetID(title);
        plot = gp.Plots.GetByKey(id);
    }
    if (!plot) return;
    ImPlotItemGroup& items = plot->Items;
    const int count = items.GetLegendCount();
    for (int i = 0; i < count; ++i) {
        ImPlotItem* item = items.GetLegendItem(i);
        item->Show = !item->Show;
    }
    return;
}



/*
ImPlotItem* ImPlot::GetItem(const char* plot_title, const char* item_label);
*/



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
    //static bool                         stretch_column_1        = true;

    //  COLUMN-SPECIFIC FLAGS...
    static ImGuiTableColumnFlags        col0_flags              = ImGuiTableColumnFlags_WidthFixed;
    static ImGuiTableColumnFlags        col1_flags              = ImGuiTableColumnFlags_WidthStretch; //stretch_column_1 ? ImGuiTableColumnFlags_WidthStretch : ImGuiTableColumnFlags_WidthFixed;
    static ImGuiTableFlags              flags                   = ImGuiTableFlags_None | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoKeepColumnsVisible; //| ImGuiTableFlags_ScrollX;
        
        
        

    //  1.  CONTROL PARAMETERS [FREE-STANDING / TEMPORARY]...
    //      if (ImGui::Button("Toggle All Plots")) {
    //          this->m_toggle_mst_plots = true; //this->ToggleAllPlots( this->ms_PLOT_UUIDs[0] );
    //      }
    //
    //
    //
    //  2.  DEFINE EACH WIDGET IN CONTROL PANEL...
    constexpr float                     margin                  = 0.75f;
    constexpr float                     pad                     = 10.0f;
    static const utl::WidgetRow         rows[]                  = {
    //
    //  1.  CONTROL PARAMETERS [TABLE]...
        {"Test",                                    []
            {// BEGIN.
                const char *    popup_id    = "Delete?";
                if (ImGui::Button("Delete"))
                    ImGui::OpenPopup(popup_id);
                    
                ImVec2 center = ImGui::GetMainViewport()->GetCenter();
                ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

                utl::Popup_AskOkCancel(popup_id);
            
            }// END.
        },
    //
    //
    //
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
                        ImGui::PushStyleColor(ImGuiCol_Button,          ImVec4(0.800f, 0.216f, 0.180f, 1.00f) );
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   app::DEF_APPLE_RED );
                        if (ImGui::Button("Stop Process", ImVec2(ImGui::GetContentRegionAvail().x - pad, 0)) ) {
                            proc.stop();
                            started = false;
                        }
                        ImGui::PopStyleColor(2);
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
        {"Coincidence Window",                      [this]
            {// BEGIN.
                ImGui::SetNextItemWidth( margin * ImGui::GetColumnWidth() );
                if (ImGui::SliderFloat("##CoincidenceWindow",       &m_coincidence_window,   8.0f,    22.0f,   "%.1f nano sec",    SLIDER_FLAGS) )   {
                    if (started)    {
                        //  char cmd[48];
                        //  std::snprintf(cmd, sizeof(cmd), "delay %.3f\n", delay_s);
                        //  proc.send(cmd);
                    }
                }
            }// END.
        },
    //
        {"Integration Window",                      []
            {// BEGIN.
                ImGui::SetNextItemWidth( margin * ImGui::GetColumnWidth() );
                if (ImGui::SliderFloat("##IntegrationWindow",       &delay_s,   0.01f,    2.50f,   "%.3f sec", SLIDER_FLAGS) )   {
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
        {"Colormap",                                [this]
            {
                float w = ImGui::GetColumnWidth();
                if (ImPlot::ColormapButton(ImPlot::GetColormapName(m_cmap), ImVec2(w, 0), m_cmap))
                {
                    m_colormap_cache_invalid    = true;
                    m_cmap                      = (m_cmap + 1) % ImPlot::GetColormapCount();
                    //ImPlot::BustColorCache(cc::heatmap_uuid);
                }
                //ImPlot::PushColormap(m_cmap);
            }}
    };
    
    //  RE-ASSIGN COLORMAP COLORS IF USER HAS CHANGED COLOR-MAP SELECTION...
    if (this->m_colormap_cache_invalid) {
        m_colormap_cache_invalid    = false;
        m_plot_colors               = cb::utl::GetColormapSamples( ms_NUM, m_cmap );
    }



    //  1.  PRIMARY TABLE ENTRY...
    //
    if (ImGui::BeginTable("CBCCounterControls",     2,  flags))
    {
        if (freeze_column || freeze_header)
            ImGui::TableSetupScrollFreeze(freeze_column ? 1 : 0, freeze_header ? 1 : 0);


        ImGui::TableSetupColumn("Label",    col0_flags,     LABEL_COLUMN_WIDTH);
        ImGui::TableSetupColumn("Widget",   col1_flags,     1.0f);//stretch_column_1 ? 1.0f : WIDGET_COLUMN_WIDTH);
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

/***********************************************************************************
*
*       ********************************************************************
*       ****                  T O O L S  ____  F I L E                  ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond
*               DATED:      May 24, 2025.
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
static ImGuiInputTextFlags      write_file_flags            = ImGuiInputTextFlags_None | ImGuiInputTextFlags_ElideLeft | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue;

static bool                     started                     = false;
static bool                     enter                       = false;
static float                    history_s                   = 30.0f;                // seconds visible in sparklines
static float                    row_height_px               = 60.0f;
static float                    master_row_height_px        = 400.f;                // independent of per‑row height
static float                    delay_s                     = 1.0f;                 // command slider
static char                     line_buf[512]{};                                    // manual send box



// *************************************************************************** //
//
//
//  2.  COINCIDENCE COUNTER TESTING FUNC...
// *************************************************************************** //
// *************************************************************************** //

//  "ShowCCPlots"
//
void CCounterApp::ShowCCPlots(void)
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
            float   xmin    = now - ms_CENTER * m_history_length.value;
            float   xmax    = xmin + m_history_length.value;
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
            for (size_t row = 0; row < static_cast<size_t>( ms_NUM - m_PLOT_LIMIT.value); ++row)
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
                    ScrollingSparkline(now, m_history_length.value, buf, m_plot_flags,
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


// *************************************************************************** //
//
//
//  2.  PRIMARY "PLOT" AND "CONTROL" FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "ShowCCControls"
//
void CCounterApp::ShowCCControls(void)
{
    //  CONSTANTS...
    static constexpr float              LABEL_COLUMN_WIDTH      = 200.0f;
    static constexpr float              WIDGET_COLUMN_WIDTH     = 250.0f;

    //  INTERACTIVE VARIABLES...
    static ImGuiSliderFlags             SLIDER_FLAGS            = ImGuiSliderFlags_AlwaysClamp;



    //  1.  CONTROL PARAMETERS [FREE-STANDING / TEMPORARY]...
    //      if (ImGui::Button("Toggle All Plots")) {
    //          this->m_toggle_mst_plots = true; //this->ToggleAllPlots( this->ms_PLOT_UUIDs[0] );
    //      }
    //
    //
    //
    //  2.  DEFINE EACH WIDGET IN CONTROL PANEL...


    
    //  RE-ASSIGN COLORMAP COLORS IF USER HAS CHANGED COLOR-MAP SELECTION...
    if (this->m_colormap_cache_invalid) {
        m_colormap_cache_invalid    = false;
        m_plot_colors               = cb::utl::GetColormapSamples( ms_NUM, m_cmap );
    }


    
    
    
    //  1.  PRIMARY TABLE ENTRY...
    static utl::TableCFG<2>    ctrl_table_CFG    = {
        "CCCounterControlsTable"
    };
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::CollapsingHeader("Controls") ) {
        utl::MakeCtrlTable(this->ms_CTRL_ROWS, ctrl_table_CFG);
    }
    

    
    //  2.  APPEARANCE TABLE ENTRY...
    static utl::TableCFG<2>    appearance_table_CFG    = {
        "CCounterAppearanceTable"
    };
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::CollapsingHeader("Appearance Settings") ) {
        utl::MakeCtrlTable(this->ms_APPEARANCE_ROWS, appearance_table_CFG);
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
//  ?       UTILITY PLOTS...
// *************************************************************************** //
// *************************************************************************** //

//  "ToggleAllPlots"
//
void CCounterApp::ToggleAllPlots(const char * title) {
    ImPlotContext &         gp          = *GImPlot;
    ImPlotPlot *            plot        = nullptr;
    
    // First try current plot
    if (gp.CurrentPlot && strcmp(gp.CurrentPlot->GetTitle(), title) == 0) {
        plot = gp.CurrentPlot;
    }
    else {
        // Fallback: lookup by ID in the active window
        ImGuiWindow *       win         = GImGui->CurrentWindow;
        ImGuiID             id          = win->GetID(title);
        plot                            = gp.Plots.GetByKey(id);
    }
    if (!plot) return;
    ImPlotItemGroup &       items       = plot->Items;
    const int               count       = items.GetLegendCount();
    for (int i = 0; i < count; ++i) {
        ImPlotItem* item = items.GetLegendItem(i);
        item->Show = !item->Show;
    }
    
    
    
    return;
}







// *************************************************************************** //
//
//
//  ?       UTILITY PLOTS...
// *************************************************************************** //
// *************************************************************************** //


//  "init_ctrl_rows"
//
void CCounterApp::init_ctrl_rows(void)
{
    static constexpr float              LABEL_COLUMN_WIDTH      = 200.0f;
    static constexpr float              WIDGET_COLUMN_WIDTH     = 250.0f;

    //  INTERACTIVE VARIABLES...
    static ImGuiSliderFlags             SLIDER_FLAGS            = ImGuiSliderFlags_AlwaysClamp;

    constexpr float                     margin                  = 0.75f;
    constexpr float                     pad                     = 10.0f;



    ms_CTRL_ROWS            = {
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
                enter   = ImGui::InputText("##PyFilepath", m_filepath, ms_BUFFER_SIZE, write_file_flags);
                ImGui::SameLine(0.0f, pad);
                //
                if (enter)
                {
                    if ( utl::file_exists(m_filepath) ) {
                        std::snprintf(m_filepath, ms_BUFFER_SIZE, "%s", "INVALID FILEPATH");
                        enter = false;
                    }
                    else {
                        proc.set_filepath(m_filepath);
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
                    char cmd[ms_BUFFER_SIZE];
                    std::snprintf(cmd, ms_BUFFER_SIZE, "integration_window %.3f\n", m_integration_window.value);
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
                    char cmd[ms_BUFFER_SIZE];
                        std::snprintf(cmd, ms_BUFFER_SIZE, "integration_window %.3f\n", delay_s);
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
                //if (ImGui::SliderInt("##CoincidenceWindow",       &m_coincidence_window,   8,    100,   "%03d clicks",    SLIDER_FLAGS) )
                
                if ( ImGui::SliderScalar("##CoincidenceWindow",    ImGuiDataType_U64,      &m_coincidence_window.value,
                                        &m_coincidence_window.limits.min, &m_coincidence_window.limits.max,  "%llu ticks", SLIDER_FLAGS) )
                {
                    if (started)    {
                        char cmd[ms_BUFFER_SIZE];
                        std::snprintf(cmd, ms_BUFFER_SIZE, "coincidence_window %llu\n", m_coincidence_window.value);
                        proc.send(cmd);
                    }
                }
            }// END.
        },
    //
        {"Integration Window",                      [this]
            {// BEGIN.
                ImGui::SetNextItemWidth( margin * ImGui::GetColumnWidth() );
                if ( ImGui::SliderScalar("##IntegrationWindow",          ImGuiDataType_Double,       &m_integration_window.value,
                                    &m_integration_window.limits.min,     &m_integration_window.limits.max,     "%.3f seconds", SLIDER_FLAGS) ) {
                    if (started) {
                        char cmd[ms_BUFFER_SIZE];
                        std::snprintf(cmd, ms_BUFFER_SIZE, "integration_window %.3f\n", m_integration_window.value);
                        proc.send(cmd);
                    }
                }
                ImGui::SameLine(0.0f, pad);
                if (ImGui::Button("Apply", ImVec2(ImGui::GetContentRegionAvail().x - pad, 0)) ) {
                    char cmd[ms_BUFFER_SIZE];
                    std::snprintf(cmd, ms_BUFFER_SIZE, "integration_window %.3f\n", delay_s);
                    proc.send(cmd);
                }
                ImGui::Dummy( ImVec2(pad, 0.0f) );
            }// END.
        },
    //
        {"History Length",                          [this]
            {// BEGIN.
                ImGui::SetNextItemWidth( margin * ImGui::GetColumnWidth() );
                ImGui::SliderScalar("##HistoryLength",                  ImGuiDataType_Double,           &m_history_length.value,
                                    &m_history_length.limits.min,       &m_history_length.limits.max,   "%.1f seconds", SLIDER_FLAGS);
                ImGui::SameLine(0.0f, pad);
                
                if ( ImGui::Button("Clear Plot", ImVec2(ImGui::GetContentRegionAvail().x - pad, 0)) ) {
                    for (auto & b : m_buffers) {
                        b.Erase();
                    }
                    std::fill(std::begin(m_max_counts), std::end(m_max_counts), 0.f);
                }
                ImGui::Dummy( ImVec2(pad, 0.0f) );
            }// END.
        }
    };

    
    
    

    ms_APPEARANCE_ROWS  = {
        {"",                     []
            {// BEGIN.
                ImGui::TextUnformatted("Appearance Settings");
            }// END.
        },
    //
        {"Master Plot Height",                      []
            {
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::SliderFloat("##MasterPlotHeight",    &master_row_height_px,  120,  750.f,  "%.0f px",  SLIDER_FLAGS);
            }
        },
    //
        {"Individual Plot Height",                      []
            {
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::SliderFloat("##SinglePlotHeight",    &row_height_px,         30.f,   240,  "%.0f px",  SLIDER_FLAGS);
            }
        },
    //
        {"Plot Limiter",                      [this]
            {
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::SliderScalar("##PlotLimiter",    ImGuiDataType_U64,      &m_PLOT_LIMIT.value,
                                        &m_PLOT_LIMIT.limits.min, &m_PLOT_LIMIT.limits.max,  "%llu", SLIDER_FLAGS);
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

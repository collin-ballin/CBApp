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

//  Per‑channel ring buffers for ImPlot
//static std::array<utl::ScrollingBuffer, ms_NUM>   buffers;
//static float                    max_counts[ms_NUM]         = {};

//  Python process wrapper

static utl::PyStream            proc(app::PYTHON_DUMMY_FPGA_FILEPATH);





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
    using                           namespace               ccounter;
    std::string                     raw;
    bool                            got_packet              = false;
    //
    float                           xmin                    = 0.0f,
                                    xmax                    = m_history_length.value;
    //
    //const float                     now                     = static_cast<float>(ImGui::GetTime());
    static float                    now                     = static_cast<float>(ImGui::GetTime());
                                    
                                    
                                    
    if ( m_process_running )
        now += ImGui::GetIO().DeltaTime;
    
    
    //TIME += ImGui::GetIO().DeltaTime;
    const float                     spark_now               = (!m_counter_running ? m_freeze_now
                                                                : (m_smooth_scroll ? now : m_last_packet_time));
    
    
    // ------------------------------------------------------------
    // 1.  Poll child process and push new points
    // ------------------------------------------------------------
    while (m_python.try_receive(raw))
    {
        got_packet = true;
        if (auto pkt = utl::parse_packet(raw, m_use_mutex_count))
        {
            const auto &counts = pkt->counts;
            for (int i = 0; i < ms_NUM; ++i)
            {
                const size_t ch         = ms_channels[i].idx;
                const float   v         = static_cast<float>(counts[ch]);
                m_buffers[i].push_back({ now, v });
                m_max_counts[i]         = std::max(m_max_counts[i], v);
                m_avg_counts[i].push_back({ now, ComputeAverage( m_buffers[i], m_avg_mode, m_avg_window_samp.value,
                                                                 m_avg_window_sec.value, spark_now ) });
            }
        }
    }

    if (got_packet) {
        m_last_packet_time = now;
    }

    // streaming considered active if we’ve received data within timeout
    m_streaming_active = (now - m_last_packet_time) < m_stream_timeout;


    //------------------------------------------------------------------
    // 2.  Update cached freeze range when streaming or on first frame
    //------------------------------------------------------------------
    if (!m_counter_running) {               //  Recording stopped – freeze completely.
        xmin = m_freeze_xmin;
        xmax = m_freeze_xmax;
    }
    else if (m_smooth_scroll) {             //  Continuous crawl following real time.
        xmin = now - ms_CENTER * m_history_length.value;
        xmax = xmin + m_history_length.value;
        m_freeze_xmin = xmin;               //  Keep cache fresh in case we pause later.
        m_freeze_xmax = xmax;
    }
    else { // Stepped mode
        if (got_packet) {                   //  Jump so the newest packet sits at right edge of window.
            xmin            = m_last_packet_time - ms_CENTER * m_history_length.value;
            xmax            = xmin + m_history_length.value;
            m_freeze_xmin   = xmin;
            m_freeze_xmax   = xmax;
        }
        else {                              //  Hold still between packets.
            xmin            = m_freeze_xmin;
            xmax            = m_freeze_xmax;
        }
    }

    //  Ensure an initial range on app startup
    if (xmax <= xmin) {
        xmin            = 0.f;
        xmax            = m_history_length.value;
        m_freeze_xmin   = xmin;
        m_freeze_xmax   = xmax;
    }


    //  5.      "MASTER" PLOT FOR MASTER PLOT...
    //
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::CollapsingHeader("Master Plot") )
    {
        //  5.1.    DISPLAY VERTICAL SLIDER BESIDE PLOT...
        ImGui::VSliderFloat("##MasterPlotHeight", ImVec2(m_mst_plot_slider_height, m_mst_plot_height), &m_mst_plot_height, 750.0f, 150.0f, "");
        //
        //
        //  5.2.    DISPLAY MASTER PLOT...
        ImGui::SameLine();
        ImGui::PushID(ms_PLOT_UUIDs[0]);    //  Adjust x‑axis flags: disable AutoFit when paused so ImPlot won't override our limits
        ImPlotAxisFlags xflags = m_mst_xaxis_flags;
        if (!m_counter_running) xflags &= ~ImPlotAxisFlags_AutoFit;
        
        
        
        if (ImPlot::BeginPlot(ms_PLOT_UUIDs[0], ImVec2(-1, m_mst_plot_height), m_mst_PLOT_flags))    //  m_mst_plot_flags
        {
            ImPlot::SetupAxes(ms_mst_axis_labels[0], ms_mst_axis_labels[1], xflags, m_mst_yaxis_flags);
            ImPlot::SetupLegend(m_mst_legend_loc, m_mst_legend_flags);
            ImPlot::SetupAxisLimits(ImAxis_X1, xmin, xmax, ImGuiCond_Always);


            for (int k = 0; k < ms_NUM; ++k)
            {
                const auto &        buf         = m_buffers[k];
                const auto &        avg         = m_avg_counts[k];
                auto &              channel     = ms_channels[k];
                
                if ( buf.empty() )
                    continue;
                
    
                //  2.  PLOT AVERAGE COUNTER VALUES...
                ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0,0));
                ImPlot::SetNextLineStyle( ImVec4( m_plot_colors[k].x, m_plot_colors[k].y, m_plot_colors[k].z, m_AVG_OPACITY.value ),   m_AVG_LINEWIDTH.value);
                ImPlot::SetNextFillStyle(m_plot_colors[k], 0.0f);
                if (!channel.vis.average)   ImPlot::HideNextItem( true, ImGuiCond_Always );
                else                        ImPlot::HideNextItem( false, ImGuiCond_Always );
                ImPlot::PlotLine("",
                                 &avg.front().x, &avg.front().y,
                                 static_cast<int>( avg.size() ), ImPlotLineFlags_Shaded,
                                 static_cast<int>( avg.offset() ), sizeof(ImVec2));
                ImPlot::PopStyleVar();
                
                
                //  1.  PLOT MAIN COUNTER VALUES...
                ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0,0));
                ImPlot::SetNextLineStyle(m_plot_colors[k], 1.5f);
                ImPlot::SetNextFillStyle(m_plot_colors[k], 0.0f);
                if (!channel.vis.master)    ImPlot::HideNextItem( true, ImGuiCond_Always );
                else                        ImPlot::HideNextItem( false, ImGuiCond_Always );
                ImPlot::PlotLine(ms_channels[k].name,
                                 &buf.front().x, &buf.front().y,
                                 static_cast<int>( buf.size() ), ImPlotLineFlags_Shaded,
                                 static_cast<int>( buf.offset() ), sizeof(ImVec2));
                ImPlot::PopStyleVar();
                                 
            
            
            } // END FOR-LOOP.
            
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
    //
    //  const float spark_now = (!m_counter_running ? m_freeze_now
    //                              : (m_smooth_scroll ? now : m_last_packet_time));

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::CollapsingHeader("Individual Counters") )
    {
        if (ImGui::BeginTable(ms_PLOT_UUIDs[1],    6,      ms_i_plot_table_flags)) //  ImGuiTableFlags_BordersOuter | ImGuiTableFlags_RowBg
        {
            ImGui::TableSetupColumn("Visibility",   ms_i_plot_column_flags, 2 * ms_I_PLOT_COL_WIDTH);
            ImGui::TableSetupColumn("Counter(s)",   ms_i_plot_column_flags, ms_I_PLOT_COL_WIDTH);
            ImGui::TableSetupColumn("Max",          ms_i_plot_column_flags, ms_I_PLOT_COL_WIDTH);
            ImGui::TableSetupColumn("Avg.",         ms_i_plot_column_flags, ms_I_PLOT_COL_WIDTH);
            ImGui::TableSetupColumn("Current",      ms_i_plot_column_flags, ms_I_PLOT_COL_WIDTH);
            ImGui::TableSetupColumn("Plot",         ms_i_plot_plot_flags,   ms_I_PLOT_PLOT_WIDTH);
            ImGui::TableHeadersRow();

            for (size_t row = 0; row < static_cast<size_t>(ms_NUM - m_PLOT_LIMIT.value); ++row)
            {
                auto &          buf         = m_buffers[row];
                auto &          channel     = ms_channels[row];
                ImGui::TableNextRow();


                //  1.  COLUMN 1.       VISIBILITY SWITCHES...
                ImGui::TableSetColumnIndex(0);
                ImGui::Checkbox( channel.vis.master_ID,     &channel.vis.master );
                ImGui::SameLine();
                ImGui::Checkbox( channel.vis.average_ID,    &channel.vis.average );
                ImGui::SameLine();
                ImGui::Checkbox( channel.vis.single_ID,    &channel.vis.single );
                
                
                
                //  2.  COLUMN 2.       CHANNEL ID...
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(ms_channels[row].name);
    
    
    
                //  3.  COLUMN 3.       MAXIMUM VALUE...
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%.0f", m_max_counts[row]);



                //  4.  COLUMN 4.       AVERAGE VALUE...
                ImGui::TableSetColumnIndex(3);
                //  loc_avg = ComputeAverage(buf, m_avg_mode,
                //                   m_avg_window_samp.value,
                //                   m_avg_window_sec.value,
                //                   spark_now);
                ImGui::Text("%.2f", (buf.empty()) ? 0.0f : m_avg_counts[row].top().y );



                //  5.  COLUMN 5.       CURRENT VALUE...
                ImGui::TableSetColumnIndex(4);
                const float curr = buf.empty() ? 0.f : buf.back().y;
                ImGui::Text("%.0f", curr);



                //  6.  COLUMN 6.       ANIMATED PLOT...
                ImGui::TableSetColumnIndex(5);
                if ( channel.vis.single )
                {
                    ImGui::PushID(static_cast<int>(row));
                    if (!buf.empty())
                        utl::ScrollingSparkline(spark_now, m_history_length.value, buf, m_plot_flags,
                                                m_plot_colors[row], ImVec2(-1, row_height_px), ms_CENTER);
                    ImGui::PopID();
                }
                
                
            }// END "FOR-LOOP".
        
        
        ImGui::EndTable();
        }// END "table".
    //
    //
    //
    }// END TABLE...



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
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.















// *************************************************************************** //
// *************************************************************************** //
//
//  END.

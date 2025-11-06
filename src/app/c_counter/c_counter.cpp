/***********************************************************************************
*
*       ********************************************************************
*       ****       _ C O I N C I D E N C E . C P P  ____  F I L E       ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond.
*               DATED:      May 10, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/c_counter/c_counter.h"




namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//      1A.     PUBLIC API...
// *************************************************************************** //
// *************************************************************************** //

//  "save"
//
void CCounterApp::save(void) {
    CB_LOG( LogLevel::Info, "CCounter--save" );
    return;
}


//  "undo"
//
void CCounterApp::undo(void) {
    S.m_logger.info("CCounter--undo");
    return;
}


//  "redo"
//
void CCounterApp::redo(void) {
    S.m_logger.info("CCounter--redo");
    return;
}




    

// *************************************************************************** //
//
//
//
//      1B.     PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "Begin"
//
void CCounterApp::Begin([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
{
    //      1.      CREATING THE HOST WINDOW...
    ImGui::Begin(uuid, p_open, flags);
        this->_Begin_Plots_IMPL();
    ImGui::End();
    
    
    
    //      2.      CREATE TOP WINDOW FOR PLOTS...
    if (m_detview_window.open)
    {
        //ImGui::SetNextWindowClass(&this->m_window_class[1]);
        ImGui::Begin( m_detview_window.uuid.c_str(), nullptr, m_detview_window.flags );
            this->_Begin_Controls_IMPL();
        ImGui::End();
    }

    
    return;
}






// *************************************************************************** //
//
//
//
//      2.      PROTECTED MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_Begin_Controls_IMPL"              //  formerly:   "display_controls"
//
void CCounterApp::_Begin_Controls_IMPL(void) noexcept
{
    //      DEFINE MISC. VARIABLES...
    const ImGuiWindowFlags          CHILD_FLAGS     = ImGuiWindowFlags_AlwaysVerticalScrollbar;
    
    
    
    //      BEGIN THE TAB BAR...
    if ( ImGui::BeginTabBar(m_tabbar_uuids[1], m_tabbar_flags[1]) )
    {
        //      2.3     DRAW EACH OF THE TAB ITEMS...
        for (auto & tab : this->ms_CTRL_TABS)
        {
        
            //          PLACING EACH TAB...
            if ( ImGui::BeginTabItem( tab.get_uuid(), (tab.no_close) ? nullptr : &tab.open, tab.flags ) )
            {
                ImGui::BeginChild("##Control_TabBar_ChildWindow", ImVec2(0,0), /*border=*/false, CHILD_FLAGS);
            
                    if (tab.render_fn) {
                        tab.render_fn( tab.get_uuid(), &tab.open, tab.flags );
                    }
                    else {
                        this->DefaultCtrlTabRenderFunc(tab.get_uuid(), &tab.open, tab.flags);
                    }
                
            ImGui::EndChild();          //  <- scrollable
            ImGui::EndTabItem();
            //
            }// END "BeginTabItem".
        //
        //
        } // END "for auto & tab".


    ImGui::EndTabBar();
    //
    //
    }// END "BeginTabBar".

    return;
}












// *************************************************************************** //
//
//
//
//      ?.      OLD "ShowCCPlots"...
// *************************************************************************** //
// *************************************************************************** //

//  "ShowCCPlots"       //  formerly: "ShowCCPlots"
//
void CCounterApp::ShowCCPlots(void)
{
    namespace               cc                  = ccounter;
    std::string             raw;
    bool                    got_packet          = false;
    //
    float                   xmin                = 0.0f,
                            xmax                = m_history_length.value;
    //
    static float            now                 = static_cast<float>(ImGui::GetTime());
                                    
                                    
                                    
    if ( m_process_running )
        now += ImGui::GetIO().DeltaTime;
    
    
    const float             spark_now           = (!m_counter_running   ? m_freeze_now    : ( (m_smooth_scroll)     ? now   : m_last_packet_time) );
    
    
    // ------------------------------------------------------------
    // 1.  Poll child process and push new points
    // ------------------------------------------------------------
    while ( m_python.try_receive(raw) )
    {
        got_packet = true;
        if (auto pkt = cc::parse_packet(raw, m_use_mutex_count))
        {
            const auto &counts = pkt->counts;
            for (int i = 0; i < static_cast<int>(ms_NUM); ++i)
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
        xmin            = 0.0f;
        xmax            = m_history_length.value;
        m_freeze_xmin   = xmin;
        m_freeze_xmax   = xmax;
    }


    //      5.      "MASTER" PLOT FOR MASTER PLOT...
    //
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::CollapsingHeader("Master Plot") )
    {
        //          5.1.    DISPLAY VERTICAL SLIDER BESIDE PLOT...
        ImGui::VSliderFloat("##MasterPlotHeight", ImVec2(m_mst_plot_slider_height, m_mst_plot_height), &m_mst_plot_height, 750.0f, 150.0f, "");
        //
        //
        //          5.2.    DISPLAY MASTER PLOT...
        ImGui::SameLine();
        ImGui::PushID(ms_PLOT_UUIDs[0]);    //  Adjust x‑axis flags: disable AutoFit when paused so ImPlot won't override our limits
        ImPlotAxisFlags xflags = m_mst_xaxis_flags;
        if (!m_counter_running) xflags &= ~ImPlotAxisFlags_AutoFit;
        
        
        
        if (ImPlot::BeginPlot(ms_PLOT_UUIDs[0], ImVec2(-1, m_mst_plot_height), m_mst_PLOT_flags))    //  m_mst_plot_flags
        {
            ImPlot::SetupAxes(ms_mst_axis_labels[0], ms_mst_axis_labels[1], xflags, m_mst_yaxis_flags);
            ImPlot::SetupLegend(m_mst_legend_loc, m_mst_legend_flags);
            ImPlot::SetupAxisLimits(ImAxis_X1, xmin, xmax, ImGuiCond_Always);


            for (int k = 0; k < static_cast<int>(ms_NUM); ++k)
            {
                const auto &        buf         = m_buffers[k];
                const auto &        avg         = m_avg_counts[k];
                auto &              channel     = ms_channels[k];
                
                if ( buf.empty() )      { continue; }
                
    
    
                //      2.      PLOT AVERAGE COUNTER VALUES...
                ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0,0));
                ImPlot::SetNextLineStyle( ImVec4( m_plot_colors[k].x, m_plot_colors[k].y, m_plot_colors[k].z, m_AVG_OPACITY.value ),   m_AVG_LINEWIDTH.value);
                ImPlot::SetNextFillStyle(m_plot_colors[k], 0.0f);
                //
                    if ( !channel.vis.average )     { ImPlot::HideNextItem( true    , ImGuiCond_Always );   }
                    else                            { ImPlot::HideNextItem( false   , ImGuiCond_Always );   }
                    ImPlot::PlotLine(
                          ""
                        , &avg.raw()[0].x
                        , &avg.raw()[0].y
                        , static_cast<int>( avg.size() )
                        , ImPlotLineFlags_Shaded
                        , static_cast<int>( avg.offset() )
                        , sizeof(ImVec2)
                    );
                //
                ImPlot::PopStyleVar();
                
                
                
                //      1.      PLOT MAIN COUNTER VALUES...
                ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0,0));
                ImPlot::SetNextLineStyle(m_plot_colors[k], 1.5f);
                ImPlot::SetNextFillStyle(m_plot_colors[k], 0.0f);
                //
                    if (!channel.vis.master)        { ImPlot::HideNextItem( true    , ImGuiCond_Always ); }
                    else                            { ImPlot::HideNextItem( false   , ImGuiCond_Always ); }
                    ImPlot::PlotLine(
                          ms_channels[k].name
                        , &buf.raw()[0].x
                        , &buf.raw()[0].y
                        , static_cast<int>( buf.size() )
                        , ImPlotLineFlags_Shaded
                        , static_cast<int>( buf.offset() )
                        , sizeof(ImVec2)
                    );
                //
                ImPlot::PopStyleVar();
            //
            //
            //
            } // END FOR-LOOP.
            
            ImPlot::EndPlot();
        }
        ImGui::PopID();
    //
    //
    //
    //ImGui::TreePop();
    }// END TREE NODE.
        


    //      6.      DRAW THE TABLE OF EACH INDIVIDUAL COUNTER...
    //
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::CollapsingHeader("Individual Counters") )
    {
        if ( ImGui::BeginTable(ms_PLOT_UUIDs[1],  6,  ms_i_plot_table_flags) ) //  ImGuiTableFlags_BordersOuter | ImGuiTableFlags_RowBg
        {
            ImGui::TableSetupColumn     ("Visibility     "   ,   ms_i_plot_column_flags  , 2 * ms_I_PLOT_COL_WIDTH);
            ImGui::TableSetupColumn     ("Counter(s)"        ,   ms_i_plot_column_flags  , ms_I_PLOT_COL_WIDTH   );
            ImGui::TableSetupColumn     ("Max"               ,   ms_i_plot_column_flags  , ms_I_PLOT_COL_WIDTH   );
            ImGui::TableSetupColumn     ("Avg."              ,   ms_i_plot_column_flags  , ms_I_PLOT_COL_WIDTH   );
            ImGui::TableSetupColumn     ("Current"           ,      ms_i_plot_column_flags  , ms_I_PLOT_COL_WIDTH   );
            ImGui::TableSetupColumn     ("Plot"              ,         ms_i_plot_plot_flags    , ms_I_PLOT_PLOT_WIDTH  );
            ImGui::TableHeadersRow();


            for (size_t row = 0; row < static_cast<size_t>(ms_NUM - m_PLOT_LIMIT.value); ++row)
            {
                auto &          buf         = m_buffers[row];
                auto &          channel     = ms_channels[row];
                const bool      is_empty    = buf.empty();
                ImGui::TableNextRow();


                //      1.      COLUMN 1.       VISIBILITY SWITCHES...
                ImGui::TableSetColumnIndex(0);
                ImGui::Checkbox( channel.vis.master_ID,     &channel.vis.master );
                ImGui::SameLine();
                ImGui::Checkbox( channel.vis.average_ID,    &channel.vis.average );
                ImGui::SameLine();
                ImGui::Checkbox( channel.vis.single_ID,    &channel.vis.single );
                
                
                //      2.      COLUMN 2.       CHANNEL ID...
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(ms_channels[row].name);
    
    
                //      3.      COLUMN 3.       MAXIMUM VALUE...
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%.0f", m_max_counts[row]);


                //      4.      COLUMN 4.       AVERAGE VALUE...
                ImGui::TableSetColumnIndex(3);
                //  loc_avg = ComputeAverage(buf, m_avg_mode,
                //                   m_avg_window_samp.value,
                //                   m_avg_window_sec.value,
                //                   spark_now);
                ImGui::Text("%.2f", (is_empty)  ? 0.0f  : m_avg_counts[row].top().y );


                //      5.      COLUMN 5.       CURRENT VALUE...
                ImGui::TableSetColumnIndex(4);
                const float     curr    = (is_empty)    ? 0.0f   : buf.back().y;
                ImGui::Text("%.0f", curr);


                //      6.      COLUMN 6.       ANIMATED PLOT...
                ImGui::TableSetColumnIndex(5);
                if ( channel.vis.single )
                {
                    ImGui::PushID( static_cast<int>(row) );
                    if ( !is_empty  &&  channel.vis.single )
                    {
                        //  This is bitching at me b.c. I made the func. "inline" but it is in a diff. .cpp file...
                        //
                        //      this->plot_sparkline(
                        //            buf
                        //          , this->m_plot_colors[row]
                        //          , ImVec2(-1, cc::row_height_px)
                        //          , spark_now
                        //          , this->m_history_length.Value()
                        //          , this->ms_CENTER
                        //          , this->m_plot_flags
                        //      );
                    }
                    ImGui::PopID();
                }
            //
            //
            }// END "FOR-LOOP".
        
        
        ImGui::EndTable();
        }// END "table".
    //
    //
    //
    }// END TABLE...



    //ImPlot::PopColormap();
    //ImGui::End();
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

/***********************************************************************************
*
*       *********************************************************************
*       ****              P L O T S . C P P  ____  F I L E               ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      November 9, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/c_counter/c_counter.h"




namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //












// *************************************************************************** //
//
//
//
//      2.      PLOTTING FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_PlotMaster"
//
void CCounterApp::_PlotMaster(void) const noexcept
{
    namespace                   cc          = ccounter;
    const Style &               CS          = this->m_style;
    const cc::PerFrame_t &      PF          = this->m_perframe;
    ImPlotAxisFlags             xflags      = CS.mst_axes[0].flags;
    
    CS.m_mst_avail                          = ImGui::GetContentRegionAvail();
    
    
    
    if ( !m_counter_running )       { xflags &= ~ImPlotAxisFlags_AutoFit; }
    
    
    
    ImGui::PushID( ms_PLOT_UUIDs[0] );    //  Adjust x‑axis flags: disable AutoFit when paused so ImPlot won't override our limits
    
    //      BEGIN MASTER PLOT...
    //
    if ( !ImPlot::BeginPlot( ms_PLOT_UUIDs[0], ImVec2(-1, CS.m_mst_avail.y), CS.mst_plot_flags ) )        { ImGui::PopID();  return; }
    
    
    
    {
        //      1.      CONFIGURE THE "IMPLOT" APPEARANCE...
        ImPlot::SetupAxes(  CS.mst_axes[0].uuid     , CS.mst_axes[1].uuid               //  5A.     Axis Names & Flags.
                          , xflags                  , CS.mst_axes[1].flags );
                          
        //      2.      SET-UP THE MASTER-PLOT LEGEND...
        ImPlot::SetupLegend         ( CS.legend.location    , CS.legend.flags );
                          
        //      3.      SET THE INITIAL SIZE OF THE CANVAS...
        ImPlot::SetupAxisLimits     (ImAxis_X1              , PF.xmin               , PF.xmax       , ImGuiCond_Always          );
        

    
        for (int k = 0; k < static_cast<int>(ms_NUM); ++k)
        {
            const auto &        buf         = m_buffers[k];
            const auto &        avg         = m_avg_counts[k];
            auto &              channel     = ms_channels[k];
            //
            const float         avg_lw      = this->m_avg_linewidth.Value();
            const float         plot_lw     = this->m_plot_linewidth.Value();
            
            if ( buf.empty() )              { continue; }
            


            //      2.      PLOT AVERAGE COUNTER VALUES...
            ImPlot::PushStyleVar        (ImPlotStyleVar_PlotPadding, ImVec2(0,0));
            ImPlot::SetNextLineStyle    ( m_avg_colors[k], avg_lw);
            ImPlot::SetNextFillStyle    ( m_avg_colors[k], 0.0f );
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
            ImPlot::PushStyleVar        (ImPlotStyleVar_PlotPadding     , ImVec2(0,0));
            ImPlot::SetNextLineStyle    (m_plot_colors[k]               , plot_lw);
            ImPlot::SetNextFillStyle    (m_plot_colors[k]               , 0.0f);
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
        }// END FOR-LOOP.
        
        ImPlot::EndPlot();
    }
    //
    //
    ImGui::PopID();
    
    
    return;
}


//  "_PlotSingles"
//
void CCounterApp::_PlotSingles(void) const noexcept
{
    namespace           cc          = ccounter;
    const PerFrame &    PF          = this->m_perframe;
    const Style &       CS          = this->m_style;
    
        


    //      6.      DRAW THE TABLE OF EACH INDIVIDUAL COUNTER...
    //
    if ( ImGui::BeginTable(ms_PLOT_UUIDs[1],  6,  ms_i_plot_table_flags) ) //  ImGuiTableFlags_BordersOuter | ImGuiTableFlags_RowBg
    {
        ImGui::TableSetupColumn     ("Visibility     "      , ms_i_plot_column_flags   , 2 * ms_I_PLOT_COL_WIDTH  );
        ImGui::TableSetupColumn     ("APD Counter"          , ms_i_plot_column_flags   , ms_I_PLOT_COL_WIDTH      );
        ImGui::TableSetupColumn     ("Max"                  , ms_i_plot_column_flags   , ms_I_PLOT_COL_WIDTH      );
        ImGui::TableSetupColumn     ("Avg."                 , ms_i_plot_column_flags   , ms_I_PLOT_COL_WIDTH      );
        ImGui::TableSetupColumn     ("Current"              , ms_i_plot_column_flags   , ms_I_PLOT_COL_WIDTH      );
        ImGui::TableSetupColumn     ("Plot"                 , ms_i_plot_plot_flags     , ms_I_PLOT_PLOT_WIDTH     );
        ImGui::TableHeadersRow();


        for (size_t row = 0; row < static_cast<size_t>(ms_NUM); ++row)
        {
            const auto &        buf             = m_buffers[row];
            const auto &        channel         = ms_channels[row];
            const bool          is_empty        = buf.empty();
            ImGui::TableNextRow();


            //      1.      COLUMN 1.       VISIBILITY SWITCHES...
            ImGui::TableSetColumnIndex(0);
            ImGui::Checkbox( channel.vis.master_ID      , &channel.vis.master );
            ImGui::SameLine();
            ImGui::Checkbox( channel.vis.average_ID     , &channel.vis.average );
            ImGui::SameLine();
            ImGui::Checkbox( channel.vis.single_ID      , &channel.vis.single );
            
            
            //      2.      COLUMN 2.       CHANNEL ID...
            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted(ms_channels[row].name);


            //      3.      COLUMN 3.       MAXIMUM VALUE...
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%.0f", m_max_counts[row]);


            //      4.      COLUMN 4.       AVERAGE VALUE...
            ImGui::TableSetColumnIndex(3);
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
                    this->plot_sparkline(
                          buf
                        , this->m_plot_colors[row]
                        , ImVec2(-1, cc::row_height_px)
                        , PF.spark_now
                        , this->m_history_length.Value()
                        , this->ms_CENTER
                        , CS.m_ind_pline_flags
                    );
                }
                ImGui::PopID();
            }
        //
        //
        }// END "FOR-LOOP".
    
    
    ImGui::EndTable();
    //
    //
    }// END "table".


    return;
}




//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "2.  PLOTTING FUNCTIONS".












// *************************************************************************** //
//
//
//
//      3.      PLOTTING UTILITIES...
// *************************************************************************** //
// *************************************************************************** //



//  "plot_sparkline"
//
template<typename RB>
inline void CCounterApp::plot_sparkline(
      const RB &                data
    , const ImVec4 &            color
    , const ImVec2              size
    , const float               time
    , const float               window
    , const float               center
    , const ImPlotAxisFlags     flags) const noexcept
{
    constexpr float         fill_alpha      =  0.25f;
    const ImPlotAxisFlags   axis_flags      = flags | ImPlotAxisFlags_AutoFit;
        
        

    const float             xmin            = time - center * window;
    const float             xmax            = xmin + window;
    
        
        
    ImPlot::PushStyleVar( ImPlotStyleVar_PlotPadding, ImVec2(0, 0) );

    if ( ImPlot::BeginPlot("##spark", size, ImPlotFlags_CanvasOnly) )
    {
        ImPlot::SetupAxes(nullptr, nullptr, axis_flags, axis_flags);
        
        ImPlot::SetupAxisLimits(ImAxis_X1, xmin, xmax, ImGuiCond_Always);

        if ( !data.empty() )
        {
            ImPlot::SetNextLineStyle    (color);
            ImPlot::SetNextFillStyle    (color, fill_alpha);

            //      IMPORTANT:      base pointer + offset + stride (no &data.front())
            ImPlot::PlotLine(
                  "##data"
                , &data.raw()[0].x
                , &data.raw()[0].y
                , static_cast<int>( data.size() )
                , ImPlotLineFlags_Shaded
                , static_cast<int>( data.offset() )
                , static_cast<int>( sizeof(ImVec2) )
            );
        }

        ImPlot::EndPlot();
    //
    //
    }// END PLOT.
    
    
    ImPlot::PopStyleVar();
    
    return;
}




//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "2.  PLOTTING UTILITIES".












// *************************************************************************** //
//
//
//
//      ?       UTILITY PLOTS...
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



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "4.  UTILITIES".


















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

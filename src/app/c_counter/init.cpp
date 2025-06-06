/***********************************************************************************
*
*       ********************************************************************
*       ****                  I N I T  ____  F I L E                   ****
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
    ms_I_PLOT_COL_WIDTH                                        *= S.m_dpi_scale;
    ms_SPACING                                                 *= S.m_dpi_scale;
    m_mst_plot_slider_height                                   *= S.m_dpi_scale;
    m_mst_plot_height                                          *= S.m_dpi_scale;
    //ms_COLLAPSE_BUTTON_SIZE.x                                *= S.m_dpi_scale;
    //ms_COLLAPSE_BUTTON_SIZE.y                                *= S.m_dpi_scale;
    m_cmap                                                      = ImPlot::GetColormapIndex("CCounter_Map");
    
    
    //  1.      ASSIGN THE CHILD-WINDOW CLASS PROPERTIES...
    m_window_class[0].DockNodeFlagsOverrideSet                  = ImGuiDockNodeFlags_HiddenTabBar;      //  ImGuiDockNodeFlags_HiddenTabBar; //ImGuiDockNodeFlags_NoTabBar;
    m_window_class[1].DockNodeFlagsOverrideSet                  = ImGuiDockNodeFlags_HiddenTabBar;      //ImGuiDockNodeFlags_HiddenTabBar; //ImGuiDockNodeFlags_NoTabBar;
    
    
    std::snprintf(m_filepath, ms_BUFFER_SIZE, "%s", app::PYTHON_DUMMY_FPGA_FILEPATH);
    
        
        
    //  2.      DEFAULT TAB OPTIONS...
    static ImGuiTabItemFlags        ms_DEF_PLOT_TAB_FLAGS       = ImGuiTabItemFlags_None;
    static ImGuiTabItemFlags        ms_DEF_CTRL_TAB_FLAGS       = ImGuiTabItemFlags_None;
    
    
    //  3A.     TABS FOR PLOT WINDOW...
    ms_PLOT_TABS                                                = {
    //          TAB NAME.                   OPEN.           NOT CLOSE-ABLE.     FLAGS.                          CALLBACK.
        Tab_t(  "C-Counter Plots",          true,           true,               ms_DEF_PLOT_TAB_FLAGS,          nullptr)
    };
    

    //  4A.     TABS FOR CONTROL WINDOW...
    ms_CTRL_TABS                                                = {
    //          TAB NAME.                   OPEN.           NOT CLOSE-ABLE.     FLAGS.                          CALLBACK.
        Tab_t(  "C-Counter Controls",       true,           true,               ms_DEF_CTRL_TAB_FLAGS,          nullptr)
    };
    
    
    //  3B.     ASSIGN THE CALLBACK RENDER FUNCTIONS FOR EACH PLOT TAB...
    for (std::size_t i = 0; i < ms_PLOT_TABS.size(); ++i) {
        auto &      tab                     = ms_PLOT_TABS[i];
        ms_PLOT_TABS[i].render_fn           = [this, tab]([[maybe_unused]] const char * id, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
                                              { this->dispatch_plot_function( tab.uuid ); };
    }
    //  4B.     ASSIGN CALLBACKS TO EACH CTRL TAB...
    for (std::size_t i = 0; i < ms_CTRL_TABS.size(); ++i) {
        auto &      tab                     = ms_CTRL_TABS[i];
        ms_CTRL_TABS[i].render_fn           = [this, tab]([[maybe_unused]] const char * id, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
                                              { this->dispatch_ctrl_function( tab.uuid ); };
    }
    
    
    
    
    
    
    //m_plot_colors                           = cb::utl::GetColormapSamples( m_NUM_COLORS, m_cmap );
    this->init_ctrl_rows();
    
    
    
    //  END INITIALIZATION...
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
//  2.  TAB FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "dispatch_plot_function"
//
void CCounterApp::dispatch_plot_function(const std::string & uuid)
{
    bool            match       = false;
    const size_t    N           = ms_PLOT_TABS.size();
    size_t          idx         = N + 1;       //   Default:    if (NO MATCH):  "N < idx"

    //  1.  FIND THE INDEX AT WHICH THE TAB WITH NAME "uuid" IS FOUND...
    for (size_t i = 0; i < N && !match; ++i) {
        match = ( uuid == ms_PLOT_TABS[i].uuid );
        if (match) idx = i;
    }
    
    if (!match) return;
    
    
    auto &          T       = ms_PLOT_TABS[idx];
    //  DISPATCH EACH RENDER FUNCTION FOR EACH WINDOW OF THE APPLICATION...
    switch (idx)
    {
        //      0.  Model PARAMETERS...
        case 0:         {
            this->ShowCCPlots();
            break;
        }
        //
        //
        //
        default: {
            break;
        }
    }
    return;
}


//  "dispatch_ctrl_function"
//
void CCounterApp::dispatch_ctrl_function(const std::string & uuid)
{
    bool            match       = false;
    const size_t    N           = ms_CTRL_TABS.size();
    size_t          idx         = N + 1;       //   Default:    if (NO MATCH):  "N < idx"

    //  1.  FIND THE INDEX AT WHICH THE TAB WITH NAME "uuid" IS FOUND...
    for (size_t i = 0; i < N && !match; ++i) {
        match = ( uuid == ms_CTRL_TABS[i].uuid );
        if (match) idx = i;
    }
    if (!match) return;
    
    
    auto &          T       = ms_PLOT_TABS[idx];
    
    
    //  DISPATCH EACH RENDER FUNCTION FOR EACH WINDOW OF THE APPLICATION...
    switch (idx)
    {
        //      0.  Model PARAMETERS...
        case 0:         {
            this->ShowCCControls();
            break;
        }
        //
        //
        //
        default: {
            break;
        }
    }
    return;
}


//  "DefaultPlotTabRenderFunc"
//
void CCounterApp::DefaultPlotTabRenderFunc([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags) {
    if (!p_open)
        return;
        
    ImGui::Text("Window Tab \"%s\".  Here is some default text dispatched by \"DefaultPlotTabRenderFunc()\".", uuid);
    return;
}


//  "DefaultCtrlTabRenderFunc"
//
void CCounterApp::DefaultCtrlTabRenderFunc([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags) {
    if (!p_open)
        return;
        
    ImGui::BulletText("Tab UUID: \"%s\".", uuid);
        
    ImGui::Text("Window Tab \"%s\".  Here is some default text dispatched by \"DefaultCtrlTabRenderFunc()\".", uuid);
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













/*
void CCounterApp::ShowCCPlots(void)
{

    //---------------------------------------------
    // 3)  Poll child stdout and update buffers
    //---------------------------------------------
    std::string         raw;
    const float         now         = static_cast<float>(ImGui::GetTime());
    while (proc.try_receive(raw))
    {
        if ( auto pkt = utl::parse_packet(raw, m_use_mutex_count) )
        {
            const auto &    counts = pkt->counts; // std::array<int,16>
            for (int i = 0; i < ms_NUM; ++i)
            {
                size_t      channel_idx     = ms_channels[i].idx;
                float       v               = static_cast<float>(counts[channel_idx]);
                m_buffers[i].AddPoint( ImVec2(now, v) );
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
                    if ( !data.empty() ) {
                        //  ImPlot::PlotLine( ms_channels[k].name,     &data.Data[0].x,     &data.Data[0].y,     data.Data.size(),        ImPlotLineFlags_Shaded, data.Offset, 2 * sizeof(float));
                        ImPlot::PlotLine(
                            ms_channels[k].name,          // label
                            &data.raw()->x,               // pointer to first X
                            &data.raw()->y,               // pointer to first Y
                            data.size(),                  // sample count
                            ImPlotLineFlags_Shaded,       // flags as before
                            data.offset(),                // chronological start index
                            sizeof(ImVec2)                // stride bytes (two floats)
                        );
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
                //const auto &        vec         = buf.Data();


                ImGui::TableNextRow();              //  ROW 0 :     COUNTER(S)...
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted( ms_channels[row].name );
                
                
                
                
                ImGui::TableSetColumnIndex(1);      //  ROW 1 :     MAX...
                //ImGui::BulletText("%.0f",           m_max_counts[row]);
                //
                ImGui::Text("%.0f", m_max_counts[row]);
                  
                
                
                ImGui::TableSetColumnIndex(2);      //  ROW 2 :     AVERAGE...
                //float avg = 0.f;
                if (!vec.empty()) {
                    for (const auto & pt : vec)
                        m_avg_counts[row] += pt.y;
                        
                        m_avg_counts[row] /= static_cast<float>(vec.size());
                }
                ImGui::Text("%.2f", m_avg_counts[row]);
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
*/

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



// *************************************************************************** //
//
//
//
//      1.      PUBLIC API FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "Begin"
//
void CCounterApp::Begin([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
{
    //      1.      INVOKE THE MAIN PER-FRAME GUI-UPDATE-LOOP FOR THE CCOUNTER APP...
    ImGui::Begin(uuid, p_open, flags);
        this->_Begin_IMPL();
    ImGui::End();
    
    
    //      2.      DRAW THE SECONDARY WINDOW [ DOCKED IN THE DETVIEW OF `CBAPP` ]...
    //                  ** ************************************************************* **
    //                  **    DO NOT INVOKE ANY CRITICAL OPERATIONS FROM THIS FUNC!!!    **
    //                  **       This is b.c. they will NOT be invoked if the user       **
    //                  **               has CLOSED the DetView of CBApp.                **
    //                  ** ************************************************************* **
    if (m_detview_window.open)
    {
        //ImGui::SetNextWindowClass(&this->m_window_class[1]);
        ImGui::Begin( m_detview_window.uuid.c_str(), nullptr, m_detview_window.flags );
            this->_Begin_DetView_IMPL();
        ImGui::End();
    }

    return;
}



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



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "1.  MAIN API".












// *************************************************************************** //
//
//
//
//      2.      INTERNAL API FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_Begin_IMPL"
//
inline void CCounterApp::_Begin_IMPL(void) noexcept
{
    namespace           cc              = ccounter;
    this->m_perframe.now                = static_cast<float>( ImGui::GetTime() );
                                    
    if ( m_process_running )            { this->m_perframe.now += ImGui::GetIO().DeltaTime; }
    
    
    this->m_perframe.spark_now          = (!m_counter_running   ? m_freeze_now    : ( (m_smooth_scroll)     ? this->m_perframe.now   : m_last_packet_time) );
    //  const float         spark_now       = (!m_counter_running   ? m_freeze_now    : ( (m_smooth_scroll)     ? now   : m_last_packet_time) );
    //  const auto          [xmin, xmax]    = this->_FetchData(now, spark_now);
    this->_FetchData();
    
    
    
    
    
    
    //      1.      PER-FRAME CACHE OPERATIONS...
    this->_MECH_per_frame_cache();
    
    
    
    //      2.      DRAW TOP-MOST CONTROL BAR...
    this->_MECH_draw_controls();
    
    
    
    
    

    //      3.      MASTER PLOT...
    //  ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    //  if ( ImGui::CollapsingHeader("Master Plot") )
    //  {
    //
        this->_PlotMaster();
    //
    //
    //  }



    //      4.      INDIVIDUAL COUNTER PLOTS...
    //  ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    //  if ( ImGui::CollapsingHeader("Individual Counters") )
    //  {
    //      this->_PlotSingles();
    //  }



    return;
}



//  "_Begin_DetView_IMPL"              //  formerly:   "display_controls"
//
inline void CCounterApp::_Begin_DetView_IMPL(void) noexcept
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
                        this->DefaultTabRenderFunc(tab.get_uuid(), &tab.open, tab.flags);
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





//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "2.  INTERNAL API".












// *************************************************************************** //
//
//
//
//      3A.     CORE "_MECH_*" MECHANICS...
// *************************************************************************** //
// *************************************************************************** //



//  "_MECH_per_frame_cache"
//
inline void CCounterApp::_MECH_per_frame_cache(void) noexcept
{
    namespace           cc              = ccounter;
    this->m_perframe.now                = static_cast<float>( ImGui::GetTime() );
                                    
    if ( m_process_running )            { this->m_perframe.now += ImGui::GetIO().DeltaTime; }
    
    
    this->m_perframe.spark_now          = (!m_counter_running   ? m_freeze_now    : ( (m_smooth_scroll)     ? this->m_perframe.now   : m_last_packet_time) );
    //  const float         spark_now       = (!m_counter_running   ? m_freeze_now    : ( (m_smooth_scroll)     ? now   : m_last_packet_time) );
    //  const auto          [xmin, xmax]    = this->_FetchData(now, spark_now);
    this->_FetchData();
    
    
    
    
    
    
    return;
}



//  "_MECH_draw_controls"
//
inline void CCounterApp::_MECH_draw_controls(void) noexcept      //  formerly: "_draw_control_bar"
{
    using                                   IconAnchor                  = utl::icon_widgets::Anchor;
    using                                   Padding                     = utl::icon_widgets::PaddingPolicy;
    //
    static constexpr const char *           uuid                        = "##Editor_Controls_Columns";
    static constexpr int                    ms_NC                       = 7;
    static constexpr int                    ms_NE                       = 2;
    static constexpr float                  ms_TOOLBAR_ICON_SCALE       = 1.5f;
    //
    static ImGuiOldColumnFlags              COLUMN_FLAGS                = ImGuiOldColumnFlags_None;
    static ImVec2                           WIDGET_SIZE                 = ImVec2( -1,  32 );
    static ImVec2                           BUTTON_SIZE                 = ImVec2( 32,   WIDGET_SIZE.y );
   
   
   
    const bool      running     = this->m_process_running;
    const bool      rec         = this->m_process_recording;
   
   
   
    //      BEGIN COLUMNS...
    //
    this->S.PushFont( Font::Small );
    ImGui::Columns(ms_NC, uuid, COLUMN_FLAGS);
    //
    //
    //
        //      1A.         START/STOP PYTHON SCRIPT...
        this->S.column_label( (running)     ? "Stop Process:"   : "Start Process:" );
        //
        ImGui::PushItemWidth( BUTTON_SIZE.x );
        this->S.PushFont(Font::Main);
        {
            if ( utl::IconButton(   "##CCounter_Controls_StartProcess"
                                  , (running)     ? this->S.SystemColor.Yellow      : this->S.SystemColor.Blue
                                  , (running)     ? ICON_FA_PAUSE                   : ICON_FA_PLAY
                                  , ms_TOOLBAR_ICON_SCALE ) )
            {
                if ( !running )     { this->_start_process(/*run_and_rec=*/false);  }   //      CASE 1 :    START PROCESS...
                else                { this->_stop_process(/*pause=*/true);          }   //      CASE 2 :    PAUSE PROCESS...
            }
        }
        //
        //
        //
        //      1B.         START/STOP RECORDING DATA...
        {
            ImGui::SameLine();
            if ( utl::IconButton(   "##CCounter_Controls_RecordProcess"
                                  , (rec)       ? this->S.SystemColor.Red       : this->S.SystemColor.Disabled
                                  , (rec)       ? ICON_FA_MICROPHONE_LINES      : ICON_FA_MICROPHONE_LINES_SLASH
                                  , ms_TOOLBAR_ICON_SCALE ) )
            {
            //
            //
                //      CASE 1 :    START SCRIPT WITH RECORDING...
                if ( !rec ) {
                    if ( !running )     { this->_start_process(/*run_and_rec=*/true);   }   //      CASE 1 :    START PROCESS AND BEGIN RECORDING...
                    else                { this->_start_recording();                     }   //      CASE 2 :    RECORD EXISTING SESSION...
                }
                else {
                    if ( running )      { this->_stop_process(/*pause=*/false);         }   //      CASE 3 :    STOP PLAYBACK; FINALIZE RECORDING...
                    else                { this->_stop_recording();                      }   //      CASE 4 :    FINALIZE RECORDING...
                }
            //
            //
            }
        }
        //
        this->S.PopFont();
        
        
        
        
        
        
        
        /*
        if (!m_process_running)
        {
            if ( ImGui::Button("Start Process") )
            {
                m_process_running               = m_python.start();
                if ( !m_process_running )
                {
                    ImGui::OpenPopup("launch_error");
                }
                else {
                    m_max_counts[0] = 0.0f;
                } // reset stats
            }
        }
        //
        //      CASE 2 :    SCRIPT  **IS**  RUNNING...
        else
        {
            if ( utl::CButton("Stop Process", app::DEF_APPLE_RED) ) {
                m_python.stop();
                m_process_running = false;
            }
            //      ImGui::PopStyleColor(2);
        }*/
        
        
        
        
        //      2.      Plot Crawling.
        ImGui::NextColumn();        this->S.column_label("Plot Crawling:");
        ImGui::Checkbox("##CCounterControls_PlotCrawling",              &m_smooth_scroll);
    
    
    
        
        //      3.      Mutex Counts.
        ImGui::NextColumn();        this->S.column_label("Use Mutex Counts:");
        ImGui::SetNextItemWidth( BUTTON_SIZE.x );
        ImGui::Checkbox("##CCounterControls_UseMutexCounts",           &m_use_mutex_count);
            
                
        
        
        //      4.      Reset Averages.
        ImGui::NextColumn();
        if ( ImGui::Button("Reset Averages") )
        {
            for (auto & vec : m_avg_counts) {
                vec.clear();    //  b.Erase();
            }
        }
        
        
        
        //      5.      Clear Plot.
        ImGui::NextColumn();
        if ( ImGui::Button("Clear Plot") )
        {
            for (auto & b : m_buffers) {
                b.clear(); //b.Erase();
            }
            std::fill(std::begin(m_max_counts), std::end(m_max_counts), 0.0f);
        }






        //      ?.      EMPTY SPACES FOR LATER...
        for (int i = ImGui::GetColumnIndex(); i < ms_NC - ms_NE; ++i) {
            ImGui::Dummy( ImVec2(0,0) );    ImGui::NextColumn();
        }






        //      X.1.    CLEAR ALL...
        this->S.column_label("Clear Data:");
        //
        ImGui::PushItemWidth( BUTTON_SIZE.x );
        this->S.PushFont(Font::Main);
        {
            if ( utl::IconButton(   "##Editor_Controls_ClearAllData"
                                  , this->S.SystemColor.Red
                                  , ICON_FA_TRASH_CAN
                                  , ms_TOOLBAR_ICON_SCALE ) )
            {
                ui::ask_ok_cancel(
                      "Clear Data"
                    , (rec)
                    ? "This action will clear all data from the plot and stop the current recording.\n\nDo you wish to proceed?"
                    : "This action will clear all data from the plot.\n\nDo you wish to proceed?"
                    , [this]{ this->_clear_all(); }
                );
            }
            //  this->m_tooltip.UpdateTooltip( TooltipKey::ClearData );
        }
        this->S.PopFont();
        ImGui::PopItemWidth();
        
        
        
    //  popup::Draw();
    //
    //
    //
    ImGui::Columns(1);      //  END COLUMNS...
    
    
    
    
    
    
    
    this->S.PopFont();
   
    return;
}



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "3A.  CORE "_MECH_*" MECHANICS".












// *************************************************************************** //
//
//
//
//      3B.     "_MECH_*" HELPER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_FetchData"
//
inline void/*std::pair<float, float>*/ CCounterApp::_FetchData(void) noexcept
{
    namespace               cc                  = ccounter;
    //  std::string             raw;
    //  bool                    got_packet          = false;
    //
    this->m_perframe.got_packet                 = false;
    this->m_perframe.xmin                       = 0.0f;
    this->m_perframe.xmax                       = m_history_length.value;
    //  float                   xmin                = 0.0f,
    //                          xmax                = m_history_length.value;
                                    
          
    //      1.      POLL THE CHILD-PROCESS.  PUSH NEW DATA-POINTS...
    //
    while ( this->m_python.try_receive( this->m_perframe.raw ) )
    {
        this->m_perframe.got_packet = true;
        //  if ( auto packet = cc::parse_packet(raw, m_use_mutex_count) )
        if ( auto packet = cc::parse_packet(this->m_perframe.raw, m_use_mutex_count) )
        {
            const auto &    counts      = packet->counts;
            for (int i = 0; i < static_cast<int>(ms_NUM); ++i)
            {
                const size_t        ch_idx          = ms_channels[i].idx;
                const float         current         = static_cast<float>( counts[ch_idx] );
                const float         avg             = this->ComputeAverage(
                      m_buffers[i]
                    , m_avg_mode
                    , m_avg_window_samp.value
                    , m_avg_window_sec.value
                    , this->m_perframe.spark_now    //    spark_now
                );
                    
                
                m_buffers[i]        .push_back({ this->m_perframe.now, current });               //  1.  PUSH-BACK MOST RECENT FPGA DATA PACKET.
                m_max_counts[i]     = std::max(m_max_counts[i], current);       //  2.  COMPUTE CURRENT MAX VALUE FOR THIS COUNTER.
                m_avg_counts[i]     .push_back({ this->m_perframe.now, avg });                   //  3.
            }
        }
    }
    //
    //  if (got_packet)     { this->m_last_packet_time = now; }
    if (this->m_perframe.got_packet)        { this->m_last_packet_time = this->m_perframe.now; }


    //      streaming considered active if we’ve received data within timeout
    //  m_streaming_active  = (now - m_last_packet_time) < m_stream_timeout;
    m_streaming_active  = (this->m_perframe.now - m_last_packet_time) < m_stream_timeout;



    //------------------------------------------------------------------
    // 2.  Update cached freeze range when streaming or on first frame
    //------------------------------------------------------------------
    if (!m_counter_running) {                                   //  Recording stopped – freeze completely.
        //  xmin = m_freeze_xmin;
        //  xmax = m_freeze_xmax;
        this->m_perframe.xmin = m_freeze_xmin;
        this->m_perframe.xmax = m_freeze_xmax;
    }
    else if (m_smooth_scroll) {                                 //  Continuous crawl following real time.
        //  xmin            = now - ms_CENTER * m_history_length.value;
        //  xmax            = xmin + m_history_length.value;
        //  m_freeze_xmin   = xmin;                                 //  Keep cache fresh in case we pause later.
        //  m_freeze_xmax   = xmax;
        //
        this->m_perframe.xmin   = this->m_perframe.now - ms_CENTER * m_history_length.Value();
        this->m_perframe.xmax   = this->m_perframe.xmin + m_history_length.Value();
        m_freeze_xmin           = this->m_perframe.xmin;                                 //  Keep cache fresh in case we pause later.
        m_freeze_xmax           = this->m_perframe.xmax;
    }
    else { // Stepped mode
        if (this->m_perframe.got_packet) {                                       //  Jump so the newest packet sits at right edge of window.
            //  xmin            = m_last_packet_time - ( this->ms_CENTER * m_history_length.value );
            //  xmax            = xmin + m_history_length.value;
            //  m_freeze_xmin   = xmin;
            //  m_freeze_xmax   = xmax;                                  //  Jump so the newest packet sits at right edge of window.
            //
            this->m_perframe.xmin   = m_last_packet_time - ( this->ms_CENTER * m_history_length.value );
            this->m_perframe.xmax   = this->m_perframe.xmin + m_history_length.Value();
            m_freeze_xmin           = this->m_perframe.xmin;
            m_freeze_xmax           = this->m_perframe.xmax;
        }
        else {                              //  Hold still between packets.
            //  xmin            = m_freeze_xmin;
            //  xmax            = m_freeze_xmax;
            //
            this->m_perframe.xmin            = m_freeze_xmin;
            this->m_perframe.xmax            = m_freeze_xmax;
        }
    }

    //  Ensure an initial range on app startup
    //  if (xmax < xmin) {
    //      xmin            = 0.0f;
    //      xmax            = m_history_length.value;
    //      m_freeze_xmin   = xmin;
    //      m_freeze_xmax   = xmax;
    //  }
    if (this->m_perframe.xmax < this->m_perframe.xmin) {
        this->m_perframe.xmin       = 0.0f;
        this->m_perframe.xmax       = m_history_length.value;
        m_freeze_xmin               = this->m_perframe.xmin;
        m_freeze_xmax               = this->m_perframe.xmax;
    }

    return;
    //return {this->m_perframe.xmin, this->m_perframe.xmax};
}


//  "ComputeAverage"
//      Helper to compute localized average
//
inline float CCounterApp::ComputeAverage(const buffer_type &buf, AvgMode mode, ImU64 N_samples, double seconds, float now) const
{
    const std::size_t   size        = buf.size();
    float               sum         = 0.0f;
    std::size_t         count       = 0;
    
    
    
    if (size == 0)      { return 0.0f; }



    //      CASE 1 :    COUNT-BASED AVERAGE...
    if (mode == AvgMode::Samples)
    {
        const ImU64         N       = std::min<ImU64>(N_samples, size);
        const std::size_t   start   = size - static_cast<std::size_t>(N);
        for (std::size_t i = start; i < size; ++i)
        {
            sum += buf[i].y;
            ++count;
        }
    }
    //
    //      CASE 2 :    TIME-BASED AVERAGE...
    else
    {
        for (std::size_t i = size; i-- > 0; )
        {
            const float     dt      = now - buf[i].x;
            if (dt > seconds)       { break; }
            sum += buf[i].y;
            ++count;
        }
    }
    return (count)    ? sum / static_cast<float>(count)     : 0.0f;
}








//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "3B.  CORE-MECHANIC HELPERS".
























// *************************************************************************** //
//
//
//
//      ?.      OLD "ShowCCPlots"...
// *************************************************************************** //
// *************************************************************************** //


//  "ShowCCPlots"       //  formerly: "ShowCCPlots"
/*
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


            for (size_t row = 0; row < static_cast<size_t>(ms_NUM); ++row)
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
}*/
















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

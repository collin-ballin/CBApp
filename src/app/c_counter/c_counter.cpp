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
    static cblib::ndRingBuffer<float> test;
    

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


//  "open"
//
void CCounterApp::open(void) {
    CB_LOG( LogLevel::Info, "CCounter--open" );
    //  m_editor.open();
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
    //      1.      PERFORM ALL PER-FRAME COMPUTE OPERATIONS...
    this->_MECH_per_frame_cache();
    
    
    
    //      2.      DRAW TOP-MOST CONTROL BAR...
    this->_MECH_draw_controls();
    
    
    
    //      3.      MASTER PLOT...
    this->_PlotMaster();



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
	PerFrame_t &    PF			    = this->m_perframe;

    // 1)       Wall clock & dt
    const float     wall_now        = static_cast<float>( ImGui::GetTime() );
    const float     dt_frame        = ImGui::GetIO().DeltaTime;



    // 2)       Let _FetchData() pull packets and set PF.got_packet
    //
    //      NOTE: _FetchData() must not be called again elsewhere this frame.
    this->_FetchData();                    // sets PF.got_packet, updates m_last_packet_time, PF.xmin/xmax seed


    // 3)       Run state & user intent
    const bool   running       = (m_process_running  &&  m_counter_running);
    const bool   smooth        = (m_smooth_scroll != 0);
    const float  dt_packet     = m_integration_window.Value();


    // 4)       Last sample time from the anchor channel (0)
    const float  last_x        = m_buffers[0].empty()   ? 0.0f  : m_buffers[0].back().x;


    // 5)       Between-packet crawl (local static; reset on PF.got_packet)
    //
    //              IMPORTANT: Do NOT try to infer arrivals from size() once the ring is full.
    static float s_crawl_off   = 0.0f;
    if (PF.got_packet)               { s_crawl_off = 0.0f;                                          }
    else if (running && smooth)      { s_crawl_off = std::min(s_crawl_off + dt_frame, dt_packet);   }
    // paused or stepped → hold s_crawl_off as-is


    // 6)       Final crawl gate (no dependency on m_stream_timeout)
    const bool   crawling      = (running && smooth);
    PF.crawling                = crawling;


    // 7)       Compute the window from the right edge; clamp between-packet advance
    const float     history_len     = this->m_history_length.Value();
    const float     right_edge      = crawling   ? (last_x + s_crawl_off)    : last_x;
    float           xmin            = right_edge - ms_CENTER * history_len;
    float           xmax            = xmin       +             history_len;


    // 8)       Latch/hold policy
    if (!m_counter_running) {
        xmin    = m_freeze_xmin;
        xmax    = m_freeze_xmax;
    }
    else if (crawling) {
        m_freeze_xmin = xmin;  m_freeze_xmax = xmax;         // keep cache fresh while crawling
    }
    else { // stepped or paused
        if (!smooth) {
            if (PF.got_packet) {
                m_freeze_xmin = xmin;  m_freeze_xmax = xmax; // jump only on arrivals
            }
        }
        xmin = m_freeze_xmin;  xmax = m_freeze_xmax;
    }


    // 9)       Publish per-frame window
    if (xmax <= xmin)   { xmin = 0.0f; xmax = history_len; }
    PF.xmin = xmin;
    PF.xmax = xmax;


    // 10)      Spark timing consistent with the same semantics
    PF.spark_now = (!m_counter_running)     ? m_freeze_now
                                            : ( smooth
                                                ? (crawling
                                                    ? right_edge    : m_freeze_now)
                                                :  last_x );


    // (Optional) Your UI LED can still use the timeout:
    m_streaming_active = (wall_now - m_last_packet_time) < m_stream_timeout;

    // 11)      Revalidate colormap cache if needed
    if (m_colormap_cache_invalid)   { _validate_colormap_cache(); }
    
    return;
}

/*
{
    namespace           cc      = ccounter;
    PerFrame &          PF      = this->m_perframe;
    
    PF.now                      = static_cast<float>( ImGui::GetTime() );
            
    if ( m_process_running )     { PF.now += ImGui::GetIO().DeltaTime; }
    
    
    PF.spark_now                = (!m_counter_running   ? m_freeze_now    : ( (m_smooth_scroll)     ? PF.now   : m_last_packet_time) );
    //  const float         spark_now       = (!m_counter_running   ? m_freeze_now    : ( (m_smooth_scroll)     ? now   : m_last_packet_time) );
    //  const auto          [xmin, xmax]    = this->_FetchData(now, spark_now);
    
    
    
    //      2.      FETCH DATA FROM PYTHON PROCESS...
    this->_FetchData();
    
    
    //      3.      VERIFY IF COLOR-MAP CACHE NEEDS RE-VALIDATION...
    if ( this->m_colormap_cache_invalid ) {
        this->_validate_colormap_cache();
    }
    
    
    return;
}*/



//  "_MECH_draw_controls"
//
inline void CCounterApp::_MECH_draw_controls(void) noexcept      //  formerly: "_draw_control_bar"
{
    using                                   IconAnchor                  = utl::icon_widgets::Anchor;
    using                                   Padding                     = utl::icon_widgets::PaddingPolicy;
    //
    static constexpr const char *           uuid                        = "##Editor_Controls_Columns";
    static constexpr int                    ms_NC                       = 8;    //  # columns at BEGINNING.
    static constexpr int                    ms_NE                       =  4;    //  # cols at END (*AFTER* the spacer/empty columns).
    //
    static ImGuiOldColumnFlags              COLUMN_FLAGS                = ImGuiOldColumnFlags_None;
    static ImVec2                           WIDGET_SIZE                 = ImVec2( -1,  32 );
    static ImVec2                           BUTTON_SIZE                 = ImVec2( 32,   WIDGET_SIZE.y );
   
   
   
    const float         scale       = this->m_style.ms_TOOLBAR_ICON_SCALE;
    const bool          running     = this->m_process_running;
    const bool          rec         = this->m_process_recording;
   
   
    //      BEGIN COLUMNS...
    //
    this->S.PushFont( Font::Small );
    ImGui::Columns(ms_NC, uuid, COLUMN_FLAGS);
    //
    //
    //
        //      1.      ** PROCESS CONTROLS **
        //
        //              1A.         Start / Stop Script:
        //this->S.column_label( (running)     ? "Stop Process:"   : "Start Process:" );
        this->S.column_label( "Process Controls:" );
        //
        ImGui::PushItemWidth( BUTTON_SIZE.x );
        this->S.PushFont(Font::Main);
        {
            if ( utl::IconButton(   "##CCounter_ProcessControls_StartStopProcess"
                                  , (running)     ? this->S.SystemColor.Yellow      : this->S.SystemColor.Blue
                                  , (running)     ? ICON_FA_PAUSE                   : ICON_FA_PLAY
                                  , scale ) )
            {
                if ( !running )     { this->_start_process(/*run_and_rec=*/false);  }   //      CASE 1 :    START PROCESS...
                else                { this->_stop_process(/*pause=*/true);          }   //      CASE 2 :    PAUSE PROCESS...
            }
        }
        //
        //
        //
        //              1B.         Arm the Recording Process:
        {
            ImGui::SameLine();
            if ( utl::IconButton(   "##CCounter_ProcessControls_ArmRecording"
                                  , (this->m_recording_armed)   ? this->S.SystemColor.Red       : this->S.SystemColor.Disabled
                                  , (this->m_recording_armed)   ? ICON_FA_MICROPHONE_LINES      : ICON_FA_MICROPHONE_LINES_SLASH
                                  , scale ) )
            {
            //
            //
                //  CASE 1 :    SCRIPT IS RUNNING...
                if ( running )
                {
                    /*  [[ TO-DO ]]:    DO NOT ALLOW RECORDING TO BE START/STOPPED WITHOUT STOPPING ENTIRE SCRIPT...  */
                }
                //
                //
                //  CASE 2 :    SCRIPT IS *NOT* RUNNING...
                else
                {
                    /*  [[ TO-DO ]]:
                     *
                     *      - Open "File Dialog" menu if the user attempt to ARM recording but there is no output file specified...
                     *
                    */
                    
                    this->m_recording_armed     = !this->m_recording_armed;
                }
            //
            //
            }
        }
        //
        //
        //
        //              1C.         Start / Stop Recording Data:
        {
            const bool  active   = m_counter_running;   //this->m_process_running;
            
            
            //
            ImGui::SameLine();
            if ( utl::IconButton(   "##CCounter_ProcessControls_ProcessStatus"
                                  , (active)       ? this->S.SystemColor.Green      : this->S.SystemColor.Red
                                  , (active)       ? ICON_FA_CIRCLE                 : ICON_FA_CIRCLE
                                  , scale ) )
            {
                //  ...
            }
        }
        //
        this->S.PopFont();      //  [[ END "1. PROCESS CONTROLS." ]].
        
        
        
        
        
        
        //      2.      ** PLOT CONTROLS **
        //
        ImGui::NextColumn();
        this->S.column_label( "Plot Controls:" );
        //
        ImGui::PushItemWidth( BUTTON_SIZE.x );
        this->S.PushFont(Font::Main);
        {
            //          2A.         Plot Crawling:
            if ( utl::IconButton(   "##CCounter_PlotControls_PlotCrawling"
                                  , this->S.SystemColor.Blue
                                  , (this->m_smooth_scroll)         ? ICON_FA_REPEAT                    : ICON_FA_STAIRS
                                  , scale ) )
            {
                this->m_smooth_scroll = !this->m_smooth_scroll;
            }
            
            //          2B.         Relative Y-Axis Range:
            ImGui::SameLine();
            if ( utl::IconButton(   "##CCounter_PlotControls_RelativeRange"
                                  , this->S.SystemColor.Blue
                                  , (this->m_use_relative_range)     ? ICON_FA_ARROW_TREND_UP           : ICON_FA_CHART_LINE
                                  , scale ) )
            {
                this->m_use_relative_range = !this->m_use_relative_range;
            }
            
            //          2C.         Shuffle Color-Map Plot Colors:
            ImGui::SameLine();
            if ( utl::IconButton(   "##CCounter_PlotControls_StartProcess"
                                  , this->S.SystemColor.Blue
                                  , (this->m_use_shuffled_colormap)     ? ICON_FA_CHART_SIMPLE          : ICON_FA_SIGNAL  //    ICON_FA_ARROW_UP_WIDE_SHORT,
                                  , this->m_style.ms_TOOLBAR_ICON_SCALE ) )
            {
                this->m_use_shuffled_colormap       = !this->m_use_shuffled_colormap;
                this->m_colormap_cache_invalid      = true;
            }
        }
        //
        this->S.PopFont();      //  [[ END "2. PLOT CONTROLS" ]].
        
        
        
        
        
        
        //      3.      ** DAQ CONTROLS **
        //
        ImGui::NextColumn();
        this->S.column_label( "DAQ Controls:" );
        //
        ImGui::PushItemWidth( BUTTON_SIZE.x );
        this->S.PushFont(Font::Main);
        {
            //          3A.         Use Mutex Counts:
            if ( utl::IconButton(   "##CCounter_DAQControls_UseMutexCounts"
                                  , this->S.SystemColor.Orange
                                  , (this->m_use_mutex_count)           ? ICON_FA_CHART_PIE     : ICON_FA_CIRCLE_HALF_STROKE
                                  , scale ) )
            {
                this->m_use_mutex_count = !this->m_use_mutex_count;
            }
            
            
            
            //          3B.         Reset Average Values:
            ImGui::SameLine();
            const bool      clicked_avg     = utl::IconButton(
                /*  const char *            id          */   "##CCounter_DAQControls_ResetAverages"
                /*  ImVec4                  color       */ , this->S.SystemColor.Orange
                /*  char *                  icon_utf8   */ , "avg"
                /*  float                   scale       */ , scale
                /*  ImVec2 &                size        */ //, ImVec2(32.0f, 32.0f)
                /*  icon_w::Anchor          anchor      */ , IconAnchor::TextBaseline    // TextBaseline    South   Center
                /*  icon_w::PaddingPolicy   pad         */ , Padding::Tight
                /*  ImVec2 &                nudge       */ , ImVec2(0.0f, 0.0f)
            );
            //
            if (clicked_avg) {
                this->_reset_average_values();
            }
            
            
            
            //          3C.         Reset Plot Data:
            ImGui::SameLine();
            const bool      clicked_plot    = utl::IconButton(
                /*  const char *            id          */   "##CCounter_DAQControls_ResetPlot"
                /*  ImVec4                  color       */ , this->S.SystemColor.Orange
                /*  char *                  icon_utf8   */ , "plot"
                /*  float                   scale       */ , scale
                /*  icon_w::Anchor          anchor      */ , IconAnchor::TextBaseline    // TextBaseline    South   Center
                /*  icon_w::PaddingPolicy   pad         */ , Padding::Tight
            );
            //
            if (clicked_plot) {
                this->_clear_plot_data();
            }
            
            
            
        }
        //
        this->S.PopFont();      //  [[ END "3. PLOT CONTROLS" ]].
        
        
        
        
        
        
        //      2.      Plot Crawling.
        //  ImGui::NextColumn();        this->S.column_label("Plot Crawling:");
        //  ImGui::Checkbox("##CCounterControls_PlotCrawling",              &m_smooth_scroll);
    
    
    
        
        //      3.      Mutex Counts.
        //  ImGui::NextColumn();        this->S.column_label("Use Mutex Counts:");
        //  ImGui::SetNextItemWidth( BUTTON_SIZE.x );
        //  ImGui::Checkbox("##CCounterControls_UseMutexCounts",           &m_use_mutex_count);






        //      ?.      EMPTY SPACES FOR LATER...
        for (int i = ImGui::GetColumnIndex(); i < ms_NC - ms_NE; ++i) {
            ImGui::Dummy( ImVec2(0,0) );    ImGui::NextColumn();
        }
        
        
        
        
        
        
        //      X.1.    # OF PACKETS.
        ImGui::NextColumn();
        this->S.column_label("Packets Received:");
        ImGui::Text("%08zu", this->m_num_packets);
        
        
        



        this->S.PushFont(Font::Main);
        ImGui::PushItemWidth( BUTTON_SIZE.x );
        
        //      X.2.    SETTINGS...
        ImGui::NextColumn();    this->S.column_label("Settings:");
        {
            {
                if ( utl::IconButton(   "##CCounter_SettingsControls_OpenSettings"
                                      , this->S.SystemColor.White
                                      , ICON_FA_GEARS    //  ICON_FA_GEARS   ICON_FA_GEAR    ICON_FA_SLIDERS
                                      , scale ) )
                {
                    //  ui::open_preferences_popup( GetMenuID(PopupHandle::Settings), [this](popup::Context & ctx) { _draw_editor_settings(ctx); } );
                }
                //  this->m_tooltip.UpdateTooltip( TooltipKey::OpenSettings );
            }
        }
        //
        //
        //
        //      X.3.    CLEAR ALL...
        ImGui::NextColumn();        this->S.column_label("Clear Data:");
        {
            if ( utl::IconButton(   "##CCounter_SettingsControls_ClearAllData"
                                  , this->S.SystemColor.Red
                                  , ICON_FA_TRASH_CAN
                                  , scale ) )
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
        //
        //
        //
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
inline void CCounterApp::_FetchData(void) noexcept
{
    namespace                   cc                  = ccounter;
    PerFrame_t &                PF                  = this->m_perframe;
    const float                 history_len         = this->m_history_length.Value();
    //
    PF.got_packet                                   = false;
    PF.xmin                                         = 0.0f;
    PF.xmax                                         = history_len;

    // ------------------------------------------------------------------
    // 1) Poll Python; for each packet:
    //    - compute x_next on the Δt grid to avoid FP drift
    //    - stamp all channels at the same x_next
    // ------------------------------------------------------------------
    const float                 wall_now            = static_cast<float>(ImGui::GetTime());
    const bool                  running             = (m_process_running && m_counter_running);
    const bool                  smooth              = (m_smooth_scroll != 0);
    const float                 dt_packet           = m_integration_window.Value();

    ipc::PythonPacketType       py_type             = ipc::PythonPacketType::Unknown;
    std::string                 py_msg              {   };

    py_msg.reserve(IPCState::ms_MESSAGE_BUFFER_SIZE);



    // ------------------------------------------------------------------
    // DEBUG: dump raw stdout lines received from Python to terminal.
    // Enable by flipping `dump_enabled` to true. Throttled by `dump_limit`.
    // ------------------------------------------------------------------
    static bool                 dump_enabled        = true;
    static uint32_t             dump_count          = 0U;
    static constexpr uint32_t   dump_limit          = 256U;

    while ( this->m_python.try_receive(PF.raw) )
    {
        ++this->m_num_packets;

        if ( dump_enabled && (dump_count < dump_limit) )
        {
            ++dump_count;

            std::fputs("[py stdout] ", stderr);
            std::fwrite(PF.raw.data(), 1ULL, PF.raw.size(), stderr);

            if ( PF.raw.empty() || (PF.raw.back() != '\n') )    { std::fputc('\n', stderr); }
            std::fflush(stderr);
        }

        py_type = ipc::PythonPacketType::Unknown;
        py_msg.clear();


        if ( auto packet_ptr = ipc::parse_python_message<Packet_t>(PF.raw, m_use_mutex_count, py_type, py_msg) )
        {
            PF.got_packet = true;

            const Packet_t &    packet          = *packet_ptr;
            auto &              buf0            = m_buffers[0];
            const float         xprev_grid      = buf0.empty()
                                                    ? 0.0f
                                                    : std::round(buf0.back().x / dt_packet) * dt_packet;
            const float         xnext           = xprev_grid + dt_packet;

            bool                have_master_vis = false;
            Counter_t           y_min_vis       = Counter_t(0);
            Counter_t           y_max_vis       = Counter_t(0);
            cc::ChannelID       id_min_vis      = cc::ChannelID::None;
            cc::ChannelID       id_max_vis      = cc::ChannelID::None;


            //      ITERATE THRU EACH CHANNEL...
            for (size_t i = 0; i < ms_NUM; ++i)
            {
                const ChIndex   ch_idx      = static_cast<ChIndex>( ms_channels[i].idx );
                const float     current     = static_cast<float>( packet[ch_idx] );
                const float     avg         = this->ComputeAverage(
                      m_buffers[i]
                    , m_avg_mode
                    , m_avg_window_samp.Value()
                    , m_avg_window_sec.Value()
                    , PF.spark_now
                );

                m_buffers[i]    .push_back({ xnext, current });
                m_avg_counts[i] .push_back({ xnext, avg     });

                {
                    const double    curr_d     = static_cast<double>( packet[ch_idx] );
                    const double    prev_d     = static_cast<double>( m_max_counts[i] );
                    m_max_counts[i]            = static_cast<float>( std::max(prev_d, curr_d) );
                }

                if ( ms_channels[i].vis.master )
                {
                    const Counter_t     y_curr     = static_cast<Counter_t>( packet[ch_idx] );
                    const cc::ChannelID  ch_id      = static_cast<cc::ChannelID>( ms_channels[i].idx );

                    if (!have_master_vis)
                    {
                        have_master_vis          = true;
                        y_min_vis                = y_curr;
                        y_max_vis                = y_curr;
                        id_min_vis               = ch_id;
                        id_max_vis               = ch_id;
                    }
                    else
                    {
                        if (y_curr < y_min_vis)  { y_min_vis = y_curr;   id_min_vis = ch_id; }
                        if (y_curr > y_max_vis)  { y_max_vis = y_curr;   id_max_vis = ch_id; }
                    }
                }
            }

            if (have_master_vis)
            {
                PF.ymin                 = y_min_vis;
                PF.ymax                 = y_max_vis;
                PF.current_min          = id_min_vis;
                PF.current_max          = id_max_vis;
            }
            else
            {
                PF.ymin                 = Counter_t(0);
                PF.ymax                 = Counter_t(0);
                PF.current_min          = cc::ChannelID::None;
                PF.current_max          = cc::ChannelID::None;
            }

            continue;
        }

        switch (py_type)
        {
            case ipc::PythonPacketType::Status:
            {
                this->m_ipc_state.OnStatus(py_msg, wall_now);
                break;
            }
            case ipc::PythonPacketType::Error:
            {
                this->m_ipc_state.OnError(py_msg, wall_now);
                break;
            }
            default:
            {
                break;
            }
        }
    }


    //  Wall-clock "recent activity" (keep for UI; do NOT use to advance axis)
    //
    if ( PF.got_packet )
    {
        this->m_last_packet_time = wall_now;
    
        #ifdef __CBAPP_DEBUG__
        CB_LOG(
              LogLevel::Warning
            , "[[CCounter]] (Y-Min, Y-Max):\t({}, {})"
            , PF.ymin
            , PF.ymax
        );
        #endif  //  __CBAPP_DEBUG__  //
    }
    m_streaming_active = (wall_now - m_last_packet_time) < m_stream_timeout;



    // ------------------------------------------------------------------
    // 2) Master plot window (latched). Smooth crawl is:
    //    right_edge = last_x + clamp(elapsed_since_last_packet, 0, Δt)
    //    This prevents the center from ever outrunning the playhead by > Δt.
    // ------------------------------------------------------------------
    const float         last_x              = m_buffers[0].empty()  ? 0.0f  : m_buffers[0].back().x;
    const float         elapsed_wall        = std::max(0.0f, wall_now - m_last_packet_time);
    const float         offset_between      = running  &&  smooth  &&  !m_buffers[0].empty()
                                                ? std::min(elapsed_wall, dt_packet)
                                                : 0.0f;
    const float         right_edge          = last_x + offset_between;



    //      CASE 1 :    APPLICATION IS PAUSED...
    if (!m_counter_running) {
        PF.xmin = m_freeze_xmin;
        PF.xmax = m_freeze_xmax;
    }
    //
    //      CASE 2 :    MODE: SMOOTH UPDATE ("Plot Crawling")...
    else if (smooth)
    {
        //      2A.         Smooth crawl (decoupled from process timeout; clamped to ≤ Δt).
        if (running) {
            // Smooth crawl (decoupled from process timeout; clamped to ≤ Δt)
            PF.xmin       = right_edge - this->ms_CENTER * history_len;
            PF.xmax       = PF.xmin + history_len;
            m_freeze_xmin = PF.xmin;      // latch while crawling
            m_freeze_xmax = PF.xmax;
        }
        //      2B.         Smooth requested but not running     ---> hold
        else {
            PF.xmin = m_freeze_xmin;
            PF.xmax = m_freeze_xmax;
        }
    }
    //
    //      CASE 3 :    MODE: STEPPED PLOT UPDATE MODE (Non-"Plot Crawling)...
    else
    {
        if (PF.got_packet) {
            PF.xmin       = last_x - this->ms_CENTER * history_len;
            PF.xmax       = PF.xmin + history_len;
            m_freeze_xmin = PF.xmin;
            m_freeze_xmax = PF.xmax;
        }
        else {
            PF.xmin = m_freeze_xmin;
            PF.xmax = m_freeze_xmax;
        }
    }


    //  Safety init on first frames
    if (PF.xmax < PF.xmin) {
        PF.xmin         = 0.0f;
        PF.xmax         = history_len;
        m_freeze_xmin   = PF.xmin;
        m_freeze_xmax   = PF.xmax;
    }
    
    return;
}
/*
{
    namespace           cc                  = ccounter;
    PerFrame_t &        PF                  = this->m_perframe;
    const float         history_len         = this->m_history_length.Value();
    //
    PF.got_packet                           = false;
    PF.xmin                                 = 0.0f;
    PF.xmax                                 = history_len;


    // ------------------------------------------------------------------
    // 1) Poll Python; for each packet:
    //    - compute x_next on the Δt grid to avoid FP drift
    //    - stamp all channels at the same x_next
    // ------------------------------------------------------------------
    const float         wall_now            = static_cast<float>(ImGui::GetTime());
    const bool          running             = (m_process_running && m_counter_running);
    const bool          smooth              = (m_smooth_scroll != 0);
    const float         dt_packet           = m_integration_window.Value();

    ipc::PythonPacketType       py_type     = ipc::PythonPacketType::Unknown;
    std::string                 py_msg      {   };

    py_msg.reserve(IPCState::ms_MESSAGE_BUFFER_SIZE);

    while ( this->m_python.try_receive(PF.raw) )
    {
        ++this->m_num_packets;

        py_type = ipc::PythonPacketType::Unknown;
        py_msg.clear();

        if (auto packet_ptr = ipc::parse_python_message<Packet_t>(PF.raw, m_use_mutex_count, py_type, py_msg))
        {
            PF.got_packet = true;

            const Packet_t &    packet      = *packet_ptr;

            //  Use channel 0 as the timeline anchor (all channels share the same x)
            auto &          buf0            = m_buffers[0];
            const float     xprev_grid      = buf0.empty()
                                                ? 0.0f
                                                : std::round(buf0.back().x / dt_packet) * dt_packet;
            const float     xnext           = xprev_grid + dt_packet;   // fixed grid: eliminates long-run drift

            for (size_t i = 0; i < ms_NUM; ++i)
            {
                const ChIndex   ch_idx      = static_cast<ChIndex>( ms_channels[i].idx );
                const float     current     = static_cast<float>( packet[ch_idx] );
                const float     avg         = this->ComputeAverage(
                                                  m_buffers[i]
                                                , m_avg_mode
                                                , m_avg_window_samp.Value()
                                                , m_avg_window_sec.Value()
                                                , PF.spark_now
                                              );

                m_buffers[i]    .push_back({ xnext, current });
                m_avg_counts[i] .push_back({ xnext, avg     });
                m_max_counts[i] = std::max(m_max_counts[i], current);
            }

            continue;
        }

        switch (py_type)
        {
            case ipc::PythonPacketType::Status:
            {
                this->m_ipc_state.OnStatus(py_msg, wall_now);
                break;
            }
            case ipc::PythonPacketType::Error:
            {
                this->m_ipc_state.OnError(py_msg, wall_now);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    // Wall-clock "recent activity" (keep for UI; do NOT use to advance axis)
    if (PF.got_packet) {
        this->m_last_packet_time = wall_now;
    }
    m_streaming_active = (wall_now - m_last_packet_time) < m_stream_timeout;


    // ------------------------------------------------------------------
    // 2) Master plot window (latched). Smooth crawl is:
    //    right_edge = last_x + clamp(elapsed_since_last_packet, 0, Δt)
    //    This prevents the center from ever outrunning the playhead by > Δt.
    // ------------------------------------------------------------------
    const float         last_x              = m_buffers[0].empty()  ? 0.0f  : m_buffers[0].back().x;
    const float         elapsed_wall        = std::max(0.0f, wall_now - m_last_packet_time);
    const float         offset_between      = running && smooth && !m_buffers[0].empty()
                                                ? std::min(elapsed_wall, dt_packet)
                                                : 0.0f;
    const float         right_edge          = last_x + offset_between;



    //      CASE 1 :    APPLICATION IS PAUSED...
    if (!m_counter_running) {
        PF.xmin = m_freeze_xmin;
        PF.xmax = m_freeze_xmax;
    }
    //
    //      CASE 2 :    MODE: SMOOTH UPDATE ("Plot Crawling")...
    else if (smooth)
    {
        //      2A.         Smooth crawl (decoupled from process timeout; clamped to ≤ Δt).
        if (running) {
            // Smooth crawl (decoupled from process timeout; clamped to ≤ Δt)
            PF.xmin       = right_edge - this->ms_CENTER * history_len;
            PF.xmax       = PF.xmin + history_len;
            m_freeze_xmin = PF.xmin;      // latch while crawling
            m_freeze_xmax = PF.xmax;
        }
        //      2B.         Smooth requested but not running     ---> hold
        else {
            PF.xmin = m_freeze_xmin;
            PF.xmax = m_freeze_xmax;
        }
    }
    //
    //      CASE 3 :    MODE: STEPPED PLOT UPDATE MODE (Non-"Plot Crawling)...
    else
    {
        if (PF.got_packet) {
            PF.xmin       = last_x - this->ms_CENTER * history_len;
            PF.xmax       = PF.xmin + history_len;
            m_freeze_xmin = PF.xmin;
            m_freeze_xmax = PF.xmax;
        }
        else {
            PF.xmin = m_freeze_xmin;
            PF.xmax = m_freeze_xmax;
        }
    }


    //  Safety init on first frames
    if (PF.xmax < PF.xmin) {
        PF.xmin         = 0.0f;
        PF.xmax         = m_history_length.Value();
        m_freeze_xmin   = PF.xmin;
        m_freeze_xmax   = PF.xmax;
    }
    
    return;
}*/



/*
{
    namespace               cc                  = ccounter;
    PerFrame &              PF                  = this->m_perframe;
    //
    PF.got_packet                 = false;
    PF.xmin                       = 0.0f;
    PF.xmax                       = m_history_length.value;
    //  float                   xmin                = 0.0f,
    //                          xmax                = m_history_length.value;
                                    
          
    //      1.      POLL THE CHILD-PROCESS.  PUSH NEW DATA-POINTS...
    //
    while ( this->m_python.try_receive( PF.raw ) )
    {
        PF.got_packet = true;
        ++this->m_num_packets;
        
        //  if ( auto packet = cc::parse_packet(raw, m_use_mutex_count) )
        if ( auto packet_ptr = cc::parse_packet(PF.raw, m_use_mutex_count) )
        {
            //  const auto &    counts      = packet->counts;
            const Packet &  packet  = *packet_ptr;
            
            for (size_t i = 0ULL; i < ms_NUM; ++i)
            {
                //  const size_t        ch_idx          = ms_channels[i].idx;
                const ChIndex       ch_idx          = static_cast<ChIndex>( ms_channels[i].idx );
                const float         current         = static_cast<float>( packet[ch_idx] );
                const float         avg             = this->ComputeAverage(
                      m_buffers[i]
                    , m_avg_mode
                    , m_avg_window_samp.Value()
                    , m_avg_window_sec.Value()
                    , PF.spark_now    //    spark_now
                );
                    
                
                m_buffers[i]        .push_back({ PF.now, current });               //  1.  PUSH-BACK MOST RECENT FPGA DATA PACKET.
                m_max_counts[i]     = std::max(m_max_counts[i], current);       //  2.  COMPUTE CURRENT MAX VALUE FOR THIS COUNTER.
                m_avg_counts[i]     .push_back({ PF.now, avg });                   //  3.
            }
        }
    }
    //
    //  if (got_packet)     { this->m_last_packet_time = now; }
    if (PF.got_packet)        { this->m_last_packet_time = PF.now; }


    //      streaming considered active if we’ve received data within timeout
    //  m_streaming_active  = (now - m_last_packet_time) < m_stream_timeout;
    m_streaming_active  = (PF.now - m_last_packet_time) < m_stream_timeout;



    //------------------------------------------------------------------
    // 2.  Update cached freeze range when streaming or on first frame
    //------------------------------------------------------------------
    if (!m_counter_running) {                                   //  Recording stopped – freeze completely.
        //  xmin = m_freeze_xmin;
        //  xmax = m_freeze_xmax;
        PF.xmin = m_freeze_xmin;
        PF.xmax = m_freeze_xmax;
    }
    else if (this->m_smooth_scroll)
    {
        if ( this->m_process_running  &&  this->m_streaming_active ) {
            PF.xmin             = PF.now - this->ms_CENTER * this->m_history_length.Value();
            PF.xmax             = PF.xmin + this->m_history_length.Value();
            m_freeze_xmin       = PF.xmin;                 // keep cache fresh while crawling
            m_freeze_xmax       = PF.xmax;
        }
    }
    else { // Stepped mode
        if (PF.got_packet) {                                       //  Jump so the newest packet sits at right edge of window.
            //  xmin            = m_last_packet_time - ( this->ms_CENTER * m_history_length.value );
            //  xmax            = xmin + m_history_length.value;
            //  m_freeze_xmin   = xmin;
            //  m_freeze_xmax   = xmax;                                  //  Jump so the newest packet sits at right edge of window.
            //
            PF.xmin   = m_last_packet_time - ( this->ms_CENTER * m_history_length.value );
            PF.xmax   = PF.xmin + m_history_length.Value();
            m_freeze_xmin           = PF.xmin;
            m_freeze_xmax           = PF.xmax;
        }
        else {                              //  Hold still between packets.
            //  xmin            = m_freeze_xmin;
            //  xmax            = m_freeze_xmax;
            //
            PF.xmin            = m_freeze_xmin;
            PF.xmax            = m_freeze_xmax;
        }
    }

    //  Ensure an initial range on app startup
    //  if (xmax < xmin) {
    //      xmin            = 0.0f;
    //      xmax            = m_history_length.value;
    //      m_freeze_xmin   = xmin;
    //      m_freeze_xmax   = xmax;
    //  }
    if (PF.xmax < PF.xmin) {
        PF.xmin       = 0.0f;
        PF.xmax       = m_history_length.value;
        m_freeze_xmin               = PF.xmin;
        m_freeze_xmax               = PF.xmax;
    }

    return;
    //return {PF.xmin, PF.xmax};
}*/



//  "ComputeAverage"
//      Helper to compute localized average
//
inline float CCounterApp::ComputeAverage(const buffer_type &buf, AvgMode mode, ImU64 N_samples, double seconds, float now) const
{
    const std::size_t       size            = buf.size();
    float                   sum             = 0.0f;
    std::size_t             count           = 0;
    
    if (size == 0)          { return 0.0f; }


    
    switch (mode)
    {
        //      CASE 1 :    AVERAGE BASED ON N-NUMBER OF SAMPLES...
        case AvgMode::Samples :
        {
            const ImU64         N       = std::min<ImU64>(N_samples, size);
            const std::size_t   start   = size - static_cast<std::size_t>( N );
            
            for (std::size_t i = start; i < size; ++i)
            {
                sum += buf[i].y;
                ++count;
            }
            break;
        }
        //
        //      CASE 2 :    AVERAGE BASED ON T-NUMBER OF SECONDS OF DATA...
        case AvgMode::Seconds :
        {
            for ( std::size_t i = size; i-- > 0; )
            {
                const float     dt      = now - buf[i].x;
                if (dt > seconds)       { break; }
                sum += buf[i].y;
                ++count;
            }
            break;
        }
        //
        //      CASE 3 :    COMPUTE AVERAGE OVER **ALL** DATA INSIDE BUFFER...
        case AvgMode::All :
        {
            for (std::size_t i = 0; i < size; ++i)
            {
                sum += buf[i].y;
                ++count;
            }
            break;
        }
        //
        //
        default :
        {
            IM_ASSERT(false);
            break;
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

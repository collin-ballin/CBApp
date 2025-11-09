/***********************************************************************************
*
*       ********************************************************************
*       ****                  I N I T  ____  F I L E                   ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond.
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
//
//      1.      INITIALIZATION  | DEFAULT CONSTRUCTOR, DESTRUCTOR, ETC...
// *************************************************************************** //
// *************************************************************************** //

//  Default Constructor.
//
CCounterApp::CCounterApp(app::AppState & src) noexcept
    : S(src)
{
    //      1.      ASSIGN THE CHILD-WINDOW CLASS PROPERTIES...
    m_window_class[0].DockNodeFlagsOverrideSet                  = ImGuiDockNodeFlags_HiddenTabBar;      //  ImGuiDockNodeFlags_HiddenTabBar; //ImGuiDockNodeFlags_NoTabBar;
    m_window_class[1].DockNodeFlagsOverrideSet                  = ImGuiDockNodeFlags_HiddenTabBar;      //    ImGuiDockNodeFlags_NoTabBar; //ImGuiDockNodeFlags_HiddenTabBar; //ImGuiDockNodeFlags_NoTabBar;
    
    //  this->m_detview_window.render_fn    = std::bind_front(&CCounterApp::display_controls, this);
    return;
}


//  "initialize"
//
void CCounterApp::initialize(void)
{
    if (this->m_initialized)    { return; }
        
    this->init();
    return;
}


//  "init"
//
void CCounterApp::init(void)
{
    this->m_cmap        = ImPlot::GetColormapIndex("CCounter_Map");
    
    this->m_python.set_filepath( this->m_filepath );
    
    
    
    //      1.      ASSIGN THE CHILD-WINDOW CLASS PROPERTIES...
    m_window_class[0].DockNodeFlagsOverrideSet                      = ImGuiDockNodeFlags_HiddenTabBar;      //  ImGuiDockNodeFlags_HiddenTabBar; //ImGuiDockNodeFlags_NoTabBar;
    m_window_class[1].DockNodeFlagsOverrideSet                      = ImGuiDockNodeFlags_HiddenTabBar;      //ImGuiDockNodeFlags_HiddenTabBar; //ImGuiDockNodeFlags_NoTabBar;
    
        
        
    //      2.      DEFAULT TAB OPTIONS...
    //  static ImGuiTabItemFlags        ms_DEF_PLOT_TAB_FLAGS           = ImGuiTabItemFlags_None;
    //
    static ImGuiTabItemFlags        ms_DEF_INDIVIDUALS_TAB_FLAGS    = ImGuiTabItemFlags_None;
    static ImGuiTabItemFlags        ms_DEF_CTRL_TAB_FLAGS           = ImGuiTabItemFlags_None;
    static ImGuiTabItemFlags        ms_DEF_APPEARANCE_TAB_FLAGS     = ImGuiTabItemFlags_None;
    
    
    //      3A.     TABS FOR PLOT WINDOW...
#ifndef DEF_REFACTOR_CC
    ms_PLOT_TABS    = {
    //          TAB NAME.                   OPEN.           NOT CLOSE-ABLE.         FLAGS.                              CALLBACK.
        Tab_t(  "Home"                      , true          , true                  , ms_DEF_PLOT_TAB_FLAGS             , nullptr     )
    };
#endif  //  DEF_REFACTOR_CC  //
    

    //      4A.     TABS FOR CONTROL WINDOW...
    ms_CTRL_TABS    = {
    //            TAB NAME.                 OPEN.           NOT CLOSE-ABLE.         FLAGS.                              CALLBACK.
          Tab_t(  "Indivual Counters"       , true          , true                  , ms_DEF_INDIVIDUALS_TAB_FLAGS      , nullptr     )
        , Tab_t(  "Controls"                , true          , true                  , ms_DEF_CTRL_TAB_FLAGS             , nullptr     )
        , Tab_t(  "Appearance"              , true          , true                  , ms_DEF_APPEARANCE_TAB_FLAGS       , nullptr     )
    };
    
    
    
    //      3B.     ASSIGN THE CALLBACK RENDER FUNCTIONS FOR EACH PLOT TAB...
#ifndef DEF_REFACTOR_CC
    for (size_t i = 0; i < ms_PLOT_TABS.size(); ++i)
    {
        auto &      tab                     = ms_PLOT_TABS[i];
        ms_PLOT_TABS[i].render_fn           = [this, tab]([[maybe_unused]] const char * id, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
                                              { this->dispatch_plot_function( tab.uuid ); };
    }
#endif  //  DEF_REFACTOR_CC  //


    //      4B.     ASSIGN CALLBACKS TO EACH CTRL TAB...
    for (size_t i = 0ULL; i < ms_CTRL_TABS.size(); ++i)
    {
        auto &      tab                     = ms_CTRL_TABS[i];
        ms_CTRL_TABS[i].render_fn           = [this, tab]([[maybe_unused]] const char * id, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
                                              { this->dispatch_ctrl_function( tab.uuid ); };
    }
    
    
    //      Set the initial "freeze" limits so plot begins with sensible X-Limits.
    m_freeze_xmin   = 0.0f;
    m_freeze_xmax   = this->m_history_length.Value();
    
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



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "1.  INITIALIZATIONS".












// *************************************************************************** //
//
//
//
//      X.      UTILITY...
// *************************************************************************** //
// *************************************************************************** //

//  "init_ctrl_rows"
//
void CCounterApp::init_ctrl_rows(void)
{
    namespace                       cc                  = ccounter;
    namespace                       fs                  = std::filesystem;
    
    //  INTERACTIVE VARIABLES...
    static ImGuiSliderFlags         SLIDER_FLAGS        = ImGuiSliderFlags_AlwaysClamp;

    constexpr float                 margin              = 0.75f;
    constexpr float                 pad                 = 10.0f;



    std::snprintf( m_filebuffer, ms_MSG_BUFFER_SIZE, "%s", this->m_filepath.string().c_str() );
    
    
        
    ms_CTRL_ROWS            = {
    //
    //  1.  CONTROL PARAMETERS [TABLE]...
    /*
        {"Record",                              [this]
            {// BEGIN.
            //
            //  1.  PYTHON-SCRIPT FILEPATH FIELD...
                ImGui::SetNextItemWidth( margin * ImGui::GetColumnWidth() );
                cc::enter   = ImGui::InputText("##PyFilepath", m_filebuffer, ms_MSG_BUFFER_SIZE, cc::write_file_flags);
                //
                //
                //
                ImGui::SameLine(0.0f, pad);
                //
                if (cc::enter)
                {
                    this->m_filepath        = fs::path{ this->m_filebuffer };
                    const bool valid_file   = ( !this->m_filepath.empty() && fs::exists(this->m_filepath) && fs::is_regular_file(this->m_filepath) );
                    //
                    //
                    if ( valid_file ) {
                        m_python.set_filepath(m_filepath);
                    }
                    else
                    {
                        cc::enter = false;
                        //  std::snprintf(m_filepath, ms_MSG_BUFFER_SIZE, "%s", "INVALID FILEPATH");
                        
                        this->m_filepath        = fs::path{   };
                        this->S.m_logger.debug( std::format("CCounter | invalid filepath, \"{}\"", this->m_filepath.string()) );
                    }
                }
                //
                //
                //
                //  2.  START/STOP PYTHON SCRIPT BUTTON...
                //
                //      CASE 1 :    SCRIPT IS  **NOT**  RUNNING...
                    if (!m_process_running)
                    {
                        if (ImGui::Button("Start Process", ImVec2(ImGui::GetContentRegionAvail().x - pad, 0)) )
                        {
                            m_process_running         = m_python.start();
                            if (!m_process_running) {
                                ImGui::OpenPopup("launch_error");
                            }
                            else {
                                m_max_counts[0] = 0.0f; // reset stats
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
                            m_python.stop();
                            m_process_running = false;
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
                if (m_process_running)    {
                    char cmd[ms_MSG_BUFFER_SIZE];
                    std::snprintf(cmd, ms_MSG_BUFFER_SIZE, "integration_window %.3f\n", m_integration_window.value);
                    m_python.send(cmd);
                }
            //
            //
            //
            //
            } // END.
        },
        */
        {"Communicate",                         [this]
            {// BEGIN.
                ImGui::BeginDisabled();
                ImGui::BeginGroup();
                //
                //
                ImGui::SetNextItemWidth( margin * ImGui::GetColumnWidth() );
                if ( ImGui::InputText("##send", cc::line_buf, cc::ms_CMD_BUFFER_SIZE,  ImGuiInputTextFlags_EnterReturnsTrue) )   {
                    if (m_process_running)    {
                    char cmd[ms_MSG_BUFFER_SIZE];
                        std::snprintf(cmd, ms_MSG_BUFFER_SIZE, "integration_window %.3f\n", cc::delay_s);
                        m_python.send(cmd);
                    }
                }
                ImGui::SameLine(0.0f, pad);
                if ( ImGui::Button("Send", ImVec2(ImGui::GetContentRegionAvail().x - pad, 0)) || ImGui::IsItemDeactivatedAfterEdit() )
                {
                    if (m_process_running && cc::line_buf[0]) {
                        m_python.send(cc::line_buf);        // passthrough; must include \n if desired
                        cc::line_buf[0]     = '\0';
                    }
                }
                ImGui::Dummy( ImVec2(pad, 0.0f) );
                //
                //
                ImGui::EndGroup();//  TOOL-TIP FOR "COMMUNICATION...
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_NoSharedDelay))
                    ImGui::SetTooltip( "Manual communication to the child process.  DO NOT TOUCH THIS UNLESS YOU KNOW WHAT YOU'RE DOING." );
                //
                //
                ImGui::EndDisabled();
            }// END.
            //
        },
    //
        {"Coincidence Window",                  [this]
            {// BEGIN.
                ImGui::SetNextItemWidth( margin * ImGui::GetColumnWidth() );
                //if (ImGui::SliderInt("##CoincidenceWindow",       &m_coincidence_window,   8,    100,   "%03d clicks",    SLIDER_FLAGS) )
                
                if ( ImGui::SliderScalar("##CoincidenceWindow",    ImGuiDataType_U64,      &m_coincidence_window.value,
                                        &m_coincidence_window.limits.min, &m_coincidence_window.limits.max,  "%llu ticks", SLIDER_FLAGS) )
                {
                    if (m_process_running)    {
                        char cmd[ms_MSG_BUFFER_SIZE];
                        std::snprintf(cmd, ms_MSG_BUFFER_SIZE, "coincidence_window %llu\n", m_coincidence_window.value);
                        m_python.send(cmd);
                    }
                }
            }// END.
        },
    //
        {"Integration Window",                  [this]
            {// BEGIN.
                ImGui::SetNextItemWidth( margin * ImGui::GetColumnWidth() );
                if ( ImGui::SliderScalar("##IntegrationWindow",          ImGuiDataType_Double,       &m_integration_window.value,
                                    &m_integration_window.limits.min,     &m_integration_window.limits.max,     "%.3f seconds", SLIDER_FLAGS) ) {
                    if (m_process_running) {
                        char cmd[ms_MSG_BUFFER_SIZE];
                        std::snprintf(cmd, ms_MSG_BUFFER_SIZE, "integration_window %.3f\n", m_integration_window.value);
                        m_python.send(cmd);
                    }
                }
                ImGui::SameLine(0.0f, pad);
                if (ImGui::Button("Apply", ImVec2(ImGui::GetContentRegionAvail().x - pad, 0)) ) {
                    char cmd[ms_MSG_BUFFER_SIZE];
                    std::snprintf(cmd, ms_MSG_BUFFER_SIZE, "integration_window %.3f\n", cc::delay_s);
                    m_python.send(cmd);
                }
                ImGui::Dummy( ImVec2(pad, 0.0f) );
            }// END.
        },
    //
        {"History Length",                      [this]
            {// BEGIN.
                ImGui::SetNextItemWidth( margin * ImGui::GetColumnWidth() );
                ImGui::SliderScalar("##HistoryLength",                  ImGuiDataType_Double,           &m_history_length.value,
                                    &m_history_length.limits.min,       &m_history_length.limits.max,   "%.1f seconds", SLIDER_FLAGS);
                ImGui::SameLine(0.0f, pad);
                
                if ( ImGui::Button("Clear Plot", ImVec2(ImGui::GetContentRegionAvail().x - pad, 0)) ) {
                    for (auto & b : m_buffers) {
                        b.clear(); //b.Erase();
                    }
                    std::fill(std::begin(m_max_counts), std::end(m_max_counts), 0.f);
                }
                ImGui::Dummy( ImVec2(pad, 0.0f) );
            }// END.
        },
    //
        {"Average",                             [this]
            {// BEGIN.
            //
                const char *        avg_items []        = { "Last N samples", "Last T seconds" };
                int                 mode_idx            = (m_avg_mode == AvgMode::Samples ? 0 : 1);
            //
            //
                ImGui::SetNextItemWidth( margin * ImGui::GetColumnWidth() );
                if (m_avg_mode == AvgMode::Samples)
                {
                    ImGui::SliderScalar("##AverageDuration_Samples",        ImGuiDataType_U64,                  &m_avg_window_samp.value,
                                        &m_avg_window_samp.limits.min,      &m_avg_window_samp.limits.max,      "%llu samples", SLIDER_FLAGS);
                }
                else {
                    ImGui::SliderScalar("##AverageDuration_Time",           ImGuiDataType_Double,               &m_avg_window_sec.value,
                                        &m_avg_window_sec.limits.min,       &m_avg_window_sec.limits.max,      "%.2f seconds", SLIDER_FLAGS);
                }
                //
                //
                //
                ImGui::SameLine(0.0f, pad);
                ImGui::SetNextItemWidth( ImGui::GetContentRegionAvail().x - pad );
                if ( ImGui::Combo("##AverageModeSelector",   &mode_idx,      avg_items,      IM_ARRAYSIZE(avg_items)) )
                {
                    m_avg_mode = (mode_idx == 0 ? AvgMode::Samples : AvgMode::Seconds);
                }
                ImGui::Dummy( ImVec2(pad, 0.0f) );
                            
   
            }// END.
        },
    //
    //
        {"Misc.",                          [this]
            {// BEGIN.
            
                ImGui::Checkbox("Use Mutex Counts", &m_use_mutex_count);
                ImGui::SameLine();
                
                if ( ImGui::Button("Reset Averages") ) {
                    for (auto & vec : m_avg_counts)
                        vec.clear(); //b.Erase();
                }
                
                ImGui::SameLine();
                
                if ( ImGui::Button("Reset Max") )
                    std::fill(std::begin(m_max_counts), std::end(m_max_counts), 0.f);
                
                ImGui::SameLine();
                ImGui::Checkbox("Plot Crawling", &m_smooth_scroll);
                
                
            }// END.
        }
    };

    
    
    

    ms_APPEARANCE_ROWS  = {
    //
        {"Average Appearance",                  [this]
            {// BEGIN.
                //
                //      Alpha.
                const float     width   = 0.33 * ImGui::GetColumnWidth();
                bool            dirty1  = false;
                bool            dirty2  = false;
                
                ImGui::SetNextItemWidth( width );
                dirty1  = ImGui::SliderScalar( "##AvgOpacity",                     ImGuiDataType_Float,            &m_avg_opacity.value,
                                               &m_avg_opacity.limits.min,          &m_avg_opacity.limits.max,      "Opacity: %.2f %%", SLIDER_FLAGS );
                ImGui::SameLine(0.0f, pad);
                
                
                //      Tint.
                ImGui::SetNextItemWidth( width );
                dirty2 = ImGui::SliderScalar( "##AvgTint",                        ImGuiDataType_Float,            &m_avg_color_shade.value,
                                              &m_avg_color_shade.limits.min,      &m_avg_color_shade.limits.max,  "Tint/Shade: %.2f%%", SLIDER_FLAGS );
                ImGui::SameLine(0.0f, pad);
                
                
                //      LineWidth.
                ImGui::SetNextItemWidth( -1 );
                ImGui::SliderScalar("##AvgLineWidth",                   ImGuiDataType_Float,            &m_avg_linewidth.value,
                                    &m_avg_linewidth.limits.min,        &m_avg_linewidth.limits.max,    "Linewidth: %.2f px", SLIDER_FLAGS);
                
                
                ImGui::Dummy( ImVec2(pad, 0.0f) );
                //
                //
                this->m_colormap_cache_invalid  = ( dirty1  ||  dirty2 );
                
            }// END.
        },
    //
        {"Individual Plot Height",              []
            {
                ImGui::SetNextItemWidth( ImGui::GetColumnWidth() );
                ImGui::SliderFloat("##SinglePlotHeight",    &cc::row_height_px,         30.f,   240,  "%.0f px",  SLIDER_FLAGS);
            }
        },
    //
        {"Colormap",                            [this]
            {
                float   w   = ImGui::GetColumnWidth();
                
                
                
                
                if (ImPlot::ColormapButton( ImPlot::GetColormapName(m_cmap), ImVec2(w, 0), m_cmap) )
                {
                    //  m_colormap_cache_invalid    = true;
                    //  m_cmap                      = (m_cmap + 1) % ImPlot::GetColormapCount();
                    //ImPlot::BustColorCache(cc::heatmap_uuid);
                }
                //
                //          1.1.    OPEN COLOR-MAP MENU...
                const bool  open_cmap_menu  = ImGui::BeginPopupContextItem(
                      this->ms_CMAP_SELECTION_MENU /* GetMenuID( PopupHandle::ToolSelection )*/
                    , ImGuiPopupFlags_MouseButtonLeft
                );
                if (open_cmap_menu)
                {
                    if ( !this->_MENU_cmap_selection(this->m_cmap) )
                        { ImGui::CloseCurrentPopup(); }
                    //
                    ImGui::EndPopup();
                }
                
                
            }
        }
    };


    //      INITIAL ASSIGNMENT OF CMAP...
    this->_validate_colormap_cache();

    return;
}



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "X.  UTILITY".
















// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.















// *************************************************************************** //
// *************************************************************************** //   END.

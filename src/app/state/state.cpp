/***********************************************************************************
*
*       ********************************************************************
*       ****                S T A T E . h  ____  F I L E                ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond
*               DATED:      May 10, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/app.h"
#include "app/state/_state.h"



namespace cb { namespace app { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//  1.1     STRUCT INITIALIZATION FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
//  Default Constructor.
//
AppState::AppState(void)
    : m_logger{Logger::instance()}
{
    size_t      i       = size_t(0);

    //  1.      INITIALIZE WINDOW INFOS...
    for (i = 0; i < static_cast<size_t>(Window_t::Count); ++i) {
        m_windows.data[i] = APPLICATION_WINDOW_INFOS[i];
    }

    //  2.      INITIALIZE APPLICATION FONTS...
    for (i = 0; i < static_cast<size_t>(Font::Count); ++i) {
        m_fonts.data[i] = nullptr;  // load later in your init()
    }
       
       
    //  3A.     INITIALIZE APPLICATION'S APPLETS...
    this->m_applets     = {
        std::addressof( m_windows[ Window::MainApp      ].uuid      ),
        std::addressof( m_windows[ Window::CCounterApp  ].uuid      ),
        std::addressof( m_windows[ Window::EditorApp    ].uuid      ),
        std::addressof( m_windows[ Window::GraphApp     ].uuid      )
    };
    //
    //  3B.     END OF FIRST-FRAME INITIALIZATIONS (SET THE INITIAL WINDOW FOCUS)...
    #if defined(__CBAPP_BUILD_CCOUNTER_APP__)
        m_current_task      = Applet::CCounterApp;
    # elif defined(__CBAPP_BUILD_EDITOR_APP__)
        m_current_task      = Applet::EditorApp; 
    # elif defined(__CBAPP_BUILD_FDTD_APP__)
        m_current_task      = Applet::GraphApp;
    # else
        m_current_task      = Applet::MainApp; 
    #endif  //  __CBAPP_BUILD_CCOUNTER_APP__  //
    
    //m_current_task


}


//  Default Destructor.
//
AppState::~AppState(void) = default;












// *************************************************************************** //
//
//
//      1.3B    APPLICATION OPERATION HELPER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "DockAtHome"
//
void AppState::DockAtHome(const Window & idx) {
    app::WinInfo &w = this->m_windows[static_cast<Window>(idx)];
    if (w.open)     ImGui::DockBuilderDockWindow(w.uuid.c_str(), this->m_main_dock_id);
    return;
}
//
void AppState::DockAtHome(const char * uuid)                    { ImGui::DockBuilderDockWindow( uuid, m_main_dock_id ); }


//  "DockAtDetView"
//
void AppState::DockAtDetView(const Window & idx) {
    app::WinInfo &  w = this->m_windows[static_cast<Window>(idx)];
    if (w.open)     ImGui::DockBuilderDockWindow(w.uuid.c_str(), this->m_detview_dockspace_id);
    return;
}
//
void AppState::DockAtDetView(const char * uuid)                 { ImGui::DockBuilderDockWindow( uuid, m_detview_dockspace_id ); }
    
    
    
    
//  "PushFont"
//
void AppState::PushFont( [[maybe_unused]] const Font & which)   { ImGui::PushFont( this->m_fonts[which] ); return; }


//  "PopFont"
//
void AppState::PopFont(void)                                    { ImGui::PopFont(); return; }



// *************************************************************************** //
//
//
//      1.3C    MISC. HELPER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "log_startup_info"
//
void AppState::log_startup_info(void) noexcept
{
    IM_ASSERT( m_notes.size() == 1 && "\"S.m_notes\" should have 1 timestamp at this point");

    Timestamp_t         start_time              = cblib::utl::get_timestamp();
    auto                dt                      = cblib::utl::format_elapsed_timestamp(start_time - m_notes[0].first);
    auto                startup_info_log        = std::format("PROGRAM BOOT INFO...\n"
        "Spawn                      : {}\n"
        "Initialized                : {}\n"
        "Load Time                  : {}\n"
        "Total Persistent Windows   : {}\n"
        "Total Application Fonts    : {}",
        m_notes[0].first,
        start_time,
        dt,
        this->m_windows.data.size(),
        this->m_fonts.data.size()
    );
    this->m_notes.push_back( std::make_pair(start_time, "Program started ({})") );
    auto                startup_debug_log       = std::format("PROGRAM BOOT DEBUG-INFO...\n"
        "Start Task                 : {}\n"
        "Start App Color Style      : {}\n"
        "Start Plot Color Style     : {}",
        this->current_task(),
        current_app_color_style(),
        current_plot_color_style()
    );
    
    this->m_logger.notify( "PROGRAM BOOTED SUCCESSFULLY" );
    CB_LOG( LogLevel::Info,     startup_info_log    );
    CB_LOG( LogLevel::Debug,    startup_debug_log   );
    

    return;
}


//  "log_shutdown_info"
//
void AppState::log_shutdown_info(void) noexcept
{
    Timestamp_t         end_time        = cblib::utl::get_timestamp();
    auto                dt              = cblib::utl::format_elapsed_timestamp(end_time - m_notes[0].first);
    auto                ending_log      = std::format("PROGRAM TERMINATION INFO...\n"
        "Total Runtime Time     : {}\n",
        "Exit Code              : [NOT IMPLEMENTED]\n",
        dt
    );
    
    this->m_logger.notify( "PROGRAM TERMINATING" );
    CB_LOG( LogLevel::Info, ending_log );

    return;
}






// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" :: "app" NAMESPACE.






// *************************************************************************** //
// *************************************************************************** //
//
//  END.

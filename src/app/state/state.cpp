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
#include "app/state/state.h"
//
#include <filesystem>
#include <system_error>
#include <chrono>
#include <format>



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
    size_t          i                   = size_t(0);



    //  1.      INITIALIZE WINDOW INFOS...
    for (i = 0; i < static_cast<size_t>(Window_t::Count); ++i) {
        m_windows.data[i] = APPLICATION_WINDOW_INFOS[i];
    }
    
    //  2.      INITIALIZE APPLICATION FONTS...
    for (i = 0; i < static_cast<size_t>(Font::Count); ++i) {
        m_fonts.data[i] = nullptr;  // load later in your init()
    }






    //  ?.      INITIALIZE SUB-STATE OBJECTS...
    // *************************************************************************** //
    // *************************************************************************** //
    //          ?.1.    TaskState INITIALIZATION...
    //
    //                  END OF FIRST-FRAME INITIALIZATIONS (SET THE INITIAL WINDOW FOCUS).
    #if defined(__CBAPP_BUILD_CCOUNTER_APP__)
        m_task_state.m_current_task     = Applet::CCounterApp;
    # elif defined(__CBAPP_BUILD_EDITOR_APP__)
        m_task_state.m_current_task     = Applet::EditorApp;
    # elif defined(__CBAPP_BUILD_FDTD_APP__)
        m_task_state.m_current_task     = Applet::GraphApp;
    # else
        m_task_state.m_current_task     = Applet::MainApp;
    #endif  //  __CBAPP_BUILD_CCOUNTER_APP__  //
    //
    m_task_state.m_applets   = {
        std::addressof( m_task_state.ms_IDLE_APPLET_NAME            ),      //  THESE ARE FOR:  (1) App is IDLE, (2) No NAMED-Window is open.
        std::addressof( m_task_state.ms_UNDEFINED_APPLET_NAME       ),      //  I had to store these seperate from the "m_windows[ Window::Name ]" because it makes no sense to
    //                                                                          define an additional member in the "m_windows" array to hold data for windows that are not stored.
        std::addressof( m_windows[ Window::MainApp      ].uuid      ),
        std::addressof( m_windows[ Window::CCounterApp  ].uuid      ),
        std::addressof( m_windows[ Window::EditorApp    ].uuid      ),
        std::addressof( m_windows[ Window::GraphApp     ].uuid      )
    };
    

    return;
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



// *************************************************************************** //
//      SERIALIZATION...
// *************************************************************************** //

//  "SaveWithValidation"
//
template<typename Callback>
bool AppState::SaveWithValidation(const std::filesystem::path & path, Callback & callback, const std::string_view & caller_tag, const std::string_view & failure_msg)
{
    namespace                                   fs              = std::filesystem;
    using                                       time_type       = fs::file_time_type;   //    std::chrono::time_point<std::filesystem::_FilesystemClock>;
    using                                       size_type       = std::uintmax_t;
    
    std::error_code                             ec              {  };
    //
    std::pair<bool,bool>                        exists          = {false, false};
    std::pair<time_type,time_type>              timestamp       = {  };
    std::pair<size_type,size_type>              filesize        = {  };
    //
    std::string                                 log             = {  };
    bool                                        success         = true;



    //      0.      MAKE A SNAPSHOT OF THE INITIAL STATE OF THE FILE...
    {
        std::error_code         EC  { };
        exists.first                = fs::exists(path, EC);
        timestamp.first             = ( exists.first ) ? fs::last_write_time(path,  EC)     : time_type::min();
        filesize.first              = ( exists.first ) ? fs::file_size(path,        EC)     : -1;
        //
        if (EC) {
            m_logger.critical( std::format( "{} | UNKNOWN ERROR: ERROR_CODE FAILURE INSIDE \"SaveWithValidation()\" FOR PATH \"{}\"", caller_tag, path.generic_string() ) );
        }
    }



    //      1.      MAIN-LOOP OF PERFORMING EACH CHECK...
    do {
    //
    //
    //              1.1.    ENSURE THE PARENT DIR. EXISTS.
        if ( auto parent = path.parent_path(); !parent.empty() && !fs::exists(parent, ec) )
        {
            if ( !fs::create_directories(parent, ec) || ec ) {
                log                 = std::format( "{} | failed to create parent dirs. while attempting to save file \"{}\"; error_code: {}", caller_tag, parent.generic_string(), ec.message() );
                success             = false;    break;
            }
        }
        //
        //          1.2.    INVOKE THE USER-SUPPLIED CALLBACK FUNCTION.
        if ( !callback(path) ) {
            log                 = std::format( "{} | validation failure: {} \"{}\"", caller_tag, failure_msg, path.filename().string() );
            success             = false;    break;
        }
        exists.second       = fs::exists(path, ec);
        filesize.second     = fs::file_size(path, ec);
        timestamp.second    = fs::last_write_time(path, ec);
        
        
        
        //          2.1.    [ EXISTS ]          POST-SAVE VALIDATION.
        if ( !exists.second ) {
            log                 = std::format( "{} | validation failure: file not found after write: \"{}\"", caller_tag, path.generic_string() );
            success             = false;    break;
        }
        //
        //          2.2.    [ FILE SIZE ]       POST-SAVE VALIDATION.
        if ( filesize.second == 0 ) {
            log                 = std::format( "{} | validation failure: file \"{}\" was written with a size of zero-bytes", caller_tag, path.generic_string() );
            success             = false;    break;
        }
        //
        //          2.3.    [ TIMESTAMP ]       POST-SAVE VALIDATION.
        if ( exists.first ) {
            using   namespace   std::chrono_literals;
            
            if ( timestamp.second <= timestamp.first + 1s ) {
                log                 = std::format( "{} | verification: timestamp of written file unchanged for \"{}\"", caller_tag, path.filename().string() );
                success             = false;    break;
            }
        }
    //
    //
    } while (false);
    
    
    //  CASE 1 :    FAILURE...
    if (!success)       { m_logger.error( log ); }
    //
    //  CASE 2 :    SUCCESS...
    else
    {
        //  1.  CREATED THE FILE.
        if ( exists.first ) {
            log = std::format( "{} | created file \"{}\" ({}) ", caller_tag, path.filename().string(), cblib::utl::fmt_file_size(filesize.second) );
        }
        //
        //  2.  OVERWROTE THE FILE.
        else {
            log = std::format( "{} | overwrote file \"{}\" ({}) ", caller_tag, path.filename().string(), cblib::utl::fmt_file_size(filesize.second) );
        }
    
    }



    m_logger.info( log );
    
    return success;            // all checks passed
}
                                                                      







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
        this->current_task_name(),
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

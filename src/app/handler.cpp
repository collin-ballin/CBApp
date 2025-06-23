/**
 * @file    handler.cpp
 * @author  Collin A. Bond
 * @date    2025-06-16
 * @brief   Outermost API/Interface for CBApp and definition of signal handlers.
 *
 *
 * @details     Contains the local "main" function, "run_application()", which is invoked by "int main()"
 *              to transfer responsibility over to our application.  This file also defines the functions which
 *              are set to handle various signals that may be sent to the application during runtime.  This includes
 *
 * @note    [Any additional notes]
 * @warning [Any important warnings]
 *
 *
 * @todo    Flush-out signal handlers to provide graceful application exits across a variety of unexpected termination events.
 *
 *
 *
 * @since   [Version when introduced]
 * @see     [Optional: related symbol]
 * @deprecated [Use NewFunction() instead]
 */
/***********************************************************************************
*
*       *********************************************************************
*       ****            H A N D L E R . C P P  ____  F I L E             ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      June 07, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/app.h"
#include CBAPP_USER_CONFIG

#include <iostream>   // std::cout
#include <csignal>    // std::signal, SIGINT, SIGTERM, SIGHUP, SIGABRT, SIGPIPE
#include <new>        // std::set_new_handler, std::bad_alloc
#include <cstdlib>    // std::exit

#ifdef __CBAPP_DEBUG__              //  <======|  Fix for issue wherein multiple instances of application
# include <thread>                  //            are launched when DEBUG build is run inside Xcode IDE...
# include <chrono>
#endif     //  __CBAPP_DEBUG__  //



//  0.      STATIC FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//
//  ...
//






// *************************************************************************** //
//
//
//  1.      UTILITY FUNCTIONS [NON-MEMBER FUNCTIONS]...
// *************************************************************************** //
// *************************************************************************** //

//  "run_application"
//  Client-code interface to creating and running the application...
//
int cb::run_application([[maybe_unused]] int argc, [[maybe_unused]] char ** argv)
{
    constexpr const char *  xcp_header              = "MAIN | ";
    constexpr const char *  xcp_type_runtime        = "Caught std::runtime_error exception";
    constexpr const char *  xcp_type_unknown        = "Caught std::runtime_error exception";
    constexpr const char *  xcp_at_start            = "while initializing the application.\n";
    constexpr const char *  xcp_at_runtime          = "during program runtime.\n";
    int                     status                  = EXIT_SUCCESS;
    
#ifdef __CBAPP_DEBUG__    //  WORK-AROUND / PATCH FOR XCODE ISSUE...
    std::this_thread::sleep_for(std::chrono::seconds(1));
#endif

    try {   //  1.     CREATE APPLICATION INSTANCE...
        App &   my_app      = App::instance();
        
        try {   //  2.     ENTER APPLICATION MAIN-LOOP...
            my_app.run();
        }
        catch (const std::runtime_error & e) {      //  2.1     CATCH RUNTIME ERROR (PROGRAM WAS INITIALIZED CORRECTLY)...
            std::cerr << xcp_header << xcp_type_runtime << xcp_at_runtime << "Traceback: " << e.what() << "\n";
            status = EXIT_FAILURE;
        }
        catch (...) {                               //  2.2     CATCH OTHER EXCEPTIONS AND EXIT...
            std::cerr << xcp_header << xcp_type_unknown << xcp_at_runtime;
            status = EXIT_FAILURE;
        }
    }
    catch (const std::runtime_error & e) {      //  1.1     CATCH INITIALIZATION-TIME ERROR (DURING "DEAR IMGUI" INITIALIZATION)...
        std::cerr << xcp_header << xcp_type_runtime << xcp_at_start << "Traceback: " << e.what() << "\n";
        status = EXIT_FAILURE;
    }
    catch (...) {                               //  1.2.    CATCH OTHER EXCEPTIONS AND EXIT...
        std::cerr << xcp_header << xcp_type_unknown << xcp_at_start;
        status = EXIT_FAILURE;
    }
    
    return status;
}




// *************************************************************************** //
//
//
//  2.      APPLICATION SIGNAL HANDLERS...
// *************************************************************************** //
// *************************************************************************** //

namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //

static void        on_shutdown              ([[maybe_unused]] int sig);
static void        on_reload                ([[maybe_unused]] int sig);
static void        on_new_handler           ([[maybe_unused]] int sig);



//  "install_signal_handlers"
//
void App::install_signal_handlers(void) {
    // new‐failure handler
    //  std::set_new_handler(on_memory_exhausted);


//  CASE 1 :    POSIX SIGNALS...
// *************************************************************************** //
#ifndef _WIN32

    std::signal(        SIGINT,         on_shutdown     );      //  Ctrl+C
    std::signal(        SIGTERM,        on_shutdown     );      //  Kill
    std::signal(        SIGQUIT,        on_shutdown     );      //  Quit
    std::signal(        SIGHUP,         on_reload       );      //  Reload config.
    std::signal(        SIGABRT,        on_shutdown     );      //  abort()
    std::signal(        SIGPIPE,        SIG_IGN         );      //  Ignore broken‐pipe errors
    //  std::signal(    SIGINT,         on_shutdown     );
    //  std::signal(    SIGTERM,        on_shutdown     );
    //  std::signal(    SIGQUIT,        on_shutdown     );
    //  std::signal(    SIGHUP,         on_reload       );
    //  std::signal(    SIGPIPE,        SIG_IGN         );
    
    // …add SIGUSR1/SIGUSR2 as custom hooks if desired…
  
// *************************************************************************** //
// *************************************************************************** //
//
//
//
//  CASE 2 :    WINDOWS SIGNALS...
// *************************************************************************** //
# else
    
    //
    //  ...
    //
    
    
// *************************************************************************** //
// *************************************************************************** //
#endif  //  _WIN32  //
    
    
//       std::set_new_handler(on_new_handler);
//
//   #ifndef _WIN32
//       std::signal(SIGINT,  on_shutdown);
//       std::signal(SIGTERM, on_shutdown);
//       std::signal(SIGQUIT, on_shutdown);
//       std::signal(SIGHUP,  on_reload);
//       std::signal(SIGPIPE, SIG_IGN);
//   #else
//       SetConsoleCtrlHandler(console_ctrl_handler, TRUE);
//   #endif
    
    
    
    
    
}



// *************************************************************************** //
//
//
//  HANDLER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "on_shutdown"
//
void on_shutdown([[maybe_unused]] int sig) {
    return;
}


//  "on_reload"
//
void on_reload([[maybe_unused]] int sig) {
    return;
}


//  "on_new_handler"
//
void on_new_handler([[maybe_unused]] int sig) {
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

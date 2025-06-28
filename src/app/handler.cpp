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
#include <cstdlib>    // std::exit
#include <csignal>    // std::signal, SIGINT, SIGTERM, SIGHUP, SIGABRT, SIGPIPE
#include <new>        // std::set_new_handler, std::bad_alloc

#ifdef __CBAPP_DEBUG__              //  <======|  Fix for issue wherein multiple instances of application
# include <thread>                  //            are launched when DEBUG build is run inside Xcode IDE...
# include <chrono>
#endif     //  __CBAPP_DEBUG__  //

#ifdef _WIN32
# include "windows.h"
#endif  //  _WIN32  //



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

/// @def        _CBAPP_MAIN_XCP_HEADER(HEADER, TYPE, DESC, TRACEBACK)
/// @brief      Helper for consistent error messages with location info for usage in main function only.
/// @note       USAGE:          XCP_HEADER(type, phase, details)
///             EXPANDS TO:     "<HEADER><TYPE><DESC>[func: ; in file: , line ] Traceback: <TRACEBACK>"
///
#define _CBAPP_MAIN_XCP_HEADER(HEADER, TYPE, DESC, TRACEBACK)                                                                       \
    do {                                                                                                                            \
        std::cerr << HEADER << ":\n"                                                                                                \
                  << "\tcaught "        << TYPE         << " exception "    << DESC        << ".\n"                                 \
                  << "\t[ func: "       << __func__     << "; in file: "    << __FILE__    << ", line " << __LINE__ << " ]\n"       \
                  << "traceback:\n\t"   << TRACEBACK    << '\n';                                                                    \
    } while (false);
    
    
//  "run_application"
//      Client-code interface to creating and running the application...
//
int cb::run_application([[maybe_unused]] int argc, [[maybe_unused]] char ** argv)
{
    constexpr const char *  XCP_HEADER              = "fatal error";
    constexpr const char *  XCP_TYPE_RUNTIME        = "std::runtime_error";
    constexpr const char *  XCP_TYPE_UNKNOWN        = "unknown-type";
    constexpr const char *  XCP_AT_START            = "while initializing the application";
    constexpr const char *  XCP_AT_RUNTIME          = "during program runtime";
    constexpr const char *  XCP_UNKNOWN_TRACEBACK   = "<traceback unavailable>";
    int                     status                  = EXIT_SUCCESS;
    
#ifdef __CBAPP_DEBUG__    //  WORK-AROUND/PATCH FOR ISSUE WHEN RUNNING THE PROGRAM INSIDE XCODE...
    std::this_thread::sleep_for( std::chrono::seconds(1) );
#endif

    //  1.     CREATE APPLICATION INSTANCE...
    try {
        App &   my_app      = App::instance();
        
        
        
        //  2.     ENTER APPLICATION MAIN-LOOP...
        try {
            my_app.run();
        }
        //
        //  2.1     CATCH RUNTIME ERROR (PROGRAM WAS INITIALIZED CORRECTLY)...
        catch (const std::runtime_error & e) {
            _CBAPP_MAIN_XCP_HEADER(XCP_HEADER, XCP_TYPE_RUNTIME, XCP_AT_RUNTIME, e.what())
            status = EXIT_FAILURE;
        }
        //
        //  2.2     CATCH OTHER EXCEPTIONS AND EXIT...
        catch (...) {
            _CBAPP_MAIN_XCP_HEADER(XCP_HEADER, XCP_TYPE_UNKNOWN, XCP_AT_RUNTIME, XCP_UNKNOWN_TRACEBACK)
            status = EXIT_FAILURE;
        }
    }
    //
    //
    //
    //  1.1     CATCH INITIALIZATION-TIME ERROR (DURING "DEAR IMGUI" INITIALIZATION)...
    catch (const std::runtime_error & e) {
        _CBAPP_MAIN_XCP_HEADER(XCP_HEADER, XCP_TYPE_RUNTIME, XCP_AT_START, e.what())
        status = EXIT_FAILURE;
    }
    //
    //  1.2.    CATCH OTHER EXCEPTIONS AND EXIT...
    catch (...) {
        _CBAPP_MAIN_XCP_HEADER(XCP_HEADER, XCP_TYPE_UNKNOWN, XCP_AT_START, XCP_UNKNOWN_TRACEBACK)
        status = EXIT_FAILURE;
    }
    
    
    
    return status;
}
#undef _CBAPP_MAIN_XCP_HEADER



// *************************************************************************** //
//
//
//  2.      APPLICATION SIGNAL HANDLERS...
// *************************************************************************** //
// *************************************************************************** //

namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //

static void         on_new_handler          ( void );
static void         on_shutdown             ( [[maybe_unused]] int sig );
static void         on_reload               ( [[maybe_unused]] int sig );
#ifdef _WIN32
    BOOL WINAPI     console_ctrl_handler    ( DWORD type );
#endif  //  _WIN32  //



//  "install_signal_handlers"
//
void App::install_signal_handlers(void)
{

    std::set_new_handler(                       on_new_handler          );      //  Handle for "New" Failure...


//  CASE 1 :    POSIX SIGNALS...
// *************************************************************************** //
#ifndef _WIN32

    std::signal(                SIGINT,                 on_shutdown             );      //  Ctrl+C
    std::signal(                SIGTERM,                on_shutdown             );      //  Kill
    std::signal(                SIGQUIT,                on_shutdown             );      //  Quit
    std::signal(                SIGABRT,                on_shutdown             );      //  abort()
    std::signal(                SIGHUP,                 on_reload               );      //  Reload config.
    std::signal(                SIGPIPE,                SIG_IGN                 );      //  Ignore broken‐pipe errors
    
    
    
    //  …add SIGUSR1/SIGUSR2 as custom hooks if desired…
  
// *************************************************************************** //
// *************************************************************************** //
//
//
//
//  CASE 2 :    WINDOWS SIGNALS...
// *************************************************************************** //
# else
    
    SetConsoleCtrlHandler(      console_ctrl_handler,   TRUE                    );
    
// *************************************************************************** //
// *************************************************************************** //
#endif  //  _WIN32  //
    
    
    return;
}



// *************************************************************************** //
//
//
//  HANDLER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//{ App::instance().enqueue_signal(CBSignalFlags_Shutdown); }


//  "on_new_handler"
//
void on_new_handler(void)
{
    //  1.  Report the signal handler Tell the user / CI what happened (async-signal-safe: fputs)
    std::fputs( "invoked signal handler <on_new_handler()>:\n\t"
                "fatal error has occured during call to [operator new].\n\t"
                "requesting application shutdown.\n", stderr );

    //  2.  Request application to terminate at the next frame boundary.
    App::instance().enqueue_signal( app::CBSignalFlags_NewFailure );

    //  Returning lets "operator new" throw std::bad_alloc.  We can either...
    //
    //      (1)     Catch it in a high-level try/catch around App::run().
    //      (2)     OR, simply call  std::abort()  for an immediate abort.
    //
}


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



#ifdef _WIN32
// *************************************************************************** //
// *************************************************************************** //

/// @fn         BOOL WINAPI console_ctrl_handler(DWORD type);
///
/// @brief      Posix equivalent for handling console signals on Windows builds.
/// @param      type        ???
/// @return                 TRUE: Tell Windows that WE handled the error.  FALSE: Allow default handler to run
///
BOOL WINAPI console_ctrl_handler(DWORD type)
{
    BOOL    value       = FALSE;
    
    switch (type)
    {
        case CTRL_C_EVENT :         {               //  Ctrl-C
            value = FALSE;      break;
        }
        case CTRL_BREAK_EVENT :     {               //  Ctrl-Break
            value = FALSE;      break;
        }
        case CTRL_CLOSE_EVENT :     {               //  Console window close button.
            value = FALSE;      break;
        }
        case CTRL_SHUTDOWN_EVENT :  {               //  System is shutting down.
            // g_should_quit.store(true, std::memory_order_relaxed);
            value = TRUE;       break;
        }
        default :                   {
            value = FALSE;      break;
        }
    }
    
    
    return value;                   // let default handler run
}


// *************************************************************************** //
// *************************************************************************** //
#endif  //  _WIN32  //






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

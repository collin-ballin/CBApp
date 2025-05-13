/***********************************************************************************
*
*       ********************************************************************
*       ****            _ P Y S T R E A M . H  ____  F I L E            ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      May 09, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_UTILITY_PYSTREAM_H
#define _CBAPP_UTILITY_PYSTREAM_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  1.1.        ** MY **  HEADERS...
#include "_config.h"
#include "cblib.h"
//#include "widgets/widgets.h"
#include "app/_init.h"
#include "app/state/_state.h"



//  1.2     STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <cstdlib>          // C-Headers...
#include <stdio.h>

#include <string>           //  <======| std::string, ...
#include <string_view>
#include <vector>           //  <======| std::vector, ...
#include <stdexcept>        //  <======| ...
#include <deque>
#include <thread>
#include <mutex>
#include <atomic>

#include <limits.h>
#include <math.h>



//  1.3     "DEAR IMGUI" HEADERS...
#include "imgui.h"
#include "implot.h"
#include "imgui_internal.h"



#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <fcntl.h>
    #include <signal.h>
    #include <sys/wait.h>
#endif





namespace cb { namespace utl { //     BEGINNING NAMESPACE "cb" :: "utl"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************** //
// *************************************************************** //
// 3.  PRIMARY CLASS INTERFACE
// @brief NO DESCRIPTION PROVIDED
// *************************************************************** //
// *************************************************************** //

class PyStream
{
    //CBAPP_APPSTATE_ALIAS_API                //  CLASS-DEFINED, NESTED TYPENAME ALIASES.
    //friend class                App;
// *************************************************************************** //
// *************************************************************************** //
public:
    
    //  1               PUBLIC MEMBER FUNCTIONS...
    // *************************************************************************** //
    //  1.1                     Default Constructor, Destructor, etc.       [app/sidebar/sidebar.cpp]...
    explicit                    PyStream                            (const std::string & script_path,
                                                                     const std::vector<std::string> & args = {});
                                ~PyStream                           (void);                                 //  Def. Destructor.
    
    //  1.2                     Primary Class Interface.                    [app/sidebar/sidebar.cpp]...
    bool                        start                               (void);                                 //  Launch process & reader thread.
    void                        stop                                (void);                                 //  Terminate child & join thread.
    
    //  1.3                     Secondary Class Interface.
    void                        set_filepath                        (const char * );
    void                        set_filepath                        (const std::string & );
    std::string                 get_filepath                        (void);
    
    bool                        send                                (const std::string & msg);              //  write msg + \n
    bool                        try_receive                         (std::string & out);                    //  pop next complete line
    bool                        is_running                          (void)  const;

    //  1.3                     Deleted Operators, Functions, etc.
                                PyStream                            (const PyStream &    src)            = delete;   //  Copy. Constructor.
                                PyStream                            (PyStream &&         src)            = delete;   //  Move Constructor.
    PyStream &                  operator =                          (const PyStream &    src)            = delete;   //  Assgn. Operator.
    PyStream &                  operator =                          (PyStream &&         src)            = delete;   //  Move-Assgn. Operator.

    
    
// *************************************************************************** //
// *************************************************************************** //
protected:
    //  2.A             PROTECTED DATA-MEMBERS...
    // *************************************************************************** //

    //                          5.  IMPORTANT VARIABLES...
    std::string                 m_script_path;
    std::vector<std::string>    m_args;

    std::atomic_bool            m_running { false };

    std::thread                 m_reader_thread;
    std::mutex                  m_queue_mutex;
    std::deque<std::string>     m_recv_queue;      // pending messages
    //AppState &                  CBAPP_STATE_NAME;
    
    
#ifdef _WIN32
    HANDLE                      m_child_stdin_w                     = nullptr;
    HANDLE                      m_child_stdout_r                    = nullptr;
    PROCESS_INFORMATION         m_proc_info { 0 };
# else
    int                         m_child_stdin_fd                    = -1;
    int                         m_child_stdout_fd                   = -1;
    pid_t                       m_child_pid                         = -1;
#endif  //  _WIN32  //
    
    
    
    //  2.B             PROTECTED MEMBER FUNCTIONS...
    // *************************************************************************** //
    
    //  2B.1                    Secondary Class Methods.            [app/titlebar/titlebar.cpp]...
    bool                        launch_process                      (void);
    void                        reader_thread_func                  (void);
#ifdef _WIN32
    bool                        write_pipe                          (const char * data, size_t n);
#else
    bool                        write_fd                            (int fd, const char * data, size_t n);
#endif
    
    
    
    
    
    
// *************************************************************************** //
// *************************************************************************** //
private:
    //  3.              PRIVATE MEMBER FUNCTIONS...
    // *************************************************************************** //
    
    //
    //  ...
    //


// *************************************************************************** //
// *************************************************************************** //
};//	END "Menubar" INLINE CLASS DEFINITION.






// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" :: "utl" NAMESPACE.












#endif      //  _CBAPP_SIDEBAR_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.

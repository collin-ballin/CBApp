/***********************************************************************************
*
*       ********************************************************************
*       ****            _ P Y S T R E A M . H  ____  F I L E            ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
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
#include CBAPP_USER_CONFIG
#include "cblib.h"
//#include "widgets/widgets.h"
//  #include "app/_init.h"
#include "app/state/state.h"



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
    #include <errno.h>
#endif





namespace cb { namespace utl { //     BEGINNING NAMESPACE "cb" :: "utl"...
// *************************************************************************** //
// *************************************************************************** //







// *************************************************************************** //
// *************************************************************************** //
//                PRIMARY CLASS INTERFACE:
// 		Class-Interface for the "PyStream" Abstraction.
// *************************************************************************** //
// *************************************************************************** //

#define     PYSTREAM_REFACTOR   1

class PyStream
{
//      0.          CONSTANTS AND ALIASES...
// *************************************************************************** //
// *************************************************************************** //
public:

    // *************************************************************************** //
    //      0. |    NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    //  using                               MyAlias                         = MyTypename_t;
    using                                   path_t                          = std::filesystem::path;
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      0. |    STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    //  static constexpr float              ms_MY_CONSTEXPR_VALUE           = 240.0f;
    static constexpr size_t                 ms_READ_BUFFER_SIZE             = 4096ULL;
    
//
//
// *************************************************************************** //
// *************************************************************************** //   END "0.  CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//
//      1.          CLASS DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //
protected:
    
    // *************************************************************************** //
    //      1. |    STATE VARIABLES.
    // *************************************************************************** //
    //  std::string                         m_script_path                   ;
    path_t                                  m_script_path                   ;   // = {"../../scripts/python/fpga_stream.py"};
    

    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      1. |    IMPORTANT DATA-MEMBERS.
    // *************************************************************************** //
    std::vector<std::string>            m_args                          ;
    std::deque<std::string>             m_recv_queue                    ;      //   pending messages
    std::thread                         m_reader_thread                 ;
    std::mutex                          m_queue_mutex                   ;
    //
#ifdef PYSTREAM_REFACTOR
    std::mutex                          m_write_mutex                   ;
#endif  //  PYSTREAM_REFACTOR  //
    
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      1. |    _WIN32 DATA.
    // *************************************************************************** //
#ifdef _WIN32
    HANDLE                              m_child_stdin_w                 = nullptr;
    HANDLE                              m_child_stdout_r                = nullptr;
    PROCESS_INFORMATION                 m_proc_info {
          nullptr           //  hProcess
        , nullptr           //  hThread
        , 0U                //  dwProcessId
        , 0U                //  dwThreadId
    };
# else
    int                                 m_child_stdin_fd                = -1;
    int                                 m_child_stdout_fd               = -1;
    pid_t                               m_child_pid                     = -1;
#endif  //  _WIN32  //
    

    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      1. |    TRANSIENT STATE DATA.
    // *************************************************************************** //
    std::atomic_bool                    m_running                       { false };
    
    
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "1.  CLASS DATA-MEMBERS".



// *************************************************************************** //
//
//
//      2.A.        PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
public:
    
    // *************************************************************************** //
    //      2.A. |  INITIALIZATION METHODS.         |   "init.cpp" ...
    // *************************************************************************** //
    explicit                            PyStream                            (const std::string & script_path,   const std::vector<std::string> & args = {});  //  Def. Constructor.
                                        ~PyStream                           (void);                             //  Def. Destructor.
    
    // *************************************************************************** //
    //      2.A. |  DELETED FUNCTIONS.              |   ...
    // *************************************************************************** //
                                        PyStream                            (const PyStream &    src)       = delete;   //  Copy. Constructor.
                                        PyStream                            (PyStream &&         src)       = delete;   //  Move Constructor.
    PyStream &                          operator =                          (const PyStream &    src)       = delete;   //  Assgn. Operator.
    PyStream &                          operator =                          (PyStream &&         src)       = delete;   //  Move-Assgn. Operator.
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      2.A. |  MAIN API.                       |   "interface.cpp" ...
    // *************************************************************************** //
    bool                                start                               (void);                             //  Launch process & reader thread.
    void                                stop                                (void);                             //  Terminate child & join thread.
    
    bool                                send                                (const std::string & msg);          //  write msg + \n
    bool                                try_receive                         (std::string & out);                //  pop next complete line
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "2A.  PUBLIC MEMBER FUNCS".


    
// *************************************************************************** //
//
//
//      2.B.        PROTECTED MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
protected:

    // *************************************************************************** //
    //      2.B. |  OPERATION FUNCTIONS.            |   "tools.cpp" ...
    // *************************************************************************** //
    bool                                launch_process                      (void);
    void                                reader_thread_func                  (void);
#ifdef _WIN32
    bool                                write_pipe                          (const char * data, size_t n);
#else
    bool                                write_fd                            (int fd, const char * data, size_t n);
#endif
    
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "2B.  PROTECTED" FUNCTIONS.

    
   
// *************************************************************************** //
//
//
//      2.C.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
public:

    // *************************************************************************** //
    //      2.C. |  QUERY FUNCTIONS.
    // *************************************************************************** //

    //  "is_running"
    [[nodiscard]] inline bool           is_running                          (void) const noexcept       { return this->m_running.load(); }


    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      2.C. |  SETTER/GETTER FUNCTIONS.
    // *************************************************************************** //
    
    //  "set_filepath"
    inline void                         set_filepath                        (const char * path) noexcept            { this->m_script_path = path_t{ path }; }
    inline void                         set_filepath                        (const std::string & path) noexcept     { this->m_script_path = path_t{ path }; }
    inline void                         set_filepath                        (const path_t & path) noexcept          { this->m_script_path = path; }

    //  "get_filepath"
    [[nodiscard]] inline std::string    get_filepath                        (void) const noexcept                   { return this->m_script_path.string(); }


    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      2.C. |  CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    
    
    
    // *************************************************************************** //
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "2C.  INLINE" FUNCTIONS.






// *************************************************************************** //
// *************************************************************************** //
};//	END "PyStream" INLINE CLASS DEFINITION.




















// *************************************************************** //
// *************************************************************** //
// 3.  PRIMARY CLASS INTERFACE
// @brief NO DESCRIPTION PROVIDED
// *************************************************************** //
// *************************************************************** //
/*
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
//  PROCESS_INFORMATION         m_proc_info { 0 };
    PROCESS_INFORMATION         m_proc_info {
                                    nullptr,    // hProcess
                                    nullptr,    // hThread
                                    0u,         // dwProcessId
                                    0u          // dwThreadId
                                };
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
};//	END "PyStream" INLINE CLASS DEFINITION.

*/




// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" :: "utl" NAMESPACE.












#endif      //  _CBAPP_SIDEBAR_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.

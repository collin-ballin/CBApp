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
//
//
//
//      0.      SMALL_INTERNAL_NAMESPACE...
// *************************************************************************** //
// *************************************************************************** //
//
namespace process { //     BEGINNING NAMESPACE "process"...



// *************************************************************************** //
//      "process" |     TYPES.
// *************************************************************************** //

//  "ProcessState"
//
enum class ProcessState : uint8_t
{
      None = 0                      //  No process exists / not our child / already reaped.
    , Running                       //  alive and running.
    , Zombie                        //  POSIX: exited but not yet reaped (still has a PID).
    , Exited                        //  Terminated and reaped (or, on Windows: not STILL_ACTIVE)
    , Unknown                       //  Non-destructive probe (the process could be RUNNING or a ZOMBIE but we have to kill it to check).
    , COUNT
};


//  "ProcessInfo"
//
struct ProcessInfo
{
    ProcessState        state           = ProcessState::None;       //  presence/liveness
    bool                reaped          = false;                    //  POSIX: true iff this call reaped the child
    int                 exit_code       = INT_MIN;                  //  valid if state==Exited (or Zombie->Exited on this call)
#ifndef _WIN32
    int                 term_sig        = 0;                        //  POSIX only; >0 if signaled; else 0
#endif  //  _WIN32  //
};



//
// *************************************************************************** //
// *************************************************************************** //   END [ 1.0.  "TYPES" ].



// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}// END NAMESPACE "process".












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
    using                                   ProcessState                    = process::ProcessState;
    using                                   ProcessInfo                     = process::ProcessInfo;
    using                                   path_t                          = std::filesystem::path;
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      0. |    STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    static constexpr size_t                 ms_READ_BUFFER_SIZE             = 4096ULL;
  # ifdef _WIN32
    static constexpr DWORD                  ms_PROCESS_TIMEOUT_MS           = 5000UL;       //  TIMING CONSTANTS (5 seconds)
  # endif  //  _WIN32  //
    //
    //
    static constexpr size_t                 ms_DEF_QUEUE_CAPACITY           = 1024ULL;
    static constexpr size_t                 ms_MIN_QUEUE_CAPACITY           = 128ULL;
    static constexpr size_t                 ms_MAX_QUEUE_CAPACITY           = 16'384ULL;    //  2^14
    
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
    //      1. |    PARAMETER VARIABLES  [ PROVIDE AS CSTOR ARGS ].
    // *************************************************************************** //
    //  std::string                         m_script_path                   ;
    path_t                                  m_script_path                   ;                           // = {"../../scripts/python/fpga_stream.py"};
    //
    std::vector<std::string>                m_args                          ;
    size_t                                  m_queue_capacity                = ms_DEF_QUEUE_CAPACITY;    //  cap; tune as needed


    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      1. |    IMPORTANT DATA-MEMBERS.
    // *************************************************************************** //
    std::deque<std::string>                 m_recv_queue                    ;      //   pending messages
    std::thread                             m_reader_thread                 ;
    std::mutex                              m_queue_mutex                   ;
    //
    std::mutex                              m_write_mutex                   ;
    
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      1. |    _WIN32 DATA.
    // *************************************************************************** //
#ifdef _WIN32
    HANDLE                                  m_child_stdin_w                 = nullptr;
    HANDLE                                  m_child_stdout_r                = nullptr;
    PROCESS_INFORMATION                     m_proc_info {
          nullptr           //  hProcess
        , nullptr           //  hThread
        , 0U                //  dwProcessId
        , 0U                //  dwThreadId
    };
# else
    int                                     m_child_stdin_fd                = -1;
    int                                     m_child_stdout_fd               = -1;
    pid_t                                   m_child_pid                     = -1;
#endif  //  _WIN32  //
    

    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      1. |    TRANSIENT, INTERNAL STATE DATA  [ PURELY INTERNAL ].
    // *************************************************************************** //
    std::atomic_bool                        m_running                       { false };
    std::atomic<size_t>                     m_dropped_lines                 { 0 };          //  drop-old counter
    //
    //                                  TELEMETRY ACCESSORS:
    std::atomic<int>                        m_last_exit_code                { INT_MIN };    //  POSIX: WEXITSTATUS or -1;       Windows: GetExitCodeProcess
#ifndef _WIN32
    std::atomic<int>                        m_last_term_sig                 { 0 };          //  POSIX only; 0 if none
#endif  //  _WIN32  //

    
    
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
    //                              OPERATION FUNCTIONS:
    bool                                start                               (void);                             //  Launch process & reader thread.
    void                                stop                                (void);                             //  Terminate child & join thread.
    void                                shutdown                            (void);                             //  [[ NOT IMPLEMENTED ]]
    //
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
//      2.C.        PUBLIC INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
public:

    // *************************************************************************** //
    //      2.C. |  QUERY FUNCTIONS.
    // *************************************************************************** //

    //  "is_running"
    [[nodiscard]] inline bool           is_running                          (void) const noexcept       { return this->m_running.load(); }


    //  "is_process_running"
    //
    [[nodiscard]] inline bool           is_process_running                  (void) const noexcept {
    #ifdef _WIN32
        //      1.      [ _WIN32 ] :    STILL_ACTIVE == running...
        if ( this->m_proc_info.hProcess == nullptr )  { return false; }

        DWORD       code        = 0;
        const BOOL  ok          = ::GetExitCodeProcess( this->m_proc_info.hProcess, &code );
        if ( ok == FALSE )                              { return false; }
        if ( code == STILL_ACTIVE )                     { return true;  }
        else                                            { return false; }
    #else
        //      1.      [ POSIX ] :     NON-DESTRUCTIVE: "kill(pid, 0)"...
        if ( this->m_child_pid <= 0 )                   { return false; }

        const int   rc          = ::kill( this->m_child_pid, 0 );
        if ( rc == 0 )                                  { return true;  }   // present (Running or Zombie)
        switch ( errno )
        {
            case EPERM:     { return true;  }   // present but no permission
            case ESRCH:     { return false; }   // no such process
            default:        { return false; }   // conservative
        }
    #endif
    }
    
    
    //  "get_process_info"
    //
    # ifdef _WIN32
    //
    //
    [[nodiscard]] inline ProcessInfo    get_process_info                    (void) const noexcept {
        using           State       = ProcessState;
        ProcessInfo     out         = {   };
        DWORD           code        = 0;


        //      1.      [ _WIN32 ] :    QUERY EXIT CODE; NO ZOMBIE STATE...
        if ( this->m_proc_info.hProcess == nullptr )    { out.state = State::None;  return out; }

        const BOOL      ok          = ::GetExitCodeProcess( this->m_proc_info.hProcess, &code );
        
        if ( ok == FALSE )                  { out.state = State::None;      return out; }
        if ( code == STILL_ACTIVE )         { out.state = State::Running;   return out; }

        out.state           = State::Exited;                //  terminated (handle signaled)
        out.exit_code       = static_cast<int>(code);
        out.reaped          = false;                        //  Windows: no reap concept

        return out;
    }
    //
    //
    # else
    //
    //
    [[nodiscard]] inline ProcessInfo    get_process_info                    (const bool allow_reap=false) const noexcept {
        using           State       = ProcessState;
        ProcessInfo     out         = {   };
        int             rc_kill     = ::kill( this->m_child_pid, 0 );
        int             status      = 0;
        pid_t           r           = -1;
    
    
        //      1.      CHECK IF ANY PROCESS IS PRESENT...
        if ( this->m_child_pid <= 0 )       { out.state = State::None; return out; }

        rc_kill     = ::kill( this->m_child_pid, 0 );
        if ( rc_kill == -1 )
        {
            switch ( errno ) {
                case EPERM :    { /* present but no permission */ break;    }
                case ESRCH :    { out.state = State::None;  return out;     }
                default    :    { out.state = State::None;  return out;     }
            }
        }


        //      2.      PROCESS *IS* PRESENT (either RUNNING or ZOMBIE)...
        //
        //              2A.     NON-OBSERVATORY CHECK  [ Caller chooses NOT to disambiguate ].
        if ( !allow_reap )              { out.state = State::Unknown;  return out; }
        //
        //              2B.     COLLAPSE WAVE-FUNC. [ Disambiguate with waitpid(WNOHANG) ].
        r       = ::waitpid( this->m_child_pid, &status, WNOHANG );


        //      3.      COLLECT DISAMBIGUATED PROCESS INFO...
        //
        //              3A.     CHILD HAS NOT CHANGED STATE (not waitable -> Running (not zombie)).
        if ( r == 0 )       { out.state = State::Running;  return out; }
        //
        //              3B.     TERMINAL STATE (we REAPED it in this call (zombie or just exited)).
        if ( r == this->m_child_pid )
        {
            out.reaped  = true;
            if ( WIFEXITED(status) ) {
                out.state       = State::Exited;
                out.exit_code   = WEXITSTATUS(status);
                out.term_sig    = 0;
                return out;
            }
            if ( WIFSIGNALED(status) ) {
                out.state       = State::Exited;
                out.exit_code   = -1;
                out.term_sig    = WTERMSIG(status);
                return out;
            }
            //
            //      should not reach here; treat as exited with unknown details
            out.state       = State::Exited;
            out.exit_code   = -1;
            out.term_sig    = 0;
            return out;
        }
        //
        //              3C.     NOT OUR CHILD / ALREADY REAPED BY SOMEBODY ELSE / ERROR.
        if ( r == -1 )  { out.state = State::None;  return out; }
    
        return out;
    }
    //
    //
    #endif  //  _WIN32  //


    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      2.C. |  SETTER/GETTER FUNCTIONS.
    // *************************************************************************** //
    
    //  "set_filepath"
    inline void                         set_filepath                        (const char * path) noexcept            { this->m_script_path = path_t{ path }; }
    inline void                         set_filepath                        (const std::string & path) noexcept     { this->m_script_path = path_t{ path }; }
    inline void                         set_filepath                        (const path_t & path) noexcept          { this->m_script_path = path; }


    //  "set_queue_capacity"
    inline void                         set_queue_capacity                  (const size_t cap) {
        if ( (cap < PyStream::ms_MIN_QUEUE_CAPACITY)  ||  (cap > PyStream::ms_MAX_QUEUE_CAPACITY) ) {
            throw std::out_of_range(  std::format("queue_capacity: value must be inside range [{}, {}]"
                                    , PyStream::ms_MIN_QUEUE_CAPACITY, PyStream::ms_MAX_QUEUE_CAPACITY) );
        }
        this->m_queue_capacity = cap;
    }



    //  "get_filepath"
    [[nodiscard]] inline std::string    get_filepath                        (void) const noexcept   { return this->m_script_path.string(); }

    //  "get_queue_capacity"
    [[nodiscard]] inline size_t         get_queue_capacity                  (void) const noexcept   { return this->m_queue_capacity;            }
    [[nodiscard]] inline size_t         get_dropped_lines                   (void) const noexcept   { return this->m_dropped_lines.load();      }

    //  "get_last_exit_code"        TELEMETRY ACCESSORS...
    [[nodiscard]] inline int            get_last_exit_code                  (void) const noexcept   { return this->m_last_exit_code.load();     }
  # ifdef _WIN32
    [[nodiscard]] inline int            get_last_termination_signal         (void) const noexcept   { return -1;                                }   //  DISABLED ON NON-POSIX.
  # else
    [[nodiscard]] inline int            get_last_termination_signal         (void) const noexcept   { return this->m_last_term_sig.load();      }
  # endif  //  _WIN32  //
  
  
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "2C.  PUBLIC INLINE".

    
   
// *************************************************************************** //
//
//
//      2.D.        PROTECTED INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
protected:

    // *************************************************************************** //
    //      2.D. |  CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    
    //  "enqueue_line_"
    inline void                         enqueue_line_                       (std::string && s)
    {
        std::lock_guard<std::mutex>     lock    (this->m_queue_mutex);
        if ( this->m_recv_queue.size() >= this->m_queue_capacity ) {
            this->m_recv_queue.pop_front(); ++this->m_dropped_lines;    // drop-old policy
        }
        this->m_recv_queue.emplace_back(std::move(s));
        return;
    }

    
    // *************************************************************************** //
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "2D.  PROTECTED INLINE".






// *************************************************************************** //
// *************************************************************************** //
};//	END "PyStream" INLINE CLASS DEFINITION.












// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" :: "utl" NAMESPACE.












#endif      //  _CBAPP_SIDEBAR_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.

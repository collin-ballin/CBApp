/***********************************************************************************
*
*       ********************************************************************
*       ****            _ P Y S T R E A M . H  ____  F I L E            ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond.
*               DATED:      May 09, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/app.h"
#include "utility/pystream/pystream.h"
#include <sstream>
#include <iostream>
#include <cstring>


#ifdef _WIN32
//
    #ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0600     //  for CancelSynchronousIo on Vista+
    #endif
    #include <windows.h>
    //  no POSIX headers on Windows
//
# else
//
    #include <unistd.h>             //  fork, execvp, pipe, dup2, read, write, close
    #include <fcntl.h>              //  fcntl, FD_CLOEXEC, O_CLOEXEC
    #include <sys/types.h>          //  pid_t
    #include <sys/wait.h>           //  waitpid, WIFEXITED, WEXITSTATUS, WIFSIGNALED, WTERMSIG
    #include <errno.h>              //  errno, EINTR, EAGAIN, EWOULDBLOCK, EPIPE
//
#endif  //  _WIN32  //






namespace cb { namespace utl { //     BEGINNING NAMESPACE "cb" :: "utl"...
// *************************************************************************** //
// *************************************************************************** //



//      0.      STATIC INLINE HELPER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
//
#ifdef _WIN32

//  "s_utf8_to_wide"
//
static inline std::wstring s_utf8_to_wide(const std::string & s)
{
    if ( s.empty() )        { return {}; }

    const int n = ::MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);   // includes NUL
    if ( n <= 0 )           { return {}; }

    std::wstring w;
    w.resize(static_cast<size_t>(n));                                             // room for NUL

    const int wrote = ::MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, w.data(), n);
    if ( wrote <= 0 )   { return {}; }

    // drop trailing NUL (MultiByteToWideChar writes it when cchWideChar == -1)
    if ( !w.empty() && w.back() == L'\0' ) { w.pop_back(); }

    return w;
}


//  "s_quote_if_needed"
//
static inline std::wstring s_quote_if_needed(const std::wstring & s)
{
    return (s.find_first_of(L" \t\"") == std::wstring::npos) ? s : L"\"" + s + L"\"";
}


//  "_win32_last_error_string"
//      Windows helper for readable errors.
//
static inline std::string _win32_last_error_string(void)
{
    const DWORD     e           = ::GetLastError();
    LPSTR           buf         = nullptr;
    std::string     message     = {  };
    const DWORD     n           = ::FormatMessageA(
          FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS
        , nullptr
        , e
        , MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)
        , (LPSTR)&buf
        , 0
        , nullptr
    );
    message                     = (n && buf)    ? std::string(buf, n)   : std::string("unknown Win32 error");
    if (buf)    { ::LocalFree(buf); }
    
    return message;
}



#endif  //  _WIN32  //
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 0.  "STATIC UTILS." ]].












// *************************************************************************** //
//
//
//
//      1.      INITIALIZATION  | DEFAULT CONSTRUCTOR, DESTRUCTOR, ETC...
// *************************************************************************** //
// *************************************************************************** //

//  Parametric Constructor 1.
//
PyStream::PyStream(const std::string & script_path, const std::vector<std::string> & args)
    : m_script_path( path_t(script_path) )
    , m_args(args)
{  }


//  Destructor.
//
PyStream::~PyStream(void)
{
    this->stop();
}






// *************************************************************************** //
//
//
//
//      1B.     MAIN PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "start"
//
///     @brief              Start the child Python process and the reader thread; returns the child PID on success.
///
///     @description        Launches the configured Python interpreter (`m_python_exe`) with the configured script
///                         (`m_script_path`) and arguments (`m_args`). On success, marks the stream as running,
///                         spawns the background reader thread, and returns a snapshot of the child PID.
///                         This function enforces preconditions and converts spawn failures into rich exceptions.
///
///     @return                         The operating-system process identifier (PID) of the newly launched child process.
///     @retval     >0                  A valid child PID. (Zero is never returned.)
///     @throw  std::logic_error        Thrown when the stream is already running, or when no script path has been set.
///     @throw  std::invalid_argument   Thrown if the configured script path is empty, does not exist, or is not a regular file (via internal filesystem validation).
///     @throw  std::runtime_error      Thrown if filesystem queries themselves fail unexpectedly (e.g., permission/IO errors during existence/type checks), with the underlying diagnostic message.
///     @throw  std::runtime_error      **WINDOWS ONLY:** thrown when process creation fails; the message includes the textual form of the last Win32 error (from GetLastError / FormatMessage).
///     @throw  std::system_error       **POSIX ONLY:** thrown when process creation fails; carries the failing errno and category.  Also thrown if the reader thread cannot be created (std::thread construction failure).
///
///     @attention      A returned PID is not a liveness guarantee; use the class’s probe utilities
///                         (e.g., child_alive / child_probe) to check the current state after launch.
///     @warning        This call is not reentrant; concurrent calls will result in exactly one launch
///                         and the others throwing std::logic_error (“already running”).
///     @remark         On success, resources are fully initialized (pipes + reader thread). On any failure,
///                         the function performs best-effort rollback: the child (if created) is terminated,
///                         pipe handles/fds are closed, and @c m_running is reset to @c false.
//
#ifdef PYSTREAM_REFACTOR
[[nodiscard]] uint32_t PyStream::start(void)
{
    bool    expected    = false;
    
    
    //      CASE 0 :    ATTEMPT TO RUN A PROCESS WITHOUT SPECIFYING IT'S FILEPATH
    //                  *OR*  ATTEMPT TO "start()" WHILE ALREADY RUNNING...
    if ( this->m_script_path.empty() )                                  { throw std::logic_error("PyStream::start: no script path set");    }
    if ( this->m_running.load() )                                       { throw std::logic_error("PyStream::start: already running");       }

    //      Re-validate that the script file still exists (in case the file was removed since set-time).
    (void)PyStream::_validate_filepath(this->m_script_path, "PyStream::start", false);
    
    
    if ( !this->m_running.compare_exchange_strong(expected, true) )     { throw std::logic_error("PyStream::start: already running"); }


    //      1.      SPAWN THE CHILD PROCESS...
    if ( !this->launch_process() )
    {
        this->m_running.store(false);
    # ifdef _WIN32
        throw std::runtime_error(std::string("PyStream::start: launch_process() failed: ") + _win32_last_error_string());
    # else
        const int e = errno;
        throw std::system_error(e, std::system_category(), "PyStream::start: launch_process() failed");
    # endif  //  _WIN32  //
    }



    //      2A.     ATTEMPT TO BEGIN THE "READER" THREAD.  [ IF THIS FAILS, ROLL-BACK CLEANLY ]...
    try {
        this->m_reader_thread = std::thread(&PyStream::reader_thread_func, this);
    }
    //
    //      2B.     FAILURE TO SPAWN THE READER THREAD  [ EXCEPTION WAS THROWN ]...
    //                  -- Terminate the child; close our ends (best-effort rollback).
    catch (...)
    {
    # ifdef _WIN32
        if (this->m_proc_info.hProcess)
        {
            ::TerminateProcess(this->m_proc_info.hProcess, 0);
            ::WaitForSingleObject(this->m_proc_info.hProcess, PyStream::ms_PROCESS_TIMEOUT_MS);
            ::CloseHandle(this->m_proc_info.hProcess);  this->m_proc_info.hProcess = nullptr;
            ::CloseHandle(this->m_proc_info.hThread );  this->m_proc_info.hThread  = nullptr;
        }
        if (this->m_child_stdin_w )         { ::CloseHandle(this->m_child_stdin_w );  this->m_child_stdin_w  = nullptr; }
        if (this->m_child_stdout_r)         { ::CloseHandle(this->m_child_stdout_r);  this->m_child_stdout_r = nullptr; }
    # else
        if (this->m_child_pid > 0)          { ::kill(this->m_child_pid, SIGTERM);  ::waitpid(this->m_child_pid, nullptr, 0);  this->m_child_pid = -1; }
        if (this->m_child_stdin_fd  != -1)  { ::close(this->m_child_stdin_fd ); this->m_child_stdin_fd  = -1; }
        if (this->m_child_stdout_fd != -1)  { ::close(this->m_child_stdout_fd); this->m_child_stdout_fd = -1; }
    # endif  //  _WIN32  //
    
        this->m_running.store(false);
        throw; // rethrow original thread exception
    }

    return this->get_pid();   // success → PID snapshot
}
//
#else
//
bool PyStream::start(void)
{
    bool    expected    = false;
    
    
    //      CASE 0 :    ATTEMPT TO RUN A PROCESS WITHOUT SPECIFYING IT'S FILEPATH
    //                  *OR*  ATTEMPT TO "start()" WHILE ALREADY RUNNING...
    if ( this->m_script_path.empty() )                          { throw std::logic_error("PyStream::start: no script path set");    }
    if ( this->m_running.load() )                               { throw std::logic_error("PyStream::start: already running");       }



    //      Re-validate that the script file still exists (in case the file was removed since set-time).
    (void)PyStream::_validate_filepath(this->m_script_path, "PyStream::start.script", /*require_exec=*/false);


    if ( !this->m_running.compare_exchange_strong(expected, true) )
        { throw std::logic_error("PyStream::start: already running"); }


    // Spawn child; translate “false” to a descriptive exception.
    if ( !this->launch_process() )
    {
        this->m_running.store(false);
    # ifdef _WIN32
        throw std::runtime_error(std::string("PyStream::start: launch_process() failed: ") + _win32_last_error_string());
    # else
        const int e = errno;
        throw std::system_error(e, std::system_category(), "PyStream::start: launch_process() failed");
    # endif  //  _WIN32  //
    }



    //      2A.     ATTEMPT TO BEGIN THE "READER" THREAD.  [ IF THIS FAILS, ROLL-BACK CLEANLY ]...
    try {
        this->m_reader_thread = std::thread(&PyStream::reader_thread_func, this);
    }
    //
    //      2B.     FAILURE TO SPAWN THE READER THREAD  [ EXCEPTION WAS THROWN ]...
    //                  -- Terminate the child; close our ends (best-effort rollback).
    catch (...)
    {
    # ifdef _WIN32
        if (this->m_proc_info.hProcess)
        {
            ::TerminateProcess(this->m_proc_info.hProcess, 0);
            ::WaitForSingleObject(this->m_proc_info.hProcess, PyStream::ms_PROCESS_TIMEOUT_MS);
            ::CloseHandle(this->m_proc_info.hProcess);  this->m_proc_info.hProcess = nullptr;
            ::CloseHandle(this->m_proc_info.hThread );  this->m_proc_info.hThread  = nullptr;
        }
        if (this->m_child_stdin_w )         { ::CloseHandle(this->m_child_stdin_w );  this->m_child_stdin_w  = nullptr; }
        if (this->m_child_stdout_r)         { ::CloseHandle(this->m_child_stdout_r);  this->m_child_stdout_r = nullptr; }
    # else
        if (this->m_child_pid > 0)          { ::kill(this->m_child_pid, SIGTERM);  ::waitpid(this->m_child_pid, nullptr, 0);  this->m_child_pid = -1; }
        if (this->m_child_stdin_fd  != -1)  { ::close(this->m_child_stdin_fd ); this->m_child_stdin_fd  = -1; }
        if (this->m_child_stdout_fd != -1)  { ::close(this->m_child_stdout_fd); this->m_child_stdout_fd = -1; }
    # endif  //  _WIN32  //
    
        this->m_running.store(false);
        throw; // rethrow original thread exception
    }

    return true;
}
//
#endif  //  PYSTREAM_REFACTOR  //



//  "try_start"
//
[[nodiscard]] std::optional<uint32_t> PyStream::try_start(void) noexcept {
    try             { return this->start();     }
    catch (...)     { return std::nullopt;      }
}



//  "stop"
//
void PyStream::stop(void)
#ifdef PYSTREAM_REFACTOR
{
    if ( !this->m_running.exchange(false) )     { return; }       //  already stopped

#ifdef _WIN32
    //      1.      [ _WIN32 ] :    CANCEL THE BLOCKED "ReadFile" IN THE READER-THREAD (IF ANY)...
    if ( this->m_reader_thread.joinable() ) {
        ::CancelSynchronousIo( (HANDLE)this->m_reader_thread.native_handle() );     //  native_handle() is a HANDLE on Windows
        this->m_reader_thread.join();                                              //  ** ensure reader is done before touching pipe handles **
    }


    //      2.      [ _WIN32 ] :    TERMINATE CHILD PROCESS AND WAIT FOR IT TO EXIT;  COLLECT EXIT_CODE AND SIGNAL...
    if ( this->m_proc_info.hProcess )
    {
        DWORD                           code        = 0;
        [[maybe_unused]] const BOOL     kill_ok     = ::TerminateProcess    (this->m_proc_info.hProcess , 0) ;
        const DWORD                     wait_rc     = ::WaitForSingleObject (this->m_proc_info.hProcess , PyStream::ms_PROCESS_TIMEOUT_MS );
    
        //      2A.     DESTROY THE CHILD.  CORRUPT THEM ALL.
        switch ( wait_rc )
        {
            //      CASE 0 :        Child dies within the timeout duration.
            case WAIT_OBJECT_0 :    { break; }
            //
            //      CASE 1 :        Child is STILL ALIVE beyond the timeout duration.
            case WAIT_TIMEOUT  :    { /* ::TerminateProcess(...); ::WaitForSingleObject(..., INFINITE); */  break; }
            //
            //      CASE 2 :        Child is STILL ALIVE beyond the timeout duration.
            case WAIT_FAILED   :    { /* log GetLastError(); proceed to close handles */                    break; }
            //
            //
            //      DEFAULT :       Defensive Default Case.
            default            :    {  break; }
        }
    
        if ( ::GetExitCodeProcess(this->m_proc_info.hProcess, &code) )   { this->m_last_exit_code.store((int)code); }
        else                                                             { this->m_last_exit_code.store(-1); }

        ::CloseHandle(this->m_proc_info.hProcess);   this->m_proc_info.hProcess = nullptr;
        ::CloseHandle(this->m_proc_info.hThread );   this->m_proc_info.hThread  = nullptr;
    }
    //  this->m_last_term_sig.store(0);     //  ** DO NOT UN-COMMENT **     (No m_last_term_sig on Windows).


    //      3.      [ _WIN32 ] :    CLOSE OUR PIPE-ENDS (idempotent)...
    if (this->m_child_stdin_w)      { CloseHandle(this->m_child_stdin_w);   this->m_child_stdin_w  = nullptr; }
    if (this->m_child_stdout_r)     { CloseHandle(this->m_child_stdout_r);  this->m_child_stdout_r = nullptr; }
//
//
# else
//
//
    //      1.      [ POSIX ] :     TERMINATE & COLLECT EXIT-STATUS...
    if ( this->m_child_pid > 0 )
    {
        int     status      = 0;
        ::kill(this->m_child_pid, SIGTERM);
        ::waitpid(this->m_child_pid, &status, 0);

        if      ( WIFEXITED(status) )           { this->m_last_exit_code.store(WEXITSTATUS(status))     ; this->m_last_term_sig.store(0);                   }
        else if ( WIFSIGNALED(status) )         { this->m_last_exit_code.store(-1)                      ; this->m_last_term_sig.store(WTERMSIG(status));    }
        else                                    { this->m_last_exit_code.store(-1)                      ; this->m_last_term_sig.store(0);                   }
        this->m_child_pid = -1;
    }
    
    //      2.      [ POSIX ] :     CLOSE OUR PIPE-ENDS (idempotent)...
    if ( this->m_child_stdin_fd  != -1 )        { ::close( this->m_child_stdin_fd  );   this->m_child_stdin_fd  = -1; }
    if ( this->m_child_stdout_fd != -1 )        { ::close( this->m_child_stdout_fd );   this->m_child_stdout_fd = -1; }
//
//
#endif  //  _WIN32  //

    //      4.      JOIN READER & CLEAR QUEUE...
    if ( this->m_reader_thread.joinable() )     { this->m_reader_thread.join(); }

    std::lock_guard<std::mutex>     lock    (this->m_queue_mutex);
    this->m_recv_queue.clear();
    return;
}
//
#else
//
{
    if ( !m_running.exchange(false) )   { return; }     //  already stopped.

#ifdef _WIN32
    if (m_proc_info.hProcess)
    {
        // ask nicely first — send CTRL‑BREAK equivalent?
        TerminateProcess(m_proc_info.hProcess, 0);
        WaitForSingleObject(m_proc_info.hProcess, 5000);
        CloseHandle(m_proc_info.hProcess);
        CloseHandle(m_proc_info.hThread);
    }
    if (m_child_stdin_w)    { CloseHandle(m_child_stdin_w);  }
    if (m_child_stdout_r)   { CloseHandle(m_child_stdout_r); }
#else
    if (m_child_pid > 0)
    {
        kill(m_child_pid, SIGTERM);
        waitpid(m_child_pid, nullptr, 0);
    }
    if (m_child_stdin_fd  != -1)    { close(m_child_stdin_fd);  }
    if (m_child_stdout_fd != -1)    { close(m_child_stdout_fd); }
#endif

    if (m_reader_thread.joinable())
        m_reader_thread.join();

    std::lock_guard<std::mutex> lock(m_queue_mutex);
    m_recv_queue.clear();
    return;
}
#endif  //  PYSTREAM_REFACTOR  //


//  "send"
//
bool PyStream::send(const std::string & msg)
#ifdef PYSTREAM_REFACTOR
{
    std::string                         line            = msg;
    std::lock_guard<std::mutex>         guard           (this->m_write_mutex);
    
    if ( !m_running.load() )                            { return false; }

    if ( line.empty()  ||  (line.back() != '\n') )      { line.push_back('\n'); }


# ifdef _WIN32
    return this->write_pipe(line.data(), line.size());
# else
    return write_fd(this->m_child_stdin_fd, line.data(), line.size());
# endif  //  _WIN32  //
}
//
#else
//
{
    if ( !m_running.load() )                        { return false; }
    std::string line = msg;
    if ( line.empty() || line.back() != '\n' )      { line.push_back('\n'); }
# ifdef _WIN32
    return write_pipe(line.data(), line.size());
# else
    return write_fd(m_child_stdin_fd, line.data(), line.size());
# endif
    return;
}
#endif  //  PYSTREAM_REFACTOR  //
    

//  "try_receive"
//      try_receive (non‑blocking)
//
bool PyStream::try_receive(std::string & out)
{
    std::lock_guard<std::mutex>     lock    (m_queue_mutex);
    if ( m_recv_queue.empty() )     { return false; }
    
    out             = std::move(m_recv_queue.front());
    m_recv_queue    .pop_front();
    return true;
}


//  "reader_thread_func"
//      reader thread — blocks on stdout and pushes complete lines
//
void PyStream::reader_thread_func(void)
#ifdef PYSTREAM_REFACTOR
{
    constexpr size_t            BSIZE       = PyStream::ms_READ_BUFFER_SIZE;
    static thread_local char    s_buffer    [BSIZE];
    //
    std::string                 line        = {   };        //     accumulates current line (without '\n')
    char                        ch          = '\0';
#ifdef _WIN32
    DWORD                       n           = 0;
# else
    ssize_t                     n           = 0;
#endif  //  _WIN32  //
    line.reserve(BSIZE);
    

#ifdef _WIN32
    while ( this->m_running.load() )
    {
        const BOOL  ok      = ReadFile(m_child_stdout_r, s_buffer, static_cast<DWORD>(BSIZE), &n, nullptr);
        
        if ( !ok || n == 0 )            { break; }  //  CASE 1 :    BREAK LOOP ON EOF or ERROR...
         
        //      2.      ITERATE THRU INPUT...
        for (DWORD i = 0; i < n; ++i)
        {
            ch = s_buffer[i];
            
            switch (ch)
            {
                //      CASE 2.1 :      CRLF -> LF normalize.
                case '\r': {
                    continue;
                }
                //      CASE 2.2. :     ???.
                case '\n': {
                    enqueue_line_(std::move(line));  line.clear();
                    break;
                }
                //
                //      CASE 2.3. :     DEFAULT / ELSE.
                default: {
                    line.push_back(ch);
                    break;
                }
            }
        //
        }// END  input-iteration.
    //
    }// END "while(run)".
#else
    for (;;)
    {
        n               = ::read(this->m_child_stdout_fd, s_buffer, BSIZE);
        
        if (n <= 0)     { break; }      //  CASE 1 :    BREAK LOOP ON EOF or ERROR...
        
        //      2.      ITERATE THRU INPUT...
        for (ssize_t i = 0; i < n; ++i)
        {
            ch = s_buffer[i];
            
            switch (ch)
            {
                //      CASE 2.1. :     ???.
                case '\n': {
                    enqueue_line_(std::move(line));  line.clear();
                    break;
                }
                //
                //      CASE 2.2. :     DEFAULT / ELSE.
                default: {
                    line.push_back(ch);
                    break;
                }
            }
        //
        }// END  input-iteration.
        
        if ( !m_running.load() )    { break; }
    //
    //
    }// END "while(run)".
    
#endif
    if ( !line.empty() )    { enqueue_line_(std::move(line));  line.clear(); }      //  flush any unterminated tail on EOF.

    this->m_running.store(false);     // reflect EOF in liveness
    return;
}
//
#else
//
{
    std::string     buf;
    char            ch;
    buf             .reserve(this->ms_READ_BUFFER_SIZE);
    
#ifdef _WIN32
    DWORD nread;
    while ( m_running.load() )
    {
        if ( !ReadFile(m_child_stdout_r, &ch, 1, &nread, nullptr) || nread == 0 )   { break; }  // pipe closed
        if (ch == '\n')
        {
            std::lock_guard<std::mutex>     lock    (m_queue_mutex);
            m_recv_queue                    .emplace_back(std::move(buf));
            buf.clear();
        }
        else buf.push_back(ch);
    }
#else
    while ( m_running.load() )
    {
        ssize_t     n       = ::read(m_child_stdout_fd, &ch, 1);
        if (n <= 0)         { break; }
        if (ch == '\n')
        {
            std::lock_guard<std::mutex> lock(m_queue_mutex);
            m_recv_queue.emplace_back(std::move(buf));
            buf.clear();
        }
        else buf.push_back(ch);
    }
#endif

    return;
}
#endif  //  PYSTREAM_REFACTOR  //



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 1.  "PUBLIC API" ]].












// *************************************************************************** //
//
//
//
//      2.      PROTECTED MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//      2A. PROTECTED. |     MULTI-PLATFORM IMPLEMENTATION.
// *************************************************************************** //

//  "launch_process"
//
bool PyStream::launch_process(void)
#ifdef PYSTREAM_REFACTOR
{
#ifdef _WIN32
    //      1.      WINDOWS : CREATE PIPES...
    SECURITY_ATTRIBUTES     sa              { sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };
    HANDLE                  stdin_r         = nullptr;
    HANDLE                  stdout_w        = nullptr;

    if ( !::CreatePipe(&stdin_r, &this->m_child_stdin_w, &sa, 0) )              { return false; }
    if ( !::CreatePipe(&this->m_child_stdout_r, &stdout_w, &sa, 0) )            { ::CloseHandle(stdin_r); ::CloseHandle(this->m_child_stdin_w); return false; }

    ::SetHandleInformation(this->m_child_stdin_w , HANDLE_FLAG_INHERIT, 0);
    ::SetHandleInformation(this->m_child_stdout_r, HANDLE_FLAG_INHERIT, 0);

    //      2.      BUILD COMMAND LINE (WIDE)...
    std::wstringstream      ss;
    ss  << L"python " << s_quote_if_needed(this->m_script_path.wstring());
    for (const auto & a : this->m_args) { ss << L" " << s_quote_if_needed(s_utf8_to_wide(a)); }
    std::wstring            cmd     = ss.str();

    //      3.      STARTUP INFO...
    STARTUPINFOW            si      {   };
    si.cb                           = sizeof(si);
    si.dwFlags                      = STARTF_USESTDHANDLES;
    si.hStdInput                    = stdin_r;
    si.hStdOutput                   = stdout_w;
    si.hStdError                    = stdout_w;

    BOOL                        ok  =
        ::CreateProcessW( nullptr, cmd.data(), nullptr, nullptr, TRUE, 0, nullptr, nullptr, &si, &this->m_proc_info );

    ::CloseHandle(stdin_r);
    ::CloseHandle(stdout_w);

    return ok == TRUE;

#else   //  POSIX  //

    int         in_pipe  [2]        = { -1, -1 };       //  parent writes   -> child reads
    int         out_pipe [2]        = { -1, -1 };       //  child writes    -> parent reads


    //      1.      CREATE PIPES  (USE <CLOEXEC>, IF AVAILABLE)...
# if defined(__linux__)  &&  defined(O_CLOEXEC)
//
    if ( ::pipe2(in_pipe , O_CLOEXEC) == -1 )           { return false; }
    if ( ::pipe2(out_pipe, O_CLOEXEC) == -1 )           { ::close(in_pipe[0]); ::close(in_pipe[1]); return false; }
//
# else
//
    if ( ::pipe(in_pipe)  == -1 )                        { return false; }
    if ( ::pipe(out_pipe) == -1 )                        { ::close(in_pipe[0]); ::close(in_pipe[1]); return false; }
    auto set_cloexec = [](int fd) {
        int flags = ::fcntl(fd, F_GETFD);
        if (flags != -1) { ::fcntl(fd, F_SETFD, flags | FD_CLOEXEC); }
    };
    set_cloexec(in_pipe [0]);   set_cloexec(in_pipe [1]);
    set_cloexec(out_pipe[0]);   set_cloexec(out_pipe[1]);
//
# endif  //  defined(__linux__)  &&  defined(0_CLOEXEC)  //


    //      2.      FORK...
    this->m_child_pid   = ::fork();
    if ( this->m_child_pid == -1 )
    {
        ::close(in_pipe[0]);    ::close(in_pipe[1]);
        ::close(out_pipe[0]);   ::close(out_pipe[1]);
        return false;
    }

    //      3.      CHILD...
    if ( m_child_pid == 0 )
    {
        std::vector<char*>      argv;
        argv    .reserve    ( 2 + this->m_args.size() + 1);
        argv    .push_back( const_cast<char*>("python3") );
        argv    .push_back( const_cast<char*>( m_script_path.c_str()) );
        
        
        //          3A.     REDIRECT STDIO.
        if ( ::dup2(in_pipe [0], STDIN_FILENO ) == -1 )     { _exit(1); }
        if ( ::dup2(out_pipe[1], STDOUT_FILENO) == -1 )     { _exit(1); }
        if ( ::dup2(out_pipe[1], STDERR_FILENO) == -1 )     { _exit(1); }


        //          3B.     CLOSE INHERITED FDs.
        ::close(in_pipe [0]);  ::close(in_pipe [1]);
        ::close(out_pipe[0]);  ::close(out_pipe[1]);


        //          3C.     ???.
        for (auto & a : m_args)     { argv.push_back(const_cast<char*>( a.c_str() )); }
        argv.push_back(nullptr);

        ::execvp( "python3", argv.data() );
        _exit(1);    // exec failed
    }

    //      4.      PARENT; CLOSE UNUSED ENDS, ADOPT OUR ENDS...
    ::close(in_pipe [0]);
    ::close(out_pipe[1]);

    this->m_child_stdin_fd      = in_pipe [1];
    this->m_child_stdout_fd     = out_pipe[0];

    return true;
//
//
# endif  //  _WIN32  //
}
/*
//
//
//
{
    int         in_pipe  [2]        = { -1, -1 };       //  parent writes   -> child reads
    int         out_pipe [2]        = { -1, -1 };       //  child writes    -> parent reads
    //
    auto        set_cloexec         = [](int fd) {
        int flags = ::fcntl(fd, F_GETFD);
        if (flags != -1) { ::fcntl(fd, F_SETFD, flags | FD_CLOEXEC); }
    };
    

    //      1.      CREATE PIPES WITH <CLOEXEC>, IF AVAILABLE...
#if defined(__linux__)  &&  defined(O_CLOEXEC)
    if ( ::pipe2(in_pipe , O_CLOEXEC) == -1 )   { return false; }
    if ( ::pipe2(out_pipe, O_CLOEXEC) == -1 )   { ::close(in_pipe[0]);  ::close(in_pipe[1]);  return false; }
#else
    if ( ::pipe(in_pipe)  == -1 )               { return false; }
    if ( ::pipe(out_pipe) == -1 )               { ::close(in_pipe[0]);  ::close(in_pipe[1]);  return false; }
    //
    set_cloexec(in_pipe [0]);   set_cloexec(in_pipe [1]);
    set_cloexec(out_pipe[0]);   set_cloexec(out_pipe[1]);
#endif  //  defined(__linux__)  && defined(0_CLOEXEC)  //


    //      2.      FORK...
    this->m_child_pid   = ::fork();
    if ( this->m_child_pid == -1 )
    {
        ::close(in_pipe[0]);    ::close(in_pipe[1]);
        ::close(out_pipe[0]);   ::close(out_pipe[1]);
        return false;
    }

    //      3.      CHILD...
    if ( m_child_pid == 0 )
    {
        std::vector<char*>      argv;
        argv    .reserve    ( 2 + this->m_args.size() + 1);
        argv    .push_back( const_cast<char*>("python3") );
        argv    .push_back( const_cast<char*>( m_script_path.c_str()) );
        
        
        //          3A.     REDIRECT STDIO.
        if ( ::dup2(in_pipe [0], STDIN_FILENO ) == -1 )     { _exit(1); }
        if ( ::dup2(out_pipe[1], STDOUT_FILENO) == -1 )     { _exit(1); }
        if ( ::dup2(out_pipe[1], STDERR_FILENO) == -1 )     { _exit(1); }


        //          3B.     CLOSE INHERITED FDs.
        ::close(in_pipe [0]);  ::close(in_pipe [1]);
        ::close(out_pipe[0]);  ::close(out_pipe[1]);


        //          3C.     ???.
        for (auto & a : m_args)     { argv.push_back(const_cast<char*>( a.c_str() )); }
        argv.push_back(nullptr);

        ::execvp( "python3", argv.data() );
        _exit(1);    // exec failed
    }

    //      4.      PARENT; CLOSE UNUSED ENDS, ADOPT OUR ENDS...
    ::close(in_pipe [0]);
    ::close(out_pipe[1]);

    this->m_child_stdin_fd      = in_pipe [1];
    this->m_child_stdout_fd     = out_pipe[0];

    return true;
}*/
//
#else
//
{
    SECURITY_ATTRIBUTES     sa          { sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };
    HANDLE                  stdin_r     = nullptr;
    HANDLE                  stdout_w    = nullptr;
    std::wstring            cmd;


    if ( !CreatePipe(&stdin_r           , &m_child_stdin_w  , &sa   , 0 ) )     { return false; }
    if ( !CreatePipe(&m_child_stdout_r  , &stdout_w         , &sa   , 0 ) )     { return false; }
    SetHandleInformation(m_child_stdin_w,  HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(m_child_stdout_r, HANDLE_FLAG_INHERIT, 0);

    {
        ss << s_quote_if_needed(this->m_python_exe.wstring())       // <- native wide path
           << L" "
           << s_quote_if_needed(this->m_script_path.wstring());
        for (const auto& a : m_args) {
            ss << L" " << s_quote_if_needed(s_utf8_to_wide(a));     // args remain UTF-8 -> wide
        }
        cmd = ss.str();
    }

    STARTUPINFOW    si          {   };                  //  value-initialises every byte to 0
    si.cb                       = sizeof(si);           //  required by CreateProcessW

    si.dwFlags                  = STARTF_USESTDHANDLES;
    si.hStdInput                = stdin_r;
    si.hStdOutput               = stdout_w;
    si.hStdError                = stdout_w;             //  merge stderr

    const bool      spawned     = CreateProcessW(nullptr, cmd.data(), nullptr, nullptr, TRUE, 0, nullptr, nullptr, &si, &m_proc_info);
    if (!spawned)               { return false; }

    CloseHandle(stdin_r);
    CloseHandle(stdout_w);
    return true;
}
#endif  //  PYSTREAM_REFACTOR  //






// *************************************************************************** //
//      2A. PROTECTED. |     PLATFORM-SPECIFIC      [[ WIN32 ]].
// *************************************************************************** //
#ifdef _WIN32


//  "write_pipe"
//
bool PyStream::write_pipe(const char * data, size_t n)
# ifdef PYSTREAM_REFACTOR
{
    const char *    p       = data;
    size_t          r       = n;
    DWORD           wrote   = 0;
    DWORD           err     = 0;

    if ( !this->m_child_stdin_w )       { return false; }


    //  WriteFile can legally short-write; loop until done.
    while (r > 0)
    {
        wrote                   = 0;
        const bool      valid   = WriteFile(m_child_stdin_w, p, (DWORD)std::min<size_t>(r, 0x7fffffff), &wrote, nullptr);
        
        if ( !valid )
        {
            err = GetLastError();
            
            if ( err == ERROR_BROKEN_PIPE )     { return false; }       //  Clean shutdown: ERROR_BROKEN_PIPE when child closed stdin
            
            return false;   //  treat other errors as failure; no retries (for simplicity).
        }
        if (wrote == 0)     { return false; }   // shouldn’t happen; treat as failure
        p += wrote;
        r -= wrote;
    }
    return true;
}
# else
{
    DWORD written;
    return WriteFile(m_child_stdin_w, data, (DWORD)n, &written, nullptr) && written == n;
}
# endif  //  PYSTREAM_REFACTOR  //


//
// *************************************************************************** //
// *************************************************************************** //   END "2A-1.  WIN32".



// *************************************************************************** //
//      2A. PROTECTED. |     PLATFORM-SPECIFIC      [[ POSIX ]].
// *************************************************************************** //
#else


//  "write_fd"
//
bool PyStream::write_fd(int fd, const char * data, size_t n)
#ifdef PYSTREAM_REFACTOR
{
    const char *    p       = data;
    size_t          r       = n;
    ssize_t         w       = 0;
    
    
    if (fd < 0)     { return false; }


    while (r > 0)
    {
        w   = ::write(fd, p, r);
        
        if (w > 0)      { p += w;  r -= (size_t)w;  continue; }

        if (w == -1)
        {
            if (errno == EINTR)     { continue; }   //  interrupted, retry immediately.
            
    # ifdef EWOULDBLOCK
            if (errno == EAGAIN)
    # else
            if ( (errno == EAGAIN)  ||  (errno == EWOULDBLOCK) )
    # endif  //  EWOULDBLOCK  //
            {
                continue;                           //    pipe temporarily full; retry
            }
            
            return false;       //  EPIPE (child closed) or other hard error
        }
        return false;       //  w == 0: unusual for pipes, so treat as failure...
    }
    return true;
}
//
#else
//
{
    while (n > 0)
    {
        ssize_t     w   = ::write(fd, data, n);
        if ( w <= 0 )   { return false; }
        data           += w;
        n              -= w;
    }
    return true;
}
#endif  //  PYSTREAM_REFACTOR  //
  

//
// *************************************************************************** //
// *************************************************************************** //   END "2A-2.  POSIX".
#endif  // _WIN32  //



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 2.  "PROTECTED" ]].












// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" :: "utl" NAMESPACE.






// *************************************************************************** //
// *************************************************************************** //
//
//  END.

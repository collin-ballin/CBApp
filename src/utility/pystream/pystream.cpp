/***********************************************************************************
*
*       ********************************************************************
*       ****           P Y S T R E A M . C P P  ____  F I L E           ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond.
*               DATED:      May 09, 2025.
*
*       ********************************************************************
*                file:      pystream.cpp
*        last_updated:      2026-03-18
*             version:      1.6.1.
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
//      Windows argv-style quoting for CreateProcessW() command-line construction.
//          - Leaves strings unquoted if they contain no whitespace and no quotes.
//          - If quoting is required, escapes embedded quotes and preserves backslashes
//            according to the standard Windows parsing rules (backslashes before quotes
//            and trailing backslashes must be doubled).
//
static inline std::wstring s_quote_if_needed(const std::wstring & s)
{
    std::wstring    out                 {  };
    size_t          num_backslashes     = 0ULL;
    
    
    //      CASE 0 :    EMPTY ARGUMENT --- MUST REMAIN A DISTINCT EMPTY TOKEN...
    if ( s.empty() )            { return L"\"\""; }

    
    //      CASE 1 :    NO QUOTES NEEDED --- RETURN THE STRING AS-IS...
    const bool      needs_quotes        = ( s.find_first_of(L" \t\n\v\"") != std::wstring::npos );  //  (No whitespace and no embedded quotes)
    if (!needs_quotes)          { return s; }


    //      CASE 2 :    QUOTE REQUIRED.
    //                  Wrap in quotes and apply escaping rules:
    //                    * Backslashes are accumulated.
    //                    * On a literal quote, emit: (2*N + 1) backslashes + quote.
    //                    * On any other char, emit pending backslashes + char.
    out.reserve(s.size() + 2);
    out.push_back(L'"');
    for (wchar_t ch : s)
    {
        //      CASE 2.1 :    ACCUMULATE BACKSLASHES...
        if (ch == L'\\') {
            ++num_backslashes;
            continue;
        }
        //      CASE 2.2 :    ESCAPE A LITERAL QUOTE...
        if (ch == L'"') {
            out.append(num_backslashes * 2 + 1, L'\\');
            out.push_back(L'"');
            num_backslashes = 0;
            continue;
        }
        //      CASE 2.3 :    EMIT PENDING BACKSLASHES THEN THE CHAR...
        if (num_backslashes > 0)    { out.append(num_backslashes, L'\\'); num_backslashes = 0; }
        out.push_back(ch);
    }
    //
    //      CASE 2.4 :    TRAILING BACKSLASHES BEFORE CLOSING QUOTE MUST BE DOUBLED...
    if (num_backslashes > 0)    { out.append(num_backslashes * 2, L'\\'); }
    out.push_back(L'"');
    
    return out;
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
    : m_script_path( process::make_path_from_utf8(script_path) )
{
    this->_set_args(args);
}



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
#ifndef _PYSTREAM_FORCE_UNBUFFERED_PYTHON
// *************************************************************************** //
//
//
[[nodiscard]] uint32_t PyStream::start(void)
{
    bool    expected    = false;
    
    
    //      CASE 0 :    ATTEMPT TO RUN A PROCESS WITHOUT SPECIFYING IT'S FILEPATH
    //                  *OR*  ATTEMPT TO "start()" WHILE ALREADY RUNNING...
    if ( this->m_script_path.empty() )                                  { throw std::logic_error("PyStream::start: no script path set");    }
    if ( this->m_running.load() )                                       { throw std::logic_error("PyStream::start: already running");       }

    //      Re-validate that the script file still exists (in case the file was removed since set-time).
    (void)PyStream::_validate_filepath(this->m_script_path, "PyStream::start", false);
    
    
#ifdef _WIN32
    if ( this->m_reader_thread.joinable() || this->m_proc_info.hProcess || this->m_proc_info.hThread || this->m_child_stdin_w || this->m_child_stdout_r || this->m_child_stderr_r )
#else
    if ( this->m_reader_thread.joinable() || (this->m_child_pid > 0) || (this->m_child_stdin_fd != -1) || (this->m_child_stdout_fd != -1) || (this->m_child_stderr_fd != -1) )
#endif
    {
        this->stop();
    }
    
    
    this->m_last_exit_code.store(INT_MIN);
#ifndef _WIN32
    this->m_last_term_sig.store(0);
#endif
    
    
    {
        std::lock_guard<std::mutex>     lock    (this->m_queue_mutex);
        this->m_recv_queue.clear();
    }
    this->m_dropped_lines.store(0);
    
    
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

#ifdef _WIN32
    //      1B.     [ _WIN32 ] :    DEFINITIVE CHECK: DID THE CHILD EXIT IMMEDIATELY?
    //                  This catches "spawn succeeded but python/script immediately failed" cases.
    if ( this->m_proc_info.hProcess )
    {
        DWORD   code    = 0;
        if ( ::GetExitCodeProcess(this->m_proc_info.hProcess, &code) == TRUE )
        {
            if ( code != STILL_ACTIVE )
            {
                this->m_last_exit_code.store((int)code);
                this->m_running.store(false);
                
                //  Ensure resources are cleaned up before throwing.
                this->stop();

                throw std::runtime_error(
                    std::string("PyStream::start: child exited immediately (exit_code=")
                    + std::to_string((int)code)
                    + std::string(") | invocation: ")
                    + this->get_invocation()
                );
            }
        }
    }
#endif  //  _WIN32  //



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
        if (this->m_child_stderr_r)         { ::CloseHandle(this->m_child_stderr_r);  this->m_child_stderr_r = nullptr; }
    # else
        if (this->m_child_pid > 0)          { ::kill(this->m_child_pid, SIGTERM);  ::waitpid(this->m_child_pid, nullptr, 0);  this->m_child_pid = -1; }
        if (this->m_child_stdin_fd  != -1)  { ::close(this->m_child_stdin_fd ); this->m_child_stdin_fd  = -1; }
        if (this->m_child_stdout_fd != -1)  { ::close(this->m_child_stdout_fd); this->m_child_stdout_fd = -1; }
        if (this->m_child_stderr_fd != -1)  { ::close(this->m_child_stderr_fd); this->m_child_stderr_fd = -1; }
    # endif  //  _WIN32  //
    
        this->m_running.store(false);
        throw; // rethrow original thread exception
    }

    return this->get_pid();   // success → PID snapshot
}
//
//
// *************************************************************************** //
# else      //  ( #ifndef _PYSTREAM_FORCE_UNBUFFERED_PYTHON )
// *************************************************************************** //
//
//
[[nodiscard]] uint32_t PyStream::start(void)
{
    bool    expected    = false;
    
    
    //      CASE 0 :    ATTEMPT TO RUN A PROCESS WITHOUT SPECIFYING IT'S FILEPATH
    //                  *OR*  ATTEMPT TO "start()" WHILE ALREADY RUNNING...
    if ( this->m_script_path.empty() )                                  { throw std::logic_error("PyStream::start: no script path set");    }
    if ( this->m_running.load() )                                       { throw std::logic_error("PyStream::start: already running");       }

    //      Re-validate that the script file still exists (in case the file was removed since set-time).
    (void)PyStream::_validate_filepath(this->m_script_path, "PyStream::start", false);
    
    
#ifdef _WIN32
    if ( this->m_reader_thread.joinable() || this->m_proc_info.hProcess || this->m_proc_info.hThread || this->m_child_stdin_w || this->m_child_stdout_r || this->m_child_stderr_r )
#else
    if ( this->m_reader_thread.joinable() || (this->m_child_pid > 0) || (this->m_child_stdin_fd != -1) || (this->m_child_stdout_fd != -1) || (this->m_child_stderr_fd != -1) )
#endif
    {
        this->stop();
    }
    
    
    this->m_last_exit_code.store(INT_MIN);
#ifndef _WIN32
    this->m_last_term_sig.store(0);
#endif
    
    
    {
        std::lock_guard<std::mutex>     lock    (this->m_queue_mutex);
        this->m_recv_queue.clear();
    }
    this->m_dropped_lines.store(0);
    
    
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

#ifdef _WIN32
    //      1B.     [ _WIN32 ] :    DEFINITIVE CHECK: DID THE CHILD EXIT IMMEDIATELY?
    //                  This catches "spawn succeeded but python/script immediately failed" cases.
    if ( this->m_proc_info.hProcess )
    {
        DWORD   code    = 0;
        if ( ::GetExitCodeProcess(this->m_proc_info.hProcess, &code) == TRUE )
        {
            if ( code != STILL_ACTIVE )
            {
                this->m_last_exit_code.store((int)code);
                this->m_running.store(false);
                
                //  Ensure resources are cleaned up before throwing.
                this->stop();

                throw std::runtime_error(
                    std::string("PyStream::start: child exited immediately (exit_code=")
                    + std::to_string((int)code)
                    + std::string(") | invocation: ")
                    + this->get_invocation()
                );
            }
        }
    }
#endif  //  _WIN32  //



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
        if (this->m_child_stderr_r)         { ::CloseHandle(this->m_child_stderr_r);  this->m_child_stderr_r = nullptr; }
    # else
        if (this->m_child_pid > 0)          { ::kill(this->m_child_pid, SIGTERM);  ::waitpid(this->m_child_pid, nullptr, 0);  this->m_child_pid = -1; }
        if (this->m_child_stdin_fd  != -1)  { ::close(this->m_child_stdin_fd ); this->m_child_stdin_fd  = -1; }
        if (this->m_child_stdout_fd != -1)  { ::close(this->m_child_stdout_fd); this->m_child_stdout_fd = -1; }
        if (this->m_child_stderr_fd != -1)  { ::close(this->m_child_stderr_fd); this->m_child_stderr_fd = -1; }
    # endif  //  _WIN32  //
    
        this->m_running.store(false);
        throw; // rethrow original thread exception
    }

    return this->get_pid();   // success → PID snapshot
}
//
//
// *************************************************************************** //
#endif  //  _PYSTREAM_FORCE_UNBUFFERED_PYTHON   //






//  "try_start"
//
[[nodiscard]] std::optional<uint32_t> PyStream::try_start(void) noexcept {
    try             { return this->start();     }
    catch (...)     { return std::nullopt;      }
}



//  "stop"
//
void PyStream::stop(void)
{
    if ( !this->m_running.exchange(false) )     { /* already stopped */ }       //  already stopped

#ifdef _WIN32
    //      1.      [ _WIN32 ] :    UNBLOCK ANY BLOCKED "ReadFile" IN THE READER-THREAD (IF ANY)...
    if ( this->m_reader_thread.joinable() ) {
        if (this->m_child_stdout_r)     { CloseHandle(this->m_child_stdout_r);  this->m_child_stdout_r = nullptr; }
        if (this->m_child_stderr_r)     { CloseHandle(this->m_child_stderr_r);  this->m_child_stderr_r = nullptr; }
        this->m_reader_thread.join();                                              //  ** ensure reader is done before touching other handles **
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
    this->m_proc_info = { nullptr, nullptr, 0U, 0U };
    //  this->m_last_term_sig.store(0);     //  ** DO NOT UN-COMMENT **     (No m_last_term_sig on Windows).


    //      3.      [ _WIN32 ] :    CLOSE OUR PIPE-ENDS (idempotent)...
    {
        std::lock_guard<std::mutex>         guard           (this->m_write_mutex);
        if (this->m_child_stdin_w)      { CloseHandle(this->m_child_stdin_w);   this->m_child_stdin_w  = nullptr; }
    }
    if (this->m_child_stdout_r)     { CloseHandle(this->m_child_stdout_r);  this->m_child_stdout_r = nullptr; }
    if (this->m_child_stderr_r)     { CloseHandle(this->m_child_stderr_r);  this->m_child_stderr_r = nullptr; }
//
//
# else
//
//
    //      1.      [ POSIX ] :     TERMINATE & COLLECT EXIT-STATUS...
    if ( this->m_child_pid > 0 )
    {
        int     status      = 0;
        pid_t   r           = -1;
        bool    got_status  = false;


        //      1A.     PRE-CHECK: avoid signaling a PID that may have already been reaped elsewhere.
        do {
            r = ::waitpid(this->m_child_pid, &status, WNOHANG);
        } while (r == -1 && errno == EINTR);

        if (r == this->m_child_pid) {
            got_status = true;
        }
        else if (r == -1 && errno == ECHILD) {
            this->m_child_pid = -1;
        }


        if ( !got_status && this->m_child_pid > 0 )
        {
            ::kill(this->m_child_pid, SIGTERM);

            //      Bounded wait (avoid indefinite freeze).  Escalate to SIGKILL on timeout.
            {
                constexpr int   TIMEOUT_MS  = 5000;
                constexpr int   POLL_MS     = 10;
                int             waited_ms   = 0;

                while (true)
                {
                    r = ::waitpid(this->m_child_pid, &status, WNOHANG);

                    if (r == this->m_child_pid) { got_status = true; break; }     //  reaped
                    if (r == 0)
                    {
                        if (waited_ms >= TIMEOUT_MS)
                        {
                            ::kill(this->m_child_pid, SIGKILL);
                            do { r = ::waitpid(this->m_child_pid, &status, 0); } while (r == -1 && errno == EINTR);
                            if (r == this->m_child_pid) { got_status = true; }
                            break;
                        }

                        ::usleep(POLL_MS * 1000);
                        waited_ms += POLL_MS;
                        continue;
                    }

                    if ( r == -1 && errno == EINTR ) { continue; }

                    //  r == -1 : error (best-effort; treat as unknown termination)
                    ::kill(this->m_child_pid, SIGKILL);
                    do { r = ::waitpid(this->m_child_pid, &status, 0); } while (r == -1 && errno == EINTR);
                    if (r == this->m_child_pid) { got_status = true; }
                    break;
                }
            }
        }

        if ( got_status )
        {
            if      ( WIFEXITED(status) )           { this->m_last_exit_code.store(WEXITSTATUS(status))     ; this->m_last_term_sig.store(0);                   }
            else if ( WIFSIGNALED(status) )         { this->m_last_exit_code.store(-1)                      ; this->m_last_term_sig.store(WTERMSIG(status));    }
            else                                    { this->m_last_exit_code.store(-1)                      ; this->m_last_term_sig.store(0);                   }
        }
        else
        {
            this->m_last_exit_code.store(-1);
            this->m_last_term_sig.store(0);
        }

        this->m_child_pid = -1;
    }

    //      1B.     [ POSIX ] :     JOIN READER THREAD BEFORE CLOSING FDs...
    if ( this->m_reader_thread.joinable() )     { this->m_reader_thread.join(); }
    
    //      2.      [ POSIX ] :     CLOSE OUR PIPE-ENDS (idempotent)...
    {
        std::lock_guard<std::mutex>         guard           (this->m_write_mutex);
        if ( this->m_child_stdin_fd  != -1 )        { ::close( this->m_child_stdin_fd  );   this->m_child_stdin_fd  = -1; }
    }
    if ( this->m_child_stdout_fd != -1 )        { ::close( this->m_child_stdout_fd );   this->m_child_stdout_fd = -1; }
    if ( this->m_child_stderr_fd != -1 )        { ::close( this->m_child_stderr_fd );   this->m_child_stderr_fd = -1; }
//
//
#endif  //  _WIN32  //

    //      4.      JOIN READER & CLEAR QUEUE...
    if ( this->m_reader_thread.joinable() )     { this->m_reader_thread.join(); }

    {
        std::lock_guard<std::mutex>     lock    (this->m_queue_mutex);
        this->m_recv_queue.clear();
    }
    {
        std::lock_guard<std::mutex>     lock    (this->m_err_queue_mutex);
        this->m_err_queue.clear();
    }
    this->m_dropped_err_lines.store(0);
    return;
}











//  "send"
//
bool PyStream::send(const std::string & msg)
{
    std::string                         line            = msg;
    std::lock_guard<std::mutex>         guard           (this->m_write_mutex);
    
    if ( line.find('\0') != std::string::npos )         { return false; }

    if ( !m_running.load() )                            { return false; }

    // Normalize line endings: \r\n and \r become \n
    for (size_t i = 0; i < line.size(); ++i)
    {
        if (line[i] == '\r')
        {
            line[i] = '\n';
            if (i + 1 < line.size() && line[i+1] == '\n') {
                line.erase(i + 1, 1);
            }
        }
    }

    if ( line.empty()  ||  (line.back() != '\n') )      { line.push_back('\n'); }


# ifdef _WIN32
    return this->write_pipe(line.data(), line.size());
# else
    return write_fd(this->m_child_stdin_fd, line.data(), line.size());
# endif  //  _WIN32  //
}



//  "try_receive"
//      try_receive (non-blocking)
//
bool PyStream::try_receive(std::string & out)
{
    std::lock_guard<std::mutex>     lock    (m_queue_mutex);
    if ( m_recv_queue.empty() )     { return false; }
    
    out.swap(m_recv_queue.front());
    m_recv_queue    .pop_front();
    return true;
}


//  "try_receive_err"
//      try_receive_err (non-blocking) for stderr
//
bool PyStream::try_receive_err(std::string & out)
{
    std::lock_guard<std::mutex>     lock    (m_err_queue_mutex);
    if ( m_err_queue.empty() )     { return false; }
    
    out.swap(m_err_queue.front());
    m_err_queue    .pop_front();
    return true;
}




//  "reader_thread_func"
//      single reader thread — waits on both stdout and stderr using platform-native waiting
//
void PyStream::reader_thread_func(void)
{
    constexpr size_t            BSIZE       = PyStream::ms_READ_BUFFER_SIZE;
    static thread_local char    s_buffer    [BSIZE];

#ifdef _WIN32
    HANDLE                      wait_handles[2];
    DWORD                       n           = 0;
#else
    fd_set                      readfds;
    int                         max_fd      = -1;
    ssize_t                     n           = 0;
#endif

    std::string                 line        = {   };
    char                        ch          = '\0';
    line.reserve(BSIZE);


#ifdef _WIN32
    while ( this->m_running.load() )
    {
        wait_handles[0] = this->m_child_stdout_r;
        wait_handles[1] = this->m_child_stderr_r;

        const DWORD wait_rc = ::WaitForMultipleObjects(2, wait_handles, FALSE, INFINITE);

        if ( wait_rc == WAIT_FAILED ) {
            break;
        }

        HANDLE ready_handle = (wait_rc == WAIT_OBJECT_0) ? this->m_child_stdout_r : this->m_child_stderr_r;

        if ( !ready_handle ) { break; }

        const BOOL ok = ::ReadFile(ready_handle, s_buffer, BSIZE, &n, nullptr);
        if ( !ok || n == 0 ) { break; }

        for (DWORD i = 0; i < n; ++i)
        {
            ch = s_buffer[i];
            switch (ch)
            {
                case '\r': continue;
                case '\n':
                    if (ready_handle == this->m_child_stdout_r) {
                        enqueue_line_(std::move(line));
                    } else {
                        enqueue_err_line_(std::move(line));
                    }
                    line.clear();
                    break;
                default:
                    line.push_back(ch);
                    break;
            }
        }
    }
#else
    for (;;)
    {
        FD_ZERO(&readfds);
        if (this->m_child_stdout_fd != -1)  { FD_SET(this->m_child_stdout_fd, &readfds); max_fd = std::max(max_fd, this->m_child_stdout_fd); }
        if (this->m_child_stderr_fd != -1)  { FD_SET(this->m_child_stderr_fd, &readfds); max_fd = std::max(max_fd, this->m_child_stderr_fd); }

        if (max_fd == -1) break;

        const int rc = ::select(max_fd + 1, &readfds, nullptr, nullptr, nullptr);
        if (rc == -1) {
            if (errno == EINTR) continue;
            break;
        }
        if (rc == 0) continue;

        // stdout ready?
        if (this->m_child_stdout_fd != -1 && FD_ISSET(this->m_child_stdout_fd, &readfds))
        {
            n = ::read(this->m_child_stdout_fd, s_buffer, BSIZE);
            if (n <= 0) { if (n == 0) break; if (errno != EINTR) break; continue; }

            for (ssize_t i = 0; i < n; ++i)
            {
                ch = s_buffer[i];
                switch (ch)
                {
                    case '\r': continue;
                    case '\n': enqueue_line_(std::move(line)); line.clear(); break;
                    default:   line.push_back(ch); break;
                }
            }
        }

        // stderr ready?
        if (this->m_child_stderr_fd != -1 && FD_ISSET(this->m_child_stderr_fd, &readfds))
        {
            n = ::read(this->m_child_stderr_fd, s_buffer, BSIZE);
            if (n <= 0) { if (n == 0) break; if (errno != EINTR) break; continue; }

            for (ssize_t i = 0; i < n; ++i)
            {
                ch = s_buffer[i];
                switch (ch)
                {
                    case '\r': continue;
                    case '\n': enqueue_err_line_(std::move(line)); line.clear(); break;
                    default:   line.push_back(ch); break;
                }
            }
        }
    }
#endif

    if ( !line.empty() )
    {
        enqueue_line_(std::move(line));
    }

    this->m_running.store(false);
    return;
}



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
#ifndef _PYSTREAM_FORCE_UNBUFFERED_PYTHON
// *************************************************************************** //
//
//
bool PyStream::launch_process(void)
{
#ifdef _WIN32
    //      1.      WINDOWS : CREATE PIPES...
    SECURITY_ATTRIBUTES     sa              { sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };
    HANDLE                  stdin_r         = nullptr;
    HANDLE                  stdout_w        = nullptr;
    HANDLE                  stderr_w        = nullptr;

    this->m_proc_info = { nullptr, nullptr, 0U, 0U };

    if ( !::CreatePipe(&stdin_r, &this->m_child_stdin_w, &sa, 0) )              { this->m_child_stdin_w = nullptr; return false; }
    if ( !::CreatePipe(&this->m_child_stdout_r, &stdout_w, &sa, 0) )            { ::CloseHandle(stdin_r); ::CloseHandle(this->m_child_stdin_w); this->m_child_stdin_w = nullptr; this->m_child_stdout_r = nullptr; return false; }
    if ( !::CreatePipe(&this->m_child_stderr_r, &stderr_w, &sa, 0) )            { ::CloseHandle(stdin_r); ::CloseHandle(this->m_child_stdin_w); ::CloseHandle(stdout_w); this->m_child_stdin_w = nullptr; this->m_child_stdout_r = nullptr; return false; }

    ::SetHandleInformation(this->m_child_stdin_w , HANDLE_FLAG_INHERIT, 0);
    ::SetHandleInformation(this->m_child_stdout_r, HANDLE_FLAG_INHERIT, 0);
    ::SetHandleInformation(this->m_child_stderr_r, HANDLE_FLAG_INHERIT, 0);


    //      2.      BUILD COMMAND LINE (WIDE)...
    std::wstringstream      ss;
    ss  << s_quote_if_needed(this->m_python_exe.wstring()) << L" " << s_quote_if_needed(this->m_script_path.wstring());
    for (const auto & a : this->m_args) {
        ss << L" " << s_quote_if_needed(s_utf8_to_wide(a));
    }
    std::wstring            cmd             = ss.str();
    std::wstring            cwd             = this->m_cwd.wstring();


    //      3.      STARTUP INFO...
    STARTUPINFOW            si              {   };
    si.cb                                   = sizeof(si);
    si.dwFlags                              = STARTF_USESTDHANDLES;
    si.hStdInput                            = stdin_r;
    si.hStdOutput                           = stdout_w;
    si.hStdError                            = stderr_w;

    const bool              is_bare_name    = ( this->m_python_exe.parent_path().empty()  &&  !this->m_python_exe.has_root_path() );
    const wchar_t *         app_name        = is_bare_name  ? nullptr   : this->m_python_exe.c_str();
    BOOL                    ok              = ::CreateProcessW(
          app_name
        , cmd.data()
        , nullptr
        , nullptr
        , TRUE
        , 0
        , nullptr
        , cwd.empty()   ? nullptr   : cwd.c_str()
        , &si
        , &this->m_proc_info
     );

    ::CloseHandle(stdin_r);
    ::CloseHandle(stdout_w);
    ::CloseHandle(stderr_w);

    if ( ok != TRUE )
    {
        if ( this->m_child_stdin_w )        { ::CloseHandle(this->m_child_stdin_w );  this->m_child_stdin_w  = nullptr; }
        if ( this->m_child_stdout_r )       { ::CloseHandle(this->m_child_stdout_r);  this->m_child_stdout_r = nullptr; }
        if ( this->m_child_stderr_r )       { ::CloseHandle(this->m_child_stderr_r);  this->m_child_stderr_r = nullptr; }
        this->m_proc_info = { nullptr, nullptr, 0U, 0U };
    }

    return ok == TRUE;
//
//
#else   //  POSIX  //
//
//
    int         in_pipe      [2]        = { -1, -1 };       //  parent writes   -> child reads
    int         out_pipe     [2]        = { -1, -1 };       //  child writes    -> parent reads
    int         err_pipe     [2]        = { -1, -1 };       //  child writes    -> parent reads (stderr)
    int         exec_pipe    [2]        = { -1, -1 };       //  child signals   -> parent detects exec/chdir failure


    //      1.      CREATE PIPES  (USE <CLOEXEC>, IF AVAILABLE)...
# if defined(__linux__)  &&  defined(O_CLOEXEC)
//
    if ( ::pipe2(in_pipe , O_CLOEXEC) == -1 )           { return false; }
    if ( ::pipe2(out_pipe, O_CLOEXEC) == -1 )           { ::close(in_pipe[0]); ::close(in_pipe[1]); return false; }
    if ( ::pipe2(err_pipe , O_CLOEXEC) == -1 )          { ::close(in_pipe[0]); ::close(in_pipe[1]); ::close(out_pipe[0]); ::close(out_pipe[1]); return false; }
    if ( ::pipe2(exec_pipe, O_CLOEXEC) == -1 )          { ::close(in_pipe[0]); ::close(in_pipe[1]); ::close(out_pipe[0]); ::close(out_pipe[1]); ::close(err_pipe[0]); ::close(err_pipe[1]); return false; }
//
# else
//
    if ( ::pipe(in_pipe)  == -1 )                       { return false; }
    if ( ::pipe(out_pipe) == -1 )                       { ::close(in_pipe[0]); ::close(in_pipe[1]); return false; }
    if ( ::pipe(err_pipe) == -1 )                       { ::close(in_pipe[0]); ::close(in_pipe[1]); ::close(out_pipe[0]); ::close(out_pipe[1]); return false; }
    if ( ::pipe(exec_pipe) == -1 )                      { ::close(in_pipe[0]); ::close(in_pipe[1]); ::close(out_pipe[0]); ::close(out_pipe[1]); ::close(err_pipe[0]); ::close(err_pipe[1]); return false; }
    auto set_cloexec = [](int fd) {
        int flags = ::fcntl(fd, F_GETFD);
        if (flags != -1) { ::fcntl(fd, F_SETFD, flags | FD_CLOEXEC); }
    };
    set_cloexec(in_pipe [0]);   set_cloexec(in_pipe [1]);
    set_cloexec(out_pipe[0]);   set_cloexec(out_pipe[1]);
    set_cloexec(err_pipe [0]);  set_cloexec(err_pipe [1]);
    set_cloexec(exec_pipe[0]);  set_cloexec(exec_pipe[1]);
//
# endif  //  defined(__linux__)  &&  defined(O_CLOEXEC)  //


    //      2.      FORK...
    this->m_child_pid   = ::fork();
    if ( this->m_child_pid == -1 )
    {
        ::close(in_pipe[0]);    ::close(in_pipe[1]);
        ::close(out_pipe[0]);   ::close(out_pipe[1]);
        ::close(err_pipe[0]);   ::close(err_pipe[1]);
        ::close(exec_pipe[0]);  ::close(exec_pipe[1]);
        return false;
    }

    //      3.      CHILD...
    if ( m_child_pid == 0 )
    {
        std::vector<char*>      argv;
        argv    .reserve    ( 2 + this->m_args.size() + 1);
        argv    .push_back( const_cast<char*>( this->m_python_exe.c_str() ) );
        argv    .push_back( const_cast<char*>( m_script_path.c_str()) );
        
        
        //          3A.     REDIRECT STDIO.
        if ( ::dup2(in_pipe [0], STDIN_FILENO ) == -1 )     { char c = 1; ::write(exec_pipe[1], &c, 1); _exit(1); }
        if ( ::dup2(out_pipe[1], STDOUT_FILENO) == -1 )     { char c = 1; ::write(exec_pipe[1], &c, 1); _exit(1); }
        if ( ::dup2(err_pipe [1], STDERR_FILENO) == -1 )    { char c = 1; ::write(exec_pipe[1], &c, 1); _exit(1); }


        //          3B.     CLOSE INHERITED FDs.
        ::close(in_pipe [0]);  ::close(in_pipe [1]);
        ::close(out_pipe[0]);  ::close(out_pipe[1]);
        ::close(err_pipe [0]); ::close(err_pipe [1]);
        ::close(exec_pipe[0]);


        //          3C.     ???.
        if ( !this->m_cwd.empty() ) { if ( ::chdir(this->m_cwd.c_str()) != 0 ) { char c = 1; ::write(exec_pipe[1], &c, 1); _exit(1); } }
        for (auto & a : m_args)     { argv.push_back(const_cast<char*>( a.c_str() )); }
        argv.push_back(nullptr);

        ::execvp( this->m_python_exe.c_str(), argv.data() );
        { char c = 1; ::write(exec_pipe[1], &c, 1); _exit(1); }    // exec failed
    }

    //      4.      PARENT; CLOSE UNUSED ENDS, ADOPT OUR ENDS...
    ::close(in_pipe [0]);
    ::close(out_pipe[1]);
    ::close(err_pipe [1]);
    ::close(exec_pipe[1]);

    //      4A.     WAIT FOR CHILD TO EITHER EXEC (EOF) OR REPORT FAILURE (byte)...
    {
        char        c       = 0;
        ssize_t     r       = 0;

        do {
            r = ::read(exec_pipe[0], &c, 1);
        } while (r == -1 && errno == EINTR);

        ::close(exec_pipe[0]);

        if ( r > 0 )
        {
            int status = 0;
            ::waitpid(this->m_child_pid, &status, 0);
            this->m_child_pid = -1;

            ::close(in_pipe [1]);
            ::close(out_pipe[0]);
            ::close(err_pipe [0]);

            return false;
        }
    }

    this->m_child_stdin_fd      = in_pipe [1];
    this->m_child_stdout_fd     = out_pipe[0];
    this->m_child_stderr_fd     = err_pipe [0];

# ifdef F_SETNOSIGPIPE
    ::fcntl(this->m_child_stdin_fd, F_SETNOSIGPIPE, 1);
# endif  //  F_SETNOSIGPIPE  //

    return true;
//
//
# endif  //  _WIN32  //
}
//
//
// *************************************************************************** //
# else      //  ( #ifndef _PYSTREAM_FORCE_UNBUFFERED_PYTHON )
// *************************************************************************** //
//
//
//
bool PyStream::launch_process(void)
{
#ifdef _WIN32
    //      1.      WINDOWS : CREATE PIPES...
    SECURITY_ATTRIBUTES     sa              { sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };
    HANDLE                  stdin_r         = nullptr;
    HANDLE                  stdout_w        = nullptr;
    HANDLE                  stderr_w        = nullptr;

    //      Defensive: reset proc_info before CreateProcessW writes into it.
    this->m_proc_info = { nullptr, nullptr, 0U, 0U };

    if ( !::CreatePipe(&stdin_r, &this->m_child_stdin_w, &sa, 0) )              { this->m_child_stdin_w = nullptr; return false; }
    if ( !::CreatePipe(&this->m_child_stdout_r, &stdout_w, &sa, 0) )            { ::CloseHandle(stdin_r); ::CloseHandle(this->m_child_stdin_w); this->m_child_stdin_w = nullptr; this->m_child_stdout_r = nullptr; return false; }
    if ( !::CreatePipe(&this->m_child_stderr_r, &stderr_w, &sa, 0) )            { ::CloseHandle(stdin_r); ::CloseHandle(this->m_child_stdin_w); ::CloseHandle(stdout_w); this->m_child_stdin_w = nullptr; this->m_child_stdout_r = nullptr; return false; }

    ::SetHandleInformation(this->m_child_stdin_w , HANDLE_FLAG_INHERIT, 0);
    ::SetHandleInformation(this->m_child_stdout_r, HANDLE_FLAG_INHERIT, 0);
    ::SetHandleInformation(this->m_child_stderr_r, HANDLE_FLAG_INHERIT, 0);

    //      2.      BUILD COMMAND LINE (WIDE)...
    //
    //              NOTE:
    //              * We run Python with "-u" to force unbuffered stdio when redirected to pipes.
    //                This is critical for "concurrent" streaming behavior on Windows.
    std::wstringstream      ss;
    ss  << s_quote_if_needed(this->m_python_exe.wstring())
        << L" -u "
        << s_quote_if_needed(this->m_script_path.wstring());
    for (const auto & a : this->m_args) { ss << L" " << s_quote_if_needed(s_utf8_to_wide(a)); }
    std::wstring            cmd     = ss.str();
    std::wstring            cwd     = this->m_cwd.wstring();

    //      3.      STARTUP INFO...
    STARTUPINFOW            si      {   };
    si.cb                           = sizeof(si);
    si.dwFlags                      = STARTF_USESTDHANDLES;
    si.hStdInput                    = stdin_r;
    si.hStdOutput                   = stdout_w;
    si.hStdError                    = stderr_w;

    //      4.      CREATE PROCESS...
    const bool              is_bare_name    = (this->m_python_exe.parent_path().empty() && !this->m_python_exe.has_root_path());
    const wchar_t *         app_name        = is_bare_name ? nullptr : this->m_python_exe.c_str();

    BOOL                    ok              =
        ::CreateProcessW( app_name, cmd.data(), nullptr, nullptr, TRUE, 0, nullptr, cwd.empty() ? nullptr : cwd.c_str(), &si, &this->m_proc_info );

    ::CloseHandle(stdin_r);
    ::CloseHandle(stdout_w);
    ::CloseHandle(stderr_w);

    if ( ok != TRUE )
    {
        if ( this->m_child_stdin_w )        { ::CloseHandle(this->m_child_stdin_w );  this->m_child_stdin_w  = nullptr; }
        if ( this->m_child_stdout_r )       { ::CloseHandle(this->m_child_stdout_r);  this->m_child_stdout_r = nullptr; }
        if ( this->m_child_stderr_r )       { ::CloseHandle(this->m_child_stderr_r);  this->m_child_stderr_r = nullptr; }
        this->m_proc_info = { nullptr, nullptr, 0U, 0U };
    }

    return ok == TRUE;

#else   //  POSIX  //

    int         in_pipe      [2]        = { -1, -1 };       //  parent writes   -> child reads
    int         out_pipe     [2]        = { -1, -1 };       //  child writes    -> parent reads
    int         err_pipe     [2]        = { -1, -1 };       //  child writes    -> parent reads (stderr)
    int         exec_pipe    [2]        = { -1, -1 };       //  child signals   -> parent detects exec/chdir failure


    //      1.      CREATE PIPES  (USE <CLOEXEC>, IF AVAILABLE)...
# if defined(__linux__)  &&  defined(O_CLOEXEC)
//
    if ( ::pipe2(in_pipe , O_CLOEXEC) == -1 )           { return false; }
    if ( ::pipe2(out_pipe, O_CLOEXEC) == -1 )           { ::close(in_pipe[0]); ::close(in_pipe[1]); return false; }
    if ( ::pipe2(err_pipe , O_CLOEXEC) == -1 )          { ::close(in_pipe[0]); ::close(in_pipe[1]); ::close(out_pipe[0]); ::close(out_pipe[1]); return false; }
    if ( ::pipe2(exec_pipe, O_CLOEXEC) == -1 )          { ::close(in_pipe[0]); ::close(in_pipe[1]); ::close(out_pipe[0]); ::close(out_pipe[1]); ::close(err_pipe[0]); ::close(err_pipe[1]); return false; }
//
# else
//
    if ( ::pipe(in_pipe)  == -1 )                        { return false; }
    if ( ::pipe(out_pipe) == -1 )                        { ::close(in_pipe[0]); ::close(in_pipe[1]); return false; }
    if ( ::pipe(err_pipe) == -1 )                        { ::close(in_pipe[0]); ::close(in_pipe[1]); ::close(out_pipe[0]); ::close(out_pipe[1]); return false; }
    if ( ::pipe(exec_pipe) == -1 )                       { ::close(in_pipe[0]); ::close(in_pipe[1]); ::close(out_pipe[0]); ::close(out_pipe[1]); ::close(err_pipe[0]); ::close(err_pipe[1]); return false; }
    auto set_cloexec = [](int fd) {
        int flags = ::fcntl(fd, F_GETFD);
        if (flags != -1) { ::fcntl(fd, F_SETFD, flags | FD_CLOEXEC); }
    };
    set_cloexec(in_pipe [0]);   set_cloexec(in_pipe [1]);
    set_cloexec(out_pipe[0]);   set_cloexec(out_pipe[1]);
    set_cloexec(err_pipe [0]);  set_cloexec(err_pipe [1]);
    set_cloexec(exec_pipe[0]);  set_cloexec(exec_pipe[1]);
//
# endif  //  defined(__linux__)  &&  defined(O_CLOEXEC)  //


    //      2.      FORK...
    this->m_child_pid   = ::fork();
    if ( this->m_child_pid == -1 )
    {
        ::close(in_pipe[0]);    ::close(in_pipe[1]);
        ::close(out_pipe[0]);   ::close(out_pipe[1]);
        ::close(err_pipe[0]);   ::close(err_pipe[1]);
        ::close(exec_pipe[0]);  ::close(exec_pipe[1]);
        return false;
    }

    //      3.      CHILD...
    if ( m_child_pid == 0 )
    {
        std::vector<char*>      argv;
        argv    .reserve    ( 3 + this->m_args.size() + 1);
        argv    .push_back( const_cast<char*>( this->m_python_exe.c_str() ) );
        argv    .push_back( const_cast<char*>("-u") );
        argv    .push_back( const_cast<char*>( m_script_path.c_str()) );
        
        
        //          3A.     REDIRECT STDIO.
        if ( ::dup2(in_pipe [0], STDIN_FILENO ) == -1 )     { char c = 1; ::write(exec_pipe[1], &c, 1); _exit(1); }
        if ( ::dup2(out_pipe[1], STDOUT_FILENO) == -1 )     { char c = 1; ::write(exec_pipe[1], &c, 1); _exit(1); }
        if ( ::dup2(err_pipe [1], STDERR_FILENO) == -1 )    { char c = 1; ::write(exec_pipe[1], &c, 1); _exit(1); }


        //          3B.     CLOSE INHERITED FDs.
        ::close(in_pipe [0]);  ::close(in_pipe [1]);
        ::close(out_pipe[0]);  ::close(out_pipe[1]);
        ::close(err_pipe [0]); ::close(err_pipe [1]);
        ::close(exec_pipe[0]);


        //          3C.     ???.
        if ( !this->m_cwd.empty() ) { if ( ::chdir(this->m_cwd.c_str()) != 0 ) { char c = 1; ::write(exec_pipe[1], &c, 1); _exit(1); } }
        for (auto & a : m_args)     { argv.push_back(const_cast<char*>( a.c_str() )); }
        argv.push_back(nullptr);

        ::execvp( this->m_python_exe.c_str(), argv.data() );
        { char c = 1; ::write(exec_pipe[1], &c, 1); _exit(1); }    // exec failed
    }

    //      4.      PARENT; CLOSE UNUSED ENDS, ADOPT OUR ENDS...
    ::close(in_pipe [0]);
    ::close(out_pipe[1]);
    ::close(err_pipe [1]);
    ::close(exec_pipe[1]);

    //      4A.     WAIT FOR CHILD TO EITHER EXEC (EOF) OR REPORT FAILURE (byte)...
    {
        char        c       = 0;
        ssize_t     r       = 0;

        do {
            r = ::read(exec_pipe[0], &c, 1);
        } while (r == -1 && errno == EINTR);

        ::close(exec_pipe[0]);

        if ( r > 0 )
        {
            int status = 0;
            ::waitpid(this->m_child_pid, &status, 0);
            this->m_child_pid = -1;

            ::close(in_pipe [1]);
            ::close(out_pipe[0]);
            ::close(err_pipe [0]);

            return false;
        }
    }

    this->m_child_stdin_fd      = in_pipe [1];
    this->m_child_stdout_fd     = out_pipe[0];
    this->m_child_stderr_fd     = err_pipe [0];

# ifdef F_SETNOSIGPIPE
    ::fcntl(this->m_child_stdin_fd, F_SETNOSIGPIPE, 1);
# endif  //  F_SETNOSIGPIPE  //

    return true;
//
//
# endif  //  _WIN32  //
}
//
//
// *************************************************************************** //
#endif  //  _PYSTREAM_FORCE_UNBUFFERED_PYTHON  //








#ifdef _WIN32
//
//
//
// *************************************************************************** //
//      2A. PROTECTED. |     PLATFORM-SPECIFIC      [[ WIN32 ]].
// *************************************************************************** //


//  "write_pipe"
//
bool PyStream::write_pipe(const char * data, size_t n)
{
    const char *    p       = data;
    size_t          r       = n;
    DWORD           wrote   = 0;
    DWORD           err     = 0;

    if ( !this->m_child_stdin_w )       { return false; }
    if ( r == 0 )                       { return true;  }

    //  WriteFile can legally short-write; loop until done.
    while (r > 0)
    {
        wrote                   = 0;
        const bool      valid   = WriteFile(m_child_stdin_w, p, (DWORD)std::min<size_t>(r, 0x7fffffff), &wrote, nullptr);
        
        if ( !valid )
        {
            err = GetLastError();
            
            //  Clean shutdown / teardown races (child closed stdin, or we closed handle during stop()).
            if ( err == ERROR_BROKEN_PIPE )         { return false; }
            if ( err == ERROR_NO_DATA )             { return false; }
            if ( err == ERROR_PIPE_NOT_CONNECTED )  { return false; }
            if ( err == ERROR_INVALID_HANDLE )      { return false; }
            if ( err == ERROR_OPERATION_ABORTED )   { return false; }

            return false;   //  treat other errors as failure; no retries (for simplicity).
        }
        if ( wrote == 0 )     { return false; }   // shouldn’t happen; treat as failure
        p += wrote;
        r -= wrote;
    }
    return true;
}



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "2A-1.  PLATFORM-SPECIFIC: WIN32".
//
//
//
# else  //  _WIN32  //
//
//
//
// *************************************************************************** //
//      2A. PROTECTED. |     PLATFORM-SPECIFIC      [[ POSIX ]].
// *************************************************************************** //

//  "write_fd"
//      POSIX ONLY!
//
bool PyStream::write_fd(int fd, const char * data, size_t n)
{
    const char *    p       = data;
    size_t          r       = n;
    ssize_t         w       = 0;
    
    
    if (fd < 0)     { return false; }
    if (r == 0)     { return true;  }

# ifndef F_SETNOSIGPIPE
#  ifdef SIGPIPE
    //  On platforms without F_SETNOSIGPIPE (e.g., Linux), a write to a closed pipe can raise SIGPIPE and
    //  terminate the process. Ignore SIGPIPE once and handle the resulting EPIPE return instead.
    static std::once_flag s_ignore_sigpipe_once;
    std::call_once(s_ignore_sigpipe_once, [](){ ::signal(SIGPIPE, SIG_IGN); });
#  endif  //  SIGPIPE
# endif   //  F_SETNOSIGPIPE

    while (r > 0)
    {
        const size_t    to_write    = (r > 0x7fffffff) ? 0x7fffffff : r;
        w   = ::write(fd, p, to_write);
        
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
                ::usleep(1000);
                continue;                           //    pipe temporarily full; retry
            }

            if (errno == EPIPE)     { return false; }   //  child closed pipe (expected failure mode)

            return false;       //  other hard error
        }
        return false;       //  w == 0: unusual for pipes, so treat as failure...
    }
    return true;
}
  


//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "2A-2.  PLATFORM-SPECIFIC: POSIX".
//
//
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

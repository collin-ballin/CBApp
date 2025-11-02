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



namespace cb { namespace utl { //     BEGINNING NAMESPACE "cb" :: "utl"...
// *************************************************************************** //
// *************************************************************************** //



//      1.      INITIALIZATION  | DEFAULT CONSTRUCTOR, DESTRUCTOR, ETC...
// *************************************************************************** //
// *************************************************************************** //

//  Default Constructor.
//
PyStream::PyStream(const std::string & script_path, const std::vector<std::string> & args)
    : m_script_path( path_t(script_path) )
    , m_args(args)
{ }


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
//      1B.     PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "start"
//
bool PyStream::start(void)
#ifdef PYSTREAM_REFACTOR
{
    //      1.      Race-proof: only one caller can transition false -> true
    bool expected = false;
    
    if ( !m_running.compare_exchange_strong(expected, true) ) {
        return false; // already running
    }


    //      2.      Spawn the child process; rollback flag on failure
    if ( !launch_process() )
    {
        m_running.store(false);
        return false;
    }


    //      3.      Start reader thread; on failure, stop() cleans up the spawned child
    try {
        m_reader_thread = std::thread(&PyStream::reader_thread_func, this);
    }
    catch (...) {
        stop();           // safe: sets m_running=false, closes pipes, waits child, joins if needed
        return false;
    }

    return true;
}
#else
{
    if ( m_running.load() )     { return false; }
    if ( !launch_process() )    { return false; }
    
    m_running           .store(true);
    m_reader_thread     = std::thread(&PyStream::reader_thread_func, this);
    return true;
}
#endif  //  PYSTREAM_REFACTOR  //



//  "stop"
//
void PyStream::stop(void)
#ifdef PYSTREAM_REFACTOR
{
    if ( !this->m_running.exchange(false) )   { return; }     // already stopped

#ifdef _WIN32
    //      1.      Cancel the blocked ReadFile in the reader thread (if any)
    if ( this->m_reader_thread.joinable() ) {
        ::CancelSynchronousIo( (HANDLE)this->m_reader_thread.native_handle() );     //  native_handle() is a HANDLE on Windows
    }

    //      2.      Terminate child and wait briefly for it to exit
    if ( this->m_proc_info.hProcess )
    {
        TerminateProcess(this->m_proc_info.hProcess, 0);
        WaitForSingleObject(this->m_proc_info.hProcess, 5000);
        CloseHandle(this->m_proc_info.hProcess);      this->m_proc_info.hProcess = nullptr;
        CloseHandle(this->m_proc_info.hThread);       this->m_proc_info.hThread  = nullptr;
    }

    //      3.      Close our pipe ends (idempotent)
    if (this->m_child_stdin_w)      { CloseHandle(this->m_child_stdin_w);   this->m_child_stdin_w  = nullptr; }
    if (this->m_child_stdout_r)     { CloseHandle(this->m_child_stdout_r);  this->m_child_stdout_r = nullptr; }
#else
    if ( this->m_child_pid > 0 )
    {
        kill(this->m_child_pid, SIGTERM);
        waitpid(this->m_child_pid, nullptr, 0);
        this->m_child_pid = -1;
    }
    if ( this->m_child_stdin_fd  != -1 )        { close(this->m_child_stdin_fd);        this->m_child_stdin_fd  = -1; }
    if ( this->m_child_stdout_fd != -1 )        { close(this->m_child_stdout_fd);       this->m_child_stdout_fd = -1; }
#endif  //  _WIN32  //

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
    std::string     buf;
    char            ch      = '\0';
    ssize_t         n       = 0;
    
    
    buf.reserve(this->ms_READ_BUFFER_SIZE);


#ifdef _WIN32
    DWORD           nread;
    while ( m_running.load() )
    {
        const bool      good_read   = ReadFile(this->m_child_stdout_r, &ch, 1, &nread, nullptr);
        
        if ( !good_read  ||  nread == 0 )   { break;    }
        if ( ch == '\r' )                   { continue; }              //   normalize CRLF -> LF
        if (ch == '\n') {
            std::lock_guard<std::mutex>     lock    (this->m_queue_mutex);
            this->m_recv_queue              .emplace_back(std::move(buf));
            buf.clear();
        }
        else { buf.push_back(ch); }
    }
#else
    while ( m_running.load() )
    {
        n   = ::read(m_child_stdout_fd, &ch, 1);
        
        if ( n <= 0 )           { break; }
        if ( ch == '\n' )
        {
            std::lock_guard<std::mutex>     lock    (this->m_queue_mutex);
            this->m_recv_queue              .emplace_back(std::move(buf));
            buf.clear();
        }
        else { buf.push_back(ch); }
    }
#endif  //  _WIN32  //

    //  Flush final unterminated line on EOF
    if ( !buf.empty() )
    {
        std::lock_guard<std::mutex>     lock    (this->m_queue_mutex);
        this->m_recv_queue              .emplace_back(std::move(buf));
    }

    //  Reflect child/pipe EOF in liveness flag
    this->m_running.store(false);
    return;
}
//
#else
//
{
    std::string     buf;
    char            ch;
    buf             .reserve(4096);
    
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
// *************************************************************************** //   END [[ 1.  "PUBLIC" ]].












// *************************************************************************** //
//
//
//
//      2.      PROTECTED MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//      2A. PROTECTED. |     PLATFORM-SPECIFIC      [[ WIN32 ]].
// *************************************************************************** //
#ifdef _WIN32



//  "s_utf8_to_wide"
//
static inline std::wstring s_utf8_to_wide(const std::string & s)
{
    int             n       = -1;
    std::wstring    w;
    
    if ( s.empty() )    { return {}; }
    
    n       = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
    w       = ( (n)     ? n - 1     : 0, L'\0' );               //  exclude the null terminator
    if ( n )            { MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, w.data(), n); }
    
    return w;
}

//  "s_quote_if_needed"
//
static inline std::wstring s_quote_if_needed(const std::wstring & s)
{
    return (s.find_first_of(L" \t\"") == std::wstring::npos) ? s : L"\"" + s + L"\"";
}




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



//  "launch_process"
//
bool PyStream::launch_process(void)
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
        std::wstringstream ss;
        ss << L"python " << s_quote_if_needed(m_script_path.wstring());
        for (const auto& a : m_args) {
            ss << L" " << s_quote_if_needed(s_utf8_to_wide(a));
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
  

//  "launch_process"
//
bool PyStream::launch_process(void)
{
    int     in_pipe     [2];        //  parent writes -> child reads
    int     out_pipe    [2];        //  child writes  -> parent reads
    
    if ( pipe(in_pipe)  == -1 )     { return false; }
    if ( pipe(out_pipe) == -1 )     {
        close(in_pipe[0]);
        close(in_pipe[1]);
        return false;
    }

    m_child_pid = fork();
    if ( m_child_pid == -1 )  { return false; }

    if ( m_child_pid == 0 )
    {
        dup2(in_pipe [0]    , STDIN_FILENO  );        // child process
        dup2(out_pipe[1]    , STDOUT_FILENO );
        dup2(out_pipe[1]    , STDERR_FILENO );

        close(in_pipe [0]);     close(in_pipe [1]);
        close(out_pipe[0]);     close(out_pipe[1]);

        std::vector<char*>      argv;               //  build argv
        argv.push_back(const_cast<char*>("python3"));
        argv.push_back(const_cast<char*>(m_script_path.c_str()));
        
        for (auto & a : m_args) {
            argv.push_back(const_cast<char*>(a.c_str()));
        }
        argv.push_back(nullptr);

        execvp("python3", argv.data());
        _exit(1);   //  exec failed
    }

    //  parent — close unused ends
    close(in_pipe [0]);
    close(out_pipe[1]);

    m_child_stdin_fd    = in_pipe [1];
    m_child_stdout_fd   = out_pipe[0];

    // set close‑on‑exec & optionally non‑blocking
    fcntl(m_child_stdout_fd, F_SETFD, FD_CLOEXEC);
    fcntl(m_child_stdin_fd,  F_SETFD, FD_CLOEXEC);

    return true;
}

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

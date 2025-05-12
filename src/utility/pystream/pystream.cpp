/***********************************************************************************
*
*       ********************************************************************
*       ****            _ P Y S T R E A M . H  ____  F I L E            ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond
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



//  1.      INITIALIZATION  | DEFAULT CONSTRUCTOR, DESTRUCTOR, ETC...
// *************************************************************************** //
// *************************************************************************** //

//  Default Constructor.
//
PyStream::PyStream(const std::string & script_path, const std::vector<std::string> & args)
    : m_script_path { script_path }, m_args { args }    {}


//  Destructor.
//
PyStream::~PyStream(void)
    { stop(); }






// *************************************************************************** //
//
//
//  1B.     PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "start"
//
bool PyStream::start(void)
{
    if (m_running.load())
        return false;
    if (!launch_process())
        return false;
    m_running.store(true);
    m_reader_thread = std::thread(&PyStream::reader_thread_func, this);
    return true;
}


//  "stop"
//
void PyStream::stop(void)
{
    if (!m_running.exchange(false))
        return; // already stopped

#ifdef _WIN32
    if (m_proc_info.hProcess)
    {
        // ask nicely first — send CTRL‑BREAK equivalent?
        TerminateProcess(m_proc_info.hProcess, 0);
        WaitForSingleObject(m_proc_info.hProcess, 5000);
        CloseHandle(m_proc_info.hProcess);
        CloseHandle(m_proc_info.hThread);
    }
    if (m_child_stdin_w)  CloseHandle(m_child_stdin_w);
    if (m_child_stdout_r) CloseHandle(m_child_stdout_r);
#else
    if (m_child_pid > 0)
    {
        kill(m_child_pid, SIGTERM);
        waitpid(m_child_pid, nullptr, 0);
    }
    if (m_child_stdin_fd  != -1) close(m_child_stdin_fd);
    if (m_child_stdout_fd != -1) close(m_child_stdout_fd);
#endif

    if (m_reader_thread.joinable())
        m_reader_thread.join();

    std::lock_guard<std::mutex> lock(m_queue_mutex);
    m_recv_queue.clear();
    return;
}


//  "send"
//
bool PyStream::send(const std::string & msg)
{
    if (!m_running.load()) return false;
    std::string line = msg;
    if (line.empty() || line.back() != '\n') line.push_back('\n');
#ifdef _WIN32
    return write_pipe(line.data(), line.size());
#else
    return write_fd(m_child_stdin_fd, line.data(), line.size());
#endif
}


//  "is_running"
//
bool PyStream::is_running(void) const
{ return m_running.load(); }
    

//  "try_receive"
//      try_receive (non‑blocking)
bool PyStream::try_receive(std::string & out)
{
    std::lock_guard<std::mutex> lock(m_queue_mutex);
    if (m_recv_queue.empty())
        return false;
    out = std::move(m_recv_queue.front());
    m_recv_queue.pop_front();
    return true;
}


//  "reader_thread_func"
//      reader thread — blocks on stdout and pushes complete lines
void PyStream::reader_thread_func(void)
{
    std::string buf;
    buf.reserve(4096);
    char ch;
#ifdef _WIN32
    DWORD nread;
    while (m_running.load())
    {
        if (!ReadFile(m_child_stdout_r, &ch, 1, &nread, nullptr) || nread == 0)
            break; // pipe closed
        if (ch == '\n')
        {
            std::lock_guard<std::mutex> lock(m_queue_mutex);
            m_recv_queue.emplace_back(std::move(buf));
            buf.clear();
        }
        else buf.push_back(ch);
    }
#else
    while (m_running.load())
    {
        ssize_t n = ::read(m_child_stdout_fd, &ch, 1);
        if (n <= 0) break;
        if (ch == '\n')
        {
            std::lock_guard<std::mutex> lock(m_queue_mutex);
            m_recv_queue.emplace_back(std::move(buf));
            buf.clear();
        }
        else buf.push_back(ch);
    }
#endif
}




// *************************************************************************** //
//
//
//  2.      PROTECTED MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //






// *************************************************************************** //
//
//
//  3.      PLATFORM-SPECIFIC HELPERS...
// *************************************************************************** //
// *************************************************************************** //

#ifdef _WIN32


//  "write_pipe"
//
bool PyStream::write_pipe(const char * data, size_t n)
{
    DWORD written;
    return WriteFile(m_child_stdin_w, data, (DWORD)n, &written, nullptr) && written == n;
}


//  "launch_process"
//
bool PyStream::launch_process()
{
    SECURITY_ATTRIBUTES sa{ sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };
    HANDLE stdin_r = nullptr, stdout_w = nullptr;

    if (!CreatePipe(&stdin_r, &m_child_stdin_w, &sa, 0))  return false;
    if (!CreatePipe(&m_child_stdout_r, &stdout_w, &sa, 0)) return false;
    SetHandleInformation(m_child_stdin_w,  HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(m_child_stdout_r, HANDLE_FLAG_INHERIT, 0);

    std::wstring cmd;
    {
        std::wstringstream ss;
        ss << L"python " << std::wstring(m_script_path.begin(), m_script_path.end());
        for (auto & a : m_args)
        {
            ss << L" " << std::wstring(a.begin(), a.end());
        }
        cmd = ss.str();
    }

    STARTUPINFOW si{ sizeof(STARTUPINFOW) };
    si.dwFlags   = STARTF_USESTDHANDLES;
    si.hStdInput = stdin_r;
    si.hStdOutput= stdout_w;
    si.hStdError = stdout_w; // merge stderr

    if (!CreateProcessW(nullptr, cmd.data(), nullptr, nullptr, TRUE,
                        0, nullptr, nullptr, &si, &m_proc_info))
        return false;

    CloseHandle(stdin_r);
    CloseHandle(stdout_w);
    return true;
}


//  //  //  //
//
#else   // POSIX ----------------------------------------------------
//
//  //  //  //


//  "write_fd"
//
bool PyStream::write_fd(int fd, const char * data, size_t n)
{
    while (n > 0)
    {
        ssize_t w = ::write(fd, data, n);
        if (w <= 0) return false;
        data += w;
        n    -= w;
    }
    return true;
}
  

//  "launch_process"
//
bool PyStream::launch_process(void)
{
    int in_pipe [2]; // parent writes -> child reads
    int out_pipe[2]; // child writes  -> parent reads
    if (pipe(in_pipe) == -1)  return false;
    if (pipe(out_pipe)== -1) { close(in_pipe[0]); close(in_pipe[1]); return false; }

    m_child_pid = fork();
    if (m_child_pid == -1) return false;

    if (m_child_pid == 0)
    {
        // child process
        dup2(in_pipe [0], STDIN_FILENO);
        dup2(out_pipe[1], STDOUT_FILENO);
        dup2(out_pipe[1], STDERR_FILENO);

        close(in_pipe [0]); close(in_pipe [1]);
        close(out_pipe[0]); close(out_pipe[1]);

        // build argv
        std::vector<char*> argv;
        argv.push_back(const_cast<char*>("python3"));
        argv.push_back(const_cast<char*>(m_script_path.c_str()));
        for (auto & a : m_args) argv.push_back(const_cast<char*>(a.c_str()));
        argv.push_back(nullptr);

        execvp("python3", argv.data());
        _exit(1); // exec failed
    }

    // parent — close unused ends
    close(in_pipe [0]);
    close(out_pipe[1]);

    m_child_stdin_fd  = in_pipe [1];
    m_child_stdout_fd = out_pipe[0];

    // set close‑on‑exec & optionally non‑blocking
    fcntl(m_child_stdout_fd, F_SETFD, FD_CLOEXEC);
    fcntl(m_child_stdin_fd,  F_SETFD, FD_CLOEXEC);

    return true;
}

#endif // platform













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

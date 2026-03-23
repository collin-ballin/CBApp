/***********************************************************************************
*
*       ********************************************************************
*       ****             L O G G E R . C P P  ____  F I L E             ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond.
*               DATED:      May 11, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "utility/_logger.h"
#include CBAPP_USER_CONFIG



namespace cb { namespace utl { //     BEGINNING NAMESPACE "cb" :: "utl"...
// *************************************************************************** //
// *************************************************************************** //


//  0.      STATIC HELPER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

static void append_indent(std::ostream& os, size_t n) { os << std::string(n, ' '); }

static std::string tid_hex(std::thread::id tid)
{
    std::ostringstream s;
    s << std::hex << std::uppercase << std::hash<std::thread::id>{}(tid);
    return s.str();
}






// *************************************************************************** //
//
//
//  1A.     INITIALIZATION  | DEFAULT CONSTRUCTOR, DESTRUCTOR, ETC...
// *************************************************************************** //
// *************************************************************************** //
    
//  Default Constructor.            | PRIVATE BECAUSE SINGLETON...
//
Logger::Logger(void) {
#if defined(_WIN32)     //  1.  BUILDING FOR WINDOWS...
//
    #ifdef CBAPP_DISABLE_TERMINAL_COLORS
        m_vt_enabled = false;
    # else
        enable_vt_win();            // sets m_vt_enabled
    #endif  //  CBAPP_DISABLE_TERMINAL_COLORS  //
//
#else                   //  2.  BUILDING FOR macOS...
//
    #ifdef CBAPP_DISABLE_TERMINAL_COLORS
        m_vt_enabled = false;
    # else
        m_vt_enabled = true;
    #endif  //  CBAPP_DISABLE_TERMINAL_COLORS  //
//
#endif  //  _WIN32  //
    

#if CBAPP_LOG_ENABLED
    start_worker();
#endif
}


//  Default Destructor.             | PRIVATE BECAUSE SINGLETON...
//
Logger::~Logger(void) {
#if CBAPP_LOG_ENABLED
        stop_worker();
#endif
}



// *************************************************************************** //
//
//
//  2A.     PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "log"
//
void Logger::log(const char * msg,          LogType lvl)          { enqueue(msg, lvl); }
void Logger::log(const std::string & msg,   LogType lvl)          { enqueue(msg.c_str(), lvl); }


//  "debug"
//
void Logger::debug(const char * msg)                            { enqueue(msg,          LogType::Debug);      }
void Logger::debug(const std::string & msg)                     { enqueue(msg.c_str(),  LogType::Debug);      }


//  "info"
//
void Logger::info(const char * msg)                             { enqueue(msg,          LogType::Info);       }
void Logger::info(const std::string & msg)                      { enqueue(msg.c_str(),  LogType::Info);       }


//  "warning"
//
void Logger::warning(const char * msg)                          { enqueue(msg,          LogType::Warning);    }
void Logger::warning(const std::string & msg)                   { enqueue(msg.c_str(),  LogType::Warning);    }


//  "exception"
//
void Logger::exception(const char * msg)                        { enqueue(msg,          LogType::Exception);  }
void Logger::exception(const std::string & msg)                 { enqueue(msg.c_str(),  LogType::Exception);  }


//  "error"
//
void Logger::error(const char * msg)                            { enqueue(msg,          LogType::Error);      }
void Logger::error(const std::string & msg)                     { enqueue(msg.c_str(),  LogType::Error);      }


//  "notify"
//
void Logger::notify(const char * msg)                           { enqueue(msg,          LogType::Notify);   }
void Logger::notify(const std::string & msg)                    { enqueue(msg.c_str(),  LogType::Notify);   }


//  "critical"
//
void Logger::critical(const char * msg)                         { enqueue(msg,          LogType::Critical);   }
void Logger::critical(const std::string & msg)                  { enqueue(msg.c_str(),  LogType::Critical);   }






// *************************************************************************** //
//
//
//  3.      PROTECTED MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //


#if CBAPP_LOG_ENABLED
// *************************************************************************** //

//  "enqueue"
//
void Logger::enqueue(const char * msg, LogType lvl)
{
    this->enqueue_event(LogEvent{
          lvl
        , msg
        , this->next_count(lvl)
        , /*file=*/ nullptr
        , /*line=*/ 0
        , /*func=*/ nullptr
        , std::this_thread::get_id()
        , Logger::s_iso_timestamp()
    });
    
    return;
}


//  "enqueue_event"
//
void Logger::enqueue_event(LogEvent && ev)
{
    
    if ( static_cast<int>(ev.level) < static_cast<int>(m_threshold.load(std::memory_order_relaxed)) )       { return; }

    std::unique_lock<std::mutex>    lock        (m_mtx);
    
    m_cv.wait(lock, [this]{ return m_queue.size() < cv_QUEUE_CAPACITY; });
    m_queue.push(std::move(ev));
    lock.unlock();
    m_cv.notify_one();
    
    return;
}


//  "start_worker"
//
void Logger::start_worker(void)
{
    m_running.store(true, std::memory_order_relaxed);
    m_worker    = std::thread( [this]
        {
            std::unique_lock<std::mutex>    lock    (m_mtx);
            
            while ( m_running.load(std::memory_order_relaxed) || !m_queue.empty() )     //  while #1.
            {
                m_cv.wait(lock, [this]{ return !m_queue.empty() || !m_running.load(std::memory_order_relaxed); });
                while ( !m_queue.empty() )                  //  while #2.
                {
                    LogEvent    ev = std::move(m_queue.front());
                    m_queue.pop();
                    lock.unlock();
                    write_event(ev);
                    lock.lock();
                    m_cv.notify_all();   // signal space to any waiting producers
                    
                }//     END "while #1".
            //
            }// END "while empty queue".
        //
        //
        }// END LAMBDA.
    );
    
    return;
}


//  "stop_worker"
//
void Logger::stop_worker(void)
{
    {
        std::lock_guard<std::mutex> lg(m_mtx);
        m_running.store(false, std::memory_order_relaxed);
    }
    m_cv.notify_all();
    if (m_worker.joinable())    { m_worker.join(); }
    
    return;
}


// ---------------------------------------------------------------------------
// FORMATTED OUTPUT WITH HANGING-INDENT + WORD WRAP
// ---------------------------------------------------------------------------

//  "build_header"
//      Member helper: build textual header "[CBLOG LEVEL ###] : "
//
[[nodiscard]] std::string Logger::build_header(const LogEvent & ev)
{
    std::ostringstream      ss;
    
    ss << '[' << cv_HEADER     << ' '
       << std::left         << std::setw(cv_MAX_LEVEL_LEN) << Logger::ms_LOGLEVEL_NAME[ev.level]   << ' '
       << std::right        << std::setfill('0')        << std::setw(cv_COUNTER_WIDTH)             << ev.count
       << std::setfill(' ') << "]    : ";
       
    return ss.str();
}


//  "write_body"
//      Member helper: hanging‑indent, word‑wrapped body (writes directly to `out`)
//
void Logger::write_body(const std::string& msg, std::ostream& out, size_t indent_len) const
{
    out << cv_BODY_OPEN_DELIM;
    size_t col = indent_len + std::char_traits<char>::length(cv_BODY_OPEN_DELIM);

    std::ostringstream line;
    auto flush_line = [&]{ out << line.str(); line.str({}); line.clear(); };
    auto newline    = [&]{ flush_line(); out << '\n'; append_indent(out, indent_len); col = indent_len; };

    for (size_t i = 0; i < msg.size();) {
        if (msg[i] == '\n') { newline(); ++i; continue; }
        size_t word_end = msg.find_first_of(" \n", i);
        if (word_end == std::string::npos) word_end = msg.size();
        std::string_view word(&msg[i], word_end - i);

        if (col > indent_len && col + word.size() + 1 > cv_MAX_COL_WIDTH) newline();
        if (col > indent_len) { line << ' '; ++col; }
        line << word; col += word.size();
        i = word_end;
        if (i < msg.size() && msg[i] == ' ') ++i; // skip single space
    }
    flush_line();
    out << cv_BODY_CLOSE_DELIM;
    return;
}



//  "build_metadata"
//      Member helper: metadata line (file, line, func, thread, timestamp)
//
std::string Logger::build_metadata(const LogEvent & ev, size_t indent_len)
{
    bool                    first       = true;
    LoggerFieldFlags        fmt         = m_fields.load(std::memory_order_relaxed);
    size_t                  path_depth  = m_path_depth.load(std::memory_order_relaxed);
    std::ostringstream      ss;
    
    
    append_indent(ss, indent_len);
    
    ss << '(';
    if (fmt & LoggerFieldFlags_ThreadID) {                                       //  1.  Thread ID.
        if (!first) ss << ' ';
        ss << "thread: 0x" << tid_hex(ev.thread_id) << ".";
        first = false;
    }
    
    if ((fmt & LoggerFieldFlags_Function) && ev.func) {                           //  2.  Function-Name.
        if (!first) ss << ' ';
        ss << "func: " << ev.func << ".";
        first = false;
    }
    
    if ((fmt & LoggerFieldFlags_File) && ev.file) {                               //  3.  Filename.
        if (!first) ss << ' ';
        ss << "file: " << path_tail(ev.file, path_depth) << ".";
        first = false;
    }
    
    if ((fmt & LoggerFieldFlags_Line) && (ev.line > 0)) {                         //  4.  Line Number.
        if (!first) ss << ' ';
        ss << "line: " << ev.line << ".";
        first = false;
    }
    
    if (fmt & LoggerFieldFlags_Timestamp) {                                       //  5.  Time-Stamp.
        if (!first) ss << ' ';
        ss << ev.ts_iso8601;
        first = false;
    }
    
    if (first)    { return ""; }
    
    ss << ')';
    return ss.str();
}



//  "write_event"
//      Public sink: write_event – orchestrates helpers
//
void Logger::write_event(const LogEvent & ev)
{
    const bool              color           = m_vt_enabled.load(std::memory_order_relaxed);
    const char *            prefix          = color     ? Logger::ms_LOGLEVEL_TO_ASCII_COLOR[ ev.level ]        : "";
    const char *            suffix          = color     ? Logger::ms_LOGLEVEL_TO_ASCII_COLOR[ LogType::None ]  : "";
    const std::string       header          = this->build_header(ev);
    const size_t            indent_len      = header.size();
    const std::string       metadata        = this->build_metadata(ev, indent_len);
    std::ostringstream      out;
    
    
    out << header;
    this->write_body(ev.text, out, indent_len);
    
    if (!metadata.empty()) {
        out << '\n' << metadata;
    }
    
    std::cout << prefix << out.str() << suffix << std::endl;
    return;
}


//
//
// *************************************************************************** //
# else
// *************************************************************************** //
//
//
    void Logger::enqueue([[maybe_unused]] const char * , [[maybe_unused]] const LogType )      { return; }
//
//
//
// *************************************************************************** //
#endif
    
    
    
    
//  "enable_vt_win"
//
#if CBAPP_LOG_ENABLED
// *************************************************************************** //
//
//
//
# ifdef _WIN32
void Logger::enable_vt_win(void)
{
    HANDLE      h       = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD       mode    = 0;
    
    if ( !h || h == INVALID_HANDLE_VALUE )      { this->m_vt_enabled.store(false, std::memory_order_relaxed);  return; }
    if ( !GetConsoleMode(h, &mode) )            { this->m_vt_enabled.store(false, std::memory_order_relaxed);  return; }

    if ( SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING) )     { this->m_vt_enabled.store(true, std::memory_order_relaxed);   }
    else                                                                    { this->m_vt_enabled.store(false, std::memory_order_relaxed);  }       // older console; no VT
    
    return;
}
# else
    void Logger::enable_vt_win(void)    { return; }
#endif  //  _WIN32  //
//
//
//
// *************************************************************************** //
#endif  //  CBAPP_LOG_ENABLED  //





// *************************************************************************** //
//
//
//      UTILITY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "path_tail"
//
[[nodiscard]] std::string Logger::path_tail(std::string_view s, size_t depth)
{
    // works for '/' and '\\'
    if ((depth == 0ULL) || s.empty())    { return std::string{s}; }

    size_t      pos         = s.size();
    size_t      sep_count   = 0ULL;

    while (pos > 0ULL)
    {
        pos = s.find_last_of("/\\", pos - 1ULL);
        if (pos == std::string_view::npos)      { break; }

        ++sep_count;
        if (sep_count == depth)
        {
            s.remove_prefix(pos + 1ULL);
            break;
        }
    }
    return std::string{s};
}






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



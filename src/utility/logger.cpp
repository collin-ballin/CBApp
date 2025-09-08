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

static void append_indent(std::ostream& os, std::size_t n) { os << std::string(n, ' '); }

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
void Logger::log(const char * msg,          Level lvl)          { enqueue(msg, lvl); }
void Logger::log(const std::string & msg,   Level lvl)          { enqueue(msg.c_str(), lvl); }


//  "debug"
//
void Logger::debug(const char * msg)                            { enqueue(msg,          Level::Debug);      }
void Logger::debug(const std::string & msg)                     { enqueue(msg.c_str(),  Level::Debug);      }


//  "info"
//
void Logger::info(const char * msg)                             { enqueue(msg,          Level::Info);       }
void Logger::info(const std::string & msg)                      { enqueue(msg.c_str(),  Level::Info);       }


//  "warning"
//
void Logger::warning(const char * msg)                          { enqueue(msg,          Level::Warning);    }
void Logger::warning(const std::string & msg)                   { enqueue(msg.c_str(),  Level::Warning);    }


//  "exception"
//
void Logger::exception(const char * msg)                        { enqueue(msg,          Level::Exception);  }
void Logger::exception(const std::string & msg)                 { enqueue(msg.c_str(),  Level::Exception);  }


//  "error"
//
void Logger::error(const char * msg)                            { enqueue(msg,          Level::Error);      }
void Logger::error(const std::string & msg)                     { enqueue(msg.c_str(),  Level::Error);      }


//  "notify"
//
void Logger::notify(const char * msg)                           { enqueue(msg,          Level::Notify);   }
void Logger::notify(const std::string & msg)                    { enqueue(msg.c_str(),  Level::Notify);   }


//  "critical"
//
void Logger::critical(const char * msg)                         { enqueue(msg,          Level::Critical);   }
void Logger::critical(const std::string & msg)                  { enqueue(msg.c_str(),  Level::Critical);   }






// *************************************************************************** //
//
//
//  2B.     PUBLIC UTILITY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "set_level"
void  Logger::set_level(const Level & level)                    { this->m_threshold = level; }


//  "get_level"
Logger::Level Logger::get_level(void) const                     { return m_threshold; }


//  "set_console_format"
void Logger::set_console_format(ConsoleFormat fmt)              { m_console_fmt = fmt; }


//  "get_console_format"
Logger::ConsoleFormat Logger::get_console_format(void) const    { return m_console_fmt; }

//  "set_path_depth"
void Logger::set_path_depth(std::size_t d)                      { m_path_depth = d; }

//  "get_path_depth"
std::size_t Logger::get_path_depth(void) const                  { return m_path_depth; }



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
void Logger::enqueue(const char* msg, Level lvl)
{
    enqueue_event(LogEvent{
        lvl, msg, next_count(lvl),
        /*file*/ nullptr,
        /*line*/ 0,
        /*func*/ nullptr,
        std::this_thread::get_id(),
        iso_timestamp()
    });
}


//  "enqueue_event"
//
void Logger::enqueue_event(LogEvent&& ev)
{
    if (static_cast<int>(ev.level) < static_cast<int>(m_threshold))
        return;

    std::unique_lock<std::mutex> lock(m_mtx);
    m_cv.wait(lock, [this]{ return m_queue.size() < QUEUE_CAPACITY; });
    m_queue.push(std::move(ev));
    lock.unlock();
    m_cv.notify_one();
}


//  "start_worker"
//
void Logger::start_worker(void)
{
    m_running   = true;
    m_worker    = std::thread( [this]{
        std::unique_lock<std::mutex>    lock(m_mtx);
        
        while ( m_running || !m_queue.empty() )     //  while #1.
        {
            m_cv.wait(lock, [this]{ return !m_queue.empty() || !m_running; });
            while ( !m_queue.empty() )                  //  while #2.
            {
                LogEvent    ev = std::move(m_queue.front());
                m_queue.pop();
                lock.unlock();
                write_event(ev);
                lock.lock();
                m_cv.notify_all();   // signal space to any waiting producers
                
            }//     END "while #1".
            
        }// END "while #1".
        
    } );
    
    return;
}


//  "stop_worker"
//
void Logger::stop_worker(void) {
    {
        std::lock_guard<std::mutex> lg(m_mtx);
        m_running = false;
    }
    m_cv.notify_all();
    if (m_worker.joinable()) m_worker.join();
}


// ---------------------------------------------------------------------------
// FORMATTED OUTPUT WITH HANGING-INDENT + WORD WRAP
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Member helper: build textual header "[CBLOG LEVEL ###] : "
// ---------------------------------------------------------------------------
std::string Logger::build_header(const LogEvent& ev)
{
    std::ostringstream ss;
    ss << '[' << HEADER << ' '
       << std::left  << std::setw(MAX_LEVEL_LEN) << level_str(ev.level) << ' '
       << std::right << std::setfill('0') << std::setw(COUNTER_WIDTH) << ev.count
       << std::setfill(' ') << "]    : ";
    return ss.str();
}

// ---------------------------------------------------------------------------
// Member helper: hanging‑indent, word‑wrapped body (writes directly to `out`)
// ---------------------------------------------------------------------------
void Logger::write_body(const std::string& msg, std::ostream& out, std::size_t indent_len) const
{
    out << BODY_OPEN_DELIM;
    std::size_t col = indent_len + std::char_traits<char>::length(BODY_OPEN_DELIM);

    std::ostringstream line;
    auto flush_line = [&]{ out << line.str(); line.str({}); line.clear(); };
    auto newline    = [&]{ flush_line(); out << '\n'; append_indent(out, indent_len); col = indent_len; };

    for (std::size_t i = 0; i < msg.size();) {
        if (msg[i] == '\n') { newline(); ++i; continue; }
        std::size_t word_end = msg.find_first_of(" \n", i);
        if (word_end == std::string::npos) word_end = msg.size();
        std::string_view word(&msg[i], word_end - i);

        if (col > indent_len && col + word.size() + 1 > MAX_COL_WIDTH) newline();
        if (col > indent_len) { line << ' '; ++col; }
        line << word; col += word.size();
        i = word_end;
        if (i < msg.size() && msg[i] == ' ') ++i; // skip single space
    }
    flush_line();
    out << BODY_CLOSE_DELIM;
}

// ---------------------------------------------------------------------------
// Member helper: metadata line (file, line, func, thread, timestamp)
// ---------------------------------------------------------------------------
std::string Logger::build_metadata(const LogEvent & ev, std::size_t indent_len)
{
    bool                    first   = true;
    ConsoleFormat           fmt     = m_console_fmt.load(std::memory_order_relaxed);
    std::ostringstream      ss;
    append_indent(ss, indent_len);
    
    ss << '(';
    if (fmt & CF_THREAD_ID) {                                       //  1.  Thread ID.
        if (!first) ss << ' ';
        ss << "thread: 0x" << tid_hex(ev.thread_id) << ".";
        first = false;
    }
    
    if ((fmt & CF_FUNCTION) && ev.func) {                           //  2.  Function-Name.
        if (!first) ss << ' ';
        ss << "func: " << ev.func << ".";
        first = false;
    }
    
    if ((fmt & CF_FILE) && ev.file) {                               //  3.  Filename.
        if (!first) ss << ' ';
        ss << "file: " << path_tail(ev.file, m_path_depth) << ".";
        first = false;
    }
    
    if ((fmt & CF_LINE) && ev.file) {                               //  4.  Line Number.
        if (!first) ss << ' ';
        ss << "line: " << ev.line << ".";
    }
    
    if (fmt & CF_TIMESTAMP) {                                       //  5.  Time-Stamp.
        if (!first) ss << ' ';
        ss << ev.ts_iso8601;
        first = false;
    }
    
    ss << ')';
    return ss.str();
}

// ---------------------------------------------------------------------------
// Public sink: write_event – orchestrates helpers
// ---------------------------------------------------------------------------
void Logger::write_event(const LogEvent & ev)
{
    const bool          color       = m_vt_enabled.load(std::memory_order_relaxed);
    const char *        prefix      = color ? ansi_code(level_to_color(ev.level)) : "";
    const char *        suffix      = color ? ansi_code(TermColor::Reset)         : "";
    const std::string   header      = this->build_header(ev);
    const std::size_t   indent_len  = header.size();

    std::ostringstream out;
    out << header;
    this->write_body(ev.text, out, indent_len);
    out << '\n' << this->build_metadata(ev, indent_len);

    std::cout << prefix << out.str() << suffix << std::endl;
}


//
//
// *************************************************************************** //
# else
// *************************************************************************** //
//
//
    void Logger::enqueue(const char *, Level )      { }
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
# ifdef _WIN32
void Logger::enable_vt_win()
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (!h || h == INVALID_HANDLE_VALUE) { m_vt_enabled = false; return; }

    DWORD mode = 0;
    if (!GetConsoleMode(h, &mode)) { m_vt_enabled = false; return; }

    if (SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING))
        m_vt_enabled = true;
    else
        m_vt_enabled = false;       // older console; no VT
}
# else
    void Logger::enable_vt_win(void)    { }
#endif  //  _WIN32
//
// *************************************************************************** //
#endif





// *************************************************************************** //
//
//
//      UTILITY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "level_to_color"
//
Logger::TermColor Logger::level_to_color(Level lvl)
{
    switch (lvl) {
        case Level::Debug:     return TermColor::Grey;
        case Level::Info:      return TermColor::Green;
        case Level::Warning:   return TermColor::Yellow;
        case Level::Error:     return TermColor::Red;
        case Level::Exception: return TermColor::Cyan;
        case Level::Critical:  return TermColor::Magenta;
        default:               return TermColor::Reset;
    }
}

//  "path_tail"
//
std::string Logger::path_tail(std::string_view s, std::size_t depth) {
    // works for '/' and '\\'
    for (std::size_t i = 0; i < depth && !s.empty(); ++i) {
        auto pos = s.find_last_of("/\\");
        if (pos == std::string_view::npos) break;
        s.remove_prefix(pos + 1);
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



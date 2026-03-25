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
#include <filesystem>


#include "utility/_logger.h"
#include CBAPP_USER_CONFIG






namespace cb { namespace utl { //     BEGINNING NAMESPACE "cb" :: "utl"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//
//
//
//      0.      STATIC HELPER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

static void append_indent(std::ostream& os, size_t n) { os << std::string(n, ' '); }

static std::string tid_hex(std::thread::id tid)
{
    std::ostringstream s;
    s << std::hex << std::uppercase << std::hash<std::thread::id>{}(tid);
    return s.str();
}



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 0.  "STATIC HELPERS" ]].






// *************************************************************************** //
//
//
//
//      1.      PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//      1A. PUBLIC. |       INITIALIZATION  | CONSTRUCTORS, ETC.
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
    if (this->m_sink_states[LogSinkType::File].enabled) {
        this->file_sink_open();
        if (!this->m_file_stream.is_open())    { this->m_sink_states[LogSinkType::File].enabled = false; }
    }
    start_worker();
#endif  //  CBAPP_LOG_ENABLED  //
}


//  Default Destructor.             | PRIVATE BECAUSE SINGLETON...
//
Logger::~Logger(void) {
#if CBAPP_LOG_ENABLED
        stop_worker();
        file_sink_close();
#endif  //  CBAPP_LOG_ENABLED  //
}


//
//
// *************************************************************************** //   END [[ 1A.  "INITIALIZATION" ]].



// *************************************************************************** //
//      1B. PUBLIC. |       PRIMARY PUBLIC API.
// *************************************************************************** //

//  "log"
//
void Logger::log(const char * msg,          LogType lvl)          { enqueue(msg, lvl); }
void Logger::log(const std::string & msg,   LogType lvl)          { enqueue(msg.c_str(), lvl); }


//  "log_sync"
//
void Logger::log_sync(const char * msg,          LogType lvl)
{
#if CBAPP_LOG_ENABLED
    this->dispatch_event_sync(LogEvent{
          lvl
        , msg
        , this->next_count(lvl)
        , /*file=*/ nullptr
        , /*line=*/ 0
        , /*func=*/ nullptr
        , std::this_thread::get_id()
        , Logger::s_iso_timestamp()
    });
#else
    (void)msg; (void)lvl;
#endif
    return;
}
void Logger::log_sync(const std::string & msg,   LogType lvl)     { log_sync(msg.c_str(), lvl); }


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
void Logger::exception_sync(const char * msg)                   { log_sync(msg,         LogType::Exception);  }
void Logger::exception_sync(const std::string & msg)            { log_sync(msg,         LogType::Exception);  }


//  "error"
//
void Logger::error(const char * msg)                            { enqueue(msg,          LogType::Error);      }
void Logger::error(const std::string & msg)                     { enqueue(msg.c_str(),  LogType::Error);      }
void Logger::error_sync(const char * msg)                       { log_sync(msg,         LogType::Error);      }
void Logger::error_sync(const std::string & msg)                { log_sync(msg,         LogType::Error);      }


//  "notify"
//
void Logger::notify(const char * msg)                           { enqueue(msg,          LogType::Notify);   }
void Logger::notify(const std::string & msg)                    { enqueue(msg.c_str(),  LogType::Notify);   }


//  "critical"
//
void Logger::critical(const char * msg)                         { enqueue(msg,          LogType::Critical);   }
void Logger::critical(const std::string & msg)                  { enqueue(msg.c_str(),  LogType::Critical);   }
void Logger::critical_sync(const char * msg)                    { log_sync(msg,         LogType::Critical);   }
void Logger::critical_sync(const std::string & msg)             { log_sync(msg,         LogType::Critical);   }


//
//
// *************************************************************************** //   END [[ 1B.  "PRIMARY PUBLIC API" ]].



// *************************************************************************** //
//      1C. PUBLIC. |       SECONDARY PUBLIC API.
// *************************************************************************** //


//
//
// *************************************************************************** //   END [[ 1C.  "SECONDARY PUBLIC API" ]].



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 1.  "PUBLIC MEMBERS" ]].






// *************************************************************************** //
//
//
//
//      2.      PROTECTED MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

#if CBAPP_LOG_ENABLED
// *************************************************************************** //
// *************************************************************************** //
// *************************************************************************** //
//
//
//

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
    std::unique_lock<std::mutex>    lock        (m_mtx);
    
    if (m_queue.size() >= cv_QUEUE_CAPACITY)    { this->accounting_note_queue_wait(); }
    
    m_cv.wait(lock, [this]{ return m_queue.size() < cv_QUEUE_CAPACITY; });
    m_queue.push(std::move(ev));
    this->accounting_note_queue_size(m_queue.size());
    lock.unlock();
    m_cv.notify_one();
    
    return;
}


//  "accounting_note_queue_wait"
//
void Logger::accounting_note_queue_wait(void)
{
    std::lock_guard<std::mutex>     lg          (this->m_accounting_mtx);
    
    ++this->m_accounting[LogAccountingCounter::QueueProducerWaitCount];
    return;
}


//  "accounting_note_queue_size"
//
void Logger::accounting_note_queue_size(const size_t queue_size)
{
    std::lock_guard<std::mutex>     lg          (this->m_accounting_mtx);
    
    if (queue_size > this->m_accounting[LogAccountingCounter::QueueHighWaterMark]) {
        this->m_accounting[LogAccountingCounter::QueueHighWaterMark] = queue_size;
    }
    return;
}


//  "accounting_note_sync_dispatch"
//
void Logger::accounting_note_sync_dispatch(void)
{
    std::lock_guard<std::mutex>     lg          (this->m_accounting_mtx);
    
    ++this->m_accounting[LogAccountingCounter::SyncEventDispatchCount];
    return;
}


//  "accounting_note_file_failure"
//
void Logger::accounting_note_file_failure(void)
{
    std::lock_guard<std::mutex>     lg          (this->m_accounting_mtx);
    
    ++this->m_accounting[LogAccountingCounter::FileSinkWriteFailureCount];
    return;
}


//  "accounting_note_message_loss"
//
void Logger::accounting_note_message_loss(const LogType level, const counter_type count)
{
    std::lock_guard<std::mutex>     lg          (this->m_accounting_mtx);
    
    this->m_loss_counts[level] += count;
    this->m_accounting[LogAccountingCounter::TotalLostMessageCount] += count;
    return;
}






//  "dispatch_event"
//
void Logger::dispatch_event(const LogEvent & ev)
{
    std::lock_guard<std::mutex>     lg          (this->m_sink_io_mtx);
    
    this->dispatch_sink_event(LogSinkType::Terminal, ev);
    this->dispatch_sink_event(LogSinkType::File, ev);
    
    return;
}


//  "dispatch_event_sync"
//
void Logger::dispatch_event_sync(const LogEvent & ev)
{
    this->accounting_note_sync_dispatch();
    
    std::lock_guard<std::mutex>     lg          (this->m_sink_io_mtx);
    
    this->dispatch_sink_event_sync(LogSinkType::Terminal, ev);
    this->dispatch_sink_event_sync(LogSinkType::File, ev);
    
    return;
}






//  "dispatch_sink_event"
//
void Logger::dispatch_sink_event(const LogSinkType sink, const LogEvent & ev)
{
    switch (sink) {
        case LogSinkType::None : {
            break;
        }
        case LogSinkType::Terminal : {
            const LogSinkState      sink_state  = this->get_sink_state(sink);
            
            if (!sink_state.enabled)                                                                        { break; }
            if (static_cast<int>(ev.level) < static_cast<int>(sink_state.threshold))                        { break; }
            
            this->terminal_sink_write_event(ev);
            break;
        }
        case LogSinkType::File : {
            const LogSinkState      sink_state  = this->get_sink_state(sink);
            
            if (!sink_state.enabled)                                                                        { break; }
            if (static_cast<int>(ev.level) < static_cast<int>(sink_state.threshold))                        { break; }
            
            this->file_sink_write_event(ev);
            break;
        }
        case LogSinkType::COUNT : {
            break;
        }
        default : {
            break;
        }
    }
    
    return;
}


//  "dispatch_sink_event_sync"
//
void Logger::dispatch_sink_event_sync(const LogSinkType sink, const LogEvent & ev)
{
    switch (sink) {
        case LogSinkType::None : {
            break;
        }
        case LogSinkType::Terminal : {
            const LogSinkState      sink_state  = this->get_sink_state(sink);
            
            if (!sink_state.enabled)                                                                        { break; }
            if (!sink_state.sync_severe_enabled)                                                            { break; }
            if (static_cast<int>(ev.level) < static_cast<int>(sink_state.threshold))                        { break; }
            
            this->terminal_sink_write_event(ev);
            break;
        }
        case LogSinkType::File : {
            const LogSinkState      sink_state  = this->get_sink_state(sink);
            
            if (!sink_state.enabled)                                                                        { break; }
            if (!sink_state.sync_severe_enabled)                                                            { break; }
            if (static_cast<int>(ev.level) < static_cast<int>(sink_state.threshold))                        { break; }
            
            this->file_sink_write_event(ev);
            break;
        }
        case LogSinkType::COUNT : {
            break;
        }
        default : {
            break;
        }
    }
    
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
                    dispatch_event(ev);
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



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 2.  "PROTECTED FUNCTIONS" ]].






// *************************************************************************** //
//
//
//
//      3.      SINK FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//      3A. PROTECTED. |    TERMINAL SINK FUNCTIONS.
// *************************************************************************** //

//  "terminal_sink_build_header"
//      Member helper: build terminal-sink header "[CBLOG LEVEL ###] : "
//
[[nodiscard]] std::string Logger::terminal_sink_build_header(const LogEvent & ev)
{
    std::ostringstream      ss;
    
    ss << '[' << cv_HEADER     << ' '
       << std::left         << std::setw(cv_MAX_LEVEL_LEN) << Logger::ms_LOGLEVEL_NAME[ev.level]   << ' '
       << std::right        << std::setfill('0')        << std::setw(cv_COUNTER_WIDTH)             << ev.count
       << std::setfill(' ') << "]    : ";
       
    return ss.str();
}


//  "terminal_sink_write_body"
//      Member helper: terminal-sink hanging-indent, word-wrapped body
//
void Logger::terminal_sink_write_body(const std::string & msg, std::ostream & out, size_t indent_len) const
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


//  "terminal_sink_build_metadata"
//      Member helper: terminal-sink metadata line
//
std::string Logger::terminal_sink_build_metadata(const LogEvent & ev, size_t indent_len)
{
    const LogSinkState      sink_state  = this->get_sink_state(LogSinkType::Terminal);
    bool                    first       = true;
    LoggerFieldFlags        fmt         = sink_state.field_flags;
    size_t                  path_depth  = sink_state.path_depth;
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


//  "terminal_sink_write_event"
//      Terminal sink orchestrator: render and emit one event to `std::cout`
//
void Logger::terminal_sink_write_event(const LogEvent & ev)
{
    const bool              color           = m_vt_enabled.load(std::memory_order_relaxed);
    const char *            prefix          = color     ? Logger::ms_LOGLEVEL_TO_ASCII_COLOR[ ev.level ]        : "";
    const char *            suffix          = color     ? Logger::ms_LOGLEVEL_TO_ASCII_COLOR[ LogType::None ]  : "";
    const std::string       header          = this->terminal_sink_build_header(ev);
    const size_t            indent_len      = header.size();
    const std::string       metadata        = this->terminal_sink_build_metadata(ev, indent_len);
    std::ostringstream      out;
    
    
    out << header;
    this->terminal_sink_write_body(ev.text, out, indent_len);
    
    if (!metadata.empty()) {
        out << '\n' << metadata;
    }
    
    std::cout << prefix << out.str() << suffix << std::endl;
    return;
}


//
//
// *************************************************************************** //   END [[ 3A.  "TERMINAL SINK" ]].



// *************************************************************************** //
//      3B. PROTECTED. |    FILE SINK FUNCTIONS.
// *************************************************************************** //

//  "file_sink_open"
//
void Logger::file_sink_open(void)
{
    std::ios_base::openmode     open_mode   = std::ios::out;
    
    switch (this->m_file_open_mode) {
        case LogFileOpenMode::None : {
            open_mode |= std::ios::app;
            break;
        }
        case LogFileOpenMode::Append : {
            open_mode |= std::ios::app;
            break;
        }
        case LogFileOpenMode::Truncate : {
            open_mode |= std::ios::trunc;
            break;
        }
        case LogFileOpenMode::COUNT : {
            open_mode |= std::ios::app;
            break;
        }
        default : {
            open_mode |= std::ios::app;
            break;
        }
    }
    
    if (this->m_file_stream.is_open())    { this->m_file_stream.close(); }
    this->m_file_stream.clear();
    this->m_file_stream.open(this->m_file_path, open_mode);
    return;
}


//  "file_sink_close"
//
void Logger::file_sink_close(void)
{
    if (this->m_file_stream.is_open())    { this->m_file_stream.close(); }
    this->m_file_stream.clear();
    return;
}


//  "file_sink_recover"
//
void Logger::file_sink_recover(void)
{
    this->file_sink_close();
    this->file_sink_open();
    
    if (!this->m_file_stream.is_open() || !this->m_file_stream.good()) {
        std::lock_guard<std::mutex>     lg          (this->m_sink_cfg_mtx);
        
        this->m_sink_states[LogSinkType::File].enabled = false;
    }
    return;
}


//  "file_sink_rotate_if_needed"
//
void Logger::file_sink_rotate_if_needed(const size_t pending_bytes)
{
    namespace                   fs              = std::filesystem;
    std::error_code             ec              ;
    uintmax_t                   current_size    = 0ULL;
    
    if (Logger::cv_DEF_FILE_SINK_MAX_BYTES == 0ULL)    { return; }

    if (fs::exists(this->m_file_path, ec)) {
        current_size = fs::file_size(this->m_file_path, ec);
        if (ec) {
            this->accounting_note_file_failure();
            ec.clear();
            current_size = 0ULL;
        }
    }
    else {
        ec.clear();
    }
    
    if ((current_size + pending_bytes) <= Logger::cv_DEF_FILE_SINK_MAX_BYTES)    { return; }
    
    this->file_sink_close();
    this->file_sink_rotate_files();
    this->file_sink_open();
    
    if (!this->m_file_stream.is_open() || !this->m_file_stream.good()) {
        this->accounting_note_file_failure();
        this->file_sink_recover();
    }
    return;
}


//  "file_sink_rotate_files"
//
void Logger::file_sink_rotate_files(void)
{
    namespace                   fs                  = std::filesystem;
    const size_t                max_backup_count    = Logger::cv_DEF_FILE_SINK_BACKUP_COUNT;
    std::error_code             ec                  ;
    
    if (max_backup_count == 0ULL) {
        if (fs::exists(this->m_file_path, ec)) {
            fs::remove(this->m_file_path, ec);
            if (ec)    { this->accounting_note_file_failure(); }
        }
        return;
    }
    
    {
        const std::string       oldest_path         = this->file_sink_backup_path(max_backup_count);
        
        if (fs::exists(oldest_path, ec)) {
            fs::remove(oldest_path, ec);
            if (ec)    { this->accounting_note_file_failure(); }
            ec.clear();
        }
    }
    
    for (size_t idx = max_backup_count; idx > 0ULL; --idx)
    {
        const std::string       source_path         = (idx == 1ULL) ? this->m_file_path : this->file_sink_backup_path(idx - 1ULL);
        const std::string       target_path         = this->file_sink_backup_path(idx);
        
        if (!fs::exists(source_path, ec)) {
            ec.clear();
            continue;
        }
        
        if (fs::exists(target_path, ec)) {
            fs::remove(target_path, ec);
            if (ec)    { this->accounting_note_file_failure(); }
            ec.clear();
        }
        
        fs::rename(source_path, target_path, ec);
        if (ec) {
            this->accounting_note_file_failure();
            ec.clear();
        }
    }
    return;
}


//  "file_sink_backup_path"
//
[[nodiscard]] std::string Logger::file_sink_backup_path(const size_t index) const
{
    return this->m_file_path + "." + std::to_string(index);
}


//  "file_sink_build_record"
//      Member helper: build one stable persisted file-sink record
//
std::string Logger::file_sink_build_record(const LogEvent & ev)
{
    const LogSinkState      sink_state  = this->get_sink_state(LogSinkType::File);
    bool                    first       = true;
    LoggerFieldFlags        fmt         = sink_state.field_flags;
    size_t                  path_depth  = sink_state.path_depth;
    std::ostringstream      ss;
    
    
    ss << '[' << cv_HEADER     << ' '
       << std::left         << std::setw(cv_MAX_LEVEL_LEN) << Logger::ms_LOGLEVEL_NAME[ev.level]   << ' '
       << std::right        << std::setfill('0')        << std::setw(cv_COUNTER_WIDTH)             << ev.count
       << std::setfill(' ') << "] : "
       << ev.text;
    
    if (fmt & LoggerFieldFlags_ThreadID) {                                       //  1.  Thread ID.
        if (first)              { ss << '\n' << '('; }
        else                    { ss << ' ';         }
        ss << "thread: 0x" << tid_hex(ev.thread_id) << ".";
        first = false;
    }
    
    if ((fmt & LoggerFieldFlags_Function) && ev.func) {                           //  2.  Function-Name.
        if (first)              { ss << '\n' << '('; }
        else                    { ss << ' ';         }
        ss << "func: " << ev.func << ".";
        first = false;
    }
    
    if ((fmt & LoggerFieldFlags_File) && ev.file) {                               //  3.  Filename.
        if (first)              { ss << '\n' << '('; }
        else                    { ss << ' ';         }
        ss << "file: " << path_tail(ev.file, path_depth) << ".";
        first = false;
    }
    
    if ((fmt & LoggerFieldFlags_Line) && (ev.line > 0)) {                         //  4.  Line Number.
        if (first)              { ss << '\n' << '('; }
        else                    { ss << ' ';         }
        ss << "line: " << ev.line << ".";
        first = false;
    }
    
    if (fmt & LoggerFieldFlags_Timestamp) {                                       //  5.  Time-Stamp.
        if (first)              { ss << '\n' << '('; }
        else                    { ss << ' ';         }
        ss << ev.ts_iso8601;
        first = false;
    }
    
    if (!first)    { ss << ')'; }
    
    ss << '\n';
    return ss.str();
}


//  "file_sink_write_event"
//      File sink orchestrator: persist one event and flush it immediately
//
void Logger::file_sink_write_event(const LogEvent & ev)
{
    const std::string       record      = this->file_sink_build_record(ev);
    
    if (!this->m_file_stream.is_open()) {
        this->accounting_note_file_failure();
        this->file_sink_recover();
        if (!this->m_file_stream.is_open())    { return; }
    }
    
    this->file_sink_rotate_if_needed(record.size());
    if (!this->m_file_stream.is_open())        { return; }
    
    this->m_file_stream << record;
    this->m_file_stream.flush();
    
    if (!this->m_file_stream.good()) {
        this->accounting_note_file_failure();
        this->file_sink_recover();
        if (!this->m_file_stream.is_open())    { return; }
        
        this->file_sink_rotate_if_needed(record.size());
        if (!this->m_file_stream.is_open())    { return; }
        
        this->m_file_stream << record;
        this->m_file_stream.flush();
        if (!this->m_file_stream.good()) {
            this->accounting_note_file_failure();
            this->file_sink_recover();
        }
    }
    return;
}



//
//
// *************************************************************************** //   END [[ 3B.  "FILE SINK" ]].



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
//
//
//
// *************************************************************************** //
// *************************************************************************** //
// *************************************************************************** //
#endif  //  CBAPP_LOG_ENABLED   //



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 3.  "SINK FUNCTIONS" ]].











// *************************************************************************** //
//
//
//
//          4.      UTILITY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //



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



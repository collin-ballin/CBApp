/***********************************************************************************
*
*       ********************************************************************
*       ****               L O G G E R . H  ____  F I L E               ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      May 12, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_UTILITY_LOGGER_H
#define _CBAPP_UTILITY_LOGGER_H  1



//  0.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
#include "json.hpp"


//  0.2     STANDARD LIBRARY HEADERS...
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstddef>
#include <string>
#include <string_view>

#include <chrono>
#include <ctime>

// ---------- compile‑time enable switch ----------
#define CBAPP_LOG_ENABLED 1


#ifdef _WIN32
    #include<windows.h>
  #include <time.h>
#endif  //  _WIN32  //


#if CBAPP_LOG_ENABLED
    #include <queue>
    #include <thread>
    #include <mutex>
    #include <condition_variable>
    #include <atomic>
    #include <array>
#endif

#include <format>
//  #ifndef __cpp_lib_format
//    #include <fmt/format.h>
//    namespace std { using fmt::format; }
//  #endif
        


namespace cb { namespace utl { //     BEGINNING NAMESPACE "cb" :: "utl"...
// *************************************************************************** //
// *************************************************************************** //



//      1.  STRUCTS FOR LOGGER CLASS...
// *************************************************************************** //
// *************************************************************************** //

// ---------------------------------------------------------------------
//  ConsoleFormat – bit-flags that tell write_event() what to print
// ---------------------------------------------------------------------
enum ConsoleField : unsigned {
    CF_FILE       = 1u << 0,   //   "path/to/file.cpp"
    CF_LINE       = 1u << 1,   //   ":123"
    CF_FUNCTION   = 1u << 2,   //   "my_func"
    CF_THREAD_ID  = 1u << 3,   //   "tid=12345"
    CF_TIMESTAMP  = 1u << 4    //   ISO-8601
};

//  using ConsoleField      = usigned
inline static constexpr     unsigned    CF_DEFAULT  = CF_FILE | CF_LINE | CF_FUNCTION | CF_THREAD_ID | CF_TIMESTAMP;
      
      
//  "LogLevel"
//
enum class LogLevel : int {
    None,
    Debug           = 10,
    Info            = 20,
    Warning         = 30,
    Exception       = 35,
    Error           = 40,
    Notify          = 45,
    Critical        = 50,
    Count
};


//  "TermColor"
//
enum class TermColor : unsigned {
    Reset, Grey, Green, Yellow, Red, Magenta, Cyan
};

static constexpr const char* ansi_code(TermColor c) {
    switch (c) {
        case TermColor::Grey:    return "\x1b[90m";
        case TermColor::Green:   return "\x1b[32m";
        case TermColor::Yellow:  return "\x1b[33m";
        case TermColor::Red:     return "\x1b[31m";
        case TermColor::Magenta: return "\x1b[35m";
        case TermColor::Cyan:    return "\x1b[36m";
        default:                 return "\x1b[0m";   // Reset
    }
}


//  "LogCounter"
//      - Simple struct to maintain a count of each log invocation.
struct LogCounter {
    size_t      none            = 0ULL;
    size_t      debug           = 0ULL;
    size_t      info            = 0ULL;
    size_t      warning         = 0ULL;
    size_t      exception       = 0ULL;
    size_t      error           = 0ULL;
    size_t      notify          = 0ULL;
    size_t      critical        = 0ULL;
    size_t      unknown         = 0ULL;
};


//  "LogEvent"
//
struct LogEvent {
    LogLevel            level;
    std::string         text;
    std::size_t         count;       // running counter

    // optional metadata
    const char*         file;
    int                 line;
    const char*         func;
    std::thread::id     thread_id;
    std::string         ts_iso8601;
};



// *************************************************************************** //
//
//
//      2.  INLINE HELPER FUNCTIONS FOR LOGGER CLASS...
// *************************************************************************** //
// *************************************************************************** //

//  "iso_timestamp"
//
inline std::string iso_timestamp(void)
{
    using namespace std::chrono;

    const auto  now   = system_clock::now();
    const auto  secs  = time_point_cast<seconds>(now);               // truncate
    const auto  ms    = duration_cast<milliseconds>(now - secs).count(); // 0-999

    std::time_t tt = system_clock::to_time_t(secs);
    std::tm      tm;
#if defined(_WIN32)
    gmtime_s(&tm, &tt);          // Windows
#else
    gmtime_r(&tt, &tm);          // POSIX / macOS / Linux
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S")
        << '.' << std::setw(3) << std::setfill('0') << ms
        << 'Z';
    return oss.str();
}
/*
{
    using namespace std::chrono;

    const auto   now      = system_clock::now();
    const auto   sec_part = time_point_cast<seconds>(now);
    const auto   us_part  = duration_cast<microseconds>(now - sec_part).count();

    std::time_t  tt = system_clock::to_time_t(now);
    std::tm      tm;
#if defined(_WIN32)
    gmtime_s(&tm, &tt);          // Windows
#else
    gmtime_r(&tt, &tm);          // POSIX / macOS / Linux
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S")
        << '.' << std::setw(6) << std::setfill('0') << us_part
        << 'Z';                 // UTC designator
    return oss.str();
}
*/





// *************************************************************************** //
//
//
//      3.  LOGGER CLASS...
// *************************************************************************** //
// *************************************************************************** //

class Logger
{
// *************************************************************************** //
// *************************************************************************** //
public:
    //  0.               PUBLIC CLASS-NESTED ALIASES...
    // *************************************************************************** //
    using                   Level                       = LogLevel;
    using                   TermColor                   = TermColor;
    using                   ConsoleFormat               = unsigned;

    
    //  1.               PUBLIC MEMBER FUNCTIONS...
    // *************************************************************************** //
    // *************************************************************************** //
    
    //
    //
    //  1.2A            Public API Logging Functions (Part 1/3)...
    // *************************************************************************** //
    static inline Logger &  instance                    (void)                                      { static Logger inst; return inst; }
    
    //  "log_ex"
    //
    template<class... Args>
    inline void             log_ex( std::format_string<Args...> fmt,
                                    Level lvl,
                                    const char* file,
                                    int line,
                                    const char* func,
                                    std::thread::id tid,
                                    Args&&... args ) {
    #if CBAPP_LOG_ENABLED
        std::string msg = std::format(fmt, std::forward<Args>(args)...);
        enqueue_event(LogEvent{
            lvl, std::move(msg), next_count(lvl),
            file, line, func, tid, iso_timestamp()
        });
    #else
        (void)fmt; (void)lvl; (void)file; (void)line; (void)func; (void)tid;
    #endif
    }
    
    //  "log_ex"
    //
    inline void             log_ex( std::string_view msg,
                                    Level           lvl,
                                    const char*     file,
                                    int             line,
                                    const char*     func,
                                    std::thread::id tid ) {
        #if CBAPP_LOG_ENABLED
            enqueue_event(LogEvent{
                lvl, std::string(msg), next_count(lvl),
                file, line, func, tid, iso_timestamp()
            });
        #else
            (void)msg; (void)lvl; (void)file; (void)line; (void)func; (void)tid;
        #endif
    }
    //
    //
    //  1.2B            Public API Logging Functions (Part 2/3)...
    // *************************************************************************** //

    //  "logf"
    //
    template<class... Args>
    inline void             logf                        (Level lvl, std::format_string<Args...> fmt, Args&&... args) {
    #if CBAPP_LOG_ENABLED
        std::string msg = std::format(fmt, std::forward<Args>(args)...);
        log(msg.c_str(), lvl);
    #else
            (void)fmt; (void)sizeof...(args);
    #endif
    }
    
    //  "logf"
    //
    template<typename... Args>
    inline void logf(const std::string& fmt_str, Args&&... args) {
        auto    s = std::format(fmt_str, std::forward<Args>(args)...);
        write_to_sink(s);
    }
    //
    //
    //  1.2C            Public API Logging Functions (Part 3/3)...
    // *************************************************************************** //
    
    void                    log                         (const char * , Level );
    void                    log                         (const std::string & , Level );
    template<class... Args>
    inline void             debugf                      (std::format_string<Args...> f, Args&&... a)    { logf(Level::Debug,        f, std::forward<Args>(a)...);   }
    void                    debug                       (const char * );
    void                    debug                       (const std::string & );
    template<class... Args>
    inline void             infof                       (std::format_string<Args...> f, Args&&... a)    { logf(Level::Info,         f, std::forward<Args>(a)...);   }
    void                    info                        (const char * );
    void                    info                        (const std::string & );
    template<class... Args>
    inline void             warningf                    (std::format_string<Args...> f, Args&&... a)    { logf(Level::Warning,      f, std::forward<Args>(a)...);   }
    void                    warning                     (const char * );
    void                    warning                     (const std::string & );
    template<class... Args>
    inline void             exceptionf                  (std::format_string<Args...> f, Args&&... a)    { logf(Level::Exception,    f, std::forward<Args>(a)...);   }
    void                    exception                   (const char * );
    void                    exception                   (const std::string & );
    template<class... Args>
    inline void             errorf                      (std::format_string<Args...> f, Args&&... a)    { logf(Level::Error,        f, std::forward<Args>(a)...);   }
    void                    error                       (const char * );
    void                    error                       (const std::string & );
    template<class... Args>
    inline void             notifyf                     (std::format_string<Args...> f, Args&&... a)    { logf(Level::Notify,       f, std::forward<Args>(a)...);   }
    void                    notify                      (const char * );
    void                    notify                      (const std::string & );
    template<class... Args>
    inline void             criticalf                   (std::format_string<Args...> f, Args&&... a)    { logf(Level::Critical,     f, std::forward<Args>(a)...);   }
    void                    critical                    (const char * );
    void                    critical                    (const std::string & );

                        
    //
    //
    //  1.3                 Public Utility Functions...
    // *************************************************************************** //
    
    void                    set_level                   (const Level &);
    Level                   get_level                   (void)  const;
    void                    set_console_format          (ConsoleFormat );
    ConsoleFormat           get_console_format          (void) const;
    void                    set_path_depth              (std::size_t d);              // runtime knob
    std::size_t             get_path_depth              (void) const;


    //  1.4                 Deleted Operators, Functions, etc...
                            Logger                      (const Logger & )           = delete;
                            Logger                      (Logger && )                = delete;
    Logger &                operator =                  (const Logger &)            = delete;
    Logger &                operator =                  (Logger && )                = delete;


// *************************************************************************** //
// *************************************************************************** //
protected:
    //  2.A             PROTECTED DATA-MEMBERS...
    // *************************************************************************** //
    
    //  CONSTANTS...
    static constexpr const char *           HEADER                      = "CBLOG";      //  prefix
    static constexpr std::size_t            COUNTER_WIDTH               = 3;            //  zero‑pad width
    static constexpr std::size_t            MAX_LEVEL_LEN               = 9;            //  strlen("EXCEPTION")
    static constexpr std::size_t            QUEUE_CAPACITY              = 1024;         //  bounded queue
    static constexpr std::size_t            MAX_COL_WIDTH               = 120;          //  wrap column
    static constexpr const char *           BODY_OPEN_DELIM             = "";
    static constexpr const char *           BODY_CLOSE_DELIM            = "";
    
    //  DATA...
    std::array< std::size_t, static_cast<int>(Level::Count) >
                                            m_counts                    = { };
    std::queue<LogEvent>                    m_queue;
    std::mutex                              m_mtx;
    std::condition_variable                 m_cv;
    std::thread                             m_worker;
    std::atomic<bool>                       m_running                   {false};
    std::atomic<bool>                       m_vt_enabled                {false};        //  set once in ctor
    std::atomic<std::size_t>                m_path_depth                { 2ULL };       //  default “.../dir/file.cpp”
#if defined(__CBAPP_DEBUG__) || defined(__CBLIB_RELEASE_WITH_DEBUG_INFO__)
    Level                                   m_threshold                 = Level::Debug;
    std::atomic<ConsoleFormat>              m_console_fmt               { CF_DEFAULT };
# else
    Level                                   m_threshold                 = Level::Info;
    std::atomic<ConsoleFormat>              m_console_fmt               { CF_DEFAULT };
#endif  //  __CBAPP_DEBUG__ || __CBLIB_RELEASE_WITH_DEBUG_INFO__  //
    
    
    
    //  2.B                 PROTECTED MEMBER FUNCTIONS...
    // *************************************************************************** //
    // *************************************************************************** //
    
    //  2B.1                Class Initializations.          [Logger.cpp]...     ---Private for Singleton.
                            Logger                          (void);                     //  Def. Constructor.
                            ~Logger                         (void);                     //  Def. Destructor.
                        
                        
    
    //  2B.2            Inline Class Utility Functions.
    // *************************************************************************** //
    inline static constexpr
    const char *                        level_str                       (Level lvl) {
        switch (lvl) {
            case Level::None        : { return "NONE";      }
            case Level::Debug       : { return "DEBUG";     }
            case Level::Info        : { return "INFO";      }
            case Level::Warning     : { return "WARNING";   }
            case Level::Exception   : { return "EXCEPTION"; }
            case Level::Error       : { return "ERROR";     }
            case Level::Notify      : { return "NOTIFY";    }
            case Level::Critical    : { return "CRITICAL";  }
            default                 : { return "UNKNOWN";   }
        }
    }
    [[nodiscard]] inline std::size_t    next_count                      (Level lvl)     { return ++m_counts[static_cast<int>(lvl)]; }
                        
    
    //  2B.3                            Class Utility Functions.        [Logger.cpp]...
    void                                enqueue                         (const char * , Level );
    void                                enqueue_event                   (LogEvent && );
    void                                start_worker                    (void);
    void                                stop_worker                     (void);
    //
    //
    void                                write_event                     (const LogEvent & ev);
    //
    std::string                         build_header                    (const LogEvent & );
    void                                write_body                      (const std::string & , std::ostream & out, std::size_t ) const;
    std::string                         build_metadata                  (const LogEvent & , std::size_t );
    static std::string                  path_tail                       (std::string_view full, std::size_t depth);
    //
    TermColor                           level_to_color                  (Level lvl);
    void                                enable_vt_win                   (void);
    
    



// *************************************************************************** //
// *************************************************************************** //
};//	END "Logger" CLASS PROTOTYPE.









// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" NAMESPACE.






#endif      //  _CBAPP_UTILITY_LOGGER_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.

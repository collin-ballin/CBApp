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


//      0.1.    SYSTEM INCLUDES...
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstddef>
#include <string>
#include <string_view>

#include <chrono>
#include <ctime>
#include <format>
#include <type_traits>
#include <utility>



//      0.2.    WINDOWS-ONLY INCLUDES...
#ifdef _WIN32
    #include <windows.h>
    #include <time.h>
#endif  //  _WIN32  //



//      0.3.    *MY* INCLUDES...
#include CBAPP_USER_CONFIG
#include "cblib.h"
#include "json.hpp"
#include "cblib.h"



//      0.4.    compile‑time enable switch...
#define CBAPP_LOG_ENABLED 1

#if CBAPP_LOG_ENABLED
    #include <queue>
    #include <thread>
    #include <mutex>
    #include <condition_variable>
    #include <atomic>
    #include <array>
#endif  //  CBAPP_LOG_ENABLED  //


        



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
namespace anon { //     BEGINNING NAMESPACE "anon"...



// *************************************************************************** //
//      "anon" |    ENUMS.
// *************************************************************************** //

//  "ConsoleFormat"
//      bit-flags that tell write_event() what to print.
//
enum LoggerFieldFlags : uint16_t {
      LoggerFieldFlags_File         = 1U << 0           //  source filename         "path/to/file.cpp"
    , LoggerFieldFlags_Line         = 1U << 1           //  filename line num.      ":123"
    , LoggerFieldFlags_Function     = 1U << 2           //  function name           "my_func_name"
    , LoggerFieldFlags_ThreadID     = 1U << 3           //  thread ID               "tid=12345"
    , LoggerFieldFlags_Timestamp    = 1U << 4           //  ISO timestamp           ISO-8601
    , LoggerFieldFlags_COUNT                            //
//
    , LoggerFieldFlags_Basic        = LoggerFieldFlags_File | LoggerFieldFlags_Line | LoggerFieldFlags_Function
    , LoggerFieldFlags_All          = LoggerFieldFlags_File | LoggerFieldFlags_Line | LoggerFieldFlags_Function | LoggerFieldFlags_ThreadID | LoggerFieldFlags_Timestamp
};


//  "LogType"
//
enum class LogType : uint8_t {
      None              = 0U
    , Debug
    , Info
    , Warning
    , Exception
    , Error
    , Notify
    , Critical
    , COUNT
};


//  "TermColor"
//
enum class TermColor : uint8_t {
      None              = 0U
    , Grey
    , Green
    , Yellow
    , Red
    , Magenta
    , Cyan
    , COUNT
};



// *************************************************************************** //
//      "anon" |    COMPILE-TIME ARRAYS.
// *************************************************************************** //

//  "DEF_LOGLEVEL_VALUES"
//
static constexpr cblib::EnumArray< LogType, uint8_t >
DEF_LOGLEVELS                   = { {
      /*  None        */        0U
    , /*  Debug       */        10U
    , /*  Info        */        20U
    , /*  Warning     */        30U
    , /*  Exception   */        35U
    , /*  Error       */        40U
    , /*  Notify      */        45U
    , /*  Critical    */        50U
} };


//  "DEF_LOGLEVEL_NAMES"
//
static constexpr cblib::EnumArray< LogType, const char * >
DEF_LOGLEVEL_NAMES              = { {
      /*  None        */        "NONE"
    , /*  Debug       */        "DEBUG"
    , /*  Info        */        "INFO"
    , /*  Warning     */        "WARNING"
    , /*  Exception   */        "EXCEPTION"
    , /*  Error       */        "ERROR"
    , /*  Notify      */        "NOTIFY"
    , /*  Critical    */        "CRITICAL"
} };



//  "DEF_LOGLEVEL_TO_ASCII_COLOR"
//
static constexpr cblib::EnumArray< LogType, const char * >
DEF_LOGLEVEL_TO_ASCII_COLOR     = { {
      /*  None        */        "\x1b[0m"           //  Reset / default
    , /*  Debug       */        "\x1b[32m"          //  Green
    , /*  Info        */        "\x1b[33m"          //  Yellow
    , /*  Warning     */        "\x1b[31m"          //  Red
    , /*  Exception   */        "\x1b[36m"          //  Cyan
    , /*  Error       */        "\x1b[35m"          //  Magenta
    , /*  Notify      */        "\x1b[0m"           //  Reset / default
    , /*  Critical    */        "\x1b[35m"          //  Magenta
} };



//  "DEF_LOGLEVEL_TO_TERMCOLOR"
//
static constexpr cblib::EnumArray< LogType, TermColor >
DEF_LOGLEVEL_TO_TERMCOLOR       = { {
      /*  None        */        TermColor::None
    , /*  Debug       */        TermColor::Green
    , /*  Info        */        TermColor::Yellow
    , /*  Warning     */        TermColor::Red
    , /*  Exception   */        TermColor::Cyan
    , /*  Error       */        TermColor::Magenta
    , /*  Notify      */        TermColor::None
    , /*  Critical    */        TermColor::Magenta
} };




// *************************************************************************** //
//      "anon" |    COMPILE-TIME VALUES.
// *************************************************************************** //

static constexpr const char *       cv_HEADER                       = "CBLOG"                       ;   //  prefix for all log-messages
static constexpr size_t             cv_COUNTER_WIDTH                = 5ULL                          ;   //  zero-pad width for log count display
static constexpr size_t             cv_MAX_LEVEL_LEN                = 9ULL                          ;   //  strlen("EXCEPTION")
static constexpr size_t             cv_QUEUE_CAPACITY               = 1'024ULL                      ;   //  bounded queue
static constexpr size_t             cv_MAX_COL_WIDTH                = 120ULL                        ;   //  wrap column
static constexpr const char *       cv_BODY_OPEN_DELIM              = ""                            ;
static constexpr const char *       cv_BODY_CLOSE_DELIM             = ""                            ;
static constexpr const char *       cv_TIMESTAMP_FMT_STRING         = "%Y-%m-%dT%H:%M:%S";
//
//
static constexpr LogType            cv_LOGGER_THRESHOLD             = LogType::Info                 ;
static constexpr LoggerFieldFlags   cv_DEFAULT_LOGGER_FIELD_FLAGS   = LoggerFieldFlags_All          ;



// *************************************************************************** //
//      "anon" |    STRUCTS.
// *************************************************************************** //

//  "LogEvent"
//
/// @brief
/// Canonical destination-neutral log-event packet for the current logger.
///
/// This struct represents the abstract logging payload before any specific sink
/// or formatter renders it to an output medium.  At the current development
/// stage, the event still carries a rendered ISO-8601 timestamp string because
/// that is already consistent with the existing implementation and is not
/// currently considered a high-priority optimization target.
///
/// Important design intent:
/// - fields in this packet represent logical event data, not terminal-only
///   presentation artifacts,
/// - the normal asynchronous queue transports this packet,
/// - future sink-oriented refactors should treat this struct as the canonical
///   event record and render it per destination.
//
template <typename counter_type = size_t>
struct LogEvent {
                            //  LogEvent(
//
//
//
    LogType                 level                           ;       //  logical severity / category
    std::string             text                            ;       //  message body text
    counter_type            count                           ;       //  running event counter
//
//  Optional Metadata...
    const char *            file                            ;       //  source file path, if available
    int                     line                            ;       //  source line number, if available
    const char *            func                            ;       //  source function name, if available
    std::thread::id         thread_id                       ;       //  producer thread identity token
    std::string             ts_iso8601                      ;       //  current-stage rendered timestamp representation
};



// *************************************************************************** //
//      "anon" |    FUNCTIONS.
// *************************************************************************** //

//
//  ...
//



//
// *************************************************************************** //
// *************************************************************************** //   END [ 1.0.  "TYPES" ].



// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}// END NAMESPACE "anon".










// *************************************************************************** //
//
//
//
//      3.      LOGGER CLASS...
// *************************************************************************** //
// *************************************************************************** //

//  "Logger"
//
/// @brief
/// Centralized design-invariant block for the current `Logger` abstraction.
///
/// This comment documents the public-facing contract and implementation
/// invariants that are intended to remain stable while the logger is refactored
/// toward its future sink-oriented architecture.
///
/// CURRENT ROLE OF THIS CLASS:
/// - Process-global singleton logger accessed through `Logger::instance()`.
/// - Normal-path asynchronous logger using:
///   - producer threads,
///   - a shared bounded queue,
///   - a dedicated worker thread,
///   - queue-drain-to-output behavior.
/// - Enum-driven logger design using `cblib::EnumArray<E, T>` for compile-time
///   metadata mappings.
/// - Console-oriented output path at the current stage of development.
/// - Rich metadata support, including file / line / function / thread / timestamp.
///
/// PUBLIC API SURFACE THAT SHOULD REMAIN STABLE:
/// - `instance()`
/// - `log_ex(...)`
/// - `logf(...)`
/// - `log(...)`
/// - severity convenience functions:
///   - `debug*`
///   - `info*`
///   - `warning*`
///   - `exception*`
///   - `error*`
///   - `notify*`
///   - `critical*`
/// - runtime configuration functions:
///   - `set_level()` / `get_level()`
///   - `set_field_flags()` / `get_field_flags()`
///   - `set_path_depth()` / `get_path_depth()`
///
/// CURRENT NORMAL-PATH BEHAVIORAL INVARIANTS:
/// - The ordinary logging path remains asynchronous.
/// - The queue remains bounded and blocking in the current development stage.
/// - Silent drop-on-full behavior is NOT the current preferred policy.
/// - Console hanging-indent formatting remains supported.
/// - The logger remains enum-driven rather than map-driven or string-key-driven.
///
/// FUTURE-REFactor INVARIANTS:
/// - Future sink / destination support should be added behind the existing
///   front-end API wherever practical.
/// - Per-destination behavior is the intended long-term design direction when a
///   feature sensibly belongs to a sink rather than to the logger globally.
/// - Persistent file-backed logging is a required future capability.
/// - Severe / failure-path synchronous write-and-flush behavior is intended to
///   be additive and must not replace the normal asynchronous path.
/// - Future overload / sacrifice policy, if ever introduced, must preserve
///   high-value diagnostics preferentially and must account for loss explicitly.
///
/// This block exists to reduce architectural drift during future roadmap work.
//
class Logger
{
//      0.          CONSTANTS AND ALIASES...
// *************************************************************************** //
// *************************************************************************** //
public:

    // *************************************************************************** //
    //      0. |    NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    using                                counter_type                       = size_t                                        ;
    //
    using                                LogType                            = anon::LogType                                 ;
    using                                LoggerFieldFlags                   = anon::LoggerFieldFlags                        ;
    using                                enum                               anon::LoggerFieldFlags                          ;   //  cpp-20 feature.
    //
    using                                LogEvent                           = anon::LogEvent< counter_type >                ;

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
    //      1. |    STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    static constexpr const char *           cv_HEADER                           = anon::cv_HEADER                   ;   //  prefix for all log-messages
    static constexpr size_t                 cv_COUNTER_WIDTH                    = anon::cv_COUNTER_WIDTH            ;   //  zero-pad width for log count display
    static constexpr size_t                 cv_MAX_LEVEL_LEN                    = anon::cv_MAX_LEVEL_LEN            ;   //  strlen("EXCEPTION")
    static constexpr size_t                 cv_QUEUE_CAPACITY                   = anon::cv_QUEUE_CAPACITY           ;   //  bounded queue
    static constexpr size_t                 cv_MAX_COL_WIDTH                    = anon::cv_MAX_COL_WIDTH            ;   //  wrap column
    static constexpr const char *           cv_BODY_OPEN_DELIM                  = anon::cv_BODY_OPEN_DELIM          ;
    static constexpr const char *           cv_BODY_CLOSE_DELIM                 = anon::cv_BODY_CLOSE_DELIM         ;
    static constexpr const char *           cv_TIMESTAMP_FMT_STRING             = anon::cv_TIMESTAMP_FMT_STRING     ;
    //
    static constexpr LoggerFieldFlags       cv_DEFAULT_FIELDS                   = LoggerFieldFlags_All              ;  //  ...
    static constexpr LogType                cv_DEFAULT_LOGGER_THRESHOLD         = anon::cv_LOGGER_THRESHOLD         ;
  
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      1. |    REFERENCES TO GLOBAL ARRAYS.
    // *************************************************************************** //
    static constexpr auto &                 ms_LOGLEVEL_VALUE                   = anon::DEF_LOGLEVELS                   ;
    static constexpr auto &                 ms_LOGLEVEL_NAME                    = anon::DEF_LOGLEVEL_NAMES              ;
    static constexpr auto &                 ms_LOGLEVEL_TO_ASCII_COLOR          = anon::DEF_LOGLEVEL_TO_ASCII_COLOR     ;
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      1. |    DATA MEMBERS...
    // *************************************************************************** //
    //
    //                  TRANSIENT STATE VARIABLES:
    //
    //
    //                  LOGGER-STATE / BEHAVIOR-TOGGLES:
    std::atomic<bool>                           m_running                       { false };
    std::atomic<bool>                           m_vt_enabled                    { false };       //  set once in ctor
    //
    //
    //                  LOGGER FORMATTING:
    std::atomic<LogType>                        m_threshold                     { Logger::cv_DEFAULT_LOGGER_THRESHOLD };
    std::atomic<LoggerFieldFlags>               m_fields                        { Logger::cv_DEFAULT_FIELDS };
    std::atomic<size_t>                         m_path_depth                    { 2ULL };        //  default “.../dir/file.cpp”
    //
    //
    //                  CONCURRENCY STUFF:
    std::mutex                                  m_counts_mtx                    ;
    std::mutex                                  m_mtx                           ;
    std::condition_variable                     m_cv                            ;
    std::thread                                 m_worker                        ;
    std::queue<LogEvent>                        m_queue                         ;
    //
    //
    //                  IMPORTANT DATA:
    cblib::EnumArray< LogType, counter_type >   m_counts                        = { { counter_type(0) } };
    //  std::array< counter_type, static_cast<size_t>(LogType::COUNT) >
    //                                          m_counts                        = { };



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
    //      2.A. |  PUBLIC API LOGGING FUNCTIONS (PART 1/3)...
    // *************************************************************************** //
    static inline Logger &                      instance                        (void)                                      { static Logger inst; return inst; }
    
    //  "log_ex"
    //
    template<class... Args>
    inline void                                 log_ex                          (std::format_string<Args...>    fmt
                                                                                , const LogType                 type
                                                                                , const char *                  file
                                                                                , int                           line
                                                                                , const char *                  func
                                                                                , std::thread::id               tid
                                                                                , Args &&...                    args)
    {
        
    #if CBAPP_LOG_ENABLED
        std::string msg = std::format(fmt, std::forward<Args>(args)...);
        enqueue_event(LogEvent{
              type
            , std::move(msg)
            , this->next_count(type)
            , file
            , line
            , func
            , tid
            , Logger::s_iso_timestamp()
        });
    #else
        (void)fmt; (void)type; (void)file; (void)line; (void)func; (void)tid; (void)sizeof...(args);
    #endif  //  CBAPP_LOG_ENABLED  //
        return;
    }
    
    //  "log_ex"
    //
    inline void                                 log_ex                          (std::string_view               msg
                                                                                , const LogType                 type
                                                                                , const char *                  file
                                                                                , int                           line
                                                                                , const char *                  func
                                                                                , std::thread::id               tid)
    {
    #if CBAPP_LOG_ENABLED
        enqueue_event(LogEvent{
              type
            , std::string(msg)
            , this->next_count(type)
            , file
            , line
            , func
            , tid
            , Logger::s_iso_timestamp()
        });
    #else
        (void)msg; (void)type; (void)file; (void)line; (void)func; (void)tid;
    #endif  //  CBAPP_LOG_ENABLED  //
        return;
    }
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.A. |  PUBLIC API LOGGING FUNCTIONS (PART 2/3)...
    // *************************************************************************** //
    
    //  "logf"
    //
    template<class... Args>
    inline void                                 logf                            (const LogType                  type
                                                                                , std::format_string<Args...>   fmt
                                                                                , Args && ...                   args)
    {
    #if CBAPP_LOG_ENABLED
        std::string msg = std::format(fmt, std::forward<Args>(args)...);
        log(msg, type);
    #else
        (void)type; (void)fmt; (void)sizeof...(args);
    #endif  //  CBAPP_LOG_ENABLED  //
        return;
    }
    
    //  "logf"
    //
    template<typename... Args>
    inline void                                 logf                            (const std::string &            fmt_str
                                                                                , Args && ...                   args)
    {
    #if CBAPP_LOG_ENABLED
        std::string msg = std::vformat(fmt_str, std::make_format_args(args...));
        log(msg, LogType::Info);
    #else
        (void)fmt_str; (void)sizeof...(args);
    #endif  //  CBAPP_LOG_ENABLED  //
        return;
    }
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.A. |  PUBLIC API LOGGING FUNCTIONS (PART 3/3)...
    // *************************************************************************** //
    void                                        log                             (const char * , LogType );
    void                                        log                             (const std::string & , LogType );
    
    template<class... Args>
    inline void                                 debugf                          (std::format_string<Args...> f, Args&&... a)    { logf(LogType::Debug, f, std::forward<Args>(a)...);       }
    void                                        debug                           (const char * );
    void                                        debug                           (const std::string & );
    
    template<class... Args>
    inline void                                 infof                           (std::format_string<Args...> f, Args&&... a)    { logf(LogType::Info, f, std::forward<Args>(a)...);        }
    void                                        info                            (const char * );
    void                                        info                            (const std::string & );
    
    template<class... Args>
    inline void                                 warningf                        (std::format_string<Args...> f, Args&&... a)    { logf(LogType::Warning, f, std::forward<Args>(a)...);     }
    void                                        warning                         (const char * );
    void                                        warning                         (const std::string & );
    
    template<class... Args>
    inline void                                 exceptionf                      (std::format_string<Args...> f, Args&&... a)    { logf(LogType::Exception, f, std::forward<Args>(a)...);   }
    void                                        exception                       (const char * );
    void                                        exception                       (const std::string & );
    
    template<class... Args>
    inline void                                 errorf                          (std::format_string<Args...> f, Args&&... a)    { logf(LogType::Error, f, std::forward<Args>(a)...);       }
    void                                        error                           (const char * );
    void                                        error                           (const std::string & );
    
    template<class... Args>
    inline void                                 notifyf                         (std::format_string<Args...> f, Args&&... a)    { logf(LogType::Notify, f, std::forward<Args>(a)...);      }
    void                                        notify                          (const char * );
    void                                        notify                          (const std::string & );
    
    template<class... Args>
    inline void                                 criticalf                       (std::format_string<Args...> f, Args&&... a)    { logf(LogType::Critical, f, std::forward<Args>(a)...);    }
    void                                        critical                        (const char * );
    void                                        critical                        (const std::string & );
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.A. |  PUBLIC SETTER/GETTER FUNCTIONS...
    // *************************************************************************** //
    void                                        set_level                       (const LogType & level) noexcept            { this->m_threshold.store(level, std::memory_order_relaxed);   }
    void                                        set_field_flags                 (const LoggerFieldFlags flags) noexcept     { this->m_fields.store(flags, std::memory_order_relaxed);      }
    void                                        set_path_depth                  (const size_t depth) noexcept               { this->m_path_depth.store(depth, std::memory_order_relaxed);  }
    
    [[nodiscard]] LogType                       get_level                       (void) const noexcept                       { return this->m_threshold.load(std::memory_order_relaxed);    }
    [[nodiscard]] LoggerFieldFlags              get_field_flags                 (void) const noexcept                       { return this->m_fields.load(std::memory_order_relaxed);       }
    [[nodiscard]] size_t                        get_path_depth                  (void) const noexcept                       { return this->m_path_depth.load(std::memory_order_relaxed);   }
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.A. |  DELETED OPERATORS, FUNCTIONS, ETC...
    // *************************************************************************** //
                                                Logger                          (const Logger & )           = delete;
                                                Logger                          (Logger && )                = delete;
    Logger &                                    operator =                      (const Logger & )           = delete;
    Logger &                                    operator =                      (Logger && )                = delete;


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
    //      2.B. |  CLASS INITIALIZATIONS.          [Logger.cpp]...     ---Private for Singleton.
    // *************************************************************************** //
                                                Logger                          (void);                     //  Def. Constructor.
                                                ~Logger                         (void);                     //  Def. Destructor.
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.B. |  CLASS UTILITY FUNCTIONS.        [Logger.cpp]...
    // *************************************************************************** //
    void                                        enqueue                         ([[maybe_unused]] const char * , [[maybe_unused]] const LogType );
    void                                        enqueue_event                   (LogEvent && );
    void                                        start_worker                    (void);
    void                                        stop_worker                     (void);
    //
    //
    void                                        write_event                     (const LogEvent & );
    //
    [[nodiscard]] std::string                   build_header                    (const LogEvent & );
    void                                        write_body                      (const std::string & , std::ostream & , size_t ) const;
    [[nodiscard]] std::string                   build_metadata                  (const LogEvent & , size_t );
    [[nodiscard]] static std::string            path_tail                       (std::string_view , size_t );
    //
    void                                        enable_vt_win                   (void);

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
protected:
    
    // *************************************************************************** //
    //      2.C. |  STATIC INLINE FUNCTIONS.
    // *************************************************************************** //
    
    //  "iso_timestamp"
    [[nodiscard]] static std::string            s_iso_timestamp                 (const char * fmt_string=Logger::cv_TIMESTAMP_FMT_STRING)
    {
        using                   namespace       std::chrono;
        const auto              now             = system_clock::now();
        const auto              secs            = time_point_cast<seconds>(now);                        //  truncate
        const auto              ms              = duration_cast<milliseconds>(now - secs).count();      //  0-999
        std::time_t             tt              = system_clock::to_time_t(secs);
        std::tm                 tm              ;
        std::ostringstream      oss             ;

    #ifdef _WIN32
        gmtime_s(&tm, &tt);          // Windows
    #else
        gmtime_r(&tt, &tm);          // POSIX / macOS / Linux
    #endif  //  _WIN32  //

        oss << std::put_time(&tm, fmt_string)
            << '.' << std::setw(3) << std::setfill('0') << ms
            << 'Z';
            
        return oss.str();
    }
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.C. |  INLINE CLASS UTILITY FUNCTIONS.
    // *************************************************************************** //
    [[nodiscard]] inline counter_type           next_count                      (const LogType level)
    {
        std::lock_guard<std::mutex>             lg                              (this->m_counts_mtx);
        
        return ++this->m_counts[level];
    }
    //
    //
    //  [[nodiscard]] inline std::size_t    next_count                      (const LogType lvl) noexcept       { return ++this->m_counts[static_cast<int>(lvl)]; }
    
//
//
// *************************************************************************** //
// *************************************************************************** //   END "2C.  INLINE" FUNCTIONS.



// *************************************************************************** //
// *************************************************************************** //
};//	END "Logger" INLINE CLASS DEFINITION.












// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb::utl" NAMESPACE.






#endif      //  _CBAPP_UTILITY_LOGGER_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  EOF.

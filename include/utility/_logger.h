/***********************************************************************************
*
*       ********************************************************************
*       ****               L O G G E R . H  ____  F I L E               ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
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
#include "_config.h"
#include "cblib.h"
//  #include "utility/_constants.h"
//      #include "utility/_templates.h"
//  #include "utility/pystream/pystream.h"
//  #include "json.hpp"


//  0.2     STANDARD LIBRARY HEADERS...
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstddef>
#include <string>
#include <string_view>

// ---------- compile‑time enable switch ----------
//#ifdef __CBAPP_LOG__
    #define CBAPP_LOG_ENABLED 1
// #else
//     #define CBAPP_LOG_ENABLED 0
// #endif

#ifdef _WIN32
    #include<windows.h>
#endif  //  _WIN32  //


//#if CBAPP_LOG_ENABLED
    #include <queue>
    #include <thread>
    #include <mutex>
    #include <condition_variable>
    #include <atomic>
    #include <array>
//#endif


//  0.3     "DEAR IMGUI" HEADERS...
// #include "imgui.h"                      //  0.3     "DEAR IMGUI" HEADERS...
// #include "imgui_internal.h"
// #include "imgui_impl_glfw.h"
// #include "imgui_impl_opengl3.h"
// #include "implot.h"
// #if defined(IMGUI_IMPL_OPENGL_ES2)
// # include <GLES2/gl2.h>
// #endif      //  IMGUI_IMPL_OPENGL_ES2  //
// #include <GLFW/glfw3.h>     //  <======| Will drag system OpenGL headers



namespace cb { namespace utl { //     BEGINNING NAMESPACE "cb" :: "utl"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//
//
//  3.  LOGGER CLASS...
// *************************************************************************** //
// *************************************************************************** //

//  "LogLevel"
enum class LogLevel : int {
    None,
    Debug           = 10,
    Info            = 20,
    Warning         = 30,
    Exception       = 35,
    Error           = 40,
    Critical        = 50,
    Count
};


//  "LogCounter"
//      - Simple struct to maintain a count of each log invocation.
struct LogCounter {
    size_t      none            = 0ULL;
    size_t      debug           = 0ULL;
    size_t      info            = 0ULL;
    size_t      warning         = 0ULL;
    size_t      exception       = 0ULL;
    size_t      error           = 0ULL;
    size_t      critical        = 0ULL;
    size_t      unknown         = 0ULL;
};



class Logger
{
// *************************************************************************** //
// *************************************************************************** //
public:
    //  0.               PUBLIC CLASS-NESTED ALIASES...
    // *************************************************************************** //
    using                   Level                       = LogLevel;
    #if CBAPP_LOG_ENABLED
    struct                  LogEvent {
        Level                       level;
        std::string                 text;
        std::size_t                 count;
    };
    #endif

    
    //  1.               PUBLIC MEMBER FUNCTIONS...
    // *************************************************************************** //
    //  1.1             Default Constructor, Destructor, etc...
    static Logger &         instance                    (void);
                        
    //  1.2                 Public Member Functions...
    void                    log                         (const char * , Level );
    void                    debug                       (const char * );
    void                    info                        (const char * );
    void                    warning                     (const char * );
    void                    exception                   (const char * );
    void                    error                       (const char * );
    void                    critical                    (const char * );
    
    template<class... Args>
    inline void             debugf                      (std::format_string<Args...> f, Args&&... a)    { logf(Level::Debug,        f, std::forward<Args>(a)...);   }
    template<class... Args>
    inline void             infof                       (std::format_string<Args...> f, Args&&... a)    { logf(Level::Info,         f, std::forward<Args>(a)...);   }
    template<class... Args>
    inline void             warningf                    (std::format_string<Args...> f, Args&&... a)    { logf(Level::Warning,      f, std::forward<Args>(a)...);   }
    template<class... Args>
    inline void             exceptionf                  (std::format_string<Args...> f, Args&&... a)    { logf(Level::Exception,    f, std::forward<Args>(a)...);  }
    template<class... Args>
    inline void             errorf                      (std::format_string<Args...> f, Args&&... a)    { logf(Level::Error,        f, std::forward<Args>(a)...);   }
    template<class... Args>
    inline void             criticalf                   (std::format_string<Args...> f, Args&&... a)    { logf(Level::Critical,     f, std::forward<Args>(a)...);   }

    template<class... Args>
    inline void             logf                        (Level lvl, std::format_string<Args...> fmt, Args&&... args) {
    #if CBAPP_LOG_ENABLED
        std::string msg = std::format(fmt, std::forward<Args>(args)...);
        log(msg.c_str(), lvl);
    #else
            (void)fmt; (void)sizeof...(args);
    #endif
    }

                        
    //  1.3                 Public Utility Functions...
    void                    set_level                   (const Level &);
    Level                   get_level                   (void)  const;


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
    static constexpr const char *           HEADER                      = "CBLOG";      // prefix
    static constexpr std::size_t            COUNTER_WIDTH               = 3;            // zero‑pad width
    static constexpr std::size_t            MAX_LEVEL_LEN               = 9;            // strlen("EXCEPTION")
    static constexpr std::size_t            QUEUE_CAPACITY              = 1024;         // bounded queue
    
    //  DATA...
    std::array< std::size_t, static_cast<int>(Level::Count) >
                                            m_counts                    = { };
    Level                                   m_threshold                 = Level::None;
    std::queue<LogEvent>                    m_queue;
    std::mutex                              m_mtx;
    std::condition_variable                 m_cv;
    std::thread                             m_worker;
    std::atomic<bool>                       m_running{false};
    
    
    //  2.B                 PROTECTED MEMBER FUNCTIONS...
    // *************************************************************************** //
    
    //  2B.1                Class Initializations.          [Logger.cpp]...     ---Private for Singleton.
                            Logger                          (void);                     //  Def. Constructor.
                            ~Logger                         (void);                     //  Def. Destructor.
                        
    
    //  2B.2            Inline Class Utility Functions.
    inline static constexpr
    const char *            level_str(Level lvl) {
        switch (lvl) {
            case Level::None :          return "NONE";
            case Level::Debug :         return "DEBUG";
            case Level::Info :          return "INFO";
            case Level::Warning :       return "WARNING";
            case Level::Exception :     return "EXCEPTION";
            case Level::Error :         return "ERROR";
            case Level::Critical :      return "CRITICAL";
            default :                   return "UNKNOWN";
        }
    }
    inline std::size_t      next_count                      (Level lvl)
    { return ++m_counts[static_cast<int>(lvl)]; }
                        
    
    //  2B.3                Class Utility Functions.        [Logger.cpp]...
    void                    enqueue                         (const char * , Level );
    void                    start_worker                    (void);
    void                    stop_worker                     (void);
    static void             write_event                     (const LogEvent & ev);
    //void                    enable_vt_win                   (void);
    
    



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

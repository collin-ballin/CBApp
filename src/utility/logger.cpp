/***********************************************************************************
*
*       ********************************************************************
*       ****             L O G G E R . C P P  ____  F I L E             ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond
*               DATED:      May 11, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "utility/_logger.h"
#include CBAPP_USER_CONFIG



namespace cb { namespace utl { //     BEGINNING NAMESPACE "cb" :: "utl"...
// *************************************************************************** //
// *************************************************************************** //


//  1A.     INITIALIZATION  | DEFAULT CONSTRUCTOR, DESTRUCTOR, ETC...
// *************************************************************************** //
// *************************************************************************** //
    
//  Default Constructor.            | PRIVATE BECAUSE SINGLETON...
//
Logger::Logger(void) {
#if CBAPP_LOG_ENABLED
    //enable_vt_win();
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


//  "instance"
//
Logger & Logger::instance(void) {
    static Logger inst;
    return inst;
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
//
void  Logger::set_level(const Level & level)    { this->m_threshold = level; }


//  "get_level"
//
Logger::Level Logger::get_level(void) const     { return m_threshold; }







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
void Logger::enqueue(const char * msg, Level lvl)
{
    //  1.  APPLY FILTER...
    if ( static_cast<int>(lvl) < static_cast<int>(m_threshold) )
        return;
    
    std::unique_lock<std::mutex>    lock(m_mtx);
    
    m_cv.wait(lock, [this]{ return m_queue.size() < QUEUE_CAPACITY; });
    
    m_queue.push(LogEvent{lvl, msg, next_count(lvl)});
    
    
    lock.unlock();
    m_cv.notify_one();   // wake worker
    return;
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


//  "write_event"
//
void Logger::write_event(const LogEvent & ev)
{
    std::ostringstream os;
    
    os << '[' << HEADER << ' ';
    os << std::left  << std::setw(MAX_LEVEL_LEN) << level_str(ev.level) << ' ';
    os << std::right << std::setfill('0') << std::setw(COUNTER_WIDTH) << ev.count << std::setfill(' ') << ']';
    os << "    : \"" << ev.text << "\"";
    std::cout << os.str() << std::endl;
    
    return;
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
    // void Logger::enable_vt_win(void)
    // {
    //     HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    //     if (h == INVALID_HANDLE_VALUE) { m_colour_enabled = false; return; }
    //     DWORD mode = 0;
    //     if (!GetConsoleMode(h, &mode)) { m_colour_enabled = false; return; }
    //     if (!(mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING))
    //         SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    // }
# else
    //void Logger::enable_vt_win(void)    { }
#endif  //  _WIN32
//
// *************************************************************************** //
#endif






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



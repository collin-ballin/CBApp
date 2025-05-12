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
#include "utility/utility.h"
#include "utility/_constants.h"
#include "_config.h"



namespace cb { namespace utl { //     BEGINNING NAMESPACE "cb" :: "utl"...
// *************************************************************************** //
// *************************************************************************** //


/*
//  "CBLog"
//
void CBLog(const char * log, const LogLevel & level)
{
    return;
}


//  "CBLog_IMPL"
//
void CBLog_IMPL(const char * log, const LogLevel & level)
{
    using                       Level           = LogLevel;
    
    
    constexpr const char *      HEADER          = "CBLOG";
    static LogCounter           counts          = LogCounter();
    
    
    
    
    //  SWITCH FOR EACH LOG LEVEL CASE...
    switch (level)
    {
        case Level::None:       {   //  0.  NONE...
            ++counts.none;
            break;
        }
        case Level::Debug:      {   //  1.  DEBUG...
            ++counts.debug;
            break;
        }
        case Level::Info:       {   //  2.  INFO...
            ++counts.info;
            break;
        }
        case Level::Warning:    {   //  3.  WARNING...
            ++counts.warning;
            break;
        }
        case Level::Exception:  {   //  4.  EXCEPTION...
            ++counts.exception;
            break;
        }
        case Level::Error:      {   //  5.  ERROR...
            ++counts.error;
            break;
        }
        case Level::Critical:   {   //  6.  ERROR...
            ++counts.critical;
            break;
        }
        
        
        //  99. DEFAULT CASE...
        default:                {
            ++counts.unknown;
            break;
        }
    }
    
    
    

    return;
}

*/





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

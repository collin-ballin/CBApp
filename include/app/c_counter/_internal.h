/***********************************************************************************
*
*       ********************************************************************
*       ****            _ I N T E R N A L . H  ____  F I L E            ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      October 31, 2025.
*              MODULE:      CBAPP > CCOUNTER/           | _internal.h
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_COUNTER_INTERNAL_H
#define _CBAPP_COUNTER_INTERNAL_H  1
#include CBAPP_USER_CONFIG



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//      0.1.        ** MY **  HEADERS...
#include "cblib.h"
//  #include "widgets/widgets.h"
//  #include "utility/utility.h"
//  #include "utility/pystream/pystream.h"
//
#include "app/c_counter/_utility.h"


//      0.2         STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <cstdlib>          // C-Headers...
#include <stdio.h>
#include <unistd.h>
#include <random>
#include <tuple>
#include <utility>
#include <algorithm>

#include <string>           //  <======| std::string, ...
#include <string_view>
#include <vector>           //  <======| std::vector, ...
#include <stdexcept>        //  <======| ...
#include <limits.h>
#include <math.h>


//      0.3.        "DEAR IMGUI" HEADERS...
#include "json.hpp"
//
#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_internal.h"
#include "implot.h"
#include "implot_internal.h"





namespace cb { namespace ccounter { //     BEGINNING NAMESPACE "cb::ccounter"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//
//
//
//      1.      TYPES AND ABSTRACTIONS...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//      1A. TYPES |        ABSTRACTIONS FOR THE COINCIDENCE-COUNTER.
// *************************************************************************** //


//  "PythonCMD"
//
enum class PythonCMD : uint8_t {
      None = 0
    , IntegrationWindow
    , CoincidenceWindow
//
    , COUNT   // = 16
};


//  "DEF_PYTHON_CMD_FMT_STRINGS"
//
static constexpr cblib::EnumArray< PythonCMD, const char * >
DEF_PYTHON_CMD_FMT_STRINGS      = {
{
    /*  None                    */    "%s\n"
    /*  IntegrationWindow       */  , "integration_window %.3f\n"
    /*  CoincidenceWindow       */  , "Parsing Error"
} };






//  "PerFrame_t"
//
//      [[ TO-DO ]]:    REPLACE THIS!!!
//                      This is just a POD struct to carry the per-frame data we need throughout the CCounter class.
//
struct PerFrame_t {
    std::string     raw             {   };
    float           xmin            = -1.0f;
    float           xmax            = -1.0f;
    float           now             = -1.0f;
    float           spark_now       = -1.0f;
//
    bool            got_packet      = false;
    bool            crawling        = false;
//
//
//
    //  "clear"
    //
    inline void clear(void) noexcept
    {
        this->raw             .clear();
        this->xmin            = -1.0f;
        this->xmax            = -1.0f;
        this->now             = -1.0f;
        this->spark_now       = -1.0f;
        this->got_packet      = false;
        return;
    }
};






//  "ChannelID"
//
enum class ChannelID : uint8_t {
      UNUSED = 0
    , D         , C         , CD
    , B         , BD        , BC        , BCD
    , A         , AD        , AC        , ACD
    , AB        , ABD       , ABC       , ABCD
//
    , COUNT   // = 16
};


//  "CoincidencePacket"
//      Holds a single FPGA “packet”
//
struct CoincidencePacket
{
    using                                   Index               = ChannelID;
    using                                   value_type          = int;
//
//
//
    cblib::EnumArray<Index, value_type>     counts              = {   };
    int                                     cycles              {   };
    //
    //
    //                                      CONVENIENCE ACCESSORS:
    [[nodiscard]] value_type                    d                   (void) const noexcept           {   return this->counts[ Index::D       ];      }
    [[nodiscard]] value_type                    c                   (void) const noexcept           {   return this->counts[ Index::C       ];      }
    [[nodiscard]] value_type                    cd                  (void) const noexcept           {   return this->counts[ Index::CD      ];      }
    [[nodiscard]] value_type                    b                   (void) const noexcept           {   return this->counts[ Index::B       ];      }
    [[nodiscard]] value_type                    bd                  (void) const noexcept           {   return this->counts[ Index::BD      ];      }
    [[nodiscard]] value_type                    bc                  (void) const noexcept           {   return this->counts[ Index::BC      ];      }
    [[nodiscard]] value_type                    bcd                 (void) const noexcept           {   return this->counts[ Index::BCD     ];      }
    [[nodiscard]] value_type                    a                   (void) const noexcept           {   return this->counts[ Index::A       ];      }
    [[nodiscard]] value_type                    ad                  (void) const noexcept           {   return this->counts[ Index::AD      ];      }
    [[nodiscard]] value_type                    ac                  (void) const noexcept           {   return this->counts[ Index::AC      ];      }
    [[nodiscard]] value_type                    acd                 (void) const noexcept           {   return this->counts[ Index::ACD     ];      }
    [[nodiscard]] value_type                    ab                  (void) const noexcept           {   return this->counts[ Index::AB      ];      }
    [[nodiscard]] value_type                    abd                 (void) const noexcept           {   return this->counts[ Index::ABD     ];      }
    [[nodiscard]] value_type                    abc                 (void) const noexcept           {   return this->counts[ Index::ABC     ];      }
    [[nodiscard]] value_type                    abcd                (void) const noexcept           {   return this->counts[ Index::ABCD    ];      }
    //
    //                                      OVERLOADED ACCESSORS:
    [[nodiscard]] inline value_type &           operator []         (const Index id) noexcept       { return counts[id]; }
    [[nodiscard]] inline const value_type &     operator []         (const Index id) const noexcept { return counts[id]; }
};



//  "parse_packet"
//      Parse one JSON‑line; returns nullopt on format errors
//
inline std::optional<CoincidencePacket>
parse_packet(std::string_view line)
{
    using               json            = nlohmann::json;
    using               Packet          = CoincidencePacket;
    using               Index           = Packet::Index;
    //
    Packet              packet          {   };

    try
    {
        json            j       = json::parse(line);
        const auto &    arr     = j.at("counts");
        const size_t    N       = arr.size();
        size_t          i       = 0ULL;
        Index           idx     = static_cast<Index>(0);
        
        
        //      CASE 0 :    MORE COUNTER-VALUES RETURNED THAN WE EXPECTED  [ THIS SHOULD NEVER HAPPEN ]...
        if ( N != static_cast<size_t>(Index::COUNT) )       { return std::nullopt; }

        
        //      1.      FETCH THE VALUE OF EACH COUNTER FROM THE DATA-DELIVERY...
        for (i = 0ULL; i < N; idx = static_cast<Index>(++i) ) {
            packet.counts[idx]      = arr[i].get<int>();
        }
        packet.cycles   = j.at("cycles").get<int>();
    }
    //
    //      ERROR :     Some type of malformed JSON / JSON-Keys, etc...
    catch (const json::exception & )
    {
        return std::nullopt;
    }
    
    return packet;
}


//  "parse_packet"
//
inline std::optional<CoincidencePacket>
parse_packet(std::string_view line, bool mutual_exclusion)   // NEW ARG (default = previous behaviour)
{
    using               json            = nlohmann::json;
    using               Packet          = CoincidencePacket;
    using               Index           = Packet::Index;
    //
    Packet              packet          {   };


    try
    {
        json            j       = json::parse(line);
        const auto &    arr     = j.at("counts");
        const size_t    N       = arr.size();
        size_t          i       = 0ULL;
        Index           idx     = static_cast<Index>(0);
        
        
        //      CASE 0 :    MORE COUNTER-VALUES RETURNED THAN WE EXPECTED  [ THIS SHOULD NEVER HAPPEN ]...
        if ( N != static_cast<size_t>(Index::COUNT) )       { return std::nullopt; }

        
        //      1.      FETCH THE VALUE OF EACH COUNTER FROM THE DATA-DELIVERY...
        for (i = 0ULL; i < N; idx = static_cast<Index>(++i) ) {
            packet.counts[idx]      = arr[i].get<int>();
        }
        packet.cycles   = j.at("cycles").get<int>();


        //      2.      ADAPT GEORGES' FPGA VALUES FROM:  [ NON-MUTEX (Default) ] -- TO -- [ MUTEX ]...
        if ( !mutual_exclusion )
        {
            //      The index value encodes which APD channels participated:
            //          bit3=A,     bit2=B,     bit1=C,     bit0=D      (e.g. 0b1100 == AB)
            //
            for (i = 0ULL, idx = static_cast<Index>(i); i < N; idx = static_cast<Index>(++i) )
            {
                const int       val     = packet.counts[idx];
                const uint8_t   mask    = static_cast<uint8_t>(i);
                
                if (val == 0)           { continue; }

                //  skip single channels or UNUSED (they already hold the count)
                if ( mask == 0 || mask == 1 || mask == 2 || mask == 4 || mask == 8 )    { continue; }

                if (mask & 0x8)         { packet.counts[Index::A] += val; }     //  A.
                if (mask & 0x4)         { packet.counts[Index::B] += val; }     //  B.
                if (mask & 0x2)         { packet.counts[Index::C] += val; }     //  C.
                if (mask & 0x1)         { packet.counts[Index::D] += val; }     //  D.
            }
        }
    }
    //
    //      ERROR :     Some type of malformed JSON / JSON-Keys, etc...
    catch (const json::exception & )
    {
        return std::nullopt;
    }
    
    return packet;
}





    
// *************************************************************************** //
//      1A. TYPES |        PRAGMATIC ABSTRACTIONS.
// *************************************************************************** //

//  "VisSpec"
//      - Define the visibility of each COUNTER PLOT...
//
struct VisSpec {
    bool    master;         const char * master_ID;
    bool    single;         const char * single_ID;
    bool    average;        const char * average_ID;
};


//  "ChannelSpec"
//      POD Struct to define each COUNTER PLOT for the COINCIDENCE COUNTER...
//
struct ChannelSpec {
    const size_t            idx;
//
    const char *            name;
    mutable VisSpec         vis;
};






//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 1.  "TYPES & ABSTRACT." ]].












// *************************************************************************** //
//
//
//
//      2.      OTHER...
// *************************************************************************** //
// *************************************************************************** //




// *************************************************************************** //
//      2A. OTHER |         CCOUNTER---STYLE.
// *************************************************************************** //

//  "CCounterStyle"
//      PLAIN-OLD-DATA (POD) STRUCT.
//
struct CCounterStyle
{
    // *************************************************************************** //
    //      0. |    NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    CBAPP_APPSTATE_ALIAS_API            //  *OR*    CBAPP_CBLIB_TYPES_API       //  FOR CBLIB...
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      0. |    STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    static constexpr float              ms_TOOLBAR_ICON_SCALE           = 1.5f;
    
//
// *************************************************************************** //
// *************************************************************************** //   END "0.  CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//      1.          DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      1. |    IMPORTANT DATA-MEMBERS.
    // *************************************************************************** //
    
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      1. |    UI---DIMENSION VARIABLES.
    // *************************************************************************** //
    //
    //                                  MASTER PLOT:
    float                                   m_mst_plot_slider_height        = 20.0f;
    float                                   m_mst_plot_height               = 400.0f;
    mutable ImVec2                          m_mst_avail                     = { -1.0f   , -1.0f };
    
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      1. |    MASTER PLOT STUFF.
    // *************************************************************************** //
    //  ImPlotLineFlags                                         m_channel_flags                 = ImPlotLineFlags_None | ImPlotLineFlags_Shaded;
    //  ImPlotAxisFlags                                         m_plot_flags                    = ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoDecorations;
    //  ImPlotFlags                                             m_mst_PLOT_flags                = ImPlotFlags_None | ImPlotFlags_NoTitle;
    //  ImPlotAxisFlags                                         m_mst_plot_flags                = ImPlotAxisFlags_None | ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoSideSwitch;
    //  ImPlotAxisFlags                                         m_mst_xaxis_flags               = ImPlotAxisFlags_None | ImPlotAxisFlags_AutoFit;                           // enable grid, disable decorations.
    //  ImPlotAxisFlags                                         m_mst_yaxis_flags               = ImPlotAxisFlags_None | ImPlotAxisFlags_AutoFit;                           // enable grid, disable decorations.
    
    //  ImPlotLocation                                          m_mst_legend_loc                = ImPlotLocation_NorthWest;                                                 // legend position.
    //  ImPlotLegendFlags                                       m_mst_legend_flags              = ImPlotLegendFlags_None; //ImPlotLegendFlags_Outside; // | ImPlotLegendFlags_Horizontal;
    
    
    ImPlotFlags                             mst_plot_flags                  =  ImPlotFlags_NoFrame | ImPlotFlags_NoBoxSelect | ImPlotFlags_NoTitle; // ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoDecorations;  //  ImPlotFlags_Equal | ImPlotFlags_NoFrame | ImPlotFlags_NoBoxSelect | ImPlotFlags_NoMenus | ImPlotFlags_NoLegend | ImPlotFlags_NoTitle;
    utl::AxisCFG                            mst_axes [2]                    = {
          { "Time  [sec]"       , ImPlotAxisFlags_None | ImPlotAxisFlags_Opposite | ImPlotAxisFlags_NoHighlight     }
        , { "Counts  [Arb.]"    , ImPlotAxisFlags_None | ImPlotAxisFlags_AutoFit  | ImPlotAxisFlags_NoHighlight     }
    };
    utl::LegendCFG                          legend                          = { ImPlotLocation_NorthWest, ImPlotLegendFlags_Outside | ImPlotLegendFlags_Horizontal /* ImPlotLegendFlags_Outside; // | ImPlotLegendFlags_Horizontal; */ };
    
    
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      1. |    INDIVIDUAL PLOT STUFF.
    // *************************************************************************** //
    ImPlotAxisFlags                         m_ind_pline_flags               = ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoDecorations;
    
    
    
    
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      1. |    PLOT---STYLE VARIABLES.
    // *************************************************************************** //
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      1. |    TRANSIENT / INTERNAL STATE VARIABLES.
    // *************************************************************************** //
    
    
//
// *************************************************************************** //
// *************************************************************************** //   END "1.  DATA-MEMBERS".



// *************************************************************************** //
//
//      2.A.        MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
    // *************************************************************************** //
    //      INITIALIZATION METHODS.         |   "init.cpp" ...
    // *************************************************************************** //
    //  explicit                        CCounterStyle           (app::AppState & );             //  Def. Constructor.
                                        CCounterStyle           (void) noexcept                 = default;
                                        ~CCounterStyle          (void)                          = default;
    
    // *************************************************************************** //
    //      DELETED FUNCTIONS.              |   ...
    // *************************************************************************** //
                                        CCounterStyle           (const CCounterStyle &    src)       = delete;   //  Copy. Constructor.
                                        CCounterStyle           (CCounterStyle &&         src)       = delete;   //  Move Constructor.
    CCounterStyle &                     operator =              (const CCounterStyle &    src)       = delete;   //  Assgn. Operator.
    CCounterStyle &                     operator =              (CCounterStyle &&         src)       = delete;   //  Move-Assgn. Operator.
    
//
// *************************************************************************** //
// *************************************************************************** //   END "2A.  MEMBER FUNCS".

    
   
// *************************************************************************** //
//
//      2.B.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      2.B. |  QUERY FUNCTIONS.
    // *************************************************************************** //
    //  "_no_op"
    inline void                         _no_op                              (void)      { return; };
    
    
//
// *************************************************************************** //
// *************************************************************************** //   END "2B.  INLINE" FUNCTIONS.



//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "CCounterStyle" INLINE STRUCT DEFINITION.






//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 2.  "OTHER" ]].











// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb::ccounter" NAMESPACE.












#endif      //  _CBAPP_COUNTER_INTERNAL_H  //
// *************************************************************************** //
// *************************************************************************** //   END FILE.

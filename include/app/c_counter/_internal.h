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
};






//  "ChannelIdx"
//
enum ChannelIdx : uint8_t {
    UNUSED = 0,
    D,      C,      CD,
    B,      BD,     BC,     BCD,
    A,      AD,     AC,     ACD,
    AB,     ABD,    ABC,    ABCD,
    CHANNEL_COUNT   // = 16
};


//  "CoincidencePacket"
//      Holds a single FPGA “packet”
//
struct CoincidencePacket
{
    std::array<int, CHANNEL_COUNT>      counts          {   };
    int                                 cycles          {   };

    //  Convenience accessors
    int d           (void) const    { return counts[D];  }
    int c           (void) const    { return counts[C];  }
    int cd          (void) const    { return counts[CD]; }
    int b           (void) const    { return counts[B];  }
    int bd          (void) const    { return counts[BD]; }
    int bc          (void) const    { return counts[BC]; }
    int bcd         (void) const    { return counts[BCD];}
    int a           (void) const    { return counts[A];  }
    int ad          (void) const    { return counts[AD]; }
    int ac          (void) const    { return counts[AC]; }
    int acd         (void) const    { return counts[ACD];}
    int ab          (void) const    { return counts[AB]; }
    int abd         (void) const    { return counts[ABD];}
    int abc         (void) const    { return counts[ABC];}
    int abcd        (void) const    { return counts[ABCD];}
};


//  "parse_packet"
//      Parse one JSON‑line; returns nullopt on format errors
//
inline std::optional<CoincidencePacket>
parse_packet(std::string_view line)
{
    using               json        = nlohmann::json;
    CoincidencePacket   pkt         {   };

    try
    {
        json j = json::parse(line);
        const auto& arr = j.at("counts");
        if (arr.size() != CHANNEL_COUNT)         // sanity check
            return std::nullopt;

        for (std::size_t i = 0; i < CHANNEL_COUNT; ++i)
            pkt.counts[i] = arr[i].get<int>();

        pkt.cycles = j.at("cycles").get<int>();

    }
    catch (const json::exception & )
    {
        return std::nullopt;                     // malformed JSON / keys
    }
    
    return pkt;
}


//  "parse_packet"
//
inline std::optional<CoincidencePacket>
parse_packet(std::string_view line, bool mutual_exclusion)   // NEW ARG (default = previous behaviour)
{
    using               json        = nlohmann::json;
    CoincidencePacket   pkt         {   };

    constexpr size_t    A_IDX       = ChannelIdx::A;
    constexpr size_t    B_IDX       = ChannelIdx::B;
    constexpr size_t    C_IDX       = ChannelIdx::C;
    constexpr size_t    D_IDX       = ChannelIdx::D;


    try
    {
        json            j       = json::parse(line);
        const auto &    arr     = j.at("counts");
        
        if ( arr.size() != CHANNEL_COUNT )      { return std::nullopt; }

        for (size_t i = 0; i < CHANNEL_COUNT; ++i) {
            pkt.counts[i] = arr[i].get<int>();
        }

        pkt.cycles = j.at("cycles").get<int>();

        // ------------------------------------------------------------------
        //  Optional reconciliation of mutually exclusive counts
        // ------------------------------------------------------------------
        if (!mutual_exclusion)
        {
            // The index value encodes which APD channels participated:
            // bit3=A, bit2=B, bit1=C, bit0=D  (e.g. 0b1100 == AB)
            for (size_t idx = 0; idx < CHANNEL_COUNT; ++idx)
            {
                const int       n       = pkt.counts[idx];
                const uint8_t   mask    = static_cast<uint8_t>(idx);
                
                if (n == 0)     { continue; }

                //  skip single channels or UNUSED (they already hold the count)
                if ( mask == 0 || mask == 1 || mask == 2 || mask == 4 || mask == 8 )
                    { continue; }

                if (mask & 0x8)     { pkt.counts[A_IDX] += n; }     //  A.
                if (mask & 0x4)     { pkt.counts[B_IDX] += n; }     //  B.
                if (mask & 0x2)     { pkt.counts[C_IDX] += n; }     //  C.
                if (mask & 0x1)     { pkt.counts[D_IDX] += n; }     //  D.
            }
        }
    }
    catch (const json::exception & )
    {
        return std::nullopt;
    }
    
    return pkt;
}





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
    static constexpr float              ms_MY_CONSTEXPR_VALUE           = 240.0f;
    
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
    float                                   m_mst_plot_slider_height        = 20.0f;
    float                                   m_mst_plot_height               = 400.0f;
    
    
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
    
    
    ImPlotFlags                             plot_flags                      = ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoDecorations;  //  ImPlotFlags_Equal | ImPlotFlags_NoFrame | ImPlotFlags_NoBoxSelect | ImPlotFlags_NoMenus | ImPlotFlags_NoLegend | ImPlotFlags_NoTitle;
    utl::AxisCFG                            mst_axes [2]                    = {
        {"Time  [sec]"      ,   ImPlotAxisFlags_None | ImPlotAxisFlags_NoSideSwitch | ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_NoInitialFit | ImPlotAxisFlags_Opposite },
        {"Counts  [Arb.]"   ,   ImPlotAxisFlags_None | ImPlotAxisFlags_NoSideSwitch | ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_NoInitialFit  }
    };
    utl::LegendCFG                          legend                          = { ImPlotLocation_NorthWest, ImPlotLegendFlags_None };
    
    
    
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

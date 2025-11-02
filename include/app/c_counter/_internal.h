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
//      2A. DATA |      STATIC DATA-MEMBERS USED FOR CCOUNTER CLASS.
// *************************************************************************** //






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

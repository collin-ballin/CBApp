/***********************************************************************************
*
*       ********************************************************************
*       ****               _ T Y P E S . H  ____  F I L E               ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      July 4, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_COUNTER_APP_TYPES_H
#define _CBAPP_COUNTER_APP_TYPES_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
//#include "app/app.h"

//  0.2     STANDARD LIBRARY HEADERS...
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

//  0.3     "DEAR IMGUI" HEADERS...
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
//      1.      COINCIDENCE-COUNTER PRAGMATIC TYPE ABSTRACTIONS...
// *************************************************************************** //
// *************************************************************************** //

    
// *************************************************************************** //
//      1A. TYPES |        ENUM TYPES.
// *************************************************************************** //

//  "ChannelID"
//
enum class ChannelID : uint8_t {
      None = 0
    , D         , C         , CD
    , B         , BD        , BC        , BCD
    , A         , AD        , AC        , ACD
    , AB        , ABD       , ABC       , ABCD
//
    , COUNT   // = 16
};


//  "AvgMode"
//      Averaging mode selector.
//
enum class AvgMode : uint8_t {
      Samples = 0
    , Seconds
    , All
    , COUNT
};
//
//  "DEF_AVG_MODE_NAMES"
static constexpr cblib::EnumArray< AvgMode, const char * >
DEF_AVG_MODE_NAMES          = { {
    /*  Samples     */        "Samples"
    /*  Seconds     */      , "Seconds"
    /*  All         */      , "All"
} };
//
//  "DEF_AVG_MODE_FMT_STRINGS"
static constexpr cblib::EnumArray< AvgMode, const char * >
DEF_AVG_MODE_FMT_STRINGS    = { {
    /*  Samples     */        "%llu samples"
    /*  Seconds     */      , "%.2f seconds"
    /*  All         */      , "N/A"
} };





    
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
// *************************************************************************** //   END [[ 1.  "PRAGMATIC TYPES." ]].
























// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb::ccounter" NAMESPACE.






#endif      //  _CBAPP_COUNTER_APP_TYPES_H  //
// *************************************************************************** //
// *************************************************************************** //   END.


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



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //


//      1.      ENUM TYPES...
// *************************************************************************** //
// *************************************************************************** //

//  "AvgMode"
//      Averaging mode selector.
//
enum class AvgMode {
    Samples, Seconds, COUNTER
};



    


// *************************************************************************** //
//
//
//      2.      STRUCTS / TYPES...
// *************************************************************************** //
// *************************************************************************** //

//  "VisSpec"
//      - Define the visibility of each COUNTER PLOT...
struct VisSpec {
    bool    master;         const char * master_ID;
    bool    single;         const char * single_ID;
    bool    average;        const char * average_ID;
};


//  "ChannelSpec"
//      POD Struct to define each COUNTER PLOT for the COINCIDENCE COUNTER...
struct ChannelSpec {
    const size_t        idx;
    const char *        name;
    VisSpec             vis;
};






// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.






#endif      //  _CBAPP_COUNTER_APP_TYPES_H  //
// *************************************************************************** //
// *************************************************************************** //   END.


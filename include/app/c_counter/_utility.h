/***********************************************************************************
*
*       ********************************************************************
*       ****             _ U T I L I T Y . H  ____  F I L E             ****
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
#ifndef _CBAPP_COUNTER_APP_UTILITY_H
#define _CBAPP_COUNTER_APP_UTILITY_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
#include "app/c_counter/_types.h"
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



//      STATIC DATA MEMBERS...
// *************************************************************************** //
// *************************************************************************** //

//  "ccounter" NAMESPACE...
//
namespace ccounter {
    inline static constexpr size_t      ms_CMD_BUFFER_SIZE              = 512;
    //
    static bool                         enter                           = false;
    static float                        row_height_px                   = 60.0f;
//
//
    static float                        delay_s                         = 1.0f;                 // command slider
    static char                         line_buf[ms_CMD_BUFFER_SIZE]    {   };                  // manual send box
//
    static std::string                  s_last_error_title              {   };                  // FOR ERRORS...
    static std::string                  s_last_error_message            {   };                  // FOR ERRORS...
//
//
    static ImGuiInputTextFlags          write_file_flags            = ImGuiInputTextFlags_None | ImGuiInputTextFlags_ElideLeft | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue;
}












// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.






#endif      //  _CBAPP_COUNTER_APP_UTILITY_H  //
// *************************************************************************** //
// *************************************************************************** //   END.


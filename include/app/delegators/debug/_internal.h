/***********************************************************************************
*
*       ********************************************************************
*       ****            _ I N T E R N A L . H  ____  F I L E            ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      January 15, 2026.
*
*       ********************************************************************
*                FILE:      [./DetailView.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_APP_DEBUGGER_INTERNAL_H
#define _CBAPP_APP_DEBUGGER_INTERNAL_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  1.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
#include "utility/utility.h"
#include "widgets/widgets.h"
//  #include "app/_init.h"
#include "app/state/state.h"



//  1.2     STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <cstdlib>          // C-Headers...
#include <stdio.h>
#include <unistd.h>

#include <string>           //  <======| std::string, ...
#include <string_view>
#include <filesystem>
#include <vector>           //  <======| std::vector, ...
#include <stdexcept>        //  <======| ...
#include <limits.h>
#include <math.h>



//  1.3     "DEAR IMGUI" HEADERS...
#include "imgui.h"
#include "implot.h"
# include "imgui_internal.h"



namespace cb { namespace debug { // BEGINNING NAMESPACE "cb::debug"...
// *************************************************************** //
// *************************************************************** //



//  "MenuType"
//
enum class MenuType : uint8_t {
      None      = 0
    , CBDemo
    , UnitTesting
    , Generic
//
    , COUNT
};
//
//  "DEF_MENU_TYPE_NAMES"
static constexpr cblib::EnumArray< MenuType, const char * >
DEF_MENU_TYPE_NAMES      = { {
    /*  None            */    "None"
    /*  CBDemo          */  , "CBDemo"
    /*  UnitTesting     */  , "Unit Testing"
    /*  Generic         */  , "Generic"
} };












// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} } //   END OF "cb::debug" NAMESPACE.












#endif      //  _CBAPP_APP_DEBUGGER_INTERNAL_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.

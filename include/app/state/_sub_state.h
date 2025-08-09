/***********************************************************************************
*
*       ********************************************************************
*       ****           _ S U B _ S T A T E . h  ____  F I L E           ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      August 09, 2025.
*
*       ********************************************************************
*                FILE:      [include/app/state/_state.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_APP_SUB_STATE_H
#define _CBAPP_APP_SUB_STATE_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  1.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
#include "utility/utility.h"
#include "widgets/widgets.h"
//
#include "app/state/_init.h"
#include "app/state/_config.h"
#include "app/state/_types.h"



//  1.2     STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <cstdlib>          // C-Headers...
#include <stdio.h>
#include <unistd.h>
#include <chrono>

#include <string>           //  <======| std::string, ...
#include <string_view>
#include <string_view>
#include <vector>           //  <======| std::vector, ...
#include <stdexcept>        //  <======| ...
#include <functional>
#include <limits.h>
#include <math.h>
#include <atomic>



//  1.3     "DEAR IMGUI" HEADERS...
#include "imgui.h"
#include "implot.h"
#include "imgui_internal.h"






namespace cb { namespace app { //     BEGINNING NAMESPACE "cb" :: "app"...
// *************************************************************************** //
// *************************************************************************** //



//  "TaskState_t"
//
struct TaskState_t {
    _CBAPP_APPSTATE_ALIAS_API       //  CLASS-DEFINED, NESTED TYPENAME ALIASES.
// *************************************************************************** //
// *************************************************************************** //
//
//                                  MAIN STATE INFORMATION:
    Applet                              m_current_task              = Applet::None;
    ImGuiViewport *                     focused_viewport            = nullptr; // Which viewport (if any)
//
//
//                                  IMPORTANT:
    bool                                has_focus                   = false;   // Any ImGui platform window focused by OS?
    ImGuiWindow *                       nav_window                  = nullptr;
//
//
//                                  FUTURE (MUTABLE STATE VARIABLES):
    std::atomic<bool>                   m_running                   = { true };
    std::atomic<bool>                   m_closeable                 = { true };
//
//
//                                  PENDING ACTION STATE:
    std::atomic<CBSignalFlags>          m_pending                   = { CBSignalFlags_None };
    bool                                m_dialog_queued             = false;
    bool                                m_rebuild_dockspace         = false;
//
//
//                                  UTILITY:
    bool                                reliable                    = false;   // True if determined via Platform_GetWindowFocus
};












// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" :: "app" NAMESPACE.






#endif      //  _CBAPP_APP_SUB_STATE_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.

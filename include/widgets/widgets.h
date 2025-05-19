/***********************************************************************************
*
*       ********************************************************************
*       ****              W I D G E T S . H  ____  F I L E              ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond
*               DATED:      April 26, 2025.
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_WIDGETS_H
#define _CBAPP_WIDGETS_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include "_config.h"
#include "cblib.h"
#include "widgets/_heatmap.h"
#include "utility/utility.h"


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
#include "implot.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



namespace toolbar { //     BEGINNING NAMESPACE "toolbar"...
// *************************************************************************** //
// *************************************************************************** //
//
//
struct config {
    const char *      parent_uuid               = nullptr;                                // reference / parent window (optional)
    utl::Anchor       anchor                    = utl::Anchor::NorthEast;                 // anchor on parent window
    //
    //  SEPERATE HOST VARIABLES...
    const char *      host_uuid                 = nullptr;                                // reference / parent window (optional)
    utl::Anchor       host_anchor               = utl::Anchor::SouthEast;                 // anchor on parent window
    //
    //
    ImVec2            offset                    = {0.0f, 0.0f};                           // extra offset
    ImVec2            host_offset               = {0.0f, 0.0f};                           // extra offset
    float             bg_alpha                  = 0.70f;                                  // background alpha
    ImGuiWindowFlags  flags                     = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings;
    bool              vis_locked_to_parent      = true;                                   // hide when parent not visible
    bool              lock_x_to_parent          = true;                                   // if false, x follows viewport
    bool              lock_y_to_parent          = true;                                   // if false, y follows viewport
};

// Begin a floating toolbar window. Always returns the ImGui::Begin boolean.
// Caller must invoke toolbar::end() exactly once after begin().
inline bool begin(const char * id, const config & cfg = {})
{
    bool                open            = false;
    ImVec2              pos             = {-1.0f, -1.0f};
    ImGuiWindow *       parent_win      = cfg.parent_uuid   ? ImGui::FindWindowByName(cfg.parent_uuid)      : nullptr;
    ImGuiViewport *     parent_vp       = parent_win        ? parent_win->Viewport                          : ImGui::GetMainViewport();
    ImGuiWindow *       host_win        = cfg.parent_uuid   ? ImGui::FindWindowByName(cfg.host_uuid)        : nullptr;
    ImGuiViewport *     host_vp         = host_win          ? host_win->Viewport                            : ImGui::GetMainViewport();

    //  1.      DETERMINE VISIBILITY OF PARENT (The window that can be closed)...
    const bool          parent_visible  = parent_win && parent_win->DockIsActive && !parent_win->Hidden;
    const bool          case_1          = cfg.vis_locked_to_parent && !parent_visible;
    const bool          case_2          = host_win && host_win->DockIsActive && !host_win->Hidden;
    
    
    
    //  2A.     CASE 1 :    PARENT WINDOW IS COLLAPSED...
    if (case_1 || case_2) {
        if (case_1) {
            pos     = host_win ? utl::GetImGuiWindowCoords(cfg.host_uuid, cfg.host_anchor) : host_vp->WorkPos;      // DEFAULT POSITION: Use Anchor on parent or viewport
            if (!cfg.lock_x_to_parent)  pos.x = host_vp->WorkPos.x;     //  Override axes when unlocked...
            if (!cfg.lock_y_to_parent)  pos.y = host_vp->WorkPos.y;
            pos.x                            += cfg.host_offset.x;      //  APPLY WINDOW OFFSETS...
            pos.y                            += cfg.host_offset.y;
        }
        //
        //  2B.     CASE 2 :    PARENT WINDOW IS VISIBLE...
        else {
            pos     = parent_win ? utl::GetImGuiWindowCoords(cfg.parent_uuid, cfg.anchor) : parent_vp->WorkPos;     // Base position = anchor on parent or viewport
            if (!cfg.lock_x_to_parent)  pos.x = parent_vp->WorkPos.x;   //  Override axes when unlocked...
            if (!cfg.lock_y_to_parent)  pos.y = parent_vp->WorkPos.y;
            pos.x                            += cfg.offset.x;           //  APPLY WINDOW OFFSETS...
            pos.y                            += cfg.offset.y;
        }
    }
    //  2C.         CASE 3 :    NEITHER PARENT NOR HOST WINDOW IS VISIBLE...
    else {
        return false;
    }


    ImGui::SetNextWindowViewport( (case_1) ? parent_vp->ID : host_vp->ID );                    //  SETUP STYLE FOR NEXT WINDOW...
    ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(cfg.bg_alpha);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    open                                = ImGui::Begin(id, nullptr, cfg.flags);
    ImGui::PopStyleVar();

    return open;
}

// End the floating toolbar window.
inline void end()
{
    ImGui::End();
}



// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "toolbar" NAMESPACE.






// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.






#endif      //  _CBAPP_WIDGETS_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.


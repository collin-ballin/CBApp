/***********************************************************************************
*
*       ********************************************************************
*       ****           _ C O N S T A N T S . H  ____  F I L E           ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      June 30, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBWIDGETS_EDITOR_CONSTANTS_H
#define _CBWIDGETS_EDITOR_CONSTANTS_H  1


//      //  From the 2nd frame onward, test drag distance
//      //  if ( m_pen.pending_time >= PEN_DRAG_TIME_THRESHOLD && ImGui::IsMouseDragPastThreshold(ImGuiMouseButton_Left, PEN_DRAG_MOVEMENT_THRESHOLD) )
//      if ( m_pen.pending_time >= PEN_DRAG_TIME_THRESHOLD && ImGui::IsMouseDragPastThreshold(ImGuiMouseButton_Left, PEN_DRAG_MOVEMENT_THRESHOLD) )
//      {
//          _pen_begin_handle_drag( m_pen.pending_vid, /*out_handle=*/true, /*force_select=*/true );
//          if ( Vertex * v = find_vertex_mut(m_vertices, m_pen.pending_vid) )
//              v->in_handle = ImVec2(0,0);              // make handle visible
//
// Updated declarations for selection overlay functions (no origin argument)
// void _draw_selection_highlight(ImDrawList* dl) const;
// void _draw_selection_bbox(ImDrawList* dl) const;
// void _draw_selected_handles(ImDrawList* dl) const;

//
//          m_pen.pending_handle = false;                // hand-off to drag logic
//          m_pen.pending_time   = 0.0f;
//          _pen_update_handle_drag(it);                 // first update
//          return;
//      }
        


//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
#include "utility/utility.h"

//  0.2     STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <cstdlib>          // C-Headers...
#include <cstdint>
#include <stdio.h>
#include <unistd.h>
#include <random>
#include <tuple>

#include <array>
#include <unordered_set>

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







// *************************************************************************** //
//
//
//
//  2.  NEW CONSTANTS...
// *************************************************************************** //
// *************************************************************************** //
inline static constexpr uint32_t        Z_EDITOR_BACK               = 1;                        // grid / background
inline static constexpr uint32_t        Z_FLOOR_USER                = 255;                      // first user layer
inline static constexpr uint32_t        Z_EDITOR_FRONT              = UINT32_MAX - 2;           // overlays, guides
inline static constexpr uint32_t        Z_CEIL_USER                 = Z_EDITOR_FRONT - 1;       // max allowed for user items
inline static constexpr uint32_t        RENORM_THRESHOLD            = 10'000;                   // span triggering re-pack
//
//
//
//constexpr const char* ICON_EYE        = u8"\uf06e";
//constexpr const char* ICON_EYE_SLASH  = u8"\uf070";
//constexpr const char* ICON_LOCK       = u8"\uf023";

//inline static constexpr ImWchar         FA_ICON_EYE                 = ICON_FA_EYE;        // \uf06e
//inline static constexpr ImWchar         FA_ICON_EYE_SLASH           = ICON_FA_EYE_SLASH;  // \uf070
//inline static constexpr ImWchar         FA_ICON_LOCK                = ICON_FA_LOCK;       // \uf023;








// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.






#endif      //  _CBWIDGETS_EDITOR_CONSTANTS_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.

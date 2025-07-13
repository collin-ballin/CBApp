/***********************************************************************************
*
*       ********************************************************************
*       ****             _ H I S T O R Y . H  ____  F I L E             ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      July 13, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBWIDGETS_EDITOR_HISTORY_H
#define _CBWIDGETS_EDITOR_HISTORY_H  1


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
#include "widgets/editor/_constants.h"

//  0.2     STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <cstdlib>          // C-Headers...
#include <stdio.h>
#include <unistd.h>
#include <random>
#include <tuple>
#include <utility>

#include <algorithm>
#include <array>
#include <unordered_set>
#include <optional>

#include <string>           //  <======| std::string, ...
#include <string_view>
#include <vector>           //  <======| std::vector, ...
#include <stdexcept>        //  <======| ...
#include <limits.h>
#include <math.h>

//  0.3     "DEAR IMGUI" HEADERS...
#include "json.hpp"
//
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
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.












#endif      //  _CBWIDGETS_EDITOR_HISTORY_H  //
// *************************************************************************** //
// *************************************************************************** //   END FILE.

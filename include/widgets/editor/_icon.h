/***********************************************************************************
*
*       ********************************************************************
*       ****                _ I C O N . H  ____  F I L E                ****
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
#ifndef _CBWIDGETS_EDITOR_ICON_H
#define _CBWIDGETS_EDITOR_ICON_H  1


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





//  1.  STATIC / CONSTEXPR / CONSTANTS...
// *************************************************************************** //
// *************************************************************************** //

enum class IconType { Eye, EyeOff, Lock, Unlock };






// *************************************************************************** //
//
//
//
//  1.  STATIC / CONSTEXPR / CONSTANTS...
// *************************************************************************** //
// *************************************************************************** //

//───────────────────────────────────────────────────────────────────────────
//  Simple 18×18 icons drawn with ImDrawList primitives
//      ─ eye   (visible)         ─ eye_off   (hidden)
//      ─ lock  (locked)          ─ unlock    (unlocked)
//      Each icon draws inside rc_min..rc_min+size, always centred.
//      Background can be supplied via draw_icon_background before calling icon fn.
//───────────────────────────────────────────────────────────────────────────

//  "draw_icon_background"
//
static inline void draw_icon_background(ImDrawList* dl,
                                        ImVec2 rc_min, ImVec2 size,
                                        ImU32 bg_col, float rounding = 3.0f)
{
    if (bg_col & 0xFF000000)
        dl->AddRectFilled(rc_min,
                          { rc_min.x + size.x, rc_min.y + size.y },
                          bg_col, rounding);
}




//  "draw_eye_icon"    [OPEN]
//
static inline void draw_eye_icon(ImDrawList* dl,
                                 ImVec2 rc_min, ImVec2 size,
                                 ImU32 fg_col, float thick = 1.5f)
{
    ImVec2 c{ rc_min.x + size.x * 0.5f, rc_min.y + size.y * 0.5f };
    float  r = size.x * 0.36f;
    dl->AddCircle(c, r, fg_col, 16, thick);
    dl->AddCircleFilled(c, r * 0.4f, fg_col);
}


//  "draw_eye_icon"    [CLOSED]
//
static inline void draw_eye_off_icon(ImDrawList* dl,
                                     ImVec2 rc_min, ImVec2 size,
                                     ImU32 fg_col, float thick = 1.5f)
{
    ImVec2 c{ rc_min.x + size.x * 0.5f, rc_min.y + size.y * 0.5f };
    float  r = size.x * 0.36f;
    dl->AddCircle(c, r, fg_col, 16, thick);
    dl->AddLine({ rc_min.x + 3, rc_min.y + size.y - 3 },
                { rc_min.x + size.x - 3, rc_min.y + 3 },
                fg_col, thick);
}


//  "draw_lock_icon"    [LOCKED]
//
static inline void draw_lock_icon(ImDrawList* dl,
                                  ImVec2 rc_min, ImVec2 size,
                                  ImU32 fg_col, float thick = 1.5f)
{
    ImVec2 c{ rc_min.x + size.x * 0.5f, rc_min.y + size.y * 0.5f };
    float  w = size.x * 0.55f;
    float  h = size.y * 0.45f;
    ImVec2 body_tl{ c.x - w * 0.5f, c.y + size.y * 0.05f };
    ImVec2 body_br{ body_tl.x + w,  body_tl.y + h };
    dl->AddRectFilled(body_tl, body_br, fg_col, 1.0f);

    float r = size.x * 0.22f;
    dl->PathArcTo({ c.x, body_tl.y }, r, IM_PI, IM_PI * 2);
    dl->PathStroke(fg_col, 0, thick);
}


//  "draw_lock_icon"    [UNLOCKED]
//
static inline void draw_unlock_icon(ImDrawList* dl,
                                    ImVec2 rc_min, ImVec2 size,
                                    ImU32 fg_col, float thick = 1.5f)
{
    ImVec2 c{ rc_min.x + size.x * 0.5f, rc_min.y + size.y * 0.5f };
    float  w = size.x * 0.55f;
    float  h = size.y * 0.45f;
    ImVec2 body_tl{ c.x - w * 0.5f, c.y + size.y * 0.05f };
    ImVec2 body_br{ body_tl.x + w,  body_tl.y + h };
    dl->AddRectFilled(body_tl, body_br, fg_col, 1.0f);

    float r = size.x * 0.22f;
    dl->PathArcTo({ c.x - r * 0.3f, body_tl.y }, r,
                  IM_PI * 0.7f, IM_PI * 1.7f);
    dl->PathStroke(fg_col, 0, thick);
}












// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.






#endif      //  _CBWIDGETS_EDITOR_ICON_H  //
// *************************************************************************** //
// *************************************************************************** //   END.

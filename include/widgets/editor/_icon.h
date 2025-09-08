/***********************************************************************************
*
*       ********************************************************************
*       ****                _ I C O N . H  ____  F I L E                ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
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








namespace icon {  //     BEGINNING NAMESPACE "icon"...
// *************************************************************************** //
// *************************************************************************** //

/*
//
// ── shared ───────────────────────────────────────────────────────────────
inline static float  CELL_SZ             = 18.0f;
inline static float  BG_ROUNDING         = 6.0f;
inline static float  STROKE_THICK        = 6.0f;

// ── eye ──────────────────────────────────────────────────────────────────
inline static int    EYE_SEG             = 20;              // ellipse resolution (fixed)
inline static float  EYE_OUTER_FRAC      = 0.45f;           // outer-rx / cell
inline static float  EYE_ASPECT          = 0.63f;           // outer-ry / outer-rx
inline static float  EYE_PUPIL_FRAC      = 0.600f;           // pupil-rx / outer-rx
inline static float  EYE_PUPIL_ASPECT    = 0.750f;           // pupil-ry / pupil-rx
inline static ImU32  EYE_DILATE_COLOR    = IM_COL32(  0,  0,  0, 255); // dark spot
inline static float  SLASH_PAD           = 25.0f;            // eye-off slash inset (px)
inline static float  SLASH_THICK         = 2.0f;            // × STROKE_THICK

// ── lock ─────────────────────────────────────────────────────────────────
inline static float  LOCK_BODY_W_FRAC    = 0.70f;
inline static float  LOCK_BODY_H_FRAC    = 0.55f;
inline static float  LOCK_BODY_ROUND     = 6.0f;
inline static float  LOCK_RADIUS_FRAC    = 0.22f;
inline static float  SHACKLE_GAP_FRAC    = 0.300f;
inline static float  LOCK_SHACKLE_TILT   = 0.000f;
inline static float  LOCK_LATCH_THICK    = 20.00f;            // px

// ── preview window ───────────────────────────────────────────────────────
inline static ImVec2 PREVIEW_CELL_SZ     = { 256.0f, 256.0f };
inline static float  PREVIEW_PAD         = 16.0f;
inline static ImU32  PREVIEW_BG_COL      = IM_COL32( 50, 50, 50, 255);
inline static ImU32  PREVIEW_FG_COL      = IM_COL32_WHITE;
//
*/

inline constexpr float      DESIGN_PX               = 256.0f;
inline static ImU32         EYE_DILATE_COLOR        = IM_COL32(  0,  0,  0, 255); // dark spot
inline static ImVec2        PREVIEW_CELL_SZ         = { 256.0f, 256.0f };
inline static float         CELL_SZ                 = 18.0f;
inline static int           EYE_SEG                 = 20;              // ellipse resolution (fixed)
//
inline static float         PREVIEW_PAD             = 16.0f;
inline static ImU32         PREVIEW_BG_COL          = IM_COL32( 50, 50, 50, 255);
inline static ImU32         PREVIEW_FG_COL          = IM_COL32_WHITE;
//
//
//
//––––– tuned values –––––//
inline static float         BG_ROUNDING             = 0.421875f;
inline static float         STROKE_THICK            = 0.421875f;

// eye
inline static float         EYE_OUTER_FRAC          = 0.45f;
inline static float         SLASH_PAD               = 1.757812f;
inline static float         EYE_ASPECT              = 0.63f;
inline static float         SLASH_THICK             = 0.84375f;
inline static float         EYE_PUPIL_FRAC          = 0.60f;
inline static float         EYE_PUPIL_ASPECT        = 0.75f;
inline static float         EYE_DILATE_FRAC         = 0.30f;

// lock
inline static float         LOCK_BODY_W_FRAC        = 0.70f;
inline static float         LOCK_BODY_ROUND         = 0.421875f;
inline static float         LOCK_BODY_H_FRAC        = 0.55f;
inline static float         LOCK_LATCH_THICK        = 1.40625f;
inline static float         LOCK_RADIUS_FRAC        = 0.22f;
inline static float         SHACKLE_GAP_FRAC        = 0.30f;
inline static float         LOCK_SHACKLE_TILT       = 0.0f;



// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "icon" NAMESPACE.






//───────────────────────────────────────────────────────────────────────────
//  Simple 18×18 icons drawn with ImDrawList primitives
//      ─ eye   (visible)         ─ eye_off   (hidden)
//      ─ lock  (locked)          ─ unlock    (unlocked)
//      Each icon draws inside rc_min..rc_min+size, always centred.
//      Background can be supplied via draw_icon_background before calling icon fn.
//───────────────────────────────────────────────────────────────────────────

//  "draw_icon_background"
//
//      static inline void draw_icon_background(ImDrawList* dl,
//                                              ImVec2 rc_min, ImVec2 size,
//                                              ImU32 bg_col,
//                                              float rounding = icon::BG_ROUNDING)
//      {
//          if ((bg_col & 0xFF000000) == 0) return;
//          dl->AddRectFilled(rc_min,
//                            ImVec2{ rc_min.x + size.x, rc_min.y + size.y },
//                            bg_col, rounding);
//      }


//  "draw_icon_background"
//
static inline void draw_icon_background( ImDrawList *   dl,
                                         ImVec2         tl,
                                         ImVec2         size,
                                         ImU32          fg_col,
                                         float          rounding = icon::BG_ROUNDING )
{
    if ((fg_col & 0xFF000000) == 0) return;
    dl->AddRectFilled(tl,
                      ImVec2{ tl.x + size.x, tl.y + size.y },
                      fg_col, rounding);
}



//  "draw_eye_icon"    [OPEN]
//
static inline void draw_eye_icon(ImDrawList* dl,
                                 ImVec2 tl, ImVec2 sz, ImU32 fg_col)
{
    using namespace icon;
    const ImVec2 c{ tl.x + sz.x * 0.5f, tl.y + sz.y * 0.5f };

    // outer ellipse
    const float rx  = sz.x * EYE_OUTER_FRAC;
    const float ry  = rx * EYE_ASPECT;
    dl->PathClear();
    for (int i = 0; i < EYE_SEG; ++i) {
        const float a = 2.0f * IM_PI * i / static_cast<float>(EYE_SEG);
        dl->PathLineTo({ c.x + rx * cosf(a), c.y + ry * sinf(a) });
    }
    dl->PathStroke(fg_col, ImDrawFlags_Closed, STROKE_THICK);

    // pupil ellipse (filled)
    const float prx = rx * EYE_PUPIL_FRAC;
    const float pry = prx * EYE_PUPIL_ASPECT;
    dl->PathClear();
    for (int i = 0; i < EYE_SEG; ++i) {
        const float a = 2.0f * IM_PI * i / static_cast<float>(EYE_SEG);
        dl->PathLineTo({ c.x + prx * cosf(a), c.y + pry * sinf(a) });
    }
    dl->PathFillConvex(fg_col);

    // dilation spot
    dl->AddCircleFilled(c, prx * EYE_DILATE_FRAC, EYE_DILATE_COLOR);
}


//  "draw_eye_off_icon"    [CLOSED]
//
static inline void draw_eye_off_icon(ImDrawList* dl,
                                     ImVec2 tl, ImVec2 sz, ImU32 fg_col)
{
    using namespace icon;

    // same ellipse shell as open eye
    const ImVec2  c { tl.x + sz.x * 0.5f, tl.y + sz.y * 0.5f };
    const float   rx = sz.x * EYE_OUTER_FRAC;
    const float   ry = rx * EYE_ASPECT;

    dl->PathClear();
    for (int i = 0; i < EYE_SEG; ++i) {
        const float a = 2.0f * IM_PI * (float)i / (float)EYE_SEG;
        dl->PathLineTo({ c.x + rx * cosf(a), c.y + ry * sinf(a) });
    }
    dl->PathStroke(fg_col, ImDrawFlags_Closed, STROKE_THICK);

    // slash
    dl->AddLine(ImVec2{ tl.x + SLASH_PAD,          tl.y + sz.y - SLASH_PAD },
                ImVec2{ tl.x + sz.x - SLASH_PAD,   tl.y + SLASH_PAD },
                fg_col, STROKE_THICK * SLASH_THICK);
}


//  "draw_lock_icon"    [LOCKED]
//
static inline void draw_lock_icon(ImDrawList* dl,
                                  ImVec2 tl, ImVec2 sz, ImU32 fg_col)
{
    using namespace icon;
    const ImVec2 c{ tl.x + sz.x * 0.5f, tl.y + sz.y * 0.5f };

    const float bw   = sz.x * LOCK_BODY_W_FRAC;
    const float bh   = sz.y * LOCK_BODY_H_FRAC;
    const float r    = sz.x * LOCK_RADIUS_FRAC;
    const float gap  = r * SHACKLE_GAP_FRAC;
    const float top  = c.y - (r + gap + bh) * 0.5f;

    const float shackle_y = top + r;
    const ImVec2 body_tl{ c.x - bw * 0.5f, shackle_y + gap };
    const ImVec2 body_br{ body_tl.x + bw,  body_tl.y + bh };

    dl->AddRectFilled(body_tl, body_br, fg_col, LOCK_BODY_ROUND);
    dl->AddRect      (body_tl, body_br, fg_col, LOCK_BODY_ROUND, 0, STROKE_THICK);

    dl->PathArcTo(ImVec2{ c.x, shackle_y }, r, IM_PI, IM_PI * 2.0f);
    dl->PathStroke(fg_col, 0, LOCK_LATCH_THICK);
}


//  "draw_unlock_icon"    [UNLOCKED]
//
static inline void draw_unlock_icon(ImDrawList* dl,
                                    ImVec2 tl, ImVec2 sz, ImU32 fg_col)
{
    using namespace icon;
    const ImVec2 c{ tl.x + sz.x * 0.5f, tl.y + sz.y * 0.5f };

    const float bw   = sz.x * LOCK_BODY_W_FRAC;
    const float bh   = sz.y * LOCK_BODY_H_FRAC;
    const float r    = sz.x * LOCK_RADIUS_FRAC;
    const float gap  = r * SHACKLE_GAP_FRAC;
    const float top  = c.y - (r + gap + bh) * 0.5f;

    const float shackle_y = top + r;
    const ImVec2 body_tl{ c.x - bw * 0.5f, shackle_y + gap };
    const ImVec2 body_br{ body_tl.x + bw,  body_tl.y + bh };

    dl->AddRectFilled(body_tl, body_br, fg_col, LOCK_BODY_ROUND);
    dl->AddRect      (body_tl, body_br, fg_col, LOCK_BODY_ROUND, 0, STROKE_THICK);

    const float x_off = r * LOCK_SHACKLE_TILT;
    dl->PathArcTo(ImVec2{ c.x - x_off, shackle_y }, r,
                  IM_PI * 0.70f, IM_PI * 1.70f);
    dl->PathStroke(fg_col, 0, LOCK_LATCH_THICK);
}



// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //

static inline void show_icon_preview_window(void)
{
    using namespace icon;
    static bool     open            = true;
    ImVec2          WINDOW_SIZE     = ImVec2(750, 1000);
    
    
    ImGui::SetNextWindowSize( WINDOW_SIZE, ImGuiCond_Once);
    if ( !ImGui::Begin("Icon Preview", &open) ) return;

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 org     = ImGui::GetCursorScreenPos();

    // ── 2×2 grid of icons ────────────────────────────────────────────────
    auto cell = [&](IconType t, ImVec2 p)
    {
        draw_icon_background(dl, p, PREVIEW_CELL_SZ, PREVIEW_BG_COL, BG_ROUNDING);
        switch (t) {
        case IconType::Eye:     draw_eye_icon    (dl, p, PREVIEW_CELL_SZ, PREVIEW_FG_COL); break;
        case IconType::EyeOff:  draw_eye_off_icon(dl, p, PREVIEW_CELL_SZ, PREVIEW_FG_COL); break;
        case IconType::Lock:    draw_lock_icon   (dl, p, PREVIEW_CELL_SZ, PREVIEW_FG_COL); break;
        case IconType::Unlock:  draw_unlock_icon (dl, p, PREVIEW_CELL_SZ, PREVIEW_FG_COL); break;
        }
    };
    cell(IconType::Eye,    org);
    cell(IconType::EyeOff, { org.x + PREVIEW_CELL_SZ.x + PREVIEW_PAD, org.y });
    cell(IconType::Lock,   { org.x,                                  org.y + PREVIEW_CELL_SZ.y + PREVIEW_PAD });
    cell(IconType::Unlock, { org.x + PREVIEW_CELL_SZ.x + PREVIEW_PAD, org.y + PREVIEW_CELL_SZ.y + PREVIEW_PAD });
    ImGui::Dummy({ 2 * PREVIEW_CELL_SZ.x + PREVIEW_PAD,
                   2 * PREVIEW_CELL_SZ.y + PREVIEW_PAD });

    // ── tweak panels ────────────────────────────────────────────────────
    if (ImGui::CollapsingHeader("Global", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderFloat("Stroke",      &STROKE_THICK,   0.5f, 30.0f, "%.1f");
        ImGui::SliderFloat("BG rounding", &BG_ROUNDING,    0.0f, 30.0f, "%.1f");
    }
    if (ImGui::CollapsingHeader("Eye", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderFloat("Outer frac",   &EYE_OUTER_FRAC,   0.20f, 0.60f, "%.2f");
        ImGui::SliderFloat("Aspect",       &EYE_ASPECT,       0.30f, 1.00f, "%.2f");
        ImGui::SliderFloat("Pupil frac",   &EYE_PUPIL_FRAC,   0.20f, 0.80f, "%.2f");
        ImGui::SliderFloat("Pupil aspect", &EYE_PUPIL_ASPECT, 0.30f, 1.00f, "%.2f");
        ImGui::SliderFloat("Dilate frac",  &EYE_DILATE_FRAC,  0.05f, 0.50f, "%.2f");
        ImGui::SliderFloat("Slash pad",    &SLASH_PAD,        0.0f,  30.0f, "%.1f");
        ImGui::SliderFloat("Slash thick",  &SLASH_THICK,      0.5f,  20.0f, "%.1f");
    }
    if (ImGui::CollapsingHeader("Lock", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderFloat("Body W",       &LOCK_BODY_W_FRAC, 0.30f, 0.80f, "%.2f");
        ImGui::SliderFloat("Body H",       &LOCK_BODY_H_FRAC, 0.20f, 0.70f, "%.2f");
        ImGui::SliderFloat("Body round",   &LOCK_BODY_ROUND,  0.0f,  20.0f, "%.1f");
        ImGui::SliderFloat("Radius",       &LOCK_RADIUS_FRAC, 0.10f, 0.40f, "%.3f");
        ImGui::SliderFloat("Gap frac",     &SHACKLE_GAP_FRAC, 0.0f,  0.60f, "%.3f");
        ImGui::SliderFloat("Shackle tilt", &LOCK_SHACKLE_TILT,0.0f,  0.60f, "%.3f");
        ImGui::SliderFloat("Latch thick",  &LOCK_LATCH_THICK, 0.5f,  20.0f, "%.2f");
    }




    // ── clipboard helper ────────────────────────────────────────────────
    if (ImGui::Button("Copy constants to clipboard")) {
        std::string o;
        auto add = [&](const char* n, float v){ o += n; o += " = "; o += std::to_string(v); o.push_back('\n'); };

        add("CELL_SZ",            CELL_SZ);      add("BG_ROUNDING",      BG_ROUNDING);      add("STROKE_THICK",   STROKE_THICK);

        add("EYE_OUTER_FRAC",     EYE_OUTER_FRAC);   add("EYE_ASPECT",       EYE_ASPECT);
        add("EYE_PUPIL_FRAC",     EYE_PUPIL_FRAC);   add("EYE_PUPIL_ASPECT", EYE_PUPIL_ASPECT);
        add("EYE_DILATE_FRAC",    EYE_DILATE_FRAC);  add("SLASH_PAD",        SLASH_PAD);
        add("SLASH_THICK",        SLASH_THICK);

        add("LOCK_BODY_W_FRAC",   LOCK_BODY_W_FRAC); add("LOCK_BODY_H_FRAC", LOCK_BODY_H_FRAC);
        add("LOCK_BODY_ROUND",    LOCK_BODY_ROUND);  add("LOCK_RADIUS_FRAC", LOCK_RADIUS_FRAC);
        add("SHACKLE_GAP_FRAC",   SHACKLE_GAP_FRAC); add("LOCK_SHACKLE_TILT",LOCK_SHACKLE_TILT);
        add("LOCK_LATCH_THICK",   LOCK_LATCH_THICK);

        ImGui::SetClipboardText(o.c_str());
    }


    if (ImGui::Button("Copy converted constants")) {
        const float s = CELL_SZ / PREVIEW_CELL_SZ.x;        // e.g. 18 / 256
        std::string o;

        auto add = [&](const char* n, float v){             // unchanged (fractions)
            o += n; o += " = "; o += std::to_string(v); o.push_back('\n');
        };
        auto add_px = [&](const char* n, float v){           // scaled (px)
            o += n; o += " = "; o += std::to_string(v * s); o.push_back('\n');
        };

        // global px               relative
        add_px ("BG_ROUNDING",      BG_ROUNDING);      add("CELL_SZ",            CELL_SZ);
        add_px ("STROKE_THICK",     STROKE_THICK);

        // eye (fractions stay)  px slash pad & thick scale
        add   ("EYE_OUTER_FRAC",    EYE_OUTER_FRAC);   add_px("SLASH_PAD",       SLASH_PAD);
        add   ("EYE_ASPECT",        EYE_ASPECT);       add_px("SLASH_THICK",     SLASH_THICK * STROKE_THICK); // effective px
        add   ("EYE_PUPIL_FRAC",    EYE_PUPIL_FRAC);
        add   ("EYE_PUPIL_ASPECT",  EYE_PUPIL_ASPECT);
        add   ("EYE_DILATE_FRAC",   EYE_DILATE_FRAC);

        // lock – fractions unchanged, pixel knobs scaled
        add   ("LOCK_BODY_W_FRAC",  LOCK_BODY_W_FRAC); add_px("LOCK_BODY_ROUND", LOCK_BODY_ROUND);
        add   ("LOCK_BODY_H_FRAC",  LOCK_BODY_H_FRAC); add_px("LOCK_LATCH_THICK",LOCK_LATCH_THICK);
        add   ("LOCK_RADIUS_FRAC",  LOCK_RADIUS_FRAC);
        add   ("SHACKLE_GAP_FRAC",  SHACKLE_GAP_FRAC);
        add   ("LOCK_SHACKLE_TILT", LOCK_SHACKLE_TILT);

        ImGui::SetClipboardText(o.c_str());
    }


    ImGui::End();
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

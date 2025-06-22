/***********************************************************************************
*
*       *********************************************************************
*       ****             C A M E R A . C P P  ____  F I L E              ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      June 21, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "widgets/editor/editor.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//      UTILITIES / HELPERS...
// *************************************************************************** //
// *************************************************************************** //







// *************************************************************************** //
//
//
//
//      UTILITIES / HELPERS...
// *************************************************************************** //
// *************************************************************************** //

//  "_apply_wheel_zoom"
//
void Editor::_apply_wheel_zoom(const Interaction& it)
{
    ImGuiIO &       io              = ImGui::GetIO();
    ImVec2          view_sz         = m_avail;
    ImVec2          scr_anchor;
    float           ppw_old         = m_cam.pixels_per_world(view_sz, m_world_bounds);


    if (it.hovered)     { scr_anchor = ImVec2(io.MousePos.x - m_p0.x, io.MousePos.y - m_p0.y); }
    else                { scr_anchor = ImVec2(view_sz.x * 0.5f, view_sz.y * 0.5f); }

    // world-space anchor before zoom
    ImVec2 world_anchor{
        m_cam.pan.x + scr_anchor.x / ppw_old,
        m_cam.pan.y + scr_anchor.y / ppw_old
    };

    // magnification grows 10 % per wheel notch
    m_cam.zoom_mag *= (1.f + io.MouseWheel * 0.10f);
    m_cam.zoom_mag  = std::clamp(m_cam.zoom_mag, CAM_ZOOM_MIN,
                                 CAM_ZOOM_MAX);

    float ppw_new = m_cam.pixels_per_world(view_sz, m_world_bounds);

    // keep anchor fixed in screen space
    m_cam.pan.x = world_anchor.x - scr_anchor.x / ppw_new;
    m_cam.pan.y = world_anchor.y - scr_anchor.y / ppw_new;

    _clamp_pan(view_sz);
}


//  "_apply_pan"
//
void Editor::_apply_pan(const Interaction & it)
{
    ImGuiIO& io = ImGui::GetIO();
    float ppw   = m_ppw;                         // cached pixels-per-world

    // convert pixel delta to world-space delta
    m_cam.pan.x -= io.MouseDelta.x / ppw;
    m_cam.pan.y -= io.MouseDelta.y / ppw;

    _clamp_pan(m_avail);                         // keep viewport inside 0-1000²
}


//  "_clamp_pan"
//
void Editor::_clamp_pan(const ImVec2& view_sz)
{
    float ppw = m_cam.pixels_per_world(view_sz, m_world_bounds);

    // viewport size in world units
    float view_w = view_sz.x / ppw;
    float view_h = view_sz.y / ppw;

    float world_w = m_world_bounds.max_x - m_world_bounds.min_x;
    float world_h = m_world_bounds.max_y - m_world_bounds.min_y;

    // if the viewport is larger than the world on an axis, keep it centred
    if (view_w >= world_w) {
        m_cam.pan.x = m_world_bounds.min_x - (view_w - world_w) * 0.5f;
    } else {
        m_cam.pan.x = std::clamp(
            m_cam.pan.x,
            m_world_bounds.min_x,
            m_world_bounds.max_x - view_w);
    }

    if (view_h >= world_h) {
        m_cam.pan.y = m_world_bounds.min_y - (view_h - world_h) * 0.5f;
    } else {
        m_cam.pan.y = std::clamp(
            m_cam.pan.y,
            m_world_bounds.min_y,
            m_world_bounds.max_y - view_h);
    }
}
    







// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.











// *************************************************************************** //
// *************************************************************************** //
//
//  END.

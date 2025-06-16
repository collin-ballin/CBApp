/***********************************************************************************
*
*       *********************************************************************
*       ****             E D I T O R . C P P  ____  F I L E              ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      June 12, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "widgets/editor/editor.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //






//  0.      STATIC VARIABLES...
// *************************************************************************** //
// *************************************************************************** //


//  0.      STATIC FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

















// *************************************************************************** //
//
//
//
//  2.  PUBLIC API...
// *************************************************************************** //
// *************************************************************************** //

//  "Begin"
//
void Editor::Begin(const char * id)
{
    const float     bar_h           = ImGui::GetFrameHeight() + ImGui::GetStyle().ItemSpacing.y;


    //  0.  LAYOUT RECTANGLE...
    ImVec2          p0              = ImGui::GetCursorScreenPos();
    ImVec2          avail           = ImGui::GetContentRegionAvail();
    avail.x                         = std::max(avail.x, 50.f);
    avail.y                         = std::max(avail.y, 50.f);
    if (avail.y > bar_h)
        avail.y -= bar_h;
        
    ImVec2          p1              = { p0.x + avail.x, p0.y + avail.y };
    ImDrawList *    dl              = ImGui::GetWindowDrawList();
    ImGuiIO &       io              = ImGui::GetIO();

    dl->AddRectFilled(p0, p1, IM_COL32(50, 50, 50, 255));
    dl->AddRect      (p0, p1, IM_COL32(255, 255, 255, 255));



    //  1.  CAPTURE AREA...
    //
    ImGui::InvisibleButton(id, avail, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
    bool            hovered         = ImGui::IsItemHovered();
    bool            active          = ImGui::IsItemActive();
    bool            space           = ImGui::IsKeyDown(ImGuiKey_Space);



    //  2.  LOCAL HOT KEYS --- Only active when cursor is over the canvas area...
    //
    if (hovered) {
        const bool no_mod = !io.KeyCtrl && !io.KeyShift && !io.KeyAlt && !io.KeySuper;
        if ( no_mod && ImGui::IsKeyPressed(ImGuiKey_V) )
            m_mode = Mode::Default;
        if ( no_mod && ImGui::IsKeyPressed(ImGuiKey_P) )
            m_mode = Mode::Point;
        if ( no_mod && ImGui::IsKeyPressed(ImGuiKey_N) )
            m_mode = Mode::Pen;
    }

    ImVec2          origin          = { p0.x + m_scroll.x, p0.y + m_scroll.y };
    ImVec2          mouse_canvas    = { io.MousePos.x - origin.x, io.MousePos.y - origin.y };
    Interaction     it              { hovered, active, space, mouse_canvas, origin, p0, dl };



    //  3.  CURSOR HINT --- Draw "hand shaped cursor" when hovering over a selectable object...
    //
    if ( !space && _mode_has(Cap_Select) )
        _update_cursor_select(it);



    //  4.  PAN NAVIGATOR WHEN USER IS HOLDING DOWN "SPACE" BAR...
    //
    if ( space && hovered )
        ImGui::SetMouseCursor(ImGui::IsMouseDown(ImGuiMouseButton_Left) ? ImGuiMouseCursor_ResizeAll : ImGuiMouseCursor_Hand);



    if ( space && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f) )
    {
        m_scroll.x += io.MouseDelta.x;
        m_scroll.y += io.MouseDelta.y;
    }

    // Global selection / drag (unless Space held or mode opts out)
    if ( !space && _mode_has(Cap_Select) )
        _process_selection(it);



    // Mode‑specific processing (skip while panning)
    if ( !(space && ImGui::IsMouseDown(ImGuiMouseButton_Left)) )
    {
        switch (m_mode)
        {
            case Mode::Default:     _handle_default(it);    break;
            case Mode::Line:        _handle_line(it);       break;
            case Mode::Point:       _handle_point(it);      break;
            case Mode::Pen:         _handle_pen(it);        break;
            default: break;
        }
    }



    // ------------------ Rendering ------------------
    dl->PushClipRect(p0, p1, true);
    if (m_show_grid) _draw_grid(dl, p0, avail);
    
    _draw_lines(dl, origin);
    _draw_paths(dl, origin);   // new
    _draw_points(dl, origin);
    
    _draw_selection_overlay(dl, origin);
    dl->PopClipRect();

    // ------------------ Control bar ----------------
    _draw_controls({ p0.x, p1.y + ImGui::GetStyle().ItemSpacing.y });
    ImGui::Dummy({ avail.x, bar_h });
    
    
    return;
}



// *************************************************************************** //
//
//
//
//  3.  MAIN MANAGER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_handle_default"
//
void Editor::_handle_default(const Interaction& it)
{
    ImGuiIO & io = ImGui::GetIO();
    
    
    //  0.  BBOX HANDLE HOVER...
    if ( !m_boxdrag.active && m_hover_handle != -1 && ImGui::IsMouseClicked(ImGuiMouseButton_Left) )
    {
        ImVec2 tl, br;
        if (_selection_bounds(tl, br))
            _start_bbox_drag(static_cast<uint8_t>(m_hover_handle), tl, br);
    }
        
        
    //  2.   Update BBOX...
    if (m_boxdrag.active)
        _update_bbox();
    
    
    //  3.  IGNORE ALL INPUT IF SPACE KEY IS HELD DOWN...
    if (it.space)
        return;


    //  4.  LASSO START...                                                      //          |=== nothing under cursor
    if ( !m_lasso_active && it.hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !_hit_any(it) )
        this->_start_lasso_tool();
    
    
    //  5.  LASSO UPDATE...
    if (m_lasso_active) {
        this->_update_lasso(it);
        return;   // Skip zoom handling while dragging lasso
    }
        

    //  6.   ZOOM (mouse wheel) – only when not lassoing...
    if (it.hovered && io.MouseWheel != 0.0f)
        this->_zoom_canvas(it);
        
    
    return;
}








//  "_handle_line"
//
void Editor::_handle_line(const Interaction& it)
{
    if (it.space) return; // ignore while panning

    ImVec2 w = { it.canvas.x / m_zoom, it.canvas.y / m_zoom };

    // 1) Begin a brand‑new line/path on LMB press
    if (it.hovered && !m_drawing && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        uint32_t a = _add_vertex(w);
        uint32_t b = _add_vertex(w);
        // Create visible anchor glyphs so the endpoints behave like legacy points
        m_points.push_back({ a, { COL_POINT_DEFAULT, DEFAULT_POINT_RADIUS, true } });
        m_points.push_back({ b, { COL_POINT_DEFAULT, DEFAULT_POINT_RADIUS, true } });

        // Legacy Line container (kept for now)
        // m_lines.push_back({ a, b });

        // NEW: provisional Path (open polyline of length 2)
        Path p;
        p.verts = { a, b };
        p.closed = false;
        m_paths.push_back(std::move(p));

        m_drawing = true;
    }

    // 2) Rubber‑band the second vertex while the mouse is down
    if (m_drawing)
    {
        // Update the Path’s second vertex while rubber‑banding
        Path& last_path = m_paths.back();
        if (last_path.verts.size() >= 2)
        {
            if (Pos* v = find_vertex(m_vertices, last_path.verts[1]))
            {
                v->x = w.x; v->y = w.y;
            }
        }

        // 3) Commit once the mouse button is released
        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
            m_drawing = false;
    }
}


//  "_handle_point"
//
void Editor::_handle_point(const Interaction& it)
{
    if (it.space) return;

    ImVec2 w = { it.canvas.x / m_zoom, it.canvas.y / m_zoom };

    if (it.hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        // global selection already ran; only add a new point if click hit nothing
        if (!_hit_any(it))
            _add_point(w);
    }
}


//  "_handle_pen"
//
void Editor::_handle_pen(const Interaction & it)
{
    if (it.space) return;                           // ignore while panning

    // 0) Highest-priority: cancel key
    if (_pen_cancel_if_escape(it))
        return;

    // 1) If we're already dragging a handle, update & early-out
    if (m_pen.dragging_handle)
    {
        _pen_update_handle_drag(it);
        return;
    }

    // 2) Start handle-drag on Alt-click (returns true if it consumed the click)
    if (_pen_try_begin_handle_drag(it))
        return;

    // 3) Mouse-click logic for creating / extending / closing a path
    if (it.hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        if (!m_pen.active)
            _pen_begin_path_if_click_empty(it);
        else
            _pen_append_or_close_live_path(it);
    }
    
    return;
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

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
    m_bar_h                         = ImGui::GetFrameHeight() + ImGui::GetStyle().ItemSpacing.y;
    m_avail                         = ImGui::GetContentRegionAvail();
    m_avail.x                       = std::max(m_avail.x,   50.f);
    m_avail.y                       = std::max(m_avail.y,   50.f);
    if (m_avail.y > m_bar_h)
        m_avail.y -= m_bar_h;
    
    m_p0                            = ImGui::GetCursorScreenPos();
    m_p1                            = { m_p0.x + m_avail.x,     m_p0.y + m_avail.y };



    //  0.  LAYOUT RECTANGLE...
    ImDrawList *    dl              = ImGui::GetWindowDrawList();
    ImGuiIO &       io              = ImGui::GetIO();

    dl->AddRectFilled(m_p0, m_p1,   IM_COL32(50, 50, 50, 255));
    dl->AddRect      (m_p0, m_p1,   IM_COL32(255, 255, 255, 255));



    //  1.  CAPTURE AREA...
    //
    ImGui::InvisibleButton(id, m_avail, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
    bool            hovered         = ImGui::IsItemHovered();
    bool            active          = ImGui::IsItemActive();
    bool            space           = ImGui::IsKeyDown(ImGuiKey_Space);



    //  2.  LOCAL HOT KEYS --- Only active when cursor is over the canvas area...
    //
    if (hovered)
    {
        const bool no_mod = !io.KeyCtrl && !io.KeyShift && !io.KeyAlt && !io.KeySuper;
        
        if ( no_mod && ImGui::IsKeyPressed(ImGuiKey_V) )        { m_mode = Mode::Default;           }
        if ( no_mod && ImGui::IsKeyPressed(ImGuiKey_N) )        { m_mode = Mode::Point;             }
        if ( no_mod && ImGui::IsKeyPressed(ImGuiKey_P) )        { m_mode = Mode::Pen;               }
        //
        if ( no_mod && ImGui::IsKeyPressed(ImGuiKey_C) )        { m_mode = Mode::Scissor;           }
        if ( no_mod && ImGui::IsKeyPressed(ImGuiKey_Equal) )    { m_mode = Mode::AddAnchor;         }
        if ( no_mod && ImGui::IsKeyPressed(ImGuiKey_Minus) )    { m_mode = Mode::RemoveAnchor;      }
    }
    
    if ( m_mode != Mode::Pen )  m_pen = {};          // clears active/dragging state


    ImVec2          origin          = { m_p0.x + m_scroll.x, m_p0.y + m_scroll.y };
    ImVec2          mouse_canvas    = { io.MousePos.x - origin.x, io.MousePos.y - origin.y };
    Interaction     it              { hovered, active, space, mouse_canvas, origin, m_p0, dl };



    //  3.  CURSOR HINT --- Draw "hand shaped cursor" when hovering over a selectable object...
    //
    if ( !space && _mode_has(Cap_Select) )
        _update_cursor_select(it);

    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_J))
        _join_selected_open_path();



    //  4.  PAN NAVIGATOR WHEN USER IS HOLDING DOWN "SPACE" BAR...
    //
    if ( space && hovered )
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);



    //  5.  HANDLE "PANNING" IF USER (1) HOLDS SPACE KEY (2) CLICKS-AND-DRAGS WITH LMB...
    if ( it.space && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f) ) {
        m_scroll.x += io.MouseDelta.x;   // ← add instead of subtract
        m_scroll.y += io.MouseDelta.y;   // ← add instead of subtract
        _clamp_scroll();
    }

    //  Global selection / drag (unless Space held or mode opts out)
    if ( !space && _mode_has(Cap_Select) )
        _process_selection(it);



    //  MODE DISPATCHER (Mode‑specific processing---skip while panning)...
    if ( !(space && ImGui::IsMouseDown(ImGuiMouseButton_Left)) )
    {
        switch (m_mode) {
            case Mode::Default:         this->_handle_default           (it);       break;
            case Mode::Line:            this->_handle_line              (it);       break;
            case Mode::Point:           this->_handle_point             (it);       break;
            case Mode::Pen:             this->_handle_pen               (it);       break;
            case Mode::Scissor:         this->_handle_scissor           (it);       break;
            case Mode::AddAnchor:       this->_handle_add_anchor        (it);       break;
            case Mode::RemoveAnchor:    this->_handle_remove_anchor     (it);       break;
            //
            default: break;
        }
    }

    //  PEN TOOL CURSOR...
    //if ( m_mode == Mode::Pen && it.hovered && !space )
    //    _draw_pen_cursor(it);



    // ------------------ Rendering ------------------
    dl->PushClipRect(m_p0, m_p1, true);
    _grid_handle_shortcuts();         // adjust step if hot-key pressed
    _grid_draw(dl, m_p0, m_avail);    // draw if visible


    
    _draw_lines(dl, origin);
    _draw_paths(dl, origin);   // new
    _draw_points(dl, origin);
    
    _draw_selection_overlay(dl, origin);
    dl->PopClipRect();
    
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
    ImGuiIO &   io          = ImGui::GetIO();
    
    //  0.  BBOX HANDLE HOVER...
    if ( !m_boxdrag.active && m_hover_handle != -1 && ImGui::IsMouseClicked(ImGuiMouseButton_Left) ) {
        ImVec2 tl, br;
        if (_selection_bounds(tl, br))
            _start_bbox_drag(static_cast<uint8_t>(m_hover_handle), tl, br);
    }
        
        
    //  2.   Update BBOX...
    if (m_boxdrag.active)                           { _update_bbox(); }
    
    
    //  3.  IGNORE ALL INPUT IF SPACE KEY IS HELD DOWN...
    if (it.space)                                   { return; }


    //  4.  LASSO START...                                                      //          |=== nothing under cursor
    if ( !m_lasso_active && it.hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !_hit_any(it) )
        this->_start_lasso_tool();
    
    
    //  5.  LASSO UPDATE...
    if (m_lasso_active)                             { this->_update_lasso(it); return; }        // Skip zoom handling while dragging lasso
        

    //  6.   ZOOM (mouse wheel) – only when not lassoing...
    if (it.hovered && io.MouseWheel != 0.0f)        { this->_zoom_canvas(it); }
        
        
    //  7.   EDIT BEZIER CTRL POINTS IN DEFAULT STATE...
    if (m_pen.dragging_handle)                      { _pen_update_handle_drag(it); return; }    // continue an active handle drag
    if ( _pen_try_begin_handle_drag(it) )           { return; }                                 // Alt-click started a new drag
    
    
    
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
    // handle-drag branch (unchanged)
    if (m_pen.dragging_handle) {
        _pen_update_handle_drag(it);
        return;
    }

    // ── Click logic ──────────────────────────────────────────────────────
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && it.hovered)
    {
        // (A) If a path is already live → append / close as usual
        if (m_pen.active) {
            _pen_append_or_close_live_path(it);
            return;
        }

        // (B) No live path: did we click on an existing point?
        int pi = _hit_point(it);
        if (pi >= 0) {
            uint32_t vid = m_points[pi].v;
            if (auto idx = _path_idx_if_last_vertex(vid)) {
                // Continue that path on the NEXT click
                m_pen.active     = true;
                m_pen.path_index = *idx;
                m_pen.last_vid   = vid;
                return;          // wait for next click to add a vertex
            }
        }

        // (C) Otherwise start a brand-new path
        ImVec2 ws{ it.canvas.x / m_zoom, it.canvas.y / m_zoom };
        uint32_t vid = _add_vertex(ws);
        _add_point_glyph(vid);

        Path p;
        p.verts.push_back(vid);
        m_paths.push_back(std::move(p));

        m_pen.active     = true;
        m_pen.path_index = m_paths.size() - 1;
        m_pen.last_vid   = vid;
    }
}


//  "_handle_pen"
//
void Editor::_handle_pen(const Interaction & it)
{
    // Abort current path on ESC
    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        m_pen = {};
        return;
    }

    // Handle-drag (unchanged)
    if (m_pen.dragging_handle) {
        _pen_update_handle_drag(it);
        return;
    }

    // Hover test for extendable endpoint
    bool can_extend_here = false;
    int  point_idx = _hit_point(it);
    uint32_t end_vid = 0;
    size_t   end_path = static_cast<size_t>(-1);

    if (point_idx >= 0) {
        end_vid = m_points[point_idx].v;
        if (auto idx = _path_idx_if_last_vertex(end_vid)) {
            can_extend_here = true;
            end_path = *idx;
        }
    }

    // Cursor hint (green if extendable, else yellow)
    if (it.hovered && !it.space) {
        ImVec2 mouse_screen = ImGui::GetIO().MousePos;
        _draw_pen_cursor(mouse_screen,
                         can_extend_here ? PEN_COL_EXTEND : PEN_COL_NORMAL);
    }

    // Click handling (unchanged logic but colour consts removed)
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && it.hovered)
    {
        if (can_extend_here && !m_pen.active) {
            m_pen.active     = true;
            m_pen.path_index = end_path;
            m_pen.last_vid   = end_vid;
            return;                 // wait for next click to extend
        }

        if (m_pen.active) {
            _pen_append_or_close_live_path(it);
            return;
        }

        // Start new path
        ImVec2 ws{ it.canvas.x / m_zoom, it.canvas.y / m_zoom };
        uint32_t vid = _add_vertex(ws);
        _add_point_glyph(vid);

        Path p;
        p.verts.push_back(vid);
        m_paths.push_back(std::move(p));

        m_pen.active     = true;
        m_pen.path_index = m_paths.size() - 1;
        m_pen.last_vid   = vid;
    }
}



//  "_handle_scissor"
//
void Editor::_handle_scissor(const Interaction & it)
{
    if (!it.hovered) return;

    // test once per frame
    std::optional<PathHit> hit = _hit_path_segment(it);

    // cursor hint (vertical beam) whenever a cut location is valid
    if (hit) ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);

    // left-click performs the cut
    if (hit && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        _scissor_cut(*hit);
        
    return;
}


//  "_handle_add_anchor"
//
void Editor::_handle_add_anchor(const Interaction & it)
{
    return;
}


//  "_handle_remove_anchor"
//
void Editor::_handle_remove_anchor(const Interaction & it)
{
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

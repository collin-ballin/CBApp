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
    const float bar_h = ImGui::GetFrameHeight() + ImGui::GetStyle().ItemSpacing.y;

    // ------------------ Layout rect ------------------
    ImVec2 p0 = ImGui::GetCursorScreenPos();
    ImVec2 avail = ImGui::GetContentRegionAvail();
    avail.x = std::max(avail.x, 50.f);
    avail.y = std::max(avail.y, 50.f);
    if (avail.y > bar_h) avail.y -= bar_h;
    ImVec2 p1 = { p0.x + avail.x, p0.y + avail.y };

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImGuiIO& io    = ImGui::GetIO();


    dl->AddRectFilled(p0, p1, IM_COL32(50, 50, 50, 255));
    dl->AddRect      (p0, p1, IM_COL32(255, 255, 255, 255));

    // Capture area
    ImGui::InvisibleButton(id, avail, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
    bool hovered = ImGui::IsItemHovered();
    bool active  = ImGui::IsItemActive();
    bool space   = ImGui::IsKeyDown(ImGuiKey_Space);

    // Local hot‑keys — only active when cursor is over the canvas area
    if (hovered) {
        const bool no_mod = !io.KeyCtrl && !io.KeyShift && !io.KeyAlt && !io.KeySuper;
        if (no_mod && ImGui::IsKeyPressed(ImGuiKey_V))
            m_mode = Mode::Default;
        if (no_mod && ImGui::IsKeyPressed(ImGuiKey_P))
            m_mode = Mode::Point;
        if (no_mod && ImGui::IsKeyPressed(ImGuiKey_N))
            m_mode = Mode::Pen;
    }

    ImVec2 origin = { p0.x + m_scroll.x, p0.y + m_scroll.y };
    ImVec2 mouse_canvas = { io.MousePos.x - origin.x, io.MousePos.y - origin.y };

    Interaction it{ hovered, active, space, mouse_canvas, origin, p0, dl };

    // Cursor hint: hand when object under mouse is selectable
    if (!space && _mode_has(Cap_Select))
        _update_cursor_select(it);

    // Cursor + pan with Space+LMB
    if (space && hovered)
        ImGui::SetMouseCursor(ImGui::IsMouseDown(ImGuiMouseButton_Left) ? ImGuiMouseCursor_ResizeAll : ImGuiMouseCursor_Hand);

    if (space && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f))
    {
        m_scroll.x += io.MouseDelta.x;
        m_scroll.y += io.MouseDelta.y;
    }

    // Global selection / drag (unless Space held or mode opts out)
    if (!space && _mode_has(Cap_Select))
        _process_selection(it);

    // Mode‑specific processing (skip while panning)
    if (!(space && ImGui::IsMouseDown(ImGuiMouseButton_Left)))
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
    // Ignore everything else while the user is panning with the Space key
    if (it.space)
        return;

    ImGuiIO& io = ImGui::GetIO();

    /*---------------------------------------------------------------------
     * 1) LASSO START
     *    Only begins if the user clicks on empty canvas (no object hit).
     *    Unless Shift or Ctrl is held, any prior selection is cleared.
     *-------------------------------------------------------------------*/
    if (!m_lasso_active &&
        it.hovered &&
        ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
        !_hit_any(it))                                // ← nothing under cursor
    {
        m_lasso_active = true;
        m_lasso_start  = io.MousePos;                 // in screen coords
        m_lasso_end    = m_lasso_start;
        if (!io.KeyShift && !io.KeyCtrl)
            m_sel.clear();                            // fresh selection
    }

    /*---------------------------------------------------------------------
     * 2) LASSO UPDATE
     *    While the mouse button is held, draw the translucent rectangle.
     *    When the button is released, convert the rect to world space
     *    and add all intersecting objects to the selection set.
     *-------------------------------------------------------------------*/
    if (m_lasso_active)
    {
        m_lasso_end = io.MousePos;

        // Visual feedback
        it.dl->AddRectFilled(m_lasso_start, m_lasso_end, COL_LASSO_FILL);
        it.dl->AddRect      (m_lasso_start, m_lasso_end, COL_LASSO_OUT);

        // On mouse-up: finalise selection
        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            // Screen-space rect → TL/BR order
            ImVec2 tl_scr{ std::min(m_lasso_start.x, m_lasso_end.x),
                           std::min(m_lasso_start.y, m_lasso_end.y) };
            ImVec2 br_scr{ std::max(m_lasso_start.x, m_lasso_end.x),
                           std::max(m_lasso_start.y, m_lasso_end.y) };

            // Convert to world space (inverse pan + zoom)
            ImVec2 tl_w{ (tl_scr.x - it.origin.x) / m_zoom,
                         (tl_scr.y - it.origin.y) / m_zoom };
            ImVec2 br_w{ (br_scr.x - it.origin.x) / m_zoom,
                         (br_scr.y - it.origin.y) / m_zoom };

            bool additive = io.KeyShift || io.KeyCtrl;
            if (!additive)                       // fresh selection if no modifier
                m_sel.clear();

            /* ---------- Points ---------- */
            for (size_t i = 0; i < m_points.size(); ++i)
            {
                const Pos* v = find_vertex(m_vertices, m_points[i].v);
                if (!v) continue;
                bool inside = (v->x >= tl_w.x && v->x <= br_w.x &&
                               v->y >= tl_w.y && v->y <= br_w.y);
                if (!inside) continue;

                if (additive) {                  // ⇧ / Ctrl  ⇒ toggle
                    if (!m_sel.points.erase(i))   // erase returns 0 if not present
                        m_sel.points.insert(i);
                } else {
                    m_sel.points.insert(i);
                }
            }

            // ---------- Lines (segment–rect test) ----------
            auto seg_rect_intersect = [](ImVec2 a, ImVec2 b, ImVec2 tl, ImVec2 br)->bool
            {
                auto inside = [&](ImVec2 p){
                    return p.x >= tl.x && p.x <= br.x && p.y >= tl.y && p.y <= br.y;
                };
                if (inside(a) || inside(b))
                    return true;

                // quick reject: both points on same outside side
                if ((a.x < tl.x && b.x < tl.x) || (a.x > br.x && b.x > br.x) ||
                    (a.y < tl.y && b.y < tl.y) || (a.y > br.y && b.y > br.y))
                    return false;

                // helper for segment–segment intersection
                auto ccw = [](ImVec2 p1, ImVec2 p2, ImVec2 p3){
                    return (p3.y - p1.y)*(p2.x - p1.x) > (p2.y - p1.y)*(p3.x - p1.x);
                };
                auto intersect = [&](ImVec2 p1, ImVec2 p2, ImVec2 p3, ImVec2 p4){
                    return ccw(p1,p3,p4) != ccw(p2,p3,p4) && ccw(p1,p2,p3) != ccw(p1,p2,p4);
                };

                ImVec2 tr{ br.x, tl.y }, bl{ tl.x, br.y };
                return intersect(a,b, tl,tr) || intersect(a,b,tr,br) ||
                       intersect(a,b, br,bl) || intersect(a,b, bl,tl);
            };
            
            
            for (size_t i = 0; i < m_lines.size(); ++i)
            {
                const Pos* a = find_vertex(m_vertices, m_lines[i].a);
                const Pos* b = find_vertex(m_vertices, m_lines[i].b);
                if (!a || !b) continue;

                if (!seg_rect_intersect({a->x,a->y}, {b->x,b->y}, tl_w, br_w))
                    continue;

                if (additive) {
                    if (!m_sel.lines.erase(i))
                        m_sel.lines.insert(i);    // toggle
                } else {
                    m_sel.lines.insert(i);
                }
            }
            
            
            // ---------- Paths (segment–rect test, straight segments only) ----------
            for (size_t pi = 0; pi < m_paths.size(); ++pi)
            {
                const Path& p = m_paths[pi];
                const size_t N = p.verts.size();
                if (N < 2) continue;

                bool intersects = false;
                for (size_t si = 0; si < N - 1 + (p.closed ? 1 : 0); ++si)
                {
                    const Pos* a = find_vertex(m_vertices, p.verts[si]);
                    const Pos* b = find_vertex(m_vertices, p.verts[(si+1)%N]);
                    if (!a || !b) continue;
                    if (seg_rect_intersect({a->x,a->y}, {b->x,b->y}, tl_w, br_w))
                    { intersects = true; break; }
                }
                if (!intersects) continue;

                if (additive) {
                    if (!m_sel.paths.erase(pi))
                        m_sel.paths.insert(pi);    // toggle
                } else {
                    m_sel.paths.insert(pi);
                }
            }
            

            /* ---------- Sync vertex list ---------- */
            _rebuild_vertex_selection();

            m_lasso_active = false;                   // reset
        }

        return;   // Skip zoom handling while dragging lasso
    }

    /*---------------------------------------------------------------------
     * 3) ZOOM (mouse wheel) – only when not lassoing
     *-------------------------------------------------------------------*/
    if (it.hovered && io.MouseWheel != 0.0f)
    {
        float new_zoom = std::clamp(m_zoom * (1.0f + io.MouseWheel * 0.1f),
                                    0.25f, 4.0f);
        if (std::fabs(new_zoom - m_zoom) > 1e-6f)
        {
            // Preserve cursor-centred zoom
            ImVec2 world_before{ (io.MousePos.x - it.origin.x) / m_zoom,
                                 (io.MousePos.y - it.origin.y) / m_zoom };
            m_zoom = new_zoom;
            ImVec2 new_origin{ io.MousePos.x - world_before.x * m_zoom,
                               io.MousePos.y - world_before.y * m_zoom };
            m_scroll.x = new_origin.x - it.tl.x;
            m_scroll.y = new_origin.y - it.tl.y;
        }
    }
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

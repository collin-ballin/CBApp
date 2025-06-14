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
//  1.  HELPERS...
// *************************************************************************** //
// *************************************************************************** //

//  "find_vertex"
//
Editor::Pos * Editor::find_vertex(std::vector<Pos>& verts, uint32_t id) {
    for (auto & v : verts) if (v.id == id) return &v; return nullptr;
}


//  "find_vertex"
//
const Editor::Pos * Editor::find_vertex(const std::vector<Pos>& verts, uint32_t id) const {
    for (auto & v : verts) if (v.id == id) return &v; return nullptr;
}








// *************************************************************************** //
//
//
//  2.  IMPLEMENTATION...
// *************************************************************************** //
// *************************************************************************** //

//=============================================================================
//  Editor : public API
//=============================================================================
void Editor::Begin(const char* id)
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



//=============================================================================
//  Mode handlers
//=============================================================================
//──────────────────────── handle_default  ────────────────────────────────
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


//=============================================================================
//  Data helpers
//=============================================================================
uint32_t Editor::_add_vertex(ImVec2 w)
{
    m_vertices.push_back({ m_next_id++, w.x, w.y });
    return m_vertices.back().id;
}

void Editor::_add_point(ImVec2 w)
{
    uint32_t vid = _add_vertex(w);
    m_points.push_back({ vid, { COL_POINT_DEFAULT, DEFAULT_POINT_RADIUS, true } });
}

int Editor::_hit_point(const Interaction& it) const
{
    for (size_t i = 0; i < m_points.size(); ++i)
    {
        const Pos* v = find_vertex(m_vertices, m_points[i].v);
        if (!v) continue;
        ImVec2 scr{ it.origin.x + v->x * m_zoom, it.origin.y + v->y * m_zoom };
        float dx = scr.x - ImGui::GetIO().MousePos.x;
        float dy = scr.y - ImGui::GetIO().MousePos.y;
        if (dx*dx + dy*dy <= HIT_THRESH_SQ) return static_cast<int>(i);
    }
    return -1;
}




// *************************************************************************** //
//
//
//  ?.  ???
// *************************************************************************** //
// *************************************************************************** //



//=============================================================================
//  Rendering helpers
//=============================================================================
void Editor::_draw_grid(ImDrawList* dl, const ImVec2& p0, const ImVec2& sz) const
{
    float step = GRID_STEP * m_zoom;
    float x0 = std::fmod(m_scroll.x, step);
    for (float x = x0; x < sz.x; x += step)
        dl->AddLine({ p0.x + x, p0.y }, { p0.x + x, p0.y + sz.y }, IM_COL32(200,200,200,40));
    float y0 = std::fmod(m_scroll.y, step);
    for (float y = y0; y < sz.y; y += step)
        dl->AddLine({ p0.x, p0.y + y }, { p0.x + sz.x, p0.y + y }, IM_COL32(200,200,200,40));
}

void Editor::_draw_lines(ImDrawList* dl, const ImVec2& origin) const
{
#ifdef LEGACY_LINES
    for (const auto& ln : m_lines)
    {
        const Pos* a = find_vertex(m_vertices, ln.a);
        const Pos* b = find_vertex(m_vertices, ln.b);
        if (!a || !b) continue;
        dl->AddLine({ origin.x + a->x * m_zoom, origin.y + a->y * m_zoom },
                    { origin.x + b->x * m_zoom, origin.y + b->y * m_zoom },
                    ln.color, ln.thickness);
    }
#else
    (void)dl; (void)origin; // suppress unused‑param warnings
#endif
}

void Editor::_draw_paths(ImDrawList* dl, const ImVec2& origin) const
{
    for (const Path& p : m_paths)
    {
        const size_t N = p.verts.size();
        if (N < 2) continue;

        auto draw_seg = [&](const Pos* a, const Pos* b){
            bool curved = (a->out_handle.x || a->out_handle.y ||
                           b->in_handle.x  || b->in_handle.y);

            if (!curved)    // straight line
            {
                dl->AddLine({ origin.x + a->x * m_zoom, origin.y + a->y * m_zoom },
                            { origin.x + b->x * m_zoom, origin.y + b->y * m_zoom },
                            p.style.stroke_color, p.style.stroke_width);
            }
            else            // cubic Bézier
            {
                ImVec2 P0{ origin.x + a->x * m_zoom,                  origin.y + a->y * m_zoom };
                ImVec2 P1{ origin.x + (a->x + a->out_handle.x) * m_zoom,
                           origin.y + (a->y + a->out_handle.y) * m_zoom };
                ImVec2 P2{ origin.x + (b->x + b->in_handle.x)  * m_zoom,
                           origin.y + (b->y + b->in_handle.y)  * m_zoom };
                ImVec2 P3{ origin.x + b->x * m_zoom,                  origin.y + b->y * m_zoom };

                dl->AddBezierCubic(P0, P1, P2, P3,
                                   p.style.stroke_color,
                                   p.style.stroke_width,
                                   ms_BEZIER_SEGMENTS);  // 0 ⇒ ImGui default tessellation
            }
        };

        for (size_t i = 0; i < N - 1; ++i)
            if (const Pos* a = find_vertex(m_vertices, p.verts[i]))
                if (const Pos* b = find_vertex(m_vertices, p.verts[i+1]))
                    draw_seg(a, b);

        if (p.closed)
            if (const Pos* a = find_vertex(m_vertices, p.verts.back()))
                if (const Pos* b = find_vertex(m_vertices, p.verts.front()))
                    draw_seg(a, b);
    }
}


void Editor::_draw_points(ImDrawList* dl, const ImVec2& origin) const
{
    for (size_t i = 0; i < m_points.size(); ++i)
    {
        const Point& pt = m_points[i]; if (!pt.sty.visible) continue;
        const Pos* v = find_vertex(m_vertices, pt.v); if (!v) continue;
        ImU32 col = (m_dragging && m_sel.points.count(i)) ? COL_POINT_HELD : pt.sty.color;
        dl->AddCircleFilled({ origin.x + v->x * m_zoom, origin.y + v->y * m_zoom }, pt.sty.radius, col);
    }
}




//=============================================================================
//  UI controls & clear
//=============================================================================
void Editor::_draw_controls(const ImVec2& pos)
{
    ImGui::SetCursorScreenPos(pos);
    ImGui::BeginGroup();

    // Mode selector
    int mode_i = static_cast<int>(m_mode);
    ImGui::SetNextItemWidth(110.0f);
    ImGui::Combo("Mode", &mode_i, ms_MODE_LABELS.data(),
                 static_cast<int>(Mode::Count));
    m_mode = static_cast<Mode>(mode_i);

    // Grid toggle
    ImGui::SameLine(0.0f, 15.0f);
    ImGui::Checkbox("Grid", &m_show_grid);

    // Clear button
    ImGui::SameLine(0.0f, 15.0f);
    if (ImGui::Button("Clear"))
        _clear_all();

    ImGui::EndGroup();
}


// ────────────────────────── clear everything ─────────────────────────────
void Editor::_clear_all()
{
    m_vertices.clear();
    m_points.clear();
    m_lines.clear();
    m_sel.clear();
    m_lasso_active  = false;
    m_dragging      = false;
    m_drawing       = false;
    m_next_id       = 1;
    m_pen = {};
}









// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //

//──────────────────────────────── hit any object ────────────────────────────
std::optional<Hit> Editor::_hit_any(const Interaction& it) const
{
    // points first
    int pi = _hit_point(it);
    if (pi >= 0) return Hit{ Hit::Type::Point, static_cast<size_t>(pi) };

    // then lines
    ImVec2 w{ it.canvas.x / m_zoom, it.canvas.y / m_zoom };
    const float thresh_sq = HIT_THRESH_SQ / (m_zoom * m_zoom);
    for (size_t i = 0; i < m_lines.size(); ++i)
    {
        const Pos* a = find_vertex(m_vertices, m_lines[i].a);
        const Pos* b = find_vertex(m_vertices, m_lines[i].b);
        if (!a || !b) continue;

        ImVec2 ab{ b->x - a->x, b->y - a->y };
        ImVec2 ap{ w.x - a->x,  w.y - a->y };
        float ab_len_sq = ab.x*ab.x + ab.y*ab.y;
        float t = (ab_len_sq > 0.f) ? (ap.x*ab.x + ap.y*ab.y) / ab_len_sq : 0.f;
        t = std::clamp(t, 0.f, 1.f);
        ImVec2 c{ a->x + ab.x*t, a->y + ab.y*t };
        float dx = w.x - c.x, dy = w.y - c.y;
        if (dx*dx + dy*dy <= thresh_sq)
            return Hit{ Hit::Type::Line, i };
    }
    // then path segments (curved-aware)
    for (size_t pi = 0; pi < m_paths.size(); ++pi)
    {
        const Path& p = m_paths[pi];
        const size_t N = p.verts.size();
        if (N < 2) continue;

        for (size_t si = 0; si < N - 1 + (p.closed ? 1 : 0); ++si)
        {
            const Pos* a = find_vertex(m_vertices, p.verts[si]);
            const Pos* b = find_vertex(m_vertices, p.verts[(si+1)%N]);
            if (!a || !b) continue;

            if (!is_curved(a,b))
            {
                // --- straight segment distance test (unchanged) ---
                ImVec2 ab{ b->x - a->x, b->y - a->y };
                ImVec2 ap{ w.x - a->x,  w.y - a->y };
                float ab_len_sq = ab.x*ab.x + ab.y*ab.y;
                float t = (ab_len_sq > 0.f) ? (ap.x*ab.x + ap.y*ab.y) / ab_len_sq : 0.f;
                t = std::clamp(t, 0.f, 1.f);
                ImVec2 c{ a->x + ab.x*t, a->y + ab.y*t };
                float dx = w.x - c.x, dy = w.y - c.y;
                if (dx*dx + dy*dy <= thresh_sq)
                    return Hit{ Hit::Type::Path, pi };
            }
            else
            {
                // --- curved segment: sample ms_BEZIER_HIT_STEPS sub-segments ---
                ImVec2 prev = { a->x, a->y };
                for (int k = 1; k <= ms_BEZIER_HIT_STEPS; ++k)
                {
                    float t = static_cast<float>(k) / ms_BEZIER_HIT_STEPS;
                    ImVec2 curr = cubic_eval(a, b, t);

                    ImVec2 seg{ curr.x - prev.x, curr.y - prev.y };
                    ImVec2 to_pt{ w.x - prev.x,  w.y - prev.y };
                    float seg_len_sq = seg.x*seg.x + seg.y*seg.y;
                    float u = (seg_len_sq > 0.f) ? (to_pt.x*seg.x + to_pt.y*seg.y) / seg_len_sq : 0.f;
                    u = std::clamp(u, 0.f, 1.f);
                    ImVec2 c{ prev.x + seg.x*u, prev.y + seg.y*u };
                    float dx = w.x - c.x, dy = w.y - c.y;
                    if (dx*dx + dy*dy <= thresh_sq)
                        return Hit{ Hit::Type::Path, pi };

                    prev = curr;
                }
            }
        }
    }
    
    return std::nullopt;
}

//──────────────── selection bounding box ────────────────────────────────
bool Editor::_selection_bounds(ImVec2& tl, ImVec2& br) const
{
    if (m_sel.vertices.empty()) return false;
    bool first = true;
    for (uint32_t vid : m_sel.vertices)
        if (const Pos* v = find_vertex(m_vertices, vid))
        {
            ImVec2 p{ v->x, v->y };
            if (first) { tl = br = p; first = false; }
            else {
                tl.x = std::min(tl.x, p.x); tl.y = std::min(tl.y, p.y);
                br.x = std::max(br.x, p.x); br.y = std::max(br.y, p.y);
            }
        }
    return !first;
}

//──────────────────────── global selection / drag ─────────────────────────
void Editor::_process_selection(const Interaction& it)
{
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 w{ it.canvas.x / m_zoom, it.canvas.y / m_zoom };

    // helper: unconditionally ensure an item is in the selection
    auto select_hit = [&](const Hit& hit){
        if (hit.type == Hit::Type::Point) {
            size_t idx = hit.index;
            uint32_t vid = m_points[idx].v;
            m_sel.points.insert(idx);
            m_sel.vertices.insert(vid);
        }
        else if (hit.type == Hit::Type::Path)
        {
            size_t idx = hit.index;
            const Path& p = m_paths[idx];
            m_sel.paths.insert(idx);
            for (uint32_t vid : p.verts)
                m_sel.vertices.insert(vid);
        }
        else {
            size_t idx = hit.index;
            uint32_t va = m_lines[idx].a, vb = m_lines[idx].b;
            m_sel.lines.insert(idx);
            m_sel.vertices.insert(va);
            m_sel.vertices.insert(vb);
        }
    };

    //------------- mouse press: record candidate --------------------------
    if (it.hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        m_pending_hit   = _hit_any(it);                // may be nullopt
        m_pending_clear = !io.KeyShift && !io.KeyCtrl; // will clear unless additive
    }

    //------------- drag move / drag start ---------------------------
    bool lmb = ImGui::IsMouseDown(ImGuiMouseButton_Left);
    bool dragging_now = ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f);

    // case A: selection already exists and cursor is inside it (original logic)
    if (!m_dragging && lmb && !m_sel.empty() && !m_pending_hit)
    {
        static ImVec2 press_world{};
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            press_world = w;

        bool inside_selected = false;

        int idx = _hit_point(it);
        if (idx >= 0 && m_sel.points.count(idx))
            inside_selected = true;

        if (!inside_selected && m_sel.vertices.size() > 1)
        {
            ImVec2 tl, br;
            if (_selection_bounds(tl, br))
                inside_selected = (press_world.x >= tl.x && press_world.x <= br.x &&
                                   press_world.y >= tl.y && press_world.y <= br.y);
        }

        if (inside_selected && dragging_now)
            m_dragging = true;
    }

    // case B: mouse is dragging on an *unselected* object (m_pending_hit present)
    if (!m_dragging && lmb && m_pending_hit && dragging_now)
    {
        if (m_pending_clear)
            m_sel.clear();
        select_hit(*m_pending_hit);     // add the new object
        m_pending_hit.reset();
        m_pending_clear = false;
        m_dragging = true;
    }

    //-------------- apply per‑frame translation ----------------------
    if (m_dragging)
    {
        ImVec2 d_world{ io.MouseDelta.x / m_zoom,
                        io.MouseDelta.y / m_zoom };

        if (d_world.x || d_world.y)
        {
            for (uint32_t vid : m_sel.vertices)
                if (auto* v = find_vertex(m_vertices, vid))
                {
                    v->x += d_world.x;
                    v->y += d_world.y;
                }
        }
        if (!lmb) m_dragging = false;
    }

    //------------- mouse release: apply pending selection -----------------
    if (it.hovered && ImGui::IsMouseReleased(ImGuiMouseButton_Left) && !m_dragging)
    {
        if (m_pending_hit || !m_pending_clear)   // only act if there was a press
        {
            if (m_pending_clear)
                m_sel.clear();

            if (m_pending_hit)
            {
                const auto& hit = *m_pending_hit;
                if (hit.type == Hit::Type::Point)
                {
                    size_t idx = hit.index;
                    uint32_t vid = m_points[idx].v;
                    if (!m_sel.points.erase(idx)) { m_sel.points.insert(idx); m_sel.vertices.insert(vid); }
                    else                           { m_sel.vertices.erase(vid); }
                }
                else if (hit.type == Hit::Type::Path)
                {
                    size_t idx = hit.index;
                    const Path& p = m_paths[idx];
                    if (!m_sel.paths.erase(idx)) {                // wasn’t selected → add
                        m_sel.paths.insert(idx);
                        for (uint32_t vid : p.verts)
                            m_sel.vertices.insert(vid);
                    } else {                                      // was selected → remove
                        for (uint32_t vid : p.verts)
                            m_sel.vertices.erase(vid);
                    }
                }
                else // line
                {
                    size_t idx = hit.index;
                    uint32_t va = m_lines[idx].a, vb = m_lines[idx].b;
                    if (!m_sel.lines.erase(idx)) {
                        m_sel.lines.insert(idx);
                        m_sel.vertices.insert(va); m_sel.vertices.insert(vb);
                    } else {
                        m_sel.vertices.erase(va);  m_sel.vertices.erase(vb);
                    }
                }
            }
        }
        m_pending_hit.reset();
        m_pending_clear = false;
    }

    //------------- context menu ------------------------
    if (it.hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right) && !m_sel.empty())
        ImGui::OpenPopup("SelectionCtx");

    if (!m_sel.empty() && ImGui::BeginPopup("SelectionCtx"))
    {
        if (ImGui::MenuItem("Delete"))
        {
            std::vector<size_t> pts(m_sel.points.begin(),  m_sel.points.end());
            std::vector<size_t> lns(m_sel.lines.begin(),   m_sel.lines.end());
            std::vector<size_t> pth(m_sel.paths.begin(),   m_sel.paths.end());

            std::sort(pts.rbegin(), pts.rend());
            std::sort(lns.rbegin(), lns.rend());
            std::sort(pth.rbegin(), pth.rend());

            for (size_t i : pts) m_points.erase(m_points.begin() + static_cast<long>(i));
            for (size_t i : lns) m_lines.erase (m_lines.begin()  + static_cast<long>(i));
            for (size_t i : pth) m_paths.erase(m_paths.begin()   + static_cast<long>(i));

            m_sel.clear();
        }
        ImGui::EndPopup();
    }
}

//──────────────────────── pointer-cursor hint ───────────────────────────
void Editor::_update_cursor_select(const Interaction& it) const
{
    if (!it.hovered) return;                    // outside canvas
    if (_hit_any(it))                           // anything under mouse?
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand); // ImGui’s “link” cursor
}


//──────────────────────── selection overlay ────────────────────────────
void Editor::_draw_selection_overlay(ImDrawList* dl, const ImVec2& origin) const
{
    const ImU32 col = COL_SELECTION_OUT;

    // Highlight selected points
    for (size_t idx : m_sel.points)
    {
        if (idx >= m_points.size()) continue;
        const Point& pt = m_points[idx];
        if (const Pos* v = find_vertex(m_vertices, pt.v))
            dl->AddCircle({ origin.x + v->x * m_zoom,
                            origin.y + v->y * m_zoom },
                          pt.sty.radius + 2.f,   // small outset
                          col, 0, 2.f);          // thickness 2 px
    }

    // Highlight selected lines
    for (size_t idx : m_sel.lines)
    {
        if (idx >= m_lines.size()) continue;
        const Line& ln = m_lines[idx];
        const Pos* a = find_vertex(m_vertices, ln.a);
        const Pos* b = find_vertex(m_vertices, ln.b);
        if (a && b)
            dl->AddLine({ origin.x + a->x * m_zoom, origin.y + a->y * m_zoom },
                        { origin.x + b->x * m_zoom, origin.y + b->y * m_zoom },
                        col, ln.thickness + 2.f); // 2 px thicker than original
    }
    
    // Highlight selected paths (now for curved segments too)
    for (size_t idx : m_sel.paths)
    {
        if (idx >= m_paths.size()) continue;
        const Path& p = m_paths[idx];
        const size_t N = p.verts.size();
        if (N < 2) continue;

        auto draw_seg = [&](const Pos* a, const Pos* b){
            const float w = p.style.stroke_width + 2.0f;   // outline = 2 px thicker
            if (is_curved(a,b))
            {
                ImVec2 P0{ origin.x + a->x * m_zoom,                   origin.y + a->y * m_zoom };
                ImVec2 P1{ origin.x + (a->x + a->out_handle.x) * m_zoom,
                           origin.y + (a->y + a->out_handle.y) * m_zoom };
                ImVec2 P2{ origin.x + (b->x + b->in_handle.x)  * m_zoom,
                           origin.y + (b->y + b->in_handle.y)  * m_zoom };
                ImVec2 P3{ origin.x + b->x * m_zoom,                   origin.y + b->y * m_zoom };
                dl->AddBezierCubic(P0,P1,P2,P3, col, w, ms_BEZIER_SEGMENTS);
            }
            else
            {
                dl->AddLine({ origin.x + a->x * m_zoom, origin.y + a->y * m_zoom },
                            { origin.x + b->x * m_zoom, origin.y + b->y * m_zoom },
                            col, w);
            }
        };

        for (size_t i = 0; i < N - 1; ++i)
            if (const Pos* a = find_vertex(m_vertices, p.verts[i]))
                if (const Pos* b = find_vertex(m_vertices, p.verts[i+1]))
                    draw_seg(a, b);

        if (p.closed)
            if (const Pos* a = find_vertex(m_vertices, p.verts.back()))
                if (const Pos* b = find_vertex(m_vertices, p.verts.front()))
                    draw_seg(a, b);
    }
    
}


void Editor::_rebuild_vertex_selection()
{
    m_sel.vertices.clear();
    for (size_t pi : m_sel.points)
        if (pi < m_points.size())
            m_sel.vertices.insert(m_points[pi].v);

    for (size_t li : m_sel.lines)
        if (li < m_lines.size()) {
            m_sel.vertices.insert(m_lines[li].a);
            m_sel.vertices.insert(m_lines[li].b);
        }

    // Include vertices from any selected paths
    for (size_t pi : m_sel.paths)
        if (pi < m_paths.size())
            for (uint32_t vid : m_paths[pi].verts)
                m_sel.vertices.insert(vid);
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

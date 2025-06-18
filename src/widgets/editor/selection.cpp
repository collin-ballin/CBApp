/***********************************************************************************
*
*       *********************************************************************
*       ****          S E L E C T I O N . C P P  ____  F I L E           ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      June 14, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "widgets/editor/editor.h"



namespace cb {  //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//      CORE SELECTION LOGIC STUFF...
// *************************************************************************** //
// *************************************************************************** //

//  "_hit_point"
//
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


//  "_hit_any"
//
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
        // interior test for closed shapes
        if (p.closed)
        {
            std::vector<ImVec2> poly;
            poly.reserve(N * 4);          // room for Bézier sampling

            for (size_t vi = 0; vi < N; ++vi)
            {
                const Pos* a = find_vertex(m_vertices, p.verts[vi]);
                const Pos* b = find_vertex(m_vertices, p.verts[(vi + 1) % N]);
                if (!a || !b) continue;

                if (!is_curved(a, b))
                {
                    poly.push_back({ a->x, a->y });
                }
                else
                {
                    for (int k = 0; k <= ms_BEZIER_HIT_STEPS; ++k)
                    {
                        float t = static_cast<float>(k) / ms_BEZIER_HIT_STEPS;
                        ImVec2 wpt = cubic_eval(a, b, t);
                        poly.push_back({ wpt.x, wpt.y });
                    }
                }
            }

            if (!poly.empty() && point_in_polygon(poly, w))
                return Hit{ Hit::Type::Path, pi };
        }
    }
    
    return std::nullopt;
}


//  "_hit_path_segment"
//
//      Segment-precision hit-test (straight + Bézier).
//      Returns nearest segment within a 6-px pick radius (early-out on miss).
//
std::optional<PathHit> Editor::_hit_path_segment(const Interaction& it) const
{
    constexpr float PICK_PX   = 6.0f;
    const float     thresh_sq = (PICK_PX * PICK_PX) / (m_zoom * m_zoom);

    // mouse position in world space
    const float mx = it.canvas.x / m_zoom;
    const float my = it.canvas.y / m_zoom;

    std::optional<PathHit> best;
    float best_d2 = thresh_sq;

    auto is_zero = [](const ImVec2& v){ return v.x == 0.0f && v.y == 0.0f; };

    for (size_t pi = 0; pi < m_paths.size(); ++pi)
    {
        const Path&  p = m_paths[pi];
        const size_t N = p.verts.size();
        if (N < 2) continue;

        const bool   closed     = p.closed;
        const size_t seg_count  = N - (closed ? 0 : 1);

        for (size_t si = 0; si < seg_count; ++si)
        {
            uint32_t id0 = p.verts[si];
            uint32_t id1 = p.verts[(si + 1) % N];

            const Pos* a = find_vertex(m_vertices, id0);
            const Pos* b = find_vertex(m_vertices, id1);
            if (!a || !b) continue;          // dangling ID safety

            bool curved = !is_zero(a->out_handle) || !is_zero(b->in_handle);

            if (!curved)
            {
                // ------ straight-segment distance ------
                float ax = a->x, ay = a->y;
                float bx = b->x, by = b->y;

                float abx = bx - ax, aby = by - ay;
                float ab_len_sq = abx * abx + aby * aby;
                if (ab_len_sq == 0.0f) continue;

                float apx = mx - ax, apy = my - ay;
                float t   = std::clamp((apx * abx + apy * aby) / ab_len_sq, 0.0f, 1.0f);

                float cx = ax + abx * t;
                float cy = ay + aby * t;

                float dx = mx - cx, dy = my - cy;
                float d2 = dx * dx + dy * dy;

                if (d2 < best_d2) {
                    best_d2 = d2;
                    best    = PathHit{ pi, si, t, ImVec2(cx, cy) };
                }
            }
            else
            {
                // ------ Bézier segment: sample ------
                const int STEPS = ms_BEZIER_HIT_STEPS;
                for (int k = 1; k <= STEPS; ++k)
                {
                    float  t     = static_cast<float>(k) / STEPS;
                    ImVec2 p_ws  = cubic_eval(a, b, t);   // your existing helper

                    float dx = mx - p_ws.x, dy = my - p_ws.y;
                    float d2 = dx * dx + dy * dy;

                    if (d2 < best_d2) {
                        best_d2 = d2;
                        best    = PathHit{ pi, si, t, p_ws };
                    }
                }
            }
        }
    }
    return best;   // std::nullopt if nothing within PICK_PX
}






// *************************************************************************** //
//
//
//      SELECTION IMPLEMENTATIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_process_selection"
//      global selection / drag ─────────────────────────
//
void Editor::_process_selection(const Interaction& it)
{
    ImGuiIO &       io              = ImGui::GetIO();
    const bool      lmbDown         = io.MouseDown[ImGuiMouseButton_Left];
    const bool      lmbClick        = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
    const bool      lmbUp           = ImGui::IsMouseReleased(ImGuiMouseButton_Left);
    const bool      draggingNow     = lmbDown && ImGui::IsMouseDragPastThreshold(ImGuiMouseButton_Left, 0.0f);
    //
    bool            lmb             = ImGui::IsMouseDown(ImGuiMouseButton_Left); //    drag move / drag start ---------------------------
    bool            dragging_now    = ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f);
    ImVec2          w               = ImVec2( it.canvas.x / m_zoom, it.canvas.y / m_zoom );


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



    // ------------------------------------------------------------------ //
    // 1) Record a pending hit on mouse-press (unchanged)
    // ------------------------------------------------------------------ //
    if (it.hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        m_pending_hit   = _hit_any(it);                // may be nullopt
        m_pending_clear = !io.KeyShift && !io.KeyCtrl; // will clear unless additive
    }


    // ------------------------------------------------------------------ //
    // 2) Decide when a move-drag starts  (unchanged selection rules)
    // ------------------------------------------------------------------ //
    if (!m_dragging && lmb && !m_sel.empty() && !m_pending_hit)
    {
        // Case A: drag started inside an existing selection
        static ImVec2 press_world{};
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            press_world = w;

        bool inside_selected = false;

        int idx = _hit_point(it);
        if (idx >= 0 && m_sel.points.count(idx))
            inside_selected = true;

        // Case B: drag started on a non-selected object
        if (!inside_selected && m_sel.vertices.size() > 1)
        {
            ImVec2 tl, br;
            if (_selection_bounds(tl, br))
                inside_selected = (press_world.x >= tl.x && press_world.x <= br.x &&
                                   press_world.y >= tl.y && press_world.y <= br.y);
        }

        //  BEGIN DRAGGING...
        if (inside_selected && dragging_now)
        {
            // ----- begin move-drag ------------------------------------------------
            m_dragging = true;
            m_movedrag = {};                        // clear old
            m_movedrag.active    = true;
            m_movedrag.anchor_ws = _world_from_screen(it.canvas);

            m_movedrag.v_ids.reserve(m_sel.vertices.size());
            m_movedrag.v_orig.reserve(m_sel.vertices.size());

            for (uint32_t vid : m_sel.vertices)
            {
                if (const Pos* v = find_vertex(m_vertices, vid))
                {
                    m_movedrag.v_ids .push_back(vid);
                    m_movedrag.v_orig.push_back({v->x, v->y});
                }
            }
        }
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
        if (!lmb)
        {
            /*  snap the vertices we just moved  */
            if (m_dragging && m_grid.snap_on)
                for (uint32_t vid : m_sel.vertices)
                    if (Pos* v = find_vertex(m_vertices, vid))
                    {
                        ImVec2 snapped = _grid_snap({v->x, v->y});
                        v->x = snapped.x;
                        v->y = snapped.y;
                    }

            m_dragging = false;
        }
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


//  "_update_cursor_select"
//      pointer-cursor hint ───────────────────────────
//
void Editor::_update_cursor_select(const Interaction& it) const
{
    if (!it.hovered) return;               // cursor not over canvas

    // ignore while dragging a selection or bbox
    if (m_dragging || m_boxdrag.active) return;

    auto hit = _hit_any(it);               // point / path / line / none
    if (!hit) return;

    switch (hit->type)
    {
        case Hit::Type::Point:             // a vertex glyph
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            break;

        case Hit::Type::Path:              // open or closed path
        case Hit::Type::Line:
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
            break;

        default:
            break;
    }
}


//  "_rebuild_vertex_selection"
//
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
//      SELECTION HIGHLIGHT / APPEARANCE...
// *************************************************************************** //
// *************************************************************************** //

//  "_draw_selection_overlay"
//
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
    
    this->_draw_selected_handles(dl, origin);
    this->_draw_selection_bbox(dl, origin);
    
    return;
    
}






// *************************************************************************** //
//
//
//      SELECTION BOUNDING BOX...
// *************************************************************************** //
// *************************************************************************** //

//  "_selection_bounds"
//
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


//  "_draw_selection_bbox"
//
void Editor::_draw_selection_bbox(ImDrawList* dl, const ImVec2& origin) const
{
    ImVec2 tl, br;
    if (!_selection_bounds(tl, br)) { m_hover_handle = -1; return; }

    // world → screen
    ImVec2 p0{ origin.x + tl.x * m_zoom, origin.y + tl.y * m_zoom };
    ImVec2 p1{ origin.x + br.x * m_zoom, origin.y + br.y * m_zoom };

    // draw rectangle
    dl->AddRect(p0, p1, SELECTION_BBOX_COL, 0.0f,
                ImDrawFlags_None, SELECTION_BBOX_TH);

    // build 8 handle positions (world then screen)
    ImVec2 hw = { (tl.x + br.x) * 0.5f, (tl.y + br.y) * 0.5f };      // centre
    const ImVec2 ws[8] = {
        tl,
        { hw.x, tl.y },
        { br.x, tl.y },
        { br.x, hw.y },
        br,
        { hw.x, br.y },
        { tl.x, br.y },
        { tl.x, hw.y }
    };

    // detect hover & draw
    m_hover_handle = -1;
    for (int i = 0; i < 8; ++i)
    {
        ImVec2 s = { origin.x + ws[i].x * m_zoom,
                     origin.y + ws[i].y * m_zoom };
        ImVec2 min = { s.x - HANDLE_BOX_SIZE, s.y - HANDLE_BOX_SIZE };
        ImVec2 max = { s.x + HANDLE_BOX_SIZE, s.y + HANDLE_BOX_SIZE };

        bool hovered = ImGui::IsMouseHoveringRect(min, max);
        if (hovered) m_hover_handle = i;

        dl->AddRectFilled(min, max, hovered ? HANDLE_HOVER_COL
                                            : HANDLE_COL);
    }
}


//  "_draw_selected_handles"
//
void Editor::_draw_selected_handles(ImDrawList* dl, const ImVec2& origin) const
{
    for (size_t pi : m_sel.points)
    {
        if (pi >= m_points.size()) continue;
        const Vertex* v = find_vertex(m_vertices, m_points[pi].v);
        if (!v) continue;

        ImVec2 a{ origin.x + v->x * m_zoom,
                  origin.y + v->y * m_zoom };

        auto draw_handle = [&](const ImVec2& off){
            if (off.x == 0.f && off.y == 0.f) return;
            ImVec2 h{ origin.x + (v->x + off.x) * m_zoom,
                      origin.y + (v->y + off.y) * m_zoom };
            dl->AddLine(a, h, ms_HANDLE_COL, 1.0f);
            dl->AddRectFilled({ h.x - ms_HANDLE_SIZE, h.y - ms_HANDLE_SIZE },
                              { h.x + ms_HANDLE_SIZE, h.y + ms_HANDLE_SIZE },
                              ms_HANDLE_COL);
        };

        draw_handle(v->out_handle);
        draw_handle(v->in_handle);
    }
}










// *************************************************************************** //
//
//
//      BOUNDING BOX MECHANICS...
// *************************************************************************** //
// *************************************************************************** //

//  "_start_bbox_drag"
//
void Editor::_start_bbox_drag(uint8_t hidx, const ImVec2& tl, const ImVec2& br)
{
    // reset drag state first
    m_boxdrag = {};
    m_boxdrag.active        = true;
    m_boxdrag.handle_idx    = hidx;
    m_boxdrag.bbox_tl_ws    = tl;
    m_boxdrag.bbox_br_ws    = br;

    // pivot (opposite corner or mid-edge)
    ImVec2 hw{ (tl.x + br.x) * 0.5f, (tl.y + br.y) * 0.5f };
    const ImVec2 pivots[8] = {
        br,                 // 0 TL  -> pivot BR
        { hw.x, br.y },     // 1 T   -> B
        { tl.x, br.y },     // 2 TR  -> BL
        { tl.x, hw.y },     // 3 R   -> L
        tl,                 // 4 BR  -> TL
        { hw.x, tl.y },     // 5 B   -> T
        { br.x, tl.y },     // 6 BL  -> TR
        { br.x, hw.y }      // 7 L   -> R
    };
    m_boxdrag.anchor_ws = pivots[hidx];

    /* snapshot affected vertices */
    for (uint32_t vid : m_sel.vertices)
        if (const Pos* v = find_vertex(m_vertices, vid))
        {
            m_boxdrag.v_ids.push_back(vid);
            m_boxdrag.v_orig.push_back({ v->x, v->y });
        }

    /* store mouse position in world coords AFTER reset */
    ImVec2 m_scr = ImGui::GetIO().MousePos;
    m_boxdrag.mouse_ws0 = { (m_scr.x - m_origin_scr.x) / m_zoom,
                            (m_scr.y - m_origin_scr.y) / m_zoom };

    /* reset first-frame flag for update routine */
    m_boxdrag.first_frame = true;
}


//  "_update_bbox"
//
void Editor::_update_bbox()
{
    if (!m_boxdrag.active)
        return;

    /* if cursor hasn't moved since click, keep original scale */
    if (m_boxdrag.first_frame &&
        ImGui::GetIO().MouseDelta.x == 0.f &&
        ImGui::GetIO().MouseDelta.y == 0.f)
        return;

    m_boxdrag.first_frame = false;   // first frame handled

    /* stop drag */
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        m_boxdrag.first_frame = true;          // reset for next drag
        m_boxdrag = {};
        return;
    }

    /* current mouse in world coords */
    ImVec2 cur_ws{ (ImGui::GetIO().MousePos.x - m_origin_scr.x) / m_zoom,
                   (ImGui::GetIO().MousePos.y - m_origin_scr.y) / m_zoom };

    /* reference point for scaling */
    ImVec2 ref_ws = m_boxdrag.first_frame ? m_boxdrag.mouse_ws0 : cur_ws;

    ImVec2 h_ws{ ref_ws.x - m_boxdrag.anchor_ws.x,
                 ref_ws.y - m_boxdrag.anchor_ws.y };

    /* original vector from anchor to handle */
    ImVec2 h0_ws;
    switch (m_boxdrag.handle_idx)
    {
        case 0: h0_ws = { m_boxdrag.bbox_tl_ws.x - m_boxdrag.anchor_ws.x,
                          m_boxdrag.bbox_tl_ws.y - m_boxdrag.anchor_ws.y }; break; // TL
        case 1: h0_ws = { 0,
                          m_boxdrag.bbox_tl_ws.y - m_boxdrag.anchor_ws.y }; break; // T
        case 2: h0_ws = { m_boxdrag.bbox_br_ws.x - m_boxdrag.anchor_ws.x,
                          m_boxdrag.bbox_tl_ws.y - m_boxdrag.anchor_ws.y }; break; // TR
        case 3: h0_ws = { m_boxdrag.bbox_br_ws.x - m_boxdrag.anchor_ws.x,
                          0 }; break;                                              // R
        case 4: h0_ws = { m_boxdrag.bbox_br_ws.x - m_boxdrag.anchor_ws.x,
                          m_boxdrag.bbox_br_ws.y - m_boxdrag.anchor_ws.y }; break; // BR
        case 5: h0_ws = { 0,
                          m_boxdrag.bbox_br_ws.y - m_boxdrag.anchor_ws.y }; break; // B
        case 6: h0_ws = { m_boxdrag.bbox_tl_ws.x - m_boxdrag.anchor_ws.x,
                          m_boxdrag.bbox_br_ws.y - m_boxdrag.anchor_ws.y }; break; // BL
        case 7: h0_ws = { m_boxdrag.bbox_tl_ws.x - m_boxdrag.anchor_ws.x,
                          0 }; break;                                              // L
    }

    float sx = (h0_ws.x != 0.f) ? h_ws.x / h0_ws.x : 1.f;
    float sy = (h0_ws.y != 0.f) ? h_ws.y / h0_ws.y : 1.f;

    /* lock one axis for edge handles (1=T,3=R,5=B,7=L) */
    const bool handle_top    = (m_boxdrag.handle_idx == 1);
    const bool handle_right  = (m_boxdrag.handle_idx == 3);
    const bool handle_bottom = (m_boxdrag.handle_idx == 5);
    const bool handle_left   = (m_boxdrag.handle_idx == 7);

    if (handle_top || handle_bottom)  sx = 1.f;   // vertical edge → X scale locked
    if (handle_left || handle_right)  sy = 1.f;   // horizontal edge → Y scale locked

    /* Shift → uniform scale */
    if (ImGui::GetIO().KeyShift)
    {
        float s = std::max(std::fabs(sx), std::fabs(sy));
        sx = (sx < 0.f ? -s : s);
        sy = (sy < 0.f ? -s : s);
    }

    /* re‑assert locked axis after uniform adjustment */
    if (handle_top || handle_bottom)  sx = 1.f;
    if (handle_left || handle_right)  sy = 1.f;

    /* apply to vertices */
    for (size_t i = 0; i < m_boxdrag.v_ids.size(); ++i)
    {
        Pos* v = find_vertex(m_vertices, m_boxdrag.v_ids[i]);
        const ImVec2& o = m_boxdrag.v_orig[i];
        if (v)
        {
            ImVec2 d{ o.x - m_boxdrag.anchor_ws.x,
                      o.y - m_boxdrag.anchor_ws.y };
            v->x = m_boxdrag.anchor_ws.x + d.x * sx;
            v->y = m_boxdrag.anchor_ws.y + d.y * sy;
        }
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

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
{
    const ImVec2 ms = ImGui::GetIO().MousePos;          // mouse in px

    for (size_t i = 0; i < m_points.size(); ++i)
    {
        const Vertex* v = find_vertex(m_vertices, m_points[i].v);
        if (!v) continue;

        ImVec2 scr = world_to_pixels({ v->x, v->y });   // NEW
        float dx = scr.x - ms.x;
        float dy = scr.y - ms.y;
        if (dx*dx + dy*dy <= HIT_THRESH_SQ)
            return static_cast<int>(i);
    }
    return -1;
}
}


//  "_hit_any"
//
std::optional<Editor::Hit> Editor::_hit_any(const Interaction& it) const
{
    // mouse position in screen‑pixel coordinates
    const ImVec2 ms = ImGui::GetIO().MousePos;

    // handy lambda: convert world‑space ⇒ screen‑pixel
    auto ws2px = [&](const ImVec2& w) { return world_to_pixels(w); };

    // ───────────────────────────────────────────────────────────── 1. handles
    constexpr float PAD  = 4.0f;                       // pick‑box grow
    const float     half = HANDLE_BOX_SIZE * 0.5f + PAD;

    auto overlap = [&](ImVec2 s)
    {
        return fabsf(ms.x - s.x) <= half &&
               fabsf(ms.y - s.y) <= half;
    };

    for (const Vertex& v : m_vertices)
    {
        if (!m_sel.vertices.count(v.id)) continue;     // show handles only on selected

        ImVec2 scr_anchor = ws2px({ v.x, v.y });

        if (v.out_handle.x || v.out_handle.y) {
            ImVec2 scr = ws2px({ v.x + v.out_handle.x,
                                 v.y + v.out_handle.y });
            if (overlap(scr))
                return Hit{ Hit::Type::Handle,
                            static_cast<size_t>(v.id), true };
        }
        if (v.in_handle.x || v.in_handle.y) {
            ImVec2 scr = ws2px({ v.x + v.in_handle.x,
                                 v.y + v.in_handle.y });
            if (overlap(scr))
                return Hit{ Hit::Type::Handle,
                            static_cast<size_t>(v.id), false };
        }
    }

    // ───────────────────────────────────────────────────────────── 2. point glyphs
    int pi = _hit_point(it);                    // (_hit_point will be refactored next)
    if (pi >= 0 && static_cast<size_t>(pi) < m_points.size())
        return Hit{ Hit::Type::Point, static_cast<size_t>(pi) };

    // ───────────────────────────────────────────────────────────── 3. standalone lines
    for (size_t i = 0; i < m_lines.size(); ++i)
    {
        const Vertex* a = find_vertex(m_vertices, m_lines[i].a);
        const Vertex* b = find_vertex(m_vertices, m_lines[i].b);
        if (!a || !b) continue;

        ImVec2 A = ws2px({ a->x, a->y });
        ImVec2 B = ws2px({ b->x, b->y });

        ImVec2 AB{ B.x - A.x, B.y - A.y };
        ImVec2 AP{ ms.x - A.x, ms.y - A.y };
        float  len_sq = AB.x*AB.x + AB.y*AB.y;
        float  t = (len_sq > 0.f) ? (AP.x*AB.x + AP.y*AB.y) / len_sq : 0.f;
        t = std::clamp(t, 0.f, 1.f);

        ImVec2 C{ A.x + AB.x*t, A.y + AB.y*t };
        float  dx = ms.x - C.x, dy = ms.y - C.y;
        if (dx*dx + dy*dy <= HIT_THRESH_SQ)
            return Hit{ Hit::Type::Line, i };
    }

    // ───────────────────────────────────────────────────────────── 4. paths
    for (size_t pi_path = 0; pi_path < m_paths.size(); ++pi_path)
    {
        const Path& p = m_paths[pi_path];
        size_t N = p.verts.size();
        if (N < 2) continue;

        // edge hit‑test
        for (size_t si = 0; si < N - 1 + (p.closed ? 1u : 0u); ++si)
        {
            const Vertex* a = find_vertex(m_vertices, p.verts[si]);
            const Vertex* b = find_vertex(m_vertices, p.verts[(si + 1) % N]);
            if (!a || !b) continue;

            if (!is_curved<VertexID>(a, b))
            {
                ImVec2 A = ws2px({ a->x, a->y });
                ImVec2 B = ws2px({ b->x, b->y });
                ImVec2 AB{ B.x - A.x, B.y - A.y };
                ImVec2 AP{ ms.x - A.x, ms.y - A.y };
                float len_sq = AB.x*AB.x + AB.y*AB.y;
                float t = (len_sq > 0.f) ? (AP.x*AB.x + AP.y*AB.y) / len_sq : 0.f;
                t = std::clamp(t, 0.f, 1.f);

                ImVec2 C{ A.x + AB.x*t, A.y + AB.y*t };
                float  dx = ms.x - C.x, dy = ms.y - C.y;
                if (dx*dx + dy*dy <= HIT_THRESH_SQ)
                    return Hit{ Hit::Type::Path, pi_path };
            }
            else
            {
                ImVec2 prev_ws{ a->x, a->y };
                ImVec2 prev_px = ws2px(prev_ws);

                for (int k = 1; k <= ms_BEZIER_HIT_STEPS; ++k)
                {
                    float  t  = static_cast<float>(k) / ms_BEZIER_HIT_STEPS;
                    ImVec2  cur_ws = cubic_eval<VertexID>(a, b, t);
                    ImVec2  cur_px = ws2px(cur_ws);

                    ImVec2 seg{ cur_px.x - prev_px.x, cur_px.y - prev_px.y };
                    ImVec2 to_pt{ ms.x - prev_px.x,  ms.y - prev_px.y };
                    float  len_sq = seg.x*seg.x + seg.y*seg.y;
                    float  u = (len_sq > 0.f) ? (to_pt.x*seg.x + to_pt.y*seg.y) / len_sq : 0.f;
                    u = std::clamp(u, 0.f, 1.f);

                    ImVec2 C{ prev_px.x + seg.x*u, prev_px.y + seg.y*u };
                    float  dx = ms.x - C.x, dy = ms.y - C.y;
                    if (dx*dx + dy*dy <= HIT_THRESH_SQ)
                        return Hit{ Hit::Type::Path, pi_path };

                    prev_px = cur_px;
                }
            }
        }

        // interior point‑in‑polygon (use pixel coords)
        if (p.closed)
        {
            std::vector<ImVec2> poly;
            poly.reserve(N * 4);

            for (size_t vi = 0; vi < N; ++vi)
            {
                const Vertex* a = find_vertex(m_vertices, p.verts[vi]);
                const Vertex* b = find_vertex(m_vertices, p.verts[(vi + 1) % N]);
                if (!a || !b) continue;

                if (!is_curved<VertexID>(a, b))
                {
                    poly.push_back(ws2px({ a->x, a->y }));
                }
                else
                {
                    for (int k = 0; k <= ms_BEZIER_HIT_STEPS; ++k)
                    {
                        float  t   = static_cast<float>(k) / ms_BEZIER_HIT_STEPS;
                        ImVec2 wpt = cubic_eval<VertexID>(a, b, t);
                        poly.push_back(ws2px(wpt));
                    }
                }
            }
            if (!poly.empty() && point_in_polygon(poly, ms))
                return Hit{ Hit::Type::Path, pi_path };
        }
    }

    return std::nullopt;    // nothing hit
}


//  "_hit_path_segment"
//
//      Segment-precision hit-test (straight + Bézier).
//      Returns nearest segment within a 6-px pick radius (early-out on miss).
//
std::optional<Editor::PathHit> Editor::_hit_path_segment(const Interaction & /*it*/) const
{
    constexpr float PICK_PX   = 6.0f;
    const float     thresh_sq = PICK_PX * PICK_PX;

    // mouse position in pixel space
    ImVec2 ms = ImGui::GetIO().MousePos;

    std::optional<PathHit> best;
    float best_d2 = thresh_sq;

    auto ws2px = [this](ImVec2 w){ return world_to_pixels(w); };
    auto lerp   = [](float a, float b, float t){ return a + (b - a) * t; };
    auto is_zero = [](const ImVec2& v){ return v.x == 0.f && v.y == 0.f; };

    for (size_t pi = 0; pi < m_paths.size(); ++pi)
    {
        const Path&  p = m_paths[pi];
        const size_t N = p.verts.size();
        if (N < 2) continue;

        const bool   closed     = p.closed;
        const size_t seg_count  = N - (closed ? 0 : 1);

        for (size_t si = 0; si < seg_count; ++si)
        {
            const Vertex* a = find_vertex(m_vertices, p.verts[si]);
            const Vertex* b = find_vertex(m_vertices, p.verts[(si + 1) % N]);
            if (!a || !b) continue;

            bool curved = !is_zero(a->out_handle) || !is_zero(b->in_handle);

            if (!curved)
            {
                // ---- straight line in pixel space ----
                ImVec2 A = ws2px({ a->x, a->y });
                ImVec2 B = ws2px({ b->x, b->y });

                ImVec2 AB{ B.x - A.x, B.y - A.y };
                ImVec2 AM{ ms.x - A.x, ms.y - A.y };

                float len_sq = AB.x*AB.x + AB.y*AB.y;
                if (len_sq == 0.f) continue;

                float t = std::clamp((AM.x*AB.x + AM.y*AB.y) / len_sq, 0.f, 1.f);
                ImVec2 C{ A.x + AB.x*t, A.y + AB.y*t };

                float dx = ms.x - C.x, dy = ms.y - C.y;
                float d2 = dx*dx + dy*dy;

                if (d2 < best_d2) {
                    best_d2 = d2;

                    float cx_ws = lerp(a->x, b->x, t);
                    float cy_ws = lerp(a->y, b->y, t);

                    best = PathHit{ pi, si, t, ImVec2(cx_ws, cy_ws) };
                }
            }
            else
            {
                // ---- cubic Bézier: sample STEPS sub‑segments ----
                const int STEPS = ms_BEZIER_HIT_STEPS;
                ImVec2 prev_px = ws2px({ a->x, a->y });
                ImVec2 prev_ws{ a->x, a->y };

                for (int k = 1; k <= STEPS; ++k)
                {
                    float  t  = static_cast<float>(k) / STEPS;
                    ImVec2 cur_ws = cubic_eval<VertexID>(a, b, t);
                    ImVec2 cur_px = ws2px(cur_ws);

                    // distance mouse → segment [prev,cur]
                    ImVec2 AB{ cur_px.x - prev_px.x, cur_px.y - prev_px.y };
                    ImVec2 AM{ ms.x - prev_px.x,     ms.y - prev_px.y     };

                    float len_sq = AB.x*AB.x + AB.y*AB.y;
                    float u = (len_sq > 0.f) ? (AM.x*AB.x + AM.y*AB.y) / len_sq : 0.f;
                    u = std::clamp(u, 0.f, 1.f);

                    ImVec2 C{ prev_px.x + AB.x*u, prev_px.y + AB.y*u };
                    float dx = ms.x - C.x, dy = ms.y - C.y;
                    float d2 = dx*dx + dy*dy;

                    if (d2 < best_d2) {
                        best_d2 = d2;

                        ImVec2 pos_ws{
                            prev_ws.x + (cur_ws.x - prev_ws.x) * u,
                            prev_ws.y + (cur_ws.y - prev_ws.y) * u
                        };
                        best = PathHit{ pi, si, t, pos_ws };
                    }
                    prev_px = cur_px;
                    prev_ws = cur_ws;
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
    update_move_drag_state(it);             // helpers (2-4 merged)
    resolve_pending_selection(it);          // helper (5)
    show_selection_context_menu(it);        // helper (6)
}

//////////////////////////////////////////////////////////////
//      HELPERS FOR "PROCESS_SELECTION" IMPLEMENTATION      //
//////////////////////////////////////////////////////////////

//  "add_hit_to_selection"
//      selection helper: adds a Hit to m_sel (ignores handles)
//
void Editor::add_hit_to_selection(const Hit & hit)
{
    if (hit.type == Hit::Type::Handle) return;

    if (hit.type == Hit::Type::Point) {
        size_t    idx  = hit.index;
        uint32_t  vid  = m_points[idx].v;
        m_sel.points.insert(idx);
        m_sel.vertices.insert(vid);
    }
    else if (hit.type == Hit::Type::Path) {
        size_t idx = hit.index;
        const Path & p = m_paths[idx];
        m_sel.paths.insert(idx);
        for (uint32_t vid : p.verts)
            m_sel.vertices.insert(vid);
    }
    else { // Line
        size_t    idx = hit.index;
        uint32_t  va  = m_lines[idx].a,
                  vb  = m_lines[idx].b;
        m_sel.lines.insert(idx);
        m_sel.vertices.insert(va);
        m_sel.vertices.insert(vb);
    }
}


//  "start_move_drag"
//
void Editor::start_move_drag(const ImVec2 & press_ws)
{
    m_dragging           = true;
    m_movedrag           = {};
    m_movedrag.active    = true;
    m_movedrag.press_ws  = press_ws;            // remember grab point

    // compute rigid-snap reference = bbox top-left
    ImVec2 tl, br;
    if (_selection_bounds(tl, br))
        m_movedrag.anchor_ws = tl;

    m_movedrag.v_ids .reserve(m_sel.vertices.size());
    m_movedrag.v_orig.reserve(m_sel.vertices.size());

    for (uint32_t vid : m_sel.vertices)
        if (const Vertex * v = find_vertex(m_vertices, vid)) {
            m_movedrag.v_ids .push_back(vid);
            m_movedrag.v_orig.push_back({v->x, v->y});
        }
}


//  "update_move_drag_state"
//      Press / drag / release handling ---------------------------------------
//
void Editor::update_move_drag_state(const Interaction & it)
{
    ImGuiIO& io           = ImGui::GetIO();
    const bool lmb        = io.MouseDown[ImGuiMouseButton_Left];
    const bool lmb_click  = ImGui::IsMouseClicked  (ImGuiMouseButton_Left);
    const bool lmb_release= ImGui::IsMouseReleased (ImGuiMouseButton_Left);
    const bool dragging_now =
    ImGui::IsMouseDragPastThreshold(ImGuiMouseButton_Left, 0.0f);

    ImVec2 w_mouse = pixels_to_world(io.MousePos);      // NEW

    // remember exact press-position once ----------------------------------
    static ImVec2 press_ws{};
    if (lmb_click) press_ws = pixels_to_world(io.MousePos);   // NEW

    // --------------------------------------------------------------------
    // 0. NEW: start drag immediately on a fresh hit
    // --------------------------------------------------------------------
    if (!m_dragging && lmb_click && it.hovered)
    {
        m_pending_hit = _hit_any(it);               // fresh pick

        if (m_pending_hit)                          // hit something
        {
            // clear selection unless additive
            if (!io.KeyCtrl && !io.KeyShift) m_sel.clear();
            add_hit_to_selection(*m_pending_hit);   // select the hit
            m_pending_hit.reset();
            m_pending_clear = false;

            start_move_drag(press_ws);              // begin move-drag now
            return;                                // skip lasso test this frame
        }
        // if no hit, normal lasso logic continues below
    }

    // --------------------------------------------------------------------
    // 1. drag inside an already-selected region (threshold kept)
    // --------------------------------------------------------------------
    if (!m_dragging && lmb && !m_sel.empty() && !m_pending_hit)
    {
        bool inside_sel = false;

        int idx = _hit_point(it);
        if (idx >= 0 && m_sel.points.count(idx))
            inside_sel = true;
        else if (m_sel.vertices.size() > 1)
        {
            ImVec2 tl, br;
            if (_selection_bounds(tl, br))
                inside_sel = (press_ws.x >= tl.x && press_ws.x <= br.x &&
                               press_ws.y >= tl.y && press_ws.y <= br.y);
        }

        if (inside_sel && dragging_now)
            start_move_drag(press_ws);
    }

    // --------------------------------------------------------------------
    // 2. per-frame rigid-snap translation (unchanged)
    // --------------------------------------------------------------------
    if (m_dragging)
    {
        ImVec2 delta{
            w_mouse.x - m_movedrag.press_ws.x,
            w_mouse.y - m_movedrag.press_ws.y
        };

        if (want_snap()) {
            ImVec2 snapped  = snap_to_grid({ m_movedrag.anchor_ws.x + delta.x,    m_movedrag.anchor_ws.y + delta.y });
            delta.x         = snapped.x - m_movedrag.anchor_ws.x;
            delta.y         = snapped.y - m_movedrag.anchor_ws.y;
        }

        for (size_t i = 0; i < m_movedrag.v_ids.size(); ++i)
            if (Vertex* v = find_vertex_mut(m_vertices, m_movedrag.v_ids[i]))
            {
                const ImVec2& orig = m_movedrag.v_orig[i];
                v->x = orig.x + delta.x;
                v->y = orig.y + delta.y;
            }

        if (lmb_release) m_dragging = false;
    }

    // ------------------------------------------------------------------
    // 4. per-frame translation while dragging  (rigid snap)
    // ------------------------------------------------------------------
    if (m_dragging)
    {
        // raw translation from initial press
        ImVec2 delta{
            w_mouse.x - m_movedrag.press_ws.x,
            w_mouse.y - m_movedrag.press_ws.y
        };

        // snap the delta **once**, via the bbox anchor
        if ( this->want_snap() )
        {
            ImVec2 snapped_anchor   = snap_to_grid({ m_movedrag.anchor_ws.x + delta.x,    m_movedrag.anchor_ws.y + delta.y });

            delta.x                 = snapped_anchor.x - m_movedrag.anchor_ws.x;
            delta.y                 = snapped_anchor.y - m_movedrag.anchor_ws.y;
        }

        // apply the same delta to every vertex
        for (size_t i = 0; i < m_movedrag.v_ids.size(); ++i)
            if (Vertex * v = find_vertex_mut(m_vertices, m_movedrag.v_ids[i])) {
                const ImVec2 & orig = m_movedrag.v_orig[i];
                v->x = orig.x + delta.x;
                v->y = orig.y + delta.y;
            }

        if (lmb_release)               // drag finished
            m_dragging = false;
    }
    return;
}


//  "resolve_pending_selection"
//
void Editor::resolve_pending_selection(const Interaction & it)
{
    if (!it.hovered || m_dragging) return;

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        if (m_pending_hit || !m_pending_clear)
        {
            if (m_pending_clear) m_sel.clear();

            if (m_pending_hit)
            {
                const Hit & hit = *m_pending_hit;

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
                    const Path & p = m_paths[idx];
                    if (!m_sel.paths.erase(idx)) {
                        m_sel.paths.insert(idx);
                        for (uint32_t vid : p.verts) m_sel.vertices.insert(vid);
                    } else {
                        for (uint32_t vid : p.verts) m_sel.vertices.erase(vid);
                    }
                }
                else if (hit.type == Hit::Type::Line)
                {
                    size_t idx = hit.index;
                    uint32_t va = m_lines[idx].a, vb = m_lines[idx].b;
                    if (!m_sel.lines.erase(idx)) {
                        m_sel.lines.insert(idx); m_sel.vertices.insert(va); m_sel.vertices.insert(vb);
                    } else {
                        m_sel.vertices.erase(va); m_sel.vertices.erase(vb);
                    }
                }
            }
        }
        m_pending_hit.reset();
        m_pending_clear = false;
    }
}


//  "show_selection_context_menu"
//
void Editor::show_selection_context_menu(const Interaction & it)
{
    // Only when cursor is over the canvas and something is selected
    if (!it.hovered || m_sel.empty())
        return;

    // Open & render popup on RMB anywhere in the plot window
    if (ImGui::BeginPopupContextWindow("SelectionCtx",
                                       ImGuiPopupFlags_MouseButtonRight))
    {
        if (ImGui::MenuItem("Delete"))
        {
            // Gather indices then erase back-to-front
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



// *************************************************************************** //
//
//
//      SELECTION IMPLEMENTATIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_update_cursor_select"
//      pointer-cursor hint ───────────────────────────
//
void Editor::_update_cursor_select(const Interaction& it) const
{
    if (!it.hovered) return;                        // cursor not over canvas
    if (m_dragging || m_boxdrag.active) return;     // ignore while dragging
    // optional: also skip while a handle is already being dragged
    // if (m_dragging_handle) return;

    auto hit = _hit_any(it);                        // point / path / line / handle / none
    if (!hit) return;

    switch (hit->type)
    {
        case Hit::Type::Handle:                     // NEW — Bézier handle square
        case Hit::Type::Point:                      // vertex glyph
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            break;

        case Hit::Type::Path:                       // open or closed path
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

// -----------------------------------------------------------------------------
//  "_draw_selection_highlight"
//      Draw outlines for selected primitives plus bbox/handles.
// -----------------------------------------------------------------------------
void Editor::_draw_selection_highlight(ImDrawList * dl) const
{
    const ImU32 col = COL_SELECTION_OUT;

    auto ws2px = [this](ImVec2 w){ return world_to_pixels(w); };

    // ───── Highlight selected points
    for (size_t idx : m_sel.points)
    {
        if (idx >= m_points.size()) continue;
        const Point& pt = m_points[idx];
        if (const Vertex* v = find_vertex(m_vertices, pt.v))
        {
            ImVec2 scr = ws2px({ v->x, v->y });
            dl->AddCircle(scr,
                          pt.sty.radius + 2.f,        // small outset
                          col, 0, 2.f);               // thickness 2 px
        }
    }

    // ───── Highlight selected standalone lines
    for (size_t idx : m_sel.lines)
    {
        if (idx >= m_lines.size()) continue;
        const Line& ln = m_lines[idx];
        const Vertex* a = find_vertex(m_vertices, ln.a);
        const Vertex* b = find_vertex(m_vertices, ln.b);
        if (a && b)
            dl->AddLine(ws2px({ a->x, a->y }),
                        ws2px({ b->x, b->y }),
                        col, ln.thickness + 2.f);
    }

    // ───── Highlight selected paths
    for (size_t idx : m_sel.paths)
    {
        if (idx >= m_paths.size()) continue;
        const Path& p = m_paths[idx];
        const size_t N = p.verts.size();
        if (N < 2) continue;

        auto draw_seg = [&](const Vertex* a, const Vertex* b){
            const float w = p.style.stroke_width + 2.0f;
            if ( is_curved<VertexID>(a,b) )
            {
                ImVec2 P0 = ws2px({ a->x,                         a->y });
                ImVec2 P1 = ws2px({ a->x + a->out_handle.x,       a->y + a->out_handle.y });
                ImVec2 P2 = ws2px({ b->x + b->in_handle.x,        b->y + b->in_handle.y  });
                ImVec2 P3 = ws2px({ b->x,                         b->y });
                dl->AddBezierCubic(P0, P1, P2, P3, col, w, ms_BEZIER_SEGMENTS);
            }
            else
            {
                dl->AddLine(ws2px({ a->x, a->y }),
                            ws2px({ b->x, b->y }),
                            col, w);
            }
        };

        for (size_t i = 0; i < N - 1; ++i)
            if (const Vertex* a = find_vertex(m_vertices, p.verts[i]))
                if (const Vertex* b = find_vertex(m_vertices, p.verts[i+1]))
                    draw_seg(a, b);

        if (p.closed)
            if (const Vertex* a = find_vertex(m_vertices, p.verts.back()))
                if (const Vertex* b = find_vertex(m_vertices, p.verts.front()))
                    draw_seg(a, b);
    }

    _draw_selected_handles(dl);
    _draw_selection_bbox(dl);
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
        if (const Vertex* v = find_vertex(m_vertices, vid))
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


// -----------------------------------------------------------------------------
// "_draw_selection_bbox"
// -----------------------------------------------------------------------------
void Editor::_draw_selection_bbox(ImDrawList* dl) const
{
    const bool has_paths_or_lines = !m_sel.paths.empty() || !m_sel.lines.empty();
    const bool single_vertex_only = (m_sel.vertices.size() <= 1) && !has_paths_or_lines;
    if (single_vertex_only) return;

    ImVec2 tl, br;
    if (!_selection_bounds(tl, br)) { m_hover_handle = -1; return; }

    auto ws2px = [this](ImVec2 w){ return world_to_pixels(w); };

    ImVec2 p0 = ws2px(tl);
    ImVec2 p1 = ws2px(br);

    dl->AddRect(p0, p1, SELECTION_BBOX_COL, 0.0f,
                ImDrawFlags_None, SELECTION_BBOX_TH);

    // handle positions
    ImVec2 hw{ (tl.x + br.x) * 0.5f, (tl.y + br.y) * 0.5f };
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

    m_hover_handle = -1;
    for (int i = 0; i < 8; ++i)
    {
        ImVec2 s = ws2px(ws[i]);
        ImVec2 min{ s.x - HANDLE_BOX_SIZE, s.y - HANDLE_BOX_SIZE };
        ImVec2 max{ s.x + HANDLE_BOX_SIZE, s.y + HANDLE_BOX_SIZE };

        bool hovered = ImGui::IsMouseHoveringRect(min, max);
        if (hovered) m_hover_handle = i;

        dl->AddRectFilled(min, max, hovered ? ms_HANDLE_HOVER_COLOR
                                            : ms_HANDLE_COLOR);
    }
}


// -----------------------------------------------------------------------------
// "_draw_selected_handles"
// -----------------------------------------------------------------------------
void Editor::_draw_selected_handles(ImDrawList* dl) const
{
    auto ws2px = [this](ImVec2 w){ return world_to_pixels(w); };

    for (size_t pi : m_sel.points)
    {
        if (pi >= m_points.size()) continue;
        const Vertex* v = find_vertex(m_vertices, m_points[pi].v);
        if (!v) continue;

        ImVec2 a = ws2px({ v->x, v->y });

        auto draw_handle = [&](const ImVec2& off){
            if (off.x == 0.f && off.y == 0.f) return;
            ImVec2 h = ws2px({ v->x + off.x, v->y + off.y });
            dl->AddLine(a, h, ms_HANDLE_COLOR, 1.0f);
            dl->AddRectFilled({ h.x - ms_HANDLE_SIZE, h.y - ms_HANDLE_SIZE },
                              { h.x + ms_HANDLE_SIZE, h.y + ms_HANDLE_SIZE },
                              ms_HANDLE_COLOR);
        };

        draw_handle(v->out_handle);
        draw_handle(v->in_handle);
    }
}






// *************************************************************************** //
//
//
//      LASSO TOOL...
// *************************************************************************** //
// *************************************************************************** //

//  "_start_lasso_tool"
//
//  LASSO START
//      Only begins if the user clicks on empty canvas (no object hit).
//      Unless Shift or Ctrl is held, any prior selection is cleared.
void Editor::_start_lasso_tool(void)
{
    ImGuiIO &       io      = ImGui::GetIO();
    
    m_lasso_active          = true;
    m_lasso_start           = io.MousePos;                      // in screen coords
    m_lasso_end             = m_lasso_start;
    
    if ( !io.KeyShift && !io.KeyCtrl )
        m_sel.clear();  // fresh selection

    return;
}



//  "_update_lasso"
//
//  LASSO UPDATE
//      While the mouse button is held, draw the translucent rectangle.
//      When the button is released, convert the rect to world space
//      and add all intersecting objects to the selection set.
void Editor::_update_lasso(const Interaction & it)
{
    ImGuiIO &       io      = ImGui::GetIO();
    m_lasso_end             = io.MousePos;



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

        // Convert to world space via ImPlot helper
        ImVec2 tl_w = pixels_to_world(tl_scr);     // NEW
        ImVec2 br_w = pixels_to_world(br_scr);     // NEW
        // Normalise rectangle so tl_w is min corner, br_w is max corner
        if (tl_w.x > br_w.x) std::swap(tl_w.x, br_w.x);
        if (tl_w.y > br_w.y) std::swap(tl_w.y, br_w.y);

        bool additive = io.KeyShift || io.KeyCtrl;
        if (!additive)                       // fresh selection if no modifier
            m_sel.clear();

        /* ---------- Points ---------- */
        for (size_t i = 0; i < m_points.size(); ++i)
        {
            const Vertex * v = find_vertex(m_vertices, m_points[i].v);
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
            const Vertex* a = find_vertex(m_vertices, m_lines[i].a);
            const Vertex* b = find_vertex(m_vertices, m_lines[i].b);
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
                const Vertex* a = find_vertex(m_vertices, p.verts[si]);
                const Vertex* b = find_vertex(m_vertices, p.verts[(si+1)%N]);
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
    
    
    return;
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
        if (const Vertex* v = find_vertex(m_vertices, vid))
        {
            m_boxdrag.v_ids.push_back(vid);
            m_boxdrag.v_orig.push_back({ v->x, v->y });
        }

    /* store mouse position in world coords AFTER reset */
    m_boxdrag.mouse_ws0 = pixels_to_world(ImGui::GetIO().MousePos);   // NEW

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
    ImVec2 cur_ws = pixels_to_world(ImGui::GetIO().MousePos);   // NEW

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
        Vertex* v = find_vertex(m_vertices, m_boxdrag.v_ids[i]);
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

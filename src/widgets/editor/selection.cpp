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
    }
    
    return std::nullopt;
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


//  "_update_cursor_select"
//      pointer-cursor hint ───────────────────────────
//
void Editor::_update_cursor_select(const Interaction& it) const
{
    if (!it.hovered) return;                    // outside canvas
    if (_hit_any(it))                           // anything under mouse?
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand); // ImGui’s “link” cursor
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
    m_boxdrag = {};                       // reset
    m_boxdrag.active     = true;
    m_boxdrag.handle_idx = hidx;
    m_boxdrag.bbox_tl_ws = tl;
    m_boxdrag.bbox_br_ws = br;

    // pivot (opposite corner or mid)
    ImVec2 hw{ (tl.x + br.x)*0.5f, (tl.y + br.y)*0.5f };
    const ImVec2 pivots[8] = {
        br,                // 0 TL → pivot BR
        { hw.x, br.y },    // 1 top-mid → bottom-mid
        { tl.x, br.y },    // 2 TR → BL
        { tl.x, hw.y },    // 3 right-mid → left-mid
        tl,                // 4 BR → TL
        { hw.x, tl.y },    // 5 bottom-mid → top-mid
        { br.x, tl.y },    // 6 BL → TR
        { br.x, hw.y }     // 7 left-mid → right-mid
    };
    m_boxdrag.anchor_ws = pivots[hidx];

    // snapshot affected vertices (all selected verts)
    for (uint32_t vid : m_sel.vertices)
    {
        if (const Pos* v = find_vertex(m_vertices, vid))
        {
            m_boxdrag.v_ids.push_back(vid);
            m_boxdrag.v_orig.push_back({ v->x, v->y });
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

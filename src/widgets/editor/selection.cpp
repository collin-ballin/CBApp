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
int Editor::_hit_point([[maybe_unused]] const Interaction & it) const
{
    const ImVec2 ms = ImGui::GetIO().MousePos;          // mouse in px

    // helper to find owning path
    auto parent_path_of_vertex = [&](VertexID vid)->const Path*
    {
        for (const Path& p : m_paths)
            for (VertexID v : p.verts)
                if (v == vid) return &p;
        return nullptr;
    };

    for (size_t i = 0; i < m_points.size(); ++i)
    {
        const Vertex* v = find_vertex(m_vertices, m_points[i].v);
        if (!v) continue;

        const Path* pp = parent_path_of_vertex(v->id);
        if (!pp || pp->locked || !pp->visible) continue;   // NEW guard

        ImVec2 scr = world_to_pixels({ v->x, v->y });
        float  dx  = scr.x - ms.x;
        float  dy  = scr.y - ms.y;
        if (dx*dx + dy*dy <= m_style.HIT_THRESH_SQ)
            return static_cast<int>(i);
    }
    return -1;
}
/*{
    {
        const ImVec2    ms  = ImGui::GetIO().MousePos;          // mouse in px

        for (size_t i = 0; i < m_points.size(); ++i)
        {
            const Vertex *      v       = find_vertex(m_vertices, m_points[i].v);
            if (!v) continue;

            ImVec2              scr     = world_to_pixels({ v->x, v->y });   // NEW
            float               dx      = scr.x - ms.x;
            float               dy      = scr.y - ms.y;
            if ( dx*dx + dy*dy <= m_style.HIT_THRESH_SQ )
                return static_cast<int>(i);
        }
        return -1;
    }
}*/


//  "_hit_any"
//
std::optional<Editor::Hit> Editor::_hit_any(const Interaction & it) const
{
    // ─── utilities ───────────────────────────────────────────────────────
    constexpr float         PAD             = 4.0f;
    const float             HALF            = m_style.HANDLE_BOX_SIZE * 0.5f + PAD;
    const ImVec2            ms              = ImGui::GetIO().MousePos;          // mouse in pixels
    auto                    ws2px           = [&](const ImVec2& w){ return world_to_pixels(w); };



    //  "parent_path_of_vertex"
    //      Helper: map vertex-id → parent Path*, or nullptr if none.
    auto            parent_path_of_vertex   = [&](VertexID vid) -> const Path *
    {
        for (const Path & p : m_paths)
        {
            for (VertexID v : p.verts) {
                if (v == vid) { return &p; }
            }
        }
        return nullptr;
    };

    //  "overlap"
    auto            overlap     = [&](ImVec2 scr)
    { return fabsf(ms.x - scr.x) <= HALF && fabsf(ms.y - scr.y) <= HALF; };



    // ───────────────────────────────────────────── 1. Bézier handles
    for (const Vertex & v : m_vertices)
    {
        if (!m_sel.vertices.count(v.id))            { continue; }  // handles only for selected verts
        const Path* pp = parent_path_of_vertex(v.id);
        
        if ( !pp || pp->locked || !pp->visible )    { continue; }


        if (v.out_handle.x || v.out_handle.y)
        {
            ImVec2 scr = ws2px({ v.x + v.out_handle.x,
                                 v.y + v.out_handle.y });
            if ( overlap(scr) )
                return Hit{ Hit::Type::Handle,
                            static_cast<size_t>(v.id), true };
        }
        if ( v.in_handle.x || v.in_handle.y )
        {
            ImVec2 scr = ws2px({ v.x + v.in_handle.x,
                                 v.y + v.in_handle.y });
            if ( overlap(scr) )
            {
                return Hit{ Hit::Type::Handle,
                            static_cast<size_t>(v.id), false };
            }
        }
    }


    // ───────────────────────────────────────────── 2. point glyphs
    int pi = _hit_point(it);
    if (pi >= 0 && static_cast<size_t>(pi) < m_points.size())
    {
        uint32_t vid = m_points[pi].v;
        const Path* pp = parent_path_of_vertex(vid);
        if (pp && !pp->locked && pp->visible)
            return Hit{ Hit::Type::Point, static_cast<size_t>(pi) };
    }


    // ───────────────────────────────────────────── 3. standalone lines (unchanged)
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
        if (dx*dx + dy*dy <= m_style.HIT_THRESH_SQ)
            return Hit{ Hit::Type::Line, i };
    }

    // ───────────────────────────────────────────── 4. paths
    // Build vector of unlocked+visible paths, sort by z (low→high), iterate reverse
    std::vector<const Path*> vec;
    vec.reserve(m_paths.size());
    for (const Path& p : m_paths)
        if (!p.locked && p.visible) vec.push_back(&p);

    std::stable_sort(vec.begin(), vec.end(),
        [](const Path* a, const Path* b){ return a->z_index < b->z_index; });

    for (auto rit = vec.rbegin(); rit != vec.rend(); ++rit)
    {
        const Path &        p           = **rit;
        const size_t        N           = p.verts.size();
        const size_t        index       = &p - m_paths.data();          // back to array idx
        if (N < 2)          { continue; }

        // edge hit-test  (body identical to previous version)
        for ( size_t si = 0; si < N - 1 + (p.closed ? 1u : 0u); ++si )
        {
            const Vertex *      a       = find_vertex(m_vertices, p.verts[si]);
            const Vertex *      b       = find_vertex(m_vertices, p.verts[(si + 1) % N]);
            if ( !a || !b )     { continue; }


            if ( !is_curved<VertexID>(a, b) )
            {
                ImVec2      A           = ws2px({ a->x, a->y });
                ImVec2      B           = ws2px({ b->x, b->y });
                ImVec2      AB          { B.x - A.x, B.y - A.y };
                ImVec2      AP          { ms.x - A.x, ms.y - A.y };
                float       len_sq      = AB.x*AB.x + AB.y*AB.y;
                float       t           = (len_sq > 0.f) ? (AP.x*AB.x + AP.y*AB.y) / len_sq : 0.f;
                t                       = std::clamp(t, 0.f, 1.f);

                ImVec2      C           { A.x + AB.x*t, A.y + AB.y*t };
                float       dx          = ms.x - C.x, dy = ms.y - C.y;
                if ( dx*dx + dy*dy <= m_style.HIT_THRESH_SQ )
                    { return Hit{ Hit::Type::Path, index }; }
            }
            else
            {
                ImVec2      prev_ws     { a->x, a->y };
                ImVec2      prev_px     = ws2px(prev_ws);

                for (int k = 1; k <= m_style.ms_BEZIER_HIT_STEPS; ++k)
                {
                    float       t           = static_cast<float>(k) / m_style.ms_BEZIER_HIT_STEPS;
                    ImVec2      cur_ws      = cubic_eval<VertexID>(a, b, t);
                    ImVec2      cur_px      = ws2px(cur_ws);

                    ImVec2      seg         { cur_px.x - prev_px.x, cur_px.y - prev_px.y };
                    ImVec2      to_pt       { ms.x - prev_px.x,  ms.y - prev_px.y };
                    float       len_sq      = seg.x*seg.x + seg.y*seg.y;
                    float       u           = (len_sq > 0.f) ? (to_pt.x*seg.x + to_pt.y*seg.y) / len_sq : 0.f;
                    u                       = std::clamp(u, 0.f, 1.f);
                    ImVec2      C           { prev_px.x + seg.x*u, prev_px.y + seg.y*u };
                    float       dx          = ms.x - C.x, dy = ms.y - C.y;
                    
                    if ( dx*dx + dy*dy <= m_style.HIT_THRESH_SQ )
                        { return Hit{ Hit::Type::Path, index }; }

                    prev_px                 = cur_px;
                }
            }
        }

        // interior point-in-polygon (same as before)
        if (p.closed)
        {
            std::vector<ImVec2>     poly;
            poly.reserve(N * 4);

            for (size_t vi = 0; vi < N; ++vi)
            {
                const Vertex* a = find_vertex(m_vertices, p.verts[vi]);
                const Vertex* b = find_vertex(m_vertices, p.verts[(vi + 1) % N]);
                if (!a || !b) continue;

                if (!is_curved<VertexID>(a, b))
                    poly.push_back(ws2px({ a->x, a->y }));
                else
                    for (int k = 0; k <= m_style.ms_BEZIER_HIT_STEPS; ++k) {
                        float  t   = static_cast<float>(k) / m_style.ms_BEZIER_HIT_STEPS;
                        ImVec2 wpt = cubic_eval<VertexID>(a, b, t);
                        poly.push_back(ws2px(wpt));
                    }
            }
            if (!poly.empty() && point_in_polygon(poly, ms))
                return Hit{ Hit::Type::Path, index };
        }
    }

    return std::nullopt;   // nothing hit
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
    ImVec2                  ms              = ImGui::GetIO().MousePos;

    std::optional<PathHit>  best;
    float                   best_d2         = thresh_sq;

    auto    ws2px          = [this](ImVec2 w){ return world_to_pixels(w); };
    auto    lerp           = [](float a, float b, float t){ return a + (b - a) * t; };
    auto    is_zero        = [](const ImVec2& v){ return v.x == 0.f && v.y == 0.f; };

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
                const int   STEPS       = m_style.ms_BEZIER_HIT_STEPS;
                ImVec2      prev_px     = ws2px({ a->x, a->y });
                ImVec2      prev_ws     { a->x, a->y };

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
//
//      SELECTION IMPLEMENTATIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_MECH_process_selection"
//
//      THIS IS WHERE "SELECTION" STARTS...
//      ---This functon is called by "Begin()" to handle ALL sunsequent selection operations.
//
void Editor::_MECH_process_selection(const Interaction & it)
{
    update_move_drag_state              (it);   //  Helpers (2-4 merged)...
    resolve_pending_selection           (it);   //  ...
    dispatch_selection_context_menus    (it);   //  Right-Click CONTEXT MENU...
    
    
    //  OPEN/CLOSE SELECTION OVERLAY WINDOW...
    if ( m_sel.is_empty() )             { return; }
    else                                { this->m_editor_S.m_show_sel_overlay = true; }
    
    
    
    return;
}


//////////////////////////////////////////////////////////////
//      HELPERS FOR "PROCESS_SELECTION" IMPLEMENTATION      //
//////////////////////////////////////////////////////////////

//  "add_hit_to_selection"
//      selection helper: adds a Hit to m_sel (ignores handles)
//
void Editor::add_hit_to_selection(const Hit & hit)
{
    //  0.  CLICKED ON HANDLE.
    if (hit.type == Hit::Type::Handle) return;


    //  1.  CLICKED ON POINT.
    if (hit.type == Hit::Type::Point)
    {
        size_t    idx  = hit.index;
        uint32_t  vid  = m_points[idx].v;
        m_sel.points.insert(idx);
        m_sel.vertices.insert(vid);
        //  NEW.
        m_show_handles.insert(vid);
    }
    //
    //  else if (hit.type == Hit::Type::Path) {
    //      size_t idx = hit.index;
    //      const Path & p = m_paths[idx];
    //      m_sel.paths.insert(idx);
    //      for (uint32_t vid : p.verts)
    //          m_sel.vertices.insert(vid);
    //  }
    //
    //
    //  2.  CLICKED ON PATH.
    else if (hit.type == Hit::Type::Path)
    {
        size_t idx = hit.index;
        const Path& p = m_paths[idx];

        m_sel.paths.insert(idx);

        // include every vertex + its glyph index
        for (uint32_t vid : p.verts)
        {
            m_sel.vertices.insert(vid);

            for (size_t gi = 0; gi < m_points.size(); ++gi)
                if (m_points[gi].v == vid)
                    m_sel.points.insert(gi);
        }
    }
    //
    //  3.  CLICKED ON LINE.
    else
    {
        size_t    idx = hit.index;
        uint32_t  va  = m_lines[idx].a,
                  vb  = m_lines[idx].b;
        m_sel.lines.insert(idx);
        m_sel.vertices.insert(va);
        m_sel.vertices.insert(vb);
    }
    
    return;
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
    ImGuiIO &       io              = ImGui::GetIO();
    const bool      lmb             = io.MouseDown[ImGuiMouseButton_Left];
    const bool      lmb_click       = ImGui::IsMouseClicked  (ImGuiMouseButton_Left);
    const bool      lmb_release     = ImGui::IsMouseReleased (ImGuiMouseButton_Left);
    const bool      dragging_now    = ImGui::IsMouseDragPastThreshold(ImGuiMouseButton_Left, 0.0f);

    ImVec2          w_mouse         = pixels_to_world(io.MousePos);      // NEW


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
            if (!io.KeyCtrl && !io.KeyShift)        { this->reset_selection(); }
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
    if ( !m_dragging && lmb && !m_sel.empty() && !m_pending_hit )
    {
        bool    inside_sel  = false;
        int     idx         = _hit_point(it);
        
        if ( idx >= 0 && m_sel.points.count(idx) )  { inside_sel = true; }
        else if ( m_sel.vertices.size() > 1 )
        {
            ImVec2 tl, br;
            if ( _selection_bounds(tl, br) ) {
                inside_sel = (press_ws.x >= tl.x && press_ws.x <= br.x &&
                               press_ws.y >= tl.y && press_ws.y <= br.y);
            }
        }
        if ( inside_sel && dragging_now )           { start_move_drag(press_ws); }
    }

    // --------------------------------------------------------------------
    // 2. per-frame rigid-snap translation (unchanged)
    // --------------------------------------------------------------------
    if (m_dragging)
    {
        ImVec2      delta    = ImVec2( w_mouse.x - m_movedrag.press_ws.x, w_mouse.y - m_movedrag.press_ws.y );

        if ( want_snap() ) {
            ImVec2 snapped  = snap_to_grid({ m_movedrag.anchor_ws.x + delta.x,    m_movedrag.anchor_ws.y + delta.y });
            delta.x         = snapped.x - m_movedrag.anchor_ws.x;
            delta.y         = snapped.y - m_movedrag.anchor_ws.y;
        }

        for (size_t i = 0; i < m_movedrag.v_ids.size(); ++i) {
            if (Vertex* v = find_vertex_mut(m_vertices, m_movedrag.v_ids[i]))
            {
                const ImVec2 &  orig    = m_movedrag.v_orig[i];
                v->x                    = orig.x + delta.x;
                v->y                    = orig.y + delta.y;
            }
        }

        if (lmb_release)    { m_dragging = false; }
    }

    // ------------------------------------------------------------------
    // 4. per-frame translation while dragging  (rigid snap)
    // ------------------------------------------------------------------
    if (m_dragging)
    {
        // raw translation from initial press
        ImVec2      delta       = ImVec2( w_mouse.x - m_movedrag.press_ws.x, w_mouse.y - m_movedrag.press_ws.y );

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
    if ( !it.hovered || m_dragging )    { return; }

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        if (m_pending_hit || !m_pending_clear)
        {
            if (m_pending_clear)    { this->reset_selection(); } // m_sel.clear();

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
    
    return;
}






// *************************************************************************** //
//
//
//
//      SELECTION UTILITIES...
// *************************************************************************** //
// *************************************************************************** //

//  "_rebuild_vertex_selection"
//
void Editor::_rebuild_vertex_selection()
{
    m_sel.vertices.clear();
    for (size_t pi : m_sel.points) {
        if (pi < m_points.size())
            m_sel.vertices.insert(m_points[pi].v);
    }

    for (size_t li : m_sel.lines) {
        if (li < m_lines.size()) {
            m_sel.vertices.insert(m_lines[li].a);
            m_sel.vertices.insert(m_lines[li].b);
        }
    }

    // Include vertices from any selected paths
    for (size_t pi : m_sel.paths) {
        if (pi < m_paths.size())
            for (uint32_t vid : m_paths[pi].verts)
                m_sel.vertices.insert(vid);
    }
    
    return;
}







// *************************************************************************** //
//
//
//
//      SELECTION HIGHLIGHT / USER-INTERACTION / APPEARANCE...
// *************************************************************************** //
// *************************************************************************** //

//  "_MECH_hit_detection"
//      pointer-cursor hint ───────────────────────────
//
void Editor::_MECH_hit_detection(const Interaction& it) const
{
    if ( !it.hovered )                      { return; }         //  cursor not over canvas
    if ( m_dragging || m_boxdrag.active )   { return; }         //  ignore while dragging
    
    
    // optional: also skip while a handle is already being dragged
    // if (m_dragging_handle) return;


    //  auto hit = _hit_any(it);                        // point / path / line / handle / none
    
    this->m_sel.hovered = _hit_any(it);
    
    if ( !this->m_sel.hovered )     { return; }




    //  DISPATCH HIT-DETECTION...
    //  switch (hit->type)
    switch ( this->m_sel.hovered->type )
    {
        //      1.  HOVERED OVER "BEZIER"-SQUARE HANDLE.
        case Hit::Type::Handle : {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            break;
        }
        
        //      2.  HOVERED OVER VERTEX-GLYPH.
        case Hit::Type::Point: {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            break;
        }

        //      3.  HOVERED OVER "LINE" OR "PATH".
        case Hit::Type::Path :                       // open or closed path
        case Hit::Type::Line : {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
            break;
        }

        default :       { break; }
    }
    
    
    
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
    
    if ( !io.KeyShift && !io.KeyCtrl )  { this->reset_selection(); } //m_sel.clear();  // fresh selection

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
    ImGuiIO &   io      = ImGui::GetIO();
    m_lasso_end         = io.MousePos;

    // Visual feedback rectangle
    it.dl->AddRectFilled(m_lasso_start, m_lasso_end, m_style.COL_LASSO_FILL);
    it.dl->AddRect      (m_lasso_start, m_lasso_end, m_style.COL_LASSO_OUT);

    // Finalise on mouse-up
    if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
    {
        // Screen → world conversion and rect normalisation
        ImVec2 tl_scr{ std::min(m_lasso_start.x, m_lasso_end.x),
                       std::min(m_lasso_start.y, m_lasso_end.y) };
        ImVec2 br_scr{ std::max(m_lasso_start.x, m_lasso_end.x),
                       std::max(m_lasso_start.y, m_lasso_end.y) };
        ImVec2 tl_w = pixels_to_world(tl_scr);
        ImVec2 br_w = pixels_to_world(br_scr);
        if (tl_w.x > br_w.x) std::swap(tl_w.x, br_w.x);
        if (tl_w.y > br_w.y) std::swap(tl_w.y, br_w.y);

        // Selection modifiers
        bool additive = io.KeyShift || io.KeyCtrl;
        if (!additive) m_sel.clear();

        // ---------- Points ----------
        for (size_t i = 0; i < m_points.size(); ++i)
        {
            const Vertex* v = find_vertex(m_vertices, m_points[i].v);
            if (!v) continue;

            const Path* pp = parent_path_of_vertex(m_points[i].v);
            if (!pp || !pp->is_mutable()) continue;          // NEW guard

            bool inside = (v->x >= tl_w.x && v->x <= br_w.x &&
                           v->y >= tl_w.y && v->y <= br_w.y);
            if (!inside) continue;

            if (additive) {
                if (!m_sel.points.erase(i)) m_sel.points.insert(i);
            } else {
                m_sel.points.insert(i);
            }
        }

        // ---------- Lines ----------
        auto seg_rect_intersect = [](ImVec2 a, ImVec2 b,
                                     ImVec2 tl, ImVec2 br)->bool
        {
            auto inside = [&](ImVec2 p){
                return p.x >= tl.x && p.x <= br.x &&
                       p.y >= tl.y && p.y <= br.y;
            };
            if (inside(a) || inside(b)) return true;
            if ((a.x < tl.x && b.x < tl.x) || (a.x > br.x && b.x > br.x) ||
                (a.y < tl.y && b.y < tl.y) || (a.y > br.y && b.y > br.y))
                return false;

            auto ccw = [](ImVec2 p1, ImVec2 p2, ImVec2 p3){
                return (p3.y - p1.y)*(p2.x - p1.x) >
                       (p2.y - p1.y)*(p3.x - p1.x);
            };
            auto intersect = [&](ImVec2 p1, ImVec2 p2,
                                 ImVec2 p3, ImVec2 p4){
                return ccw(p1,p3,p4) != ccw(p2,p3,p4) &&
                       ccw(p1,p2,p3) != ccw(p1,p2,p4);
            };

            ImVec2 tr{ br.x, tl.y }, bl{ tl.x, br.y };
            return intersect(a,b, tl,tr) || intersect(a,b,tr,br) ||
                   intersect(a,b, br,bl) || intersect(a,b, bl,tl);
        };

        for (size_t i = 0; i < m_lines.size(); ++i)
        {
            const Line& ln = m_lines[i];
            if (!ln.is_mutable()) continue;                     // NEW guard

            const Vertex* a = find_vertex(m_vertices, ln.a);
            const Vertex* b = find_vertex(m_vertices, ln.b);
            if (!a || !b) continue;
            if (!seg_rect_intersect({a->x,a->y}, {b->x,b->y}, tl_w, br_w))
                continue;

            if (additive) {
                if (!m_sel.lines.erase(i)) m_sel.lines.insert(i);
            } else {
                m_sel.lines.insert(i);
            }
        }

        // ---------- Paths ----------
        for (size_t pi = 0; pi < m_paths.size(); ++pi)
        {
            const Path& p = m_paths[pi];
            if (!p.is_mutable()) continue;                      // NEW guard

            const size_t N = p.verts.size();
            if (N < 2) continue;

            bool intersects = false;
            for (size_t si = 0; si < N - 1 + (p.closed ? 1u : 0u); ++si)
            {
                const Vertex* a = find_vertex(m_vertices, p.verts[si]);
                const Vertex* b = find_vertex(m_vertices,
                                              p.verts[(si+1)%N]);
                if (!a || !b) continue;
                if (seg_rect_intersect({a->x,a->y},
                                       {b->x,b->y}, tl_w, br_w))
                { intersects = true; break; }
            }
            if (!intersects) continue;

            if (additive) {
                if (!m_sel.paths.erase(pi)) m_sel.paths.insert(pi);
            } else {
                m_sel.paths.insert(pi);
            }
        }

        // Sync vertices and reset lasso state
        _rebuild_vertex_selection();
        m_lasso_active = false;
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
void Editor::_start_bbox_drag(uint8_t handle_idx, const ImVec2 tl_tight, const ImVec2 br_tight)
{
    ImGuiIO& io = ImGui::GetIO();

    const auto [tl, br] = _expand_bbox_by_pixels(tl_tight, br_tight, this->m_style.SELECTION_BBOX_MARGIN_PX);

    m_boxdrag = {};
    m_boxdrag.active      = true;
    m_boxdrag.first_frame = true;
    m_boxdrag.handle_idx  = handle_idx;
    m_boxdrag.bbox_tl_ws  = tl;
    m_boxdrag.bbox_br_ws  = br;
    m_boxdrag.orig_w      = br.x - tl.x;
    m_boxdrag.orig_h      = br.y - tl.y;

    m_boxdrag.mouse_ws0   = pixels_to_world(io.MousePos);
    m_boxdrag.handle_ws0  = _bbox_handle_pos_ws(handle_idx, tl, br);
    m_boxdrag.anchor_ws   = _bbox_pivot_opposite(handle_idx, tl, br);

    m_boxdrag.v_ids.clear();
    m_boxdrag.v_orig.clear();
    m_boxdrag.v_ids.reserve(m_sel.vertices.size());
    m_boxdrag.v_orig.reserve(m_sel.vertices.size());
    for (uint32_t vid : m_sel.vertices)
        if (const Vertex* v = find_vertex(m_vertices, vid)) {
            m_boxdrag.v_ids .push_back(vid);
            m_boxdrag.v_orig.push_back(ImVec2{v->x, v->y});
        }
    
    return;
}


void Editor::_update_bbox(void)
{
    if (!m_boxdrag.active) return;

    ImGuiIO& io = ImGui::GetIO();

    // 1) Read/snaps the mouse in *world* space
    ImVec2 M = pixels_to_world(io.MousePos);
    if (want_snap()) M = snap_to_grid(M);

    // 2) Establish original box & the pivot for this frame
    const ImVec2 tl0 = m_boxdrag.bbox_tl_ws;
    const ImVec2 br0 = m_boxdrag.bbox_br_ws;
    const ImVec2 c0  { (tl0.x + br0.x) * 0.5f, (tl0.y + br0.y) * 0.5f };

    // Allow “scale from center” while Alt is held *during* drag
    const bool   scale_from_center = alt_down();
    const ImVec2 P = scale_from_center ? c0
                                       : _bbox_pivot_opposite(m_boxdrag.handle_idx, tl0, br0);

    // Initial handle position relative to pivot (frozen at start)
    const ImVec2 H0 = _bbox_handle_pos_ws(m_boxdrag.handle_idx, tl0, br0);

    // 3) Compute per-axis scale from the ratio (mouse-to-pivot) / (handle0-to-pivot)
    float sx = 1.0f, sy = 1.0f;

    const bool is_corner   = (m_boxdrag.handle_idx % 2 == 0);           // 0,2,4,6
    const bool is_side_ns  = (m_boxdrag.handle_idx == 1 || m_boxdrag.handle_idx == 5);
    const bool is_side_ew  = (m_boxdrag.handle_idx == 3 || m_boxdrag.handle_idx == 7);

    // Corner or E/W side affect X
    if (is_corner || is_side_ew) sx = _safe_div(M.x - P.x, H0.x - P.x);
    // Corner or N/S side affect Y
    if (is_corner || is_side_ns) sy = _safe_div(M.y - P.y, H0.y - P.y);

    // Constrain side handles to one axis
    if (is_side_ns) sx = 1.0f;
    if (is_side_ew) sy = 1.0f;

    // Optional: hold Shift → uniform scaling (match larger magnitude)
    if (is_corner && io.KeyShift) {
        const float s = (std::fabs(sx) > std::fabs(sy)) ? sx : sy;
        sx = sy = s;
    }

    // 4) Apply affine x' = P + S*(x - P) to *original* positions
    const std::size_t n = m_boxdrag.v_ids.size();
    for (std::size_t i = 0; i < n; ++i)
        if (Vertex* v = find_vertex_mut(m_vertices, m_boxdrag.v_ids[i])) {
            const ImVec2 q0 { m_boxdrag.v_orig[i].x - P.x, m_boxdrag.v_orig[i].y - P.y };
            const ImVec2 q1 { q0.x * sx,                  q0.y * sy };
            v->x = P.x + q1.x;
            v->y = P.y + q1.y;
        }

    // 5) End gesture
    if (!io.MouseDown[ImGuiMouseButton_Left]) {
        m_boxdrag.active = false;
        m_boxdrag.first_frame = true;
    } else {
        m_boxdrag.first_frame = false;
    }
}


/*
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

    // snapshot affected vertices //
    for (uint32_t vid : m_sel.vertices)
        if (const Vertex* v = find_vertex(m_vertices, vid))
        {
            m_boxdrag.v_ids.push_back(vid);
            m_boxdrag.v_orig.push_back({ v->x, v->y });
        }

    // store mouse position in world coords AFTER reset //
    m_boxdrag.mouse_ws0 = pixels_to_world(ImGui::GetIO().MousePos);   // NEW

    // reset first-frame flag for update routine //
    m_boxdrag.first_frame = true;
}


//  "_update_bbox"
//
void Editor::_update_bbox(void)
{
    if (!m_boxdrag.active)  { return; }

    // if cursor hasn't moved since click, keep original scale //
    if (m_boxdrag.first_frame &&
        ImGui::GetIO().MouseDelta.x == 0.f &&
        ImGui::GetIO().MouseDelta.y == 0.f)
        return;

    m_boxdrag.first_frame = false;   // first frame handled

    // stop drag //
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        m_boxdrag.first_frame = true;          // reset for next drag
        m_boxdrag = {};
        return;
    }

    // current mouse in world coords //
    ImVec2 cur_ws = pixels_to_world(ImGui::GetIO().MousePos);   // NEW

    // reference point for scaling //
    ImVec2 ref_ws = m_boxdrag.first_frame ? m_boxdrag.mouse_ws0 : cur_ws;

    ImVec2 h_ws{ ref_ws.x - m_boxdrag.anchor_ws.x,
                 ref_ws.y - m_boxdrag.anchor_ws.y };

    // original vector from anchor to handle //
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

    // lock one axis for edge handles (1=T,3=R,5=B,7=L) //
    const bool handle_top    = (m_boxdrag.handle_idx == 1);
    const bool handle_right  = (m_boxdrag.handle_idx == 3);
    const bool handle_bottom = (m_boxdrag.handle_idx == 5);
    const bool handle_left   = (m_boxdrag.handle_idx == 7);

    if (handle_top || handle_bottom)  sx = 1.f;   // vertical edge → X scale locked
    if (handle_left || handle_right)  sy = 1.f;   // horizontal edge → Y scale locked

    // Shift → uniform scale //
    if (ImGui::GetIO().KeyShift)
    {
        float s = std::max(std::fabs(sx), std::fabs(sy));
        sx = (sx < 0.f ? -s : s);
        sy = (sy < 0.f ? -s : s);
    }

    // re‑assert locked axis after uniform adjustment //
    if (handle_top || handle_bottom)  sx = 1.f;
    if (handle_left || handle_right)  sy = 1.f;

    // apply to vertices //
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
}*/











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

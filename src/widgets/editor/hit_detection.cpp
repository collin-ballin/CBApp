/***********************************************************************************
*
*       *********************************************************************
*       ****      H I T _ D E T E C T I O N . C P P  ____  F I L E       ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      September 16, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "widgets/editor/editor.h"



namespace cb {  //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //






// *************************************************************************** //
//
//
//
//      1.      HIT-DETECTION ORCHESTRATOR...
// *************************************************************************** //
// *************************************************************************** //

//  "_MECH_hit_detection"           formerly named:     "update_cursor_select".
//
//      pointer-cursor hint ───────────────────────────
//
void Editor::_MECH_hit_detection(const Interaction & it) const
{
    if ( m_dragging || m_boxdrag.active )   { return; }         //  ignore while dragging
    
    
    
    
    //      1.      DISPATCH CURSOR  *ICONS*  IF MODE  *DOES NOT*  HAVE CURSOR-HINTS...
    if ( !_mode_has(CBCapabilityFlags_CursorHint) )
    {
        this->_dispatch_cursor_icon(it);
    }
    
    
    
    
    
    //      2.      CHECK IF CURSOR IS ABOVE SELECTION-BBOX HANDLE...
    if ( m_boxdrag.view.visible  &&  this->_hit_bbox_handle(it) )
    {
        return;
    }
    
    
    
    // optional: also skip while a handle is already being dragged
    // if (m_dragging_handle) return;
    
    this->m_sel.hovered = _hit_any(it);                 // point / path / line / handle / none
    


    //      CASE 2 :    DISPATCH CURSOR  *HINTS*...
    //          (ONLY IF CURRENT TOOL ALLOWS THEM *AND*  AN ITEM IS HOVERED).
    if ( this->m_sel.hovered )
    {
        this->_dispatch_cursor_hint( this->m_sel.hovered->type );
    }
    
    
    
    return;
}



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "HIT-DETECTION" ORCHESTRATOR.






// *************************************************************************** //
//
//
//
//      1.      MAIN HIT-DETECTION MECHANISMS...
// *************************************************************************** //
// *************************************************************************** //
    
//  "_dispatch_cursor_hint"
//
inline void Editor::_dispatch_cursor_hint(const Hit::Type type) const noexcept
{
    using                       HitType         = Hit::Type;
    const BrowserState &        BS              = this->m_browser_S;
    
    
    
    //      DISPATCH HIT-DETECTION CURSOR HINT...
    switch ( type )
    {
        //      1.  HOVERED OVER "BEZIER"-SQUARE HANDLE.
        case HitType::Handle :
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            break;
        }
        
        //      2.  HOVERED OVER VERTEX-GLYPH.
        case HitType::Vertex :
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            break;
        }
        
        //      3.  HOVERED OVER AN "EDGE".
        case HitType::Edge :
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);
            break;
        }

        //      4.  HOVERED OVER "PATH".
        case HitType::Surface :
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
            BS.m_hovered_canvas_obj      = -1;
            break;
        }

        default :       { break; }
    }
 
    return;
}

    
//  "_dispatch_cursor_icon"
//
inline void Editor::_dispatch_cursor_icon(const Interaction & it) const
{
    return;
}



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "HIT-DETECTION ORCHESTRATOR".






// *************************************************************************** //
//
//
//
//      1.      HIT-DETECTION UTILITIES...
// *************************************************************************** //
// *************************************************************************** //

//  "_hit_bbox_handle"
//
[[nodiscard]] inline bool Editor::_hit_bbox_handle([[maybe_unused]] const Interaction & it) const
{
    //  NEW:    Selection BBox handles take precedence
    m_boxdrag.view.hover_idx    = -1;
    const ImVec2    mp          = ImGui::GetIO().MousePos;

    for (int i = 0; i < 8; ++i)
    {
        if ( m_boxdrag.view.handle_rect_px[i].Contains(mp) )
        {
            m_boxdrag.view.hover_idx = i;
            m_hover_handle           = i;                       // ← NEW: bridge for legacy checks
            ImGui::SetMouseCursor(_cursor_for_bbox_handle(i) );
            return true;
        }
    }
    m_hover_handle              = -1;                                       // ← keep in sync
    
    return false;
}


//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "HIT-DETECTION ORCHESTRATOR".






// *************************************************************************** //
//
//
//
//      2.      MAIN HIT-DETECTION MECHANISMS...
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
    using                   HitType                 = Hit::Type;
    constexpr float         PAD                     = 4.0f;
    const float             HALF                    = m_style.HANDLE_BOX_SIZE * 0.5f + PAD;
    const ImVec2            ms                      = ImGui::GetIO().MousePos;          // mouse in pixels
    auto                    ws2px                   = [&](const ImVec2& w){ return world_to_pixels(w); };

    //  "parent_path_of_vertex"
    //      Helper: map vertex-id → parent Path*, or nullptr if none.
    auto                    parent_path_of_vertex   = [&](VertexID vid) -> const Path * {
        for (const Path & p : m_paths)
        {
            for (VertexID v : p.verts) {
                if (v == vid) { return &p; }
            }
        }
        return nullptr;
    };
    //  "overlap"
    auto                    overlap                 = [&](ImVec2 scr)       { return fabsf(ms.x - scr.x) <= HALF && fabsf(ms.y - scr.y) <= HALF; };



    // ───────────────────────────────────────────── 1. Bézier handles
    for (const Vertex & v : m_vertices)
    {
        if (!m_sel.vertices.count(v.id))            { continue; }  // handles only for selected verts
        const Path* pp = parent_path_of_vertex(v.id);
        
        if ( !pp || pp->locked || !pp->visible )    { continue; }


        if (v.m_bezier.out_handle.x || v.m_bezier.out_handle.y)
        {
            ImVec2 scr = ws2px({ v.x + v.m_bezier.out_handle.x,
                                 v.y + v.m_bezier.out_handle.y });
            if ( overlap(scr) )
                return Hit{ Hit::Type::Handle,
                            static_cast<size_t>(v.id), true };
        }
        if ( v.m_bezier.in_handle.x || v.m_bezier.in_handle.y )
        {
            ImVec2 scr = ws2px({ v.x + v.m_bezier.in_handle.x,
                                 v.y + v.m_bezier.in_handle.y });
            if ( overlap(scr) )
            {
                return Hit{ Hit::Type::Handle,
                            static_cast<size_t>(v.id), false };
            }
        }
    }


    // ───────────────────────────────────────────── 2. point glyphs
    int pi = _hit_point(it);
    if ( pi >= 0  &&  static_cast<size_t>(pi) < m_points.size() )
    {
        uint32_t        vid     = m_points[pi].v;
        const Path *    pp      = parent_path_of_vertex(vid);
        
        if ( pp && !pp->locked && pp->visible )
        {
            return Hit{ Hit::Type::Vertex, static_cast<size_t>(pi) };
        }
    }

    // ───────────────────────────────────────────── 4. paths
    // Build vector of unlocked+visible paths, sort by z (low→high), iterate reverse
    std::vector<const Path*>    vec;
    vec.reserve( m_paths.size() );
    
    for (const Path& p : m_paths) {
        if ( !p.locked  &&  p.visible )         { vec.push_back(&p); }
    }

    std::stable_sort(vec.begin(), vec.end(),
        [](const Path* a, const Path* b){ return a->z_index < b->z_index; });



    for (auto rit = vec.rbegin(); rit != vec.rend(); ++rit)
    {
        const Path &        p           = **rit;
        const size_t        N           = p.verts.size();
        const size_t        index       = &p - m_paths.data();          // back to array idx
        if (N < 2)          { continue; }


        //  CASE 1 :    HIT AN  *EDGE*  OF A PATH...
        for ( size_t si = 0; si < N - 1 + (p.closed ? 1u : 0u); ++si )
        {
            const Vertex *      a       = find_vertex(m_vertices, p.verts[si]);
            const Vertex *      b       = find_vertex(m_vertices, p.verts[(si + 1) % N]);
            if ( !a || !b )     { continue; }


            //          1.1.        STRAIGHT-EDGE.
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
                
                
                //  straight edge proximity
                if ( dx*dx + dy*dy <= m_style.HIT_THRESH_SQ )
                {
                    //  return Hit{ Hit::Type::Surface, index };
                    return Hit{ HitType::Edge, index };
                }
            }
            //
            //          1.2.        BEZIER-CURVED EDGE.
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
                    
                    
                    //  bezier edge proximity
                    if ( dx*dx + dy*dy <= m_style.HIT_THRESH_SQ )
                    {
                        //  return Hit{ Hit::Type::Surface, index };
                        return Hit{ HitType::Edge, index };
                    }


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
                const Vertex *      a       = find_vertex(m_vertices, p.verts[vi]);
                const Vertex *      b       = find_vertex(m_vertices, p.verts[(vi + 1) % N]);
                if ( !a || !b )             { continue; }

                if ( !is_curved<VertexID>(a, b) ) {
                    poly.push_back(ws2px({ a->x, a->y }));
                }
                else
                {
                    for (int k = 0; k <= m_style.ms_BEZIER_HIT_STEPS; ++k) {
                        float  t   = static_cast<float>(k) / m_style.ms_BEZIER_HIT_STEPS;
                        ImVec2 wpt = cubic_eval<VertexID>(a, b, t);
                        poly.push_back(ws2px(wpt));
                    }
                }
            }
            if ( !poly.empty() && point_in_polygon(poly, ms) )
            {
                return Hit{ Hit::Type::Surface, index };
            }
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
    // Zoom-invariant pick thresholds (px)
    constexpr float     PICK_PX             = 6.0f;   // same radius you used before
    constexpr float     ENDPOINT_EPS_PX     = 3.0f;   // reject cuts effectively "on" a vertex
    const float         thresh_sq           = PICK_PX * PICK_PX;
    const float         endpoint_eps_sq     = ENDPOINT_EPS_PX * ENDPOINT_EPS_PX;

    //  Mouse in pixel space
    const ImVec2        ms                  = ImGui::GetIO().MousePos;

    //  Helpers
    auto ws2px = [this](ImVec2 w){ return world_to_pixels(w); };
    auto lerpf = [](float a, float b, float t){ return a + (b - a) * t; };
    auto is_zero = [](const ImVec2& v){ return v.x == 0.f && v.y == 0.f; };

    //  Build Z-sorted list of eligible paths (visible & unlocked), topmost last
    std::vector<size_t> order;
    order.reserve(m_paths.size());
    for (size_t pi = 0; pi < m_paths.size(); ++pi) {
        const Path& p = m_paths[pi];
        if (!p.visible || p.locked) continue;
        order.push_back(pi);
    }
    std::stable_sort(order.begin(), order.end(),
        [&](size_t a, size_t b){ return m_paths[a].z_index < m_paths[b].z_index; });

    // Walk from topmost down; return as soon as we find a path with a valid segment hit
    for (auto rit = order.rbegin(); rit != order.rend(); ++rit)
    {
        const size_t  pi = *rit;
        const Path&   p  = m_paths[pi];
        const size_t  N  = p.verts.size();
        if (N < 2) continue;

        const bool    closed     = p.closed;
        const size_t  seg_count  = N - (closed ? 0 : 1);

        std::optional<PathHit> best_for_path;
        float                  best_d2 = thresh_sq;

        for (size_t si = 0; si < seg_count; ++si)
        {
            const Vertex* a = find_vertex(m_vertices, p.verts[si]);
            const Vertex* b = find_vertex(m_vertices, p.verts[(si + 1) % N]);
            if (!a || !b) continue;

            const bool curved = !(is_zero(a->m_bezier.out_handle) && is_zero(b->m_bezier.in_handle));

            if (!curved)
            {
                // Straight segment in pixel space
                const ImVec2 A = ws2px({ a->x, a->y });
                const ImVec2 B = ws2px({ b->x, b->y });

                const ImVec2 AB{ B.x - A.x, B.y - A.y };
                const ImVec2 AM{ ms.x - A.x, ms.y - A.y };
                const float   len_sq = AB.x*AB.x + AB.y*AB.y;
                if (len_sq == 0.f) continue;

                const float t  = std::clamp((AM.x*AB.x + AM.y*AB.y) / len_sq, 0.f, 1.f);
                const ImVec2 C { A.x + AB.x*t, A.y + AB.y*t };

                const float dx = ms.x - C.x, dy = ms.y - C.y;
                const float d2 = dx*dx + dy*dy;

                // Reject near-endpoint hits (avoid splitting at existing vertex)
                const float d2A = (ms.x - A.x)*(ms.x - A.x) + (ms.y - A.y)*(ms.y - A.y);
                const float d2B = (ms.x - B.x)*(ms.x - B.x) + (ms.y - B.y)*(ms.y - B.y);
                if (d2A <= endpoint_eps_sq || d2B <= endpoint_eps_sq) continue;

                if (d2 < best_d2) {
                    best_d2 = d2;
                    best_for_path = PathHit{
                        pi, si, t,
                        ImVec2{ lerpf(a->x, b->x, t), lerpf(a->y, b->y, t) }
                    };
                }
            }
            else
            {
                // Cubic Bézier: sample sub-segments and project in pixel space
                const int   STEPS   = m_style.ms_BEZIER_HIT_STEPS;
                ImVec2      prev_ws { a->x, a->y };
                ImVec2      prev_px = ws2px(prev_ws);

                for (int k = 1; k <= STEPS; ++k)
                {
                    const float tk    = static_cast<float>(k) / STEPS;
                    const ImVec2 cur_ws = cubic_eval<VertexID>(a, b, tk);
                    const ImVec2 cur_px = ws2px(cur_ws);

                    const ImVec2 seg   { cur_px.x - prev_px.x, cur_px.y - prev_px.y };
                    const ImVec2 to_pt { ms.x    - prev_px.x,  ms.y    - prev_px.y  };

                    const float   len_sq = seg.x*seg.x + seg.y*seg.y;
                    float         u      = (len_sq > 0.f) ? (to_pt.x*seg.x + to_pt.y*seg.y) / len_sq : 0.f;
                    u = std::clamp(u, 0.f, 1.f);

                    const ImVec2 C  { prev_px.x + seg.x*u, prev_px.y + seg.y*u };
                    const float  dx = ms.x - C.x, dy = ms.y - C.y;
                    const float  d2 = dx*dx + dy*dy;

                    // Reject near endpoints of the sub-segment
                    const float d2Prev = (ms.x - prev_px.x)*(ms.x - prev_px.x) + (ms.y - prev_px.y)*(ms.y - prev_px.y);
                    const float d2Cur  = (ms.x - cur_px.x) *(ms.x - cur_px.x)  + (ms.y - cur_px.y) *(ms.y - cur_px.y);
                    if (d2Prev <= endpoint_eps_sq || d2Cur <= endpoint_eps_sq) {
                        prev_px = cur_px;
                        prev_ws = cur_ws;
                        continue;
                    }

                    if (d2 < best_d2)
                    {
                        best_d2 = d2;

                        // Continuous curve parameter for the closest point on the full segment
                        const float t_global = (static_cast<float>(k - 1) + u) / STEPS;

                        // World-space closest point (linear on the sampled chord)
                        const ImVec2 pos_ws {
                            prev_ws.x + (cur_ws.x - prev_ws.x) * u,
                            prev_ws.y + (cur_ws.y - prev_ws.y) * u
                        };

                        best_for_path = PathHit{ pi, si, t_global, pos_ws };
                    }

                    prev_px = cur_px;
                    prev_ws = cur_ws;
                }
            }
        }

        // If the topmost eligible path has a valid hit within threshold, return it immediately
        if (best_for_path) return best_for_path;
    }

    return std::nullopt;   // nothing within PICK_PX on any eligible path
}

/*{
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

            bool curved = !is_zero(a->m_bezier.out_handle) || !is_zero(b->m_bezier.in_handle);

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
}*/



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "HIT-DETECTION".






// *************************************************************************** //
//
//
//
//      2.      HIT-DETECTION UTILITIES...
// *************************************************************************** //
// *************************************************************************** //
    
    
    



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "HIT-DETECTION UTILITIES".












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

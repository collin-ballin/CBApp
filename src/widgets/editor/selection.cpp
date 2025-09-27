/***********************************************************************************
*
*       *********************************************************************
*       ****          S E L E C T I O N . C P P  ____  F I L E           ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      June 14, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "widgets/editor/editor.h"



namespace cb {  //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//      0.      "SELECTION" ORCHESTRATOR...
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
    
    
    if ( it.hovered && !m_dragging )    { resolve_pending_selection(it); }
    
    
    dispatch_selection_context_menus    (it);   //  Right-Click CONTEXT MENU...
    
    
    //  OPEN/CLOSE SELECTION OVERLAY WINDOW...
    if ( m_sel.is_empty() )             { return; }
    else                                { this->m_editor_S.m_show_sel_overlay = true; }
    
    
    
    return;
}

//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "SELECTION ORCHESTRATOR".






// *************************************************************************** //
//
//
//
//      1.      SELECTION MOVE/DRAG...
// *************************************************************************** //
// *************************************************************************** //

//  "start_move_drag"
//
void Editor::start_move_drag(const ImVec2 & press_ws)
{
    m_dragging              = true;
    m_movedrag              = {};
    m_movedrag.active       = true;
    m_movedrag.press_ws     = press_ws;
    ImVec2                  tl{},   br{};
    
    if ( _selection_bounds(tl, br, this->m_render_ctx) )    { m_movedrag.anchor_ws = tl;        }
    else                                { m_movedrag.anchor_ws = press_ws;  }

    m_movedrag.v_ids .reserve(m_sel.vertices.size());
    m_movedrag.v_orig.reserve(m_sel.vertices.size());

    for (uint32_t vid : m_sel.vertices)
    {
        if ( const Vertex * v = find_vertex(m_vertices, vid) ) {
            m_movedrag.v_ids .push_back(vid);
            m_movedrag.v_orig.push_back({ v->x, v->y });
        }
    }
    
    return;
}


//  "update_move_drag_state"
//      Press / drag / release handling ---------------------------------------
//
inline void Editor::update_move_drag_state(const Interaction & it)
{
    ImGuiIO & io           = ImGui::GetIO();
    const bool lmb         = io.MouseDown[ImGuiMouseButton_Left];
    const bool lmb_click   = ImGui::IsMouseClicked  (ImGuiMouseButton_Left);
    const bool lmb_release = ImGui::IsMouseReleased (ImGuiMouseButton_Left);

    const float drag_px    = _drag_threshold_px();
    const bool  drag_past  = ImGui::IsMouseDragPastThreshold(ImGuiMouseButton_Left, drag_px);

    static ImVec2 press_ws{};
    static bool   press_hit_exists = false;
    static bool   press_hit_in_sel = false;

    if (lmb_click && it.hovered)
    {
        press_ws          = pixels_to_world(io.MousePos);
        m_pending_hit     = _hit_any(it);                           // may be null
        press_hit_exists  = m_pending_hit.has_value();
        press_hit_in_sel  = press_hit_exists && _hit_is_in_current_selection(*m_pending_hit);

        const bool modifiers   = (io.KeyCtrl || io.KeyShift);
        const bool inside_bbox = (!m_sel.empty() && _press_inside_selection(press_ws));
        const bool outside_sel = !press_hit_in_sel && !inside_bbox;

        m_pending_clear        = (!modifiers) && outside_sel;       // only tentatively true here
        // IMPORTANT: do NOT start drag yet; we wait for drag_past
    }

    // Begin drag once movement exceeds threshold
    if (lmb && !m_dragging && drag_past && it.hovered)
    {
        if (m_pending_hit)
        {
            if (!press_hit_in_sel) {
                if (m_pending_clear) this->reset_selection();
                add_hit_to_selection(*m_pending_hit);
            }
            start_move_drag(press_ws);

            // NEW: prevent click-resolution from firing on release
            m_pending_hit.reset();
            m_pending_clear = false;                   // ← key fix
            press_hit_exists = false;
            press_hit_in_sel = false;
            return;
        }

        // No direct hit, but press inside current selection bbox → drag whole selection
        if (!m_sel.empty() && _press_inside_selection(press_ws)) {
            start_move_drag(press_ws);
            m_pending_clear = false;                   // ← also important
            return;
        }

        // Else: (optionally) start lasso after threshold if desired
        // if (!m_lasso_active) _start_lasso_tool();
    }

    // Per-frame translation while dragging
    if (m_dragging)
    {
        const ImVec2 w_mouse = pixels_to_world(io.MousePos);
        ImVec2 delta { w_mouse.x - m_movedrag.press_ws.x,
                       w_mouse.y - m_movedrag.press_ws.y };

        if (want_snap()) {
            const ImVec2 snapped = snap_to_grid({ m_movedrag.anchor_ws.x + delta.x,
                                                  m_movedrag.anchor_ws.y + delta.y });
            delta.x = snapped.x - m_movedrag.anchor_ws.x;
            delta.y = snapped.y - m_movedrag.anchor_ws.y;
        }

        for (size_t i = 0; i < m_movedrag.v_ids.size(); ++i)
        {
            if (Vertex* v = find_vertex_mut(m_vertices, m_movedrag.v_ids[i])) {
                const ImVec2& o = m_movedrag.v_orig[i];
                v->x = o.x + delta.x;  v->y = o.y + delta.y;
            }
        }

        if (lmb_release) {
            m_dragging        = false;
            m_pending_hit.reset();
            m_pending_clear   = false;                 // ← guard against release clearing
            press_hit_exists  = false;
            press_hit_in_sel  = false;
        }
        return; // while dragging, skip click-resolution below
    }
}

/*{
    ImGuiIO &       io              = ImGui::GetIO();
    const bool      lmb             = io.MouseDown[ImGuiMouseButton_Left];
    const bool      lmb_click       = ImGui::IsMouseClicked  (ImGuiMouseButton_Left);
    const bool      lmb_release     = ImGui::IsMouseReleased (ImGuiMouseButton_Left);
    const bool      dragging_now    = ImGui::IsMouseDragPastThreshold(ImGuiMouseButton_Left, 0.0f);

    ImVec2          w_mouse         = pixels_to_world(io.MousePos);      // NEW
    static ImVec2   press_ws        {   };


    // remember exact press-position once ----------------------------------
    if ( lmb_click )    { press_ws = pixels_to_world(io.MousePos); }   // NEW


    // --------------------------------------------------------------------
    // 0. NEW: start drag immediately on a fresh hit
    // --------------------------------------------------------------------
    if ( !m_dragging  &&  lmb_click  &&  it.hovered )
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
            if ( _selection_bounds(tl, br, this->m_render_ctx) ) {
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
}*/

//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "MOVE/DRAG".






// *************************************************************************** //
//
//
//
//      2.      MAIN SELECTION BEHAVIORS...
// *************************************************************************** //
// *************************************************************************** //

//  "resolve_pending_selection"
//
inline void Editor::resolve_pending_selection([[maybe_unused]] const Interaction & it)
{
    using       HitType     = Hit::Type;
    ImGuiIO &   io          = ImGui::GetIO();
    
    
    if ( ImGui::IsMouseReleased(ImGuiMouseButton_Left) )
    {
        //  If there is a pending hit, but it was on an already-selected item (no mods, no drag), do nothing.
        if ( m_pending_hit )
        {
            const bool      modifiers   = (io.KeyCtrl || io.KeyShift);
            const bool      hit_in_sel  = _hit_is_in_current_selection(*m_pending_hit);
            
            if ( !modifiers  &&  hit_in_sel )
            {
                m_pending_hit.reset();
                m_pending_clear = false;
                return;
            }
        }

        //  Apply click selection semantics iff something is truly pending
        if ( m_pending_hit  ||  m_pending_clear )
        {
            if (m_pending_clear)    { this->reset_selection(); }


            if (m_pending_hit)
            {
                const Hit &     hit     = *m_pending_hit;
                const size_t    idx     = hit.index;

                switch (hit.type)
                {
                    //      CASE 1 :    HIT A POINT.
                    case HitType::Vertex :
                    {
                        const VertexID  vid     = m_points[idx].v;
                        
                        if ( !m_sel.points.erase(idx) )     { m_sel.points.insert(idx); m_sel.vertices.insert(vid); }
                        else                                { m_sel.vertices.erase(vid); }
                        
                        break;
                    }
                    //
                    //      CASE 2 :    HIT A PATH  *OR*  EDGE.
                    case HitType::Edge :    case HitType::Surface :
                    {
                        const Path &    p       = m_paths[idx];

                        if ( !m_sel.paths.erase(idx) )
                        {
                            //  Add whole path
                            m_sel.paths.insert(idx);
                            for (VertexID vid : p.verts)    { m_sel.vertices.insert(vid); }
                        }
                        else
                        {
                            //  Remove whole path
                            for (uint32_t vid : p.verts)    { m_sel.vertices.erase(vid); }
                            //  (Optional) also clear glyphs for those vids if you mirror points↔verts
                        }
                        break;
                    }
                    //
                    //      DEFAULT :    ASSERTION FAILURE.
                    default :       { IM_ASSERT(false); }
                }
                
            }
            
        }

        //  Clear edge state after processing
        m_pending_hit.reset();
        m_pending_clear = false;
    }
    
    return;
}

/*
{
    //  if ( !it.hovered || m_dragging )    { return; }

    if ( ImGui::IsMouseReleased(ImGuiMouseButton_Left) )
    {
        if (m_pending_hit || !m_pending_clear)
        {
            if (m_pending_clear)    { this->reset_selection(); } // m_sel.clear();

            if (m_pending_hit)
            {
                const Hit & hit = *m_pending_hit;

                if (hit.type == Hit::Type::Vertex)
                {
                    size_t idx = hit.index;
                    uint32_t vid = m_points[idx].v;
                    if (!m_sel.points.erase(idx)) { m_sel.points.insert(idx); m_sel.vertices.insert(vid); }
                    else                           { m_sel.vertices.erase(vid); }
                }
                else if (hit.type == Hit::Type::Surface)
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
*/

//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "MAIN SELECTION BEHAVIORS".






// *************************************************************************** //
//
//
//
//      3.      SELECTION UTILITIES...
// *************************************************************************** //
// *************************************************************************** //

//  "add_hit_to_selection"
//      selection helper: adds a Hit to m_sel (ignores handles)
//
void Editor::add_hit_to_selection(const Hit & hit)
{
    using HitType = Hit::Type;
    
    
    switch (hit.type)
    {
        //      1.          CLICKED ON "HANDLE".
        case HitType::Handle : {
            return;
        }
        //
        //      2.          CLICKED ON "VERTEX".
        case HitType::Vertex :
        {
            size_t          idx     = hit.index;
            VertexID        vid     = this->m_points[idx].v;
            this->m_sel.points      .insert(idx);
            this->m_sel.vertices    .insert(vid);
            this->m_show_handles    .insert(vid); //  NEW.
            break;
        }
        //
        //      3.          CLICKED ON "EDGE"  *OR*  "SURFACE".
        case HitType::Edge :
        case HitType::Surface :
        {
            size_t          idx     = hit.index;
            const Path &    p       = m_paths[idx];
            m_sel.paths             .insert(idx);

            for (VertexID vid : p.verts)        // include every vertex + its glyph index
            {
                m_sel.vertices.insert(vid);
                for (size_t gi = 0; gi < m_points.size(); ++gi)
                {
                    if (m_points[gi].v == vid)      { m_sel.points.insert(gi); }
                }
            }
            break;
        }
        //
        //      DEFAULT.    ASSERTION FAILURE.
        default:    { IM_ASSERT( false ); }
    }
    
    return;
}


//  "_rebuild_vertex_selection"
//
void Editor::_rebuild_vertex_selection(void)
{
    m_sel.vertices.clear();
    for (size_t pi : m_sel.points)
    {
        if ( pi < m_points.size() )     { m_sel.vertices.insert(m_points[pi].v); }
    }
    

    //  Include vertices from any selected paths
    for (size_t pi : m_sel.paths)
    {
        if ( pi < m_paths.size() )
        {
            for (VertexID vid : m_paths[pi].verts) {
                m_sel.vertices.insert(vid);
            }
        }
    }
    
    return;
}

//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "SELECTION UTILITIES".






// *************************************************************************** //
//
//
//
//      4.      BOUNDING BOX...
// *************************************************************************** //
// *************************************************************************** //

//  "_selection_bounds"
//
bool Editor::_selection_bounds(ImVec2 & tl, ImVec2 & br, const RenderCTX & ctx) const
{
    bool    have_any    = false;
    auto    add_pt      = [&](const ImVec2 & p)
    {
        if ( !have_any )    { tl = br = p; have_any = true; }
        else {
            tl.x = std::min(tl.x, p.x);     tl.y = std::min(tl.y, p.y);
            br.x = std::max(br.x, p.x);     br.y = std::max(br.y, p.y);
        }
    };


    // A) Vertex anchors (existing behavior)
    for (VertexID vid : m_sel.vertices)
    {
        if ( const Vertex * v = find_vertex(m_vertices, vid) )
            { add_pt(ImVec2{ v->x, v->y }); }
    }

    // B) Path control-hull union (new: captures curvature envelope)
    for (PathID pidx : m_sel.paths)
    {
        if ( pidx >= m_paths.size() )   { continue; }
        const Path & p = m_paths[pidx];

        ImVec2 p_tl{}, p_br{};
        if ( p.aabb_control_hull(p_tl, p_br, ctx) ) {
            add_pt(p_tl);
            add_pt(p_br);
        }
    }

    return have_any;
}

/*
bool Editor::_selection_bounds(ImVec2 & tl, ImVec2 & br) const
{
    if ( m_sel.vertices.empty() )   { return false; }
    
    bool first = true;
    
    for (uint32_t vid : m_sel.vertices)
    {
        if (const Vertex * v = find_vertex(m_vertices, vid))
        {
            ImVec2 p{ v->x, v->y };
            if (first)  { tl = br = p; first = false; }
            else
            {
                tl.x = std::min(tl.x, p.x); tl.y = std::min(tl.y, p.y);
                br.x = std::max(br.x, p.x); br.y = std::max(br.y, p.y);
            }
        }
    }
    
    return !first;
}*/


//  "_start_bbox_drag"
//
void Editor::_start_bbox_drag(const Editor::BoxDrag::Anchor handle_idx, const ImVec2 tl_exp, const ImVec2 br_exp)
{
    ImGuiIO& io = ImGui::GetIO();

    m_boxdrag             = {};
    m_boxdrag.active      = true;
    m_boxdrag.first_frame = true;
    m_boxdrag.handle_idx  = handle_idx;
    m_boxdrag.bbox_tl_ws  = tl_exp;
    m_boxdrag.bbox_br_ws  = br_exp;
    m_boxdrag.orig_w      = br_exp.x - tl_exp.x;
    m_boxdrag.orig_h      = br_exp.y - tl_exp.y;

    m_boxdrag.mouse_ws0   = pixels_to_world(io.MousePos);
    m_boxdrag.handle_ws0  = BoxDrag::AnchorToWorldPos(handle_idx, tl_exp, br_exp);
    m_boxdrag.anchor_ws   = BoxDrag::OppositePivot   (handle_idx, tl_exp, br_exp);

    m_boxdrag.v_ids             .clear();
    m_boxdrag.v_orig            .clear();
    m_boxdrag.v_ids             .reserve(m_sel.vertices.size());
    m_boxdrag.v_orig            .reserve(m_sel.vertices.size());
    
    
    for (VertexID vid : m_sel.vertices)
    {
        if ( const Vertex * v = find_vertex(m_vertices, vid) ) {
            m_boxdrag.v_ids .push_back(vid);
            m_boxdrag.v_orig.push_back(ImVec2{v->x, v->y});
        }
    }
    
    return;
}


//  "_update_bbox"
//
void Editor::_update_bbox(void)
{
    using           BBAnchor    = BoxDrag::Anchor;
    ImGuiIO &       io          = ImGui::GetIO();
    
    if ( !m_boxdrag.active )    { return; }


    //      1.      Read/snaps the mouse in *world* space
    ImVec2 M = pixels_to_world(io.MousePos);
    if ( want_snap() )      { M = snap_to_grid(M); }


    //      2.      Establish original box & the pivot for this frame
    const ImVec2    tl0     = m_boxdrag.bbox_tl_ws;
    const ImVec2    br0     = m_boxdrag.bbox_br_ws;
    const ImVec2    c0      { (tl0.x + br0.x) * 0.5f, (tl0.y + br0.y) * 0.5f };


    //      Allow “scale from center” while Alt is held *during* drag
    const bool      scale_from_center = alt_down();
    const ImVec2    P       = (scale_from_center)
                                ? c0 : BoxDrag::OppositePivot(m_boxdrag.handle_idx, tl0, br0);

    //      Initial handle position relative to pivot (frozen at start)
    const ImVec2    H0              = BoxDrag::AnchorToWorldPos(m_boxdrag.handle_idx, tl0, br0);


    //      3.      Compute per-axis scale from the ratio (mouse-to-pivot) / (handle0-to-pivot)
    float           sx              = 1.0f, sy = 1.0f;


    const bool      is_corner       = BoxDrag::IsDiagonal       (m_boxdrag.handle_idx);
    const bool      is_side_ns      = BoxDrag::IsVertical       (m_boxdrag.handle_idx);
    const bool      is_side_ew      = BoxDrag::IsHorizontal     (m_boxdrag.handle_idx);



    if ( is_corner || is_side_ew )      { sx = BoxDrag::SafeDiv(M.x - P.x, H0.x - P.x); }      //  Corner or E/W side affect X
    if ( is_corner || is_side_ns )      { sy = BoxDrag::SafeDiv(M.y - P.y, H0.y - P.y); }      //  Corner or N/S side affect Y


    if ( is_side_ns )   { sx = 1.0f; }  //  Constrain side handles to one axis
    if ( is_side_ew )   { sy = 1.0f; }


    //      Optional:   hold Shift → uniform scaling (match larger magnitude)
    if (is_corner && io.KeyShift) {
        const float s = (std::fabs(sx) > std::fabs(sy)) ? sx : sy;
        sx = sy = s;
    }


    //      4.      Apply affine    x' = P + S*(x - P)      to *original* positions
    const std::size_t n = m_boxdrag.v_ids.size();
    for (std::size_t i = 0; i < n; ++i)
    {
        if (Vertex* v = find_vertex_mut(m_vertices, m_boxdrag.v_ids[i])) {
            const ImVec2 q0 { m_boxdrag.v_orig[i].x - P.x, m_boxdrag.v_orig[i].y - P.y };
            const ImVec2 q1 { q0.x * sx,                  q0.y * sy };
            v->x = P.x + q1.x;
            v->y = P.y + q1.y;
        }
    }


    //      5.      End gesture
    if ( !io.MouseDown[ImGuiMouseButton_Left] )
    {
        m_boxdrag.active = false;
        m_boxdrag.first_frame = true;
    }
    else
    {
        m_boxdrag.first_frame = false;
    }
    
    return;
}



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "BOUNDING BOX".






// *************************************************************************** //
//
//
//
//      5.      LASSO TOOL...
// *************************************************************************** //
// *************************************************************************** //

//  "_start_lasso_tool"
//
//      Only begins if the user clicks on empty canvas (no object hit).
//      Unless Shift or Ctrl is held, any prior selection is cleared.
//
void Editor::_start_lasso_tool(void)
{
    ImGuiIO &       io      = ImGui::GetIO();
    
    m_lasso_active          = true;
    m_lasso_start           = io.MousePos;                      // in screen coords
    m_lasso_end             = m_lasso_start;
    
    if ( !io.KeyShift && !io.KeyCtrl )      { this->reset_selection(); } //m_sel.clear();  // fresh selection

    return;
}


//  "_update_lasso"
//
//      While the mouse button is held, draw the translucent rectangle.
//      When the button is released, convert the rect to world space
//      and add all intersecting objects to the selection set.
//
void Editor::_update_lasso(const Interaction & it)
{
    ImGuiIO &   io      = ImGui::GetIO();
    m_lasso_end         = io.MousePos;

    // Visual feedback rectangle
    it.dl->AddRectFilled(m_lasso_start, m_lasso_end, m_style.COL_LASSO_FILL);
    it.dl->AddRect      (m_lasso_start, m_lasso_end, m_style.COL_LASSO_OUT);

    // Finalise on mouse-up
    if ( !ImGui::IsMouseDown(ImGuiMouseButton_Left) )
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

            const Path * pp = parent_path_of_vertex(m_points[i].v);
            if ( !pp || !pp->IsMutable() ) continue;          // NEW guard

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

        // ---------- Paths ----------
        for (size_t pi = 0; pi < m_paths.size(); ++pi)
        {
            const Path& p = m_paths[pi];
            if ( !p.IsMutable() )   { continue; }                      // NEW guard

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
    
    return;
}



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "LASSO TOOL".






// *************************************************************************** //
//
//
//
//      X.      NEW STUFF...
// *************************************************************************** //
// *************************************************************************** //

//  "_drag_threshold_px"
//
inline float Editor::_drag_threshold_px(void) const
{
    ImGuiIO &       io      = ImGui::GetIO();
    const float &   s       = m_style.DRAG_START_DIST_PX;     // define in your style; default 0
    
    return (s > 0.0f)
                ? s
                : io.MouseDragThreshold;
}


//  "_press_inside_selection"
//
//      True iff the press position was inside the current selection's bbox.
//      Keep cheap; used only to allow "press inside selection → drag whole selection".
//
inline bool Editor::_press_inside_selection(const ImVec2 press_ws) const {
    ImVec2  tl{}, br{};
    
    if ( m_sel.empty() )                { return false; }
    
    if ( !_selection_bounds(tl, br, this->m_render_ctx) )   { return false; }
    
    return (press_ws.x >= tl.x && press_ws.x <= br.x &&
            press_ws.y >= tl.y && press_ws.y <= br.y);
}


//  "_hit_is_in_current_selection"
//      True iff the hit corresponds to something already in the selection
//
inline bool Editor::_hit_is_in_current_selection(const Hit & hit) const
{
    switch (hit.type)
    {
        //      1.      "HANDLE"...
        case Hit::Type::Handle:
        {
            // Handle stores the vertex id in index; treat "selected vertex" as selected.
            const uint32_t vid = static_cast<uint32_t>(hit.index);
            return m_sel.vertices.count(vid) != 0;
        }
        //
        //      2.      "VERTEX"...
        case Hit::Type::Vertex:
        {
            const size_t idx = hit.index;
            
            if ( m_sel.points.count(idx) )  { return true; }
            
            if ( idx < m_points.size() )
            {
                const uint32_t vid = m_points[idx].v;
                if ( m_sel.vertices.count(vid) )    { return true; }
            }
            return false;
        }
        //
        //      3.      "EDGE"  *OR*  "SURFACE"...
        case Hit::Type::Edge:
        case Hit::Type::Surface: {
            return (m_sel.paths.find(static_cast<size_t>(hit.index)) != m_sel.paths.end());
        }
        //
        //      DEFAULT.      ASSERTION FAILURE...
        default : { IM_ASSERT( false ); }
    }
    
    return false;
}



//
//
//
// *************************************************************************** //
// *************************************************************************** //	END "NEW".











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

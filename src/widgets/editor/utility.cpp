/***********************************************************************************
*
*       *********************************************************************
*       ****            U T I L I T Y . C P P  ____  F I L E             ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      June 14, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "widgets/editor/editor.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



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


//  "_endpoint_if_open"
//
std::optional<Editor::EndpointInfo> Editor::_endpoint_if_open(uint32_t vid) const
{
    for (size_t i = 0; i < m_paths.size(); ++i) {
        const Path& p = m_paths[i];
        if (p.closed || p.verts.empty()) continue;
        if (p.verts.front() == vid) return EndpointInfo{ i, /*prepend=*/true  };
        if (p.verts.back () == vid) return EndpointInfo{ i, /*prepend=*/false };
    }
    return std::nullopt;
}



// *************************************************************************** //
//
//
//  3.  DATA MODIFIER UTILITIES...
// *************************************************************************** //
// *************************************************************************** //

//  "_add_point_glyph"
//
void Editor::_add_point_glyph(uint32_t vid)
{
    PointStyle ps;
    m_points.push_back({ vid, ps });
}


//  "_add_vertex"
//
uint32_t Editor::_add_vertex(ImVec2 w) {
    w = _grid_snap(w);                       // <- snap if enabled
    m_vertices.push_back({ m_next_id++, w.x, w.y });
    return m_vertices.back().id;
}


//  "_add_point"
//
void Editor::_add_point(ImVec2 w)
{
    uint32_t vid = _add_vertex(w);
    m_points.push_back({ vid, { COL_POINT_DEFAULT, DEFAULT_POINT_RADIUS, true } });
}


//  "_erase_vertex_and_fix_paths"
//
void Editor::_erase_vertex_and_fix_paths(uint32_t vid)
{
    /* a) erase vertex record ------------------------------------------- */
    m_vertices.erase(std::remove_if(m_vertices.begin(), m_vertices.end(),
                   [vid](const Pos& v){ return v.id == vid; }),
                   m_vertices.end());

    /* b) remove this ID from every path; drop paths < 2 verts ---------- */
    for (size_t i = 0; i < m_paths.size(); /*++i done inside*/)
    {
        Path& p = m_paths[i];
        p.verts.erase(std::remove(p.verts.begin(), p.verts.end(), vid),
                      p.verts.end());

        if (p.verts.size() < 2)
            m_paths.erase(m_paths.begin() + static_cast<long>(i));
        else
            ++i;
    }

    /* c) remove any glyph referencing the vertex ----------------------- */
    m_points.erase(std::remove_if(m_points.begin(), m_points.end(),
                  [vid](const Point& pt){ return pt.v == vid; }),
                  m_points.end());
}


//  "_erase_path_and_orphans"
//
void Editor::_erase_path_and_orphans(size_t pidx)
{
    if ( pidx >= m_paths.size() ) return;

    // 1. move-out the doomed path so we still have its vertex IDs
    Path doomed = std::move(m_paths[pidx]);
    m_paths.erase(m_paths.begin() + pidx);

    // 2. collect every vertex still used anywhere
    std::unordered_set<uint32_t> still_used;
    for (const Path& p : m_paths)
        for (uint32_t vid : p.verts)
            still_used.insert(vid);

    // 3. any vertex unique to the deleted path gets fully erased
    for (uint32_t vid : doomed.verts)
        if (!still_used.count(vid))
            _erase_vertex_and_fix_paths(vid);      // your existing helper
            
    return;
}




// *************************************************************************** //
//
//
//
//  3.  APP UTILITY OPERATIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_try_begin_handle_drag"
//
bool Editor::_try_begin_handle_drag(const Interaction& it)
{
    if (m_dragging_handle ||
        !ImGui::IsMouseClicked(ImGuiMouseButton_Left) || !it.hovered)
        return false;

    if (auto h = _hit_any(it); h && h->type == Hit::Type::Handle)
    {
        m_dragging_handle = true;
        m_drag_vid        = h->index;      // vertex-id
        m_dragging_out    = h->out;
        return true;                       // we’re in drag state
    }

    /* Alt-click fallback (pull new handle) --------------------------- */
    if (alt_down()) {
        if (auto h = _hit_any(it); h && h->type == Hit::Type::Point)
        {
            m_dragging_handle = true;
            m_drag_vid        = m_points[h->index].v;
            m_dragging_out    = true;      // always out first
            if (Vertex* v = find_vertex_mut(m_vertices, m_drag_vid))
                v->out_handle = {0,0};
            return true;
        }
    }
    return false;
}


//  "_scissor_cut"
//      Core cut routine: split a Path at the hit position
//
void Editor::_scissor_cut(const PathHit& h)
{
    Path& path = m_paths[h.path_idx];
    const size_t insert_pos = h.seg_idx + 1;          // after the hit segment’s i-th vertex

    // 1. two coincident vertices: one for each side of the cut
    uint32_t vid_left  = _add_vertex(h.pos_ws);   // will live in the original path
    _add_point_glyph(vid_left);

    uint32_t vid_right = _add_vertex(h.pos_ws);   // goes into the new right-hand path
    _add_point_glyph(vid_right);

    // 2. insert the LEFT vertex into the original path
    path.verts.insert(path.verts.begin() + insert_pos, vid_left);
    path.closed = false;                          // guarantee it’s now open

    // 3. build the RIGHT-hand path
    Path right;
    right.style  = path.style;                    // clone stroke
    right.closed = false;

    // first vertex is the RIGHT duplicate
    right.verts.push_back(vid_right);

    // then everything *after* the left vertex
    right.verts.insert(right.verts.end(),
                       path.verts.begin() + insert_pos + 1,
                       path.verts.end());

    // 4. trim the original (left) path so it ends at vid_left
    path.verts.erase(path.verts.begin() + insert_pos + 1, path.verts.end());

    // 5. store the new path
    m_paths.push_back(std::move(right));

    // NOTE: Bézier handle subdivision is still “TODO” —
    //       handles on vid_left / vid_right are zeroed by _add_vertex,
    //       so curvature continuity is lost for now.
}


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

        // Convert to world space (inverse pan + zoom)
        ImVec2 tl_w{ (tl_scr.x - it.origin.x) / m_cam.zoom_mag,
                     (tl_scr.y - it.origin.y) / m_cam.zoom_mag };
        ImVec2 br_w{ (br_scr.x - it.origin.x) / m_cam.zoom_mag,
                     (br_scr.y - it.origin.y) / m_cam.zoom_mag };

        bool additive = io.KeyShift || io.KeyCtrl;
        if (!additive)                       // fresh selection if no modifier
            m_sel.clear();

        /* ---------- Points ---------- */
        for (size_t i = 0; i < m_points.size(); ++i)
        {
            const Pos * v = find_vertex(m_vertices, m_points[i].v);
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
    
    
    return;
}
    
    
    
// *************************************************************************** //
//
//
//
//      **OLD**     LOCAMOTION UTILITY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_update_world_extent"
//
void Editor::_update_world_extent()
{
    if (m_vertices.empty()) {
        m_world_bounds = { 0,0,  0,0 };
        return;
    }

    float   min_x   =  std::numeric_limits<float>::max();
    float   min_y   =  std::numeric_limits<float>::max();
    float   max_x   = -std::numeric_limits<float>::max();
    float   max_y   = -std::numeric_limits<float>::max();

    for (const Vertex & v : m_vertices) {
        min_x   = std::min(min_x, v.x);
        min_y   = std::min(min_y, v.y);
        max_x   = std::max(max_x, v.x);
        max_y   = std::max(max_y, v.y);
    }

    const float margin =  m_grid.world_step * 4.0f;     // breathing room
    m_world_bounds     = { min_x - margin,  min_y - margin,
                           max_x + margin,  max_y + margin };
}


//  "_zoom_canvas"
/*
void Editor::_zoom_canvas(const Interaction& it)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.MouseWheel == 0.0f || ImGui::IsMouseDown(ImGuiMouseButton_Left) ) return;

    ImVec2 scr_anchor = it.hovered ? it.canvas
                                   : ImVec2(m_avail.x * 0.5f, m_avail.y * 0.5f);

    ImVec2 world_anchor{
        (scr_anchor.x + m_cam.pan.x) / m_cam.zoom_mag,
        (scr_anchor.y + m_cam.pan.y) / m_cam.zoom_mag
    };

    float new_zoom = m_cam.zoom_mag * (1.f + io.MouseWheel * 0.10f);
    _clamp_zoom(new_zoom);                 // respects min/max zoom

    m_cam.pan.x = world_anchor.x * new_zoom - scr_anchor.x;
    m_cam.pan.y = world_anchor.y * new_zoom - scr_anchor.y;

    _clamp_scroll();                       // apply symmetric limits
    m_cam.zoom_mag = new_zoom;
}*/


//  "_clamp_scroll"
//
void Editor::_clamp_scroll(void)
{
    const float pad_x  =  m_avail.x * 0.5f;        // half‑viewport slack
    const float pad_y  =  m_avail.y * 0.5f;

    const float wl_px  =  m_world_bounds.min_x * m_cam.zoom_mag;   // world‑left  in px
    const float wr_px  =  m_world_bounds.max_x * m_cam.zoom_mag;   // world‑right in px
    const float wt_px  =  m_world_bounds.min_y * m_cam.zoom_mag;   // world‑top   in px
    const float wb_px  =  m_world_bounds.max_y * m_cam.zoom_mag;   // world‑bot   in px

    // X‑axis limits
    float min_scroll_x = wl_px - pad_x;
    float max_scroll_x = wr_px - m_avail.x + pad_x;
    if ((wr_px - wl_px) <= m_avail.x) {                       // world narrower than view
        const float centre = (wl_px + wr_px - m_avail.x) * 0.5f;
        min_scroll_x = centre - pad_x;
        max_scroll_x = centre + pad_x;
    }

    // Y‑axis limits
    float min_scroll_y = wt_px - pad_y;
    float max_scroll_y = wb_px - m_avail.y + pad_y;
    if ((wb_px - wt_px) <= m_avail.y) {                       // world shorter than view
        const float centre = (wt_px + wb_px - m_avail.y) * 0.5f;
        min_scroll_y = centre - pad_y;
        max_scroll_y = centre + pad_y;
    }

    m_cam.pan.x   = std::clamp(m_cam.pan.x, min_scroll_x, max_scroll_x);
    m_cam.pan.y   = std::clamp(m_cam.pan.y, min_scroll_y, max_scroll_y);
}


//  "_clamp_zoom"
//
void Editor::_clamp_zoom(float& target_zoom)
{
    const float viewFitX = m_avail.x / m_world_extent.x;
    const float viewFitY = m_avail.y / m_world_extent.y;
    const float min_zoom = std::max(viewFitX, viewFitY) * 0.25f;   // let user shrink to 25 % of “fit”
    const float max_zoom = 32.0f;                                  // arbitrary upper bound
    target_zoom = std::clamp(target_zoom, min_zoom, max_zoom);
}












// *************************************************************************** //
//
//
//
//      MISC. UTILITIES...
// *************************************************************************** //
// *************************************************************************** //

//  "_draw_controls"
//
void Editor::_draw_controls(void)
{
    static constexpr const char *   uuid            = "##Editor_Controls_Columns";
    static constexpr int            NC              = 8;
    static ImGuiOldColumnFlags      COLUMN_FLAGS    = ImGuiOldColumnFlags_None;
    static ImVec2                   WIDGET_SIZE     = ImVec2( -1,  32 );
    static ImVec2                   BUTTON_SIZE     = ImVec2( 32,   WIDGET_SIZE.y );
    //
    constexpr ImGuiButtonFlags      BUTTON_FLAGS    = ImGuiOldColumnFlags_NoPreserveWidths;
    int                             mode_i          = static_cast<int>(m_mode);
    
   
   
    //  BEGIN COLUMNS...
    //
    ImGui::Columns(NC, uuid, COLUMN_FLAGS);
    //
    //
    //
        //  1.  EDITOR STATE...
        ImGui::Text("State:");
        //
        ImGui::SetNextItemWidth( WIDGET_SIZE.x );
        if ( ImGui::Combo("##Editor_Controls_EditorState",      &mode_i,
                          ms_MODE_LABELS.data(),                static_cast<int>(Mode::Count)) )
        {
            m_mode = static_cast<Mode>(mode_i);
        }
    
    
    
        //  2.  GRID VISIBILITY...
        ImGui::NextColumn();
        ImGui::Text("Show Grid:");
        //
        ImGui::SetNextItemWidth( BUTTON_SIZE.x );
        ImGui::Checkbox("##Editor_Controls_ShowGrid",           &m_grid.visible);



        //  3.  SNAP-TO-GRID...
        ImGui::NextColumn();
        ImGui::Text("Snap-To-Grid:");
        //
        ImGui::Checkbox("##Editor_Controls_SnapToGrid",         &m_grid.snap_on);
        
        
        
        //  4.  GRID-LINE DENSITY...
        ImGui::NextColumn();
        ImGui::Text("Grid Density:");
        //
        //
        //
        if ( ImGui::ArrowButtonEx("##Editor_Controls_GridDensityDown",      ImGuiDir_Down,
                          BUTTON_SIZE,                                      BUTTON_FLAGS) )
        {
            m_grid.world_step *= 2.f;
        }
        //
        ImGui::SameLine(0.0f, 0.0f);
        //
        if ( ImGui::ArrowButtonEx("##Editor_Controls_GridDensityUp",        ImGuiDir_Up,
                          BUTTON_SIZE,                                      BUTTON_FLAGS) )
        {
            m_grid.world_step = std::max(ms_GRID_STEP_MIN, m_grid.world_step * 0.5f);
        }
        //
        ImGui::SameLine();
        //
        ImGui::Text("(%.1f)", m_grid.world_step);



        //  5.  CANVAS SETTINGS...
        ImGui::NextColumn();
        //ImGui::Text("##Editor_Controls_CanvasSettings");
        ImGui::Text("");
        //
        //
        if ( ImGui::Button("Canvas Settings", WIDGET_SIZE) ) {
            ImGui::OpenPopup("Editor_CanvasSettingsPopup");
        }
        if ( ImGui::BeginPopup("Editor_CanvasSettingsPopup") ) {
            this->_display_canvas_settings();
            ImGui::EndPopup();
        }


        
        //  6.  EMPTY SPACES FOR LATER...
        for (int i = ImGui::GetColumnIndex(); i < NC - 1; ++i) {
            ImGui::Dummy( ImVec2(0,0) );    ImGui::NextColumn();
        }



        //  X.  CLEAR ALL...
        //ImGui::NextColumn();
        //ImGui::TextUnformatted("##Editor_Controls_ClearCanvas");
        ImGui::Text("");
        if ( ImGui::Button("Clear", WIDGET_SIZE) ) {
            _clear_all();
        }
    //
    //
    //
    ImGui::Columns(1);      //  END COLUMNS...
    
    
    return;
}


//  "_display_canvas_settings"
//
void Editor::_display_canvas_settings(void)
{
    ImGui::NewLine();
    
    ImGui::DragFloat2("World extent", &m_world_extent.x,
                  10.0f, 100.0f, 5000.0f, "%.0f");
                  
    ImGui::NewLine();
                
    return;
}








// *************************************************************************** //
//
//
//  OTHER MISC...
// *************************************************************************** //
// *************************************************************************** //

//  "_clear_all"
//
void Editor::_clear_all(void)
{
    const size_t    N       = this->m_paths.size();
    
    
    //  1.  CLEAR ALL EDITOR DATA...
    for (size_t idx = 0; idx < N; ++idx) {
        _erase_path_and_orphans(idx);  // ← replaces direct m_paths.erase()
    }

    m_vertices.clear();
    m_points.clear();
    m_lines.clear();
    m_sel.clear();
    
    
    //  2.  RESET EDITOR STATE...
    m_lasso_active  = false;
    m_dragging      = false;
    m_drawing       = false;
    m_next_id       = 1;
    m_pen           = {};
    
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

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












// *************************************************************************** //
//
//
//  2.  DATA MODIFIER UTILITIES...
// *************************************************************************** //
// *************************************************************************** //

//  "_add_vertex"
//
uint32_t Editor::_add_vertex(ImVec2 w)
{
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






// *************************************************************************** //
//
//
//
//  3.  APP UTILITY OPERATIONS...
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
//      LOCAMOTION UTILITY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_world_from_screen"
//



//  "_zoom_canvas"
//
void Editor::_zoom_canvas(const Interaction& it)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.MouseWheel == 0.0f) return;

    // 1) pick anchor
    ImVec2 scr_anchor = it.hovered ? it.canvas
                                   : ImVec2(m_avail.x * 0.5f, m_avail.y * 0.5f);

    // 2) world coord under anchor before zoom
    ImVec2 world_anchor { (scr_anchor.x + m_scroll.x) / m_zoom,
                          (scr_anchor.y + m_scroll.y) / m_zoom };

    // 3) new zoom (±10 %)
    float new_zoom = m_zoom * (1.f + io.MouseWheel * 0.10f);
    _clamp_zoom(new_zoom);

    // 4) re-compute scroll so anchor stays pinned
    m_scroll.x = world_anchor.x * new_zoom - scr_anchor.x;
    m_scroll.y = world_anchor.y * new_zoom - scr_anchor.y;
    m_zoom     = new_zoom;

    _clamp_scroll();        // keep inside finite canvas
}


void Editor::_clamp_scroll()
{
    const float max_x = std::max(0.f, m_world_extent.x * m_zoom - m_avail.x);
    const float max_y = std::max(0.f, m_world_extent.y * m_zoom - m_avail.y);
    m_scroll.x = std::clamp(m_scroll.x, 0.f, max_x);
    m_scroll.y = std::clamp(m_scroll.y, 0.f, max_y);
}


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

/* enum ImGuiOldColumnFlags_
{
    ImGuiOldColumnFlags_None                    = 0,
    ImGuiOldColumnFlags_NoBorder                = 1 << 0,   // Disable column dividers
    ImGuiOldColumnFlags_NoResize                = 1 << 1,   // Disable resizing columns when clicking on the dividers
    ImGuiOldColumnFlags_NoPreserveWidths        = 1 << 2,   // Disable column width preservation when adjusting columns
    ImGuiOldColumnFlags_NoForceWithinWindow     = 1 << 3,   // Disable forcing columns to fit within window
    ImGuiOldColumnFlags_GrowParentContentsSize  = 1 << 4,   // Restore pre-1.51 behavior of extending the parent window contents size but _without affecting the columns width at all_. Will eventually remove.
};*/


//  "_draw_controls"
//
void Editor::_draw_controls(void)
{
    static constexpr const char *   uuid            = "##Editor_Controls_Columns";
    static constexpr int            NC              = 8;
    static ImGuiOldColumnFlags      column_flags    = ImGuiOldColumnFlags_None;
    static ImVec2                   WIDGET_SIZE     = ImVec2( 160,  32 );
    static ImVec2                   BUTTON_SIZE     = ImVec2( 32,   WIDGET_SIZE.y );
    //
    constexpr ImGuiButtonFlags      BUTTON_FLAGS    = ImGuiButtonFlags_None;
    int                             mode_i          = static_cast<int>(m_mode);
    
   
   
    //  BEGIN COLUMNS...
    //
    ImGui::Columns(NC, uuid, column_flags);
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
        //
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
}//   END OF "cb" NAMESPACE.











// *************************************************************************** //
// *************************************************************************** //
//
//  END.

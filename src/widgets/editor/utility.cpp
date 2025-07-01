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
Editor::Vertex * Editor::find_vertex(std::vector<Vertex>& verts, uint32_t id) {
    for (auto & v : verts) if (v.id == id) return &v; return nullptr;
}


//  "find_vertex"
//
const Editor::Vertex * Editor::find_vertex(const std::vector<Vertex>& verts, uint32_t id) const {
    for (auto & v : verts) if (v.id == id) return &v; return nullptr;
}


//  "_endpoint_if_open"
//
std::optional<Editor::EndpointInfo> Editor::_endpoint_if_open(uint32_t vid) const
{
    const PathID    N   = static_cast<PathID>( m_paths.size() );
    
    for (PathID i = 0; i < N; ++i)
    {
        const Path &    p   = m_paths[i];
        
        if ( p.closed || p.verts.empty() )      { continue; }
        if ( p.verts.front() == vid )           { return EndpointInfo{ i, /*prepend=*/true  };  }
        if ( p.verts.back () == vid )           { return EndpointInfo{ i, /*prepend=*/false };  }
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
    w = snap_to_grid(w);                       // <- snap if enabled
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
                   [vid](const Vertex& v){ return v.id == vid; }),
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

    const float margin =  m_grid.snap_step * 4.0f;     // breathing room
    m_world_bounds     = { min_x - margin,  min_y - margin,
                           max_x + margin,  max_y + margin };
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
            m_grid.snap_step *= 2.f;
        }
        //
        ImGui::SameLine(0.0f, 0.0f);
        //
        if ( ImGui::ArrowButtonEx("##Editor_Controls_GridDensityUp",        ImGuiDir_Up,
                          BUTTON_SIZE,                                      BUTTON_FLAGS) )
        {
            m_grid.snap_step = std::max(ms_GRID_STEP_MIN, m_grid.snap_step * 0.5f);
        }
        //
        ImGui::SameLine();
        //
        ImGui::Text("(%.1f)", m_grid.snap_step);



        //  5.  CANVAS SETTINGS...
        ImGui::NextColumn();
        //ImGui::Text("##Editor_Controls_CanvasSettings");
        ImGui::Text("");
        //
        //
        if (ImGui::Button("Settings", WIDGET_SIZE))
            ImGui::OpenPopup("Editor_Canvas_SettingsPopup");

        if (ImGui::BeginPopup("Editor_Canvas_SettingsPopup"))
        {
            this->_draw_system_preferences();      // << new helper
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



struct FileDialogState {
    std::filesystem::path cwd = std::filesystem::current_path();
    std::string           name_buf;          // editable file name
    int                   selected = -1;     // index in entries[]
    std::vector<std::filesystem::directory_entry> entries;
};

/// Call inside a modal popup. Returns chosen path on OK, std::nullopt on cancel.
inline std::optional<std::string>
file_dialog(FileDialogState& st, bool save_mode)
{
    // refresh listing if first time or cwd changed
    if (st.entries.empty() || !std::filesystem::exists(st.cwd)) {
        st.cwd = std::filesystem::current_path();
    }
    if (st.entries.empty()) {
        st.entries.clear();
        for (auto& e : std::filesystem::directory_iterator(st.cwd))
            st.entries.push_back(e);
        std::sort(st.entries.begin(), st.entries.end(),
                  [](auto& a, auto& b){ return a.path().filename() < b.path().filename(); });
    }

    ImGui::TextUnformatted(st.cwd.string().c_str());
    ImGui::Separator();

    ImGui::BeginChild("##file_list", ImVec2(0, 200), true);
    if (ImGui::Selectable("..", false))
        st.cwd = st.cwd.parent_path(), st.entries.clear(), st.selected = -1;

    for (int i = 0; i < (int)st.entries.size(); ++i) {
        const auto& e = st.entries[i];
        bool is_dir   = e.is_directory();
        std::string label = is_dir ? "[D] " : "    ";
        label += e.path().filename().string();

        if (ImGui::Selectable(label.c_str(), st.selected == i)) {
            st.selected = i;
            if (!is_dir) st.name_buf = e.path().filename().string();
        }
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0) && is_dir) {
            st.cwd = e;
            st.entries.clear();
            st.selected = -1;
        }
    }
    ImGui::EndChild();

    // filename field
    ImGui::InputText("File name", &st.name_buf);

    // buttons
    bool ok = false, cancel = false;
    if (ImGui::Button(save_mode ? "Save" : "Open"))
        ok = true;
    ImGui::SameLine();
    if (ImGui::Button("Cancel"))
        cancel = true;

    if (ok && !st.name_buf.empty()) {
        auto chosen = (st.cwd / st.name_buf).string();
        st = {};                         // reset for next time
        return chosen;
    }
    if (cancel) {
        st = {};
        return std::nullopt;
    }
    return std::nullopt;                // dialog still active
}



//  "_draw_system_preferences"
//
void Editor::_draw_system_preferences(void)
{
    static cb::FileDialog dlg{cb::FileDialog::Type::Save};
    //static FileDialogState saveDlg, loadDlg;


    if (ImGui::Button("Save…"))
        dlg.open();                       // start at current directory

    if (dlg.is_open())
        if (dlg.draw("SaveDlg")) {        // returns true when finished
            if (auto path = dlg.result())
                save_async(*path);        // your own handler
        }


        //ImGui::SameLine(0,20);


        //  // ------------------- LOAD -------------------
        //  if (ImGui::Button("Load…"))
        //      ImGui::OpenPopup("LoadDlg");

        //  if (ImGui::BeginPopupModal("LoadDlg", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        //  {
        //      if (auto res = file_dialog(loadDlg, /*save_mode=*/false)) {
        //          if (res) load_async(*res);        // user clicked OK
        //          ImGui::CloseCurrentPopup();
        //      }
        //      ImGui::EndPopup();
        //  }



        // status
        if (!m_io_msg.empty())
            ImGui::TextDisabled("%s", m_io_msg.c_str());
    

    // (existing Canvas/Grid prefs below …)
    return;
}






// *************************************************************************** //
//
//
//      4.  RESIDENT UTILITY STUFF...
// *************************************************************************** //
// *************************************************************************** //

//  "_draw_shape_resident"
//
void Editor::_draw_shape_resident(void)
{
    int kind_idx = static_cast<int>(m_shape.kind);



    ImGui::TextUnformatted("Shape Tool");
    //
    //
    //
    //  1.  DROP-DOWN FOR SHAPE TYPE.
    ImGui::Separator();
    if (ImGui::Combo( "##Editor_Shape_ShapeType",
                      &kind_idx,
                      this->ms_SHAPE_NAMES.data(),                          //  Names table
                      static_cast<int>(ShapeKind::Count)) )                 //  Item count
    {
        m_shape.kind = static_cast<ShapeKind>(kind_idx);
    }
    //
    //  2.  SLIDER FOR SHAPE RADIUS...
    switch (m_shape.kind)
    {
        //  2.1.    Shapes with only ONE Variable (Radius).
        case ShapeKind::Square      :
        case ShapeKind::Circle      :
        case ShapeKind::Oval        :
        case ShapeKind::Ellipse     : {
            this->_draw_shape_resident_default();
            break;
        }
        
        case ShapeKind::Rectangle   : {
            break;
        }
        
        default                     : {
            break;
        }
    
    }
    ImGui::SliderFloat("Radius", &m_shape.radius, 1.0f, 100.0f, "%.2f");
           
           
      
           
    return;
}


//  "_draw_shape_default_resident"
//
void Editor::_draw_shape_resident_default(void)
{

    ImGui::SliderFloat("Radius", &m_shape.radius, 1.0f, 100.0f, "%.2f");

    
    return;
}


//  "_draw_shape_resident_multi"
//      For drawing shapes that require more than one variable to describe.
//
void Editor::_draw_shape_resident_multi(void)
{
    ImGui::SliderFloat("R", &m_shape.radius, 1.0f, 100.0f, "%.2f");
    ImGui::SliderFloat("S", &m_shape.radius, 1.0f, 100.0f, "%.2f");
    return;
}



//  "_draw_shape_resident_custom"
//      For drawing shapes that require more than one variable to describe.
//
void Editor::_draw_shape_resident_custom(void)
{
    ImGui::SliderFloat("R", &m_shape.radius, 1.0f, 100.0f, "%.2f");
    ImGui::SliderFloat("S", &m_shape.radius, 1.0f, 100.0f, "%.2f");
    return;
}










//      SELECTION RESIDENT...
// *************************************************************************** //
// *************************************************************************** //

//  "_draw_selection_resident"
//
void Editor::_draw_selection_resident(void)
{
    ImGui::TextUnformatted("Selection");
    ImGui::Separator();
           
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

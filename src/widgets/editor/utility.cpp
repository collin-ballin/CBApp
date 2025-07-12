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
Editor::Vertex * Editor::find_vertex(std::vector<Vertex> & verts, VertexID id) {
    for (auto & v : verts) if (v.id == id) return &v; return nullptr;
}


//  "find_vertex"
//
const Editor::Vertex * Editor::find_vertex(const std::vector<Vertex> & verts, VertexID id) const {
    for (auto & v : verts) if (v.id == id) return &v; return nullptr;
}


//  "_endpoint_if_open"
//
std::optional<Editor::EndpointInfo> Editor::_endpoint_if_open(VertexID vid) const
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
void Editor::_add_point_glyph(VertexID vid)
{
    PointStyle ps;
    m_points.push_back({ vid, ps });
}


//  "_add_vertex"
//
Editor::VertexID Editor::_add_vertex(ImVec2 w) {
    w = snap_to_grid(w);                       // <- snap if enabled
    m_vertices.push_back({ m_next_id++, w.x, w.y });
    return m_vertices.back().id;
}


//  "_add_point"
//
void Editor::_add_point(ImVec2 w)
{
    VertexID vid = _add_vertex(w);
    m_points.push_back({ vid, { m_style.COL_POINT_DEFAULT, m_style.DEFAULT_POINT_RADIUS, true } });
}


//  "_erase_vertex_and_fix_paths"
//
void Editor::_erase_vertex_and_fix_paths(VertexID vid)
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
void Editor::_erase_path_and_orphans(PathID pidx)
{
    if ( pidx >= m_paths.size() ) return;

    //  1.  Move-out the doomed path so we still have its vertex IDs
    Path doomed = std::move(m_paths[pidx]);
    m_paths.erase(m_paths.begin() + pidx);

    //  2.  Collect every vertex still used anywhere
    std::unordered_set<VertexID> still_used;
    for (const Path & p : m_paths)
        for (VertexID vid : p.verts)
            still_used.insert(vid);

    //  3.  Any vertex unique to the deleted path gets fully erased
    for (VertexID vid : doomed.verts)
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
        m_drag_vid        = static_cast<VertexID>( h->index );      // vertex-id
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
void Editor::_scissor_cut(const PathHit & h)
{
    Path&  leftPath   = m_paths[h.path_idx];            // original path
    size_t insert_pos = h.seg_idx + 1;                  // after vertex i

    // ─── 1.  Two coincident vertices at the cut position ─────────────
    VertexID vid_left  = _add_vertex(h.pos_ws);         // stays in left path
    _add_point_glyph(vid_left);

    VertexID vid_right = _add_vertex(h.pos_ws);         // first in right path
    _add_point_glyph(vid_right);

    // ─── 2.  Insert left vertex and open the left path ──────────────
    leftPath.verts.insert(leftPath.verts.begin() + insert_pos, vid_left);
    leftPath.closed = false;

    // ─── 3.  Build right-hand path ──────────────────────────────────
    Path   rightPath;                                   // new container
    rightPath.id         = m_next_pid++;                // NEW unique ID
    rightPath.set_default_label(rightPath.id);          // "Path N"

    rightPath.style      = leftPath.style;              // copy appearance
    rightPath.z_index    = leftPath.z_index;
    rightPath.locked     = leftPath.locked;
    rightPath.visible    = leftPath.visible;
    rightPath.closed     = false;

    //  First vertex in right path is vid_right
    rightPath.verts.push_back(vid_right);

    //  Then every vertex AFTER vid_left in the original
    rightPath.verts.insert(rightPath.verts.end(),
                           leftPath.verts.begin() + insert_pos + 1,
                           leftPath.verts.end());

    // ─── 4.  Trim the left path so it ends at vid_left ──────────────
    leftPath.verts.erase(leftPath.verts.begin() + insert_pos + 1,
                         leftPath.verts.end());

    // ─── 5.  Store the new path ─────────────────────────────────────
    m_paths.push_back(std::move(rightPath));

    // NOTE: Bézier handle subdivision still “TODO”.
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
    
    this->S.PushFont( Font::Small );
   
    
    
   
   
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
        ImGui::NextColumn(); ImGui::NewLine();
        if ( ImGui::Button("Settings", WIDGET_SIZE) ) {
            ui::open_preferences_popup( "Editor System Preferences", [this](popup::Context & ctx) { _draw_editor_settings(ctx); } );
        }


        
        //  6.  EMPTY SPACES FOR LATER...
        for (int i = ImGui::GetColumnIndex(); i < NC - 1; ++i) {
            ImGui::Dummy( ImVec2(0,0) );    ImGui::NextColumn();
        }



        //  X.  CLEAR ALL...
        //ImGui::NextColumn();
        //ImGui::TextUnformatted("##Editor_Controls_ClearCanvas");
        ImGui::NewLine();
        if ( ImGui::Button("Clear", WIDGET_SIZE) ) {
            ui::ask_ok_cancel( "Ask Ok Cancel",
                               "This will erase all content on the plot.\nAre you sure you want to continue?",
                               [this]{ _clear_all(); } );
        }
        popup::Draw();
    //
    //
    //
    ImGui::Columns(1);      //  END COLUMNS...
    
    
    this->S.PopFont();
   
    return;
}


//  "_draw_editor_settings"
//
void Editor::_draw_editor_settings([[maybe_unused]] popup::Context & ctx)
{
        
    //  1.  EDITOR SETTINGS...
    ImGui::SeparatorText("Mechanics");
    this->_draw_settings_mechanics();



    //  2.  SAVE/LOAD SERIALIZATION...
    ImGui::SeparatorText("Serialization");
    this->_draw_settings_serialize();
    
    
    
    //  3.  USER PREFERENCES...
    ImGui::SeparatorText("User Preferences");
    this->_draw_settings_user_preferences();
    
    
    
    return;
}


// *************************************************************************** //
//
//
//      HELPER FUNCTIONS FOR EDITOR SETTINGS...
// *************************************************************************** //
// *************************************************************************** //

//  "_draw_settings_mechanics"
//
void Editor::_draw_settings_mechanics(void)
{
    return;
}

 
//  "_draw_settings_serialize"
//
void Editor::_draw_settings_serialize(void)
{
    using                       Type            = cb::FileDialog::Type;
    using                       Initializer     = cb::FileDialog::Initializer;
    static Initializer          save_data       = {
        /* starting_dir       = */  std::filesystem::current_path(),
        /* default_filename   = */  "canvas settings",
        /* required_extension = */  ".json",
        /* valid_extensions   = */  {".json", ".txt"}
    };
    static Initializer          open_data       = {
        /* starting_dir       = */  std::filesystem::current_path(),
        /* default_filename   = */  "",
        /* required_extension = */  "",
        /* valid_extensions   = */  {".json", ".txt"}
    };
    static cb::FileDialog       save_dialog;
    static cb::FileDialog       open_dialog;


    //  1.  SAVE DIALOGUE...
    if ( ImGui::Button("Save") )    { save_dialog.initialize(Type::Save, save_data ); }
    //
    if ( save_dialog.is_open() )
    {
        if ( save_dialog.Begin("Save Editor Session") ) {        // returns true when finished
            if ( auto path = save_dialog.result() )
                { save_async( *path ); }        // your own handler
        }
    }


    ImGui::SameLine(0,20);
    
    
    //  2.  LOAD DIALOGUE...
    if ( ImGui::Button("Open") )    { open_dialog.initialize(Type::Open, open_data ); }
    //
    if ( open_dialog.is_open() )
    {
        if ( open_dialog.Begin("Load session from file") ) {        // returns true when finished
            if ( auto path = open_dialog.result() )
                { load_async( *path ); }        // your own handler
        }
    }



    // status
    if ( !m_io_msg.empty() )
        ImGui::TextDisabled("%s", m_io_msg.c_str());
    

    // (existing Canvas/Grid prefs below …)
    return;
}



//  "_draw_settings_user_preferences"
//
void Editor::_draw_settings_user_preferences(void)
{
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

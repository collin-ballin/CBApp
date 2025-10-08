/***********************************************************************************
*
*       *********************************************************************
*       ****             C O M M O N . C P P  ____  F I L E              ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      June 28, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "widgets/editor/editor.h"



namespace cb {  //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//      1A.     PUBLIC API...
// *************************************************************************** //
// *************************************************************************** //

//  "save"
//
void Editor::save(void)
{
    const bool          has_file    = this->has_file();
    EditorState &       ES          = this->m_editor_S;

    
    
    //  CASE 1 :    EDITOR HAS EXISTING FILE...
    if ( has_file )
    {
        S.m_logger.debug( std::format("Editor | saving data to existing file, \"{}\"", ES.m_filepath.filename().string())  );
        this->save_async(ES.m_filepath);
    }
    //
    //  CASE 2 :    NEED TO  "SAVE AS..."  THE CURRENT FILE...
    else
    {
        S.m_logger.info( "Editor | requesting file dialog to create new file" );
        this->m_editor_S.m_sdialog_open.store(true, std::memory_order_release);
    }
    
    
    
    return;
}


//  "save_as"
//
void Editor::save_as(void)
{
    namespace           fs          = std::filesystem;
    const bool          has_file    = this->has_file();
    EditorState &       ES          = this->m_editor_S;

    
    
    //  CASE 1 :    EDITOR HAS EXISTING FILE...
    if ( has_file )
    {
        m_SAVE_DIALOG_DATA.default_dir      = fs::absolute( ES.m_filepath ).parent_path();
    }

    
    
    S.m_logger.info( "Editor | requesting file dialog to \"save as\" new file..." );
    this->m_editor_S.m_sdialog_open.store(true, std::memory_order_release);
    
    return;
}


//  "open"
//
void Editor::open(void)
{
    namespace           fs      = std::filesystem;
    EditorState &       ES      = this->m_editor_S;
    
    //  CASE 0 :    If Editor already has a file, set the default "Open" Dialog location to this file's parent dir.
    if ( this->has_file() )
    {
        m_OPEN_DIALOG_DATA.default_dir      = fs::absolute( ES.m_filepath ).parent_path();
    }



    this->m_editor_S.m_odialog_open.store(true, std::memory_order_release);
    
    return;
}


//  "undo"
//
void Editor::undo(void) {
    //  CB_LOG( LogLevel::Info, "Editor--undo" );
    return;
}


//  "redo"
//
void Editor::redo(void) {
    //  CB_LOG( LogLevel::Info, "Editor--redo" );
    return;
}


//  "GetMenuState"
[[nodiscard]] Editor::MenuState & Editor::GetMenuState(void) noexcept            { return (*this->m_menu_state); }

//  "GetMenuState"
[[nodiscard]] Editor::MenuState & Editor::GetMenuState(void) const noexcept      { return (*this->m_menu_state); }




    
    
//
//
// *************************************************************************** //
// *************************************************************************** //   END "Public API".






// *************************************************************************** //
//
//
//
//      1B.      WRAPPED FUNCTIONS FOR PUBLIC API...
// *************************************************************************** //
// *************************************************************************** //

//  "_file_dialog_handler"
//
/*
void Editor::_file_dialog_handler(void)
{
    namespace                   fs              = std::filesystem;
    std::optional<fs::path>     filepath        = std::nullopt;
    
    if ( m_saving )
    {
        //      DIALOG IS OPEN...
        if ( S.m_file_dialog.is_open() )        { return; }
        //
        //      FINISHED SAVING...
        else {
            m_saving        = false;
            filepath        = S.m_file_dialog.get_last_path().value_or("");
            if ( filepath )
            {
                S.m_logger.info( std::format("Saved to file \"{}\"", filepath->string()) );
                
                if ( this->save_to_file(filepath.value()) ) {
                    this->m_filepath = filepath.value();
                }
            }
            else    { S.m_logger.warning("Failed to save file."); }
        }
    }
    
    if ( m_loading )
    {
        //      DIALOG IS OPEN...
        if ( S.m_file_dialog.is_open() )        { return; }
        //
        //      FINISHED SAVING...
        else {
            m_loading           = false;
            filepath            = S.m_file_dialog.get_last_path().value_or("");
            if ( filepath )   {
                S.m_logger.info( std::format("Loaded from file \"{}\"", filepath->string()) );
                this->load_from_file( filepath.value() );
            }
            else    { S.m_logger.warning("Failed to open file."); }
        }
    }

    return;
}
*/




//  "_save_IMPL"
//
/*
void Editor::_save_IMPL(void)
{
    const bool      has_file    = this->has_file();
    
    
    //  CASE 1 :    SAVE TO THE CURRENT FILE...
    if (has_file)
    {
        this->save_to_file( this->m_filepath );
    }
    //
    //  CASE 2 :    OPEN FILE DIALOG TO SELECT A FILE...
    else
    {

    }
    return;
}
*/



//
//
// *************************************************************************** //
// *************************************************************************** //   END "WRAPPED API FUNCTIONS".






// *************************************************************************** //
//
//
//
//      2.      NEW...
// *************************************************************************** //
// *************************************************************************** //

//  "bring_selection_to_front"
//
void Editor::bring_selection_to_front(void)
{
    if ( m_sel.paths.empty() ) return;

    // 1. Build vector of pointers sorted by current z
    std::vector<Path*> vec;
    vec.reserve(m_paths.size());
    for (Path & p : m_paths) vec.push_back(&p);
    std::stable_sort(vec.begin(), vec.end(),
        [](const Path* a, const Path* b){ return a->z_index < b->z_index; });

    // 2. Re-order: unselected first, selected last (keep their internal order)
    std::vector<Path*> reordered;
    reordered.reserve(vec.size());
    for (Path * p : vec) {
        size_t idx = static_cast<size_t>(p - m_paths.data());
        if (!m_sel.paths.count(idx)) reordered.push_back(p);
    }
    for (Path * p : vec) {
        size_t idx = static_cast<size_t>(p - m_paths.data());
        if (m_sel.paths.count(idx)) reordered.push_back(p);
    }

    // 3. Re-assign dense z-indices (background → foreground)
    ZID z = Z_FLOOR_USER;
    for (Path* p : reordered)
        p->z_index = z++;
        
    return;
}


//  "bring_selection_forward"
//
void Editor::bring_selection_forward(void)
{
    if (m_sel.paths.empty()) return;

    // 1. build vector of pointers sorted by z
    std::vector<Path*> vec;
    vec.reserve(m_paths.size());
    for (Path& p : m_paths) vec.push_back(&p);
    std::stable_sort(vec.begin(), vec.end(),
        [](const Path* a, const Path* b){ return a->z_index < b->z_index; });

    // 2. walk from back→front; swap with next higher neighbour when selected
    for (int i = static_cast<int>(vec.size()) - 2; i >= 0; --i) {
        Path* cur = vec[i];
        Path* nxt = vec[i + 1];
        size_t idx_cur = static_cast<size_t>(cur - m_paths.data());
        size_t idx_nxt = static_cast<size_t>(nxt - m_paths.data());

        if (m_sel.paths.count(idx_cur) && !m_sel.paths.count(idx_nxt))
            std::swap(cur->z_index, nxt->z_index);
    }
    renormalise_z();
    return;
}


//  "send_selection_backward"
//
void Editor::send_selection_backward(void)
{
    if ( m_sel.paths.empty() ) return;

    // same idea, walk front→back
    std::vector<Path*> vec;
    vec.reserve(m_paths.size());
    for (Path& p : m_paths) vec.push_back(&p);
    std::stable_sort(vec.begin(), vec.end(),
        [](const Path* a, const Path* b){ return a->z_index < b->z_index; });

    for (size_t i = 1; i < vec.size(); ++i) {
        Path* cur = vec[i];
        Path* prev = vec[i - 1];
        size_t idx_cur  = static_cast<size_t>(cur  - m_paths.data());
        size_t idx_prev = static_cast<size_t>(prev - m_paths.data());

        if (m_sel.paths.count(idx_cur) && !m_sel.paths.count(idx_prev))
            std::swap(cur->z_index, prev->z_index);
    }
    renormalise_z();
    return;
}


//  "send_selection_to_back"
//
void Editor::send_selection_to_back(void)
{
    if ( m_sel.paths.empty() )      { return; }

    //      1.      Build vector sorted by current z
    std::vector<Path*> vec;
    vec.reserve(m_paths.size());
    for (Path & p : m_paths) vec.push_back(&p);
    std::stable_sort(vec.begin(), vec.end(),
        [](const Path* a, const Path* b){ return a->z_index < b->z_index; });

    //      2.      Re-order: selected first, unselected last
    std::vector<Path*> reordered;
    reordered.reserve(vec.size());
    for (Path * p : vec) {
        size_t idx = static_cast<size_t>(p - m_paths.data());
        if (m_sel.paths.count(idx)) reordered.push_back(p);
    }
    for (Path * p : vec) {
        size_t idx = static_cast<size_t>(p - m_paths.data());
        if (!m_sel.paths.count(idx)) reordered.push_back(p);
    }

    //      3.      Re-assign dense z-indices
    ZID z = Z_FLOOR_USER;
    for (Path* p : reordered)
        p->z_index = z++;
}






// *************************************************************************** //
//
//
//
//      2.  DATA MANAGEMENT FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "move_selection"
//
void Editor::move_selection(const float dx, const float dy)
{
    if (m_sel.empty()) return;

    for (uint32_t vid : m_sel.vertices)
        if (Vertex* v = find_vertex_mut(m_vertices, vid))
        {
            v->x += dx;
            v->y += dy;
        }

    // If you maintain cached bounds elsewhere, update them here:
    // _update_world_extent();
}


//  "copy_to_clipboard"
//
void Editor::copy_to_clipboard(void)
{
    if (m_sel.empty()) return;

    m_clipboard = {};                      // reset

    // 1. collect selected vertex IDs
    std::unordered_map<uint32_t, size_t> vid_to_local;   // old vid → local index
    auto add_vertex = [&](uint32_t vid)->size_t
    {
        if (auto it = vid_to_local.find(vid); it != vid_to_local.end())
            return it->second;
        const Vertex* v = find_vertex(m_vertices, vid);
        size_t idx = m_clipboard.vertices.size();
        m_clipboard.vertices.push_back(*v);  // copy, keep old id for now
        vid_to_local[vid] = idx;
        return idx;
    };

    // 2. bbox top-left for offset
    ImVec2 tl, br;
    _selection_bounds(tl, br, this->m_render_ctx);
    m_clipboard.ref_ws = tl;

    // --- Paths ---
    for (size_t pi : m_sel.paths)
    {
        const Path& src = m_paths[pi];
        Path dup = src; dup.verts.clear();
        for (uint32_t vid : src.verts)
            dup.verts.push_back(static_cast<uint32_t>(add_vertex(vid)));
        m_clipboard.paths.push_back(std::move(dup));
    }

    // --- Points ---
    for (size_t pi : m_sel.points)
    {
        const Point& src = m_points[pi];
        Point dup = src;
        dup.v = static_cast<uint32_t>(add_vertex(src.v));
        m_clipboard.points.push_back(dup);
    }
    
    return;
}


//  "paste_from_clipboard"
//
void Editor::paste_from_clipboard(ImVec2 target_ws)
{
    if (m_clipboard.empty()) return;

    const float dx = target_ws.x - m_clipboard.ref_ws.x;
    const float dy = target_ws.y - m_clipboard.ref_ws.y;

    // -----------------------------------------------------------------
    // 1. duplicate vertices – build two mappings:
    //    • new_vid_by_idx  :  local clipboard index  ->  new VertexID
    //    • new_vid_by_id   :  original VertexID      ->  new VertexID
    // -----------------------------------------------------------------
    std::vector<VertexID>                new_vid_by_idx(m_clipboard.vertices.size());
    std::unordered_map<VertexID,VertexID> new_vid_by_id;
    new_vid_by_id.reserve(m_clipboard.vertices.size());

    for (size_t i = 0; i < m_clipboard.vertices.size(); ++i)
    {
        const Vertex& v_src = m_clipboard.vertices[i];

        Vertex v = v_src;                    // copy
        v.id = m_next_id++;                  // assign fresh VID
        v.x += dx; v.y += dy;

        m_vertices.push_back(v);

        new_vid_by_idx[i]      = v.id;       // index-based mapping
        new_vid_by_id[v_src.id] = v.id;      // id-based   mapping
    }

    auto map_vertex = [&](uint32_t key)->VertexID
    {
        // Try "by index" first
        if (key < new_vid_by_idx.size())
            return new_vid_by_idx[key];

        // Fallback to "by original id"
        if (auto it = new_vid_by_id.find(key); it != new_vid_by_id.end())
            return it->second;

        // Last resort: duplicate on-demand (shouldn’t normally happen)
        Vertex dummy{};
        dummy.id = m_next_id++;
        m_vertices.push_back(dummy);
        return dummy.id;
    };

    // -----------------------------------------------------------------
    this->reset_selection();                // select pasted objects
    // -----------------------------------------------------------------

    // 2. duplicate points
    for (const Point& p_src : m_clipboard.points)
    {
        Point dup = p_src;
        dup.v = map_vertex(p_src.v);

        m_points.push_back(dup);
        m_sel.points.insert(static_cast<PointID>(m_points.size() - 1));
        m_sel.vertices.insert(dup.v);
    }

    // 4. duplicate paths
    for (const Path& p_src : m_clipboard.paths)
    {
        Path dup   = p_src;                     // copy style / flags
        dup.id     = m_next_pid++;              // fresh PathID
        dup.set_default_label(dup.id);
        dup.z_index = next_z_index();           // topmost layer

        dup.verts.clear();
        dup.closed = p_src.closed;

        for (uint32_t key : p_src.verts)
            dup.verts.push_back(map_vertex(key));

        m_paths.push_back(std::move(dup));

        m_sel.paths.insert(static_cast<PathID>(m_paths.size() - 1));
        for (VertexID vid : m_paths.back().verts)
            m_sel.vertices.insert(vid);
    }
    
    return;
}




//  "delete_selection"
//
void Editor::delete_selection(void)
{
    // Early-out
    if (m_sel.empty()) return;

    // --- gather indices -------------------------------------------------
    std::vector<size_t> pts(m_sel.points.begin(),  m_sel.points.end());
    std::vector<size_t> pth(m_sel.paths.begin(),   m_sel.paths.end());

    // erase from back → front to keep indices stable
    std::sort(pts.rbegin(), pts.rend());
    std::sort(pth.rbegin(), pth.rend());

    // --- points ---------------------------------------------------------
    for (size_t i : pts)
        m_points.erase(m_points.begin() + static_cast<long>(i));

    // --- paths (use existing helper so vertices/orphans are handled) ---
    for (size_t i : pth)
        _erase_path_and_orphans( static_cast<PathID>(i) );

    // finally
    this->reset_selection();    // m_sel.clear();
    return;
}












// *************************************************************************** //
//
//
//      GLOBAL OPERATIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_clear_all"
//
void Editor::_clear_all(void)
{
    //      1.      WIPE GEOMETRY CONTAINERS IN BULK...
    m_paths         .clear();       // clears Path<…> vector
    m_points        .clear();       // glyphs
    m_vertices      .clear();       // anchors


    //      2.      RESET SELECTION AND TOOL STATES...
    this->reset_selection();   // m_sel.clear() + related resets
    this->reset_pen();         // m_pen = {}      (live path state)


    m_lasso_active                          = false;    //  2.1.    LASSO TOOL.
    //
    m_dragging                              = false;    //  2.2.    DRAGGING SELECTION.
    this->m_movedrag.active                 = false;
    //
    m_drawing                               = false;
    //
    //
    m_next_id                               = 1;        //  RESET "NEXT-ID" COUNTERS (So new vertices/path IDs start fresh)...


    //      3.      BROWSER AND FILTER HOUSEKEEPING  [ Force filter to rebuild so the clipper sees an empty list ]...
    this->m_browser_S.clear();


    //      4.      CLEAN-UP BROWSER / OVERLAY  [ if any live ]...
    m_browser_S.m_inspector_vertex_idx      = -1;        // no vertex selected in inspector
    
    
    _prune_selection_mutability();      //  ensures nothing stale lingers

    //  Resident overlays such as selection HUD will auto-hide
    //  because `m_sel` is now empty.  No explicit overlay mutation needed.

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

/***********************************************************************************
*
*       *********************************************************************
*       ****             C O M M O N . C P P  ____  F I L E              ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      June 28, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "widgets/editor/editor.h"



namespace cb {  //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//  1A.     PUBLIC API...
// *************************************************************************** //
// *************************************************************************** //

//  "save"
//
void Editor::save(void) {
    CB_LOG( LogLevel::Info, "Editor--save" );


    m_sdialog_open.store(true, std::memory_order_release);
    
    
    return;
}


//  "open"
//
void Editor::open(void) {
    CB_LOG( LogLevel::Info, "Editor--open" );


    m_odialog_open.store(true, std::memory_order_release);
    
    
    return;
}


//  "undo"
//
void Editor::undo(void) {
    CB_LOG( LogLevel::Info, "Editor--undo" );
    return;
}


//  "redo"
//
void Editor::redo(void) {
    CB_LOG( LogLevel::Info, "Editor--redo" );
    return;
}






// *************************************************************************** //
//
//
//
//      1.  NEW...
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
    if (m_sel.paths.empty()) return;

    // 1. Build vector sorted by current z
    std::vector<Path*> vec;
    vec.reserve(m_paths.size());
    for (Path & p : m_paths) vec.push_back(&p);
    std::stable_sort(vec.begin(), vec.end(),
        [](const Path* a, const Path* b){ return a->z_index < b->z_index; });

    // 2. Re-order: selected first, unselected last
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

    // 3. Re-assign dense z-indices
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
    _selection_bounds(tl, br);
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

    // --- Lines ---
    for (size_t li : m_sel.lines)
    {
        const Line& src = m_lines[li];
        Line dup = src;
        dup.a = static_cast<uint32_t>(add_vertex(src.a));
        dup.b = static_cast<uint32_t>(add_vertex(src.b));
        m_clipboard.lines.push_back(dup);
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

    // 3. duplicate lines
    for (const Line& l_src : m_clipboard.lines)
    {
        Line dup = l_src;
        dup.a = map_vertex(l_src.a);
        dup.b = map_vertex(l_src.b);

        m_lines.push_back(dup);
        m_sel.lines.insert(static_cast<LineID>(m_lines.size() - 1));
        m_sel.vertices.insert(dup.a);
        m_sel.vertices.insert(dup.b);
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
}




//  "delete_selection"
//
void Editor::delete_selection(void)
{
    // Early-out
    if (m_sel.empty()) return;

    // --- gather indices -------------------------------------------------
    std::vector<size_t> pts(m_sel.points.begin(),  m_sel.points.end());
    std::vector<size_t> lns(m_sel.lines.begin(),   m_sel.lines.end());
    std::vector<size_t> pth(m_sel.paths.begin(),   m_sel.paths.end());

    // erase from back → front to keep indices stable
    std::sort(pts.rbegin(), pts.rend());
    std::sort(lns.rbegin(), lns.rend());
    std::sort(pth.rbegin(), pth.rend());

    // --- points ---------------------------------------------------------
    for (size_t i : pts)
        m_points.erase(m_points.begin() + static_cast<long>(i));

    // --- lines ----------------------------------------------------------
    for (size_t i : lns)
        m_lines.erase(m_lines.begin() + static_cast<long>(i));

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
    // ─── 1.  Wipe geometry containers in bulk ──────────────────────────
    m_paths.clear();       // clears Path<…> vector
    m_lines.clear();       // clears standalone Line<…> list
    m_points.clear();      // glyphs
    m_vertices.clear();    // anchors


    // ─── 2.  Reset selection & per-tool state ─────────────────────────
    this->reset_selection();   // m_sel.clear() + related resets
    this->reset_pen();         // m_pen = {}      (live path state)

    m_lasso_active          = false;
    m_dragging              = false;
    m_drawing               = false;


    //  Next ID counter back to 1 so new vertices/path IDs start fresh
    m_next_id               = 1;


    // ─── 3.  Browser & filter housekeeping ────────────────────────────
    //  Force filter to rebuild so the clipper sees an empty list.
    m_browser_filter.Build();
    m_browser_anchor        = -1;


    // ─── 4.  Inspector / overlay clean-up (if any live) ───────────────
    m_inspector_vertex_idx  = -1;        // no vertex selected in inspector
    _prune_selection_mutability();      // ensures nothing stale lingers

    //  Resident overlays such as selection HUD will auto-hide
    //  because `m_sel` is now empty.  No explicit overlay mutation needed.

    return;
}






// *************************************************************************** //
//
//
//      SERIALIZATION...
// *************************************************************************** //
// *************************************************************************** //

//  "make_snapshot"
//
EditorSnapshot Editor::make_snapshot(void) const
{
    EditorSnapshot      s;
    s.vertices          = m_vertices;        // shallow copies fine
    s.paths             = m_paths;
    s.points            = m_points;
    s.lines             = m_lines;
    s.selection         = m_sel;
    //
    //
    //  TODO:   grid, view, mode …
    return s;
}


//  "load_from_snapshot"
//
void Editor::load_from_snapshot(EditorSnapshot && snap)
{
    m_vertices  = std::move(snap.vertices);
    m_paths     = std::move(snap.paths);
    m_points    = std::move(snap.points);
    m_lines     = std::move(snap.lines);
    m_sel       = std::move(snap.selection);
    //
    //
    //
    //  TODO:   grid, view, mode …
    _recompute_next_ids();          // ← ensure unique IDs going forward
    
    return;
}


//  "pump_main_tasks"
//
void Editor::pump_main_tasks(void)
{
    std::vector<std::function<void()>> tasks;
    {
        std::lock_guard lk(m_task_mtx);
        tasks.swap(m_main_tasks);
    }
    for (auto& fn : tasks) fn();
}


//  "save_async"
//
void Editor::save_async(std::filesystem::path path)
{
    m_io_busy = true;
    auto snap = make_snapshot();

    // convert to absolute once, so worker sees a full path
    path = std::filesystem::absolute(path);

    std::thread([this, snap = std::move(snap), path]{
        save_worker(snap, path);
    }).detach();
}


//  "load_async"
//
void Editor::load_async(std::filesystem::path path)
{
    m_io_busy   = true;
    path        = std::filesystem::absolute(path);

    std::thread([this, path]{
        load_worker(path);
    }).detach();
}


//  "_draw_io_overlay"
//
void Editor::_draw_io_overlay(void)
{
    if (!m_io_busy && m_io_last == IoResult::Ok) return;   // nothing to show

    const char* txt = m_io_busy ? "Working…" : m_io_msg.c_str();
    ImVec2 pad{8,8};
    ImVec2 size = ImGui::CalcTextSize(txt);
    ImVec2 pos  = ImGui::GetMainViewport()->Pos;
    pos.y += ImGui::GetMainViewport()->Size.y - size.y - pad.y*2;

    ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.6f);
    ImGui::Begin("##io_overlay", nullptr,
                 ImGuiWindowFlags_NoDecoration |
                 ImGuiWindowFlags_NoInputs     |
                 ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::TextUnformatted(txt);
    ImGui::End();
}



// *************************************************************************** //
// *************************************************************************** //

//  "save_worker"
//
void Editor::save_worker(EditorSnapshot snap, std::filesystem::path path)
{
    nlohmann::json      j;
    j["version"]                    = kSaveFormatVersion;
    j["state"]                      = snap;
    std::ofstream       os(path, std::ios::binary);
    IoResult            res         = os ? IoResult::Ok : IoResult::IoError;
    if (res == IoResult::Ok) os << j.dump(2);
    // enqueue completion notification
    {
        {
            std::lock_guard lk(m_task_mtx);
            m_main_tasks.push_back( [this,res,path] {
                m_io_busy = false;
                m_io_last = res;
                m_io_msg  = (res == IoResult::Ok)
                          ? "Saved to " + path.string()
                          : "Save failed";
            } );
        }
    }
    
    return;
}


//  "load_worker"
//
void Editor::load_worker(std::filesystem::path path)
{
    // ---------- read file -------------------------------------------------
    nlohmann::json  j;
    std::ifstream   is(path, std::ios::binary);
    IoResult        res  = is ? IoResult::Ok : IoResult::IoError;
    EditorSnapshot  snap;

    if (res == IoResult::Ok)
    {
        try {
            is >> j;
            if (j.at("version").get<uint32_t>() != kSaveFormatVersion)
                res = IoResult::VersionMismatch;
            else
                snap = j.at("state").get<EditorSnapshot>();
        }
        catch (...) { res = IoResult::ParseError; }
    }

    // ---------- enqueue GUI-thread callback -------------------------------
    {
        std::lock_guard lk(m_task_mtx);
        m_main_tasks.push_back(
            [this, res, snap = std::move(snap), path]() mutable
            {
                m_io_busy = false;

                if (res == IoResult::Ok) {
                    load_from_snapshot(std::move(snap));
                    m_io_msg = "Loaded \"" + path.filename().string() + "\"";
                } else {
                    switch (res) {
                        case IoResult::IoError:         m_io_msg = "Load I/O error";                 break;
                        case IoResult::ParseError:      m_io_msg = "Load parse error";               break;
                        case IoResult::VersionMismatch: m_io_msg = "Save-file version mismatch";     break;
                        default:                        m_io_msg = "Unknown load error";             break;
                    }
                }
                m_io_last = res;
            });
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

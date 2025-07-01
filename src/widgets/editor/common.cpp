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
    if ( m_clipboard.empty() ) return;

    const float dx = target_ws.x - m_clipboard.ref_ws.x;
    const float dy = target_ws.y - m_clipboard.ref_ws.y;

    // map local index → new vertex ID
    std::vector<uint32_t> new_vid(m_clipboard.vertices.size());

    // --- duplicate vertices ---
    for (size_t i = 0; i < m_clipboard.vertices.size(); ++i)
    {
        Vertex v = m_clipboard.vertices[i];
        v.id = m_next_id++;
        v.x += dx; v.y += dy;
        m_vertices.push_back(v);
        new_vid[i] = v.id;
    }

    this->reset_selection();    // m_sel.clear();   // select the pasted objects

    // --- duplicate points ---
    for (const Point& p : m_clipboard.points)
    {
        Point dup = p;
        dup.v = new_vid[dup.v];
        m_points.push_back(dup);
        m_sel.points.insert(m_points.size() - 1);
        m_sel.vertices.insert(dup.v);
    }

    // --- duplicate lines ---
    for (const Line& l : m_clipboard.lines)
    {
        Line dup = l;
        dup.a = new_vid[dup.a];
        dup.b = new_vid[dup.b];
        m_lines.push_back(dup);
        m_sel.lines.insert(m_lines.size() - 1);
        m_sel.vertices.insert(dup.a);
        m_sel.vertices.insert(dup.b);
    }

    // --- duplicate paths ---
    for (const Path& p : m_clipboard.paths)
    {
        Path dup = p; dup.verts.clear();
        for (uint32_t idx : p.verts)
            dup.verts.push_back(new_vid[idx]);
        m_paths.push_back(std::move(dup));
        m_sel.paths.insert(m_paths.size() - 1);
        for (uint32_t vid : dup.verts) m_sel.vertices.insert(vid);
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
        _erase_path_and_orphans(i);

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
    const size_t    N       = this->m_paths.size();
    
    
    //  1.  CLEAR ALL EDITOR DATA...
    for (size_t idx = 0; idx < N; ++idx) {
        _erase_path_and_orphans(idx);  // ← replaces direct m_paths.erase()
    }

    m_vertices.clear();
    m_points.clear();
    m_lines.clear();
    this->reset_selection();    //  m_sel.clear();
    
    
    //  2.  RESET EDITOR STATE...
    m_lasso_active  = false;
    m_dragging      = false;
    m_drawing       = false;
    m_next_id       = 1;
    this->reset_pen();//  m_pen           = {};
    
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
    //  TODO:   grid, view, mode …
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
void Editor::save_async(const std::string& path)
{
    m_io_busy = true;
    auto snap = make_snapshot();
    std::thread([this, snap = std::move(snap), path]{
        save_worker(snap, path);
    }).detach();
}


//  "load_async"
//
void Editor::load_async(const std::string& path)
{
    m_io_busy = true;
    std::thread([this, path]{ load_worker(path); }).detach();
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
void Editor::save_worker(EditorSnapshot snap, std::string path)
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
                          ? "Saved to " + path
                          : "Save failed";
            } );
        }
    }
    
    return;
}


//  "load_worker"
//
void Editor::load_worker(std::string path)
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
            [this, res, snap = std::move(snap), path = std::move(path)]() mutable
            {
                m_io_busy = false;            // overlay flag

                if (res == IoResult::Ok) {
                    load_from_snapshot(std::move(snap));
                    m_io_msg  = "Loaded \"" + path + "\"";
                }
                else {
                    switch (res) {
                        case IoResult::IoError:         m_io_msg = "Load I/O error";          break;
                        case IoResult::ParseError:      m_io_msg = "Load parse error";        break;
                        case IoResult::VersionMismatch: m_io_msg = "Save-file version mismatch"; break;
                        default:                        m_io_msg = "Unknown load error";      break;
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

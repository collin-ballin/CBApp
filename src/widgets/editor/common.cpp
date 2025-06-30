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
//      1.  DATA MANAGEMENT FUNCTIONS...
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
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.











// *************************************************************************** //
// *************************************************************************** //
//
//  END.

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
//      MISC. UTILITIES...
// *************************************************************************** //
// *************************************************************************** //

//  "_draw_controls"
//
void Editor::_draw_controls(const ImVec2& pos)
{
    ImGui::SetCursorScreenPos(pos);
    ImGui::BeginGroup();

    // Mode selector
    int mode_i = static_cast<int>(m_mode);
    ImGui::SetNextItemWidth(110.0f);
    ImGui::Combo("Mode", &mode_i, ms_MODE_LABELS.data(),
                 static_cast<int>(Mode::Count));
    m_mode = static_cast<Mode>(mode_i);

    // Grid toggle
    ImGui::SameLine(0.0f, 15.0f);
    ImGui::Checkbox("Grid", &m_show_grid);

    // Clear button
    ImGui::SameLine(0.0f, 15.0f);
    if (ImGui::Button("Clear"))
        _clear_all();

    ImGui::EndGroup();
}


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

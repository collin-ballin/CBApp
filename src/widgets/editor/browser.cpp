/***********************************************************************************
*
*       *********************************************************************
*       ****            B R O W S E R . C P P  ____  F I L E             ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      June 13, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "widgets/editor/editor.h"



namespace cb {  //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//  0.  STATIC / INTERAL LINKAGE TO THIS TU...
//
namespace {  //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //


// Return true if a line already connects point indices a and b (order‑agnostic).
static bool line_exists(const std::vector<Line>& lines, size_t a, size_t b)
{
    for (const Line& ln : lines)
        if ((ln.a == a && ln.b == b) || (ln.a == b && ln.b == a))
            return true;
    return false;
}

// Disconnect every edge that references the given point index.
static void detach_point(std::vector<Line>& lines, size_t pi)
{
    lines.erase(std::remove_if(lines.begin(), lines.end(),
                 [pi](const Line& ln){ return ln.a == pi || ln.b == pi; }),
                 lines.end());
}

// Return index into m_points for a given vertex‑id (or -1 if not found)
static int point_index_from_vid(const std::vector<Point>& pts, uint32_t vid)
{
    for (int i = 0; i < static_cast<int>(pts.size()); ++i)
        if (pts[i].v == vid) return i;
    return -1;
}



// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "internal" NAMESPACE.






// *************************************************************************** //
//
//
//  1.  INITIALIZATION METHODS...
// *************************************************************************** //
// *************************************************************************** //

//  Default Constructor.
//
Editor::Editor(void) {
    this->m_window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_HiddenTabBar;
}

//  Destructor.
//
Editor::~Editor(void)
{
    this->_clear_all();
}




// *************************************************************************** //
//
//
//  2.  IMPLEMENTATION...
// *************************************************************************** //
// *************************************************************************** //

//-------------------------------------------------------------------------
// PUBLIC FACADE – minimal wrapper to draw both columns side‑by‑side
//-------------------------------------------------------------------------

//  "DrawBrowser"
//
void Editor::DrawBrowser(void)
{
    //  1.  CONTROL BAR...
    _draw_controls();
    
    
    //  2.  LEFTHAND BROWSER SELECTION MENU...
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize,  ms_CHILD_BORDER1);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding,    ms_CHILD_ROUND1);
    ImGui::PushStyleColor(ImGuiCol_ChildBg,             ms_CHILD_FRAME_BG1L);
    //
    //
    //
        //  2.  LEFTHAND BROWSER SELECTION MENU...
        ImGui::BeginChild("##Editor_Browser_Left", ImVec2(this->ms_LIST_COLUMN_WIDTH, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
            _draw_path_list_column();
        ImGui::EndChild();
        ImGui::PopStyleColor();
        //
        ImGui::SameLine();
        //
        //  3.  RIGHTHAND BROWSER INSPECTOR MENU...
        ImGui::PushStyleColor(ImGuiCol_ChildBg,         ms_CHILD_FRAME_BG1R);
        ImGui::BeginChild("##Editor_Browser_Right", ImVec2(0, 0), ImGuiChildFlags_Borders);
            _draw_path_inspector_column();
        ImGui::EndChild();
    //
    //
    //
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);  //  ImGuiStyleVar_ChildBorderSize, ImGuiStyleVar_ChildRounding
    
    
    return;
}



// *************************************************************************** //
//
//
//  2.  NEW INTERNAL BROWSER IMPLEMENTATIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_draw_path_list_column"
//
void Editor::_draw_path_list_column(void)
{
    // 1) filter input --------------------------------------------------
    ImGui::SetNextItemWidth(-FLT_MIN);
    if (ImGui::InputTextWithHint("##Editor_Browser_LeftFilter", "filter",
                                 m_browser_filter.InputBuf,
                                 IM_ARRAYSIZE(m_browser_filter.InputBuf)))
        m_browser_filter.Build();

    ImGui::Separator();

    const int total = static_cast<int>(m_paths.size());
    ImGuiListClipper clipper;
    clipper.Begin(total, -1);

    while (clipper.Step())
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
        {
            char label[12]; std::snprintf(label, sizeof(label), "Path%02d", i);
            if (!m_browser_filter.PassFilter(label)) continue;

            bool selected = m_sel.paths.count(static_cast<size_t>(i));
            if (ImGui::Selectable(label, selected))
            {
                const bool ctrl  = ImGui::GetIO().KeyCtrl;
                const bool shift = ImGui::GetIO().KeyShift;

                if (!ctrl && !shift)
                {   m_sel.clear();  m_sel.paths.insert(i);  m_browser_anchor = i; }
                else if (shift && m_browser_anchor >= 0)
                {   int lo = std::min(m_browser_anchor, i), hi = std::max(m_browser_anchor, i);
                    if (!ctrl) m_sel.clear();
                    for (int k = lo; k <= hi; ++k) m_sel.paths.insert(k);
                }
                else if (ctrl)
                {   if (!m_sel.paths.erase(i)) { m_sel.paths.insert(i); m_browser_anchor = i; } }

                _rebuild_vertex_selection();   // keep vertices in sync
            }
        }
    clipper.End();
}



//  "_draw_path_inspector_column"
//
void Editor::_draw_path_inspector_column(void)
{
    if (m_sel.paths.empty()) {
        ImGui::TextDisabled("No selection.");
        return;
    }

    if (m_sel.paths.size() == 1)
        _draw_single_path_inspector();
    else
        _draw_multi_path_inspector();
}







// *************************************************************************** //
//
//
//  3.  SUBSIDIARY BROWSER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_draw_vertex_list_subcolumn"
//
void Editor::_draw_vertex_list_subcolumn(Path& path)
{
    const int total = static_cast<int>(path.verts.size());
    ImGuiListClipper clipper; clipper.Begin(total, -1);

    while (clipper.Step())
        for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row)
        {
            char lbl[8]; std::snprintf(lbl, sizeof(lbl), "V%02d", row);
            bool selected = (row == m_inspector_vertex_idx);
            if (ImGui::Selectable(lbl, selected))
                m_inspector_vertex_idx = (selected ? -1 : row);      // toggle
        }
    clipper.End();
}


//  "_draw_vertex_inspector_subcolumn"
//
void Editor::_draw_vertex_inspector_subcolumn(Path & path)
{
    //  CASE 0 :    NO VALID SELECTION...
    if (m_inspector_vertex_idx < 0 ||
        m_inspector_vertex_idx >= static_cast<int>(path.verts.size()))
    {
        ImGui::TextDisabled("Select a vertex from the lefthand column...");
        return;
    }

    //  0.  OBTAIN POINTER TO VERTEX...
    uint32_t    vid     = path.verts[static_cast<size_t>(m_inspector_vertex_idx)];
    Vertex *    v       = find_vertex_mut(m_vertices, vid);
    
    //  CASE 1 :    STALE VERTEX...
    if (!v) {
        ImGui::TextDisabled("[stale vertex]");
        return;
    }



    //  1.  HEADER CONTENT...
    ImGui::Text("Vertex V%02d  (id=%u)", m_inspector_vertex_idx, vid);
    ImGui::Separator();
    //
    //      1.1.    Constants:
    const float     grid        = GRID_STEP / m_cam.zoom_mag;
    const float     speed       = 0.1f * grid;
    auto            snap        = [grid](float f){ return std::round(f / grid) * grid; };
    bool            dirty       = false;
    int             kind_idx    = static_cast<int>(v->kind);



    //  2.  WIDGETS...
    //
    //      2.1.    Position:
    utl::LeftLabel("Position:");
    dirty                      |= ImGui::DragFloat2("##Editor_VertexBrowser_Pos", &v->x, speed, -FLT_MAX, FLT_MAX, "%.3f");
    //
    if ( dirty && /*!ImGui::IsItemActive() && */ this->want_snap() ) {
        dirty       = false;
        ImVec2 s    = _grid_snap({v->x, v->y});
        v->x        = s.x;
        v->y        = s.y;
    }



    //      2.2.    Bézier Handles / Control Points
    ImGui::SeparatorText("Handles");
    //
    //              2.2A    ANCHOR TYPE (corner / smooth / symmetric):
    {
        utl::LeftLabel("Anchor Type:");
        dirty               = ImGui::Combo( "##Editor_VertexBrowser_AnchorType", &kind_idx,
                                            ANCHOR_TYPE_NAMES, IM_ARRAYSIZE(ANCHOR_TYPE_NAMES) );
        if (dirty) {
            v->kind     = static_cast<AnchorType>(kind_idx);
            dirty       = false;
        }
    }
    //
    //
    //              2.2B    OUTWARD (to next vertex):
    //
    utl::LeftLabel("Outward:");
    dirty               = ImGui::DragFloat2("##Editor_VertexBrowser_OutwardControl",    &v->out_handle.x,   speed,  -FLT_MAX,   FLT_MAX,    "%.3f");
    if ( dirty && !ImGui::IsItemActive() )
    {
        v->out_handle.x     = snap(v->out_handle.x);
        v->out_handle.y     = snap(v->out_handle.y);
        mirror_handles(*v, /*dragging_out=*/true);  // keep smooth/symmetric rule
        dirty               = false;
    }
    //
    //
    //              2.2C    INWARD (from previous vertex):
    utl::LeftLabel("Inward:");
    //
    dirty              = ImGui::DragFloat2("##Editor_VertexBrowser_InwardControl",     &v->in_handle.x,    speed,  -FLT_MAX,   FLT_MAX,    "%.3f");
    if ( dirty && !ImGui::IsItemActive() ) {
        v->in_handle.x      = snap(v->in_handle.x);
        v->in_handle.y      = snap(v->in_handle.y);
        mirror_handles(*v, /*dragging_out=*/false);
        dirty               = false;
    }






    /* delete ----------------------------------------------------------- */
    ImGui::Separator();
    if (ImGui::Button("Delete Vertex", {120,0}))
    {
        _erase_vertex_and_fix_paths(vid);
        m_inspector_vertex_idx = -1;
        _rebuild_vertex_selection();
    }
    
    
    return;
}







// *************************************************************************** //
//
//
//  4.  INSPECTOR COLUMN DISPATCHER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

/*---------------------------------------------------------------
    1.  Multi-selection inspector  (>=2 paths)
----------------------------------------------------------------*/
void Editor::_draw_multi_path_inspector(void)
{
    ImGui::Text("%zu paths selected", m_sel.paths.size());
    ImGui::Separator();

    if (ImGui::Button("Delete Selections", {150,0}))
    {
        std::vector<size_t> idxs(m_sel.paths.begin(), m_sel.paths.end());
        std::sort(idxs.rbegin(), idxs.rend());
        for (size_t i : idxs)
            if (i < m_paths.size())
                m_paths.erase(m_paths.begin() + static_cast<long>(i));

        m_sel.clear();
        m_inspector_vertex_idx = -1;
    }
}


/*---------------------------------------------------------------
    2.  Single-path inspector  (exactly one path)
----------------------------------------------------------------*/
void Editor::_draw_single_path_inspector(void)
{
    constexpr ImGuiColorEditFlags   COLOR_FLAGS     = ImGuiColorEditFlags_NoInputs;
    const ImGuiStyle &              style           = ImGui::GetStyle();
    //
    size_t                          pidx            = *m_sel.paths.begin();
    
    
    
    //  CASE 0 :    ERROR...
    if (pidx >= m_paths.size()) {
        ImGui::TextDisabled("[invalid object]"); return;
    }
    Path &          path            = m_paths[pidx];
    bool            is_area         = path.is_area();



    //  1.  HEADER-ENTRY AND "DELETE" BUTTON...
    ImGui::Text("Object %zu  (%zu vertices)", pidx, path.verts.size());
    ImGui::SameLine();
    if (ImGui::Button("Delete Object"))
    {
        _erase_path_and_orphans(pidx);   // ← replaces direct m_paths.erase()
        m_sel.clear();
        m_inspector_vertex_idx = -1;
        return;
    }
    ImGui::Separator();



    //  2.  LINE-STROKE, AREA-FILL COLORS...
    {
        ImVec4          stroke_f            = u32_to_f4(path.style.stroke_color);
        ImVec4          fill_f              = u32_to_f4(path.style.fill_color);
        bool            stroke_dirty        = false;
        bool            fill_dirty          = false;


        utl::LeftLabelSimple("Stroke:");    ImGui::SameLine();
        stroke_dirty                        = ImGui::ColorEdit4( "##Editor_VertexBrowser_LineColor",    (float*)&stroke_f,  COLOR_FLAGS );
        //
        ImGui::SameLine();
        ImGui::BeginDisabled( !is_area );
            utl::LeftLabelSimple("Fill:");  ImGui::SameLine();
            fill_dirty                      = ImGui::ColorEdit4( "##Editor_VertexBrowser_FillColor",    (float*)&fill_f,    COLOR_FLAGS );
        ImGui::EndDisabled();
        
        if (stroke_dirty)   { path.style.stroke_color = f4_to_u32(stroke_f); }
        if (fill_dirty)     { path.style.fill_color   = f4_to_u32(fill_f); }
        if (!is_area)       { path.style.fill_color  &= 0x00FFFFFF; }   // clear alpha

        ImGui::SameLine(); 
    }


    //  3.  LINE WIDTH...
    {
        static constexpr float  min_width   = 0.25;
        static constexpr float  max_width   = 32;
        bool                    dirty       = false;
        float                   w           = path.style.stroke_width;
        
        utl::LeftLabelSimple("Line Width:");    ImGui::SameLine();
        ImGui::SetNextItemWidth(200.0f);
        //
        //  CASE 1 :    Value < 2.0f
        if (w < 2.0f) {
            dirty = ImGui::InputFloat("##Editor_VertexBrowser_LineWidth", &w,   0.125f,   0.25f,      "%.3f px");
        }
        //
        //  CASE 2 :    2.0 <= Value.
        else {
            dirty = ImGui::InputFloat("##Editor_VertexBrowser_LineWidth", &w,   1.0f,     2.0f,       "%.1f px");
        }
        //
        //
        //
        if ( dirty ) {
            w = std::clamp(w, min_width, max_width);
            path.style.stroke_width = w;
        }
    }
    ImGui::Separator();



    //  4.  "VERTEX BROWSER" FOR THE OBJECT...
    float availY = ImGui::GetContentRegionAvail().y;
    float sub_h  = availY * ms_VERTEX_SUBBROWSER_HEIGHT; 
    float filler = availY - sub_h - style.WindowPadding.y;
    
    if (filler > 0.0f)
        { ImGui::Dummy(ImVec2(0, filler)); }

    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize,  ms_CHILD_BORDER2);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding,    ms_CHILD_ROUND2);
    ImGui::PushStyleColor(ImGuiCol_ChildBg,             ms_CHILD_FRAME_BG2L);
    //
    //
    //  //  4.1.    LEFT-HAND VERTEX BROWSER.
        ImGui::BeginChild("##VertexPanel_Left", ImVec2(ms_LIST_COLUMN_WIDTH, sub_h), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
            _draw_vertex_list_subcolumn(path);
        ImGui::EndChild();
        ImGui::PopStyleColor();
        //
        ImGui::SameLine();
        //
        //  4.2.    RIGHT-HAND VERTEX BROWSER.
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ms_CHILD_FRAME_BG2R);
        ImGui::BeginChild("##VertexPanel_Right", ImVec2(0, sub_h),
                          ImGuiChildFlags_Borders);
            _draw_vertex_inspector_subcolumn(path);
        ImGui::EndChild();
        ImGui::PopStyleColor();
    //
    //
    ImGui::PopStyleVar(2);   // border size, rounding
    
    
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

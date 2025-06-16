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
Editor::~Editor(void)   { }




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
    //  Fixed‑width left column
        ImGui::BeginChild("##Editor_Browser_Left", ImVec2(this->ms_LIST_COLUMN_WIDTH, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
            _draw_point_list_column();
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("##Editor_Browser_Right", ImVec2(0, 0), ImGuiChildFlags_Borders);
            _draw_point_inspector_column();
        ImGui::EndChild();
    //
    //
    return;
}


//  "DrawBrowser_Window"
//
void Editor::DrawBrowser_Window([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
{

    if (p_open) {
        //ImGui::SetNextWindowClass(&this->m_window_class);
        ImGui::Begin( uuid, nullptr, flags );
        //
        //
        //  Fixed‑width left column
            ImGui::BeginChild("##Editor_Browser_Left", ImVec2(this->ms_LIST_COLUMN_WIDTH, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
                _draw_point_list_column();
            ImGui::EndChild();

            ImGui::SameLine();

            ImGui::BeginChild("##Editor_Browser_Right", ImVec2(0, 0), ImGuiChildFlags_Borders);
                _draw_point_inspector_column();
            ImGui::EndChild();
        //
        //
        ImGui::End();
    }
    
    
    return;
}






// *************************************************************************** //
//
//
//  2.  INTERNAL IMPLEMENTATIONS...
// *************************************************************************** //
// *************************************************************************** //

//-------------------------------------------------------------------------
// LEFT COLUMN – searchable list of points
//-------------------------------------------------------------------------
void Editor::_draw_point_list_column()
{
    // 1) filter input
    ImGui::SetNextItemWidth(-FLT_MIN);
    if ( ImGui::InputTextWithHint("##Editor_Browser_LeftFilter", "filter",
                                  m_browser_filter.InputBuf,
                                  IM_ARRAYSIZE(m_browser_filter.InputBuf)) ) {
        m_browser_filter.Build();
    }
    
    
    //m_browser_filter.Draw("##Editor_Browser_LeftFilter", -1);



    // 2) list header spacing
    ImGui::Separator();

    // 3) list area
    const int       total   = static_cast<int>(m_points.size());

    ImGuiListClipper clipper;
    clipper.Begin(total, -1);
    while (clipper.Step())
    {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
        {
            // Row label "P00", "P01", ...
            char label[8];
            std::snprintf(label, sizeof(label), "P%02d", i);

            // Apply filter to ID label only (for simplicity). Extend if you
            // need to filter by type later.
            if (!m_browser_filter.PassFilter(label))
                continue;

            bool selected = m_sel.points.count(static_cast<size_t>(i));
            if (ImGui::Selectable(label, selected))
            {
                const bool ctrl  = ImGui::GetIO().KeyCtrl;
                const bool shift = ImGui::GetIO().KeyShift;

                if (!ctrl && !shift)
                {
                    // single‑select
                    m_sel.clear();
                    m_sel.points.insert(i);
                    m_browser_anchor = i;
                }
                else if (shift && m_browser_anchor >= 0)
                {
                    // range select [anchor, i]
                    int lo = std::min(m_browser_anchor, i);
                    int hi = std::max(m_browser_anchor, i);
                    if (!ctrl) m_sel.clear();
                    for (int k = lo; k <= hi; ++k)
                        m_sel.points.insert(k);
                }
                else if (ctrl)
                {
                    // toggle
                    if (!m_sel.points.erase(i))
                    {
                        m_sel.points.insert(i);
                        m_browser_anchor = i;
                    }
                }

                // keep vertex list in sync
                _rebuild_vertex_selection();
            }
        }
    }
    clipper.End();
}






//-------------------------------------------------------------------------
// RIGHT COLUMN – inspector panel adapting to selection count
//-------------------------------------------------------------------------

void Editor::_draw_point_inspector_column()
{
    const size_t sel_count = m_sel.points.size();

    /* ------------------------------------------------------------------ */
    /* 0 selected                                                         */
    /* ------------------------------------------------------------------ */
    if (sel_count == 0)
    {
        ImGui::TextDisabled("No points selected.");
        return;
    }

    /* ------------------------------------------------------------------ */
    /* 1 selected                                                         */
    /* ------------------------------------------------------------------ */
    if (sel_count == 1)
    {
        size_t idx = *m_sel.points.begin();
        if (idx >= m_points.size()) { ImGui::TextDisabled("[invalid point]"); return; }

        Point& pt = m_points[idx];
        Pos*   v  = find_vertex(m_vertices, pt.v);
        if (!v) { ImGui::TextDisabled("[invalid vertex]"); return; }

        ImGui::Text("Point P%02zu", idx);
        ImGui::Separator();

        /* X/Y controls (snap to grid on release) ----------------------- */
        const float grid   = GRID_STEP / m_zoom;          // world-units/grid
        const float speed  = 0.1f * grid;                 // drag sensitivity
        auto  snap = [grid](float f){ return std::round(f / grid) * grid; };

        bool edited = false;
        edited |= ImGui::DragFloat("X", &v->x, speed, -FLT_MAX, FLT_MAX, "%.3f");
        edited |= ImGui::DragFloat("Y", &v->y, speed, -FLT_MAX, FLT_MAX, "%.3f");
        if (edited && !ImGui::IsItemActive())
        {
            v->x = snap(v->x);
            v->y = snap(v->y);
        }

        /* Type combo --------------------------------------------------- */
        static const char* TYPES[] = { "Default", "A", "B", "C" };
        int type_idx = 0;
        for (int i = 0; i < 4; ++i)
            if (pt.sty.color == COL_POINT_DEFAULT + i) type_idx = i;

        if (ImGui::Combo("Type", &type_idx, TYPES, IM_ARRAYSIZE(TYPES)))
            pt.sty.color = COL_POINT_DEFAULT + type_idx;

        /* --- NEW: delete this point ----------------------------------- */
        ImGui::Separator();
        if (ImGui::Button("Delete Point", {120,0}))
        {
            _erase_vertex_and_fix_paths(pt.v);
            m_sel.clear();
            _rebuild_vertex_selection();
            return;
        }
        

        /* Connections list --------------------------------------------- */
        ImGui::Separator();
        ImGui::Text("Connections:");

        bool dirty = false;
        for (size_t li = 0; li < m_lines.size(); /*++li done inside*/)
        {
            const Line& ln = m_lines[li];
            if (ln.a != idx && ln.b != idx) { ++li; continue; }

            size_t peer = (ln.a == idx) ? ln.b : ln.a;
            char   lbl[16]; std::snprintf(lbl, sizeof(lbl), "P%02zu", peer);

            ImGui::BulletText("%s", lbl);
            ImGui::SameLine();
            ImGui::PushID(static_cast<int>(li));
            if (ImGui::SmallButton("X"))
            {
                m_lines.erase(m_lines.begin() + static_cast<int>(li));
                dirty = true;
            }
            else ++li;          // advance only when not erasing
            ImGui::PopID();
        }
        if (dirty) _rebuild_vertex_selection();
        return;
    }

    /* ------------------------------------------------------------------ */
    /* 2 selected                                                         */
    /* ------------------------------------------------------------------ */
    if (sel_count == 2)
    {
        std::array<size_t,2> pair{};
        std::copy(m_sel.points.begin(), m_sel.points.end(), pair.begin());
        size_t a = pair[0], b = pair[1];

        bool connected = line_exists(m_lines, a, b);
        if (ImGui::Button(connected ? "Disconnect" : "Connect", {120,0}))
        {
            if (connected)
            {
                m_lines.erase(std::remove_if(m_lines.begin(), m_lines.end(),
                             [a,b](const Line& ln){
                                 return (ln.a==a && ln.b==b) || (ln.a==b && ln.b==a);
                             }), m_lines.end());
            }
            else
            {
                m_lines.push_back({ static_cast<uint32_t>(a),
                                    static_cast<uint32_t>(b),
                                    IM_COL32_WHITE, 2.f });
            }
        }
        
        
        ImGui::Separator();
        if (ImGui::Button("Delete Selected", {150,0}))
        {
            std::vector<uint32_t> vids;
            for (size_t idx : m_sel.points)
                vids.push_back( m_points[idx].v );

            for (uint32_t vid : vids)
                _erase_vertex_and_fix_paths(vid);

            m_sel.clear();
            _rebuild_vertex_selection();
            return;
        }
        

        ImGui::Separator();
        static const char* TYPES[] = { "Default", "A", "B", "C" };
        int type_idx = 0;
        if (ImGui::Combo("Type", &type_idx, TYPES, IM_ARRAYSIZE(TYPES)))
            for (size_t idx : m_sel.points)
                m_points[idx].sty.color = COL_POINT_DEFAULT + type_idx;
        return;
    }

    /* ------------------------------------------------------------------ */
    /* >2 selected                                                        */
    /* ------------------------------------------------------------------ */
    ImGui::Text("%zu points selected", sel_count);
    ImGui::Separator();

    static const char* TYPES[] = { "Default", "A", "B", "C" };
    int type_idx = 0;
    if (ImGui::Combo("Set Type", &type_idx, TYPES, IM_ARRAYSIZE(TYPES)))
        for (size_t idx : m_sel.points)
            m_points[idx].sty.color = COL_POINT_DEFAULT + type_idx;

    if (ImGui::Button("Delete Selected", {150,0}))
    {
        std::vector<uint32_t> vids;
        for (size_t pi : m_sel.points)
            vids.push_back(m_points[pi].v);        // collect vertex IDs first

        for (uint32_t vid : vids)
            _erase_vertex_and_fix_paths(vid);      // remove vertex + fix paths

        m_sel.clear();
        _rebuild_vertex_selection();
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

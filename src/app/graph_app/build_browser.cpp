/***********************************************************************************
*
*       ********************************************************************
*       ****      B U I L D _ B R O W S E R . C P P  ____  F I L E      ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond
*               DATED:      June 11, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/graph_app/_editor.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//  0.      STATIC VARIABLES...
// *************************************************************************** //
// *************************************************************************** //
using               Channel                     = editor::Channel;
using               State                       = editor::State;
using               BrushShape                  = editor::BrushShape;
using               CBDragPoint                 = editor::DragPoint;
using               PopupAction                 = editor::PopupAction;
using               PointType                   = editor::PointType;
using               BuildCtx                    = editor::BuildCtx;
using               MenuScope                   = editor::MenuScope;
//


//  0.      STATIC FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

static inline void remove_selection(std::vector<int>& vec, int idx) {
    vec.erase(std::remove(vec.begin(), vec.end(), idx), vec.end());
}




// *************************************************************************** //
//
//
//  ?.      MAIN BROWSER FUNCTION...
// *************************************************************************** //
// *************************************************************************** //


// ── public façade: call once per frame in Build mode ───────────────────────
void SketchWidget::draw_point_browser()
{
    // keep anchor & list in sync with external plot selection
    if (!m_selected.empty())
        m_pt_anchor_idx = m_selected.front();
    if (m_pt_anchor_idx >= int(m_points.size()))
        m_pt_anchor_idx = -1;

    auto make_label = [](const CBDragPoint& p) {
        char buf[16]; std::snprintf(buf, sizeof(buf), "P%02d", p.id);
        return std::string(buf);
    };

    draw_point_list_column(make_label);
    ImGui::SameLine();
    draw_point_inspector_column();
}







// *************************************************************************** //
//
//
//  ?.      BROWSER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

void SketchWidget::draw_point_list_column(
        const std::function<std::string(const CBDragPoint&)>& label_fn)
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui::BeginChild("##pt_list", ImVec2(m_pt_left_w, 0),
                      ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);

    // filter bar
    ImGui::SetNextItemWidth(-FLT_MIN);
    if (ImGui::InputTextWithHint("##pt_filter", "filter",
                                 m_pt_filter.InputBuf,
                                 IM_ARRAYSIZE(m_pt_filter.InputBuf)))
        m_pt_filter.Build();

    // rows
    for (size_t i = 0; i < m_points.size(); ++i) {
        const CBDragPoint& p   = m_points[i];
        std::string        lbl = label_fn(p);
        if (!m_pt_filter.PassFilter(lbl.c_str()))
            continue;

        bool sel = is_selected(int(i));
        if (ImGui::Selectable(lbl.c_str(), sel, ImGuiSelectableFlags_None)) {
            bool shift = io.KeyShift;
            bool ctrl  = io.KeyCtrl;

            if (!shift && !ctrl) {                       // plain click
                clear_selection();
                add_selection(int(i), false);
                m_pt_anchor_idx = int(i);
            }
            else if (ctrl) {                             // toggle
                if (sel)  remove_selection(m_selected, int(i));
                else      add_selection(int(i), true);
                m_pt_anchor_idx = int(i);
            }
            else if (shift && m_pt_anchor_idx != -1) {   // range
                int a = std::min(m_pt_anchor_idx, int(i));
                int b = std::max(m_pt_anchor_idx, int(i));
                clear_selection();
                for (int j = a; j <= b; ++j)
                    add_selection(j, true);
            }
        }
    }

    ImGui::EndChild();
}



// ── helper: right column (inspector) ───────────────────────────────────────
void SketchWidget::draw_point_inspector_column()
{
    ImGui::BeginChild("##pt_inspector", ImVec2(0, 0),
                      ImGuiChildFlags_Borders);

    // ────────────────────────────────────────────────────────────────────
    //  0 selected
    // ────────────────────────────────────────────────────────────────────
    if (m_selected.empty()) {
        ImGui::TextUnformatted("No point selected.");
        ImGui::EndChild();
        return;
    }

    // ────────────────────────────────────────────────────────────────────
    //  exactly 1 selected  →  full editor + connections list
    // ────────────────────────────────────────────────────────────────────
    if (m_selected.size() == 1) {
        int idx = m_selected[0];
        CBDragPoint& p = m_points[idx];

        // basic props
        double x_min = 0.0,             y_min = 0.0;
        double x_max = double(m_res_x), y_max = double(m_res_y);
        ImGui::Text("ID: %d", p.id);    ImGui::Separator();
        ImGui::SliderScalar("X", ImGuiDataType_Double,
                            &p.pos.x, &x_min, &x_max, "%.0f");
        ImGui::SliderScalar("Y", ImGuiDataType_Double,
                            &p.pos.y, &y_min, &y_max, "%.0f");
        int t = int(p.type);
        if (ImGui::Combo("Type", &t,
                         POINT_TYPE_LABELS.data(),
                         int(PointType::Count)))
            propagate_type(p.id, PointType(t));

        // connections
        ImGui::Separator();
        ImGui::TextUnformatted("Connections:");
        int conn_count = 0;
        for (auto it = m_lines.begin(); it != m_lines.end(); /*++it*/) {
            const CBLine& ln = *it;
            if (ln.a == idx || ln.b == idx) {
                ++conn_count;
                int peer = (ln.a == idx) ? ln.b : ln.a;
                ImGui::BulletText("P%02d", peer);
                ImGui::SameLine();
                char btn_id[16]; std::snprintf(btn_id, sizeof(btn_id), "X##d%d", peer);
                if (ImGui::SmallButton(btn_id)) {
                    // remove this specific line
                    it = m_lines.erase(std::remove_if(it, m_lines.end(),
                                [idx,peer](const CBLine& l){
                                    return (l.a==idx && l.b==peer) ||
                                           (l.a==peer && l.b==idx);
                                }), m_lines.end());
                    continue; // 'it' already advanced by erase
                }
                ++it;
            } else ++it;
        }
        if (conn_count == 0)
            ImGui::TextDisabled("— none —");
        ImGui::EndChild();
        return;
    }

    // ────────────────────────────────────────────────────────────────────
    //  exactly 2 selected  →  bulk connect / disconnect + type change
    // ────────────────────────────────────────────────────────────────────
    if (m_selected.size() == 2) {
        int i = m_selected[0];
        int j = m_selected[1];
        CBDragPoint& p0 = m_points[i];
        CBDragPoint& p1 = m_points[j];

        ImGui::Text("P%02d  ↔  P%02d", p0.id, p1.id); ImGui::Separator();

        // connect / disconnect
        bool already = line_exists(i, j);
        const char* lbl = already ? "Disconnect" : "Connect";
        if (ImGui::Button(lbl, ImVec2(-FLT_MIN, 0))) {
            if (already) {
                m_lines.erase(std::remove_if(m_lines.begin(), m_lines.end(),
                              [i,j](const CBLine& ln){
                                  return (ln.a==i && ln.b==j) ||
                                         (ln.a==j && ln.b==i);
                              }), m_lines.end());
            }
            else add_line(i, j);
        }

        // bulk type change (both must share)
        ImGui::Separator();
        int common_type = (p0.type == p1.type) ? int(p0.type) : int(PointType::Unassigned);
        if (ImGui::Combo("Type", &common_type,
                         POINT_TYPE_LABELS.data(),
                         int(PointType::Count)))
        {
            propagate_type(i, PointType(common_type));
            propagate_type(j, PointType(common_type));
        }

        ImGui::EndChild();
        return;
    }

    // ────────────────────────────────────────────────────────────────────
    //  >2 selected  →  bulk operations (type, delete)
    // ────────────────────────────────────────────────────────────────────
    ImGui::Text("%d points selected", int(m_selected.size()));
    ImGui::Separator();

    int common_type = int(m_points[m_selected[0]].type);
    for (int idx : m_selected)
        if (int(m_points[idx].type) != common_type) { common_type = -1; break; }

    int t = (common_type >= 0) ? common_type : int(PointType::Unassigned);
    if (ImGui::Combo("Type", &t,
                     POINT_TYPE_LABELS.data(),
                     int(PointType::Count)))
    {
        for (int idx : m_selected)
            propagate_type(idx, PointType(t));
    }

    if (ImGui::Button("Delete Selected"))
        delete_selected_points();

    ImGui::EndChild();
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

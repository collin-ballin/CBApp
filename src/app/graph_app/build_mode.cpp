/***********************************************************************************
*
*       ********************************************************************
*       ****         B U I L D _ M O D E . C P P  ____  F I L E         ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond
*               DATED:      June 4, 2025.
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
//using               POINT_TYPE_LABELS           = SketchWidget::POINT_TYPE_LABELS;


static constexpr ImVec4         COL_TYPE_A          = ImVec4(0.00f, 0.48f, 1.00f, 1.00f);   // blue
static constexpr ImVec4         COL_TYPE_B          = ImVec4(1.00f, 0.23f, 0.19f, 1.00f);   // red
static constexpr ImVec4         COL_TYPE_C          = ImVec4(0.10f, 0.80f, 0.10f, 1.00f);   // green
static constexpr ImVec4         COL_UNASN           = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);   // light gray
static constexpr ImVec4         COL_HELD            = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);   // gray while dragging
static constexpr ImVec4         COL_SELECTED        = ImVec4(0.95f, 0.75f, 0.05f, 1.00f);   // gold highlight
static constexpr ImU32          COL_LASSO_OUT       = IM_COL32(255,215,0,255);   // gold outline
static constexpr ImU32          COL_LASSO_FILL      = IM_COL32(255,215,0,40);    // translucent fill
static constexpr float          POINT_RADIUS        = 10.0f;                                //  radius in pixels



//  0.      STATIC FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "GetIdleColor"
//
inline ImVec4 GetIdleColor(PointType t) {
    switch (t) {
        case PointType::TypeA:      return COL_TYPE_A;
        case PointType::TypeB:      return COL_TYPE_B;
        case PointType::TypeC:      return COL_TYPE_C;
        default:                    return COL_UNASN;
    }
}


//  "SnapToGrid"
//
static inline ImPlotPoint SnapToGrid(const ImPlotPoint& p,
                                     int max_x, int max_y,
                                     bool halfSnap)
{
    if (!halfSnap) {
        int ix = std::clamp(int(std::round(p.x)), 0, max_x);
        int iy = std::clamp(int(std::round(p.y)), 0, max_y);
        return ImPlotPoint(double(ix), double(iy));
    }
    // Half-step snapping: …0.5, 1.5, 2.5, …
    double x = std::round(p.x - 0.5) + 0.5;
    double y = std::round(p.y - 0.5) + 0.5;
    x = std::clamp(x, 0.5, double(max_x) - 0.5);
    y = std::clamp(y, 0.5, double(max_y) - 0.5);
    return ImPlotPoint(x, y);
}


//  "build_context_menu"
//
static PopupAction build_context_menu(MenuScope scope,
                                      CBDragPoint* opt_pt,
                                      PointType&   out_type,
                                      PointType&   bulk_type_ref) {
    PopupAction action = PopupAction::None;

    if (ImGui::MenuItem("Delete")) {
        action = PopupAction::Delete;
    }

    // ── Type submenu ────────────────────────────────────────────────────
    if (ImGui::BeginMenu("Type")) {
        for (int t = 0; t < int(PointType::Count); ++t) {
            bool sel = (scope == MenuScope::Single) ?
                       (opt_pt && opt_pt->type == PointType(t)) : false;
            if (ImGui::Selectable(SketchWidget::POINT_TYPE_LABELS[t], sel)) {
                if (scope == MenuScope::Single && opt_pt)
                    opt_pt->type = PointType(t);
                else
                    bulk_type_ref = PointType(t);
                action = PopupAction::SetType;
                out_type = PointType(t);
            }
            if (sel) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndMenu();
    }

    // ── Properties (only for single‑point) ─────────────────────────────
    if (scope == MenuScope::Single && opt_pt) {
        if (ImGui::BeginMenu("Properties")) {
            int temp_x = int(std::round(opt_pt->pos.x));
            int temp_y = int(std::round(opt_pt->pos.y));

            ImGui::PushID("coords_int");
            ImGui::TextUnformatted("X");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(60);
            if (ImGui::InputInt("##x", &temp_x, 1, 10))
                opt_pt->pos.x = double(temp_x);

            ImGui::SameLine();
            ImGui::TextUnformatted("Y");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(60);
            if (ImGui::InputInt("##y", &temp_y, 1, 10))
                opt_pt->pos.y = double(temp_y);
            ImGui::PopID();
            ImGui::EndMenu();
        }
    }
    return action;
}






// *************************************************************************** //
//
//
//  ?.      BUILD-MODE HELPER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "is_selected"
//
bool SketchWidget::is_selected(int idx) const {
    return std::find(m_selected.begin(), m_selected.end(), idx) != m_selected.end();
}

//  "clear_selection"
//
void SketchWidget::clear_selection() { m_selected.clear(); }

//  "add_selection"
//
void SketchWidget::add_selection(int idx, bool additive) {
    if (!additive) clear_selection();                // normal click ⇒ exclusive
    if (!is_selected(idx)) m_selected.push_back(idx);
}



// *************************************************************************** //
//
//
//  ?.      BUILD-MODE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

// Begin a new lasso (called when LMB pressed on empty space)
void SketchWidget::begin_lasso(const ImPlotPoint& start, bool additive) {
    m_lasso_active   = true;
    m_lasso_additive = additive;
    m_lasso_start    = start;
    m_lasso_end      = start;      // init to same point
    if (!additive) clear_selection();
}

// Update rectangle while mouse held
void SketchWidget::update_lasso_end(const ImPlotPoint& curr) {
    m_lasso_end = curr;
}

// Finish lasso: select all points whose positions lie inside rect
void SketchWidget::finalize_lasso() {
    if (!m_lasso_active) return;
    ImPlotPoint p0{ std::min(m_lasso_start.x, m_lasso_end.x),
                    std::min(m_lasso_start.y, m_lasso_end.y) };
    ImPlotPoint p1{ std::max(m_lasso_start.x, m_lasso_end.x),
                    std::max(m_lasso_start.y, m_lasso_end.y) };
    for (size_t i = 0; i < m_points.size(); ++i) {
        const ImPlotPoint& pos = m_points[i].pos;
        if (pos.x >= p0.x && pos.x <= p1.x &&
            pos.y >= p0.y && pos.y <= p1.y)
            add_selection(int(i), /*additive=*/true);
    }
    m_lasso_active = false;
}

// Draw translucent rectangle overlay in screen space
void SketchWidget::draw_lasso() const {
    if (!m_lasso_active) return;
    ImVec2 p0 = ImPlot::PlotToPixels(m_lasso_start);
    ImVec2 p1 = ImPlot::PlotToPixels(m_lasso_end);
    ImDrawList* dl = ImGui::GetForegroundDrawList();
    dl->AddRectFilled(p0, p1, COL_LASSO_FILL);
    dl->AddRect(p0, p1, COL_LASSO_OUT, 0.0f, 0, 1.5f);
}




//  "delete_selected_points"
//
void SketchWidget::delete_selected_points()
{
    if (m_selected.empty())
        return;

    // 1. Sort the list of indices to remove (ascending)
    std::sort(m_selected.begin(), m_selected.end());      //  e.g. [1,4,7]
    const std::vector<int>& removed = m_selected;         //  alias for clarity

    // 2. Erase any line that references a soon-to-be-deleted point
    m_lines.erase(std::remove_if(m_lines.begin(), m_lines.end(),
                  [&](const CBLine& ln){
                      return std::binary_search(removed.begin(), removed.end(), ln.a) ||
                             std::binary_search(removed.begin(), removed.end(), ln.b);
                  }),
                  m_lines.end());

    // 3. Re-map point indices for surviving lines
    auto remap = [&](int idx) {
        // subtract how many removed indices are strictly less than idx
        return idx - int(std::lower_bound(removed.begin(), removed.end(), idx) - removed.begin());
    };
    for (auto& ln : m_lines) {
        ln.a = remap(ln.a);
        ln.b = remap(ln.b);
    }

    // 4. Delete points (DESC order so earlier erases don’t shift later ones)
    for (auto it = removed.rbegin(); it != removed.rend(); ++it)
        m_points.erase(m_points.begin() + *it);

    // 5. Renumber point IDs
    for (std::size_t i = 0; i < m_points.size(); ++i)
        m_points[i].id = int(i);

    // 6. Renumber line IDs (purely cosmetic stability)
    for (std::size_t k = 0; k < m_lines.size(); ++k)
        m_lines[k].id = int(k);

    clear_selection();
}



// *************************************************************************** //
//
//
//  NEW BUILD MODE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //


//  "show_point_popup"
//
PopupAction SketchWidget::show_point_popup(CBDragPoint& pt)
{
    char buf[32];
    std::snprintf(buf, sizeof(buf), "PointPopup_%d", pt.id);
    if (!ImGui::BeginPopup(buf))
        return PopupAction::None;

    PointType dummy = PointType::Unassigned; // not used in single‑scope path
    PopupAction act = build_context_menu(MenuScope::Single, &pt, dummy, m_bulk_type);

    ImGui::EndPopup();
    return act;
}


//  "show_multi_popup"
//
PopupAction SketchWidget::show_multi_popup(PointType& out_type)
{
    if (!ImGui::BeginPopup("SelectionPopup"))
        return PopupAction::None;

    PopupAction action = PopupAction::None;

    // --- Properties submenu for exactly two points of same type -------------
    this->connect_points();
    /*if (m_selected.size() == 2) {
        int i = m_selected[0];
        int j = m_selected[1];
        if (m_points[i].type == m_points[j].type) {
            if (ImGui::BeginMenu("Properties")) {
                bool already = line_exists(i, j);
                const char* lbl = already ? "Disconnect" : "Connect";
                if (ImGui::MenuItem(lbl)) {
                    if (already) {
                        // remove line
                        m_lines.erase(std::remove_if(m_lines.begin(), m_lines.end(),
                                      [i,j](const CBLine& ln){
                                          return (ln.a == i && ln.b == j) ||
                                                 (ln.a == j && ln.b == i);
                                      }),
                                      m_lines.end());
                    }
                    else {
                        add_line(i, j);
                    }
                }
                ImGui::EndMenu();
            }
        }
    }*/

    // --- Standard multi-selection items (Type ▸, separator, Delete) ---------
    PopupAction base = build_context_menu(MenuScope::Multi,
                                          nullptr,          // no single point
                                          out_type,
                                          m_bulk_type);
    if (base != PopupAction::None)
        action = base;

    ImGui::EndPopup();
    return action;
}








//-------------------------------------------------------------------------
//  show_selection_popup : bulk context menu for current selection
//  (requires `PointType m_bulk_type` and PopupAction prototype in header)
//-------------------------------------------------------------------------
PopupAction SketchWidget::show_selection_popup()
{
    PopupAction action = PopupAction::None;
    
    if ( ImGui::BeginPopup("SelectionPopup") )
    {
        if (ImGui::MenuItem("Delete")) {
            action = PopupAction::Delete;
        }

        if (ImGui::BeginMenu("Type")) {
            for (int t = 0; t < int(PointType::Count); ++t) {
                if (ImGui::Selectable(POINT_TYPE_LABELS[t])) {
                    m_bulk_type = PointType(t);
                    action      = PopupAction::SetType;
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }
    return action;
}






// *************************************************************************** //
//
//
//  ?.      LINE-DRAWING FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

bool SketchWidget::line_exists(int i, int j) const {
    if (i > j) std::swap(i, j);
    for (const auto& ln : m_lines)
        if (ln.a == i && ln.b == j) return true;
    return false;
}

void SketchWidget::add_line(int i, int j) {
    if (i > j) std::swap(i, j);
    if (line_exists(i, j)) return; // already there
    m_lines.push_back({ int(m_lines.size()), i, j, m_points[i].type });
}

void SketchWidget::remove_lines_with_point(int idx) {
    m_lines.erase(std::remove_if(m_lines.begin(), m_lines.end(),
                 [idx](const CBLine& ln){ return ln.a == idx || ln.b == idx; }),
                 m_lines.end());
    // re‑index ids for stability
    for (size_t k = 0; k < m_lines.size(); ++k) m_lines[k].id = int(k);
}

// call this at END of build_update_points() after point positions are final:
void SketchWidget::render_lines()
{
    for (const auto& ln : m_lines) {
        const ImPlotPoint& p0 = m_points[ln.a].pos;
        const ImPlotPoint& p1 = m_points[ln.b].pos;

        double xs[2] = { p0.x, p1.x };
        double ys[2] = { p0.y, p1.y };

        // build a unique, unlabeled ID (lines are visual only)
        char label[32];
        std::snprintf(label, sizeof(label), "##ln%d", ln.id);

        // set line color (thickness 1 px)
        ImPlot::SetNextLineStyle(GetIdleColor(ln.type), 1.0f);
        ImPlot::PlotLine(label, xs, ys, 2);
    }
}






// *************************************************************************** //
//
//
//  NEW BUILD MODE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

void SketchWidget::build_add_new_point(const BuildCtx& ctx) {
    if (ctx.plot_hovered && ctx.left_click && ctx.ctrl &&
        m_points.size() < size_t(k_max_points))
    {
        m_points.push_back({ int(m_points.size()), ctx.mouse_plot });
    }
}


// Updated build_update_points (with render_lines call at top)
void SketchWidget::build_update_points(const BuildCtx& ctx,
                                       PopupAction&   global_action,
                                       PointType&     set_type)
{
    // 1. Draw all existing lines before points so they sit behind
    render_lines();

    m_internal_click_on_pt = false;

    for (size_t i = 0; i < m_points.size(); ++i) {
        auto& pt = m_points[i];

        // A) Selection: if left-clicked on this point (and not already held), select it
        if (ctx.left_click && pt.hovered && !pt.held) {
            add_selection(int(i), ctx.shift);
            m_internal_click_on_pt = true;
        }

        bool sel = is_selected(int(i));

        // B) Determine color: held? dark gray, selected? gold, else idle by type
        ImVec4 col = pt.held
                        ? COL_HELD
                        : (sel ? COL_SELECTED : GetIdleColor(pt.type));

        // C) Draw & drag the point
        ImPlot::DragPoint(pt.id,
                          &pt.pos.x, &pt.pos.y,
                          col,
                          POINT_RADIUS,
                          ImPlotDragToolFlags_None,
                          &pt.clicked, &pt.hovered, &pt.held);

        // D) Snap to grid based on point type
        switch (pt.type) {
            case PointType::TypeA:
                pt.pos = SnapToGrid(pt.pos, m_res_x, m_res_y, /*halfSnap=*/false);
                break;
            case PointType::TypeB:
                pt.pos = SnapToGrid(pt.pos, m_res_x, m_res_y, /*halfSnap=*/true);
                break;
            case PointType::TypeC:
                pt.pos = SnapToGrid(pt.pos, m_res_x, m_res_y, /*halfSnap=*/false);
                break;
            default:
                // Unassigned: clamp to bounds
                pt.pos.x = std::clamp(pt.pos.x, 0.0, double(m_res_x));
                pt.pos.y = std::clamp(pt.pos.y, 0.0, double(m_res_y));
                break;
        }

        // E) If right-clicked while hovering, open the single-point popup
        if (pt.hovered && ctx.right_click) {
            ImVec2 px = ImPlot::PlotToPixels(pt.pos);
            ImGui::SetNextWindowPos(px, ImGuiCond_Appearing, {0, 1});
            char buf[32];
            std::snprintf(buf, sizeof(buf), "PointPopup_%d", pt.id);
            ImGui::OpenPopup(buf);
        }

        // F) Process the single-point popup
        PopupAction act = show_point_popup(pt);
        if (act == PopupAction::Delete) {
            global_action = PopupAction::Delete;
        }
        else if (act == PopupAction::SetType) {
            global_action = PopupAction::SetType;
            set_type      = pt.type;
        }
    }
}


//  "build_handle_selection_popup"
//
//      Updated build_handle_selection_popup
//      (includes Connect/Disconnect menu entries when exactly two same-type points are selected)
void SketchWidget::build_handle_selection_popup(const BuildCtx &    ctx,
                                                PopupAction &       global_action,
                                                PointType &         set_type)
{
    // 1. If right-click on plot, no point under cursor, and at least one point is selected
    if (ctx.right_click && ctx.plot_hovered && !m_internal_click_on_pt && !m_selected.empty()) {
        ImGui::SetNextWindowPos(ImGui::GetIO().MousePos, ImGuiCond_Appearing, {0, 0});
        ImGui::OpenPopup("SelectionPopup");
    }

    // 2. Show the unified multi-selection popup (Type submenu + Delete at bottom)
    PopupAction sel_act = show_multi_popup(set_type);
    if (sel_act != PopupAction::None) {
        global_action = sel_act;  // Delete or SetType
    }

    
    // 3. If exactly two points are selected, and they share the same type, show Connect/Disconnect
    if (m_selected.size() == 2)
    {
        //  int     i           = m_selected[0];
        //  int     j           = m_selected[1];
        //  bool    same_type   = (m_points[i].type == m_points[j].type);
        
        //  if (same_type)
        //  {
        //      bool            already     = line_exists(i, j);
        //      const char *    lbl         = already ? "Disconnect" : "Connect";
        //
        //      if (ImGui::MenuItem(lbl))
        //      {
        //          if (already)    //      Remove the existing line between i and j
        //          {
        //              m_lines.erase(std::remove_if(m_lines.begin(), m_lines.end(),
        //                            [i, j](const CBLine& ln) {
        //                                return (ln.a == i && ln.b == j) || (ln.a == j && ln.b == i);
        //                            }),
        //                            m_lines.end());
        //          }
        //          else {          //      Add a new line between i and j
        //              add_line(i, j);
        //          }
        //      }
        //  }
    }
    
    return;
}



//  "build_apply_global_action"
//
void SketchWidget::build_apply_global_action(PopupAction global_action, PointType set_type)
{
    switch (global_action) {
        //
        //  1.  DELETE...
        case PopupAction::Delete : {
            std::sort(m_selected.begin(), m_selected.end(), std::greater<int>());
            
            for (int idx : m_selected) if (idx >= 0 && idx < int(m_points.size()))
                m_points.erase(m_points.begin() + idx);
                
            for (size_t j = 0; j < m_points.size(); ++j)
                m_points[j].id = int(j);
                
            clear_selection();
            break;
        }
        //
        //  2.  SET TYPE...
        case PopupAction::SetType : {
            for (int idx : m_selected)
            {
                if ( idx >= 0 && idx < int(m_points.size()) )
                    propagate_type(idx, set_type);
            }
            break;
        }
        //
        //  3.  DEFAULT...
        default : {
        
            break;
        }
    }
    
    
    return;
}


//  "build_update_lasso"
//
void SketchWidget::build_update_lasso(const BuildCtx& ctx) {
    if (ctx.left_click && ctx.plot_hovered && !m_internal_click_on_pt)
        begin_lasso(ctx.mouse_plot, ctx.shift);
    if (m_lasso_active && ImGui::IsMouseDown(ImGuiMouseButton_Left))
        update_lasso_end(ImPlot::GetPlotMousePos());
    if (m_lasso_active && !ImGui::IsMouseDown(ImGuiMouseButton_Left))
        finalize_lasso();

    // clear selection on empty click
    if (ctx.left_click && ctx.plot_hovered && !m_internal_click_on_pt && !ctx.shift && !m_lasso_active)
        clear_selection();
}


//  "build_nudge_selected"
//
void SketchWidget::build_nudge_selected(const BuildCtx&) {
    int dx = (ImGui::IsKeyPressed(ImGuiKey_RightArrow) ? 1 : 0) - (ImGui::IsKeyPressed(ImGuiKey_LeftArrow) ? 1 : 0);
    int dy = (ImGui::IsKeyPressed(ImGuiKey_UpArrow)    ? 1 : 0) - (ImGui::IsKeyPressed(ImGuiKey_DownArrow)  ? 1 : 0);
    if (!dx && !dy) return;
    for (int idx : m_selected) {
        if (idx < 0 || idx >= int(m_points.size())) continue;
        auto& pt = m_points[idx]; if (pt.held) continue;
        pt.pos.x += dx; pt.pos.y += dy;
        switch (pt.type) {
            case PointType::TypeA: pt.pos = SnapToGrid(pt.pos, m_res_x, m_res_y, false); break;
            case PointType::TypeB: pt.pos = SnapToGrid(pt.pos, m_res_x, m_res_y, true ); break;
            case PointType::TypeC: pt.pos = SnapToGrid(pt.pos, m_res_x, m_res_y, false); break;
            default: break;
        }
    }
}






// *************************************************************************** //
//
//
//  ?.      UTILITY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "propagate_type"
//
void SketchWidget::propagate_type(int idx, PointType new_type) {
    if (idx < 0 || idx >= int(m_points.size()))
        return;
    std::vector<bool> visited(m_points.size(), false);
    std::queue<int> bfs;
    bfs.push(idx);
    visited[idx] = true;

    while (!bfs.empty())
    {
        int cur = bfs.front(); bfs.pop();
        m_points[cur].type = new_type;
        for (auto& ln : m_lines)
        {
            if (ln.a == cur || ln.b == cur) {
                ln.type = new_type;
                int other = (ln.a == cur) ? ln.b : ln.a;
                if (!visited[other]) {
                    visited[other] = true;
                    bfs.push(other);
                }
            }
        }
    }
}


//  "connect_points"
//
void SketchWidget::connect_points(void)
{
    if (m_selected.size() != 2)
        return;
    
    int i = m_selected[0];
    int j = m_selected[1];
    
    if (m_points[i].type != m_points[j].type)
        return;
        
    
    if (ImGui::BeginMenu("Properties"))
    {
        bool already = line_exists(i, j);
        const char* lbl = already ? "Disconnect" : "Connect";
        if (ImGui::MenuItem(lbl))
        {
            if (already)// remove line
            {
                m_lines.erase(std::remove_if(m_lines.begin(), m_lines.end(),
                              [i,j](const CBLine& ln){
                                  return (ln.a == i && ln.b == j) ||
                                         (ln.a == j && ln.b == i);
                              }),
                              m_lines.end());
            }
            else
            {
                add_line(i, j);
            }
        }
        ImGui::EndMenu();
    }
//ImPlot::DragLineY(120482,&f,ImVec4(1,0.5f,1,1),1,flags, &clicked, &hovered, &held);

    return;
}













// *************************************************************************** //
//
//
//  ?.      ETCH-A-SKETCH V5 STUFF...
// *************************************************************************** //
// *************************************************************************** //





























// *************************************************************************** //
//
//
//  ?.      UTILITY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //


/*

void Demo_Querying() {
    static ImVector<ImPlotPoint> data;
    static ImVector<ImPlotRect> rects;
    static ImPlotRect limits, select;
    static bool init = true;
    if (init) {
        for (int i = 0; i < 50; ++i)
        {
            double x = RandomRange(0.1, 0.9);
            double y = RandomRange(0.1, 0.9);
            data.push_back(ImPlotPoint(x,y));
        }
        init = false;
    }

    ImGui::BulletText("Box select and left click mouse to create a new query rect.");
    ImGui::BulletText("Ctrl + click in the plot area to draw points.");

    if (ImGui::Button("Clear Queries"))
        rects.shrink(0);

    if (ImPlot::BeginPlot("##Centroid")) {
        ImPlot::SetupAxesLimits(0,1,0,1);
        if (ImPlot::IsPlotHovered() && ImGui::IsMouseClicked(0) && ImGui::GetIO().KeyCtrl) {
            ImPlotPoint pt = ImPlot::GetPlotMousePos();
            data.push_back(pt);
        }
        ImPlot::PlotScatter("Points", &data[0].x, &data[0].y, data.size(), 0, 0, 2 * sizeof(double));
        if (ImPlot::IsPlotSelected()) {
            select = ImPlot::GetPlotSelection();
            int cnt;
            ImPlotPoint centroid = FindCentroid(data,select,cnt);
            if (cnt > 0) {
                ImPlot::SetNextMarkerStyle(ImPlotMarker_Square,6);
                ImPlot::PlotScatter("Centroid", &centroid.x, &centroid.y, 1);
            }
            if (ImGui::IsMouseClicked(ImPlot::GetInputMap().SelectCancel)) {
                CancelPlotSelection();
                rects.push_back(select);
            }
        }
        for (int i = 0; i < rects.size(); ++i) {
            int cnt;
            ImPlotPoint centroid = FindCentroid(data,rects[i],cnt);
            if (cnt > 0) {
                ImPlot::SetNextMarkerStyle(ImPlotMarker_Square,6);
                ImPlot::PlotScatter("Centroid", &centroid.x, &centroid.y, 1);
            }
            ImPlot::DragRect(i,&rects[i].X.Min,&rects[i].Y.Min,&rects[i].X.Max,&rects[i].Y.Max,ImVec4(1,0,1,1));
        }
        limits  = ImPlot::GetPlotLimits();
        ImPlot::EndPlot();
    }
}




void Demo_DragPoints() {
    ImGui::BulletText("Click and drag each point.");
    static ImPlotDragToolFlags flags = ImPlotDragToolFlags_None;
    ImGui::CheckboxFlags("NoCursors", (unsigned int*)&flags, ImPlotDragToolFlags_NoCursors); ImGui::SameLine();
    ImGui::CheckboxFlags("NoFit", (unsigned int*)&flags, ImPlotDragToolFlags_NoFit); ImGui::SameLine();
    ImGui::CheckboxFlags("NoInput", (unsigned int*)&flags, ImPlotDragToolFlags_NoInputs);
    ImPlotAxisFlags ax_flags = ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoTickMarks;
    bool clicked[4] = {false, false, false, false};
    bool hovered[4] = {false, false, false, false};
    bool held[4]    = {false, false, false, false};
    if (ImPlot::BeginPlot("##Bezier",ImVec2(-1,0),ImPlotFlags_CanvasOnly)) {
        ImPlot::SetupAxes(nullptr,nullptr,ax_flags,ax_flags);
        ImPlot::SetupAxesLimits(0,1,0,1);
        static ImPlotPoint P[] = {ImPlotPoint(.05f,.05f), ImPlotPoint(0.2,0.4),  ImPlotPoint(0.8,0.6),  ImPlotPoint(.95f,.95f)};

        ImPlot::DragPoint(0,&P[0].x,&P[0].y, ImVec4(0,0.9f,0,1),4,flags, &clicked[0], &hovered[0], &held[0]);
        ImPlot::DragPoint(1,&P[1].x,&P[1].y, ImVec4(1,0.5f,1,1),4,flags, &clicked[1], &hovered[1], &held[1]);
        ImPlot::DragPoint(2,&P[2].x,&P[2].y, ImVec4(0,0.5f,1,1),4,flags, &clicked[2], &hovered[2], &held[2]);
        ImPlot::DragPoint(3,&P[3].x,&P[3].y, ImVec4(0,0.9f,0,1),4,flags, &clicked[3], &hovered[3], &held[3]);

        static ImPlotPoint B[100];
        for (int i = 0; i < 100; ++i) {
            double t  = i / 99.0;
            double u  = 1 - t;
            double w1 = u*u*u;
            double w2 = 3*u*u*t;
            double w3 = 3*u*t*t;
            double w4 = t*t*t;
            B[i] = ImPlotPoint(w1*P[0].x + w2*P[1].x + w3*P[2].x + w4*P[3].x, w1*P[0].y + w2*P[1].y + w3*P[2].y + w4*P[3].y);
        }

        ImPlot::SetNextLineStyle(ImVec4(1,0.5f,1,1),hovered[1]||held[1] ? 2.0f : 1.0f);
        ImPlot::PlotLine("##h1",&P[0].x, &P[0].y, 2, 0, 0, sizeof(ImPlotPoint));
        ImPlot::SetNextLineStyle(ImVec4(0,0.5f,1,1), hovered[2]||held[2] ? 2.0f : 1.0f);
        ImPlot::PlotLine("##h2",&P[2].x, &P[2].y, 2, 0, 0, sizeof(ImPlotPoint));
        ImPlot::SetNextLineStyle(ImVec4(0,0.9f,0,1), hovered[0]||held[0]||hovered[3]||held[3] ? 3.0f : 2.0f);
        ImPlot::PlotLine("##bez",&B[0].x, &B[0].y, 100, 0, 0, sizeof(ImPlotPoint));
        ImPlot::EndPlot();
    }
}

Demo_DragPoints
void Demo_DragLines() {
    ImGui::BulletText("Click and drag the horizontal and vertical lines.");
    static double x1 = 0.2;
    static double x2 = 0.8;
    static double y1 = 0.25;
    static double y2 = 0.75;
    static double f = 0.1;
    bool clicked = false;
    bool hovered = false;
    bool held = false;
    static ImPlotDragToolFlags flags = ImPlotDragToolFlags_None;
    ImGui::CheckboxFlags("NoCursors", (unsigned int*)&flags, ImPlotDragToolFlags_NoCursors); ImGui::SameLine();
    ImGui::CheckboxFlags("NoFit", (unsigned int*)&flags, ImPlotDragToolFlags_NoFit); ImGui::SameLine();
    ImGui::CheckboxFlags("NoInput", (unsigned int*)&flags, ImPlotDragToolFlags_NoInputs);
    if (ImPlot::BeginPlot("##lines",ImVec2(-1,0))) {
        ImPlot::SetupAxesLimits(0,1,0,1);
        ImPlot::DragLineX(0,&x1,ImVec4(1,1,1,1),1,flags);
        ImPlot::DragLineX(1,&x2,ImVec4(1,1,1,1),1,flags);
        ImPlot::DragLineY(2,&y1,ImVec4(1,1,1,1),1,flags);
        ImPlot::DragLineY(3,&y2,ImVec4(1,1,1,1),1,flags);
        double xs[1000], ys[1000];
        for (int i = 0; i < 1000; ++i) {
            xs[i] = (x2+x1)/2+fabs(x2-x1)*(i/1000.0f - 0.5f);
            ys[i] = (y1+y2)/2+fabs(y2-y1)/2*sin(f*i/10);
        }
        ImPlot::DragLineY(120482,&f,ImVec4(1,0.5f,1,1),1,flags, &clicked, &hovered, &held);
        ImPlot::SetNextLineStyle(IMPLOT_AUTO_COL, hovered||held ? 2.0f : 1.0f);
        ImPlot::PlotLine("Interactive Data", xs, ys, 1000);
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_DragRects() {

    static float x_data[512];
    static float y_data1[512];
    static float y_data2[512];
    static float y_data3[512];
    static float sampling_freq = 44100;
    static float freq = 500;
    bool clicked = false;
    bool hovered = false;
    bool held = false;
    for (size_t i = 0; i < 512; ++i) {
        const float t = i / sampling_freq;
        x_data[i] = t;
        const float arg = 2 * 3.14f * freq * t;
        y_data1[i] = sinf(arg);
        y_data2[i] = y_data1[i] * -0.6f + sinf(2 * arg) * 0.4f;
        y_data3[i] = y_data2[i] * -0.6f + sinf(3 * arg) * 0.4f;
    }
    ImGui::BulletText("Click and drag the edges, corners, and center of the rect.");
    ImGui::BulletText("Double click edges to expand rect to plot extents.");
    static ImPlotRect rect(0.0025,0.0045,0,0.5);
    static ImPlotDragToolFlags flags = ImPlotDragToolFlags_None;
    ImGui::CheckboxFlags("NoCursors", (unsigned int*)&flags, ImPlotDragToolFlags_NoCursors); ImGui::SameLine();
    ImGui::CheckboxFlags("NoFit", (unsigned int*)&flags, ImPlotDragToolFlags_NoFit); ImGui::SameLine();
    ImGui::CheckboxFlags("NoInput", (unsigned int*)&flags, ImPlotDragToolFlags_NoInputs);

    if (ImPlot::BeginPlot("##Main",ImVec2(-1,150))) {
        ImPlot::SetupAxes(nullptr,nullptr,ImPlotAxisFlags_NoTickLabels,ImPlotAxisFlags_NoTickLabels);
        ImPlot::SetupAxesLimits(0,0.01,-1,1);
        ImPlot::PlotLine("Signal 1", x_data, y_data1, 512);
        ImPlot::PlotLine("Signal 2", x_data, y_data2, 512);
        ImPlot::PlotLine("Signal 3", x_data, y_data3, 512);
        ImPlot::DragRect(0,&rect.X.Min,&rect.Y.Min,&rect.X.Max,&rect.Y.Max,ImVec4(1,0,1,1),flags, &clicked, &hovered, &held);
        ImPlot::EndPlot();
    }
    ImVec4 bg_col = held ? ImVec4(0.5f,0,0.5f,1) : (hovered ? ImVec4(0.25f,0,0.25f,1) : ImPlot::GetStyle().Colors[ImPlotCol_PlotBg]);
    ImPlot::PushStyleColor(ImPlotCol_PlotBg, bg_col);
    if (ImPlot::BeginPlot("##rect",ImVec2(-1,150), ImPlotFlags_CanvasOnly)) {
        ImPlot::SetupAxes(nullptr,nullptr,ImPlotAxisFlags_NoDecorations,ImPlotAxisFlags_NoDecorations);
        ImPlot::SetupAxesLimits(rect.X.Min, rect.X.Max, rect.Y.Min, rect.Y.Max, ImGuiCond_Always);
        ImPlot::PlotLine("Signal 1", x_data, y_data1, 512);
        ImPlot::PlotLine("Signal 2", x_data, y_data2, 512);
        ImPlot::PlotLine("Signal 3", x_data, y_data3, 512);
        ImPlot::EndPlot();
    }
    ImPlot::PopStyleColor();
    ImGui::Text("Rect is %sclicked, %shovered, %sheld", clicked ? "" : "not ", hovered ? "" : "not ", held ? "" : "not ");
}










*/







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

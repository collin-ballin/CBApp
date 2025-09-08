/***********************************************************************************
*
*       ********************************************************************
*       ****            E D I T O R . C P P  ____  F I L E              ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond.
*               DATED:      May 21, 2025.
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
using   Channel         = editor::Channel;
using   State           = editor::State;
using   BrushShape      = editor::BrushShape;
using   CBDragPoint     = editor::DragPoint;
using   PopupAction     = editor::PopupAction;
using   PointType       = editor::PointType;
using   BuildCtx        = editor::BuildCtx;


static constexpr ImVec4         COL_TYPE_A          = ImVec4(0.00f, 0.48f, 1.00f, 1.00f);   // blue
static constexpr ImVec4         COL_TYPE_B          = ImVec4(1.00f, 0.23f, 0.19f, 1.00f);   // red
static constexpr ImVec4         COL_TYPE_C          = ImVec4(0.10f, 0.80f, 0.10f, 1.00f);   // green
static constexpr ImVec4         COL_UNASN           = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);   // light gray


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









// *************************************************************************** //
//
//
//  1.      INITIALIZATION FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  Explicit Constructor #1.
//
SketchWidget::SketchWidget(utl::PlotCFG & cfg)
    : m_heatmap_id( ImHashStr("Perm_E") ), m_cfg(cfg) { }


// --- NEW CTOR ---------------------------------------------------------------
SketchWidget::SketchWidget(utl::PlotCFG& cfg, std::vector<Channel> channels)
    : m_channels(std::move(channels)), m_heatmap_id(ImHashStr("Perm_E")), m_cfg(cfg)
{
    if (m_channels.empty())
        m_channels.emplace_back();          // guarantee ≥1 channel
    m_active = 0;
}


// *************************************************************************** //
//
//
//  2.      PUBLIC API FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "Begin"
//
bool SketchWidget::Begin(int nx, int ny) {
    auto & ch = current();

    // adapt plot size to leave room for colour scale --------------------------
    ImVec2 avail = ImGui::GetContentRegionAvail();
    m_cfg.graph.size   = { static_cast<float>(avail.x - ch.scale_width - 2*ImGui::GetStyle().ItemSpacing.x), -1 };

    // ensure buffers sized -----------------------------------------------------
    resize_buffers(nx, ny);
    if (!utl::MakePlotCFG(m_cfg))
        return false;

    ImPlot::SetupAxisLimits(ImAxis_X1, 0, m_res_x, ImGuiCond_Always);
    ImPlot::SetupAxisLimits(ImAxis_Y1, 0, m_res_y, ImGuiCond_Always);

    // ────────────────────────────────────────────────────────────────────────
    //  Draw-mode input BEFORE rendering so paint is baked into the buffer
    // ────────────────────────────────────────────────────────────────────────
    if (m_mode == State::Draw)
        draw_mode_input();

    // ────────────────────────────────────────────────────────────────────────
    //  Render heat-map and colour-scale
    // ────────────────────────────────────────────────────────────────────────
    ImPlot::PushColormap(ch.cmap);
    ImPlot::PlotHeatmap(ch.map_title, ch.data.data(),
                        m_res_y, m_res_x,
                        ch.paint_value.limits.min, ch.paint_value.limits.max, ch.map_units,
                        {0,0}, { (double)m_res_x, (double)m_res_y });
    ImGui::SameLine();
    ImPlot::ColormapScale(ch.scale_title, ch.paint_value.limits.min, ch.paint_value.limits.max,
                          { static_cast<float>(ch.scale_width), -1 }, ch.scale_units);
    ImPlot::PopColormap();

    // ────────────────────────────────────────────────────────────────────────
    //  Build-mode interaction AFTER heat-map so points appear on top
    // ────────────────────────────────────────────────────────────────────────
    if (m_mode == State::Build)
        build_mode_input();

    return true;
}


//  "End"
//
void SketchWidget::End(void)
{
    if (m_mode == State::Draw && ImPlot::IsPlotHovered()) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_None);

        const ImPlotPoint mp     = ImPlot::GetPlotMousePos();
        const ImVec2      center = ImPlot::PlotToPixels(mp);

        const float unit_px = ImPlot::PlotToPixels(ImPlotPoint(1, 0)).x -
                              ImPlot::PlotToPixels(ImPlotPoint(0, 0)).x;
        const float half    = m_brush_size * unit_px * 0.5f;

        ImDrawList* dl = ImGui::GetForegroundDrawList();
        const ImU32   col = IM_COL32(255, 0, 0, 255);



        //  SELECT SPECIFIC BRUSH CASES...
        switch (m_brush_shape) {
        //
            case BrushShape::Square : {         //  CASE 1 : SQUARE.
                dl->AddRect({center.x - half, center.y - half},
                            {center.x + half, center.y + half},
                            col, 0.0f, 0, 2.0f);
                break;
            }
        //
            case BrushShape::Circle : {         //  CASE 2 : CIRCLE.
                dl->AddCircle(center, half, col, 0, 2.0f);
                break;
            }
        //
        //
        //
            default : {
                break;
            }
        //
        }
    }

    ImPlot::EndPlot();
}



// *************************************************************************** //
//
//
//  ?.      BUILD-MODE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "build_mode_input"
//
void SketchWidget::build_mode_input() {
    BuildCtx ctx = editor::make_build_ctx();
    PopupAction global_action = PopupAction::None;
    PointType   set_type      = PointType::Unassigned;

    build_add_new_point(ctx);
    build_update_points(ctx, global_action, set_type);
    build_handle_selection_popup(ctx, global_action, set_type);
    build_apply_global_action(global_action, set_type);
    build_update_lasso(ctx);
    build_nudge_selected(ctx);
    draw_lasso();
    // Delete key shortcut
    if (ctx.plot_hovered && ImGui::IsKeyPressed(ImGuiKey_Delete) && !m_selected.empty())
        delete_selected_points();
}






// *************************************************************************** //
//
//
//  3.      "DRAW-MODE" STATE - SPECIFIC FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

void SketchWidget::draw_mode_input() {
    if (ImPlot::IsPlotHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
        // mouse position in plot space (continuous)
        ImPlotPoint mp = ImPlot::GetPlotMousePos();

        // convert to integer grid indices, clamped to valid range
        int gx = static_cast<int>(std::floor(mp.x + 0.5));
        int gy = static_cast<int>(std::floor(mp.y + 0.5));

        // flip Y so row‑0 corresponds to the bottom of the plot
        gy = m_res_y - 1 - gy;

        // guard against out‑of‑bounds clicks
        if (gx < 0 || gx >= m_res_x || gy < 0 || gy >= m_res_y) {
            m_drawing = false;
            reset_prev();
            return;
        }

        if (m_prev_x < 0)
            draw_line(gx, gy, gx, gy);
        else
            draw_line(m_prev_x, m_prev_y, gx, gy);

        m_drawing = true;
        m_prev_x  = gx;
        m_prev_y  = gy;
    }
    else {
        m_drawing = false;
        reset_prev();
    }
}








// *************************************************************************** //
//
//
//  ?.      PRIVATE / PROTECTED FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "stamp"
//
void SketchWidget::stamp(int gx, int gy)
{
    auto &      ch          = current();
    auto &      buf         = ch.data;

    for (int dy = -m_brush_size + 1; dy < m_brush_size; ++dy)
    {
        for (int dx = -m_brush_size + 1; dx < m_brush_size; ++dx) {
            const int x = gx + dx;
            const int y = gy + dy;

            //  Bounds check
            if (x < 0 || x >= m_res_x || y < 0 || y >= m_res_y)
                continue;

            //  Square brush paints the full kernel, circle brush uses radius²
            if (m_brush_shape == BrushShape::Square || (dx * dx + dy * dy) < m_brush_size * m_brush_size)
                buf[static_cast<size_t>(y) * m_res_x + x] = ch.paint_value.value;
        }
    }
    return;
}


//  "draw_line"
//
void SketchWidget::draw_line(int x0, int y0, int x1, int y1)
{
    int dx  = std::abs(x1 - x0);
    int sx  = (x0 < x1) ? 1 : -1;
    int dy  = std::abs(y1 - y0);
    int sy  = (y0 < y1) ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;

    for (;;) {
        stamp(x0, y0);
        if (x0 == x1 && y0 == y1)
            break;

        const int e2 = err;
        if (e2 > -dx) { err -= dy; x0 += sx; }
        if (e2 <  dy) { err += dx; y0 += sy; }
    }
}


// *************************************************************************** //
//
//
//  ?.      UTILITY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "clear"
//
void SketchWidget::clear()
{
    std::fill(current().data.begin(), current().data.end(), 0.0f);
}


//  "resize_buffer"
//
void SketchWidget::resize_buffers(int nx, int ny)
{
    //  Early‑out when the requested resolution matches the current buffers.
    if (nx == m_res_x && ny == m_res_y)
        return;

    for (auto& ch : m_channels) {
        //  Allocate a fresh buffer initialised to 0.
        std::vector<float> new_buf(static_cast<size_t>(nx) * ny, 0.0f);

        //  Preserve any overlapping region of the previous image.
        if (m_res_x > 0 && m_res_y > 0) {
            const int copy_x = std::min(m_res_x, nx);
            const int copy_y = std::min(m_res_y, ny);

            for (int y = 0; y < copy_y; ++y) {
                std::copy_n(ch.data.data() + static_cast<size_t>(y) * m_res_x,
                            copy_x,
                            new_buf.data() + static_cast<size_t>(y) * nx);
            }
        }

        ch.data.swap(new_buf);
    }

    m_res_x = nx;
    m_res_y = ny;
}


//  "set_mode"
//
void SketchWidget::set_mode(const int mode) {
    State mode_type     = static_cast<State>(mode);
    this->m_mode        = mode_type;
    
    switch (mode_type)
    {
        case State::Build : {
            m_cfg.graph.flags       = ImPlotFlags_None | ImPlotFlags_CanvasOnly;
            break;
        }
        default : {
            m_cfg.graph.flags       = ImPlotFlags_None | ImPlotFlags_NoLegend;
            //m_cfg.graph.flags      |= ~ImPlotFlags_NoLegend;
            //m_cfg.graph.flags      |= ImPlotFlags_NoMenus;
            break;
        }
    }
    
    
    return;
}
    
    
//  "set_cmap"
//
void SketchWidget::set_cmap(const int cmap)
{
    current().cmap = cmap;
    //auto &  ch      = current();
    //ch.cmap         = ( cmap % ImPlot::GetColormapCount() );
}


//  "ShowControls"
//
void SketchWidget::ShowControls([[maybe_unused]] float scale_width)
{
    return;
}

























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

/***********************************************************************************
*
*       ********************************************************************
*       ****             T O O L S . C P P  ____  F I L E               ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond
*               DATED:      May 19, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/graph_app/graph_app.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//  0.      STATIC VARIABLES...
// *************************************************************************** //
// *************************************************************************** //

namespace sketch {
    //  MISC STUFF...
    static float                        w                       = 200.0f;
    
    //  SKETCH STUFF...
    static const char *                 heatmap_uuid            = "##EtchASketch";
    static int                          res                     = 128;
    static int                          last_res                = -1;
    static float                        vmin                    = 0.0f,     vmax = 1.0f;
    static ImPlotColormap               cmap                    = ImPlotColormap_Viridis;
    static std::vector<float>           data;
    
    //  BRUSH STUFF...
    static const char *                 brush_shapes[]          = {"Square", "Circle"};
    static int                          brush_size              = 1;
    static int                          brush_shape             = 0; // 0 = square, 1 = circle
    static float                        paint_value             = 1.0f; // value to paint into the grid
    static bool                         drawing                 = false;
}






// *************************************************************************** //
//
//
//  3.      PLOTTING FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "ShowSketch"
//
void GraphApp::ShowSketch(void)
{
    // 1. CONSTANTS AND VARIABLES
    static constexpr float LABEL_WIDTH  = 150.0f;
    static constexpr float WIDGET_WIDTH = 250.0f;

    // 1.1 Layout flags
    ImPlotHeatmapFlags hm_flags   = 0;
    ImPlotAxisFlags    axes_flags = ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoGridLines |
                                   ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoDecorations;
    ImPlotFlags        plot_flags = ImPlotFlags_NoLegend;
    ImVec2             avail      = ImGui::GetContentRegionAvail();
    ImVec2             scale_dims = ImVec2(80.0f, avail.y);
    ImVec2             plot_dims  = ImVec2(avail.x - scale_dims.x - ImGui::GetStyle().ItemSpacing.x, avail.y);

    // Resize buffer if resolution changed
    if (sketch::res != sketch::last_res) {
        sketch::data.assign(sketch::res * sketch::res, 0.0f);
        sketch::last_res = sketch::res;
    }

    // Track last drawn grid cell for Bresenham
    static int prev_x = -1, prev_y = -1;

    ImGui::BeginGroup();
    if (ImPlot::BeginPlot(sketch::heatmap_uuid, plot_dims, plot_flags)) {
        ImPlot::SetupAxes(nullptr, nullptr, axes_flags, axes_flags);
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, sketch::res, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, sketch::res, ImGuiCond_Always);

        // Bresenham line rasterization lambda
        auto draw_line = [&](int x0, int y0, int x1, int y1) {
            int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
            int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
            int err = (dx > dy ? dx : -dy) / 2;
            int x = x0, y = y0;
            while (true) {
                // Stamp brush at (x,y)
                for (int dyb = -sketch::brush_size + 1; dyb < sketch::brush_size; ++dyb) {
                    for (int dxb = -sketch::brush_size + 1; dxb < sketch::brush_size; ++dxb) {
                        int sxp = x + dxb;
                        int syp = y + dyb;
                        if (sxp >= 0 && sxp < sketch::res && syp >= 0 && syp < sketch::res) {
                            if (sketch::brush_shape == 0 || (dxb*dxb + dyb*dyb) < sketch::brush_size*sketch::brush_size) {
                                sketch::data[(sketch::res - 1 - syp) * sketch::res + sxp] = sketch::paint_value;
                            }
                        }
                    }
                }
                if (x == x1 && y == y1) break;
                int e2 = err;
                if (e2 > -dx) { err -= dy; x += sx; }
                if (e2 <  dy) { err += dx; y += sy; }
            }
        };

        // Paint or erase on drag
        if (ImPlot::IsPlotHovered() && ImGui::IsMouseDown(0)) {
            ImPlotPoint mp = ImPlot::GetPlotMousePos();
            int cx = static_cast<int>(mp.x);
            int cy = static_cast<int>(mp.y);
            if (prev_x < 0) {
                // first sample: just draw single point
                draw_line(cx, cy, cx, cy);
            } else {
                // connect from previous to current
                draw_line(prev_x, prev_y, cx, cy);
            }
            sketch::drawing = true;
            prev_x = cx; prev_y = cy;
        }
        else {
            sketch::drawing = false;
            prev_x = prev_y = -1;
        }

        // Render heatmap
        ImPlot::PlotHeatmap("heat", sketch::data.data(), sketch::res, sketch::res,
                            sketch::vmin, sketch::vmax, nullptr,
                            ImPlotPoint(0,0), ImPlotPoint((double)sketch::res, (double)sketch::res), hm_flags);

        // Brush cursor preview
        if (ImPlot::IsPlotHovered()) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_None);
            ImPlotPoint mp = ImPlot::GetPlotMousePos();
            ImVec2 center = ImPlot::PlotToPixels(mp);
            float scale  = ImPlot::PlotToPixels(ImPlotPoint(1,0)).x - ImPlot::PlotToPixels(ImPlotPoint(0,0)).x;
            float half   = sketch::brush_size * scale * 0.5f;
            ImDrawList* dl = ImGui::GetForegroundDrawList();
            if (sketch::brush_shape == 0) {
                dl->AddRect({center.x-half, center.y-half}, {center.x+half, center.y+half}, IM_COL32(255,0,0,255), 0, 0, 2.0f);
            } else {
                dl->AddCircle(center, half, IM_COL32(255,0,0,255), 0, 2.0f);
            }
        }

        ImPlot::EndPlot();
    }
    ImGui::SameLine();
    ImPlot::ColormapScale("##HeatScale", sketch::vmin, sketch::vmax, scale_dims);
    ImGui::EndGroup();
}



// *************************************************************************** //
//
//
//  4.      CONTROL-PANEL FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "ShowSketchControls"
//
void GraphApp::ShowSketchControls(void)
{
    //  CONSTANTS...
    static constexpr float              LABEL_COLUMN_WIDTH      = 200.0f;
    static constexpr float              WIDGET_COLUMN_WIDTH     = 250.0f;

    //  INTERACTIVE VARIABLES...
    static int                          param_a                 = 3;
    static int                          param_b                 = 7;
    static bool                         toggle                  = true;
    static ImVec4                       color                   = {0.5f, 0.5f, 1.0f, 1.0f};

    //  TABLE GLOBAL FLAGS...
    static bool                         freeze_header           = false;
    static bool                         freeze_column           = false;
    static bool                         stretch_column_1        = true;

    //  COLUMN-SPECIFIC FLAGS...
    static ImGuiTableColumnFlags        col0_flags              = ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize;
    static ImGuiTableColumnFlags        col1_flags              = stretch_column_1 ? ImGuiTableColumnFlags_WidthStretch : ImGuiTableColumnFlags_WidthFixed;
    static ImGuiTableFlags              flags                   = ImGuiTableFlags_None | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoKeepColumnsVisible; //| ImGuiTableFlags_ScrollX;
        
    static const utl::WidgetRow rows[]                          = {
        {"Clear Sketch",        []{ if (ImGui::Button("Clear Canvas")) std::fill(sketch::data.begin(), sketch::data.end(), 0.0f); }                                                                                             },
        {"Sketch Resolution",   []{ float w = ImGui::GetColumnWidth(); ImGui::SetNextItemWidth(w); ImGui::SliderInt("##SketchResolution",&sketch::res,16, 256); }                                                               },
    //
        {"Brush Size",          []{ float w = ImGui::GetColumnWidth(); ImGui::SetNextItemWidth(w); ImGui::SliderInt("##BrushSize",      &sketch::brush_size, 1, 8);      }                                                           },
        {"Brush Shape",         []{ float w = ImGui::GetColumnWidth(); ImGui::SetNextItemWidth(w); ImGui::Combo("##BrushShape",         &sketch::brush_shape, sketch::brush_shapes, IM_ARRAYSIZE(sketch::brush_shapes));  }      },
        {"Paint Value",         []{ float w = ImGui::GetColumnWidth(); ImGui::SetNextItemWidth(w); ImGui::SliderFloat("##BrushValue",   &sketch::paint_value, sketch::vmin, sketch::vmax); }                                     },
    //
        {"Colormap",            []{
            float w = ImGui::GetColumnWidth();
            if (ImPlot::ColormapButton(ImPlot::GetColormapName(sketch::cmap), ImVec2(w, 0), sketch::cmap))
            {
                sketch::cmap = (sketch::cmap + 1) % ImPlot::GetColormapCount();
                ImPlot::BustColorCache(sketch::heatmap_uuid);
            }
            ImPlot::PushColormap(sketch::cmap);
        }}
    };


    static utl::TableCFG<2>    sketch_table_CFG    = {
        "SketchControls"
    };


    //  1.  PRIMARY TABLE ENTRY...
    utl::MakeCtrlTable(rows, sketch_table_CFG);
    
    return;
}


// *************************************************************************** //
//
//
//  ?.      UTILITY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "ShowModelParameters"
//
void GraphApp::ShowModelParameters(void)
{
    //  OTHER CONSTANTS...
    static const ImVec2                 SPACING                     = ImVec2( 0.0f, 0.25*ImGui::GetTextLineHeightWithSpacing() );
    
    //  CONSTANTS...
    static utl::TableCFG<2>             m_stepsize_table_CFG        = {
        /*  Table UUID  =   */      "StepsizesControls",
        /*  Table Flags =   */      ImGuiTableFlags_None | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoKeepColumnsVisible | ImGuiTableFlags_SizingStretchProp,
        /*  Column CFGs =   */      {
                { "Label",          200.0f,         ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize },
                { "Widget",         -1.0f,          ImGuiTableColumnFlags_WidthStretch }
            },
        /*  Header Row  =   */      false
    };
    
    static utl::TableCFG<2>             m_sources_table_CFG         = {
        /*  Table UUID  =   */      "SourcesControls",
        /*  Table Flags =   */      ImGuiTableFlags_None | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoKeepColumnsVisible | ImGuiTableFlags_SizingStretchProp,
        /*  Column CFGs =   */      {
                { "Label",          200.0f,         ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize },
                { "Widget",         -1.0f,          ImGuiTableColumnFlags_WidthStretch }
            },
        /*  Header Row  =   */      false
    };





    //  1.  STEPSIZE TABLE...
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::CollapsingHeader("Parameters") ) {
        utl::MakeCtrlTable(this->ms_STEPSIZE_ROWS,      m_stepsize_table_CFG);
    }
    
    
    //  2.  SOURCES TABLE...
    ImGui::Dummy( SPACING );
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::CollapsingHeader("Sources") ) {
        utl::MakeCtrlTable(this->ms_SOURCES_ROWS,   m_sources_table_CFG);
    }
    
    
    

    return;
}





// *************************************************************************** //
//
//
//  ?.      UTILITY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "test_table"
//
void GraphApp::test_table(void)
{
    // Constants
    static constexpr float LABEL_COLUMN_WIDTH  = 150.0f;
    static constexpr float WIDGET_COLUMN_WIDTH = 250.0f;

    // Actual controlled variables
    static int    param_a = 3;
    static int    param_b = 7;
    static bool   toggle  = true;
    static ImVec4 color   = {0.5f, 0.5f, 1.0f, 1.0f};

    // Table flags (interactive toggles)
    static bool show_borders     = true;
    static bool show_row_bg      = true;
    static bool resizable        = false;
    static bool no_keep_cols     = true;
    static bool enable_scroll_x  = false;
    static bool enable_scroll_y  = false;
    static bool freeze_header    = false;
    static bool freeze_column    = false;
    static bool enable_sorting   = false;
    static bool enable_hiding    = false;
    static bool stretch_column_1 = false;

    // Per-column flags
    static bool col0_no_resize = false;
    static bool col0_no_sort   = false;
    static bool col0_default_hide = false;

    static bool col1_no_resize = false;
    static bool col1_no_sort   = false;
    static bool col1_default_hide = false;

    if ( ImGui::TreeNode("Table Options") )
    {
        ImGui::Checkbox("Show Borders", &show_borders);
        ImGui::Checkbox("Alternate Row Background", &show_row_bg);
        ImGui::Checkbox("Resizable Columns", &resizable);
        ImGui::Checkbox("No Keep Columns Visible", &no_keep_cols);
        ImGui::Checkbox("Scroll X", &enable_scroll_x);
        ImGui::Checkbox("Scroll Y", &enable_scroll_y);
        ImGui::Checkbox("Freeze Header Row", &freeze_header);
        ImGui::Checkbox("Freeze Label Column", &freeze_column);
        ImGui::Checkbox("Enable Sorting", &enable_sorting);
        ImGui::Checkbox("Allow Hiding Columns", &enable_hiding);
        ImGui::Checkbox("Stretch Widget Column", &stretch_column_1);
        ImGui::Spacing();
        
        ImGui::TreePop();
    }


    if ( ImGui::TreeNode("Column Settings") )
    {
        if (ImGui::BeginTable("ColumnSettings", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("Label Column");
            ImGui::TableSetupColumn("Widget Column");
            ImGui::TableHeadersRow();

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Checkbox("NoResize##0", &col0_no_resize);
            ImGui::TableSetColumnIndex(1); ImGui::Checkbox("NoResize##1", &col1_no_resize);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Checkbox("NoSort##0", &col0_no_sort);
            ImGui::TableSetColumnIndex(1); ImGui::Checkbox("NoSort##1", &col1_no_sort);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Checkbox("DefaultHide##0", &col0_default_hide);
            ImGui::TableSetColumnIndex(1); ImGui::Checkbox("DefaultHide##1", &col1_default_hide);

            ImGui::EndTable();
        }
        
        ImGui::TreePop();
    }



    // Combine global table flags
    ImGuiTableFlags flags = 0;
    if (show_borders)    flags |= ImGuiTableFlags_Borders;
    if (show_row_bg)     flags |= ImGuiTableFlags_RowBg;
    if (resizable)       flags |= ImGuiTableFlags_Resizable;
    if (no_keep_cols)    flags |= ImGuiTableFlags_NoKeepColumnsVisible;
    if (enable_scroll_x) flags |= ImGuiTableFlags_ScrollX;
    if (enable_scroll_y) flags |= ImGuiTableFlags_ScrollY;
    if (enable_sorting)  flags |= ImGuiTableFlags_Sortable;
    if (enable_hiding)   flags |= ImGuiTableFlags_Hideable;

    // Declarative per-row widget logic
    struct WidgetRow {
        const char* label;
        std::function<void()> render;
    };

    static const WidgetRow rows[] = {
        {"Param A", []{ float w = ImGui::GetColumnWidth(); ImGui::SetNextItemWidth(w); ImGui::SliderInt("##A", &param_a, 0, 10); }},
        {"Param B", []{ float w = ImGui::GetColumnWidth(); ImGui::SetNextItemWidth(w); ImGui::SliderInt("##B", &param_b, 0, 10); }},
        {"Enabled", []{ float w = ImGui::GetColumnWidth(); ImGui::SetNextItemWidth(w); ImGui::Checkbox("##Toggle", &toggle); }},
        {"Color",   []{ float w = ImGui::GetColumnWidth(); ImGui::SetNextItemWidth(w); ImGui::ColorEdit4("##Color", (float*)&color); }},
    };

    if (ImGui::BeginTable("WidgetTable", 2, flags)) {
        if (freeze_column || freeze_header)
            ImGui::TableSetupScrollFreeze(freeze_column ? 1 : 0, freeze_header ? 1 : 0);

        ImGuiTableColumnFlags col0_flags = ImGuiTableColumnFlags_WidthFixed;
        if (col0_no_resize)     col0_flags |= ImGuiTableColumnFlags_NoResize;
        if (col0_no_sort)       col0_flags |= ImGuiTableColumnFlags_NoSort;
        if (col0_default_hide)  col0_flags |= ImGuiTableColumnFlags_DefaultHide;

        ImGuiTableColumnFlags col1_flags = stretch_column_1 ? ImGuiTableColumnFlags_WidthStretch : ImGuiTableColumnFlags_WidthFixed;
        if (col1_no_resize)     col1_flags |= ImGuiTableColumnFlags_NoResize;
        if (col1_no_sort)       col1_flags |= ImGuiTableColumnFlags_NoSort;
        if (col1_default_hide)  col1_flags |= ImGuiTableColumnFlags_DefaultHide;

        ImGui::TableSetupColumn("Label", col0_flags, LABEL_COLUMN_WIDTH);
        ImGui::TableSetupColumn("Widget", col1_flags, stretch_column_1 ? 1.0f : WIDGET_COLUMN_WIDTH);
        ImGui::TableHeadersRow();

        for (const auto& row : rows) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted(row.label);
            ImGui::TableSetColumnIndex(1);
            row.render();
        }

        ImGui::EndTable();
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

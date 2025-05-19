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
//  3.      PRIVATE MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "etch_a_sketch"
//
void GraphApp::etch_a_sketch(void)
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
//  ?.      UTILITY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //












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

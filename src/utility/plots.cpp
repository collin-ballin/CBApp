//
//  plots.cpp
//  CBApp
//
//  Created by Collin Bond on 4/20/25.
//
// *************************************************************************** //
// *************************************************************************** //
#include "_config.h"
#include "utility/utility.h"
#include "utility/_constants.h"
#include "utility/_templates.h"



namespace cb { namespace utl { //     BEGINNING NAMESPACE "cb" :: "utl"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//
//
//
//  2.1     PLOTTING STUFF...
// *************************************************************************** //
// *************************************************************************** //

//  "Sparkline"
//
void Sparkline(const char* id, const float* values, int count, float min_v, float max_v, int offset, const ImVec4 & color, const ImVec2& size) {
    ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0,0));
    if (ImPlot::BeginPlot(id,size,ImPlotFlags_CanvasOnly))
    {
        ImPlot::SetupAxes(nullptr,nullptr,ImPlotAxisFlags_NoDecorations,ImPlotAxisFlags_NoDecorations);
        ImPlot::SetupAxesLimits(0, count - 1, min_v, max_v, ImGuiCond_Always);
        ImPlot::SetNextLineStyle(color);
        ImPlot::SetNextFillStyle(color, 0.25);
        ImPlot::PlotLine(id, values, count, 1, 0, ImPlotLineFlags_Shaded, offset);
        ImPlot::EndPlot();
    }
    ImPlot::PopStyleVar();
}



//  "ScrollingSparkline"
//
//      center:   0 = left.  1 = right.   0.5 = center.
void ScrollingSparkline(const float time,       const float window,     ScrollingBuffer & data,
                        const float ymin,       const float ymax,       const ImPlotAxisFlags flags,
                        const ImVec4 & color,   const ImVec2 & size,    const float center)
{
    ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0,0));
    if ( ImPlot::BeginPlot("##Scrolling", size, ImPlotFlags_CanvasOnly) )
    {
        //  1.  Create Axis...
        ImPlot::SetupAxes(nullptr, nullptr, flags, flags);
    
        float   xmin    = time - center * window;
        float   xmax    = xmin + window;
        
        ImPlot::SetupAxisLimits(ImAxis_X1, xmin, xmax, ImGuiCond_Always);       //  2.  X-Limits...
        ImPlot::SetupAxisLimits(ImAxis_Y1, ymin, ymax);                         //  3.  Y-Limits...
        
        ImPlot::SetNextLineStyle(color);                                        //  4.  Plotting the Line...
        ImPlot::SetNextFillStyle(color, 0.25);
        ImPlot::PlotLine("Mouse X", &data.Data[0].x, &data.Data[0].y, data.Data.size(), ImPlotLineFlags_Shaded, data.Offset, 2 * sizeof(float));
        ImPlot::EndPlot();
    }
    ImPlot::PopStyleVar();
    return;
}


//  "ScrollingSparkline"
//
void ScrollingSparkline(const float time,               const float window,     ScrollingBuffer & data,
                        const ImPlotAxisFlags flags,    const ImVec4 & color,   const ImVec2 & size,
                        const float center)
{
    ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0,0));
    if ( ImPlot::BeginPlot("##Scrolling", size, ImPlotFlags_CanvasOnly) )
    {
        //  1.  Create Axis...
        ImPlot::SetupAxes(nullptr, nullptr, flags|ImPlotAxisFlags_AutoFit, flags|ImPlotAxisFlags_AutoFit);
    
        float   xmin    = time - center * window;
        float   xmax    = xmin + window;
        
        ImPlot::SetupAxisLimits(ImAxis_X1, xmin, xmax, ImGuiCond_Always);       //  2.  X-Limits...
        
        ImPlot::SetNextLineStyle(color);                                        //  4.  Plotting the Line...
        ImPlot::SetNextFillStyle(color, 0.25);
        ImPlot::PlotLine("Mouse X", &data.Data[0].x, &data.Data[0].y, data.Data.size(), ImPlotLineFlags_Shaded, data.Offset, 2 * sizeof(float));
        ImPlot::EndPlot();
    }
    ImPlot::PopStyleVar();
    return;
}

       


//  2.2     UTILITY FUNCTIONS FOR IMPLOT STUFF...
// *************************************************************************** //
// *************************************************************************** //

//  "GetColormapSamples"
//
[[nodiscard]] std::vector<ImVec4> GetColormapSamples(const size_t M)
{
    std::vector<ImVec4>     colors(M);
    //ImPlotColormap  map     = ImPlot::GetCurrentColormap();
    

    for (size_t i = 0; i < M; ++i) {
        float t = (M > 1) ? float(i) / float(M - 1) : 0.0f;
        colors[i] = ImPlot::SampleColormap(t);
    }

    return colors;
}


//  "GetColorMapSamples"
//
[[nodiscard]] std::vector<ImVec4> GetColormapSamples(int M, const char * cmap)
{
    std::vector<ImVec4> cols;
    cols.reserve(M);

    // Activate the colormap so SampleColormap() uses it:
    ImPlot::PushColormap(cmap);

    for (int r = 0; r < M; r++) {
        float t = (M > 1)
                ? float(r) / float(M - 1)   // 0→1
                : 0.0f;                     // single entry
        cols.push_back(ImPlot::SampleColormap(t));
    }

    ImPlot::PopColormap();
    return cols;
}


//  "GetColormapSamples"
//
[[nodiscard]] std::vector<ImVec4> GetColormapSamples(int M, ImPlotColormap map)
{
    std::vector<ImVec4> cols;
    cols.reserve(M);

    // Activate the colormap so SampleColormap() uses it:
    ImPlot::PushColormap(map);

    for (int r = 0; r < M; r++) {
        float t = (M > 1)
                ? float(r) / float(M - 1)   // 0→1
                : 0.0f;                     // single entry
        cols.push_back(ImPlot::SampleColormap(t));
    }

    ImPlot::PopColormap();
    return cols;
}


//  "CreateTransparentColormap"
//
ImPlotColormap CreateTransparentColormap(ImPlotColormap base_index, float alpha, const char* name = nullptr)
{
    const int count = ImPlot::GetColormapSize(base_index);
    std::vector<ImVec4> colors;

    for (int i = 0; i < count; ++i) {
        ImVec4 c = ImPlot::GetColormapColor(i, base_index);  // Use the public API
        c.w = alpha; // Apply custom alpha
        colors.push_back(c);
    }

    if (!name) name = "TransparentCustom";
    return ImPlot::AddColormap( name, colors.data(), static_cast<int>(colors.size()) );
}







// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" NAMESPACE.






// *************************************************************************** //
// *************************************************************************** //
//
//  END.


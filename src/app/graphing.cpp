//
//  graphing.cpp
//  CBApp
//
//  Created by Collin Bond on 4/26/25.
//
// *************************************************************************** //
// *************************************************************************** //
#include "app/app.h"
#include <random>
#include <algorithm>



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//  "Display_Graphing_App"
//
void App::Display_Graphing_App(bool * p_open)
{
    ImGuiIO &                       io                      = ImGui::GetIO(); (void)io;
    ImGuiStyle &                    style                   = ImGui::GetStyle();
    ImGuiWindowFlags                m_graphing_win_flags    = ImGuiWindowFlags_None; //| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
    
    
    //  1.  CREATE THE WINDOW AND BEGIN APPENDING WIDGETS INTO IT...
    ImGui::Begin(app::DEF_GRAPHING_WIN_TITLE, p_open, m_graphing_win_flags);
    
    
    static cb::HeatMap              map(10, 10);
    map.Begin();
    
    
    
    
    //this->disp_graphing_app();

    
 
 
 
    ImGui::End();
    return;
}







//  "disp_graphing_app"
//
void App::disp_graphing_app(void)
{
    //  ALIASES...
    using                       value_type          = float;
    using                       size_type           = std::size_t;
    using                       vec_t               = std::vector< value_type >;
    using                       mtx_t               = std::vector< vec_t >;
    
    //  CONSTANT VALUES...
    constexpr value_type        VMIN                = -10.0f;
    constexpr value_type        VMAX                = 10.0f;
    constexpr value_type        MIN_DIMS            = 300.0f;
    constexpr value_type        MAX_DIMS            = 750.0f;
    constexpr value_type        SCALE_WIDTH         = 90.0f;
    constexpr value_type        CONTROL_WIDTH       = 225;
    
    
    static const char *         xlabels[]           = {"C1","C2","C3","C4","C5","C6","C7"};
    static const char *         ylabels[]           = {"R1","R2","R3","R4","R5","R6","R7"};
    static ImVec2               PLOT_SIZE(550.0f, 550.0);
    static ImVec2               COLORBAR_SIZE(PLOT_SIZE[0], 0);
    static ImVec2               SCALE_SIZE(SCALE_WIDTH, PLOT_SIZE[1]);
    
    static ImPlotColormap       map                 = ImPlotColormap_Viridis;
    static ImPlotHeatmapFlags   hm_flags            = 0;
    static ImPlotAxisFlags      axes_flags          = ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoTickMarks;
    static ImPlotFlags          plot_flags          = ImPlotFlags_None | ImPlotFlags_Equal | ImPlotFlags_NoLegend; // ImPlotFlags_Equal
    
    
    //  VARIABLE VALUES...
    static float                vmin                = 0;
    static float                vmax                = 10.00f;
    static size_type            X                   = 7;
    static size_type            Y                   = 7;
    static value_type           amp                 = 5;
    static value_type           freq                = 2;
    static value_type           time                = 0;
    
    static mtx_t                matrix;
    
    
    
    
    
    time += ImGui::GetIO().DeltaTime;
    //utl::sinusoid_wave(matrix, time, amp, freq);
    
    
    

    
    //  1.  PLOT DIMENSIONS...
    ImGui::SetNextItemWidth(CONTROL_WIDTH);
    ImGui::SliderFloat("Plot Width",            &PLOT_SIZE.x,   MIN_DIMS,   MAX_DIMS,      "%.1f px");
    ImGui::SetNextItemWidth(CONTROL_WIDTH);     ImGui::SameLine();
    ImGui::SliderFloat("Plot Height",           &PLOT_SIZE.y,   MIN_DIMS,   MAX_DIMS,      "%.1f px");
    
    
    //  2.  VMIN, VMAX RANGE...
    ImGui::SetNextItemWidth(CONTROL_WIDTH);
    ImGui::SliderFloat("VMin",            &vmin,   VMIN,           vmax,      "%.1f");
    ImGui::SetNextItemWidth(CONTROL_WIDTH);
    ImGui::SliderFloat("VMax",            &vmax,   vmin,      VMAX,           "%.1f");
    
    
    //  3.  COLUMN-MAJOR REPRESENTATION...
    ImGui::CheckboxFlags("Column Major", (unsigned int*)&hm_flags, ImPlotHeatmapFlags_ColMajor);
    
    
    

        
    
    
    //  4.  COLOR-MAP SELECTOR BUTTON...
    if (ImPlot::ColormapButton(ImPlot::GetColormapName(map), COLORBAR_SIZE, map)) {
        map = (map + 1) % ImPlot::GetColormapCount();       // We bust the color cache of our plots so that item colors will
        ImPlot::BustColorCache("##Heatmap1");               // resample the new colormap in the event that they have already
        ImPlot::BustColorCache("##Heatmap2");               // been created. See documentation in implot.h.
    }
    ImGui::SameLine();
    ImGui::LabelText("##Colormap Index", "%s", "Change Colormap");
    ImPlot::PushColormap(map);




    //  9.  DRAWING THE HEATMAP PLOT...
    //
    //ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
    
    /*
    if (ImPlot::BeginPlot("##Heatmap1", PLOT_SIZE, plot_flags))
    {
        ImPlot::SetupAxes(nullptr, nullptr, axes_flags, axes_flags);
        //ImPlot::SetupAxisTicks(ImAxis_X1,   0 + 1.0/14.0,      1 - 1.0/14.0, X,    xlabels);
        
        ImPlot::SetupAxisTicks(ImAxis_X1,   0 + 1.0/14.0,      1 - 1.0/14.0,    static_cast<int>(X),    nullptr);
        ImPlot::SetupAxisTicks(ImAxis_Y1,   1 - 1.0/14.0,      0 + 1.0/14.0,    static_cast<int>(Y),    nullptr);
        ImPlot::PlotHeatmap("heat", matrix, static_cast<int>(X), static_cast<int>(Y), vmin, vmax,"%g", ImPlotPoint(0,0), ImPlotPoint(1,1), hm_flags);
        ImPlot::EndPlot();
    }
    ImGui::SameLine();
    ImPlot::ColormapScale("##HeatScale", vmin, vmax, SCALE_SIZE);
    */



    return;
}
















//-----------------------------------------------------------------------------
/*
void Demo_Heatmaps() {
    static float values1[7][7]  = {{0.8f, 2.4f, 2.5f, 3.9f, 0.0f, 4.0f, 0.0f},
                                    {2.4f, 0.0f, 4.0f, 1.0f, 2.7f, 0.0f, 0.0f},
                                    {1.1f, 2.4f, 0.8f, 4.3f, 1.9f, 4.4f, 0.0f},
                                    {0.6f, 0.0f, 0.3f, 0.0f, 3.1f, 0.0f, 0.0f},
                                    {0.7f, 1.7f, 0.6f, 2.6f, 2.2f, 6.2f, 0.0f},
                                    {1.3f, 1.2f, 0.0f, 0.0f, 0.0f, 3.2f, 5.1f},
                                    {0.1f, 2.0f, 0.0f, 1.4f, 0.0f, 1.9f, 6.3f}};
    static float scale_min       = 0;
    static float scale_max       = 6.3f;
    static const char* xlabels[] = {"C1","C2","C3","C4","C5","C6","C7"};
    static const char* ylabels[] = {"R1","R2","R3","R4","R5","R6","R7"};

    static ImPlotColormap map = ImPlotColormap_Viridis;
    if (ImPlot::ColormapButton(ImPlot::GetColormapName(map),ImVec2(225,0),map)) {
        map = (map + 1) % ImPlot::GetColormapCount();
        // We bust the color cache of our plots so that item colors will
        // resample the new colormap in the event that they have already
        // been created. See documentation in implot.h.
        BustColorCache("##Heatmap1");
        BustColorCache("##Heatmap2");
    }

    ImGui::SameLine();
    ImGui::LabelText("##Colormap Index", "%s", "Change Colormap");
    ImGui::SetNextItemWidth(225);
    ImGui::DragFloatRange2("Min / Max",&scale_min, &scale_max, 0.01f, -20, 20);

    static ImPlotHeatmapFlags hm_flags = 0;

    ImGui::CheckboxFlags("Column Major", (unsigned int*)&hm_flags, ImPlotHeatmapFlags_ColMajor);

    static ImPlotAxisFlags axes_flags = ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoTickMarks;

    ImPlot::PushColormap(map);

    if (ImPlot::BeginPlot("##Heatmap1",ImVec2(225,225),ImPlotFlags_NoLegend|ImPlotFlags_NoMouseText)) {
        ImPlot::SetupAxes(nullptr, nullptr, axes_flags, axes_flags);
        ImPlot::SetupAxisTicks(ImAxis_X1,0 + 1.0/14.0, 1 - 1.0/14.0, 7, xlabels);
        ImPlot::SetupAxisTicks(ImAxis_Y1,1 - 1.0/14.0, 0 + 1.0/14.0, 7, ylabels);
        ImPlot::PlotHeatmap("heat",values1[0],7,7,scale_min,scale_max,"%g",ImPlotPoint(0,0),ImPlotPoint(1,1),hm_flags);
        ImPlot::EndPlot();
    }
    ImGui::SameLine();
    ImPlot::ColormapScale("##HeatScale",scale_min, scale_max, ImVec2(60,225));

    ImGui::SameLine();

    const int size = 80;
    static double values2[size*size];
    srand((unsigned int)(ImGui::GetTime()*1000000));
    for (int i = 0; i < size*size; ++i)
        values2[i] = RandomRange(0.0,1.0);

    if (ImPlot::BeginPlot("##Heatmap2",ImVec2(225,225))) {
        ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
        ImPlot::SetupAxesLimits(-1,1,-1,1);
        ImPlot::PlotHeatmap("heat1",values2,size,size,0,1,nullptr);
        ImPlot::PlotHeatmap("heat2",values2,size,size,0,1,nullptr, ImPlotPoint(-1,-1), ImPlotPoint(0,0));
        ImPlot::EndPlot();
    }
    ImPlot::PopColormap();

}



//-----------------------------------------------------------------------------


void Demo_Histogram2D() {
    static int count     = 50000;
    static int xybins[2] = {100,100};

    static ImPlotHistogramFlags hist_flags = 0;

    ImGui::SliderInt("Count",&count,100,100000);
    ImGui::SliderInt2("Bins",xybins,1,500);
    ImGui::SameLine();
    ImGui::CheckboxFlags("Density", (unsigned int*)&hist_flags, ImPlotHistogramFlags_Density);

    static NormalDistribution<100000> dist1(1, 2);
    static NormalDistribution<100000> dist2(1, 1);
    double max_count = 0;
    ImPlotAxisFlags flags = ImPlotAxisFlags_AutoFit|ImPlotAxisFlags_Foreground;
    ImPlot::PushColormap("Hot");
    if (ImPlot::BeginPlot("##Hist2D",ImVec2(ImGui::GetContentRegionAvail().x-100-ImGui::GetStyle().ItemSpacing.x,0))) {
        ImPlot::SetupAxes(nullptr, nullptr, flags, flags);
        ImPlot::SetupAxesLimits(-6,6,-6,6);
        max_count = ImPlot::PlotHistogram2D("Hist2D",dist1.Data,dist2.Data,count,xybins[0],xybins[1],ImPlotRect(-6,6,-6,6), hist_flags);
        ImPlot::EndPlot();
    }
    ImGui::SameLine();
    ImPlot::ColormapScale(hist_flags & ImPlotHistogramFlags_Density ? "Density" : "Count",0,max_count,ImVec2(100,0));
    ImPlot::PopColormap();
}
*/
//-----------------------------------------------------------------------------





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

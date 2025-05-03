//
//  sidebar.cpp
//  CBApp
//
//  Created by Collin Bond on 5/1/25.
//
// *************************************************************************** //
// *************************************************************************** //
#include "app/app.h"
#include "_config.h"
# include "imgui_internal.h"
//#include "utility/utility.h"
#include <random>
#include <algorithm>



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//
//
//  1.  WINDOW FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "Display_Sidebar_Menu"
//
void App::Display_Sidebar_Menu(bool * p_open)
{
    //static bool             m_sidebar_collapsed     = false;
    ImGuiIO &               io                      = ImGui::GetIO(); (void)io;
    ImGuiStyle &            style                   = ImGui::GetStyle();
    
    
    
    //  1.  CREATE THE WINDOW AND BEGIN APPENDING WIDGETS INTO IT...
    ImGui::PushStyleColor(ImGuiCol_WindowBg, this->m_sidebar_bg);   // Push before ImGui::Begin()
    
    
    ImGui::Begin(app::DEF_SIDEBAR_WIN_TITLE, p_open, this->m_sidebar_win_flags);
    ImGui::PopStyleColor();
    
    //if (ImGui::IsItemActive() || ImGui::SliderFloat("Sidebar Width", &this->m_sidebar_ratio, 0.1f, 0.5f, "%.2f")) {
    //    this->m_rebuild_dockspace = true;
    //}

    //  2.  SIDE-BAR COLLAPSE BUTTON...
    if (ImGui::Button(m_show_sidebar_window ? "<==|" : "|==>"))
    {
        //this->m_show_sidebar_window = !this->m_show_sidebar_window;

        if ( (this->m_show_sidebar_window = !this->m_show_sidebar_window) )
            this->m_sidebar_ratio = app::DEF_SB_OPEN_WIDTH;
        else
            this->m_sidebar_ratio = app::DEF_SB_CLOSED_WIDTH;

        this->m_rebuild_dockspace = true;
    }



    //  3.  DETERMINE WINDOW COLLAPSE...
    if (this->m_show_sidebar_window)
    {
        this->Display_Preferences_Menu();
    }
    else
    {
        //  ...
    }
        
        
    
 
    ImGui::End();
    
    return;
}



// *************************************************************************** //
//
//
//  2.      GENERAL MENU FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "Display_Preferences_Menu"
//
void App::Display_Preferences_Menu(void)
{
    ImGuiIO &                   io              = ImGui::GetIO(); (void)io;
    ImGuiStyle &                style           = ImGui::GetStyle();
    static bool                 show_perf       = false;
    

    ImGui::SeparatorText("System Preferences");

    //ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Appearance Mode")) {
        this->disp_appearance_mode();
        ImGui::TreePop();
    }
    
    if (ImGui::TreeNode("Font Selection")) {
        this->disp_font_selector();
        ImGui::TreePop();
    }
    
    if (ImGui::TreeNode("Color Palette")) {
        this->disp_color_palette();
        ImGui::TreePop();
    }

    
    
    this->disp_performance_metrics();
    
/*#ifndef __CBAPP_THROTTLE_PERF_METRICS__
    this->disp_performance_metrics();
# else
    if (ImGui::GetFrameCount() % __CBAPP_THROTTLE_PERF_METRICS__ == 0)
        this->disp_performance_metrics();
#endif  //  __CBAPP_THROTTLE_PERF_METRICS__  // */
        

    return;
}







//  "disp_appearance_mode"
//
void App::disp_appearance_mode(void)
{
    ImGuiIO &                   io              = ImGui::GetIO(); (void)io;
    ImGuiStyle &                style           = ImGui::GetStyle();

    static bool                 init            = true;
    ImGuiStyle *                ref             = nullptr;
    static ImGuiStyle           ref_saved_style;
    
    
    //  Style Mode Slider...
    if (ImGui::ShowStyleSelector("Appearance##Selector"))
        ref_saved_style = style;

    //  Default to using internal storage as reference
    if (init && ref == NULL)
        ref_saved_style = style;
    init = false;
    if (ref == NULL)
        ref = &ref_saved_style;


    return;
}




//  "disp_font_selector"
//
void App::disp_font_selector(void)
{
    ImGui::ShowFontSelector("Global Font##Selector");
    return;
}




//  "disp_color_palette"
//
void App::disp_color_palette(void)
{
    ImGuiIO &                       io              = ImGui::GetIO(); (void)io;
    ImGuiStyle &                    style           = ImGui::GetStyle();
    static ImVec4                   color           = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);
    static ImGuiColorEditFlags      base_flags      = ImGuiColorEditFlags_None;


    //  1.  COLOR EDITORS...
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Colors"))
    {
        static ImVec4 &     def_win_bg            = style.Colors[ImGuiCol_WindowBg];
        
        ImGui::ColorEdit4("GLFW Window Bg##2f",         (float*)&this->m_glfw_bg,       ImGuiColorEditFlags_None | ImGuiColorEditFlags_Float);
        ImGui::ColorEdit4("Dockspace Bg##2f",           (float*)&this->m_glfw_bg,       ImGuiColorEditFlags_None | ImGuiColorEditFlags_Float);
        ImGui::ColorEdit4("Sidebar Menu Bg##2f",        (float*)&this->m_sidebar_bg,    ImGuiColorEditFlags_None | ImGuiColorEditFlags_Float);
        ImGui::ColorEdit4("Main Window Bg##2f",         (float*)&this->m_main_bg,       ImGuiColorEditFlags_None | ImGuiColorEditFlags_Float);
        ImGui::ColorEdit4("Default Window Bg##2f",      (float*)&def_win_bg,            ImGuiColorEditFlags_None | ImGuiColorEditFlags_Float);
        
        ImGui::TreePop();
    }
    
    
    
    //  2.  COLOR PALLETE...
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Color Tool"))
    {
        ImGui::ColorEdit3("MyColor##1", (float*)&color, base_flags);
        ImGui::ColorEdit4("MyColor##2", (float*)&color, ImGuiColorEditFlags_DisplayHSV | base_flags);

        ImGui::ColorEdit4("MyColor##2f", (float*)&color, ImGuiColorEditFlags_Float | base_flags);
        ImGui::ColorEdit4("MyColor##3", (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | base_flags);
        

        // Generate a default palette. The palette will persist and can be edited.
        static bool saved_palette_init = true;
        static ImVec4 saved_palette[32] = {};
        if (saved_palette_init)
        {
            for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
            {
                ImGui::ColorConvertHSVtoRGB(n / 31.0f, 0.8f, 0.8f,
                    saved_palette[n].x, saved_palette[n].y, saved_palette[n].z);
                saved_palette[n].w = 1.0f; // Alpha
            }
            saved_palette_init = false;
        }
        
    
        ImGui::TreePop();
    }


    return;
}



//  "disp_performance_metrics"
//
void App::disp_performance_metrics(void) {
    using float_t                                       = float;
    using int_t                                         = int;
    
    ImGuiIO &                       io                  = ImGui::GetIO(); (void)io;
    ImGuiStyle &                    style               = ImGui::GetStyle();
    ImGuiContext &                  g                   = *GImGui;
    ImGuiMetricsConfig *            cfg                 = &g.DebugMetricsConfig;
    
    static float_t                  spf_ct, fps_ct, triangle_ct;                                            //  DATA VARIABLES.
    static int_t                    vertex_ct, index_ct, window_ct,  allocation_ct;
    static utl::ScrollingBuffer     spf_plot, index_plot, vertex_plot;
    static float_t                  TIME                = 0;
    
    static constexpr float_t        PLOT_PADDING        = 1.10f;                                            //  PLOT APPEARANCE.
    static constexpr float_t        WINDOW              = app::PERF_PLOTS_HISTORY_LENGTH;   // Plot record length (sec)
    static ImVec2                   PLOT_SIZE(-1, 150);
    
    static constexpr ImVec4         spf_c(app::DEF_APPLE_BLUE);
    static constexpr ImVec4         index_c(app::DEF_APPLE_BLUE);
    static constexpr ImVec4         vertex_c(app::DEF_APPLE_RED);
    static ImPlotAxisFlags          flags               = ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoSideSwitch |
                                                          ImPlotAxisFlags_NoSideSwitch | ImPlotAxisFlags_LockMin;
                                                          
    constexpr const char *          fps_fmt             = "Average Framerate: %.3f ms/frame (%.1f FPS)";    //  FORMAT STRINGS.
    constexpr const char *          window_fmt          = "%d Visible Windows";
    constexpr const char *          vertex_fmt          = "%d Vertices, %d Indices (%.1f triangles)";
    constexpr const char *          memory_fmt          = "%d Current Memory Allocations";
    
    
    
    
    //  0.  COMPUTE SPACING...
    static float                    SPACING             = 0.0f;
    static float                    DEF_SPACING         = 1.5 * ImGui::GetTextLineHeightWithSpacing();
    static float                    TEXT_SPACING        = 8.0 * ImGui::GetTextLineHeightWithSpacing();
    static float                    PLOT_SPACING        = 2.5 * PLOT_SIZE[1];
    
    
    SPACING                     =   (this->m_show_perf_metrics)                             ? TEXT_SPACING : DEF_SPACING;
    SPACING                    +=   (this->m_show_perf_plots && this->m_show_perf_metrics)  ? PLOT_SPACING : 0;
    
    
    
    ImGui::Dummy( ImVec2(0.0f, ImGui::GetContentRegionAvail().y - SPACING) );
    
    
    //  "PERFORMANCE METRICS"...
    ImGui::SetNextItemOpen(app::DEF_PERF_METRICS_STATE, ImGuiCond_Once);
    if (!ImGui::CollapsingHeader("Performance Metrics")) {
        this->m_show_perf_metrics = false;
        return;
    }
    else {
        this->m_show_perf_metrics = true;
    }
    
    
    
    
    if (cfg->ShowDebugLog)
        ImGui::ShowDebugLogWindow(&cfg->ShowDebugLog);
    if (cfg->ShowIDStackTool)
        ImGui::ShowIDStackToolWindow(&cfg->ShowIDStackTool);
    ImGui::DebugBreakClearData();


    //  1.  GET PERFORMANCE DATA...
    float_t     MAX_FPS         = 0.0f;
    float_t     MAX_GPU      = 0.0f;
    spf_ct                      = static_cast<float_t>(1000.0f / io.Framerate);
    fps_ct                      = static_cast<int_t>(io.Framerate);
    vertex_ct                   = static_cast<int_t>(io.MetricsRenderVertices);
    index_ct                    = static_cast<float_t>(io.MetricsRenderIndices);
    window_ct                   = io.MetricsRenderWindows;
    allocation_ct               = g.DebugAllocInfo.TotalAllocCount - g.DebugAllocInfo.TotalFreeCount;

    //  2.  COMPUTE PERFORMANCE DATA...
    TIME += ImGui::GetIO().DeltaTime;
    spf_plot.AddPoint(TIME,         spf_ct);
    index_plot.AddPoint(TIME,       index_ct);
    vertex_plot.AddPoint(TIME,      vertex_ct);
    
    for (const auto & pt : spf_plot.Data)
        MAX_FPS     = ImMax(static_cast<float>(MAX_FPS),    pt.y);
    for (const auto & pt : index_plot.Data)
        MAX_GPU     = ImMax(static_cast<float>(MAX_GPU),  pt.y);
    for (const auto & pt : vertex_plot.Data)
        MAX_GPU     = ImMax(static_cast<float>(MAX_GPU),  pt.y);
    MAX_FPS                 *= PLOT_PADDING;
    MAX_GPU                 *= PLOT_PADDING;
    
    
    
    
    
    //  3.  SPACING...
    //const float SPACING = PLOT_SIZE[1] + 3.0 * ImGui::GetTextLineHeightWithSpacing();
    // const float SPACING = 2*PLOT_SIZE[1] + 10.0 * ImGui::GetTextLineHeightWithSpacing();
    // ImGui::Dummy( ImVec2(0.0f, ImGui::GetContentRegionAvail().y - SPACING) );
    
    
        //  1.  TEXT ENTRIES:
        ImGui::BulletText(fps_fmt,              spf_ct,         fps_ct);
        ImGui::BulletText(vertex_fmt,           vertex_ct,      index_ct,       triangle_ct);
        ImGui::BulletText(window_fmt,           window_ct);
        ImGui::BulletText(memory_fmt,           allocation_ct);
        ImGui::NewLine();
    
    
    
        //  2.  PLOTS:
        ImGui::SetNextItemOpen(app::DEF_PERF_PLOTS_STATE, ImGuiCond_Once);
        if (!ImGui::TreeNode("Plots")) {
            this->m_show_perf_plots = false;
        }
        else {
            this->m_show_perf_plots = true;
            
            //      2A.     FRAMERATE...
            ImGui::SeparatorText("Framerate");
            
            //      1.2     FPS PLOTTING...
            this->PushFont( Font::Small );
            ImGui::PushID("##Scrolling");
            if (ImPlot::BeginPlot("##Scrolling", PLOT_SIZE))
            {
                ImPlot::SetupLegend(ImPlotLocation_SouthWest, ImPlotLegendFlags_None);            //    Legend Position.
                ImPlot::SetupAxes(nullptr, nullptr, flags, flags);
                ImPlot::SetupAxisLimits(ImAxis_X1, TIME - WINDOW, TIME, ImGuiCond_Always);
                ImPlot::SetupAxisLimits(ImAxis_Y1, 0, MAX_FPS);
                
                ImPlot::SetNextLineStyle(spf_c);                                        //  2A.1     "SEC-PER-FRAME"
                ImPlot::SetNextFillStyle(spf_c, 0.2);
                ImPlot::PlotLine("ms/frame", &spf_plot.Data[0].x, &spf_plot.Data[0].y, spf_plot.Data.size(), ImPlotLineFlags_Shaded, spf_plot.Offset, 2 * sizeof(float));
                ImPlot::EndPlot();
            }
            ImGui::PopID();
            this->PopFont();
            


            //      2B.     MEMORY ALLOCATIONS PLOTTING...
            ImGui::SeparatorText("GPU");
            
            this->PushFont( Font::Small );
            if (ImPlot::BeginPlot("##Scrolling", PLOT_SIZE))
            {
                ImPlot::SetupLegend(ImPlotLocation_SouthWest, ImPlotLegendFlags_None);            //    Legend Position.
                ImPlot::SetupAxes(nullptr, nullptr, flags, flags);
                ImPlot::SetupAxisLimits(ImAxis_X1, TIME - WINDOW, TIME, ImGuiCond_Always);
                ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0f, MAX_GPU, ImGuiCond_Always);

                ImPlot::SetNextLineStyle(index_c);                                          //  2B.1     "Indices"
                ImPlot::SetNextFillStyle(index_c, 0.2);
                ImPlot::PlotLine("Indices", &index_plot.Data[0].x, &index_plot.Data[0].y,
                                index_plot.Data.size(), ImPlotLineFlags_Shaded,
                                index_plot.Offset, 2 * sizeof(float));

                ImPlot::SetNextLineStyle(vertex_c);                                         //  2B.2     "Vertices"
                ImPlot::SetNextFillStyle(vertex_c, 0.2);
                ImPlot::PlotLine("Vertices",  &vertex_plot.Data[0].x, &vertex_plot.Data[0].y,
                                 vertex_plot.Data.size(), ImPlotLineFlags_Shaded,
                                 vertex_plot.Offset, 2 * sizeof(float));
                                 
                ImPlot::EndPlot();
            }
            this->PopFont();
            ImGui::TreePop();   //     END OF "PLOTS"...
        }
            
        //ImGui::TreePop();   //  END OF "PERFORMANCE METRICS"...
    //}//  END OF "PERFORMANCE METRICS"...
    
    
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

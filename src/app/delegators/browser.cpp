/***********************************************************************************
*
*       ********************************************************************
*       ****            B R O W S E R . C P P  ____  F I L E            ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond.
*               DATED:      May 06, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/app.h"
#include "app/delegators/_browser.h"
#include <random>
#include <algorithm>



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//      1.      INITIALIZATION  | DEFAULT CONSTRUCTOR, DESTRUCTOR, ETC...
// *************************************************************************** //
// *************************************************************************** //

//  Default Constructor.
//
Browser::Browser(app::AppState & src)
    : S(src)                        {   }


//  "initialize"
//
void Browser::initialize(void) {
    if (this->m_initialized)        { return; }
        
    this->init();
    return;
}


//  "init"          | protected
//
void Browser::init(void) {
    this->ms_PLOT_SIZE.y                           *= S.m_dpi_scale;
    
    this->m_window_class.DockNodeFlagsOverrideSet   = ImGuiDockNodeFlags_NoTabBar;
    return;
}


//  Destructor.
//
Browser::~Browser(void)             { this->destroy(); }


//  "destroy"       | protected
//
void Browser::destroy(void)         {   }



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 1.  "INIT." ]].












// *************************************************************************** //
//
//
//
//      2.A.    MAIN PUBLIC API...
// *************************************************************************** //
// *************************************************************************** //

//  "Begin"
//
void Browser::Begin([[maybe_unused]] const char *       uuid,
                    [[maybe_unused]] bool *             p_open,
                    [[maybe_unused]] ImGuiWindowFlags   flags)
{
    [[maybe_unused]] ImGuiIO &          io              = ImGui::GetIO();
    [[maybe_unused]] ImGuiStyle &       style           = ImGui::GetStyle();
    
    
    
    //  this->SettingsWindow();
    
    
    
    this->S.PushFont(Font::Small);
    
    
    
    //      1.      PUSH THE STYLE ITEMS FOR THIS WINDOW...
    ImGui::PushStyleColor   ( ImGuiCol_WindowBg                 , this->S.GetUIColor(app::UIColor::Browser_BG)          );      //  COLORS
    ImGui::PushStyleColor   ( ImGuiCol_ChildBg                  , this->S.GetUIColor(app::UIColor::Browser_ChildBG)     );
    //
    //
    ImGui::PushStyleVar     ( ImGuiStyleVar_ChildRounding       , this->S.m_browser_child_rounding                      );      //  DIMENSIONS
    ImGui::PushStyleVar     ( ImGuiStyleVar_ChildBorderSize     , this->S.m_browser_child_bs                            );
    //
    ImGui::PushStyleVar     ( ImGuiStyleVar_ItemSpacing         , this->S.m_browser_item_spacing                        );
    ImGui::PushStyleVar     ( ImGuiStyleVar_WindowPadding       , this->S.m_browser_window_padding                      );
    //
    //
    ImGui::SetNextWindowClass( &this->m_window_class );
    //
    //
    //      2.      BEGIN "BROWSER" WINDOW...
    ImGui::Begin(uuid, p_open, flags);
    //
        ImGui::PopStyleColor();     //  ImGuiCol_WindowBg
        ImGui::PopStyleVar();       //  ImGuiStyleVar_WindowPadding
        //
        //
        //
        this->_Begin_IMPL();
        //
        //
        //
        ImGui::PopStyleColor    (1);        //  ImGuiCol_ChildBg
        ImGui::PopStyleVar      (3);        //  ImGuiStyleVar_ItemSpacing       , ImGuiStyleVar_ChildBorderSize         , ImGuiStyleVar_ChildRounding
    //
    ImGui::End();   //  END WINDOW...
    
    
    
    
    
    
    this->S.PopFont();
    return;
}


//  "_Begin_IMPL"
//
void Browser::_Begin_IMPL(void)
{
    [[maybe_unused]] ImGuiIO &          io              = ImGui::GetIO();
    [[maybe_unused]] ImGuiStyle &       style           = ImGui::GetStyle();
    
    
    
    if (!this->S.m_show_browser_window)     { return; }
    
    
    

        
        
    //          2.1.    PER-FRAME CACHE...
    this->_perframe_cache();
    
    
    
    
    
    
    //          2.1.    DISPLAY PRIMARY SIDE-BAR CONTENT...
    //
    //                  2.1A.       SYSTEM-PREFERENCES TAB.
    if ( this->S.m_show_system_preferences )
    {
        this->DisplaySystemPreferencesTab();
    }
    //                  2.1B.       BROWSER TAB.
    else
    {
        this->DisplayBrowserTab();
    }
    
    
    
    
    
    
    //          2.2.    DISPLAY THE `INFO` TAB...
    //
    this->DisplayInfoTab();
        
        
        
    
    return;
}



//  "SettingsWindow"
//
void Browser::SettingsWindow(void)
{
    static bool         open                    = true;
    constexpr float     cv_CHILD_ROUNDING_MIN   = 0.0f;
    constexpr float     cv_CHILD_ROUNDING_MAX   = 30.0f;

    constexpr float     cv_BORDER_SIZE_MIN      = 0.0f;
    constexpr float     cv_BORDER_SIZE_MAX      = 10.0f;

    constexpr float     cv_ITEM_SPACING_MIN     = 0.0f;
    constexpr float     cv_ITEM_SPACING_MAX     = 35.0f;
    
    
    
    ImGui::SetNextWindowSize(ImVec2(600, 350), ImGuiCond_Once);
    ImGui::Begin("Browser Values", &open, ImGuiWindowFlags_None);
    //
    //
        ImGui::TextUnformatted("1. Browser Window:");
        ImGui::SliderFloat2("Window Padding", (float*)&S.m_browser_window_padding,
                            cv_ITEM_SPACING_MIN, cv_ITEM_SPACING_MAX, "%.1f");
        ImGui::SliderFloat2("Window Item-Spacing", (float*)&S.m_browser_item_spacing,
                            cv_ITEM_SPACING_MIN, cv_ITEM_SPACING_MAX, "%.1f");
        //
        ImGui::SliderFloat("Child Rounding", &S.m_browser_child_rounding,
                           cv_CHILD_ROUNDING_MIN, cv_CHILD_ROUNDING_MAX, "%.1f");
        ImGui::SliderFloat("Border Size", &S.m_browser_child_bs,
                           cv_BORDER_SIZE_MIN, cv_BORDER_SIZE_MAX, "%.1f");
        //
        //
        //
        ImGui::NewLine();
        ImGui::TextUnformatted("2. Child Window:");
        ImGui::SliderFloat2("Info Window Padding", (float*)&S.m_info_window_padding,
                            cv_ITEM_SPACING_MIN, cv_ITEM_SPACING_MAX, "%.1f");
        ImGui::SliderFloat2("Info Item-Spacing", (float*)&S.m_info_item_spacing,
                            cv_ITEM_SPACING_MIN, cv_ITEM_SPACING_MAX, "%.1f");
        //
        ImGui::SliderFloat("Info Rounding", &S.m_info_child_rounding,
                           cv_CHILD_ROUNDING_MIN, cv_CHILD_ROUNDING_MAX, "%.1f");
        ImGui::SliderFloat("Info Border Size", &S.m_info_child_bs,
                           cv_BORDER_SIZE_MIN, cv_BORDER_SIZE_MAX, "%.1f");
        //
        //
        //
        ImGui::NewLine();
        ImGui::TextUnformatted("3. Common Values:");
    //
    //
    ImGui::End();
    return;
}



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 2A.  "MAIN PUBLIC API" ]].












// *************************************************************************** //
//
//
//
//      2.B.    PRIMARY ORCHESTRATOR FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "DisplaySystemPreferencesTab"
//
void Browser::DisplaySystemPreferencesTab(void) noexcept
{
    [[maybe_unused]] ImGuiIO &          io              = ImGui::GetIO();
    [[maybe_unused]] ImGuiStyle &       style           = ImGui::GetStyle();
    
    
    
    ImGui::SeparatorText("System Preferences");



    //      SECTION:    TEXT
    if ( ImGui::TreeNode("Appearance Mode") )
    {
        this->disp_appearance_mode();
        ImGui::TreePop();
    }
    //
    //      SECTION:    TEXT
    if ( ImGui::TreeNode("Font Selection") )
    {
        this->disp_font_selector();
        ImGui::TreePop();
    }
    //
    //      SECTION:    TEXT
    if ( ImGui::TreeNode("Color Palette") )
    {
        this->disp_color_palette();
        ImGui::TreePop();
    }
    
    
    
    //  this->disp_performance_metrics();
    
    return;
}

//  "DisplayInfoTab"
//
void Browser::DisplayInfoTab(void) noexcept
{


    ImGui::PushStyleVar     ( ImGuiStyleVar_ChildRounding       , this->S.m_info_child_rounding                 );
    ImGui::PushStyleVar     ( ImGuiStyleVar_ChildBorderSize     , this->S.m_info_child_bs                       );
    //
    ImGui::PushStyleVar     ( ImGuiStyleVar_ItemSpacing         , this->S.m_info_item_spacing                   );
    ImGui::PushStyleVar     ( ImGuiStyleVar_WindowPadding       , this->S.m_info_window_padding                 );
    
    
    //      CASE 0 :    DISPLAY THE CHILD WINDOW...
    if ( ImGui::BeginChild("ChildRegion", ImVec2(0, 200), true, ImGuiWindowFlags_None) )
    {
        ImGui::Text("Inside the child window.");
        ImGui::Button("Button A");
        ImGui::Button("Button B");
    }
        
        
        
    ImGui::EndChild();
    ImGui::PopStyleVar(4);       //  ImGuiStyleVar_ChildRounding        , ImGuiStyleVar_ChildBorderSize     , ImGuiStyleVar_ItemSpacing       , ImGuiStyleVar_WindowPadding

    return;
}



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 2B.  "PRIMARY ORCHESTRATORS" ]].












// *************************************************************************** //
//
//
//
//      2.C.      "BROWSER" TAB STUFF...
// *************************************************************************** //
// *************************************************************************** //





//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 2C.  "`BROWSER` TAB" ]].












// *************************************************************************** //
//
//
//
//      2.D.      "SYSTEM PREFERENCES" TAB STUFF...
// *************************************************************************** //
// *************************************************************************** //

//  "disp_appearance_mode"
//
void Browser::disp_appearance_mode(void)
{
    [[maybe_unused]] ImGuiIO &      io              = ImGui::GetIO(); (void)io;
    [[maybe_unused]] ImGuiStyle &   style           = ImGui::GetStyle();

    //  static bool                     init            = true;
    //  ImGuiStyle *                    ref             = nullptr;
    //  static ImGuiStyle               ref_saved_style;
    
    int     app_idx         = static_cast<int>(S.m_current_app_color_style);
    int     plot_idx        = static_cast<int>(S.m_current_plot_color_style);



    //      1.      COMBO-BOX FOR APP COLOR STYLE.
    if (ImGui::Combo( "##Browser_SetAppColorStyle",
                      &app_idx,
                      S.ms_APP_COLOR_STYLE_NAMES.data(),                        //  Names table
                      static_cast<int>(app::AppColorStyle_t::COUNT)) )          //  Item count
    {
        S.SetAppColorStyle( static_cast<app::AppColorStyle_t>(app_idx) );
    }


    //      2.      COMBO-BOX FOR PLOT COLOR STYLE.
    if (ImGui::Combo( "##Browser_SetPlotColorStyle",
                      &plot_idx,
                      S.ms_PLOT_COLOR_STYLE_NAMES.data(),                       //  Names table
                      static_cast<int>(app::PlotColorStyle_t::COUNT)) )         //  Item count
    {
        S.SetPlotColorStyle( static_cast<app::PlotColorStyle_t>(plot_idx) );
    }
    
    
    //  3.  UI-SCALE WIDGETS.
    //  ImGui::BeginDisabled(true);
    //      this->S.m_ui_scaler.draw_controls();
    //  ImGui::EndDisabled();


    
    /*
    //  Style Mode Slider...
    if (ImGui::ShowStyleSelector("Appearance##Selector"))
        ref_saved_style = style;

    //  Default to using internal storage as reference
    if (init && ref == NULL)
        ref_saved_style = style;
    init = false;
    if (ref == NULL)
        ref = &ref_saved_style;
    */


    return;
}


//  "disp_font_selector"
//
void Browser::disp_font_selector(void)
{
    ImGui::ShowFontSelector("Global Font##Selector");
    return;
}


//  "disp_ui_scale"
//
void Browser::disp_ui_scale(void)
{



    if ( ImGui::BeginTabItem("Fonts") )
    {
        ImGuiIO &       io      = ImGui::GetIO();
        ImFontAtlas *   atlas   = io.Fonts;
        
        utl::HelpMarker("Read FAQ and docs/FONTS.md for details on font loading.");
        ImGui::ShowFontAtlas(atlas);

        // Post-baking font scaling. Note that this is NOT the nice way of scaling fonts, read below.
        // (we enforce hard clamping manually as by default DragFloat/SliderFloat allows CTRL+Click text to get out of bounds).
        const float     MIN_SCALE       = 0.3f;
        const float     MAX_SCALE       = 2.0f;
        static float    window_scale    = 1.0f;
        
        utl::HelpMarker(
            "Those are old settings provided for convenience.\n"
            "However, the _correct_ way of scaling your UI is currently to reload your font at the designed size, "
            "rebuild the font atlas, and call style.ScaleAllSizes() on a reference ImGuiStyle structure.\n"
            "Using those settings here will give you poor quality results."
        );
        
        
        ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
        
        
        if ( ImGui::DragFloat("window scale", &window_scale, 0.005f, MIN_SCALE, MAX_SCALE, "%.2f", ImGuiSliderFlags_AlwaysClamp) ) {  //  Scale only this window
            ImGui::SetWindowFontScale(window_scale);
        }
        
        ImGui::DragFloat("global scale", &io.FontGlobalScale, 0.005f, MIN_SCALE, MAX_SCALE, "%.2f", ImGuiSliderFlags_AlwaysClamp); // Scale everything
        ImGui::PopItemWidth();
        ImGui::EndTabItem();
    }
    
    return;
}



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 2D.  "`SYSTEM PREFERENCES` TAB" ]].












// *************************************************************************** //
//
//
//
//      2.E.      "INFO"-CONSOLE TAB STUFF...
// *************************************************************************** //
// *************************************************************************** //





//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 2E.  "`INFO` TAB" ]].












// *************************************************************************** //
//
//
//
//      2.F.      STAND-ALONE TOOLS...
// *************************************************************************** //
// *************************************************************************** //

//  "disp_color_palette"
//
void Browser::disp_color_palette(void)
{
    ImGuiIO &                       io              = ImGui::GetIO(); (void)io;
    [[maybe_unused]] ImGuiStyle &   style           = ImGui::GetStyle();
    static ImVec4                   color           = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);
    static ImGuiColorEditFlags      base_flags      = ImGuiColorEditFlags_None;



    //      1.      COLOR EDITORS...
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::TreeNode("Colors") )
    {
        this->S.m_settings._display_color_palette();
        ImGui::TreePop();
    }
    
    
    
    //      2.      COLOR PALLETE...
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if ( ImGui::TreeNode("Color Tool") )
    {
        constexpr const size_t      NUM_PALETTE     = 32ULL;
    
    
    
        ImGui::ColorEdit3("MyColor##1"  , (float*)&color    , base_flags);
        ImGui::ColorEdit4("MyColor##2"  , (float*)&color    , ImGuiColorEditFlags_DisplayHSV | base_flags);

        ImGui::ColorEdit4("MyColor##2f" , (float*)&color    , ImGuiColorEditFlags_Float | base_flags);
        ImGui::ColorEdit4("MyColor##3"  , (float*)&color    , ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | base_flags);
        

        // Generate a default palette. The palette will persist and can be edited.
        static bool         saved_palette_init              = true;
        static  ImVec4      saved_palette[NUM_PALETTE]      = {   };
        
        if ( saved_palette_init )
        {
            for (size_t n = 0; n < NUM_PALETTE; ++n)
            {
                ImGui::ColorConvertHSVtoRGB(
                      n / 31.0f
                    , 0.8f
                    , 0.8f
                    , saved_palette[n].x
                    , saved_palette[n].y
                    , saved_palette[n].z
                );
                //
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
void Browser::disp_performance_metrics(void) {
    using float_t                                       = float;
    using int_t                                         = int;
    
    [[maybe_unused]] ImGuiIO &      io                  = ImGui::GetIO(); (void)io;
    [[maybe_unused]] ImGuiStyle &   style               = ImGui::GetStyle();
    ImGuiContext &                  g                   = *GImGui;
    ImGuiMetricsConfig *            cfg                 = &g.DebugMetricsConfig;
    
    static float_t                  spf_ct, fps_ct, triangle_ct;                                            //  DATA VARIABLES.
    static int_t                    vertex_ct, index_ct, window_ct,  allocation_ct;
    static utl::ScrollingBuffer     spf_plot, index_plot, vertex_plot;
    static float_t                  TIME                = 0;
    
    static constexpr float_t        PLOT_PADDING        = 1.10f;                                            //  PLOT APPEARANCE.
    static constexpr float_t        WINDOW              = app::PERF_PLOTS_HISTORY_LENGTH;   // Plot record length (sec)
    
    static constexpr ImVec4         spf_c(app::DEF_APPLE_BLUE);
    static constexpr ImVec4         index_c(app::DEF_APPLE_BLUE);
    static constexpr ImVec4         vertex_c(app::DEF_APPLE_RED);
    static ImPlotAxisFlags          flags               = ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoSideSwitch |
                                                          ImPlotAxisFlags_NoSideSwitch | ImPlotAxisFlags_LockMin;
                                                          
    constexpr const char *          fps_fmt             = "Avg. Framerate: %.3f ms/frame (%.1f FPS)";    //  FORMAT STRINGS.
    constexpr const char *          window_fmt          = "%d Visible Windows";
    constexpr const char *          vertex_fmt          = "%d Vertices.  %d Indices.  %.1f Triangles.";
    constexpr const char *          memory_fmt          = "%d Memory Allocations";
    
    
    
    
    //  0.  COMPUTE SPACING...
    static float                    SPACING             = 0.0f;
    static float                    DEF_SPACING         = 1.5 * ImGui::GetTextLineHeightWithSpacing();
    static float                    TEXT_SPACING        = 6.5 * ImGui::GetTextLineHeightWithSpacing();
    static float                    PLOT_SPACING        = 2.5 * ms_PLOT_SIZE[1];
    
    
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
    float_t     MAX_GPU         = 0.0f;
    spf_ct                      = static_cast<float_t>(1000.0f / io.Framerate);
    fps_ct                      = static_cast<int_t>(io.Framerate);
    vertex_ct                   = static_cast<int_t>(io.MetricsRenderVertices);
    index_ct                    = static_cast<float_t>(io.MetricsRenderIndices);
    triangle_ct                 = static_cast<float_t>(vertex_ct / 3.0f);
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
        MAX_GPU     = ImMax(static_cast<float>(MAX_GPU),    pt.y);
    for (const auto & pt : vertex_plot.Data)
        MAX_GPU     = ImMax(static_cast<float>(MAX_GPU),    pt.y);
    MAX_FPS                 *= PLOT_PADDING;
    MAX_GPU                 *= PLOT_PADDING;
    
    
    
    
    
    //  3.  SPACING...
    //const float SPACING = ms_PLOT_SIZE[1] + 3.0 * ImGui::GetTextLineHeightWithSpacing();
    // const float SPACING = 2*ms_PLOT_SIZE[1] + 10.0 * ImGui::GetTextLineHeightWithSpacing();
    // ImGui::Dummy( ImVec2(0.0f, ImGui::GetContentRegionAvail().y - SPACING) );
    
    
        //  1.  TEXT ENTRIES:
        S.PushFont( Font::Small );
        ImGui::BulletText(fps_fmt,              spf_ct,         fps_ct);
        ImGui::BulletText(vertex_fmt,           vertex_ct,      index_ct,       triangle_ct);
        ImGui::BulletText(window_fmt,           window_ct);
        ImGui::BulletText(memory_fmt,           allocation_ct);
        ImGui::NewLine();
        S.PopFont();
    
    
    
        //  2.  PLOTS:
        ImGui::SetNextItemOpen(app::DEF_PERF_PLOTS_STATE, ImGuiCond_Once);
        if (!ImGui::TreeNode("Plots")) {
            this->m_show_perf_plots = false;
        }
        else
        {
            this->m_show_perf_plots = true;
            
            //      2A.     FRAMERATE...
            ImGui::SeparatorText("Framerate");
            
            //      1.2     FPS PLOTTING...
            S.PushFont( Font::FootNote );
            ImGui::PushID("##Scrolling");
            if (ImPlot::BeginPlot("##Scrolling", ms_PLOT_SIZE))
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
            S.PopFont();
            


            //      2B.     MEMORY ALLOCATIONS PLOTTING...
            ImGui::SeparatorText("GPU");
            
            S.PushFont( Font::FootNote );
            if (ImPlot::BeginPlot("##Scrolling", ms_PLOT_SIZE))
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
            S.PopFont();
            ImGui::TreePop();   //     END OF "PLOTS"...
        }
            
        //ImGui::TreePop();   //  END OF "PERFORMANCE METRICS"...
    //}//  END OF "PERFORMANCE METRICS"...
    
    
    return;
}




//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 2F.  "STAND-ALONE TOOLS" ]].












// *************************************************************************** //
//
//
//
//      2.X.      MISC. / UTILITY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_perframe_cache"
//
inline void Browser::_perframe_cache(void) noexcept
{
    this->m_avail = ImGui::GetContentRegionAvail();

    return;
}




//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 2X.  "MISC. / UTILITY" ]].















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

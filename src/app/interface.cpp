//
//  app.cpp
//  CBApp
//
//  Created by Collin Bond on 4/16/25.
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
    ImGuiIO &                       io              = ImGui::GetIO(); (void)io;
    ImGuiStyle &                    style           = ImGui::GetStyle();
    
    
    //  1.  CREATE THE WINDOW AND BEGIN APPENDING WIDGETS INTO IT...
    ImGui::PushStyleColor(ImGuiCol_WindowBg, this->m_sidebar_bg);   // Push before ImGui::Begin()
    ImGui::Begin(app::DEF_SIDEBAR_WIN_TITLE, p_open, this->m_sidebar_win_flags);
    ImGui::PopStyleColor();
    
    



    
    this->Display_Preferences_Menu();
        
        
    
 
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
            ImGui::PushFont( m_fonts[ Font::Small ] );// place legend in the upper-right corner, vertical orientation
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
            ImGui::PopFont();
            


            //      2B.     MEMORY ALLOCATIONS PLOTTING...
            ImGui::SeparatorText("GPU");
            ImGui::PushFont( m_fonts[ Font::Small ] );// place legend in the upper-right corner, vertical orientation
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
            ImGui::PopFont();
            ImGui::TreePop();   //     END OF "PLOTS"...
        }
            
        //ImGui::TreePop();   //  END OF "PERFORMANCE METRICS"...
    //}//  END OF "PERFORMANCE METRICS"...
    
    
    return;
}





// *************************************************************************** //
//
//
//  3.      MENU-BAR FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "Display_Main_Menu_Bar"
//
void App::Display_Main_Menu_Bar(void)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File")) {         //  1.  "File" MENU...
            this->disp_file_menubar();
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Edit")) {         //  2.  "Edit" MENU...
            this->disp_edit_menubar();
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("View")) {         //  3.  "View" MENU...
            this->disp_view_menubar();
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Window")) {       //  4.  "Window" MENU...
            this->disp_window_menubar();
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Tools")) {        //  5.  "Tools" MENU...
            this->disp_tools_menubar();
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Help")) {         //  9.  "Help" MENU...
            this->disp_help_menubar();
            ImGui::EndMenu();
        }
        
        
        //
        //  ... add more top‑level menus here ...
        //
        
        
        ImGui::EndMainMenuBar();
    }

    return;
}


//  "disp_file_menubar"
//
void App::disp_file_menubar(void)
{
    ImGuiIO &       io          = ImGui::GetIO(); (void)io;
    ImGuiStyle &    style       = ImGui::GetStyle();
    static cblib::ndmatrix<float>  test(4,4);


    //  1.  "New" SUB-MENU...
    if (ImGui::BeginMenu("New"))
    {
        ImGui::MenuItem("New File",         nullptr,        nullptr);
        ImGui::EndMenu();
    }

    
    //  2.  "Open" SUB-MENU...
    ImGui::Separator();
    if (ImGui::MenuItem("Open...",          "CTRL+O"))      { }
    if (ImGui::BeginMenu("Open Recent"))
    {
        //  ...
        ImGui::EndMenu();
    }
    
    
    //  3.  "Save" SUB-MENU...
    ImGui::Separator();
    if (ImGui::MenuItem("Save",                     "CTRL+S"))      { }
    if (ImGui::MenuItem("Save As...",               nullptr))       { }
    
    
    //  4.  "ImGui" FILES SUB-MENU...
    if (ImGui::BeginMenu("Save ImGui Files")) {
        this->disp_imgui_submenu();
        ImGui::EndMenu();
    }
    
    
    //  5.  MORE EXPORTS AND SAVES...
    if (ImGui::MenuItem("Export",                   nullptr))       { }
    
    
    //  4.  "Quit"...
    ImGui::Separator();
    if (ImGui::MenuItem("Quit",             "CTRL+Q"))      { }
    
    
    return;
}


//  "disp_edit_menubar"
//
void App::disp_edit_menubar(void)
{
    //  1.  "Paste" SUB-MENU...
    if (ImGui::MenuItem("Undo",             "CTRL+Z"))          { }
    if (ImGui::MenuItem("Redo",             "CTRL+Y"))          { }
    
    
    //  2.  "Paste" SUB-MENU...
    ImGui::Separator();
    if (ImGui::MenuItem("Cut",              nullptr))           { }
    if (ImGui::MenuItem("Copy",             "CTRL+C"))          { }
    
    
    //  3.  "Paste" SUB-MENU...
    if (ImGui::BeginMenu("Paste")) {
        ImGui::MenuItem("Paste",            "CTRL+V",           nullptr);
        ImGui::MenuItem("Paste Special",    "?+CTRL+V",         nullptr);
        ImGui::MenuItem("Paste and Preserve Formatting",
                                            "CTRL+ALT+V",       nullptr);
        ImGui::EndMenu();
    }
    
    
    return;
}


//  "disp_view_menubar"
//
void App::disp_view_menubar(void)
{
    ImGui::MenuItem("Item 1",         nullptr);
    ImGui::MenuItem("Item 2",         nullptr);
    return;
}


//  "disp_window_menubar"
//
void App::disp_window_menubar(void)
{
    using WinLoc    = utl::WindowLocation;

    ImGui::MenuItem("Minimize",         nullptr);
    ImGui::MenuItem("Zoom",             nullptr);
    ImGui::MenuItem("Fill",             nullptr);
    ImGui::MenuItem("Center",           nullptr);


    //  1.  "MOVE & RESIZE" SUB-MENU...
    ImGui::Separator();
    if (ImGui::BeginMenu("Move & Resize"))
    {
        //  1.1     Set GLFW Window to LEFT-HALF...
        if (ImGui::MenuItem("Left",       nullptr))
            { utl::SetGLFWWindowLocation(this->m_window, WinLoc::LeftHalf); }
        
        //  1.2     Set GLFW Window to RIGHT-HALF...
        if (ImGui::MenuItem("Right",       nullptr))
            { utl::SetGLFWWindowLocation(this->m_window, WinLoc::RightHalf); }
    
    
        //  1.9     Re-set GLFW window to default state...
        ImGui::Separator();
        if (ImGui::MenuItem("Return to Default Size",       nullptr))
            { utl::SetGLFWWindowLocation(this->m_window, WinLoc::Center, cb::app::DEF_ROOT_WINDOW_SCALE); }
        
    
        ImGui::EndMenu();
    }


    //  2.  "SHOW" SUB-MENU...
    ImGui::Separator();
    if (ImGui::BeginMenu("Show"))
    {
        ImGui::TextDisabled("Add some windows here");   //  2.1     SHOW SOME WINDOWS HERE...
        ImGui::EndMenu();
    }
        
    return;
}


//  "disp_tools_menubar"
//
void App::disp_tools_menubar(void)
{
    ImGui::MenuItem("Tool 1",     nullptr);
    ImGui::MenuItem("Tool 2",     nullptr);

    //  1.  DEBUG UTILITY MENU ITEMS...
    ImGui::Separator();
#ifndef __CBAPP_DEBUG__
    if (ImGui::BeginMenu("Debug Utilities")) {
# else
    ImGui::TextDisabled("Debug Utilities");
#endif  //  __CBAPP_DEBUG__  //
        ImGui::MenuItem("Style Editor",     nullptr,    &this->m_show_style_editor);        //  SHOW "STYLE EDITOR" APP...
        ImGui::MenuItem("Log",              nullptr,    &this->m_show_log_window);          //  SHOW "LOGGER" APP...
        ImGui::MenuItem("Console",          nullptr,    &this->m_show_console_window);      //  SHOW "CONSOLE" APP...
        ImGui::MenuItem("Metrics",          nullptr,    &this->m_show_metrics_window);      //  SHOW "METRICS" APP...
        
        ImGui::Separator();
        ImGui::MenuItem("ImGui Demo",       nullptr,    &this->m_show_imgui_demo);          //  SHOW "DEAR IMGUI" DEMO APP...
        ImGui::MenuItem("ImPlot Demo",      nullptr,    &this->m_show_implot_demo);         //  SHOW "DEAR IMPLOT" DEMO APP...
#ifdef CBAPP_ENABLE_CB_DEMO
        ImGui::MenuItem("CB Demo",          nullptr,    &this->m_show_cb_demo);             //  SHOW "CB" DEMO APP...
#endif  //  CBAPP_ENABLE_CB_DEMO  //
    
#ifndef __CBAPP_DEBUG__
        ImGui::EndMenu();
    }// END "Debug Utilities" GROUP...
#endif  //  __CBAPP_DEBUG__  //
    
    return;
}


//  "disp_help_menubar"
//
void App::disp_help_menubar(void)
{
    if (ImGui::MenuItem("User Guide"))      { }
    ImGui::Separator();
    if (ImGui::MenuItem("About"))           { }
    //  ImGui::Checkbox("\"Dear ImGui\" Demo", &this->m_show_imgui_demo);   //  <--- Using a check-button
    return;
}



// *************************************************************************** //
//
//
//      3.1     SUB-MENU MENU-BAR FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //


//  "disp_imgui_submenu"
//
void App::disp_imgui_submenu(void)
{
#ifdef CBAPP_DISABLE_INI
    ImGui::TextDisabled("Disabled at compile-time (#ifdef CBAPP_DISABLE_INI)");
# else
    ImGuiIO &                       io                  = ImGui::GetIO(); (void)io;
    ImGuiStyle &                    style               = ImGui::GetStyle();
    static ImGuiInputTextFlags      read_file_flags     = ImGuiInputTextFlags_None | ImGuiInputTextFlags_ElideLeft | ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_EnterReturnsTrue;
    static ImGuiInputTextFlags      write_file_flags    = ImGuiInputTextFlags_None | ImGuiInputTextFlags_ElideLeft | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue;
    static constexpr std::size_t    BUFF_SIZE           = 256ULL;
    static constexpr float          COOLDOWN            = 3.0f;
    
    static char                     buffer1[BUFF_SIZE];
    static char                     custom_ini_file     [BUFF_SIZE];
    static char                     current_ini_file    [BUFF_SIZE];
    static char                     default_ini_file    [BUFF_SIZE];
    static float                    TIME_CACHE          = 0.0f,     TIME = 0;
    static bool                     ONCE                = true;
    static bool                     TRIGGER             = false;
    
    if (ONCE) {
        std::strncpy(current_ini_file,  io.IniFilename,     BUFF_SIZE);
        std::strncpy(default_ini_file,  app::INI_FILEPATH,  BUFF_SIZE);
        
        ONCE = false;
    }
    
    
    //  4.1     ".ini" FILE SUB-SUB-MENU.
    if (ImGui::BeginMenu("Save ImGui \".ini\" File"))
    {
        //      4.1A    Save Custom ".ini" file.
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Custom File:");
        ImGui::SameLine();
        //
        if ( ImGui::InputText("##CustomIniFilepath", buffer1, BUFF_SIZE, write_file_flags) )
        {
            TRIGGER     = true;
            TIME_CACHE  = ImGui::GetIO().DeltaTime;
            TIME        = 0.0f;
            
            std::snprintf(custom_ini_file, BUFF_SIZE, "%s/%s%s", app::USER_DATA_DIR, buffer1, ".ini");
            ImGui::SaveIniSettingsToDisk(custom_ini_file);
        }
        ImGui::SameLine();
        utl::HelpMarker("Tooltip are typically created by using a IsItemHovered() + SetTooltip() sequence.\n\n"
                        "We provide a helper SetItemTooltip() function to perform the two with standards flags.");
        //
        //      4.1A-2  RESET COOLDOWN TIMER...
        if (TRIGGER) {
            TIME       += ImGui::GetIO().DeltaTime;
            TRIGGER     = static_cast<bool>( !(COOLDOWN <= std::abs(TIME - TIME_CACHE)) );
            ImGui::TextDisabled("Data was saved to \"%s\" at %.3f sec.", custom_ini_file, TIME_CACHE);
        }
        
        
        //      4.1B    Save *CURRENT* ".ini" file.
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Current File:");
        ImGui::SameLine();
        //
        if ( ImGui::InputText("##CurrentIniFilepath", current_ini_file, BUFF_SIZE, read_file_flags) ) {
            ImGui::SaveIniSettingsToDisk(custom_ini_file);
        }
        
        
        
        
        
        //      4.1X-1      Display default ".ini" file.
        ImGui::Separator();
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Default File:");
        ImGui::SameLine();
        ImGui::TextDisabled(app::INI_FILEPATH);
            
        //      4.1X-2      Overwrite/Save default ".ini" file.
        if (ImGui::MenuItem("Save Current Settings As Default",       nullptr)) {
            ImGui::SaveIniSettingsToDisk(cb::app::INI_FILEPATH);
        }
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_NoSharedDelay))
            ImGui::SetTooltip("Overwrite the default settings with the current ones (stored at \"%s\").", app::INI_FILEPATH);

        ImGui::EndMenu();
    }
    //
    //
    //  4.2     ".JSON" STYLE SUB-SUB-MENU.
    if (ImGui::BeginMenu("Save ImGui \"Style\" File"))
    {

        if (ImGui::MenuItem("Save ImGui \"Style\" File",       nullptr)) {
            utl::SaveStyleToDisk( ImGui::GetStyle(), cb::app::STYLE_FILEPATH);
        }


        ImGui::EndMenu();
    }
#endif  //  CBAPP_DISABLE_INI  //

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

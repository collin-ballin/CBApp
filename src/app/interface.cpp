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
    static constexpr const char *   win_title       = "Menu";       //  General Constants for the window.
    static int                      counter         = 0;
    static float                    scale           = 0.75f;
    ImGuiIO &                       io              = ImGui::GetIO(); (void)io;
    static ImGuiWindowFlags         sidebar_flags   = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar |
                                                      ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus; // ImGuiWindowFlags_NoSavedSettings
    
    
    
    
    //  1.  CREATE THE WINDOW AND BEGIN APPENDING WIDGETS INTO IT...
    ImGui::Begin(win_title, p_open, sidebar_flags);
    {
        static bool                 init            = true;
        ImGuiStyle &                style           = ImGui::GetStyle();
        ImGuiStyle *                ref             = nullptr;
        static ImGuiStyle           ref_saved_style;
        
        
        
        //      2.2     Application Settings
        ImGui::SeparatorText("Application Settings");
        {
            //  1.  GLFW Window Background...
            ImGui::ColorEdit4("Bg",     (float*)&this->m_clear_color, ImGuiColorEditFlags_DisplayHSV );
            
            //  2.  Window Scaling...
            ImGui::SliderFloat("Window Scale", &scale, 0.01f, 0.99f, "%.2f");
            scale = std::clamp(scale, 0.01f, 0.99f);    // Enforce bounds explicitly in case user types in a value
            if (ImGui::Button("Apply Scale"))
                utl::set_window_scale(this->m_window, scale);
                
                
                
            //  "SETTINGS" GROUP...
            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
            if (ImGui::TreeNode("Settings"))
            {
                //  3.  Style Mode Slider...
                if (ImGui::ShowStyleSelector("Appearance##Selector"))
                    ref_saved_style = style;
                ImGui::ShowFontSelector("Global Font##Selector");

                //  Default to using internal storage as reference
                if (init && ref == NULL)
                    ref_saved_style = style;
                init = false;
                if (ref == NULL)
                    ref = &ref_saved_style;
                    
                    
                    
                ImGui::TreePop();
            }// END "SETTINGS"...
                
                
        }
    }

        
        
        //  2.3     BOTTOM OF WINDOW | INI-Button, Frame-Rate, etc...
        {
            static constexpr float    MAX_SPF           = 12.0f;
            static constexpr float    window            = 10.0f;
            static constexpr ImVec4   color(0.244f, 0.467f, 0.847f, 1.0f);
            static ImPlotAxisFlags          flags       = ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoSideSwitch |
                                                          ImPlotAxisFlags_NoSideSwitch | ImPlotAxisFlags_LockMin;
            static ImVec2                   size(-1, 150);
            static utl::ScrollingBuffer     data;
            ImVec2                          mouse       = ImGui::GetMousePos();
            static float                    time        = 0;
            time                                       += ImGui::GetIO().DeltaTime;
            data.AddPoint(time, 1000.0f / io.Framerate);
            
            
                    
                    
                    
            //  1.  SPACING...
            const float OCCUPIED = size[1] + 2.5 * ImGui::GetTextLineHeight();
            ImGui::Dummy( ImVec2(0.0f, ImGui::GetContentRegionAvail().y - OCCUPIED) );
            
            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
            if (ImGui::TreeNode("Frame-Rate"))
            {
                //  2.  FPS-TEXT...
                ImGui::Text("Average: %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

                //  3.  FPS PLOTTING...
                //ImGui::PushFont(this->m_small_font);
                ImGui::PushFont( m_fonts[ Font::Small ] );
                if (ImPlot::BeginPlot("##Scrolling", size))
                {
                    ImPlot::SetupAxes(nullptr, nullptr, flags, flags);
                    
                    ImPlot::SetupAxisLimits(ImAxis_X1, time - window, time, ImGuiCond_Always);
                    ImPlot::SetupAxisLimits(ImAxis_Y1, 0, MAX_SPF);
                    
                    
                    //ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
                    //ImPlot::PlotShaded("msec per frame", &data.Data[0].x, &data.Data[0].y, data.Data.size(), -INFINITY, 0, data.Offset, 2 * sizeof(float));
                    
                    ImPlot::SetNextLineStyle(color);                                        //  4.  Plotting the Line...
                    ImPlot::SetNextFillStyle(color, 0.5);
                    ImPlot::PlotLine("ms/frame", &data.Data[0].x, &data.Data[0].y, data.Data.size(), ImPlotLineFlags_Shaded, data.Offset, 2 * sizeof(float));
                    ImPlot::EndPlot();
                }
                ImGui::PopFont();
                ImGui::TreePop();
            }
        }
    
 
    ImGui::End();
    
    return;
}






// *************************************************************************** //
//
//
//  2.      MENU FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //


//  "Display_Main_Menu_Bar"
//
void App::Display_Main_Menu_Bar(void)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File")) {         //  1.  "File" MENU...
            this->Display_file_menubar();
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Edit")) {         //  2.  "Edit" MENU...
            this->Display_edit_menubar();
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("View")) {         //  3.  "View" MENU...
            this->Display_view_menubar();
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Window")) {       //  4.  "Window" MENU...
            this->Display_window_menubar();
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Tools")) {        //  5.  "Tools" MENU...
            this->Display_tools_menubar();
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Help")) {         //  9.  "Help" MENU...
            this->Display_help_menubar();
            ImGui::EndMenu();
        }
        
        
        //
        //  ... add more top‑level menus here ...
        //
        
        
        ImGui::EndMainMenuBar();
    }

    return;
}


//  "Display_file_menubar"
//
void App::Display_file_menubar(void)
{
    cblib::ndmatrix<float>  test(4,4);


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
    
    
    //  4.  "ImGui" SAVING FILES...
    if (ImGui::BeginMenu("Save ImGui Files")) {
        
        if (ImGui::MenuItem("Save ImGui \".ini\" File",       nullptr)) {
            ImGui::SaveIniSettingsToDisk(cb::app::INI_FILEPATH);
        }
        if (ImGui::MenuItem("Save ImGui \"Style\" File",       nullptr)) {
            utl::SaveStyleToDisk( ImGui::GetStyle(), cb::app::STYLE_FILEPATH);
        }
        ImGui::EndMenu();
    }
    
    
    //  5.  MORE EXPORTS AND SAVES...
    if (ImGui::MenuItem("Export",                   nullptr))       { }
    
    
    //  4.  "Quit"...
    ImGui::Separator();
    if (ImGui::MenuItem("Quit",             "CTRL+Q"))      { }
    
    
    return;
}


//  "Display_edit_menubar"
//
void App::Display_edit_menubar(void)
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


//  "Display_view_menubar"
//
void App::Display_view_menubar(void)
{
    ImGui::MenuItem("Item 1",         nullptr);
    ImGui::MenuItem("Item 2",         nullptr);
    return;
}


//  "Display_window_menubar"
//
void App::Display_window_menubar(void)
{
    ImGui::MenuItem("Minimize",         nullptr);
    ImGui::MenuItem("Zoom",             nullptr);
    ImGui::MenuItem("Fill",             nullptr);
    ImGui::MenuItem("Center",           nullptr);


    //  1.  "SHOW" SUB-MENU...
    ImGui::Separator();
    if (ImGui::BeginMenu("Show"))
    {
        //  1.1     SHOW SOME WINDOWS HERE...
        ImGui::TextDisabled("Add some windows here");
    
        ImGui::EndMenu();
    }
        
    
    return;
}


//  "Display_tools_menubar"
//
void App::Display_tools_menubar(void)
{
    ImGui::MenuItem("Tool 1",     nullptr);
    ImGui::MenuItem("Tool 2",     nullptr);

    //  1.  DEBUG UTILITY MENU ITEMS...
    ImGui::Separator();
    ImGui::TextDisabled("(make sure to '#ifdef' these out for RELEASE builds...)");
    if (ImGui::BeginMenu("Debug Utilities")) {
        ImGui::MenuItem("Style Editor",     nullptr,    &this->m_show_style_editor);        //  SHOW "STYLE EDITOR" APP...
        ImGui::MenuItem("ImGui Demo",       nullptr,    &this->m_show_imgui_demo);          //  SHOW "DEAR IMGUI" DEMO APP...
        ImGui::MenuItem("ImPlot Demo",      nullptr,    &this->m_show_implot_demo);         //  SHOW "DEAR IMPLOT" DEMO APP...
    
        ImGui::EndMenu();
    }
    
    
    return;
}


//  "Display_help_menubar"
//
void App::Display_help_menubar(void)
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
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.






// *************************************************************************** //
// *************************************************************************** //
//
//  END.

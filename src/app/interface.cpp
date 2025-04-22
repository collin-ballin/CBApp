//
//  app.cpp
//  CBApp
//
//  Created by Collin Bond on 4/16/25.
//
// *************************************************************************** //
// *************************************************************************** //
#include "app/app.h"
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
    //ImVec2                          win_pos(this->m_main_viewport->WorkPos.x + 0,   this->m_main_viewport->WorkPos.x + 0);
    ImVec2                          win_pos(400,   600);
    ImVec2                          win_dims(600,       750);
    static int                      counter         = 0;
    static float                    scale           = 0.75f;
    ImGuiIO &                       io              = ImGui::GetIO(); (void)io;
    
    
    
    //  1.  ADD GENERAL WINDOW PARAMETERS ( CALL THESE **BEFORE** "BEGIN( ... )" )...
    //ImGui::SetNextWindowPos(win_pos,    ImGuiCond_Always);
    //ImGui::SetNextWindowSize(win_dims,  ImGuiCond_FirstUseEver);
    
    //  2.  CREATE THE WINDOW AND BEGIN APPENDING WIDGETS INTO IT...
    ImGui::Begin(win_title, p_open,    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
                                    
        //      2.1     Buttons to toggle each major window.
        ImGui::SeparatorText("Windows");
        {
            ImGui::Checkbox("Dear ImGui Demo",      &this->m_show_imgui_demo);
            ImGui::Checkbox("Dear ImPlot Demo",     &this->m_show_implot_demo);
            ImGui::Checkbox("My Application",       &this->m_show_main_window);
        }
        
        //      2.2     Application Settings
        ImGui::SeparatorText("Application Settings");
        {
            ImGui::SliderFloat("Float",             &this->m_clear_color.x, 0.0f, 1.0f);
            ImGui::ColorEdit3("Bg",                 (float*)&this->m_clear_color);
            
            
            ImGui::SliderFloat("Window Scale", &scale, 0.01f, 0.99f, "%.2f");
            scale = std::clamp(scale, 0.01f, 0.99f);    // Enforce bounds explicitly in case user types in a value
            if (ImGui::Button("Apply Scale"))
                utl::set_window_scale(this->m_window, scale);
        }
        
        
        
        
        //  2.3     BOTTOM OF WINDOW | INI-Button, Frame-Rate, etc...
        ImGui::Dummy( ImVec2(0.0f, ImGui::GetContentRegionAvail().y - 2.5 * ImGui::GetTextLineHeight()) );
        
        if (ImGui::Button("SAVE .ini FILE"))
            ImGui::SaveIniSettingsToDisk(cb::utl::INI_FILEPATH);
                
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    
    
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
        //  1.  "FILE" MENU...
        this->Display_file_menubar();
        
        
        //  2.  "EDIT" MENU...
        this->Display_edit_menubar();
        
        
        //  3.  "View" MENU...
        this->Display_view_menubar();
        
        
        //  4.  "Window" MENU...
        this->Display_window_menubar();
        
        
        //  5.  "Help" MENU...
        this->Display_help_menubar();
        
        
        
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
    if (ImGui::BeginMenu("File"))
    {
        //  1.  "New" SUB-MENU...
        if (ImGui::BeginMenu("New"))
        {
            ImGui::MenuItem("New File",     nullptr,        nullptr);
            ImGui::EndMenu();
        }
        
  
        
        //  2.  "Open" SUB-MENU...
        ImGui::Separator();
        if (ImGui::MenuItem("Open...",      "CTRL+O"))      { }
        if (ImGui::BeginMenu("Open Recent"))
        {
            //  ...
            ImGui::EndMenu();
        }
        
        
        //  3.  "Save" SUB-MENU...
        ImGui::Separator();
        if (ImGui::MenuItem("Save",         "CTRL+S"))      { }
        if (ImGui::MenuItem("Save As...",   nullptr))       { }
        if (ImGui::MenuItem("Export",       nullptr))       { }
        
        
        
        
        
        //  4.  "Quit"...
        ImGui::Separator();
        if (ImGui::MenuItem("Quit",         "CTRL+Q"))      { }
        
        
        
        ImGui::EndMenu();
    }
    
    return;
}


//  "Display_edit_menubar"
//
void App::Display_edit_menubar(void)
{
    if (ImGui::BeginMenu("Edit"))
    {
        if (ImGui::MenuItem("None"))    { }
        ImGui::EndMenu();
    }
    
    return;
}


//  "Display_view_menubar"
//
void App::Display_view_menubar(void)
{
    // bool ImGui::MenuItem(const char* label, const char* shortcut, bool selected, bool enabled)
    
    if (ImGui::BeginMenu("View"))
    {
    
        ImGui::EndMenu();
    }
    
    return;
}


//  "Display_window_menubar"
//
void App::Display_window_menubar(void)
{
    if (ImGui::BeginMenu("Window"))
    {
        ImGui::MenuItem("Menu item", "CMD+A");
    
    
        //  1.  "SHOW" SUB-MENU...
        ImGui::Separator();
        if (ImGui::BeginMenu("Show..."))
        {
            ImGui::MenuItem("My Application",   nullptr,    this->m_show_main_window);      //  SHOW "DEAR IMGUI" DEMO APP...
            ImGui::EndMenu();
        }
        
        
        
    
    
        //  9.  DEBUG UTILITY MENU ITEMS...
        ImGui::Separator();
        ImGui::TextDisabled("(make sure to '#ifdef' these out for RELEASE builds...)");
        if (ImGui::BeginMenu("Debug Utilities..."))
        {
            ImGui::MenuItem("ImGui Demo",       nullptr,    this->m_show_imgui_demo);       //  SHOW "DEAR IMGUI" DEMO APP...
            ImGui::MenuItem("ImPlot Demo",      nullptr,    this->m_show_implot_demo);      //  SHOW "DEAR IMPLOT" DEMO APP...
            //  ImGui::Checkbox("\"Dear ImGui\" Demo", &this->m_show_imgui_demo);   //  <--- Using a check-button
            ImGui::EndMenu();
        }
        
        
        ImGui::EndMenu();
    }
    
    return;
}


//  "Display_help_menubar"
//
void App::Display_help_menubar(void)
{
    if (ImGui::BeginMenu("Help"))
    {
        if (ImGui::MenuItem("None"))    { }
        ImGui::EndMenu();
    }
    
    return;
}


/*

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Tools")) {
            ImGui::MenuItem("Metrics",      nullptr, &show_implot_metrics);
            ImGui::MenuItem("Style Editor", nullptr, &show_implot_style_editor);
            ImGui::Separator();
            ImGui::MenuItem("ImGui Metrics",       nullptr, &show_imgui_metrics);
            ImGui::MenuItem("ImGui Style Editor",  nullptr, &show_imgui_style_editor);
            ImGui::MenuItem("ImGui Demo",          nullptr, &show_imgui_demo);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
*/










/*


    ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(600, 750), ImGuiCond_FirstUseEver);
    ImGui::Begin("ImPlot Demo", p_open, ImGuiWindowFlags_MenuBar);
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Tools")) {
            ImGui::MenuItem("Metrics",      nullptr, &show_implot_metrics);
            ImGui::MenuItem("Style Editor", nullptr, &show_implot_style_editor);
            ImGui::Separator();
            ImGui::MenuItem("ImGui Metrics",       nullptr, &show_imgui_metrics);
            ImGui::MenuItem("ImGui Style Editor",  nullptr, &show_imgui_style_editor);
            ImGui::MenuItem("ImGui Demo",          nullptr, &show_imgui_demo);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
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

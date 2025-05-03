//
//  menubar.cpp
//  CBApp
//
//  Created by Collin Bond on 5/1/25.
//
// *************************************************************************** //
// *************************************************************************** //
#include "app/menubar/_menubar.h"
//#include "app/app.h"
//#include "_config.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //


// *************************************************************************** //
//
//
//  1.      INITIALIZATION  | DEFAULT CONSTRUCTOR, DESTRUCTOR, ETC...
// *************************************************************************** //
// *************************************************************************** //

//  Default Constructor.
//
MenuBar::MenuBar(app::AppState & src)
    : m_state(src)                  { }


//  "init"          | private
//
void MenuBar::init(void)            { }


//  "load"
//
void MenuBar::load(void)            { }


//  Destructor.
//
MenuBar::~MenuBar(void)             { this->destroy(); }


//  "destroy"       | protected
//
void MenuBar::destroy(void)         { }




// *************************************************************************** //
//
//
//  1B.     PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "Begin"
//
void MenuBar::Begin([[maybe_unused]] const char *       uuid,
                    [[maybe_unused]] bool *             p_open,
                    [[maybe_unused]] ImGuiWindowFlags   flags)
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



// *************************************************************************** //
//
//
//  2.      GENERAL MENU FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//
//
//  3.      MENU-BAR FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "disp_file_menubar"
//
void MenuBar::disp_file_menubar(void)
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
void MenuBar::disp_edit_menubar(void)
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
void MenuBar::disp_view_menubar(void)
{
    ImGui::MenuItem("Item 1",         nullptr);
    ImGui::MenuItem("Item 2",         nullptr);
    return;
}


//  "disp_window_menubar"
//
void MenuBar::disp_window_menubar(void)
{
    using WinLoc                    = utl::WindowLocation;
    GLFWwindow * main_window        = glfwGetCurrentContext();

    ImGui::MenuItem("Minimize",         nullptr);
    ImGui::MenuItem("Zoom",             nullptr);
    if (ImGui::MenuItem("Fill",       nullptr))
        { utl::SetGLFWWindowLocation(main_window, WinLoc::Fill); }
    ImGui::MenuItem("Center",           nullptr);


    //  1.  "MOVE & RESIZE" SUB-MENU...
    ImGui::Separator();
    if (ImGui::BeginMenu("Move & Resize"))
    {
        //  1.1     Set GLFW Window to FILL ENTIRE SCREEN...
        if (ImGui::MenuItem("Fill",       nullptr))
            { utl::SetGLFWWindowLocation(main_window, WinLoc::Fill); }
            
        //  1.2     Set GLFW Window to LEFT-HALF...
        if (ImGui::MenuItem("Left",       nullptr))
            { utl::SetGLFWWindowLocation(main_window, WinLoc::LeftHalf); }
        
        //  1.3     Set GLFW Window to RIGHT-HALF...
        if (ImGui::MenuItem("Right",       nullptr))
            { utl::SetGLFWWindowLocation(main_window, WinLoc::RightHalf); }
    
    
        //  1.9     Re-set GLFW window to default state...
        ImGui::Separator();
        if (ImGui::MenuItem("Return to Default Size",       nullptr))
            { utl::SetGLFWWindowLocation(main_window, WinLoc::Center, cb::app::DEF_ROOT_WINDOW_SCALE); }
        
    
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
void MenuBar::disp_tools_menubar(void)
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

        ImGui::MenuItem("Style Editor",     nullptr,    &this->m_state.m_windows[Window::StyleEditor].open);        //  SHOW "STYLE EDITOR" APP...
        ImGui::MenuItem("Log",              nullptr,    &this->m_state.m_windows[Window::Logs].open);               //  SHOW "LOGGER" APP...
        ImGui::MenuItem("Console",          nullptr,    &this->m_state.m_windows[Window::Console].open);            //  SHOW "CONSOLE" APP...
        ImGui::MenuItem("Metrics",          nullptr,    &this->m_state.m_windows[Window::Metrics].open);            //  SHOW "METRICS" APP...
    
        ImGui::Separator();
        ImGui::MenuItem("ImGui Demo",       nullptr,    &this->m_state.m_windows[Window::ImGuiDemo].open);          //  SHOW "DEAR IMGUI" DEMO APP...
        ImGui::MenuItem("ImPlot Demo",      nullptr,    &this->m_state.m_windows[Window::ImPlotDemo].open);         //  SHOW "DEAR IMPLOT" DEMO APP...


#ifdef CBAPP_ENABLE_CB_DEMO
        ImGui::MenuItem("CB Demo",          nullptr,    &this->m_state.m_windows[Window::CBDemo].open);             //  SHOW "CB" DEMO APP...
#endif  //  CBAPP_ENABLE_CB_DEMO  //
    
#ifndef __CBAPP_DEBUG__
        ImGui::EndMenu();
    }// END "Debug Utilities" GROUP...
#endif  //  __CBAPP_DEBUG__  //
 
    return;
}


//  "disp_help_menubar"
//
void MenuBar::disp_help_menubar(void)
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
void MenuBar::disp_imgui_submenu(void)
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

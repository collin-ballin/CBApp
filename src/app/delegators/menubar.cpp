/***********************************************************************************
*
*       ********************************************************************
*       ****              M E N U B A R . h  ____  F I L E              ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond
*               DATED:      May 06, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/delegators/_menubar.h"
#include "app/app.h"
#include <future>
//#include CBAPP_USER_CONFIG



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
    : S(src)                  { }


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
    using                               namespace               app;
    [[maybe_unused]] ImGuiIO &          io                      = ImGui::GetIO(); (void)io;
    [[maybe_unused]] ImGuiStyle &       style                   = ImGui::GetStyle();
    //
    //
    const app::MenuState_t &            MS                      = S.GetMenuState();
    //
    const bool                          has_file_new            = MS.has_capability(CBMenuCapabilityFlags_FileNew);
    const bool                          has_file_open           = MS.has_capability(CBMenuCapabilityFlags_FileNew);
    //  static cblib::ndmatrix<float>   test(4,4);
    
    
    


    //  1.  "New" SUB-MENU...
    ImGui::BeginDisabled( !has_file_new );
        if ( has_file_new && ImGui::BeginMenu("New") )
        {
            ImGui::MenuItem("New File",         nullptr,        nullptr);
            ImGui::EndMenu();
        }
    ImGui::EndDisabled();

    
    //  2.  "Open" SUB-MENU...
    ImGui::Separator();
    ImGui::BeginDisabled( !has_file_open );
    //
        if (ImGui::MenuItem("Open...",                  "CTRL+O"))      { }
        //
        //
        //
        ImGui::BeginDisabled( !has_file_open && !MS.has_open_recent() );
            if ( MS.has_open_recent()  &&  ImGui::BeginMenu("Open Recent") )
            {
                namespace               fs          = std::filesystem;
                
                
                //  Display a list of each "Open Recent" File...
                for ( const std::filesystem::path & file : MS.m_recent_files ) {
                
                    std::error_code     ec          {  };
                    const bool          exists      = fs::is_regular_file( fs::symlink_status(file, ec) )  &&  !ec;
                    //const bool          exists      = fs::is_regular_file( fs::symlink_status(file, ec) ) && !ec;
                    
                    
                    ImGui::BeginDisabled( !exists );
                    //
                        if ( ImGui::MenuItem( file.filename().string().c_str() ) )       {
                            /*  TO-DO:  OPEN THE FILE...    */
                        }
                    //
                    ImGui::BeginDisabled( );
                }
                
                
                ImGui::Separator();
                //
                if ( ImGui::MenuItem("Clear Menu") )      {  }
                //
                ImGui::EndMenu();
            }
        //
        //
        //
        ImGui::EndDisabled();
    //
    ImGui::EndDisabled();
    
    
    
    
    
    
    //  3.  "Save" SUB-MENU...
    ImGui::Separator();
    ImGui::BeginDisabled( !MS.has_capability(CBMenuCapabilityFlags_FileSave) );
        if (ImGui::MenuItem("Save",                     "CTRL+S")) {
            io.AddKeyEvent(ImGuiMod_Ctrl, true); io.AddKeyEvent(ImGuiKey_S, true);
        }
    ImGui::EndDisabled();
    //
    ImGui::BeginDisabled( !MS.has_capability(CBMenuCapabilityFlags_FileSaveAs) );
        if (ImGui::MenuItem("Save As...",               nullptr))       { }
    ImGui::EndDisabled();
    
    
    //  4.  "ImGui" FILES SUB-MENU...
    if (ImGui::BeginMenu("Save ImGui Files")) {
        this->disp_imgui_submenu();
        ImGui::EndMenu();
    }
    
    
    //  5.  MORE EXPORTS AND SAVES...
    ImGui::BeginDisabled( !MS.has_capability(CBMenuCapabilityFlags_FileExport) );
        if (ImGui::MenuItem("Export",                   nullptr))       { }
    ImGui::EndDisabled();
    
    
    //  4.  "Quit"...
    ImGui::Separator();
    if (ImGui::MenuItem("Quit",                     "CTRL+Q"))      {
        app::instance().enqueue_signal( app::CBSignalFlags_Shutdown );
    }
    
    
    return;
}


//  "disp_edit_menubar"
//
void MenuBar::disp_edit_menubar(void)
{
    ImGuiIO &                   io                  = ImGui::GetIO();
    constexpr bool              ENABLE_UNDO         = false;
    constexpr bool              ENABLE_REDO         = false;
    constexpr bool              ENABLE_CUT          = false;
    constexpr bool              ENABLE_COPY         = false;
    constexpr bool              ENABLE_PASTE        = false;
    
    //  1.  UNDO / REDO...
    ImGui::BeginDisabled(ENABLE_UNDO);
        if (ImGui::MenuItem("Undo",                             "CTRL+Z"))
        { io.AddKeyEvent(ImGuiMod_Shift,true);   io.AddKeyEvent(ImGuiMod_Ctrl,true);     io.AddKeyEvent(ImGuiKey_Z,true); }
    ImGui::EndDisabled();
    //
    ImGui::BeginDisabled(ENABLE_REDO);
        if (ImGui::MenuItem("Redo",                             "SHIFT+CTRL+Z"))
        { io.AddKeyEvent(ImGuiMod_Shift,true);   io.AddKeyEvent(ImGuiMod_Ctrl,true);     io.AddKeyEvent(ImGuiKey_Z,true); }
    ImGui::EndDisabled();
    
    
    
    //  2.  CUT / COPY...
    ImGui::Separator();
    ImGui::BeginDisabled(ENABLE_CUT);
        if (ImGui::MenuItem("Cut",                              nullptr))           { }
    ImGui::EndDisabled();
    //
    ImGui::BeginDisabled(ENABLE_COPY);
        if (ImGui::MenuItem("Copy",                             "CTRL+C"))          { }
    ImGui::EndDisabled();
    
    
    
    //  3.  "Paste" SUB-MENU...
    ImGui::BeginDisabled(ENABLE_PASTE);
        if (ImGui::BeginMenu("Paste")) {
            ImGui::MenuItem("Paste",                            "CTRL+V",           nullptr);
            ImGui::MenuItem("Paste Special",                    "SHIFT+CTRL+V",     nullptr);
            ImGui::MenuItem("Paste and Preserve Formatting",    "CTRL+ALT+V",       nullptr);
            ImGui::EndMenu();
        }
    ImGui::EndDisabled();
    
    
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
    
    
        //  1.9     Rebuild Dockspace...
        ImGui::Separator();
        if (ImGui::MenuItem("Rebuild Dockspace", nullptr))    {
            S.m_rebuild_dockspace           = true;
            //this->S.m_browser_ratio         = app::DEF_SB_OPEN_WIDTH;
            //if (!this->S.m_show_browser_window)
            //    this->S.m_show_browser_window = false;
        }
    
    
        ImGui::EndMenu();
    }


    //  2.  "SHOW" SUB-MENU...
    ImGui::Separator();
    if (ImGui::BeginMenu("Show"))
    {
        this->disp_show_windows_menubar();              //  2.1     SHOW VISIBILITY TOGGLE MENU HERE...
        ImGui::EndMenu();
    }
       

    return;
}


//  "disp_show_windows_menubar"
//
void MenuBar::disp_show_windows_menubar(void)
{
    static size_t                   idx                 = static_cast<size_t>(0);
    
    
    //      0.      DISPLAY "About This App" AT THE TOP...
    if ( ImGui::MenuItem( this->S.m_windows[ Window::AboutMyApp ].uuid.c_str(),
                          nullptr,
                          &this->S.m_windows[ Window::AboutMyApp ].open ) ) {  }
                     
    ImGui::Separator();
                         
    
    
    //      1.      DRAW EACH VISIBILITY STATE IN "Core Windows" WINDOWS...                 | SUB-MENU.
#ifdef __CBAPP_DEBUG__
    if (ImGui::BeginMenu("Core Windows (Debug Only)"))
    {
        for (idx = S.ms_WINDOWS_BEGIN; idx < S.ms_APP_WINDOWS_BEGIN; ++idx) {
            std::string       name          = this->S.m_windows[ static_cast<Window>(idx) ].uuid;
            
            
            if (name[0] == '#' && name[1] == '#')           { name = name.erase(0, 2);       }
            else if (name[0] == '#')                        { name = name.erase(0, 1);       }
            if ( ImGui::MenuItem(name.c_str(), nullptr, &this->S.m_windows[ static_cast<Window>(idx) ].open) )
            {
                ImGui::DockBuilderDockWindow( this->S.m_windows[ static_cast<Window>(idx) ].uuid.c_str(), S.m_main_dock_id );
                if ( this->S.ms_FOCUS_ON_OPEN_WINDOW )
                    { ImGui::SetWindowFocus( this->S.m_windows[Window::EditorApp].uuid.c_str() ); }
            }
        }
        
        ImGui::EndMenu();
    }//  END "Applications" SUB-MENU.
    
    ImGui::Separator();
#endif  //  __CBAPP_DEBUG__  //
    
    
    
    //      2.      DRAW EACH VISIBILITY STATE IN "Application Windows" WINDOWS...          | SUB-MENU.
    if ( ImGui::BeginMenu("Applications") )
    {
        for (idx = S.ms_APP_WINDOWS_BEGIN; idx < S.ms_APP_WINDOWS_END; ++idx)
        {
            if ( ImGui::MenuItem(this->S.m_windows[ static_cast<Window>(idx) ].uuid.c_str(),
                                 nullptr,
                                 &this->S.m_windows[ static_cast<Window>(idx) ].open) )
            {
                ImGui::DockBuilderDockWindow( this->S.m_windows[ static_cast<Window>(idx) ].uuid.c_str(), S.m_main_dock_id );
                if ( this->S.ms_FOCUS_ON_OPEN_WINDOW )
                    { ImGui::SetWindowFocus( this->S.m_windows[Window::EditorApp].uuid.c_str() ); }
            }
        }
        
        ImGui::EndMenu();
    }//  END "Applications" SUB-MENU.
    
    
    
    ImGui::Separator();
    ImGui::TextDisabled("Tools");
    
    
    
#ifdef CBAPP_ENABLE_DEBUG_WINDOWS
//
    //      3.      DRAW EACH VISIBILITY STATE IN "Demos" WINDOWS...                | SUB-MENU.
    if ( ImGui::BeginMenu("Debugging Tools") )
    {
        for (idx = S.ms_DEBUG_WINDOWS_BEGIN; idx < S.ms_DEBUG_WINDOWS_END; ++idx)
        {
            if ( ImGui::MenuItem(this->S.m_windows[ static_cast<Window>(idx) ].uuid.c_str(),
                                 nullptr,
                                 &this->S.m_windows[ static_cast<Window>(idx) ].open) )
            {
                ImGui::DockBuilderDockWindow( this->S.m_windows[ static_cast<Window>(idx) ].uuid.c_str(), S.m_main_dock_id );
                if ( this->S.ms_FOCUS_ON_OPEN_WINDOW )
                    { ImGui::SetWindowFocus( this->S.m_windows[Window::EditorApp].uuid.c_str() ); }
            }
        }
        
        ImGui::EndMenu();
    }//  END "Demos" SUB-MENU.
//
#endif  //  CBAPP_ENABLE_DEBUG_WINDOWS  //
    
    
    
    //      4.      DRAW EACH VISIBILITY STATE IN "Tools" WINDOWS...                | SUB-MENU.
    if ( ImGui::BeginMenu("Basic Tools") )
    {
        for (idx = S.ms_TOOL_WINDOWS_BEGIN; idx < S.ms_TOOL_WINDOWS_END; ++idx)
        {
            if ( ImGui::MenuItem(this->S.m_windows[ static_cast<Window>(idx) ].uuid.c_str(),
                                 nullptr,
                                 &this->S.m_windows[ static_cast<Window>(idx) ].open) )
            {
                S.DockAtHome( this->S.m_windows[ static_cast<Window>(idx) ].uuid.c_str() );
                if ( this->S.ms_FOCUS_ON_OPEN_WINDOW )
                    { ImGui::SetWindowFocus( this->S.m_windows[Window::EditorApp].uuid.c_str() ); }
            }
        }
        
        ImGui::EndMenu();
    }//  END "Tools" SUB-MENU.
    
    
    
    //      5.      DRAW EACH VISIBILITY STATE IN "*MY* TOOLS" WINDOWS...           | SUB-MENU.
    if ( ImGui::BeginMenu("Collin's Tools") )
    {
        for (idx = S.ms_MY_TOOLS_WINDOWS_BEGIN; idx < S.ms_MY_TOOLS_WINDOWS_END; ++idx)
        {
            if ( ImGui::MenuItem(this->S.m_windows[ static_cast<Window>(idx) ].uuid.c_str(),
                                 nullptr,
                                 &this->S.m_windows[ static_cast<Window>(idx) ].open) )
            {
                ImGui::DockBuilderDockWindow( this->S.m_windows[ static_cast<Window>(idx) ].uuid.c_str(), S.m_main_dock_id );
                if ( this->S.ms_FOCUS_ON_OPEN_WINDOW )
                    { ImGui::SetWindowFocus( this->S.m_windows[Window::EditorApp].uuid.c_str() ); }
            }
        }
        
        ImGui::EndMenu();
    }//  END "Tools" SUB-MENU.



    //      6.      DRAW **EXTRA** WINDOWS...                                       | SUB-MENU.
#ifdef CBAPP_ENABLE_OPTIONAL_WINDOWS
    if ( ImGui::BeginMenu("Extra") )
    {
        for (idx = S.ms_EXTRA_WINDOWS_BEGIN; idx < S.ms_EXTRA_WINDOWS_END; ++idx)
        {
            std::string       name          = this->S.m_windows[ static_cast<Window>(idx) ].uuid;
            if (name[0] == '#' && name[1] == '#')           { name = name.erase(0, 2);       }
            else if (name[0] == '#')                        { name = name.erase(0, 1);       }
            
            if ( ImGui::MenuItem(name.c_str(), nullptr, &this->S.m_windows[ static_cast<Window>(idx) ].open) ) {
                ImGui::DockBuilderDockWindow( this->S.m_windows[ static_cast<Window>(idx) ].uuid.c_str(), S.m_main_dock_id );
                if ( this->S.ms_FOCUS_ON_OPEN_WINDOW )
                    { ImGui::SetWindowFocus( this->S.m_windows[Window::EditorApp].uuid.c_str() ); }
            }
        }
        
        ImGui::EndMenu();
    }//  END "Extra" SUB-MENU.
#endif  //  CBAPP_ENABLE_OPTIONAL_WINDOWS //



    return;
}



//  "disp_tools_menubar"
//
void MenuBar::disp_tools_menubar(void)
{
    [[maybe_unused]] static size_t              idx                 = static_cast<size_t>(0);
    [[maybe_unused]] static WinInfo &           winfo               = S.m_windows[static_cast<Window>(idx)];
    
    
    //  1.  SUB-MENU    #1 :   "CBApp Applications"...
    ImGui::TextDisabled("CBApp Applications");
    {
        for (idx = S.ms_APP_WINDOWS_BEGIN; idx < S.ms_APP_WINDOWS_END; ++idx) {
            if ( ImGui::MenuItem(this->S.m_windows[ static_cast<Window>(idx) ].uuid.c_str(),
                                 nullptr,
                                 &this->S.m_windows[ static_cast<Window>(idx) ].open) )
            {
                ImGui::DockBuilderDockWindow( this->S.m_windows[ static_cast<Window>(idx) ].uuid.c_str(), S.m_main_dock_id );
                if ( this->S.ms_FOCUS_ON_OPEN_WINDOW )
                    { ImGui::SetWindowFocus( this->S.m_windows[Window::EditorApp].uuid.c_str() ); }
            }
        }
    }//  END "Basic Tools" SUB-MENU.
    
    
    
    //  2.  SUB-MENU    #2 :   "Basic Tools"...
    ImGui::Separator();
    ImGui::TextDisabled("Basic Tools");
    {
        for (idx = S.ms_TOOL_WINDOWS_BEGIN; idx < S.ms_TOOL_WINDOWS_END; ++idx) {
            if ( ImGui::MenuItem(this->S.m_windows[ static_cast<Window>(idx) ].uuid.c_str(),
                                 nullptr,
                                 &this->S.m_windows[ static_cast<Window>(idx) ].open) )
            {
                ImGui::DockBuilderDockWindow( this->S.m_windows[ static_cast<Window>(idx) ].uuid.c_str(), S.m_main_dock_id );
                if ( this->S.ms_FOCUS_ON_OPEN_WINDOW )
                    { ImGui::SetWindowFocus( this->S.m_windows[Window::EditorApp].uuid.c_str() ); }
            }
        }
    }//  END "Custom Tools" SUB-MENU.
    
    
    
    //  3.  SUB-MENU    #3 :   "Custom Tools"...
    ImGui::Separator();
    ImGui::TextDisabled("Custom Tools");
    {
        for (idx = S.ms_MY_TOOLS_WINDOWS_BEGIN; idx < S.ms_MY_TOOLS_WINDOWS_END; ++idx) {
            if ( ImGui::MenuItem(this->S.m_windows[ static_cast<Window>(idx) ].uuid.c_str(),
                                 nullptr,
                                 &this->S.m_windows[ static_cast<Window>(idx) ].open) )
            {
                ImGui::DockBuilderDockWindow( this->S.m_windows[ static_cast<Window>(idx) ].uuid.c_str(), S.m_main_dock_id );
                if ( this->S.ms_FOCUS_ON_OPEN_WINDOW )
                    { ImGui::SetWindowFocus( this->S.m_windows[Window::EditorApp].uuid.c_str() ); }
            }
        }
    }//  END "Custom Tools" SUB-MENU.
    


    return;
}



//  "disp_help_menubar"
//
void MenuBar::disp_help_menubar(void)
{
    if ( ImGui::MenuItem(this->S.m_windows[ Window::AboutMyApp ].uuid.c_str(),
                         nullptr,
                         &this->S.m_windows[ Window::AboutMyApp ].open) ) {  }
        
        
    ImGui::Separator();
    ImGui::BeginDisabled(true);
        if (ImGui::MenuItem("User Guide", nullptr, false))
        {
            //  ...
        }
    ImGui::EndDisabled();
    
    
    
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
    [[maybe_unused]] ImGuiIO &      io                  = ImGui::GetIO(); (void)io;
    [[maybe_unused]] ImGuiStyle &   style               = ImGui::GetStyle();
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
        std::strncpy(current_ini_file,  io.IniFilename,     BUFF_SIZE-1);
        std::strncpy(default_ini_file,  app::INI_FILEPATH,  BUFF_SIZE-1);
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
            S.m_logger.info( std::format("ImGui \".ini\" data was written to file \"{}\" at {:.3f} sec.", custom_ini_file, TIME_CACHE) );
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
            CB_LOG(LogLevel::Info, "Default ImGui \".ini\" settings overwritten (\"{}\")", cb::app::INI_FILEPATH);
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

        if (ImGui::MenuItem("Save ImGui \"Style\" File",       nullptr))
        {
        
            //  SPAWN SECONDARY THREAD...
            auto _  = std::async(std::launch::async, []()
            {
            //
                //  1.  Save ImPlot Settings...
                if ( utl::SaveImGuiStyleToDisk(      ImGui::GetStyle(),      cb::app::IMGUI_STYLE_FILEPATH) ) {
                    CB_LOG(LogLevel::Info, "Default ImGui style settings overwritten (\"{}\")", cb::app::IMGUI_STYLE_FILEPATH);
                }
                else {
                    CB_LOG(LogLevel::Warning, "Failed to overwrite default ImGui style settings at (\"{}\")", cb::app::IMGUI_STYLE_FILEPATH);
                }
                
                //  2.  Save ImPlot Settings...
                if ( utl::SaveImPlotStyleToDisk(      ImPlot::GetStyle(),      cb::app::IMPLOT_STYLE_FILEPATH) ) {
                    CB_LOG(LogLevel::Info, "Default ImPlot style settings overwritten (\"{}\")", cb::app::IMPLOT_STYLE_FILEPATH);
                }
                else {
                    CB_LOG(LogLevel::Warning, "Failed to overwrite default ImPlot style settings at (\"{}\")", cb::app::IMPLOT_STYLE_FILEPATH);
                }
                
                //  utl::SaveImPlotStyleToDisk(     ImPlot::GetStyle(),     cb::app::IMPLOT_STYLE_FILEPATH);
            //
            });
            
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

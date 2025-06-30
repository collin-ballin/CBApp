/***********************************************************************************
*
*       *********************************************************************
*       ****                 A P P . C P P  ____  F I L E                ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      April 16, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/app.h"
#include CBAPP_USER_CONFIG

#include <random>
#include <algorithm>

// #ifdef CBAPP_USE_VIEWPORT
# include "imgui_internal.h"
// #endif  //  CBAPP_USE_VIEWPORT  //



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//  1B.     PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "run"
//
void App::run(void)
{
    [[maybe_unused]] ImGuiIO &          io          = ImGui::GetIO(); (void)io;
    [[maybe_unused]] ImGuiContext *     g           = ImGui::GetCurrentContext();
    this->S.log_startup_info();


    //  1.  MAIN PROGRAM LOOP...
#ifdef __EMSCRIPTEN__
    io.IniFilename = nullptr;   //  For an Emscripten build, we are disabling file-system access.  So let's not attempt an
    EMSCRIPTEN_MAINLOOP_BEGIN   //  "fopen()" of the "imgui.ini" file.  You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
#else
    while ( !glfwWindowShouldClose(this->S.m_glfw_window) && S.m_running.load(std::memory_order_relaxed) )
#endif  //  __EMSCRIPTEN__  //
    {
        //  1.1     SET A POLL AND HANDLE EVENTS (inputs, window resize, etc.)...
        //          You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        //              - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        //              - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        //          Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        if (glfwGetWindowAttrib(this->S.m_glfw_window, GLFW_ICONIFIED))    { ImGui_ImplGlfw_Sleep(10); continue; }
        

        //  2.  START THE "DEAR IMGUI" FRAME...
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        //
        //
        // *************************************************************************** //
        
        
        //  3.  DRAW MAIN "UI" ELEMENTS...
        this->run_IMPL();


        // *************************************************************************** //
        //
        //
        //  4.  RENDERING...
        ImGui::Render();
        glfwGetFramebufferSize(this->S.m_glfw_window,  &this->S.m_window_w,  &this->S.m_window_h); // int display_w, display_h;     // glfwGetFramebufferSize(this->S.m_glfw_window, &display_w, &display_h);
        glViewport(0, 0, this->S.m_window_w, this->S.m_window_w);
        glClearColor(this->S.m_glfw_bg.x * this->S.m_glfw_bg.w, this->S.m_glfw_bg.y * this->S.m_glfw_bg.w, this->S.m_glfw_bg.z * this->S.m_glfw_bg.w, this->S.m_glfw_bg.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        //  5.  UPDATE & RENDER ADDITIONAL PLATFORM WINDOWS...
        //          ( Platform functions may change the current OpenGL context.  So, we SAVE/RESTORE it to make our code easier to copy-and-paste
        //            elsewhere.  For this specific demo app, we could also call  "glfwMakeContextCurrent(window)"  directly ).
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow * backup  = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup);
        }
        glfwSwapBuffers(this->S.m_glfw_window);
        
        
        
    }// END OF MAIN-LOOP...


    #ifdef __EMSCRIPTEN__
        EMSCRIPTEN_MAINLOOP_END;
    #endif  //  __EMSCRIPTEN__  //


    S.log_shutdown_info();


    return;
}






// *************************************************************************** //
//
//
//  2.      PROTECTED MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//
//
//  3.      PRIVATE MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "run_IMPL"
//
void App::run_IMPL(void)
{
    [[maybe_unused]] ImGuiIO &      io                  = ImGui::GetIO(); (void)io;
    [[maybe_unused]] ImGuiStyle &   style               = ImGui::GetStyle();
    static size_t                   idx                 = static_cast<size_t>(0);
    static const size_t &           WINDOWS_BEGIN       = this->S.ms_WINDOWS_BEGIN;   // static_cast<size_t>(Window::Sidebar);
    static const size_t &           WINDOWS_END         = this->S.ms_WINDOWS_END;     // static_cast<size_t>(Window::Count);

    static app::WinInfo &           winfo               = S.m_windows[static_cast<Window>(idx)];
    static bool                     first_frame         = true;
    
    
    
    //  0.1     RENDER THE DOCKSPACE...
#ifndef CBAPP_NEW_DOCKSPACE
    ImGui::PushStyleColor(ImGuiCol_WindowBg, this->m_dock_bg);
    //ImGui::DockSpaceOverViewport(0, this->m_main_viewport);
    ImGui::DockSpaceOverViewport(this->m_main_viewport, ImGuiDockNodeFlags_NoTabBar);
    ImGui::PopStyleColor();
#else
    //const ImGuiViewport *           viewport            = ImGui::GetMainViewport();
    //this->S.m_dockspace_id                              = ImGui::GetID(this->S.m_dock_name);
    this->S.m_dockspace_id                              = ImGui::GetID( S.m_windows[Window::Dockspace].uuid.c_str() );


    //  1.      FIRST-FRAME INITIALIZATIONS...
    if (first_frame) [[unlikely]] {
        this->InitDockspace();
    }// END OF "first_frame"...
    
#endif  //  CBAPP_NEW_DOCKSPACE  //



    //  1.      HANDLE ANY CALLS WHICH REQUIRED RE-DRAW OF DOCKING SPACE...
    if (S.m_rebuild_dockspace) [[unlikely]] {
        this->RebuildDockLayout();
        S.m_rebuild_dockspace = false;
    }



    //  2.      DRAW APPLICATION WINDOWS...
    //
    //          POLICY:     **DO NOT** check if the " winfo.render_fn" pointer is NULL...
    //                          - We assert that these must ALL be valid in the "init_asserts()".
    //                          - If one of these is NULL, something has gone very wrong and we  *WANT*  THE PROGRAM TO CRASH !!!
    for (idx = WINDOWS_BEGIN; idx < WINDOWS_END; ++idx)
    {
        winfo           = S.m_windows[ static_cast<Window>(idx) ];
        if (winfo.open) {
            winfo.render_fn( winfo.uuid.c_str(), &winfo.open, winfo.flags );
        }
    }
    
    
    //  1.      END OF FIRST-FRAME INITIALIZATIONS (SET THE INITIAL WINDOW FOCUS)...
    if (first_frame) [[unlikely]] {
        first_frame = false;
        ImGui::SetWindowFocus( S.current_task() );
    }
    

    //  3.      HANDLE ANY KEYBOARD SHORTCUTS...
    this->KeyboardShortcutHandler();
    
    
        
    //  4.      LASTLY, QUERY EACH SIGNAL HANDLER...
    this->QuerySignalStates();
    
    
    
    return;
}









// *************************************************************************** //
//
//
//
//  3.2     MAIN APPLICATION GUI FUNCTIONS...       | PRIVATE.
// *************************************************************************** //
// *************************************************************************** //

//  "ShowMainWindow"
//
void App::ShowMainWindow([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
{
    [[maybe_unused]] ImGuiIO &          io                  = ImGui::GetIO(); (void)io;
    [[maybe_unused]] static bool        first_frame         = true;
    
    
    
    //  1.  CREATE THE WINDOW AND BEGIN APPENDING WIDGETS INTO IT...
    ImGui::PushStyleColor(ImGuiCol_WindowBg, this->S.m_main_bg);
    ImGui::Begin(uuid, p_open, flags);
        ImGui::PopStyleColor();
    


        ImGui::Text("Here is the main window of the application!");
        ImGui::Text("Not too much going on here right now due to the tabular design of the project.  Rather, most "
                    "of the user's time will be spent on the designated tab for the application they intend to work with.");
        ImGui::Separator();     ImGui::NewLine();
    
    
        //this->Test_Browser();
        
        ImGui::NewLine();
        
        //  4.  TESTING COLUMNS...
        //  {
        //      this->Test_Editor();
        //  }
    
        //  5.  TESTING TAB BAR...
        //  {
        //      this->TestTabBar();
        //  }
    
    
    
#if defined(CBAPP_ENABLE_CB_DEMO) && !defined(__CBAPP_BUILD_CCOUNTER_APP__) && !defined(__CBAPP_BUILD_FDTD_APP__)
//
//
    //  5.  EXAMPLE APPS...
    {
        constexpr const char * window_uuid  = "Example: Documents";
        
        ShowExampleAppDocuments(window_uuid, nullptr, ImGuiWindowFlags_MenuBar);
        
        if (first_frame) {
            first_frame = false;
            ImGui::DockBuilderDockWindow(window_uuid,     S.m_main_dock_id);
        }
    }


    //  2.  TESTING PLOTTING / GRAPHING 0...
    {
        static bool             first       = true;
        static const char *     plot_uuid   = "Graphing V0";
        ImGui::Begin(plot_uuid, nullptr, app::_CBAPP_CORE_WINDOW_FLAGS);
            this->ImPlot_Testing0();
        ImGui::End();
        if (first) {
            first = false;
            ImGui::DockBuilderDockWindow( plot_uuid, S.m_main_dock_id );
        }
    }
//
//
#endif  //  CBAPP_ENABLE_CB_DEMO  AND  !__CBAPP_BUILD_CCOUNTER_APP__   AND  !__CBAPP_BUILD_FDTD_APP__  //
    
    
    ImGui::End();
    return;
}


//  "ShowDockspace"
//
void App::ShowDockspace([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
{
    ImGui::SetNextWindowPos         (S.m_main_viewport->WorkPos);                         //  Set up invisible host window covering the entire viewport
    ImGui::SetNextWindowSize        (S.m_main_viewport->WorkSize);
    ImGui::SetNextWindowViewport    (S.m_main_viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,       0.0f);      //  Make host window invisible with no padding, rounding, borders, etc.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,     0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,        ImVec2(0.0f, 0.0f));


    //  Host window flags (invisible, non-interactive)
    //  ImGuiWindowFlags host_window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;   //ImGui::Begin( "##DockHostWindow", nullptr, host_window_flags); // app::_CBAPP_HOST_WINDOW_FLAGS     app::APPLICATION_WINDOW_INFOS[0].flags
    
    
    //ImGui::Begin( "##DockHostWindow", nullptr, this->S.m_windows[Window::Host].flags);
    ImGui::Begin(uuid, nullptr, flags);
        ImGui::PopStyleVar(3);  //  DockSpace() creates docking area within this host window
        ImGui::DockSpace(this->S.m_dockspace_id);
    ImGui::End();
    
    
    return;
}






// *************************************************************************** //
//
//
//
//  3.3     APPLICATION HANDLER FUNCTIONS...        | PRIVATE.
// *************************************************************************** //
// *************************************************************************** //

//  "KeyboardShortcutHandler"
//
void App::KeyboardShortcutHandler(void)
{
    static ImGuiInputFlags          browser_key_flags       = ImGuiInputFlags_None; //   | ~ImGuiInputFlags_Repeat; // Merged flags
    static ImGuiInputFlags          detview_key_flags       = ImGuiInputFlags_None; //   | ~ImGuiInputFlags_Repeat; // Merged flags
    static ImGuiInputFlags          save_key_flags          = ImGuiInputFlags_None; //   | ~ImGuiInputFlags_Repeat; // Merged flags
    static ImGuiInputFlags          undo_key_flags          = ImGuiInputFlags_None;
    static ImGuiInputFlags          redo_key_flags          = ImGuiInputFlags_None;
    static const ImGuiKeyChord      BROWSER_KEY             = ImGuiKey_GraveAccent;
    static const ImGuiKeyChord      DETVIEW_KEY             = ImGuiMod_Shift | ImGuiKey_GraveAccent; //ImGuiMod_Shift | ImGuiKey_Apostrophe;
    static const ImGuiKeyChord      SAVE_KEY                = ImGuiMod_Ctrl | ImGuiKey_S;
    static const ImGuiKeyChord      UNDO_KEY                = ImGuiMod_Ctrl | ImGuiKey_Z;
    static const ImGuiKeyChord      REDO_KEY                = ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_Z;
    //
    [[maybe_unused]] ImGuiIO &      io                      = ImGui::GetIO();
    
    
    //  0.  UPDATE CURRENT APPLICATION STATE (NEEDED BEFORE WE SAVE/UNDO/REDO/ETC)...
    S.update_current_task();


    //  1.  HOTKEY TO OPEN/CLOSE BROWSER...
    if ( ImGui::IsKeyChordPressed(BROWSER_KEY, browser_key_flags) )         { this->m_controlbar.toggle_sidebar(); }
    
    //  2.  HOTKEY TO OPEN/CLOSE DETAIL VIEW...
    if ( ImGui::IsKeyChordPressed(DETVIEW_KEY, detview_key_flags) )         { this->m_detview.toggle(); }
    
    //  3.  SAVE HANDLER...
    if ( ImGui::IsKeyChordPressed(SAVE_KEY, save_key_flags) )               { this->SaveHandler(); }

    //  4.  UNDO HANDLER...
    if ( ImGui::IsKeyChordPressed(UNDO_KEY, undo_key_flags) )               { this->UndoHandler(); }

    //  5.  REDO HANDLER...
    if ( ImGui::IsKeyChordPressed(REDO_KEY, redo_key_flags) )               { this->RedoHandler(); }



    return;
}



//  "SaveHandler"
//
void App::SaveHandler(void)
{
    switch (S.m_current_task) {
        case Applet::CCounterApp        : { this->m_counter_app.save();     break; }
        case Applet::EditorApp          : { this->m_editor_app.save();      break; }
        case Applet::GraphApp           : { this->m_graph_app.save();       break; }
        default                         : { this->SaveHandler_Default();    break; }
    }
    
    return;
}


//  "UndoHandler"
//
void App::UndoHandler(void)
{
    switch (S.m_current_task) {
        case Applet::CCounterApp        : { this->m_counter_app.undo();     break; }
        case Applet::EditorApp          : { this->m_editor_app.undo();      break; }
        case Applet::GraphApp           : { this->m_graph_app.undo();       break; }
        default                         : { this->UndoHandler_Default();    break; }
    }
    
    return;
}


//  "RedoHandler"
//
void App::RedoHandler(void)
{
    switch (S.m_current_task) {
        case Applet::CCounterApp        : { this->m_counter_app.redo();     break; }
        case Applet::EditorApp          : { this->m_editor_app.redo();      break; }
        case Applet::GraphApp           : { this->m_graph_app.redo();       break; }
        default                         : { this->RedoHandler_Default();    break; }
    }
    
    return;
}


//  "QuerySignalStates"
//
inline void App::QuerySignalStates(void)
{
    using namespace     app;
    CBSignalFlags       bits        = S.m_pending.exchange(CBSignalFlags_None, std::memory_order_acquire);
    CBSignalFlags       bit         = bits & static_cast<CBSignalFlags>(-(int)bits);      //  Isolate LSB.
    
    
    while ( bits != CBSignalFlags_None )
    {
        switch (bit)
        {
            //  1.  SIGINT / SIGTERM / CTRL_C_EVENT.
            case CBSignalFlags_Shutdown :       {
                CB_LOG(LogLevel::Info, "Program recieved <CBSignalFlags_Shutdown>.");
                
                S.m_running.store(false, std::memory_order_relaxed);
                break;
            }

            //  2.  SIGHUP.
            case CBSignalFlags_ReloadCfg :      {
                CB_LOG(LogLevel::Info, "Program recieved <CBSignalFlags_ReloadCfg>.");
                //  reload_config();
                break;
            }

            //  3.  [OPERATOR NEW] / MALLOC FAILURE.
            case CBSignalFlags_NewFailure :      {
                CB_LOG(LogLevel::Critical, "Program recieved <CBSignalFlags_NewFailure>.");
                S.m_running.store(false, std::memory_order_relaxed);
                break;
            }

            //  3.  CUSTOM SIGNAL #1 [NOT IMPLEMENTED].
            case CBSignalFlags_Custom1 :        {
                CB_LOG(LogLevel::Info, "Program recieved <CBSignalFlags_Custom1> (How did you do this? This isn't even implemented yet?)");
                break;
            }

            //  ?.  DEFAULT.
            default:                            {
                break;
            }
        }
        bits &= ~bit;         // clear processed flag
    }



    return;
}






// *************************************************************************** //
//
//
//
//  3.4     PLACEHOLDER HANDLERS...
// *************************************************************************** //
// *************************************************************************** //

//  "SaveHandler_Default"
//
void App::SaveHandler_Default(void)
{
    static constexpr const char *   save_popup_id       = "S A V E   P R O G R A M  .  .  .";
            

    ImGui::OpenPopup(save_popup_id);
    
    
    ImVec2                          center              = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    utl::Popup_Save(save_popup_id);
    
    //ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_S, flags  | ImGuiInputFlags_Tooltip);

    CB_LOG( LogLevel::Info, "Applet::MainApp--save" );
    return;
}


//  "CopyHandler_Default"
//
void App::CopyHandler_Default(void)
{
    return;
}


//  "PasteHandler_Default"
//
void App::PasteHandler_Default(void)
{
    return;
}


//  "UndoHandler_Default"
//
void App::UndoHandler_Default(void) {
    S.m_logger.info( "Applet::MainApp--undo" );
    return;
}


//  "RedoHandler_Default"
//
void App::RedoHandler_Default(void) {
    S.m_logger.info( "Applet::MainApp--undo" );
    return;
}






// *************************************************************************** //
//
//
//
//  3.4     ADDITIONAL APP FUNCTIONS...    | PRIVATE.
// *************************************************************************** //
// *************************************************************************** //

//  "InitDockspace"
//
void App::InitDockspace(void)
{
    // ------------------------------------------------------------------
    // 0.  Locals & aliases
    // ------------------------------------------------------------------
    [[maybe_unused]] ImGuiIO &      io                  = ImGui::GetIO();   (void)io;
    [[maybe_unused]] ImGuiStyle &   style               = ImGui::GetStyle();
    static size_t                   idx                 = 0;
    const float                     toolbar_px          = 1.6f * ImGui::GetTextLineHeightWithSpacing();    // + style.FramePadding.y * 2.0f + style.ItemSpacing.y;
    S.m_controlbar_ratio                                = toolbar_px / S.m_main_viewport->WorkSize.y;
    

    //  1.      CLEAR EXISTING DOCK LAYOUT...
    ImGui::DockBuilderRemoveNode    (this->S.m_dockspace_id);
    ImGui::DockBuilderAddNode       (this->S.m_dockspace_id,        ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize   (this->S.m_dockspace_id,        S.m_main_viewport->WorkSize);


    // ------------------------------------------------------------------
    // 2.  First split:  create a thin *toolbar* strip at the top (Up)
    //      ┌──────────── toolbar ────────────┐
    //      └──────────── dock_main_id ───────┘
    // ------------------------------------------------------------------
    ImGuiID work_bottom_id = 0;
    ImGui::DockBuilderSplitNode(
        S.m_dockspace_id,
        ImGuiDir_Up,
        S.m_controlbar_ratio,
        &S.m_controlbar_dock_id,
        &work_bottom_id);
        
    
    //------------------------------------------------------------------
    // 3.  Split work_bottom → Browser (Left) + right_area
    //------------------------------------------------------------------
    ImGuiID right_area_id = 0;
    ImGui::DockBuilderSplitNode(
        work_bottom_id,
        ImGuiDir_Left,
        S.m_browser_ratio,
        &S.m_browser_dock_id,
        &right_area_id);

    //------------------------------------------------------------------
    // 4.  Split right_area → DetailView (Down) + Main (remainder)
    //------------------------------------------------------------------
    ImGui::DockBuilderSplitNode(
        right_area_id,
        ImGuiDir_Down,
        S.m_detview_ratio,
        &S.m_detview_dock_id,   // bottom panel
        &S.m_main_dock_id);     // top panel

    //------------------------------------------------------------------
    // 5.  Fetch nodes & apply flags
    //------------------------------------------------------------------
    S.m_controlbar_node                 = ImGui::DockBuilderGetNode(S.m_controlbar_dock_id);
    S.m_browser_node                    = ImGui::DockBuilderGetNode(S.m_browser_dock_id);
    S.m_detview_node                    = ImGui::DockBuilderGetNode(S.m_detview_dock_id);
    S.m_main_node                       = ImGui::DockBuilderGetNode(S.m_main_dock_id);

    S.m_controlbar_node ->LocalFlags   |= S.m_controlbar_node_flags;
    S.m_browser_node    ->LocalFlags   |= S.m_browser_node_flags;
    S.m_main_node       ->LocalFlags   |= S.m_main_node_flags;
    S.m_detview_node    ->LocalFlags   |= S.m_detview_node_flags;

    //------------------------------------------------------------------
    // 6.  Dock core windows into their default locations
    //------------------------------------------------------------------
    ImGui::DockBuilderDockWindow(S.m_windows[Window::ControlBar ].uuid.c_str(), S.m_controlbar_dock_id);
    ImGui::DockBuilderDockWindow(S.m_windows[Window::Browser    ].uuid.c_str(), S.m_browser_dock_id);
    ImGui::DockBuilderDockWindow(S.m_windows[Window::DetailView ].uuid.c_str(), S.m_detview_dock_id);
    for (idx = S.ms_RHS_WINDOWS_BEGIN; idx < S.ms_WINDOWS_END; ++idx)
    {
        app::WinInfo &w = S.m_windows[static_cast<Window>(idx)];
        if (w.open)
            ImGui::DockBuilderDockWindow(w.uuid.c_str(), S.m_main_dock_id);
    }

    //------------------------------------------------------------------
    // 7.  Finalise layout
    //------------------------------------------------------------------
    ImGui::DockBuilderFinish(S.m_dockspace_id);
    return;
}



//  "RebuildDockLayout"
//
void App::RebuildDockLayout(void)
{
    app::WinInfo &           winfo               = S.m_windows[static_cast<Window>(0)];
    
    //  1.  CLEAR EXISTING DOCK LAYOUT...
    //  ImGui::DockBuilderRemoveNode    (this->S.m_dockspace_id);
    //  ImGui::DockBuilderAddNode       (this->S.m_dockspace_id,        ImGuiDockNodeFlags_DockSpace);
    //  ImGui::DockBuilderSetNodeSize   (this->S.m_dockspace_id,        S.m_main_viewport->WorkSize);

    //  //  2.  CREATE SPLIT-DOCK NODES...
    //  ImGui::DockBuilderSplitNode     (this->S.m_dockspace_id,        ImGuiDir_Left,          this->S.m_browser_ratio,
    //                                   &S.m_browser_dock_id,          &S.m_main_dock_id);


    //  2.  ENABLE WINDOW VISIBILITY...
    S.m_windows[Window::ControlBar].open    = true;
    S.m_windows[Window::Browser].open       = true;
    S.m_windows[Window::MainApp].open       = true;
    S.m_windows[Window::DetailView].open    = true;


    //  3.  RE-INSERT ALL WINDOWS INTO THEIR DEFAULT DOCKING SPACE...
    ImGui::DockBuilderDockWindow(S.m_windows[Window::ControlBar ].uuid.c_str(), S.m_controlbar_dock_id);
    ImGui::DockBuilderDockWindow(S.m_windows[Window::Browser    ].uuid.c_str(), S.m_browser_dock_id);
    ImGui::DockBuilderDockWindow(S.m_windows[Window::DetailView ].uuid.c_str(), S.m_detview_dock_id);
    for (size_t idx = S.ms_APP_WINDOWS_BEGIN; idx < S.ms_WINDOWS_END; ++idx) {                              //  3.2     Remaining Windows.
        winfo           = S.m_windows[ static_cast<Window>(idx) ];
        if (winfo.open) {
            ImGui::DockBuilderDockWindow( winfo.uuid.c_str(), S.m_main_dock_id );
        }
    }
    
    
    //  4.  RETURN FOCUS TO MAIN APPLICATION...
    #if defined(__CBAPP_BUILD_CCOUNTER_APP__)
        ImGui::SetWindowFocus( this->S.m_windows[Window::CCounterApp].uuid.c_str() );
    # elif defined(__CBAPP_BUILD_FDTD_APP__)
        ImGui::SetWindowFocus( this->S.m_windows[Window::GraphApp].uuid.c_str() );
    # elif defined(__CBAPP_BUILD_EDITOR_APP__)
        ImGui::SetWindowFocus( this->S.m_windows[Window::EditorApp].uuid.c_str() );
    # else
        ImGui::SetWindowFocus( this->S.m_windows[Window::MainApp].uuid.c_str() );
    #endif  //  __CBAPP_BUILD_CCOUNTER_APP__  //

    return;
}

//  "OnDpiScaleChanged"
//
void App::OnDpiScaleChanged([[maybe_unused]] float xs, [[maybe_unused]] float ys)
{
    [[maybe_unused]] ImGuiIO &          io          = ImGui::GetIO();
    [[maybe_unused]] ImGuiStyle &       style       = ImGui::GetStyle();
    float                               new_scale   = std::max(xs, ys);
    
    if ( std::fabs(new_scale - this->S.m_dpi_scale) < 0.01f )   return;     // Ignore noise / tiny changes


    //  1.  Rescale all style metrics.
    style.ScaleAllSizes(new_scale / this->S.m_dpi_scale);

    //  2.  Rebuild fonts.
    //  io.Fonts->Clear();
    //  io.Fonts->AddFontFromFileTTF("assets/Inter-Medium.ttf", 16.0f * new_scale);

    //  ImGui_ImplOpenGL3_DestroyFontsTexture();
    //  ImGui_ImplOpenGL3_CreateFontsTexture();

    this->S.m_dpi_scale = new_scale;             // store for next comparison
    return;
}


















//  void App::RebuildDockLayoutOLD(void)
//  {
//
//      //  1.  CLEAR EXISTING DOCK LAYOUT...
//      ImGui::DockBuilderRemoveNode    (this->S.m_dockspace_id);
//      ImGui::DockBuilderAddNode       (this->S.m_dockspace_id,        ImGuiDockNodeFlags_DockSpace);
//      ImGui::DockBuilderSetNodeSize   (this->S.m_dockspace_id,        S.m_main_viewport->WorkSize);
//
//      //  2.  CREATE SPLIT-DOCK NODES...
//      ImGui::DockBuilderSplitNode     (this->S.m_dockspace_id,        ImGuiDir_Left,          this->S.m_browser_ratio,
//                                       &S.m_browser_dock_id,          &S.m_main_dock_id);
//
//
//      //  3.  INSERT THE "CORE" WINDOWS INTO DOCK...
//      ImGui::DockBuilderDockWindow    (S.m_windows[Window::Browser].uuid.c_str(),     S.m_browser_dock_id);
//
//
//      //  4.  INSERT ALL REMAINING WINDOWS INTO RIGHT-SIDE DOCK...
//      for (const std::string & win_name : this->S.m_primary_windows)
//      {
//          ImGui::DockBuilderDockWindow(win_name.c_str(), S.m_main_dock_id);
//      }
//
//      ImGui::DockBuilderFinish(this->S.m_dockspace_id);
//
//      return;
//  }












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

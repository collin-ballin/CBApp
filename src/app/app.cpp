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
# include "imgui_internal.h"



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
        if ( glfwGetWindowAttrib(this->S.m_glfw_window, GLFW_ICONIFIED) )       { ImGui_ImplGlfw_Sleep(10); continue; }
        

        //  2.  START THE "DEAR IMGUI" FRAME...
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        //
        //
        // *************************************************************************** //
        
        
        
        //  3.  INVOKE THE PRIMARY GUI-LOOP FOR  **MY OWN**  APPLICATION.     [ Draw each application window, etc. ]...
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
    static bool                     first_frame         = true;
    
    
    
    //  0.1.    RENDER THE DOCKSPACE...
    //      this->S.m_dockspace_id                              = ImGui::GetID( S.m_windows[Window::Dockspace].uuid.c_str() );


    //      //  0.2.    FIRST-FRAME INITIALIZATIONS...
    //      if (first_frame) [[unlikely]] {
    //          this->InitDockspace();
    //      }// END OF "first_frame"...


    //      //  1.      HANDLE ANY CALLS WHICH REQUIRED RE-DRAW OF DOCKING SPACE...
    //      if (S.m_rebuild_dockspace) [[unlikely]] {
    //          this->RebuildDockLayout();
    //          S.m_rebuild_dockspace = false;
    //      }


    //  1.      PERFORM PER-FRAME-CACHE FOR  **BEGINNING**  OF THIS FRAME...
    this->PerFrameCache_Begin(first_frame);



    //
    //
    //      PRIMARY APPLICATION LOOP...     [ DRAW EACH APP WINDOW, ETC ].
    // *************************************************************************** //


    //  2.      DRAW APPLICATION WINDOWS...
    //
    //          POLICY:     **DO NOT** check if the " winfo.render_fn" pointer is NULL...
    //                          - We assert that these must ALL be valid in the "init_asserts()".
    //                          - If one of these is NULL, something has gone very wrong and we  *WANT*  THE PROGRAM TO CRASH !!!
    for (idx = WINDOWS_BEGIN; idx < WINDOWS_END; ++idx)
    {
        app::WinInfo &  winfo   = S.m_windows[ static_cast<Window>(idx) ];
        if (winfo.open) {
            winfo.render_fn( winfo.uuid.c_str(), &winfo.open, winfo.flags );
        }
    }
    
    
    // *************************************************************************** //   END "PRIMARY LOOP".
    //
    //
    //
    
    
    //  3.      PERFORM ALL PER-FRAME CACHE UPDATE
    this->PerFrameCache_End(first_frame);
    
    
    //  1.      END OF FIRST-FRAME INITIALIZATIONS (SET THE INITIAL WINDOW FOCUS)...
    //  if (first_frame) [[unlikely]] {
    //      first_frame = false;
    //      ImGui::SetWindowFocus( S.current_task() );
    //  }
    

    //  3.      HANDLE ANY KEYBOARD SHORTCUTS...
    //  this->KeyboardShortcutHandler();
    
    
        
    //  4.      LASTLY, QUERY EACH SIGNAL HANDLER...
    //  this->QuerySignalStates();
    
    
    
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
        
        
//#define CBAPP_TEST_KEYHOLD
#ifdef CBAPP_TEST_KEYHOLD
        {
            static ui::KeyHoldManager khm;      // lives for the whole program

            // 1) Queue synthetic key events for this frame
            khm.Begin();                    // <-- must run before NewFrame()

            // 2) Build a tiny UI to drive it

            auto key_toggle_button = [&](ImGuiKey key, const char* label_held, const char* label_released)
            {
                const bool is_held = khm.IsActive(key);                 // convenience accessor
                const char* label  = is_held ? label_released : label_held;
                if (ImGui::Button(label))
                {
                    if (is_held)   khm.Pop(key);                        // schedule release
                    else           khm.Push(key);                       // start hold
                }
                ImGui::SameLine();
                ImGui::TextUnformatted(is_held ? "Held" : "Released");
            };

            key_toggle_button(ImGuiKey_O, "Hold O", "Release O");
            key_toggle_button(ImGuiKey_S, "Hold S", "Release S");

        }
#endif //  CBAPP_TEST_KEYHOLD  //
        
        
        
        
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
//      3.4A.   DOCKSPACE GUI STUFF...    | PRIVATE.
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






// *************************************************************************** //
//
//
//
//      3.4B.   GUI-SCALING STUFF...    | PRIVATE.
// *************************************************************************** //
// *************************************************************************** //

//  "InitUIScaler"
/*
void App::InitUIScaler(void)
{
    UIScaler::config            cfg;
    
    
    cfg.window                  = S.m_glfw_window;
    cfg.design_font_px          = cb::app::DEF_FONT_SIZE;    //     your “Main” base size
    cfg.design_scale_hint       = 0.0f;                      //     capture current monitor as baseline
    
    cfg.rebuild_fonts           = [this](float scale){ this->rebuild_fonts(scale); };   //  When ImGui <= 1.91/1.92-WIP, UIScaler will call this on DPI/zoom changes.


    S.m_ui_scaler               = UIScaler{cfg};
    S.m_ui_scaler.init_runtime();                       // applies style scale + calls rebuild_fonts(scale)
    
    return;
}
   */

//  "OnDpiScaleChanged"
//
void App::OnDpiScaleChanged([[maybe_unused]] float xs, [[maybe_unused]] float ys)
{
    const float     new_scale   = std::max(xs, ys);
    
    if ( std::fabs(new_scale - S.m_dpi_scale) < 0.01f )     { return; }

    S.m_ui_scaler.on_scale_changed(xs, ys);                 //  central handler
    S.m_dpi_scale               = new_scale;                //  keep for logging/telemetry if desired
    return;
}
/*
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
*/


















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

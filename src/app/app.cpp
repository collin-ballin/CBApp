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
#include "_config.h"

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
    static const ImGuiIO &      io          = ImGui::GetIO(); (void)io;


    //  1.  MAIN PROGRAM LOOP...
#ifdef __EMSCRIPTEN__
    io.IniFilename = nullptr;   //  For an Emscripten build, we are disabling file-system access.  So let's not attempt an
    EMSCRIPTEN_MAINLOOP_BEGIN   //  "fopen()" of the "imgui.ini" file.  You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
#else
    while ( !glfwWindowShouldClose(this->m_glfw_window) && this->m_running )
#endif  //  __EMSCRIPTEN__  //
    {
        //  1.1     SET A POLL AND HANDLE EVENTS (inputs, window resize, etc.)...
        //          You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        //              - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        //              - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        //          Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        if (glfwGetWindowAttrib(this->m_glfw_window, GLFW_ICONIFIED))    { ImGui_ImplGlfw_Sleep(10); continue; }
        

        //  2.  START THE "DEAR IMGUI" FRAME...
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        //  3.  DRAW MAIN "UI" ELEMENTS...
        this->run_IMPL();


        //  4.  RENDERING...
        ImGui::Render();
        glfwGetFramebufferSize(this->m_glfw_window,  &this->m_window_w,  &this->m_window_h); // int display_w, display_h;     // glfwGetFramebufferSize(this->m_glfw_window, &display_w, &display_h);
        glViewport(0, 0, this->m_window_w, this->m_window_w);
        glClearColor(this->m_glfw_bg.x * this->m_glfw_bg.w, this->m_glfw_bg.y * this->m_glfw_bg.w, this->m_glfw_bg.z * this->m_glfw_bg.w, this->m_glfw_bg.w);
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
        glfwSwapBuffers(this->m_glfw_window);
        
        
    }// END OF MAIN-LOOP...


    #ifdef __EMSCRIPTEN__
        EMSCRIPTEN_MAINLOOP_END;
    #endif  //  __EMSCRIPTEN__  //


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
    static ImGuiIO &                io                  = ImGui::GetIO(); (void)io;
    static ImGuiStyle &             style               = ImGui::GetStyle();
    static size_t                   idx                 = static_cast<size_t>(0);
    static const size_t &           WINDOWS_BEGIN       = this->m_state.ms_WINDOWS_BEGIN;   // static_cast<size_t>(Window::Sidebar);
    static const size_t &           WINDOWS_END         = this->m_state.ms_WINDOWS_END;     // static_cast<size_t>(Window::Count);

    static app::WinInfo &           winfo               = m_state.m_windows[static_cast<Window>(idx)];
    static bool *                   p_open              = nullptr;      // &window.open;
    static bool                     first_frame         = true;
    
    
    
    //  0.1     RENDER THE DOCKSPACE...
#ifndef CBAPP_NEW_DOCKSPACE
    ImGui::PushStyleColor(ImGuiCol_WindowBg, this->m_dock_bg);
    //ImGui::DockSpaceOverViewport(0, this->m_main_viewport);
    ImGui::DockSpaceOverViewport(this->m_main_viewport, ImGuiDockNodeFlags_NoTabBar);
    ImGui::PopStyleColor();
#else
    const ImGuiViewport *           viewport            = ImGui::GetMainViewport();
    this->m_dockspace_id                                = ImGui::GetID(this->m_dock_name);


    //  1.      FIRST-FRAME INITIALIZATIONS...
    if (first_frame)
    {
        first_frame = false;
        ImGui::DockBuilderRemoveNode(this->m_dockspace_id );                                // Clear any existing layout
        ImGui::DockBuilderAddNode(this->m_dockspace_id , ImGuiDockNodeFlags_DockSpace | ImGuiDockNodeFlags_NoTabBar);
    
        ImGui::DockBuilderSetNodeSize(this->m_dockspace_id , viewport->WorkSize);
        ImGuiID dock_id_left, dock_id_main;
        ImGui::DockBuilderSplitNode(this->m_dockspace_id , ImGuiDir_Left, 0.2f, &dock_id_left, &dock_id_main);

        ImGui::DockBuilderDockWindow(app::DEF_SIDEBAR_WIN_TITLE, dock_id_left);             //  Make sure names match actual windows
        ImGui::DockBuilderDockWindow(app::DEF_MAIN_WIN_TITLE, dock_id_main);
        ImGui::DockBuilderFinish(this->m_dockspace_id );
        
        
        //      1.1     Set
    #ifdef __CBAPP_COINCIDENCE_COUNTER_BUILD__
        ImGui::SetWindowFocus( this->m_state.m_windows[Window::MainApp].uuid.c_str() );
    # else
        ImGui::SetWindowFocus( this->m_state.m_windows[Window::GraphingApp].uuid.c_str() );
    #endif  //  __CBAPP_COINCIDENCE_COUNTER_BUILD__  //
    }// END OF "first_frame"...


    ImGui::SetNextWindowPos(viewport->WorkPos);                         //  Set up invisible host window covering the entire viewport
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,       0.0f);      //  Make host window invisible with no padding, rounding, borders, etc.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,     0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,        ImVec2(0.0f, 0.0f));

    //  Host window flags (invisible, non-interactive)
    ImGuiWindowFlags host_window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

    ImGui::Begin( "##DockHostWindow", nullptr, host_window_flags); //this->m_state.m_windows[Window::Host].flags );
        ImGui::PopStyleVar(3);  //  DockSpace() creates docking area within this host window
        ImGui::DockSpace(this->m_dockspace_id );
    ImGui::End();
#endif  //  CBAPP_NEW_DOCKSPACE  //
    
    
    //          1.2     Certain Calls need to RE-BUILD the DOCKING SPACE.
    if (this->m_rebuild_dockspace) {
        this->RebuildDockLayout();
        this->m_rebuild_dockspace = false;
    }



    //  2.      DRAW APPLICATION WINDOWS...
    //
    //          POLICY:     **DO NOT** check if the " winfo.render_fn" pointer is NULL...
    //                          - We assert that these must ALL be valid in the "init_asserts()".
    //                          - If one of these is NULL, something has gone very wrong and we  *WANT*  THE PROGRAM TO CRASH !!!
    for (idx = WINDOWS_BEGIN; idx < WINDOWS_END; ++idx)
    {
        winfo           = m_state.m_windows[ static_cast<Window>(idx) ];
        
        if (winfo.open) {
            winfo.render_fn( winfo.uuid.c_str(), &winfo.open, winfo.flags );
        }
    }
    
    
    
    return;
}






// *************************************************************************** //
//
//
//  3.4     ADDITIONAL APP FUNCTIONS...    | PRIVATE.
// *************************************************************************** //
// *************************************************************************** //

//  "RebuildDockLayout"
//
void App::RebuildDockLayout(void)
{
    const ImGuiViewport *   viewport    = ImGui::GetMainViewport();

    ImGui::DockBuilderRemoveNode(this->m_dockspace_id);
    ImGui::DockBuilderAddNode(this->m_dockspace_id, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(this->m_dockspace_id, viewport->WorkSize);

    ImGuiID dock_id_left, dock_id_main;
    ImGui::DockBuilderSplitNode(this->m_dockspace_id, ImGuiDir_Left, this->m_sidebar_ratio,
                                &m_state.m_sidebar_dock_id,     &m_state.m_main_dock_id);



    // Only dock sidebar window once to the left
    ImGui::DockBuilderDockWindow(app::DEF_SIDEBAR_WIN_TITLE, m_state.m_sidebar_dock_id);

    // Dock all *other* persistent windows to the main area (skip sidebar)
    for (const std::string & win_name : this->m_primary_windows)
        ImGui::DockBuilderDockWindow(win_name.c_str(), m_state.m_main_dock_id);

    ImGui::DockBuilderFinish(this->m_dockspace_id);
}


//  "PushFont"
//
void App::PushFont( [[maybe_unused]] const Font & which) {
    ImGui::PushFont( this->m_state.m_fonts[which] );
    return;
}


//  "PopFont"
//
void App::PopFont(void) {
    ImGui::PopFont();
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

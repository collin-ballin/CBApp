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
    static ImGuiIO &            io          = ImGui::GetIO(); (void)io;
    ImGuiContext *              g           = ImGui::GetCurrentContext();
    


    //  1.  MAIN PROGRAM LOOP...
#ifdef __EMSCRIPTEN__
    io.IniFilename = nullptr;   //  For an Emscripten build, we are disabling file-system access.  So let's not attempt an
    EMSCRIPTEN_MAINLOOP_BEGIN   //  "fopen()" of the "imgui.ini" file.  You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
#else
    while ( !glfwWindowShouldClose(this->S.m_glfw_window) && this->S.m_running )
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


        //  3.  DRAW MAIN "UI" ELEMENTS...
        this->run_IMPL();


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
    static bool *                   p_open              = nullptr;      // &window.open;
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
    if (first_frame) [[unlikely]]
    {
        //  1.  CLEAR EXISTING DOCK LAYOUT...
        ImGui::DockBuilderRemoveNode    (this->S.m_dockspace_id);
        ImGui::DockBuilderAddNode       (this->S.m_dockspace_id,        ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize   (this->S.m_dockspace_id,        S.m_main_viewport->WorkSize);

        //  2.  CREATE SPLIT-DOCK NODES...
        ImGui::DockBuilderSplitNode     (this->S.m_dockspace_id,        ImGuiDir_Left,          this->S.m_sidebar_ratio,
                                         &S.m_sidebar_dock_id,          &S.m_main_dock_id);

        //  3.  GET DOCKING-NODES AND SET NODE-FLAGS...
        S.m_main_node                                   = ImGui::DockBuilderGetNode(S.m_main_dock_id);
        S.m_main_node->LocalFlags                      |= S.m_main_node_flags;
        S.m_sidebar_node                                = ImGui::DockBuilderGetNode(S.m_sidebar_dock_id);
        S.m_sidebar_node->LocalFlags                   |= S.m_sidebar_node_flags;
      

        //  4.  INSERT THE "CORE" WINDOWS INTO DOCK...
        ImGui::DockBuilderDockWindow    (S.m_windows[Window::SideBar].uuid.c_str(),             S.m_sidebar_dock_id);


        //  5.  DOCK ALL RIGHT-HAND WINDOWS INTO RIGHT-HAND DOCKSPACE BY DEFAULT...
        for (idx = S.ms_RHS_WINDOWS_BEGIN; idx < WINDOWS_END; ++idx) {
            winfo           = S.m_windows[ static_cast<Window>(idx) ];
            if (winfo.open) {
                ImGui::DockBuilderDockWindow( winfo.uuid.c_str(), S.m_main_dock_id );
            }
        }
        ImGui::DockBuilderFinish(this->S.m_dockspace_id);
        
    }// END OF "first_frame"...
#endif  //  CBAPP_NEW_DOCKSPACE  //
    


    //          1.2     Certain Calls need to RE-BUILD the DOCKING SPACE.
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
    #if defined(__CBAPP_BUILD_CCOUNTER_APP__)
        ImGui::SetWindowFocus(this->S.m_windows[Window::CCounterApp].uuid.c_str());
    # elif defined(__CBAPP_BUILD_FDTD_APP__)
        ImGui::SetWindowFocus(this->S.m_windows[Window::GraphApp].uuid.c_str());
    # else
        ImGui::SetWindowFocus(this->S.m_windows[Window::MainApp].uuid.c_str());
    #endif  //  __CBAPP_BUILD_CCOUNTER_APP__  //
    }
    
    return;
}









// *************************************************************************** //
//
//
//
//  3.2     MAIN APPLICATION GUI FUNCTIONS...    | PRIVATE.
// *************************************************************************** //
// *************************************************************************** //

//  "ShowMainWindow"
//
void App::ShowMainWindow([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
{
    // ImVec2                       win_pos(this->S.m_main_viewport->WorkPos.x + 750,   this->S.m_main_viewport->WorkPos.x + 20);
    ImGuiIO &                       io              = ImGui::GetIO(); (void)io;
    
    
    
    //  1.  CREATE THE WINDOW AND BEGIN APPENDING WIDGETS INTO IT...
    ImGui::PushStyleColor(ImGuiCol_WindowBg, this->S.m_main_bg);
    ImGui::Begin(uuid, p_open, flags);
    
        ImGui::PopStyleColor();
    
    
#if defined(CBAPP_ENABLE_CB_DEMO) && !defined(__CBAPP_BUILD_CCOUNTER_APP__) && !defined(__CBAPP_BUILD_FDTD_APP__)


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

    //  3.  TESTING PLOTTING / GRAPHING 2...
    {
        ImGui::SetNextItemOpen(false, ImGuiCond_Once);
        if (ImGui::TreeNode("Graphing 2"))
        {
            this->ImPlot_Testing2();
            ImGui::TreePop();
        }
    }
        
     
    //  4.  TESTING TAB BAR...
    {
        ImGui::NewLine();
        KeepProgramAwake( this->S.m_glfw_window );
        ImGui::NewLine();
        this->TestTabBar();
    }


    //  3.  TESTING PLOTTING / GRAPHING 1...
    {
        ImGui::SetNextItemOpen(false, ImGuiCond_Once);
        if (ImGui::TreeNode("Graphing 1"))
        {
            this->ImPlot_Testing1();
            ImGui::TreePop();
        }
    }


    //  4.  TESTING PLOTTING / GRAPHING 2...
    {
        ImGui::SetNextItemOpen(false, ImGuiCond_Once);
        if (ImGui::TreeNode("Graphing 2"))
        {
            this->ImPlot_Testing2();
            ImGui::TreePop();
        }
    }
    
    
    //  5.  TESTING PLOTTING / GRAPHING 2...
    {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Graphing 3"))
        {
            this->ImPlot_Testing3();
            ImGui::TreePop();
        }
    }
    
    
    
    //  10. TESTING RADIOBUTTONS...
    {
        ImGui::SetNextItemOpen(false, ImGuiCond_Once);
        if (ImGui::TreeNode("Basic Widgets"))
        {
            this->Test_Basic_Widgets();
            ImGui::TreePop();
        }
    }
    
    
    
    //  11. TESTING RADIOBUTTONS...
    {
        ImGui::SetNextItemOpen(false, ImGuiCond_Once);
        if (ImGui::TreeNode("ImGui Demo Code"))
        {
            this->ImGui_Demo_Code();
            ImGui::TreePop();
        }
    }
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
//  3.4     ADDITIONAL APP FUNCTIONS...    | PRIVATE.
// *************************************************************************** //
// *************************************************************************** //

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
    //  ImGui::DockBuilderSplitNode     (this->S.m_dockspace_id,        ImGuiDir_Left,          this->S.m_sidebar_ratio,
    //                                   &S.m_sidebar_dock_id,          &S.m_main_dock_id);


    //  3.  INSERT THE "CORE" WINDOWS INTO DOCK...
    ImGui::DockBuilderDockWindow    (S.m_windows[Window::SideBar].uuid.c_str(),     S.m_sidebar_dock_id);
    ImGui::DockBuilderDockWindow    (S.m_windows[Window::MainApp].uuid.c_str(),     S.m_main_dock_id);


    //  4.  INSERT ALL REMAINING WINDOWS INTO RIGHT-SIDE DOCK...
    for (size_t idx = S.ms_APP_WINDOWS_BEGIN; idx < S.ms_WINDOWS_END; ++idx) {
        winfo           = S.m_windows[ static_cast<Window>(idx) ];
        if (winfo.open) {
            ImGui::DockBuilderDockWindow( winfo.uuid.c_str(), S.m_main_dock_id );
        }
    }

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
//      ImGui::DockBuilderSplitNode     (this->S.m_dockspace_id,        ImGuiDir_Left,          this->S.m_sidebar_ratio,
//                                       &S.m_sidebar_dock_id,          &S.m_main_dock_id);
//
//
//      //  3.  INSERT THE "CORE" WINDOWS INTO DOCK...
//      ImGui::DockBuilderDockWindow    (S.m_windows[Window::SideBar].uuid.c_str(),     S.m_sidebar_dock_id);
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

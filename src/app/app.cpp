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
    static ImGuiIO &                io                  = ImGui::GetIO(); (void)io;
    static ImGuiStyle &             style               = ImGui::GetStyle();
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
    if (first_frame)
    {
        first_frame = false;
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

        //  5.  INSERT ALL REMAINING WINDOWS INTO RIGHT-SIDE DOCK...
        for (const std::string & win_name : this->S.m_primary_windows)
        {
            ImGui::DockBuilderDockWindow(win_name.c_str(), S.m_main_dock_id);
        }
        ImGui::DockBuilderFinish(this->S.m_dockspace_id);

        //  6.  SET FOCUS TO MAIN-WINDOW...
        ImGui::SetWindowFocus(this->S.m_windows[Window::MainApp].uuid.c_str());
        
    }// END OF "first_frame"...
#endif  //  CBAPP_NEW_DOCKSPACE  //
    


    //          1.2     Certain Calls need to RE-BUILD the DOCKING SPACE.
    if (S.m_rebuild_dockspace) {
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
    
    
#ifdef CBAPP_ETCH_A_SKETCH
    
    //  1.  CREATE THE WINDOW AND BEGIN APPENDING WIDGETS INTO IT...
    ImGui::PushStyleColor(ImGuiCol_WindowBg, this->S.m_main_bg);
    ImGui::Begin(uuid, p_open, flags);
    
        ImGui::PopStyleColor();
        
        
        
        //  2.  COLOR TOOL...
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if( ImGui::CollapsingHeader("Color Tool") )
        {
            this->color_tool();
            //ImGui::TreePop();
        }
        
        
        KeepProgramAwake( this->S.m_glfw_window );
        
        
        
//
//
# else  //  CBAPP_ETCH_A_SKETCH  //
//
//
    KeepProgramAwake( this->S.m_glfw_window );


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
#endif  //  CBAPP_ETCH_A_SKETCH  //
    
    
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
    //  1.  CLEAR EXISTING DOCK LAYOUT...
    ImGui::DockBuilderRemoveNode    (this->S.m_dockspace_id);
    ImGui::DockBuilderAddNode       (this->S.m_dockspace_id,        ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize   (this->S.m_dockspace_id,        S.m_main_viewport->WorkSize);

    //  2.  CREATE SPLIT-DOCK NODES...
    ImGui::DockBuilderSplitNode     (this->S.m_dockspace_id,        ImGuiDir_Left,          this->S.m_sidebar_ratio,
                                     &S.m_sidebar_dock_id,          &S.m_main_dock_id);


    //  3.  INSERT THE "CORE" WINDOWS INTO DOCK...
    ImGui::DockBuilderDockWindow    (S.m_windows[Window::SideBar].uuid.c_str(),
                                     S.m_sidebar_dock_id);

    //  4.  INSERT ALL REMAINING WINDOWS INTO RIGHT-SIDE DOCK...
    for (const std::string & win_name : this->S.m_primary_windows)
    {
        ImGui::DockBuilderDockWindow(win_name.c_str(), S.m_main_dock_id);
    }

    ImGui::DockBuilderFinish(this->S.m_dockspace_id);
    return;
}












// *************************************************************************** //
//
//
//  3.5     OTHER...            | PRIVATE.
// *************************************************************************** //
// *************************************************************************** //

//  "stream_test"
//
void App::stream_test()
{
    ImGui::Begin("Python stream test");

    // ---------------------------------------------------------------
    // 1)  Persistent state
    // ---------------------------------------------------------------
    // static utl::PyStream proc(app::PYTHON_COUNTER_FILEPATH);
    static utl::PyStream            proc(app::PYTHON_DUMMY_FPGA_FILEPATH);
    static bool                     started   = false;
    static char                     line_buf[256]{};
    static std::vector<std::string> log;            // console lines
    static float                    delay_s   = 1.0f; // seconds between packets

    // ---------------------------------------------------------------
    // 2)  Runtime‑adjustable delay
    // ---------------------------------------------------------------
    if (ImGui::SliderFloat("Delay (s)", &delay_s, 0.05f, 5.0f, "%.2f"))
    {
        // Send immediately while dragging *if* running
        if (started)
        {
            char cmd[48];
            std::snprintf(cmd, sizeof(cmd), "duration %.3f", delay_s);
            proc.send(cmd);                          // e.g. "duration 0.25\n"
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Apply") && started)           // extra button for mouse‑up commit
    {
        char cmd[48];
        std::snprintf(cmd, sizeof(cmd), "duration %.3f", delay_s);
        proc.send(cmd);
    }

    // ---------------------------------------------------------------
    // 3)  Start / Stop controls
    // ---------------------------------------------------------------
    if (!started)
    {
        if (ImGui::Button("Start process"))
        {
            started = proc.start();
            if (!started) ImGui::OpenPopup("launch_error");
        }
    }
    else
    {
        if (ImGui::Button("Stop process"))
        {
            proc.stop();
            started = false;
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(running)");
    }

    if (ImGui::BeginPopupModal("launch_error", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Failed to launch Python process!");
        if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    ImGui::Separator();

    // ---------------------------------------------------------------
    // 4)  Manual line send (for debugging)
    // ---------------------------------------------------------------
    ImGui::InputText("send", line_buf, IM_ARRAYSIZE(line_buf),
                     ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::SameLine();
    if (ImGui::Button("Send") || ImGui::IsItemDeactivatedAfterEdit())
    {
        if (started && line_buf[0])
        {
            proc.send(line_buf);          // raw passthrough
            line_buf[0] = '\0';
        }
    }

    // ---------------------------------------------------------------
    // 5)  Poll stdout → decode & log
    // ---------------------------------------------------------------
    std::string raw;
    while (proc.try_receive(raw))
    {
        log.push_back("RAW: " + raw);

        if (auto pkt = utl::parse_packet(raw))
        {
            const auto& p = *pkt;
            char buf[256];
            std::snprintf(buf, sizeof(buf),
                          "DECODED  A=%d  B=%d  C=%d  D=%d  ABCD=%d  cycles=%d",
                          p.a(), p.b(), p.c(), p.d(), p.abcd(), p.cycles);
            log.emplace_back(buf);
        }
        else
        {
            log.emplace_back("!! parse error");
        }
    }

    // ---------------------------------------------------------------
    // 6)  Display log
    // ---------------------------------------------------------------
    ImGui::BeginChild("log", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
    for (auto const& s : log)
        ImGui::TextUnformatted(s.c_str());
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);       // auto‑scroll
    ImGui::EndChild();

    ImGui::End();
}



//  "color_tool"
//
void App::color_tool()
{
    ImGuiIO &                       io                      = ImGui::GetIO(); (void)io;
    ImGuiStyle &                    style                   = ImGui::GetStyle();

    static ImVec4                   base_color              = ImVec4(114.0f/255.0f, 144.0f/255.0f, 154.0f/255.0f, 1.0f);    //  Base color in RGB (normalized to [0,1]).
    static char                     hex_input[8+1]          = "#728C9A";    //  Hex input for base color.
    static int                      steps                   = 3;            //  Steps and lightness delta for variants.
    static float                    delta_l                 = 0.1f;

    //  Table layout constants
    static constexpr float          LABEL_COLUMN_WIDTH      = 150.0f;
    static constexpr float          WIDGET_COLUMN_WIDTH     = 300.0f;
    static bool                     freeze_header           = false;
    static bool                     freeze_column           = false;
    static bool                     stretch_column_1        = true;

    static ImGuiTableColumnFlags    col0_flags              = ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize;
    static ImGuiTableColumnFlags    col1_flags              = stretch_column_1 ? ImGuiTableColumnFlags_WidthStretch : ImGuiTableColumnFlags_WidthFixed;
    static ImGuiTableFlags          table_flags             = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoKeepColumnsVisible; //ImGuiTableFlags_ScrollX;



    //  1.  TABLE OF WIDGETS...
    if (ImGui::BeginTable("ColorToolControls", 2, table_flags))
    {
        if (freeze_column || freeze_header)
            ImGui::TableSetupScrollFreeze(freeze_column ? 1 : 0, freeze_header ? 1 : 0);

        ImGui::TableSetupColumn("Label",  col0_flags, LABEL_COLUMN_WIDTH);
        ImGui::TableSetupColumn("Widget", col1_flags, stretch_column_1 ? 1.0f : WIDGET_COLUMN_WIDTH);
        ImGui::TableHeadersRow();

        // Hex Color row
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Hex Color");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-1);
        ImGui::InputText("##hex_input", hex_input, IM_ARRAYSIZE(hex_input));
        ImGui::SameLine();
        if (ImGui::Button("Apply Hex")) {
            std::string hex = hex_input;
            if (!hex.empty() && hex.front() == '#') hex.erase(0, 1);
            unsigned int rgb = 0;
            std::stringstream ss;
            ss << std::hex << hex;
            ss >> rgb;
            base_color.x = ((rgb >> 16) & 0xFF) / 255.0f;
            base_color.y = ((rgb >>  8) & 0xFF) / 255.0f;
            base_color.z = ( rgb        & 0xFF) / 255.0f;
        }
        ImGui::PopItemWidth();

        // Base Color picker row
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Base Color");
        ImGui::TableSetColumnIndex(1);
        ImGui::ColorEdit4("##base_color", (float*)&base_color, ImGuiColorEditFlags_DisplayHSV);

        // Steps row
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Steps");
        ImGui::TableSetColumnIndex(1);
        ImGui::SliderInt("##steps", &steps, 2, 10);

        // Δ Lightness row
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Δ Lightness");
        ImGui::TableSetColumnIndex(1);
        ImGui::SliderFloat("##delta_l", &delta_l, 0.01f, 0.5f, "%.2f");

        ImGui::EndTable();
    }

    // Generated variants visualization
    ImGui::Text("Generated Variants:");
    ImGui::BeginColumns("variants", steps, false);
    for (int i = 0; i < steps; i++) {
        float h, s, l;
        ImGui::ColorConvertRGBtoHSV(base_color.x, base_color.y, base_color.z, h, s, l);
        float offset = (i - (steps - 1) * 0.5f) * delta_l;
        float new_l = ImClamp(l + offset, 0.0f, 1.0f);
        float r, g, b;
        ImGui::ColorConvertHSVtoRGB(h, s, new_l, r, g, b);
        ImVec4 variant(r, g, b, base_color.w);

        ImGui::Text("Step %d", i);
        ImGui::ColorButton(("##variant" + std::to_string(i)).c_str(), variant,
                           ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs,
                           ImVec2(50, 50));
        ImGui::NextColumn();
    }
    ImGui::EndColumns();
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

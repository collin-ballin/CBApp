//
//  app.cpp
//  CBApp
//
//  Created by Collin Bond on 4/16/25.
//
//***************************************************************************//
//***************************************************************************//
#include "app/app.h"
//#include "utility/utility.h"
#include <random>
#include <algorithm>



namespace cb { //     BEGINNING NAMESPACE "cb"...
//***************************************************************************//
//***************************************************************************//



//***************************************************************************//
//
//
//  1.      INITIALIZATION  | DEFAULT CONSTRUCTOR, DESTRUCTOR, ETC...
//***************************************************************************//
//***************************************************************************//

//  Default Constructor.
//
App::App(void)
{
    glfwSetErrorCallback(utl::glfw_error_callback);     //  1.  SET GLFW CALLBACK & CHECK IF PROPERLY INITIALIZED...
    if (!glfwInit())    throw std::runtime_error("Call to glfwInit() returned NULL");

    this->m_glsl_version = utl::get_glsl_version();     //  2.  DECIDE WHICH GL + GLSL VERSION...
    this->init();                                       //  3.  INVOKE "HELPER" FUNCTION TO COMPLETE INITIALIZATION...
    
    
    return;
}


//  "init"          | private
//
void App::init(void)
{
    //  1.  CREATE A WINDOW WITH GRAPHICS CONTEXT...
    GLFWmonitor *           monitor     = glfwGetPrimaryMonitor();
    const GLFWvidmode *     mode        = glfwGetVideoMode( monitor );
    
    
    this->m_window = glfwCreateWindow(cb::utl::DEF_ROOT_WIN_WIDTH, cb::utl::DEF_ROOT_WIN_HEIGHT, cb::utl::DEF_ROOT_WIN_TITLE, nullptr, nullptr);
    if (!this->m_window)
        throw std::runtime_error("Call to glfwInit() returned NULL");
    utl::set_window_scale(this->m_window, cb::utl::DEF_ROOT_WINDOW_SCALE);
    
    glfwMakeContextCurrent(this->m_window);
    glfwSwapInterval(1);        // Enable vsync




    //  2.  SETUP "Dear ImGui" CONTEXT...
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO &       io          = ImGui::GetIO(); (void)io;
    
    
#if defined(CBAPP_DISABLE_INI)
    io.IniFilename            = cb::utl::INI_FILEPATH;
    ImGui::LoadIniSettingsFromDisk(cb::utl::INI_FILEPATH);
# else
    # if defined (CBAPP_USE_SWAP_INI)
    io.IniFilename            = cb::utl::INI_FILEPATH;
    ImGui::LoadIniSettingsFromDisk(cb::utl::INI_FILEPATH);
    # else
    io.IniFilename            = cb::utl::SWAP_INI_FILEPATH;
    ImGui::LoadIniSettingsFromDisk(cb::utl::SWAP_INI_FILEPATH);
    # endif     //  CBAPP_USE_SWAP_INI  //
#endif  //  CBAPP_DISABLE_INI  //
    
    
    io.ConfigFlags             |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad |      //  2.2 Configure I/O Settings.
                                  ImGuiConfigFlags_DockingEnable     | ImGuiConfigFlags_ViewportsEnable;
    //io.ConfigViewportsNoAutoMerge     = true;
    //io.ConfigViewportsNoTaskBarIcon   = true;
    
    
    //      3.1     Setup "Dear ImGui" STYLE.
    ImGui::StyleColorsDark();
    ImGuiStyle & style          = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)      //  When viewports are enabled we tweak WindowRounding/WindowBg
    {                                                           //  so platform windows can look identical to regular ones.
        style.WindowRounding                = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w   = 1.0f;
    }
    
    
    //      3.2     Setup Platform/Renderer backends.
    ImGui_ImplGlfw_InitForOpenGL(this->m_window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif      //  __EMSCRIPTEN__  //
    ImGui_ImplOpenGL3_Init(this->m_glsl_version);
    //
    //
    //      3.3     Load Fonts.
    this->m_main_font                       = io.Fonts->AddFontFromFileTTF(cb::utl::DEF_FONT_PATH, cb::utl::DEF_FONT_SIZE);
    IM_ASSERT(this->m_main_font != nullptr);
    
    
    this->m_main_viewport = ImGui::GetMainViewport();
    
    return;
}



//  Destructor.
//
App::~App(void)
{
    ImGui::SaveIniSettingsToDisk(cb::utl::SWAP_INI_FILEPATH);
    this->destroy();
}


//  "destroy"       | private
//
void App::destroy(void) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    if (this->m_window) {
        glfwDestroyWindow(this->m_window);
        this->m_window = nullptr;
    }
    glfwTerminate();
    return;
}




//***************************************************************************//
//
//
//  1B.     PUBLIC MEMBER FUNCTIONS...
//***************************************************************************//
//***************************************************************************//

//  "run"
//
void App::run(void)
{
    //  1.  MAIN PROGRAM LOOP...
    ImGuiIO &       io          = ImGui::GetIO(); (void)io;


#ifdef __EMSCRIPTEN__
    io.IniFilename = nullptr;   //  For an Emscripten build, we are disabling file-system access.  So let's not attempt an
    EMSCRIPTEN_MAINLOOP_BEGIN   //  "fopen()" of the "imgui.ini" file.  You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!glfwWindowShouldClose(this->m_window))
#endif  //  __EMSCRIPTEN__  //
    {
        //  1.1     SET A POLL AND HANDLE EVENTS (inputs, window resize, etc.)...
        //          You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        //              - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        //              - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        //          Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        if (glfwGetWindowAttrib(this->m_window, GLFW_ICONIFIED))
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }


        //  2.  START THE "DEAR IMGUI" FRAME...
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();



        //  3.  DRAW MAIN "UI" ELEMENTS...
        this->draw_ui();



        //  4.  RENDERING...
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(this->m_window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(this->m_clear_color.x * this->m_clear_color.w, this->m_clear_color.y * this->m_clear_color.w, this->m_clear_color.z * this->m_clear_color.w, this->m_clear_color.w);
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
        glfwSwapBuffers(this->m_window);
        
        
    }   //  END OF MAIN-LOOP...

    #ifdef __EMSCRIPTEN__
        EMSCRIPTEN_MAINLOOP_END;
    #endif  //  __EMSCRIPTEN__  //


    return;
}






//***************************************************************************//
//
//
//  2.      PROTECTED MEMBER FUNCTIONS...
//***************************************************************************//
//***************************************************************************//



//***************************************************************************//
//
//
//  3.      PRIVATE MEMBER FUNCTIONS...
//***************************************************************************//
//***************************************************************************//

//  "draw_ui"
//
void App::draw_ui(void)
{
    ImGuiIO &       io          = ImGui::GetIO(); (void)io;
    
    //  1.  DRAWING THE "SIDE-BAR MENU" WINDOW...
    this->Display_Sidebar_Window(&this->m_show_sidebar_window);

    //  2.  DISPLAY THE MAIN APPLICATION WINDOW...
    if (this->m_show_main_window)
        this->Display_Main_Window(&this->m_show_main_window);

    //  3.  DISPLAY THE "Dear ImGui" DEMO WINDOW...
    if (this->m_show_imgui_demo)
        ImGui::ShowDemoWindow(&this->m_show_imgui_demo);
        
    //  4.  DISPLAY THE ImPlot WINDOW...
    if (this->m_show_implot_demo)
        ImPlot::ShowDemoWindow(&this->m_show_implot_demo);
    
    return;
}




//***************************************************************************//
//
//
//  3.1     PRIVATE MEMBER FUNCTIONS...
//***************************************************************************//
//***************************************************************************//


//  "Display_Sidebar_Window"
//
void App::Display_Sidebar_Window(bool * p_open)
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

            // Enforce bounds explicitly in case user types in a value
            scale = std::clamp(scale, 0.01f, 0.99f);

            if (ImGui::Button("Apply Scale"))
            {
                utl::set_window_scale(this->m_window, scale);
            /*
                GLFWmonitor *           monitor         = cb::utl::get_current_monitor(this->m_window);
                    
                const GLFWvidmode *     mode            = glfwGetVideoMode(monitor);
                int                     screen_w        = mode->width,    screen_h = mode->height;
                const float             aspect          = static_cast<float>(screen_w) / screen_h;

                int                     new_w           = static_cast<int>(screen_w * scale);
                int                     new_h           = static_cast<int>(new_w / aspect);

                if (new_h > screen_h) {
                    new_h = static_cast<int>(screen_h * scale);
                    new_w = static_cast<int>(new_h * aspect);
                }

                int monitor_x, monitor_y;
                glfwGetMonitorPos(monitor, &monitor_x, &monitor_y);
                glfwSetWindowSize(this->m_window, new_w, new_h);
                glfwSetWindowPos(this->m_window,
                                 monitor_x + (screen_w - new_w) / 2,
                                 monitor_y + (screen_h - new_h) / 2);
            */
            }
            
        }
        
        
        
        
        //  2.3     BOTTOM OF WINDOW | INI-Button, Frame-Rate, etc...
        ImGui::Dummy( ImVec2(0.0f, ImGui::GetContentRegionAvail().y - 2.5 * ImGui::GetTextLineHeight()) );
        
        if (ImGui::Button("SAVE .ini FILE"))
            ImGui::SaveIniSettingsToDisk(cb::utl::INI_FILEPATH);
                
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    
    
    ImGui::End();
    
    return;
}



//***************************************************************************//
//
//
//  3.2     APPLICATION GUI FUNCTIONS...    | PRIVATE.
//***************************************************************************//
//***************************************************************************//

//  "Display_Main_Window"
//
void App::Display_Main_Window(bool * p_open)
{
    static constexpr const char *   win_title       = "My Application";     //  General Constants for the window.
    //ImVec2                          win_pos(1000,   100);
    ImVec2                          win_pos(this->m_main_viewport->WorkPos.x + 750,   this->m_main_viewport->WorkPos.x + 20);
    ImVec2                          win_dims(400,   800);
    
    
    
    
    /*

    ImGuiWindowFlags window_flags = 0;
    if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
    if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
    if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
    if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
    if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
    if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
    if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
    if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
    if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (no_docking)         window_flags |= ImGuiWindowFlags_NoDocking;
    if (unsaved_document)   window_flags |= ImGuiWindowFlags_UnsavedDocument;
    if (no_close)           p_open = NULL; // Don't pass our bool* to Begin
    */
    
    p_open               = nullptr;
    this->m_win_flags   |=  ImGuiWindowFlags_MenuBar + ImGuiWindowFlags_NoCollapse;
    
    
    
    
    
    //static float                    f               = 0.0f;                 //  Widget Variables.
    //static int                      counter         = 0;
    ImGuiIO &                       io              = ImGui::GetIO(); (void)io;
    
    

    //  1.  ADD GENERAL WINDOW PARAMETERS ( CALL THESE **BEFORE** "BEGIN( ... )" )...
    ImGui::SetNextWindowPos(win_pos,    ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(win_dims,  ImGuiCond_FirstUseEver);
    
    //  2.  CREATE THE WINDOW AND BEGIN APPENDING WIDGETS INTO IT...
    ImGui::Begin(win_title, p_open, this->m_win_flags);



    //  3.  ADD WIDGETS UNDER "COLLIN" TAB...
    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if (ImGui::TreeNode("Collin"))
    {
        static int e = 0;
        ImGui::RadioButton("radio a", &e, 0); ImGui::SameLine();
        ImGui::RadioButton("radio b", &e, 1); ImGui::SameLine();
        ImGui::RadioButton("radio c", &e, 2);
        
        
        // Use AlignTextToFramePadding() to align text baseline to the baseline of framed widgets elements
        // (otherwise a Text+SameLine+Button sequence will have the text a little too high by default!)
        // See 'Demo->Layout->Text Baseline Alignment' for details.
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Hold to repeat:");
        ImGui::SameLine();

        // Arrow buttons with Repeater
        //IMGUI_DEMO_MARKER("Widgets/Basic/Buttons (Repeating)");
        static int counter = 0;
        float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
        ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
        if (ImGui::ArrowButton("##left", ImGuiDir_Left)) { counter--; }
        ImGui::SameLine(0.0f, spacing);
        if (ImGui::ArrowButton("##right", ImGuiDir_Right)) { counter++; }
        ImGui::PopItemFlag();
        ImGui::SameLine();
        ImGui::Text("%d", counter);

        ImGui::Button("Tooltip");
        ImGui::SetItemTooltip("I am a tooltip");

        ImGui::LabelText("label", "Value");



        ImGui::SeparatorText("Inputs");
        {
            // To wire InputText() with std::string or any other custom string type,
            // see the "Text Input > Resize Callback" section of this demo, and the misc/cpp/imgui_stdlib.h file.
            //IMGUI_DEMO_MARKER("Widgets/Basic/InputText");
            static char str0[128] = "Hello, world!";
            ImGui::InputText("input text", str0, IM_ARRAYSIZE(str0));


            static char str1[128] = "";
            ImGui::InputTextWithHint("input text (w/ hint)", "enter text here", str1, IM_ARRAYSIZE(str1));

            //GUI_DEMO_MARKER("Widgets/Basic/InputInt, InputFloat");
            static int i0 = 123;
            ImGui::InputInt("input int", &i0);

            static float f0 = 0.001f;
            ImGui::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");

            static double d0 = 999999.00000001;
            ImGui::InputDouble("input double", &d0, 0.01f, 1.0f, "%.8f");

            static float f1 = 1.e10f;
            ImGui::InputFloat("input scientific", &f1, 0.0f, 0.0f, "%e");
            //ImGui::SameLine(); HelpMarker(
            //    "You can input value using the scientific notation,\n"
            //    "  e.g. \"1e+8\" becomes \"100000000\".");

            static float vec4a[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
            ImGui::InputFloat3("input float3", vec4a);
        }
        ImGui::TreePop();
    }
    
    
    
    
    
    
    
    
    
    
    
    //  4.  TESTING RADIOBUTTONS...
    {
        bool        radiobutton_open    = true;
        static int  selection1          = 0;
        static int  selection2          = 0;
        static int  selection3          = 0;
        
        ImGui::SetNextItemOpen(false, ImGuiCond_Once);
        if (ImGui::TreeNode("Radiobuttons"))
        {
            // Save starting cursor position
            ImVec2 cursor_start = ImGui::GetCursorScreenPos();
            float column_spacing = 120.0f; // Adjust for horizontal distance between columns

            // First column
            ImGui::BeginGroup();
            ImGui::RadioButton("Option A1", selection1 == 0); if (ImGui::IsItemClicked()) selection1 = 0;
            ImGui::RadioButton("Option B1", selection1 == 1); if (ImGui::IsItemClicked()) selection1 = 1;
            ImGui::RadioButton("Option C1", selection1 == 2); if (ImGui::IsItemClicked()) selection1 = 2;
            ImGui::EndGroup();


            // Second column - match top Y of first column, offset X
            ImGui::SameLine();
            ImGui::SetCursorScreenPos(ImVec2(cursor_start.x + column_spacing, cursor_start.y));
            ImGui::BeginGroup();
            ImGui::RadioButton("Option A2", selection2 == 0); if (ImGui::IsItemClicked()) selection2 = 0;
            ImGui::RadioButton("Option B2", selection2 == 1); if (ImGui::IsItemClicked()) selection2 = 1;
            ImGui::RadioButton("Option C2", selection2 == 2); if (ImGui::IsItemClicked()) selection2 = 2;
            ImGui::EndGroup();
            
            
            ImGui::SameLine();
            ImGui::SetCursorScreenPos(ImVec2(cursor_start.x + 2*column_spacing, cursor_start.y));
            ImGui::BeginGroup();
            ImGui::RadioButton("Option A3", selection3 == 0); if (ImGui::IsItemClicked()) selection3 = 0;
            ImGui::RadioButton("Option B3", selection3 == 1); if (ImGui::IsItemClicked()) selection3 = 1;
            ImGui::RadioButton("Option C3", selection3 == 2); if (ImGui::IsItemClicked()) selection3 = 2;
            ImGui::EndGroup();
        
        
        
            ImGui::SeparatorText("Various");
            {
                static float f0 = 1.0f, f1 = 2.0f, f2 = 3.0f;
                ImGui::PushItemWidth(80);
                const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD" };
                static int item = -1;
                ImGui::Combo("Combo", &item, items, IM_ARRAYSIZE(items)); ImGui::SameLine();
                ImGui::SliderFloat("X", &f0, 0.0f, 5.0f); ImGui::SameLine();
                ImGui::SliderFloat("Y", &f1, 0.0f, 5.0f); ImGui::SameLine();
                ImGui::SliderFloat("Z", &f2, 0.0f, 5.0f);
                ImGui::PopItemWidth();

                ImGui::PushItemWidth(80);
                ImGui::Text("Lists:");
                static int selection[4] = { 0, 1, 2, 3 };
                for (int i = 0; i < 4; i++)
                {
                    if (i > 0) ImGui::SameLine();
                    ImGui::PushID(i);
                    ImGui::ListBox("", &selection[i], items, IM_ARRAYSIZE(items));
                    ImGui::PopID();
                    //ImGui::SetItemTooltip("ListBox %d hovered", i);
                }
                ImGui::PopItemWidth();
                
                
                
            }
            ImGui::TreePop();
        }
    }
    
    
    
    //  5.  TESTING PLOTTING / GRAPHING 1...
    {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Graphing 1"))
        {
            this->ImPlot_Testing1();
            
            ImGui::TreePop();
        }
    }
    
    
    
    //  6.  TESTING PLOTTING / GRAPHING 2...
    {
        ImGui::SetNextItemOpen(false, ImGuiCond_Once);
        if (ImGui::TreeNode("Graphing 2"))
        {
            this->ImPlot_Testing2();
            
            ImGui::TreePop();
        }
    }
    
    
    
    //  6.  TESTING MORE ASPECTS OF DEMO CODE...
    /*
    {
        this->ImGui_Demo_Code();
    }
    */
    
    
    
    
    
    ImGui::End();
    return;
}


//  "ImPlot_Testing1"
//
void App::ImPlot_Testing1(void)
{
    constexpr int                                           NUM_CHANNELS                = 15;
    static std::array<utl::RollingBuffer, NUM_CHANNELS>     buffers;
    static float                                            max_counts[NUM_CHANNELS]    = {};
    static float                                            t                           = 0.0f;
    
    //  Channel count ranges (min, max) from sample data
    constexpr std::pair<int,int> count_ranges[NUM_CHANNELS] = {
        {365, 441},     {535, 641},     {0,   0},   {45668,52491},
        {0,   3},       {0,   4},       {0,   4},   {76220,82824},
        {0,   5},       {0,   5},       {0,   4},   {205, 295},
        {0,   4},       {0,   11},      {60,  99}
    };
    //  Human-readable channel labels
    static constexpr const char * channel_names[NUM_CHANNELS] = {
        "D",        "C",        "CD",       "B",        "BD",
        "BC",       "BCD",      "A",        "AD",       "AC",
        "ACD",      "AB",       "ABD",      "ABC",      "ABCD"
    };

    //  PLOTTING CONTROL WIDGETS...
    static float    history             = 30.0f;
    static float    clock_interval      = 1.00f;    //  integration window
    static float    clock_accumulator   = 0.0f;
    static float    row_height          = 35.0f;    //  Row height for sparkline plots.
    ImGui::SliderFloat("Row Height (px)",           &row_height,        20.0f,  90.0f,      "%.0f px");
    ImGui::SliderFloat("History (s)",               &history,           10.0f,  90.0f,      "%.1f s");
    ImGui::SliderFloat("Integration Window (s)",    &clock_interval,    0.5f,   5.0f,       "%.2f s");
    // Uncertainty control (%)
    static float uncertainty_pct = 0.0f;
    ImGui::SliderFloat("Uncertainty (%)", &uncertainty_pct, 0.0f, 10.0f, "%.1f%%");
    // Control animation start/stop and clearing via PLAY/PAUSE toggle
    static bool     animate             = false;
    if (animate) {
        if (ImGui::Button("Pause")) animate = false;
    } else {
        if (ImGui::Button("Record")) animate = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Clear Data")) {
        t = 0.0f;
        clock_accumulator = 0.0f;
        for (auto &buf : buffers)
            buf.Data.clear();
    }
    
    
    // Update each buffer's span immediately when history changes
    for (auto& buf : buffers)
        buf.Span = history;

    if (animate) {
        // Advance the internal clock accumulator
        clock_accumulator += ImGui::GetIO().DeltaTime;

        // When the integration window elapses, step time and generate new points
        if (clock_accumulator >= clock_interval) {
            clock_accumulator -= clock_interval;
            t += clock_interval;

            // Removed duplicate count_ranges declaration.

            // Per-channel “last value” to produce smooth trends
            static bool init = false;
            static float last_counts[NUM_CHANNELS];
            if (!init) {
                for (int i = 0; i < NUM_CHANNELS; ++i) {
                    auto [minv, maxv] = count_ranges[i];
                    last_counts[i] = 0.5f * (minv + maxv);
                }
                init = true;
            }

            // RNG for Poisson noise
            static std::mt19937_64 rng{std::random_device{}()};

            for (int i = 0; i < NUM_CHANNELS; ++i) {
                // Sample using Poisson to mimic counting noise
                auto [minv, maxv] = count_ranges[i];
                // Use last_counts[i] as the mean for Poisson
                std::poisson_distribution<int> pd(static_cast<int>(last_counts[i]));
                int sampled = pd(rng);
                float count = float(sampled);
                // Clamp to observed min/max
                count = std::clamp(count, float(minv), float(maxv));
                // Apply uncertainty noise (% of max)
                float noise_range = float(maxv) * (uncertainty_pct * 0.01f);
                std::uniform_real_distribution<float> ud(-noise_range, noise_range);
                count += ud(rng);
                // Re-clamp to valid bounds
                count = std::clamp(count, 0.0f, float(maxv));
                last_counts[i] = count;
                // Update peak seen so far
                max_counts[i] = std::max(max_counts[i], count);
                buffers[i].AddPoint(t, count);
            }
        }
    }


    // Tabular sparkline-style plotting for each channel
    ImPlot::PushColormap(ImPlotColormap_Cool);
    if (ImGui::BeginTable("##CoincTable", 5,    ImGuiTableFlags_BordersOuter | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Counter(s)",   ImGuiTableColumnFlags_WidthFixed,  80.0f);
        ImGui::TableSetupColumn("Max",          ImGuiTableColumnFlags_WidthFixed,  80.0f);
        ImGui::TableSetupColumn("Avg.",         ImGuiTableColumnFlags_WidthFixed,  80.0f);
        ImGui::TableSetupColumn("Count",        ImGuiTableColumnFlags_WidthFixed,  80.0f);
        ImGui::TableSetupColumn("Plot");
        ImGui::TableHeadersRow();
        
        //  Custom draw order: A, B, C, D, AB, AC, AD, BC, BD, CD, ABC, ABD, ACD, BCD, ABCD
        static constexpr int plot_order[NUM_CHANNELS] = {
            7,      3,      1,      0,      11,     9,      8,
            5,      4,      2,      13,     12,     10,     6,      14
        };
        
        for (int row = 0; row < NUM_CHANNELS; ++row) {
            int i = plot_order[row];
            ImGui::TableNextRow();
            // Column 0: label
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", channel_names[i]);
            // Column 1: latest count
            ImGui::TableSetColumnIndex(1);
            float curr = buffers[i].Data.empty() ? 0.0f : buffers[i].Data.back().y;
            ImGui::Text("%.0f", curr);
            // Column 2: average count
            ImGui::TableSetColumnIndex(2);
            int n = static_cast<int>(buffers[i].Data.size());
            if (n > 0) {
                std::vector<float> ydata(n);
                for (int j = 0; j < n; ++j)
                    ydata[j] = buffers[i].Data[j].y;
                float sum = 0.0f;
                for (int j = 0; j < n; ++j)
                    sum += ydata[j];
                float avg = sum / n;
                ImGui::Text("%.1f", avg);
            } else {
                ImGui::Text("0.0");
            }
            
            // Column 3: peak count
            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%.0f", max_counts[i]);
            
            // Column 4: sparkline trend
            ImGui::TableSetColumnIndex(4);
            if (n > 0) {
                std::vector<float> ydata(n);
                for (int j = 0; j < n; ++j)
                    ydata[j] = buffers[i].Data[j].y;
                float raw_max = *std::max_element(ydata.begin(), ydata.end());
                float pad = raw_max * 0.1f; // 10% headroom
                float y_max = raw_max + pad;
                utl::Sparkline(
                    ("##trend" + std::to_string(i)).c_str(),
                    ydata.data(), n,
                    0.0f, y_max,
                    0,
                    ImPlot::GetColormapColor(i),
                    ImVec2(-1, row_height)
                );
            }
        }
        ImGui::EndTable();
    
    
    // Shared time axis at bottom
    {
        ImPlotAxisFlags xflags = ImPlotAxisFlags_NoGridLines;
        if (ImPlot::BeginPlot("##TimeAxis", ImVec2(-1, 20))) {
            ImPlot::SetupAxes("Time (s)", nullptr, ImPlotAxisFlags_None, ImPlotAxisFlags_NoDecorations);
            ImPlot::SetupAxisLimits(ImAxis_X1, 0.0f, t, ImGuiCond_Always);
            ImPlot::EndPlot();
        }
    }
        ImPlot::PopColormap();
    }
}


//  "ImPlot_Testing2"
//
void App::ImPlot_Testing2(void)
{
    static ImGuiTableFlags flags    = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV |
                                      ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable;
    static bool     anim            = true;
    static int      offset          = 0;
    
    
    ImGui::BulletText("Plots can be used inside of ImGui tables as another means of creating subplots.");
    ImGui::Checkbox("Animate",&anim);
    if (anim)
        offset = (offset + 1) % 100;
        
    if (ImGui::BeginTable("##table", 3, flags, ImVec2(-1,0)))
    {
        ImGui::TableSetupColumn("Electrode",    ImGuiTableColumnFlags_WidthFixed,   75.0f);
        ImGui::TableSetupColumn("Voltage",      ImGuiTableColumnFlags_WidthFixed,   75.0f);
        ImGui::TableSetupColumn("EMG Signal");
        ImGui::TableHeadersRow();
        
        ImPlot::PushColormap(ImPlotColormap_Cool);
        for (int row = 0; row < 10; row++)
        {
            ImGui::TableNextRow();
            static float data[100];
            srand(row);
            for (int i = 0; i < 100; ++i)
                data[i] = utl::RandomRange(0.0f,10.0f);
                
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("EMG %d", row);
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.3f V", data[offset]);
            ImGui::TableSetColumnIndex(2);
            ImGui::PushID(row);
            utl::Sparkline("##spark",data,100,0,11.0f,offset,ImPlot::GetColormapColor(row),ImVec2(-1, 35));
            ImGui::PopID();
        }
        ImPlot::PopColormap();
        ImGui::EndTable();
    }

    return;
}


//  "ImGui_Demo_Code"
//
void App::ImGui_Demo_Code(void)
{
    ImGuiIO &                       io              = ImGui::GetIO(); (void)io;
    
    
    //IMGUI_DEMO_MARKER("Widgets/Basic");
    if (ImGui::TreeNode("Basic"))
    {
        ImGui::SeparatorText("General");

        //IMGUI_DEMO_MARKER("Widgets/Basic/Button");
        static int clicked = 0;
        if (ImGui::Button("Button"))
            clicked++;
        if (clicked & 1)
        {
            ImGui::SameLine();
            ImGui::Text("Thanks for clicking me!");
        }

        //IMGUI_DEMO_MARKER("Widgets/Basic/Checkbox");
        static bool check = true;
        ImGui::Checkbox("checkbox", &check);

        //IMGUI_DEMO_MARKER("Widgets/Basic/RadioButton");
        static int e = 0;
        ImGui::RadioButton("radio a", &e, 0); ImGui::SameLine();
        ImGui::RadioButton("radio b", &e, 1); ImGui::SameLine();
        ImGui::RadioButton("radio c", &e, 2);

        // Color buttons, demonstrate using PushID() to add unique identifier in the ID stack, and changing style.
        //IMGUI_DEMO_MARKER("Widgets/Basic/Buttons (Colored)");
        for (int i = 0; i < 7; i++)
        {
            if (i > 0)
                ImGui::SameLine();
            ImGui::PushID(i);
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));
            ImGui::Button("Click");
            ImGui::PopStyleColor(3);
            ImGui::PopID();
        }

        // Use AlignTextToFramePadding() to align text baseline to the baseline of framed widgets elements
        // (otherwise a Text+SameLine+Button sequence will have the text a little too high by default!)
        // See 'Demo->Layout->Text Baseline Alignment' for details.
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Hold to repeat:");
        ImGui::SameLine();

        // Arrow buttons with Repeater
        //IMGUI_DEMO_MARKER("Widgets/Basic/Buttons (Repeating)");
        static int counter = 0;
        float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
        ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
        if (ImGui::ArrowButton("##left", ImGuiDir_Left)) { counter--; }
        ImGui::SameLine(0.0f, spacing);
        if (ImGui::ArrowButton("##right", ImGuiDir_Right)) { counter++; }
        ImGui::PopItemFlag();
        ImGui::SameLine();
        ImGui::Text("%d", counter);

        ImGui::Button("Tooltip");
        ImGui::SetItemTooltip("I am a tooltip");

        ImGui::LabelText("label", "Value");

        ImGui::SeparatorText("Inputs");

        {
            // To wire InputText() with std::string or any other custom string type,
            // see the "Text Input > Resize Callback" section of this demo, and the misc/cpp/imgui_stdlib.h file.
            //IMGUI_DEMO_MARKER("Widgets/Basic/InputText");
            static char str0[128] = "Hello, world!";
            ImGui::InputText("input text", str0, IM_ARRAYSIZE(str0));


            static char str1[128] = "";
            ImGui::InputTextWithHint("input text (w/ hint)", "enter text here", str1, IM_ARRAYSIZE(str1));

            //GUI_DEMO_MARKER("Widgets/Basic/InputInt, InputFloat");
            static int i0 = 123;
            ImGui::InputInt("input int", &i0);

            static float f0 = 0.001f;
            ImGui::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");

            static double d0 = 999999.00000001;
            ImGui::InputDouble("input double", &d0, 0.01f, 1.0f, "%.8f");

            static float f1 = 1.e10f;
            ImGui::InputFloat("input scientific", &f1, 0.0f, 0.0f, "%e");
            //ImGui::SameLine(); HelpMarker(
            //    "You can input value using the scientific notation,\n"
            //    "  e.g. \"1e+8\" becomes \"100000000\".");

            static float vec4a[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
            ImGui::InputFloat3("input float3", vec4a);
        }

        ImGui::SeparatorText("Drags");

        {
            //IMGUI_DEMO_MARKER("Widgets/Basic/DragInt, DragFloat");
            static int i1 = 50, i2 = 42, i3 = 128;
            ImGui::DragInt("drag int", &i1, 1);
            //ImGui::SameLine(); HelpMarker(
            //    "Click and drag to edit value.\n"
            //    "Hold SHIFT/ALT for faster/slower edit.\n"
            //    "Double-click or CTRL+click to input value.");
            ImGui::DragInt("drag int 0..100", &i2, 1, 0, 100, "%d%%", ImGuiSliderFlags_AlwaysClamp);
            ImGui::DragInt("drag int wrap 100..200", &i3, 1, 100, 200, "%d", ImGuiSliderFlags_WrapAround);

            static float f1 = 1.00f, f2 = 0.0067f;
            ImGui::DragFloat("drag float", &f1, 0.005f);
            ImGui::DragFloat("drag small float", &f2, 0.0001f, 0.0f, 0.0f, "%.06f ns");
            //ImGui::DragFloat("drag wrap -1..1", &f3, 0.005f, -1.0f, 1.0f, NULL, ImGuiSliderFlags_WrapAround);
        }

        ImGui::SeparatorText("Sliders");

        {
            //IMGUI_DEMO_MARKER("Widgets/Basic/SliderInt, SliderFloat");
            static int i1 = 0;
            ImGui::SliderInt("slider int", &i1, -1, 3);
            //ImGui::SameLine(); HelpMarker("CTRL+click to input value.");

            static float f1 = 0.123f, f2 = 0.0f;
            ImGui::SliderFloat("slider float", &f1, 0.0f, 1.0f, "ratio = %.3f");
            ImGui::SliderFloat("slider float (log)", &f2, -10.0f, 10.0f, "%.4f", ImGuiSliderFlags_Logarithmic);

            //IMGUI_DEMO_MARKER("Widgets/Basic/SliderAngle");
            static float angle = 0.0f;
            ImGui::SliderAngle("slider angle", &angle);

            // Using the format string to display a name instead of an integer.
            // Here we completely omit '%d' from the format string, so it'll only display a name.
            // This technique can also be used with DragInt().
            //IMGUI_DEMO_MARKER("Widgets/Basic/Slider (enum)");
            enum Element { Element_Fire, Element_Earth, Element_Air, Element_Water, Element_COUNT };
            static int elem = Element_Fire;
            const char* elems_names[Element_COUNT] = { "Fire", "Earth", "Air", "Water" };
            const char* elem_name = (elem >= 0 && elem < Element_COUNT) ? elems_names[elem] : "Unknown";
            ImGui::SliderInt("slider enum", &elem, 0, Element_COUNT - 1, elem_name); // Use ImGuiSliderFlags_NoInput flag to disable CTRL+Click here.
            //ImGui::SameLine(); HelpMarker("Using the format string parameter to display a name instead of the underlying integer.");
        }

        ImGui::SeparatorText("Selectors/Pickers");

        {
            //IMGUI_DEMO_MARKER("Widgets/Basic/ColorEdit3, ColorEdit4");
            static float col1[3] = { 1.0f, 0.0f, 0.2f };
            static float col2[4] = { 0.4f, 0.7f, 0.0f, 0.5f };
            ImGui::ColorEdit3("color 1", col1);

            ImGui::ColorEdit4("color 2", col2);
        }

        {
            // Using the _simplified_ one-liner Combo() api here
            // See "Combo" section for examples of how to use the more flexible BeginCombo()/EndCombo() api.
            //IMGUI_DEMO_MARKER("Widgets/Basic/Combo");
            const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIIIIII", "JJJJ", "KKKKKKK" };
            static int item_current = 0;
            ImGui::Combo("combo", &item_current, items, IM_ARRAYSIZE(items));
            //ImGui::SameLine(); HelpMarker(
            //    "Using the simplified one-liner Combo API here.\n"
            //     "Refer to the \"Combo\" section below for an explanation of how to use the more flexible and general BeginCombo/EndCombo API.");
        }

        {
            // Using the _simplified_ one-liner ListBox() api here
            // See "List boxes" section for examples of how to use the more flexible BeginListBox()/EndListBox() api.
            //IMGUI_DEMO_MARKER("Widgets/Basic/ListBox");
            const char* items[] = { "Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon" };
            static int item_current = 1;
            ImGui::ListBox("listbox", &item_current, items, IM_ARRAYSIZE(items), 4);
            //ImGui::SameLine(); HelpMarker(
            //    "Using the simplified one-liner ListBox API here.\n"
            //    "Refer to the \"List boxes\" section below for an explanation of how to use the more flexible and general BeginListBox/EndListBox API.");
        }

        // Testing ImGuiOnceUponAFrame helper.
        //static ImGuiOnceUponAFrame once;
        //for (int i = 0; i < 5; i++)
        //    if (once)
        //        ImGui::Text("This will be displayed only once.");

        ImGui::TreePop();
    
    }


    return;
}







//***************************************************************************//
//
//
//
//***************************************************************************//
//***************************************************************************//
}//   END OF "cb" NAMESPACE.
























//***************************************************************************//
//***************************************************************************//
//
//  END.

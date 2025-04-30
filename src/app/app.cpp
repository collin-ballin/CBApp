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

#include <random>
#include <algorithm>

// #ifdef CBAPP_USE_VIEWPORT
# include "imgui_internal.h"
// #endif  //  CBAPP_USE_VIEWPORT  //

#ifdef __CBAPP_DEBUG__                //  <======| Fix for issue wherein multiple instances of application
# include <thread>                //           are launched when DEBUG build is run inside Xcode IDE...
# include <chrono>
#endif     //  __CBAPP_DEBUG__  //



//  0.      UTILITY FUNCTIONS [NON-MEMBER FUNCTIONS]...
// *************************************************************************** //
// *************************************************************************** //

//  "run_application"
//  Client-code interface to creating and running the application...
//
int cb::run_application([[maybe_unused]] int argc, [[maybe_unused]] char ** argv)
{
    constexpr const char *  xcp_header              = "MAIN | ";
    constexpr const char *  xcp_type_runtime        = "Caught std::runtime_error exception";
    constexpr const char *  xcp_type_unknown        = "Caught std::runtime_error exception";
    constexpr const char *  xcp_at_start            = "while initializing the application.\n";
    constexpr const char *  xcp_at_runtime          = "during program runtime.\n";
    int                     status                  = EXIT_SUCCESS;
    
    
#ifdef __CBAPP_DEBUG__    //  WORK-AROUND / PATCH FOR XCODE ISSUE...
    std::this_thread::sleep_for(std::chrono::seconds(1));
#endif

    try {   //  1.     CREATE APPLICATION INSTANCE...
        App my_app;
        
        try {   //  2.     ENTER APPLICATION MAIN-LOOP...
            my_app.run();
        }
        catch (const std::runtime_error & e) {      //  2.1     CATCH RUNTIME ERROR (PROGRAM WAS INITIALIZED CORRECTLY)...
            std::cerr << xcp_header << xcp_type_runtime << xcp_at_runtime << "Traceback: " << e.what() << "\n";
            status = EXIT_FAILURE;
        }
        catch (...) {                               //  2.2     CATCH OTHER EXCEPTIONS AND EXIT...
            std::cerr << xcp_header << xcp_type_unknown << xcp_at_runtime;
            status = EXIT_FAILURE;
        }
    }
    catch (const std::runtime_error & e) {      //  1.1     CATCH INITIALIZATION-TIME ERROR (DURING "DEAR IMGUI" INITIALIZATION)...
        std::cerr << xcp_header << xcp_type_runtime << xcp_at_start << "Traceback: " << e.what() << "\n";
        status = EXIT_FAILURE;
    }
    catch (...) {                               //  1.2.    CATCH OTHER EXCEPTIONS AND EXIT...
        std::cerr << xcp_header << xcp_type_unknown << xcp_at_start;
        status = EXIT_FAILURE;
    }
    
    return status;
}






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
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    this->m_window = glfwCreateWindow(cb::app::DEF_ROOT_WIN_WIDTH, cb::app::DEF_ROOT_WIN_HEIGHT, cb::app::DEF_ROOT_WIN_TITLE, nullptr, nullptr);
    if (!this->m_window)
        throw std::runtime_error("Call to glfwInit() returned NULL");
        
    //utl::SetGLFWWindowLocation(this->m_window, utl::WindowLocation::Center, cb::app::DEF_ROOT_WINDOW_SCALE);
    utl::set_window_scale(this->m_window, cb::app::DEF_ROOT_WINDOW_SCALE);
    
    
    glfwMakeContextCurrent(this->m_window);
    glfwSwapInterval(1);        // Enable vsync
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



    //  2.  SETUP "Dear ImGui" CONTEXT...
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO &       io          = ImGui::GetIO(); (void)io;
    ImGuiStyle &    style       = ImGui::GetStyle();
    this->load();


// #ifdef CBAPP_USE_VIEWPORT
    style.Colors[ImGuiCol_WindowBg].w           = 0.0f;     // host window background
    style.Colors[ImGuiCol_DockingEmptyBg].w     = 0.0f;     // the “empty” parts of your dockspace
// #endif  //  CBAPP_USE_VIEWPORT  //
    
    
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
       
    this->m_main_viewport   = ImGui::GetMainViewport();

    return;
}


//  "load"
//
void App::load(void)
{
#ifndef __EMSCRIPTEN__
    ImGuiIO &       io          = ImGui::GetIO(); (void)io;
    ImGuiStyle &    style       = ImGui::GetStyle();
    ImGui::StyleColorsDark();
    
    
#if defined(CBAPP_DISABLE_INI)
    io.IniFilename              = nullptr;
# else
    io.IniFilename              = cb::app::INI_FILEPATH;
    ImGui::LoadIniSettingsFromDisk(cb::app::INI_FILEPATH);
#endif  //  CBAPP_DISABLE_INI  //
    
    
    io.ConfigFlags             |= this->m_io_flags;     //  2.2 Configure I/O Settings.
    //io.ConfigViewportsNoAutoMerge     = true;
    //io.ConfigViewportsNoTaskBarIcon   = true;
    

#ifdef CBAPP_LOAD_STYLE_FILE
    utl::LoadStyleFromDisk(style, cb::app::STYLE_FILEPATH);
#endif  //  CBAPP_LOAD_STYLE_FILE  //


    //      3.3     Loading Fonts.
    for (int i = 0; i < int(Font::Count); ++i) {
        const auto &    info            = cb::app::APPLICATION_FONT_STYLES[i];
        m_fonts[static_cast<Font>(i)]   = io.Fonts->AddFontFromFileTTF(info.path.data(), info.size);
        IM_ASSERT(m_fonts[static_cast<Font>(i)]);
    }



#endif  //  __EMSCRIPTEN__  //
    return;
}




//  Destructor.
//
App::~App(void)
{
    this->destroy();
}


//  "destroy"       | protected
//
void App::destroy(void)
{
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




// *************************************************************************** //
//
//
//  1B.     PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "run"
//
void App::run(void)
{
    ImGuiIO &       io          = ImGui::GetIO(); (void)io;


    //  1.  MAIN PROGRAM LOOP...
#ifdef __EMSCRIPTEN__
    io.IniFilename = nullptr;   //  For an Emscripten build, we are disabling file-system access.  So let's not attempt an
    EMSCRIPTEN_MAINLOOP_BEGIN   //  "fopen()" of the "imgui.ini" file.  You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
#else
    while ( !glfwWindowShouldClose(this->m_window) && this->m_running )
#endif  //  __EMSCRIPTEN__  //
    {
        //  1.1     SET A POLL AND HANDLE EVENTS (inputs, window resize, etc.)...
        //          You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        //              - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        //              - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        //          Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        if (glfwGetWindowAttrib(this->m_window, GLFW_ICONIFIED))    { ImGui_ImplGlfw_Sleep(10); continue; }
        

        //  2.  START THE "DEAR IMGUI" FRAME...
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        //  3.  DRAW MAIN "UI" ELEMENTS...
        this->run_IMPL();


        //  4.  RENDERING...
        ImGui::Render();
        glfwGetFramebufferSize(this->m_window,  &this->m_window_w,  &this->m_window_h); // int display_w, display_h;     // glfwGetFramebufferSize(this->m_window, &display_w, &display_h);
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
        glfwSwapBuffers(this->m_window);
        
        
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
    ImGuiIO &                   io              = ImGui::GetIO(); (void)io;
    ImVec4                      sidebar_bg      = ImVec4(0.2f, 0.3f, 0.6f, 1.0f); // RGBA       | SIDEBAR_COLOR = #323232
    static cb::GraphingApp      graphing_app    = cb::GraphingApp(10, 10);
    
    //  0.1     RENDER THE DOCKSPACE...
    ImGui::PushStyleColor(ImGuiCol_WindowBg, this->m_dock_bg);
    ImGui::DockSpaceOverViewport(0, this->m_main_viewport);
    ImGui::PopStyleColor();
    
    
    //  0.2     DRAW GENERAL UI ITEMS (MAIN-MENU, ETC)...
    this->Display_Main_Menu_Bar();

    
    
    //  1.1     DRAWING THE "SIDE-BAR MENU" WINDOW...
    this->Display_Sidebar_Menu(nullptr);
    
    //  1.2     DISPLAY THE MAIN APPLICATION WINDOW...
    this->Display_Main_Window(nullptr);
    
    //  1.3     DISPLAY THE GRAPHING APPLICATION...
    graphing_app.Begin(&this->m_show_graphing_app);


    
    //  2.1     DISPLAY THE STYLE-EDITOR WINDOW...
    if (this->m_show_style_editor) {
        ImGui::Begin("Style Editor (ImGui)", &m_show_style_editor);
        ImGui::ShowStyleEditor();
        ImGui::End();
    }
    
    //  2.2     DISPLAY THE "LOGGING" WINDOW...
    if (this->m_show_log_window) {
        cb::ShowExampleAppLog(&this->m_show_log_window);
    }
    
    //  2.3     DISPLAY THE "CONSOLE" WINDOW...
    if (this->m_show_console_window) {
        cb::ShowExampleAppConsole(&this->m_show_console_window);
    }
    
    //  2.4     DISPLAY THE "METRICS / DEBUGGER" WINDOW...
    if (this->m_show_metrics_window) {
        ImGui::ShowMetricsWindow(&this->m_show_metrics_window);
    }
    
    //  2.5     DISPLAY THE "Dear ImGui" DEMO WINDOW...
    if (this->m_show_imgui_demo)
        ImGui::ShowDemoWindow(&this->m_show_imgui_demo);
        
    //  2.6     DISPLAY THE "Dear ImPlot" WINDOW...
    if (this->m_show_implot_demo)
        ImPlot::ShowDemoWindow(&this->m_show_implot_demo);
        
        
#ifdef CBAPP_ENABLE_CB_DEMO
    //  2.7     DISPLAY THE "Dear ImPlot" WINDOW...
    if (this->m_show_cb_demo)
        this->m_cb_demo.Begin(&m_show_cb_demo);
#endif  //  CBAPP_ENABLE_CB_DEMO  //
    
    
    return;
}



// *************************************************************************** //
//
//
//  3.2     MAIN APPLICATION GUI FUNCTIONS...    | PRIVATE.
// *************************************************************************** //
// *************************************************************************** //

//  "Display_Main_Window"
//
void App::Display_Main_Window(bool * p_open)
{
    // ImVec2                       win_pos(this->m_main_viewport->WorkPos.x + 750,   this->m_main_viewport->WorkPos.x + 20);
    ImGuiIO &                       io              = ImGui::GetIO(); (void)io;
    
    
    //  1.  CREATE THE WINDOW AND BEGIN APPENDING WIDGETS INTO IT...
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, this->m_main_bg);
    ImGui::Begin(app::DEF_MAIN_WIN_TITLE, p_open, this->m_main_win_flags);
    {
        ImGui::PopStyleColor();
        
        
        //  3.  TESTING PLOTTING / GRAPHING 1...
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
    
    
    
    ImGui::End();
    return;
}



// *************************************************************************** //
//
//
//  3.3     ADDITIONAL APP FUNCTIONS...    | PRIVATE.
// *************************************************************************** //
// *************************************************************************** //







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

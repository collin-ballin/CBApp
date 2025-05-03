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
    : m_menubar(m_state), m_graphing_app(10, 10)
{
    glfwSetErrorCallback(utl::glfw_error_callback);     //  1.  SET GLFW CALLBACK & CHECK IF PROPERLY INITIALIZED...
    if (!glfwInit())    throw std::runtime_error(cb::error::GLFW_INIT_ERROR);
    this->m_glsl_version = utl::get_glsl_version();     //  2.  DECIDE WHICH GL + GLSL VERSION...
    this->init();                                       //  3.  INVOKE "HELPER" FUNCTION TO COMPLETE INITIALIZATION...
    
    
    return;
}


//  "init"          | private
//
void App::init(void)
{
    //  1.  CREATE A WINDOW WITH GRAPHICS CONTEXT...
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER,    GLFW_TRUE);
    //glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER,   GLFW_TRUE);
    this->m_window = glfwCreateWindow(cb::app::DEF_ROOT_WIN_WIDTH, cb::app::DEF_ROOT_WIN_HEIGHT, cb::app::DEF_ROOT_WIN_TITLE, nullptr, nullptr);
    if (!this->m_window) {
        throw std::runtime_error(cb::error::GLFW_WINDOW_INIT_ERROR);
    }
        
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
    
    
    
    //  4.      INITIALIZE DELEGATOR CLASSES
    //          (SOME OF THESE HAVE TO BE DONE **AFTER** WE CREATE IMGUI CONTEXT)...
    this->m_graphing_app.initialize();
       
       
       
    auto & windows          = this->m_state.m_windows;
    m_primary_windows       = { std::string( windows[Window::MainApp].uuid ),
                                std::string( windows[Window::GraphingApp].uuid ) };
                                                            
       
    IM_ASSERT( std::find(this->m_primary_windows.begin(), this->m_primary_windows.end(),
                         app::DEF_SIDEBAR_WIN_TITLE) == this->m_primary_windows.end() &&
                         "Sidebar window should not be in right_docked_windows!");

    return;
}


//  "load"
//
void App::load(void)
{
#ifndef __EMSCRIPTEN__
    auto &          m_windows       = this->m_state.m_windows;
    auto &          m_fonts         = this->m_state.m_fonts;
    ImGuiIO &       io              = ImGui::GetIO(); (void)io;
    ImGuiStyle &    style           = ImGui::GetStyle();
    ImGui::StyleColorsDark();
    
    
#if defined(CBAPP_DISABLE_INI)
    io.IniFilename                  = nullptr;
# else
    io.IniFilename                  = cb::app::INI_FILEPATH;
    ImGui::LoadIniSettingsFromDisk(cb::app::INI_FILEPATH);
#endif  //  CBAPP_DISABLE_INI  //
    
    
    io.ConfigFlags                 |= this->m_io_flags;     //  2.2 Configure I/O Settings.
    //io.ConfigViewportsNoAutoMerge     = true;
    //io.ConfigViewportsNoTaskBarIcon   = true;
    

#ifdef CBAPP_LOAD_STYLE_FILE
    utl::LoadStyleFromDisk(style, cb::app::STYLE_FILEPATH);
#endif  //  CBAPP_LOAD_STYLE_FILE  //



    //      3.3     Initialize Window Parameters.
    for (std::size_t i = 0; i < static_cast<int>(Window::Count); ++i)
    {
        Window      uuid                    = static_cast<Window>(i);
        m_windows[uuid].render_fn           = [this, uuid]([[maybe_unused]] const char * id,
                                                           [[maybe_unused]] bool * p_open,
                                                           [[maybe_unused]] ImGuiWindowFlags flags) {
            this->dispatch_window_function(uuid); // uuid is now correctly captured per-lambda
        };
    }


#ifndef CBAPP_DISABLE_CUSTOM_FONTS
    //      3.4     Loading Fonts.
    for (int i = 0; i < static_cast<int>(Font::Count); ++i)
    {
        const auto &    info                = cb::app::APPLICATION_FONT_STYLES[i];
        m_fonts[static_cast<Font>(i)]       = io.Fonts->AddFontFromFileTTF(info.path.data(), info.size);
        IM_ASSERT( m_fonts[static_cast<Font>(i)] );
    }
# else
    for (int i = 0; i < static_cast<int>(Font::Count); ++i)
    {
        const auto &    info                = cb::app::APPLICATION_FONT_STYLES[i];
        ImFontConfig    config;
        config.SizePixels                   = info.size;
        m_fonts[static_cast<Font>(i)]       = io.Fonts->AddFontDefault(&config);
        IM_ASSERT( m_fonts[static_cast<Font>(i)] );
    }
#endif  //  CBAPP_DISABLE_CUSTOM_FONTS  //



#endif  //  __EMSCRIPTEN__  //
    return;
}


//  "dispatch_window_function"
//
void App::dispatch_window_function(const Window & uuid)
{
    auto &          w       = m_state.m_windows[uuid];
    
    
    //  DISPATCH EACH RENDER FUNCTION FOR EACH WINDOW OF THE APPLICATION...
    switch (uuid)
    {
        case Window::Sidebar:       {
            this->Display_Sidebar_Menu(     w.uuid.data(),      nullptr,        w.flags);
            break;
        }
        case Window::Menubar:       {
            this->m_menubar.Begin(          w.uuid.data(),      nullptr,        w.flags);
            break;
        }
        case Window::MainApp:       {
            this->Display_Main_Window(      w.uuid.data(),      nullptr,        w.flags);
            break;
        }
        case Window::GraphingApp:   {
            this->m_graphing_app.Begin(     w.uuid.data(),      nullptr,        w.flags);
            break;
        }
        //
        //
        //
        case Window::StyleEditor:   {
            cb::ShowStyleEditor(            w.uuid.data(),      &w.open,        w.flags);
            break;
        }
        case Window::Logs:          {
            cb::ShowExampleAppLog(          w.uuid.data(),      &w.open,        w.flags);
            break;
        }
        case Window::Console:       {
            cb::ShowExampleAppConsole(      w.uuid.data(),      &w.open,        w.flags);
            break;
        }
        case Window::Metrics:       {
            cb::ShowMetricsWindow(          w.uuid.data(),      &w.open,        w.flags);
            break;
        }
        //
        //
        //
        case Window::ImGuiDemo:     {
            cb::ShowImGuiDemoWindow(        w.uuid.data(),      &w.open,        w.flags);
            break;
        }
        case Window::ImPlotDemo:     {
            cb::ShowImPlotDemoWindow(       w.uuid.data(),      &w.open,        w.flags);
            break;
        }
        //
        //  ...
        //
        default: {
            break;
        }
    }
    
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
    static const ImGuiIO &      io          = ImGui::GetIO(); (void)io;


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
    static ImGuiIO &                io                  = ImGui::GetIO(); (void)io;
    static ImGuiStyle &             style               = ImGui::GetStyle();
    static size_t                   idx                 = static_cast<size_t>(0);
    static const size_t             N_WINDOWS           = static_cast<size_t>(Window::Count);
    
    static ImVec4                   sidebar_bg          = ImVec4(0.2f, 0.3f, 0.6f, 1.0f); // RGBA       | SIDEBAR_COLOR = #323232
    

    static Window                   uuid                = static_cast<Window>(idx);
    static app::WinInfo &           winfo               = m_state.m_windows[uuid];
    static bool *                   p_open              = nullptr;      // &window.open;
    
    
    
    //  0.1     RENDER THE DOCKSPACE...
#ifndef CBAPP_NEW_DOCKSPACE
    ImGui::PushStyleColor(ImGuiCol_WindowBg, this->m_dock_bg);
    //ImGui::DockSpaceOverViewport(0, this->m_main_viewport);
    ImGui::DockSpaceOverViewport(this->m_main_viewport, ImGuiDockNodeFlags_NoTabBar);
    ImGui::PopStyleColor();
#else
    const ImGuiViewport * viewport  = ImGui::GetMainViewport();
    this->m_dockspace_id            = ImGui::GetID(this->m_dock_name);

    static bool first_time = true;
    if (first_time) {
        first_time = false;

        ImGui::DockBuilderRemoveNode(this->m_dockspace_id ); // clear any existing layout
        ImGui::DockBuilderAddNode(this->m_dockspace_id , ImGuiDockNodeFlags_DockSpace | ImGuiDockNodeFlags_NoTabBar);
    
    
        ImGui::DockBuilderSetNodeSize(this->m_dockspace_id , viewport->WorkSize);

        ImGuiID dock_id_left, dock_id_main;
        ImGui::DockBuilderSplitNode(this->m_dockspace_id , ImGuiDir_Left, 0.2f, &dock_id_left, &dock_id_main);

        // Make sure names match actual windows
        ImGui::DockBuilderDockWindow(app::DEF_SIDEBAR_WIN_TITLE, dock_id_left);
        ImGui::DockBuilderDockWindow(app::DEF_MAIN_WIN_TITLE, dock_id_main);

        ImGui::DockBuilderFinish(this->m_dockspace_id );
    }


    ImGui::SetNextWindowPos(viewport->WorkPos);                 //  Set up invisible host window covering the entire viewport
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);    //  Make host window invisible with no padding, rounding, borders, etc.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    //  Host window flags (invisible, non-interactive)
    ImGuiWindowFlags host_window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

    ImGui::Begin( "##DockHostWindow", nullptr, host_window_flags); //this->m_state.m_windows[Window::Host].flags );
        ImGui::PopStyleVar(3);  //  DockSpace() creates docking area within this host window
        ImGui::DockSpace(this->m_dockspace_id );
    ImGui::End();
#endif  //  CBAPP_NEW_DOCKSPACE  //
    
    
    
    if (this->m_rebuild_dockspace)
    {
        this->RebuildDockLayout();
        this->m_rebuild_dockspace = false;
    }
    
    
    //  0.2     DRAW GENERAL UI ITEMS (MAIN-MENU, ETC)...
    // this->Display_Main_Menu_Bar();


    //  1.1     DRAW APPLICATION WINDOWS...
    for (idx = 0; idx < N_WINDOWS; ++idx)
    {
        uuid            = static_cast<Window>(idx);
        winfo           = m_state.m_windows[uuid];
        
        if (winfo.render_fn && winfo.open) {
            winfo.render_fn( winfo.uuid.data(), &winfo.open, winfo.flags );
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
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::DockBuilderRemoveNode(this->m_dockspace_id);
    ImGui::DockBuilderAddNode(this->m_dockspace_id, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(this->m_dockspace_id, viewport->WorkSize);

    ImGuiID dock_id_left, dock_id_main;
    ImGui::DockBuilderSplitNode(this->m_dockspace_id, ImGuiDir_Left, this->m_sidebar_ratio, &dock_id_left, &dock_id_main);

    // Only dock sidebar window once to the left
    ImGui::DockBuilderDockWindow(app::DEF_SIDEBAR_WIN_TITLE, dock_id_left);

    // Dock all *other* persistent windows to the main area (skip sidebar)
    for (const std::string& win_name : this->m_primary_windows) {
        if (win_name != app::DEF_SIDEBAR_WIN_TITLE) {
            ImGui::DockBuilderDockWindow(win_name.c_str(), dock_id_main);
        }
    }

    ImGui::DockBuilderFinish(this->m_dockspace_id);
}


//  "PushFont"
//
void App::PushFont( [[maybe_unused]] const Font & which) {
//#ifndef CBAPP_DISABLE_CUSTOM_FONTS
    ImGui::PushFont( this->m_state.m_fonts[which] );
//#endif  //  CBAPP_DISABLE_CUSTOM_FONTS  //
    return;
}


//  "PopFont"
//
void App::PopFont(void) {
//#ifndef CBAPP_DISABLE_CUSTOM_FONTS
    ImGui::PopFont();
//#endif  //  CBAPP_DISABLE_CUSTOM_FONTS  //
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

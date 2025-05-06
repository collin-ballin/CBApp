/***********************************************************************************
*
*       *********************************************************************
*       ****               I N I T . C P P  ____  F I L E                ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      May 03, 2025.
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



//  1A.     INITIALIZATION  | DEFAULT CONSTRUCTOR, DESTRUCTOR, ETC...
// *************************************************************************** //
// *************************************************************************** //

//  Default Constructor.
//
App::App(void)
    : m_menubar(m_state), m_graphing_app(10, 10)
{
    glfwSetErrorCallback(utl::glfw_error_callback);     //  1.  SET GLFW CALLBACK & CHECK IF PROPERLY INITIALIZED...
    if (!glfwInit())
        throw std::runtime_error(cb::error::GLFW_INIT_ERROR);
    
    
    this->m_glsl_version = utl::get_glsl_version();     //  2.  DECIDE WHICH GL + GLSL VERSION...
    this->init();                                       //  3.  INVOKE "HELPER" FUNCTION TO COMPLETE INITIALIZATION...
}


//  "init"          | private
//
void App::init(void)
{
    //  1.      CREATE A WINDOW WITH GRAPHICS CONTEXT, CREATE "IMGUI" & "IMPLOT" CONTEXTS, ETC...
    this->CreateContext();


    //  2.      SECOND PART OF INITIALIZATION...            | INTERNAL STUFF...
    this->m_main_viewport   = ImGui::GetMainViewport();
    this->init_appstate();
    this->load();
    
    
    //  3.      THIRD PART OF INITIALIZATION...             | INITIALIZE DELEGATOR CLASSES
    //          (SOME OF THESE HAVE TO BE DONE **AFTER** WE CREATE IMGUI CONTEXT)...
    this->m_graphing_app.initialize();
       
       
    //  4.      PERFORM ALL RUNTIME ASSERTION STATEMENTS AND
    //          STATE VALIDATIONS BEFORE APPLICATION BEGINS...
    this->init_asserts();

    return;
}



// *************************************************************************** //
//
//
//  1B.     INITIALIZATION  | DEFAULT CONSTRUCTOR, DESTRUCTOR, ETC...
// *************************************************************************** //
// *************************************************************************** //

//  "CreateContext"
//
void App::CreateContext(void)
{
    //  1.  CREATE A WINDOW WITH GRAPHICS CONTEXT...
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER,    GLFW_TRUE);
    //glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER,   GLFW_TRUE);
    this->m_glfw_window = glfwCreateWindow(cb::app::DEF_ROOT_WIN_WIDTH, cb::app::DEF_ROOT_WIN_HEIGHT, cb::app::DEF_ROOT_WIN_TITLE, nullptr, nullptr);
    if (!this->m_glfw_window) {
        throw std::runtime_error(cb::error::GLFW_WINDOW_INIT_ERROR);
    }
        
    //utl::SetGLFWWindowLocation(this->m_glfw_window, utl::WindowLocation::Center, cb::app::DEF_ROOT_WINDOW_SCALE);
    utl::set_window_scale(this->m_glfw_window, cb::app::DEF_ROOT_WINDOW_SCALE);
    
    glfwMakeContextCurrent(this->m_glfw_window);
    glfwSwapInterval(1);        // Enable vsync
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    //  2.  SETUP "Dear ImGui" CONTEXT...
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO &       io                          = ImGui::GetIO(); (void)io;
    ImGuiStyle &    style                       = ImGui::GetStyle();


    style.Colors[ImGuiCol_WindowBg].w           = 0.0f;     // host window background
    style.Colors[ImGuiCol_DockingEmptyBg].w     = 0.0f;     // the “empty” parts of your dockspace
    
    
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)      //  When viewports are enabled we tweak WindowRounding/WindowBg
    {                                                           //  so platform windows can look identical to regular ones.
        style.WindowRounding                = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w   = 1.0f;
    }
    
    
    
    //      3.2     Setup Platform/Renderer backends.
    ImGui_ImplGlfw_InitForOpenGL(this->m_glfw_window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif      //  __EMSCRIPTEN__  //
    ImGui_ImplOpenGL3_Init(this->m_glsl_version);


    return;
}




// *************************************************************************** //
//
//
//  1B.     SECONDARY INITIALIZATION FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "init_appstate"
//
void App::init_appstate(void)
{
    auto &          m_windows           = this->m_state.m_windows;
    auto &          m_fonts             = this->m_state.m_fonts;
    ImGuiIO &       io                  = ImGui::GetIO(); (void)io;
    ImGuiStyle &    style               = ImGui::GetStyle();
    auto [m_sys_width, m_sys_height]    = utl::GetMonitorDimensions(this->m_glfw_window);



    //  1.  LOAD APPLICATION WINDOW INFORMATION...
    for (std::size_t i = 0; i < static_cast<int>(Window::Count); ++i)
    {
        Window      uuid                    = static_cast<Window>(i);
        m_windows[uuid].render_fn           = [this, uuid]([[maybe_unused]] const char * id, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
        { this->dispatch_window_function(uuid); }; // uuid is now correctly captured per-lambda
    }

       
    //  2.  LOAD "m_primary_windows" VARIABLES...
    auto & windows          = this->m_state.m_windows;
    m_primary_windows       = { std::string( windows[Window::MainApp].uuid      ),
                                std::string( windows[Window::GraphingApp].uuid  )
                            };


    //  3.  LOAD APPLICATION FONTS...
    for (int i = 0; i < static_cast<int>(Font::Count); ++i)
    {
        const auto &    info                = cb::app::APPLICATION_FONT_STYLES[i];
        
    #ifndef CBAPP_DISABLE_CUSTOM_FONTS
        m_fonts[static_cast<Font>(i)]       = io.Fonts->AddFontFromFileTTF(info.path.c_str(), info.size);
    # else
        ImFontConfig    config;
        config.SizePixels                   = info.size;
        m_fonts[static_cast<Font>(i)]       = io.Fonts->AddFontDefault(&config);
    #endif  //  CBAPP_DISABLE_CUSTOM_FONTS  //
    }



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
        //
        //      1.  PRIMARY GUI STRUCTURE...
        case Window::Sidebar:           {
            this->Display_Sidebar_Menu(     w.uuid.c_str(),     nullptr,        w.flags);
            break;
        }
        case Window::Menubar:           {
            this->m_menubar.Begin(          w.uuid.c_str(),     nullptr,        w.flags);
            break;
        }
        case Window::MainApp:           {
            this->Display_Main_Window(      w.uuid.c_str(),     nullptr,        w.flags);
            break;
        }
        case Window::GraphingApp:       {
            this->m_graphing_app.Begin(     w.uuid.c_str(),     nullptr,        w.flags);
            break;
        }
        //
        //
        //
        //      2.  MAIN APPLICATION WINDOWS...
        case Window::StyleEditor:       {
            cb::ShowStyleEditor(            w.uuid.c_str(),     &w.open,        w.flags);
            break;
        }
        case Window::Logs:              {
            cb::ShowExampleAppLog(          w.uuid.c_str(),     &w.open,        w.flags);
            break;
        }
        case Window::Console:           {
            cb::ShowExampleAppConsole(      w.uuid.c_str(),     &w.open,        w.flags);
            break;
        }
        case Window::Metrics:           {
            cb::ShowMetricsWindow(          w.uuid.c_str(),     &w.open,        w.flags);
            break;
        }
        //
        //
        //      3.  TOOLS, DEMOS, MISC WINDOWS, ETC...
        case Window::ImGuiDemo:         {
            cb::ShowImGuiDemoWindow(        w.uuid.c_str(),     &w.open,        w.flags);
            break;
        }
        case Window::ImPlotDemo:        {
            cb::ShowImPlotDemoWindow(       w.uuid.c_str(),     &w.open,        w.flags);
            break;
        }
        case Window::AboutDearImGui:    {
            cb::ShowAboutWindow(            w.uuid.c_str(),     &w.open,        w.flags);
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




// *************************************************************************** //
//
//
//  1C.     TERTIARY INITIALIZATION FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "load"
//
void App::load(void)
{
#ifndef __EMSCRIPTEN__
    auto &          m_windows           = this->m_state.m_windows;
    auto &          m_fonts             = this->m_state.m_fonts;
    ImGuiIO &       io                  = ImGui::GetIO(); (void)io;
    ImGuiStyle &    style               = ImGui::GetStyle();
    
    
    
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



#endif  //  __EMSCRIPTEN__  //
    return;
}


//  "init_asserts"
//
void App::init_asserts(void)
{
    ImGuiIO &               io                  = ImGui::GetIO(); (void)io;
    ImGuiStyle &            style               = ImGui::GetStyle();
    const auto &            m_windows           = this->m_state.m_windows;
    const auto &            m_fonts             = this->m_state.m_fonts;
    const size_t            N_WINDOWS           = static_cast<size_t>(Window::Count);
    

    //  1.  ASSERT THAT ALL FONTS ARE VALID...
    for (int i = 0; i < static_cast<int>(Font::Count); ++i)
        IM_ASSERT( m_fonts[static_cast<Font>(i)] );


    //  2.  ASSERT THAT ALL WINDOW CALLBACK FUNCTIONS ARE VALID...
    for (size_t idx = 0; idx < N_WINDOWS; ++idx) {
        const app::WinInfo & winfo      = m_state.m_windows[ static_cast<Window>(idx) ];
        IM_ASSERT( winfo.render_fn != nullptr && error::ASSERT_INVALID_WINDOW_RENDER_FUNCTIONS );
    }

    //  3.  ASSERT THAT ALL "m_primary_windows" ARE VALID...
    IM_ASSERT( std::find(this->m_primary_windows.begin(), this->m_primary_windows.end(), app::DEF_SIDEBAR_WIN_TITLE)
               == this->m_primary_windows.end()
               && error::ASSERT_INVALID_PRIMARY_WINDOWS );

    return;
}








// *************************************************************************** //
//
//
//  3.      CLEAN-UP  | DEFAULT DESTRUCTOR, "destroy()", ETC...
// *************************************************************************** //
// *************************************************************************** //


//  Destructor.
//
App::~App(void)
{
    this->destroy();
}


//  "destroy"       | protected
//
void App::destroy(void) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    if (this->m_glfw_window) {
        glfwDestroyWindow(this->m_glfw_window);
        this->m_glfw_window = nullptr;
    }
    glfwTerminate();
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

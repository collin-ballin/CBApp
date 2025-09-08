/***********************************************************************************
*
*       *********************************************************************
*       ****               I N I T . C P P  ____  F I L E                ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      May 03, 2025.
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



//  1A.     INITIALIZATION  | DEFAULT CONSTRUCTOR, DESTRUCTOR, ETC...
// *************************************************************************** //
// *************************************************************************** //

//  Default Constructor.
//
App::App(void)
    : m_menubar(S)          , m_controlbar(S)           , m_browser(S)          , m_detview(S)
    , m_counter_app(S)      , m_editor_app(S)           , m_graph_app(S)        , m_mimic_app(S)
#ifdef _CBAPP_DEBUG_WINDOWS
    , m_cb_debugger(S)
#endif  //  _CBAPP_DEBUG_WINDOWS  //
{
    this->install_signal_handlers();
    
    S.m_notes.reserve(5);
    S.m_notes.push_back( std::make_pair(cblib::utl::get_timestamp(), "Beginning initialization ({})") );
    

    glfwSetErrorCallback(utl::glfw_error_callback);         //  1.  SET GLFW CALLBACK & CHECK IF PROPERLY INITIALIZED...
    if ( !glfwInit() ) {
        throw std::runtime_error(cb::error::GLFW_INIT_ERROR);
    }
    
    
    this->S.m_glsl_version = utl::get_glsl_version();       //  2.  DECIDE WHICH GL + GLSL VERSION...
    this->init();                                           //  3.  INVOKE "HELPER" FUNCTION TO COMPLETE INITIALIZATION...
}


//  "init"          | private
//
void App::init(void)
{
    //  1.      CREATE A WINDOW WITH GRAPHICS CONTEXT, CREATE "IMGUI" & "IMPLOT" CONTEXTS, ETC...
    this->CreateContext();



    //  2.      SECOND PART OF INITIALIZATION...            | INTERNAL STUFF...
    this->S.m_main_viewport   = ImGui::GetMainViewport();
    this->init_appstate_pre();
    this->load();
    
    
    
    //  3.      THIRD PART OF INITIALIZATION...             | INITIALIZE DELEGATOR CLASSES
    //          (SOME OF THESE HAVE TO BE DONE **AFTER** WE CREATE IMGUI CONTEXT)...
    //
    //
    this->m_controlbar      .initialize();
    this->m_browser         .initialize();
    this->m_detview         .initialize();
    this->m_counter_app     .initialize();
    this->m_editor_app      .initialize();
    this->m_graph_app       .initialize();
    this->m_mimic_app       .initialize();
#ifdef _CBAPP_DEBUG_WINDOWS
    this->m_cb_debugger     .initialize();
#endif  //  _CBAPP_DEBUG_WINDOWS  //
    //
    //
    this->init_appstate_post();
       
       
       
    //  4.      PERFORM ALL RUNTIME ASSERTION STATEMENTS AND
    //          STATE VALIDATIONS BEFORE APPLICATION BEGINS...
    if ( this->init_asserts() ) {
        S.m_logger.debug( std::format("Initial runtime assertions:      PASSED") );
    }

    return;
}



// *************************************************************************** //
//
//
//  1B.     SECONDARY INITIALIZATION...
// *************************************************************************** //
// *************************************************************************** //

//  "CreateContext"
//
void App::CreateContext(void)
{
    //  0.1.    SET GLFW WINDOW SETTINGS...     [ PRE ].
    glfwWindowHint                  ( GLFW_SCALE_TO_MONITOR,                GLFW_TRUE                       );      //  Honor per‑monitor content scaling.
    glfwWindowHint                  ( GLFW_TRANSPARENT_FRAMEBUFFER,         GLFW_TRUE                       );
    //glfwWindowHint                ( GLFW_COCOA_RETINA_FRAMEBUFFER,        GLFW_TRUE                       );
    
    
    //  0.2.    CREATE A WINDOW WITH GRAPHICS CONTEXT.
    this->S.m_glfw_window = utl::CreateGLFWWindow(this->S.m_window_w, this->S.m_window_h, this->S.m_windows[Window::Host].uuid.c_str(), nullptr, nullptr);
    //
    //      CASE 1  : FAILURE TO CREATE GLFW WINDOW...
    if ( !this->S.m_glfw_window )   { throw std::runtime_error(cb::error::GLFW_WINDOW_INIT_ERROR); }
        
    
    
    //  0.3.    ASSIGN MY OWN USER-POINTER.
#ifdef CBAPP_USE_NEW_GLFW_CALLBACKS


#endif  //  CBAPP_USE_NEW_GLFW_CALLBACKS  //
    
    
    
    //  0.4.    SET GLFW CALLBACKS...           [ POST ].
#ifndef CBAPP_USE_NEW_GLFW_CALLBACKS
//
    glfwSetWindowUserPointer            ( this->S.m_glfw_window,            this                            );      //  Allow callbacks to reach this App instance.     [THIS IS WRONG.  DO NOT READ FROM THIS POINTER IN A CALLBACK].
    glfwSetWindowContentScaleCallback   ( this->S.m_glfw_window,                                                    //  Set callback for system DPI change.
        [](GLFWwindow * win, float xs, float ys)
        {
            if ( auto * self = static_cast<App*>(glfwGetWindowUserPointer(win)) )
                self->OnDpiScaleChanged(xs, ys);    // treat xs==ys; ignore ys
        }
    );
//
# else
//
    app::register_app( this->S.m_glfw_window, this );
    glfwSetWindowContentScaleCallback   ( this->S.m_glfw_window,
        [](GLFWwindow * win, float xs, float ys)
        {
            if ( auto * self = app::lookup_app(win) ) {
                self->OnDpiScaleChanged(xs, ys);
            }
            return;
        }
    );
//
#endif  //  CBAPP_USE_NEW_GLFW_CALLBACKS  //
        
        
        
    //utl::SetGLFWWindowLocation(this->S.m_glfw_window, utl::WindowLocation::Center, cb::app::DEF_ROOT_WINDOW_SCALE);
    utl::SetGLFWWindowSize          ( this->S.m_glfw_window,                cb::app::DEF_ROOT_WINDOW_SCALE  );
    //
    glfwMakeContextCurrent          ( this->S.m_glfw_window                                                 );
    glfwSwapInterval                ( this->S.m_glfw_interval                                               );      //  Enable vsync
    glEnable                        ( GL_BLEND                                                              );
    glBlendFunc                     ( GL_SRC_ALPHA,                         GL_ONE_MINUS_SRC_ALPHA          );



    //  2.  SETUP "Dear ImGui", "ImPlot", etc, CONTEXTS...
    IMGUI_CHECKVERSION();
    if ( !ImGui::CreateContext()    )       { throw std::runtime_error(cb::error::IMGUI_CONTEXT_CREATION_ERROR);    }
    if ( !ImPlot::CreateContext()   )       { throw std::runtime_error(cb::error::IMPLOT_CONTEXT_CREATION_ERROR);   }
        
    
    
    [[maybe_unused]] ImGuiIO &      io          = ImGui::GetIO();
    [[maybe_unused]] ImGuiStyle &   style       = ImGui::GetStyle();

    style.Colors[ImGuiCol_WindowBg].w           = 0.0f;     // host window background
    style.Colors[ImGuiCol_DockingEmptyBg].w     = 0.0f;     // the “empty” parts of your dockspace
    
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)      //  When viewports are enabled we tweak WindowRounding/WindowBg
    {                                                           //  so platform windows can look identical to regular ones.
        style.WindowRounding                = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w   = 1.0f;
    }
    
    
    
    //      3.2.    Setup Platform/Renderer backends.
    ImGui_ImplGlfw_InitForOpenGL    ( this->S.m_glfw_window,                /*install_callbacks=*/true      );
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif      //  __EMSCRIPTEN__  //
    ImGui_ImplOpenGL3_Init          (this->S.m_glsl_version                                                 );



    return;
}




// *************************************************************************** //
//
//
//  1B.     SECONDARY INITIALIZATION FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "init_appstate_pre"
//
void App::init_appstate_pre(void)
{
    [[maybe_unused]] ImGuiIO &      io              = ImGui::GetIO(); (void)io;
    [[maybe_unused]] ImGuiStyle &   style           = ImGui::GetStyle();
    //  auto &                          m_fonts         = this->S.m_fonts;
    std::tie( S.m_system_w, S.m_system_h )          = utl::GetMonitorDimensions(this->S.m_glfw_window);
    //  bool                            good_fonts      = true;

    
    
    //  1.  ADJUST APPLICATION DIMENSIONS TO SUIT MONITOR'S DPI...
    S.m_dpi_scale                                   = utl::GetDPIScaling(this->S.m_glfw_window);
    S.m_dpi_fontscale                               = 1.0f;     //    S.m_dpi_scale;
#ifdef CBAPP_USE_FONTSCALE_DPI
    S.m_dpi_fontscale                               = cblib::math::round_to<3>( utl::GetDPIFontScaling(this->S.m_glfw_window) );
#endif  //  CBAPP_USE_FONTSCALE_DPI  //


    //
    //  style.ScaleAllSizes(this->S.m_dpi_scale);                   //      Apply initial scale to the style *once*.
    //
    
    
    CB_LOG(LogLevel::Debug, "System DPI Scale: {}.  System DPI Fontscale: {}",        S.m_dpi_scale, S.m_dpi_fontscale );


    //  1.  LOAD MISC. APPLICATION SETTINGS...
    //          ...


    //  2.  LOAD APPLICATION WINDOW RENDER CALLBACK FUNCTIONS...
    //
    for (std::size_t i = S.ms_WINDOWS_BEGIN; i < static_cast<int>(Window::Count); ++i)
    {
        Window      uuid                    = static_cast<Window>(i);
        //  m_windows[uuid].render_fn           = [this, uuid]([[maybe_unused]] const char * id, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
        //  { this->dispatch_window_function(uuid); };  // uuid is now correctly captured per-lambda
        
        this->S.m_windows[uuid].render_fn   = this->dispatch_window_function(uuid);   // final target stored
        
        IM_ASSERT( this->S.m_windows[uuid].render_fn && "render_fn bound here" );
    }


    //  4.  LOAD APPLICATION FONTS...
    //
    this->S.RebuildFonts( /*scale=*/ this->S.m_dpi_fontscale );
    //
    //
    //  this->S.init_ui_scaler();
    
    
    
    //      5.      LOADING CUSTOM COLORMAPS...
    S.LoadCustomColorMaps();


    return;
}


//  "init_appstate_post"
//
void App::init_appstate_post(void)
{
    //      6.      INITIALIZE OTHER MEMBERS INSIDE APPSTATE...
    //
    S.m_detview_windows.push_back(      std::addressof( this->m_graph_app.m_detview_window          )   );  //  TODO:   THIS SUCKS.
    S.m_detview_windows.push_back(      std::addressof( this->m_editor_app.m_detview_window         )   );  //          Fix it w/ better forward declarations.
    S.m_detview_windows.push_back(      std::addressof( this->m_counter_app.m_detview_window        )   );
    S.m_detview_windows.push_back(      std::addressof( this->m_mimic_app.get_detview_window()      )   );
    
    
    
    //          6.1.    Make sure these windows cannot LEAVE the DetView Dockspace...
    for (auto & win : S.m_detview_windows)
    {
        if ( win && win->uuid != this->S.m_windows[ Window::MimicApp ].uuid )    {
            win->flags |= ImGuiWindowFlags_NoMove;
        }
    }
    //
    //
    //          6.2.    Set DOCKSPACE ID for DETVIEW Windows...
    for (std::size_t i = S.ms_WINDOWS_BEGIN; i < static_cast<int>(Window::Count); ++i)
    {
        bool                match       = false;
        Window              handle      = static_cast<Window>(i);
        WinInfo &           win         = this->S.m_windows[handle];
        
        
        for (auto & detwin : S.m_detview_windows)       //  CHECK IF THIS WINDOW EXISTS IN THE DETVIEW WINDOWS...
        {
            match       = (win.uuid == detwin->uuid);
            if ( match )  {
                win.dockspace_id    = cblib::maximum_value_of_type<ImGuiID>();
                break;
            }
        }
    }

    return;
}


//  "dispatch_window_function"
//
[[nodiscard]]
App::WinRenderFn App::dispatch_window_function(const Window & uuid)
{
    //  IM_ASSERT( !S.m_running.load(std::memory_order_acquire) && error::ASSERT_DISPATCH_CALLED_DURING_RUNTIME );
    WinRenderFn     render_fn   = nullptr;
    
    
    //  DISPATCH EACH RENDER FUNCTION FOR EACH WINDOW OF THE APPLICATION...
    switch (uuid)
    {
        //
        //      1.1.    PRIMARY GUI STRUCTURE...
        case Window::Dockspace:             {
            render_fn   = [this](const char * n, [[maybe_unused]] bool * o, ImGuiWindowFlags f)
                          { this->ShowDockspace(n, nullptr, f); };
            break;
        }
        case Window::MenuBar:               {
            render_fn   = [this](const char * n, [[maybe_unused]] bool * o, ImGuiWindowFlags f)
                          { this->m_menubar.Begin(n, nullptr, f); };
            break;
        }
        case Window::ControlBar:            {
            render_fn   = [this](const char * n, [[maybe_unused]] bool * o, ImGuiWindowFlags f)
                          { this->m_controlbar.Begin(n, nullptr, f); };
            break;
        }
        case Window::Browser:               {
            render_fn   = [this](const char * n, [[maybe_unused]] bool * o, ImGuiWindowFlags f)
                          { this->m_browser.Begin(n, nullptr, f); };
            break;
        }
        case Window::DetailView:            {
            render_fn   = [this](const char * n, [[maybe_unused]] bool * o, ImGuiWindowFlags f)
                          { this->m_detview.Begin(n, nullptr, f); };
            break;
        }
        case Window::HomeApp:               {
            render_fn   = [this](const char * n, [[maybe_unused]] bool * o, ImGuiWindowFlags f)
                          { this->ShowMainWindow(n, nullptr, f); };
            break;
        }
        //
        //
        //
        //      1.2.    MAIN APPLICATION WINDOWS...
        case Window::CCounterApp:       {
            render_fn   = [this](const char * n, bool * o, ImGuiWindowFlags f)
                          { this->m_counter_app.Begin(n, o, f); };
            break;
        }
        case Window::EditorApp:             {
            render_fn   = [this](const char * n, bool * o, ImGuiWindowFlags f)
                          { this->m_editor_app.Begin(n, o, f); };
            break;
        }
        case Window::GraphApp:              {
            render_fn   = [this](const char * n, bool * o, ImGuiWindowFlags f)
                          { this->m_graph_app.Begin(n, o, f); };
            break;
        }
        case Window::MimicApp:
        {
            render_fn   = [this](const char * n, bool * o, ImGuiWindowFlags f)
                          { this->m_mimic_app.Begin(n, o, f); };
            break;
        }
        //
        //
        //
        //      1.3.    TOOLS, SUBSIDIARY APPLICATION WINDOWS...
        case Window::ImGuiStyleEditor:      {
            render_fn   = [this](const char * n, bool * o, ImGuiWindowFlags f)
                          { this->ShowImGuiStyleEditor(n, o, f); };
            break;
        }
        case Window::ImPlotStyleEditor:     {
            render_fn   = [this](const char * n, bool * o, ImGuiWindowFlags f)
                          { this->ShowImPlotStyleEditor(n, o, f); };
            break;
        }
        //
        case Window::Logs:                  {
            render_fn   = [](const char * n, bool * o, ImGuiWindowFlags f)
                          { cb::ShowExampleAppLog(n, o, f); };
            break;
        }
        case Window::Console:               {
            render_fn   = [](const char * n, bool * o, ImGuiWindowFlags f)
                          { cb::ShowExampleAppConsole(n, o, f); };
            break;
        }
        //
        //
        //
        //      1.4.    *MY* TOOLS...
        case Window::ColorTool:             {
            render_fn   = [this](const char * n, bool * o, ImGuiWindowFlags f)
                          { this->ShowColorTool(n, o, f); };
            break;
        }
        case Window::CustomRendering:       {
            render_fn   = [](const char * n, bool * o, ImGuiWindowFlags f)
                          { cb::ShowCustomRendering(n, o, f); };
            break;
        }
        //
        //
        //
        //      1.5.    DEMOS, ETC...
        case Window::ImGuiDemo:             {
            render_fn   = [this](const char * n, bool * o, ImGuiWindowFlags f)
                          { this->ShowImGuiDemoWindow(n, o, f); };
            break;
        }
        case Window::ImPlotDemo:            {
            render_fn   = [this](const char * n, bool * o, ImGuiWindowFlags f)
                          { this->ShowImPlotDemoWindow(n, o, f); };
            break;
        }
        case Window::AboutMyApp:            {
            render_fn   = [this](const char * n, bool * o, ImGuiWindowFlags f)
                          { this->ShowAboutWindow(n, o, f); };
            break;
        }
        //
        //
        //
        //
        //
        //
        //      2.1.    DEBUG-ONLY TOOLS...
#ifdef CBAPP_ENABLE_DEBUG_WINDOWS
        case Window::CBDebugger:            {
            render_fn   = [this](const char * n, bool * o, ImGuiWindowFlags f)
                          { this->m_cb_debugger.Begin(n, o, f); };
            break;
        }
        case Window::ImGuiMetrics:          {
            render_fn   = [this](const char * n, bool * o, ImGuiWindowFlags f)
                          { this->ShowImGuiMetricsWindow(n, o, f); };
            break;
        }
        case Window::ImPlotMetrics:         {
            render_fn   = [this](const char * n, bool * o, ImGuiWindowFlags f)
                          { this->ShowImPlotMetricsWindow(n, o, f); };
            break;
        }
        case Window::ImGuiIDStackTool:      {
            render_fn   = [this](const char * n, bool * o, ImGuiWindowFlags f)
                          { this->ShowImGuiIDStackTool(n, o, f); };
            break;
        }
        case Window::ImGuiItemPickerTool:   {
            render_fn   = [this](const char * n, bool * o, ImGuiWindowFlags f)
                          { this->ShowImGuiItemPickerTool(n, o, f); };
            break;
        }
#endif      //  CBAPP_ENABLE_DEBUG_WINDOWS  //
        //
        //
        //
        //      3.1.    **EXTRA** WINDOWS...
#if defined(CBAPP_ENABLE_OPTIONAL_WINDOWS)
        case Window::CBDemo:                {
            render_fn   = [this](const char * n, bool * o, ImGuiWindowFlags f)
                          { this->ShowImGuiDemoWindow(n, o, f); };
            break;
        }
#endif  //  CBAPP_ENABLE_OPTIONAL_WINDOWS) || defined(CBAPP_ENABLE_FUNCTIONAL_TESTING  //
        //
        //
            //
            //  ...
            //
        //
        //
        //
        default: {
            IM_ASSERT(true && error::ASSERT_NO_DEFAULT_WINDOW_RENDER_SWITCH_CASE);
            break;
        }
    }
    
    
    
    return render_fn;
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
    [[maybe_unused]] ImGuiIO &      io              = ImGui::GetIO(); (void)io;
    [[maybe_unused]] ImGuiStyle &   style           = ImGui::GetStyle();
    
    
#if defined(CBAPP_DISABLE_INI)
    S.m_logger.debug( std::format("#CBAPP_DISABLE_INI is defined -- Loading from \".ini\" file is disabled") );
    io.IniFilename                      = nullptr;
# else
    io.IniFilename                      = cb::app::INI_FILEPATH;
    if ( utl::LoadIniSettingsFromDisk(cb::app::INI_FILEPATH) ) {
        S.m_logger.debug( std::format("Successfully loaded ImGui \".ini\" from \"{}\"", cb::app::INI_FILEPATH) );
    }
    else
    {
        S.m_logger.warning( std::format("Failure to load \".ini\" info from file \"{}\".  Fall back to default .ini content", cb::app::INI_FILEPATH) );
    }
#endif  //  CBAPP_DISABLE_INI  //
    
    
    io.ConfigFlags                     |= this->S.m_io_flags;     //  2.2 Configure I/O Settings.
    //io.ConfigViewportsNoAutoMerge     = true;
    //io.ConfigViewportsNoTaskBarIcon   = true;
    

#ifdef CBAPP_LOAD_STYLE_FILE
    //  1.  Load ImGui Style...
    if ( utl::LoadImGuiStyleFromDisk(style, cb::app::IMGUI_STYLE_FILEPATH) ) {
        S.m_logger.debug( std::format("Successfully loaded ImGui style from \"{}\"", cb::app::IMGUI_STYLE_FILEPATH) );
    }
    else {
        S.m_logger.warning( std::format("Failure to load ImGui style from \"{}\".  Fall back to default S.SetDarkMode()", cb::app::IMGUI_STYLE_FILEPATH) );
        S.SetDarkMode();
    }
    //
    //  2.  Load ImPlot Style...
    if ( utl::LoadImPlotStyleFromDisk(ImPlot::GetStyle(), cb::app::IMPLOT_STYLE_FILEPATH) ) {
        S.m_logger.debug( std::format("Successfully loaded ImPlot style from \"{}\"", cb::app::IMPLOT_STYLE_FILEPATH) );
    }
    else {
        S.m_logger.warning( std::format("Failure to load ImPlot style from \"{}\".", cb::app::IMPLOT_STYLE_FILEPATH) );
    }
# else
    S.m_logger.debug( std::format("#CBAPP_LOAD_STYLE_FILE is defined -- Loading from external style file is disabled") );
    S.SetDarkMode();
#endif  //  CBAPP_LOAD_STYLE_FILE  //



#endif  //  __EMSCRIPTEN__  //
    return;
}


//  "init_asserts"
//
bool App::init_asserts(void)
{
    [[maybe_unused]] ImGuiIO &      io                  = ImGui::GetIO(); (void)io;
    [[maybe_unused]] ImGuiStyle &   style               = ImGui::GetStyle();
    const auto &                    m_fonts             = this->S.m_fonts;
    const size_t                    N_WINDOWS           = static_cast<size_t>(Window::Count);
    

    //  1.  ASSERT THAT  #define ImDrawIdx unsigned int  IS CORRECTLY USED IN ALL TRANSLATION UNITS...
    IM_ASSERT( sizeof(ImDrawIdx) >= 4               && error::ASSERT_32BIT_IMDRAWIDX );
    
    //  2.  ASSERT THAT  #define IMGUI_USE_WCHAR32  IS CORRECTLY USED IN ALL TRANSLATION UNITS...
    IM_ASSERT( sizeof(ImWchar) == 4                 && error::ASSERT_32BIT_WCHAR );
    
    
    
    //  3.  ASSERT THAT ALL FONTS ARE VALID...
    for (int i = 0; i < static_cast<int>(Font::Count); ++i)
        IM_ASSERT( m_fonts[static_cast<Font>(i)] );

    //  4.  ASSERT THAT ALL WINDOW CALLBACK FUNCTIONS ARE VALID...
    for (std::size_t i = S.ms_WINDOWS_BEGIN; i < N_WINDOWS; ++i) {
        const app::WinInfo & winfo      = S.m_windows[ static_cast<Window>(i) ];
        IM_ASSERT( winfo.render_fn       && error::ASSERT_INVALID_WINDOW_RENDER_FUNCTIONS );
    }

    return true;
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
void App::destroy(void)
{
    S.log_shutdown_info();



    //  1.  REMOVE THIS APP FROM GLFW CALLBACK REGISTRY...
#ifdef CBAPP_USE_NEW_GLFW_CALLBACKS
    app::unregister_app(this->S.m_glfw_window);
#endif  //  CBAPP_USE_NEW_GLFW_CALLBACKS  //
    
    
    
    //  2.  CLEAN-UP CONTEXTS FOR:    (1) OPENGL,  (2) GLFW,  (3) IMPLOT,  (4) IMGUI,    ETC...
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();



    //  3.  TERMINATE OPENGL / GLFW...
    if ( this->S.m_glfw_window )
    {
        glfwDestroyWindow(this->S.m_glfw_window);
        this->S.m_glfw_window = nullptr;
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

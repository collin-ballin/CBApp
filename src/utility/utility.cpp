//
//  utility.cpp
//  CBApp
//
//  Created by Collin Bond on 4/16/25.
//
// *************************************************************************** //
// *************************************************************************** //

#include "utility/utility.h"
#include "utility/constants.h"



namespace cb { namespace utl { //     BEGINNING NAMESPACE "cb" :: "utl"...
// *************************************************************************** //
// *************************************************************************** //



//  1.1     MISC / UTILITY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "glfw_error_callback"
//
void glfw_error_callback(int error, const char * description)
{ fprintf(stderr, "GLFW Error %d: %s\n", error, description); }


//  "get_current_monitor"
//
[[nodiscard]] GLFWmonitor * get_current_monitor(GLFWwindow * window)
{
    int                 wx, wy;
    int                 nmonitors;
    glfwGetWindowPos(window, &wx, &wy);
    GLFWmonitor **      monitors = glfwGetMonitors(&nmonitors);


    for (int i = 0; i < nmonitors; ++i) {
        int mx, my;
        glfwGetMonitorPos(monitors[i], &mx, &my);
        const GLFWvidmode * mode    = glfwGetVideoMode(monitors[i]);
        int                 mw      = mode->width,  mh = mode->height;

        if (wx >= mx && wx < mx + mw && wy >= my && wy < my + mh)
            return monitors[i];
    }

    return glfwGetPrimaryMonitor(); // fallback
}


//  "set_window_scale"
//
void set_window_scale(GLFWwindow * window, const float scale)
{
    GLFWmonitor *           monitor         = get_current_monitor(window);
    const GLFWvidmode *     mode            = glfwGetVideoMode(monitor);
    const int               sys_w           = mode->width;
    const int               sys_h           = mode->height;
    const float             aspect          = static_cast<float>(sys_w) / sys_h;
    int                     new_w           = static_cast<int>(sys_w * scale);
    int                     new_h           = static_cast<int>(new_w / aspect);
    int                     pos_x           = 0;
    int                     pos_y           = 0;


    if (new_h > sys_h) {
        new_h = static_cast<int>(sys_h * scale);
        new_w = static_cast<int>(new_h * aspect);
    }

    int monitor_x, monitor_y;
    glfwGetMonitorPos(monitor, &pos_x, &pos_y);
    glfwSetWindowSize(window, new_w, new_h);
    glfwSetWindowPos(window, pos_x + (sys_w - new_w) / 2, pos_y + (sys_h - new_h) / 2);
                     
    return;
}


//  "set_next_window_geometry"
//
void set_next_window_geometry(GLFWwindow* glfw_window, float pos_x_frac, float pos_y_frac,
                                                       float width_frac, float height_frac)
{
    int glfw_w, glfw_h;
    glfwGetWindowSize(glfw_window, &glfw_w, &glfw_h);

    float window_w = glfw_w * width_frac;
    float window_h = glfw_h * height_frac;
    float window_x = glfw_w * pos_x_frac;
    float window_y = glfw_h * pos_y_frac;

    // Adjust origin: positions in ImGui are relative to top-left of window
    // Clamp position to keep window fully visible
    window_x = std::clamp(window_x, 0.0f, static_cast<float>(glfw_w - window_w));
    window_y = std::clamp(window_y, 0.0f, static_cast<float>(glfw_h - window_h));

    ImGui::SetNextWindowPos(ImVec2(window_x, window_y), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(window_w, window_h), ImGuiCond_Once);
    return;
}



// *************************************************************************** //
//
//
//  1.2     CONTEXT CREATION / INITIALIZATION FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "get_glsl_version"
//
const char * get_glsl_version(void)
{
#if defined(IMGUI_IMPL_OPENGL_ES2)                  //  1.1     GL ES 2.0 + GLSL 100 (WebGL 1.0)
    const char * glsl_version   = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
# elif defined(IMGUI_IMPL_OPENGL_ES3)               //  1.2     GL ES 3.0 + GLSL 300 es (WebGL 2.0)
    const char * glsl_version   = "#version 300 es";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
# elif defined(__APPLE__)                           //  1.3     GL 3.2 + GLSL 150
    const char * glsl_version   = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);      //  ...3.2 + only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);                //  ...Required on Mac
# else                                              //  1.4     GL 3.0 + GLSL 130
    const char * glsl_version   = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif      //  IMGUI_IMPL_OPENGL_ES2  //

    return glsl_version;
}






//  "create_glfw_window_IMPL"
//
GLFWwindow * create_glfw_window_IMPL(int width, int height, const char * title, GLFWmonitor * monitor, GLFWwindow * share)
{ return glfwCreateWindow(width, height, title, monitor, share); }


//  "create_glfw_window"
//
GLFWwindow * create_glfw_window(const float scale, const char * title) {
    assert(scale > 0.01f && scale < 1.0f && "Scale must be greater-than 0.01 and less-than 1.0");
    
    GLFWmonitor *           monitor     = glfwGetPrimaryMonitor();
    const GLFWvidmode *     mode        = glfwGetVideoMode( monitor );
    const int               sys_width   = mode->width,      sys_height = mode->height;
    const float             aspect      = static_cast<float>(sys_width) / sys_height;
    
    int                     width       = static_cast<int>(scale * sys_width);
    int                     height      = static_cast<int>(width / aspect);
    
    
    

    return create_glfw_window_IMPL(width, height, title, nullptr, nullptr);
}








/*
//  "get_glsl_version"
//
const char * get_glsl_version(void)
{
    //  1.  CREATE A WINDOW WITH GRAPHICS CONTEXT...
    GLFWmonitor *           monitor     = glfwGetPrimaryMonitor();
    const GLFWvidmode *     mode        = glfwGetVideoMode( monitor );
    
    
    this->m_window = glfwCreateWindow(cb::utl::ROOT_WIN_DEF_WIDTH, cb::utl::ROOT_WIN_DEF_HEIGHT, cb::utl::ROOT_WIN_DEF_TITLE, nullptr, nullptr);
    if (!this->m_window)
        throw std::runtime_error("Call to glfwInit() returned NULL");
    
    glfwMakeContextCurrent(this->m_window);
    glfwSwapInterval(1);        // Enable vsync




    //  2.  SETUP "Dear ImGui" CONTEXT...
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &       io          = ImGui::GetIO(); (void)io;
#ifdef CBAPP_USE_SWAP_INI
    io.IniFilename            = cb::utl::SWAP_INI_FILEPATH;
    ImGui::LoadIniSettingsFromDisk(cb::utl::SWAP_INI_FILEPATH);
# else
    io.IniFilename            = cb::utl::INI_FILEPATH;
    ImGui::LoadIniSettingsFromDisk(cb::utl::INI_FILEPATH);
#endif      //  CBAPP_USE_SWAP_INI  //
                                                       //  2.1 Load ".ini" File..
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
}*/



// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" NAMESPACE.






// *************************************************************************** //
// *************************************************************************** //
//
//  END.











/*

//  "WORKING VERSION OF "APP::INIT" BEFORE USING UTL MODULE "get_glsl_version()"...
//
void App::init(void)
{
    glfwSetErrorCallback(utl::glfw_error_callback);
    if (!glfwInit())
        throw std::runtime_error("Call to glfwInit() returned NULL");


    //  1.  DECIDE WHICH GL + GLSL VERSION...
#if defined(IMGUI_IMPL_OPENGL_ES2)                  //  1.1     GL ES 2.0 + GLSL 100 (WebGL 1.0)
    this->m_glsl_version   = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
# elif defined(IMGUI_IMPL_OPENGL_ES3)               //  1.2     GL ES 3.0 + GLSL 300 es (WebGL 2.0)
    this->m_glsl_version   = "#version 300 es";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
# elif defined(__APPLE__)                           //  1.3     GL 3.2 + GLSL 150
    this->m_glsl_version   = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);      //  ...3.2 + only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);                //  ...Required on Mac
# else                                              //  1.4     GL 3.0 + GLSL 130
    this->m_glsl_version   = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif      //  IMGUI_IMPL_OPENGL_ES2  //
    


    //  2.  CREATE A WINDOW WITH GRAPHICS CONTEXT...
    GLFWmonitor *           monitor     = glfwGetPrimaryMonitor();
    const GLFWvidmode *     mode        = glfwGetVideoMode( monitor );
    
    
    this->m_window = glfwCreateWindow(cb::utl::ROOT_WIN_DEF_WIDTH, cb::utl::ROOT_WIN_DEF_HEIGHT, cb::utl::ROOT_WIN_DEF_TITLE, nullptr, nullptr);
    if (!this->m_window)
        throw std::runtime_error("Call to glfwInit() returned NULL");
    
    glfwMakeContextCurrent(this->m_window);
    glfwSwapInterval(1);        // Enable vsync




    //  3.  SETUP "Dear ImGui" CONTEXT...
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &       io          = ImGui::GetIO(); (void)io;
    #ifdef CBAPP_USE_SWAP_INI
      io.IniFilename            = cb::utl::SWAP_INI_FILEPATH;
      ImGui::LoadIniSettingsFromDisk(cb::utl::SWAP_INI_FILEPATH);
    # else
      io.IniFilename            = cb::utl::INI_FILEPATH;
      ImGui::LoadIniSettingsFromDisk(cb::utl::INI_FILEPATH);
    #endif      //  CBAPP_USE_SWAP_INI  //
                                                       //  3.1 Load ".ini" File..
    io.ConfigFlags             |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad |      //  3.2 Configure I/O Settings.
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

*/

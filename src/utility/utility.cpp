//
//  utility.cpp
//  CBApp
//
//  Created by Collin Bond on 4/16/25.
//
// *************************************************************************** //
// *************************************************************************** //
#include "utility/utility.h"
#include "utility/_constants.h"
#include "_config.h"



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


//  "HelpMarker"
//
void HelpMarker(const char* desc)    //  Helper to display a little (?) mark which shows a tooltip when hovered.
{                                           //  In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
    
    return;
}




// *************************************************************************** //
//
//
//  1.2     WINDOW SIZE / GEOMETRY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

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


//  "SetGLFWWindowLocation"
//
void SetGLFWWindowLocation(GLFWwindow * win, const WindowLocation loc, const float scale, const GLFWmonitor * monitor)
{
    // 1) Figure out which monitor to use
    GLFWmonitor* m = const_cast<GLFWmonitor*>(
        monitor ? monitor : get_current_monitor(win)
    );

    // 2) Query monitor position & size
    int mx, my;
    glfwGetMonitorPos(m, &mx, &my);
    const GLFWvidmode* mode = glfwGetVideoMode(m);
    int sys_w = mode->width;
    int sys_h = mode->height;

    // 3) Compute new window size & pos
    int w, h, x, y;
    switch (loc)
    {
        case WindowLocation::Center:
            // uniform scale in both dimensions
            w = static_cast<int>(sys_w * scale);
            h = static_cast<int>(sys_h * scale);
            x = mx + (sys_w - w) / 2;
            y = my + (sys_h - h) / 2;
            break;

        case WindowLocation::LeftHalf:
            // width scaled, full height
            w = static_cast<int>(sys_w * scale);
            h = sys_h;
            x = mx;      // align left edge
            y = my;      // align top edge
            break;

        case WindowLocation::RightHalf:
            // width scaled, full height
            w = static_cast<int>(sys_w * scale);
            h = sys_h;
            x = mx + (sys_w - w);  // align right edge
            y = my;                // align top edge
            break;

        case WindowLocation::Fill:
            // occupy the full monitor area, ignore scale
            w = sys_w;
            h = sys_h;
            x = mx;
            y = my;
            break;
    }

    // 4) Apply it
    glfwSetWindowSize(win, w, h);
    glfwSetWindowPos (win, x, y);
}





// *************************************************************************** //
//
//
//  1.3     CONTEXT CREATION / INITIALIZATION FUNCTIONS...
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



// *************************************************************************** //
//
//
//  1.4     MISC I/O FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  1.4A-1      SAVING/WRITING FUNCTIONS...
// *************************************************************************** //

//  "SaveStyleToDisk_IMPL"
//
bool SaveStyleToDisk_IMPL(const ImGuiStyle& style, const std::string& file_path) {
    using       json    = nlohmann::json;
    static_assert(std::is_copy_constructible_v<ImGuiStyle> || std::is_trivially_copyable_v<ImGuiStyle>);
    json        j;
    
    // scalar members
    j["Alpha"]                                      = style.Alpha;
    j["DisabledAlpha"]                              = style.DisabledAlpha;
    j["WindowRounding"]                             = style.WindowRounding;
    j["WindowBorderSize"]                           = style.WindowBorderSize;
    j["WindowBorderHoverPadding"]                   = style.WindowBorderHoverPadding;
    j["ChildRounding"]                              = style.ChildRounding;
    j["ChildBorderSize"]                            = style.ChildBorderSize;
    j["PopupRounding"]                              = style.PopupRounding;
    j["PopupBorderSize"]                            = style.PopupBorderSize;
    j["FrameRounding"]                              = style.FrameRounding;
    j["FrameBorderSize"]                            = style.FrameBorderSize;
    j["ItemSpacingX"]                               = style.ItemSpacing.x;
    j["ItemSpacingY"]                               = style.ItemSpacing.y;
    j["ItemInnerSpacingX"]                          = style.ItemInnerSpacing.x;
    j["ItemInnerSpacingY"]                          = style.ItemInnerSpacing.y;
    j["IndentSpacing"]                              = style.IndentSpacing;
    j["ColumnsMinSpacing"]                          = style.ColumnsMinSpacing;
    j["ScrollbarSize"]                              = style.ScrollbarSize;
    j["ScrollbarRounding"]                          = style.ScrollbarRounding;
    j["GrabMinSize"]                                = style.GrabMinSize;
    j["GrabRounding"]                               = style.GrabRounding;
    j["TabRounding"]                                = style.TabRounding;
    j["TabBorderSize"]                              = style.TabBorderSize;
    j["TabCloseButtonMinWidthSelected"]             = style.TabCloseButtonMinWidthSelected;
    j["TabCloseButtonMinWidthUnselected"]           = style.TabCloseButtonMinWidthUnselected;
    j["TabBarBorderSize"]                           = style.TabBarBorderSize;
    j["TabBarOverlineSize"]                         = style.TabBarOverlineSize;
    j["DockingSeparatorSize"]                       = style.DockingSeparatorSize;
    j["MouseCursorScale"]                           = style.MouseCursorScale;
    j["CurveTessellationTol"]                       = style.CurveTessellationTol;
    j["CircleTessellationMaxError"]                 = style.CircleTessellationMaxError;
    j["TreeLinesSize"]                              = style.TreeLinesSize;
    
    // vector members
    j["WindowPadding"]                              = { style.WindowPadding.x, style.WindowPadding.y };
    j["WindowMinSize"]                              = { style.WindowMinSize.x, style.WindowMinSize.y };
    j["WindowTitleAlign"]                           = { style.WindowTitleAlign.x, style.WindowTitleAlign.y };
    j["FramePadding"]                               = { style.FramePadding.x, style.FramePadding.y };
    j["CellPadding"]                                = { style.CellPadding.x, style.CellPadding.y };
    j["TouchExtraPadding"]                          = { style.TouchExtraPadding.x, style.TouchExtraPadding.y };
    j["DisplayWindowPadding"]                       = { style.DisplayWindowPadding.x, style.DisplayWindowPadding.y };
    j["DisplaySafeAreaPadding"]                     = { style.DisplaySafeAreaPadding.x, style.DisplaySafeAreaPadding.y };
    
    //  colors
    for (int i = 0; i < ImGuiCol_COUNT; ++i) {
        const ImVec4& c = style.Colors[i];
        j["Colors"][i] = { c.x, c.y, c.z, c.w };
    }
    
    //  behaviors
    j["AntiAliasedLines"]                           = style.AntiAliasedLines;
    j["AntiAliasedLinesUseTex"]                     = style.AntiAliasedLinesUseTex;
    j["AntiAliasedFill"]                            = style.AntiAliasedFill;

    // write file
    std::ofstream ofs(file_path);
    if (!ofs) return false;
    ofs << j.dump(4);
    
    return true;
}

//  "SaveStyleToDisk"
bool SaveStyleToDisk(const ImGuiStyle & style, const char * file_path)
    { return SaveStyleToDisk_IMPL(style, file_path); }

//  "SaveStyleToDisk"
bool SaveStyleToDisk(const ImGuiStyle & style, const std::string & file_path)
    { return SaveStyleToDisk_IMPL(style, file_path.c_str()); }

//  "SaveStyleToDisk"
bool SaveStyleToDisk(const ImGuiStyle & style, const std::string_view & file_path)
    { return SaveStyleToDisk_IMPL(style, std::string( file_path ).c_str()); }



//  1.4A-2      ASYNCHRONUC SAVING/WRITING FUNCTIONS...
// *************************************************************************** //

//  "SaveStyleToDiskAsync"
bool SaveStyleToDiskAsync(const ImGuiStyle & style, const char * file_path) {
    //  ** CRITICAL **  Create a local, deep-copy of "style" object to prevent race condition if the main thread alters
    bool                status  =   false;      //  the "style" data while the worker thread is writing it's data to the file.
    const ImGuiStyle    copy(style);            //      Note:   We could also accomplish this by **PASSING THE ARG BY VALUE**.
    std::thread([&status, &copy, &file_path]() {
        status = SaveStyleToDisk_IMPL(copy, file_path);
    }).detach();
    
    return status;
}

//  "SaveStyleToDiskAsync"
bool SaveStyleToDiskAsync(ImGuiStyle style, const char * file_path) {
    bool  status = false;
    std::thread([&status, &style, &file_path]()     { status = SaveStyleToDisk_IMPL(style, file_path); }).detach();
    return status;
}



//  1.4B-1      LOADING FUNCTIONS...
// *************************************************************************** //

//  "LoadStyleFromDisk_IMPL"
//
bool LoadStyleFromDisk_IMPL(ImGuiStyle & style, const char * file_path)
{
    using               json    = nlohmann::json;
    std::ifstream       ifs(file_path);
    
    if (!ifs)
        return false;
        
    json                j;
    ifs >> j;


    //  helpers to pull with default
    auto getf = [&](auto& dst, const char* key) {
        using T = std::remove_reference_t<decltype(dst)>;
        if (j.contains(key))
            dst = j[key].get<T>();
    };
    auto get2 = [&](ImVec2& v, const char* key){
        if (j.contains(key) && j[key].is_array() && j[key].size()==2) {
            v.x = j[key][0].get<float>();
            v.y = j[key][1].get<float>();
        }
    };
    auto getcol = [&](int i){
        if (j.contains("Colors") && j["Colors"].is_array() && j["Colors"].size()>i) {
            auto& arr = j["Colors"][i];
            style.Colors[i] = ImVec4(arr[0].get<float>(),
                                     arr[1].get<float>(),
                                     arr[2].get<float>(),
                                     arr[3].get<float>());
        }
    };

    getf(style.Alpha,                       "Alpha");
    getf(style.DisabledAlpha,               "DisabledAlpha");
    getf(style.WindowRounding,              "WindowRounding");
    getf(style.WindowBorderSize,            "WindowBorderSize");
    getf(style.ChildRounding,               "ChildRounding");
    getf(style.ChildBorderSize,             "ChildBorderSize");
    getf(style.PopupRounding,               "PopupRounding");
    getf(style.PopupBorderSize,             "PopupBorderSize");
    getf(style.FrameRounding,               "FrameRounding");
    getf(style.FrameBorderSize,             "FrameBorderSize");
    getf(style.ItemSpacing.x,               "ItemSpacingX");
    getf(style.ItemSpacing.y,               "ItemSpacingY");
    getf(style.ItemInnerSpacing.x,          "ItemInnerSpacingX");
    getf(style.ItemInnerSpacing.y,          "ItemInnerSpacingY");
    getf(style.IndentSpacing,               "IndentSpacing");
    getf(style.ColumnsMinSpacing,           "ColumnsMinSpacing");
    getf(style.ScrollbarSize,               "ScrollbarSize");
    getf(style.ScrollbarRounding,           "ScrollbarRounding");
    getf(style.GrabMinSize,                 "GrabMinSize");
    getf(style.GrabRounding,                "GrabRounding");
    getf(style.TabRounding,                 "TabRounding");
    getf(style.TabBorderSize,               "TabBorderSize");
    getf(style.TabBarBorderSize,            "TabBarBorderSize");
    getf(style.TabBarOverlineSize,          "TabBarOverlineSize");
    getf(style.DockingSeparatorSize,        "DockingSeparatorSize");
    getf(style.MouseCursorScale,            "MouseCursorScale");
    getf(style.CurveTessellationTol,        "CurveTessellationTol");
    getf(style.CircleTessellationMaxError,  "CircleTessellationMaxError");
    getf(style.TreeLinesSize,               "TreeLinesSize");
    
    //  vectors
    get2(style.WindowPadding,               "WindowPadding");
    get2(style.WindowMinSize,               "WindowMinSize");
    get2(style.WindowTitleAlign,            "WindowTitleAlign");
    get2(style.FramePadding,                "FramePadding");
    get2(style.CellPadding,                 "CellPadding");
    get2(style.TouchExtraPadding,           "TouchExtraPadding");
    get2(style.DisplayWindowPadding,        "DisplayWindowPadding");
    get2(style.DisplaySafeAreaPadding,      "DisplaySafeAreaPadding");
    
    //  colors
    for (int i = 0; i < ImGuiCol_COUNT; ++i)
        getcol(i);
        
    //  behaviors
    getf(style.AntiAliasedLines,            "AntiAliasedLines");
    getf(style.AntiAliasedLinesUseTex,      "AntiAliasedLinesUseTex");
    getf(style.AntiAliasedFill,             "AntiAliasedFill");

    return true;
}


//  "LoadStyleFromDisk"
bool LoadStyleFromDisk(ImGuiStyle & style, const char * file_path)
    { return LoadStyleFromDisk_IMPL(style, file_path); }
    
//  "LoadStyleFromDisk"
bool LoadStyleFromDisk(ImGuiStyle & style, const std::string & file_path)
    { return LoadStyleFromDisk_IMPL(style, file_path.c_str() ); }

//  "LoadStyleFromDisk"
bool LoadStyleFromDisk(ImGuiStyle & style, const std::string_view & file_path)
    { return LoadStyleFromDisk_IMPL(style, std::string( file_path ).c_str() ); }






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























/*

//  "WORKING VERSION OF "utl::INIT" BEFORE USING UTL MODULE "get_glsl_version()"...
//
void utl::init(void)
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

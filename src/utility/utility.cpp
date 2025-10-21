/***********************************************************************************
*
*       ********************************************************************
*       ****            U T I L I T Y . C P P  ____  F I L E            ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond.
*               DATED:      April 16, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "utility/utility.h"
#include "utility/_constants.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#if defined(IMGUI_IMPL_OPENGL_ES2)
# include <GLES2/gl2.h>
#endif      //  IMGUI_IMPL_OPENGL_ES2  //
#include <GLFW/glfw3.h>     //  <======| Will drag system OpenGL headers



namespace cb { namespace utl { //     BEGINNING NAMESPACE "cb" :: "utl"...
// *************************************************************************** //
// *************************************************************************** //



//  1.1     MISC / UTILITY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "get_opengl_version"
//
const char * get_opengl_version(void)
{  return reinterpret_cast<const char*>( glGetString(GL_VERSION) );  }



//  "get_glfw_version"
//
int get_glfw_version(char * buffer)
{
    constexpr size_t    BUFFER_SIZE     = 32;
    int                 maj             = 0,    min = 0,    rev = 0;
    if (!buffer)    return -1;

    
    glfwGetVersion(&maj, &min, &rev);
    return std::snprintf(buffer, BUFFER_SIZE, "%d.%d.%d", maj, min, rev);;
}



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
//  1.2     WINDOW / GLFW FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "glfw_error_callback"
//
void glfw_error_callback(int error, const char * description)
{ fprintf(stderr, "GLFW Error %d: %s\n", error, description); }


//  "CreateGLFWWindow"
//
[[nodiscard]] GLFWwindow * CreateGLFWWindow(int width, int height, const char * title, GLFWmonitor * monitor, GLFWwindow * share)
{
    return glfwCreateWindow(width, height, title, monitor, share);
}



// *************************************************************************** //
//
//
//  1.3     WINDOW SIZE / GEOMETRY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "GetMonitorDimensions"
//
[[nodiscard]] std::tuple<int,int> GetMonitorDimensions(GLFWwindow * window) {
    GLFWmonitor *           monitor     = cb::utl::get_current_monitor(window);
    const GLFWvidmode *     mode        = glfwGetVideoMode(monitor);
    return std::make_tuple(mode->width,mode->height);
}


//  "GetActiveMonitorBounds"
//
[[nodiscard]] ImRect GetActiveMonitorBounds(GLFWwindow * window)
{
    GLFWmonitor **      monitors            = nullptr;
    int                 monitor_count       = -1;
    ImVec2              center              = ImVec2(-1.0f, -1.0f);
    int                 win_x{-1},  win_y{-1},  win_w{-1},  win_h{-1};
    int                 mx{-1},     my{-1},     mw{-1},     mh{-1};
    
    
    
    glfwGetWindowPos(   window,     &win_x,     &win_y );
    glfwGetWindowSize(  window,     &win_w,     &win_h );
    center                                  = { win_x + win_w * 0.5f, win_y + win_h * 0.5f };
    monitors                                = glfwGetMonitors(&monitor_count);


    //  CASE 1 :    ...
    for (int i = 0; i < monitor_count; ++i)
    {
        glfwGetMonitorWorkarea( monitors[i], &mx, &my, &mw, &mh );
        if ( center.x >= mx && center.x < mx + mw &&
             center.y >= my && center.y < my + mh ) {
            return ImRect( ImVec2((float)mx,        (float)my),
                           ImVec2((float)(mx + mw), (float)(my + mh)) );
        }
    }
    //
    //  CASE 2 :    FALL-BACK TO FIRST MONITOR...
    glfwGetMonitorWorkarea(monitors[0], &mx, &my, &mw, &mh);
    
    return ImRect( ImVec2((float)mx,            (float)my),
                   ImVec2((float)(mx + mw),     (float)(my + mh)) );
                  
}


//  "GetDPIScaling"
//
[[nodiscard]] float GetDPIScaling(GLFWwindow * window) {
    //auto    ease    = [](float x) -> float { return 2.0f / (1.0f + std::exp(-1.0f * (x - 1.0f))); };
    // return ease( std::max(xscale, yscale) );
    float   xscale  = 1.0f,     yscale  = 1.0f;
    glfwGetWindowContentScale(window, &xscale, &yscale);
    return std::max(xscale, yscale);
}


//  "GetDPIFontScaling"
//
[[nodiscard]] float GetDPIFontScaling(GLFWwindow * window)
{
    constexpr float         N           = 2.6f;
    constexpr float         A           = 1.6f;
    constexpr float         B           = 0.86f;    //  BETWEEN:    0.78f   AND     0.86f;
    constexpr float         C           = 1.0f;
    constexpr float         D           = 1.0f;
    //
    //auto        ease        = [](float x) -> float { return 2.0f / (1.0f + std::exp(-1.5f * (x - 1.0f))); };      s(x) = (N) / ( D + A ( e^{ -B (x-C) } ) )
    auto                    ease        = [&](float x) -> float {
        const float     term        = std::exp( (-1.0f * B) * (x - C) );
        const float     denom       = D + ( A * term );
        return (N / denom);
    };
    //
    //
    float       xscale      = 1.0f,     yscale  = 1.0f,     fontscale = 1.0f;
    
    
    glfwGetWindowContentScale(window, &xscale, &yscale);
    fontscale               = ease( std::max(xscale, yscale) );
    return fontscale;
}


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


//  "SetGLFWWindowSize"
//
void SetGLFWWindowSize(GLFWwindow * window, const float scale)
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
        
        //  DEFAULT...
        default: {
            return;
        }
    }

    // 4) Apply it
    glfwSetWindowSize(win, w, h);
    glfwSetWindowPos (win, x, y);
    return;
}



//  "GetImGuiWindowCoords"
//
ImVec2 GetImGuiWindowCoords(const char * uuid, const Anchor & anchor)
{
    auto *       window      = ImGui::FindWindowByName(uuid);
    if (!window)    return ImVec2(0, 0);

    const ImVec2        pos         = window->Pos;
    const ImVec2        size        = window->Size;
    ImVec2              coord       = ImVec2(0, 0);
    switch (anchor)
    {
        case Anchor::Center:    {
            coord = ImVec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f);
            break;
        }
        case Anchor::East: {
            coord = ImVec2(pos.x + size.x, pos.y + size.y * 0.5f);
            break;
        }
        case Anchor::NorthEast: {
            coord = ImVec2(pos.x + size.x, pos.y);
            break;
        }
        case Anchor::North: {
            coord = ImVec2(pos.x + size.x * 0.5f, pos.y);
            break;
        }
        case Anchor::NorthWest: {
            coord = ImVec2(pos.x, pos.y);
            break;
        }
        case Anchor::West: {
            coord = ImVec2(pos.x, pos.y + size.y * 0.5f);
            break;
        }
        case Anchor::SouthWest: {
            coord = ImVec2(pos.x, pos.y + size.y);
            break;
        }
        case Anchor::South: {
            coord = ImVec2(pos.x + size.x * 0.5f, pos.y + size.y);
            break;
        }
        case Anchor::SouthEast: {
            coord = ImVec2(pos.x + size.x, pos.y + size.y);
            break;
        }
        
        //  DEFAULT...
        default: {
            coord = ImVec2(0, 0);
            break;
        }
    }

    return coord;
}



// *************************************************************************** //
//
//
//  1.4     WIDGET FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //






//          1.4B     POP-UP WINDOWS / FILE DIALOGUE WINDOWS...
// *************************************************************************** //
// *************************************************************************** //

//  "Popup_Save"
//
bool Popup_Save(const char * uuid) {

    if (ImGui::BeginPopupModal(uuid, NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        //  TOP - BAR / PROMPT...
        ImGui::Text("Here is a placeholder POP-UP window for saving the current application data.");
        //
        //
        ImGui::NewLine();
        ImGui::Text("Eventually, this will be a FILE DIALOGUE window that is native to the current Operating System.");
        ImGui::Text("For now, however, this will remain a Dear ImGui MODAL POP-UP Window...");
        const float width           = ImGui::GetItemRectSize().x;
        const float line_height     = ImGui::GetTextLineHeightWithSpacing();
        
        ImGui::Dummy( ImVec2( 0, 2 * line_height) );
        
        
        //  BOTTOM BAR...
        ImGui::Separator();
        ImGui::Dummy( ImVec2( 0, 0.5 * line_height ) );

        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        
        
        ImGui::SameLine();
        ImGui::Dummy( ImVec2( 0.70 * width, 0) );       ImGui::SameLine();
        //
        ImGui::SetItemDefaultFocus();
        if (ImGui::Button("Save", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::Dummy( ImVec2( 0, 0.5 * line_height) );


        ImGui::EndPopup();
    }// END POP-UP.
    
    return true;
}



//  "Popup_AskOkCancel"
//
bool Popup_AskOkCancel(const char * uuid) {

    if (ImGui::BeginPopupModal(uuid, NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("All those beautiful files will be deleted.\nThis operation cannot be undone!");
        ImGui::Separator();

        //static int unused_i = 0;
        //ImGui::Combo("Combo", &unused_i, "Delete\0Delete harder\0");

        static bool dont_ask_me_next_time = false;
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
        ImGui::PopStyleVar();

        if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }
    
    return true;
}


//  "file_exists"
//
bool file_exists(const char * path) {
    namespace fs = std::filesystem;
    fs::path p{path};
    return fs::exists(p) && fs::is_regular_file(p);
}



// *************************************************************************** //
//
//
//          1.4C     MISC. WIDGET FUNCTIONS [CONTD.] ...
// *************************************************************************** //
// *************************************************************************** //







// *************************************************************************** //
//
//
//          1.5     CONTEXT CREATION / INITIALIZATION FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "get_glsl_version"
//      [ WARNING ] :   THIS FUNCTION  *MUST*  BE CALLED  *BEFORE*  CALLING "glfwCreateWindow(...);" !!!
//
[[nodiscard]] const char * get_glsl_version(void) noexcept
{
#if defined(IMGUI_IMPL_OPENGL_ES2)                  //  1.1     GL ES 2.0 + GLSL 100 (WebGL 1.0)
    constexpr const char * glsl_version   = "#version 100";
//  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
//  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
//  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
# elif defined(IMGUI_IMPL_OPENGL_ES3)               //  1.2     GL ES 3.0 + GLSL 300 es (WebGL 2.0)
    constexpr const char * glsl_version   = "#version 300 es";
//  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
//  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
# elif defined(__APPLE__)                           //  1.3     GL 3.2 + GLSL 150
    constexpr const char * glsl_version   = "#version 150";
//  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
//  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);      //  ...3.2 + only
//  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);                //  ...Required on Mac
# else                                              //  1.4     GL 3.0 + GLSL 130
    constexpr const char * glsl_version   = "#version 130";
//  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif      //  IMGUI_IMPL_OPENGL_ES2  //

    return glsl_version;
}


//  "glfwApplyPlatformWindowHints"
//      [ WARNING ] :   THIS FUNCTION  *MUST*  BE CALLED  *BEFORE*  CALLING "glfwCreateWindow(...);" !!!
//
void glfwApplyPlatformWindowHints(void) noexcept
{
//  glfwDefaultWindowHints();      //  reset to defaults first
    
    
#if defined(IMGUI_IMPL_OPENGL_ES2)          //  1.      GL ES 2.0 + GLSL 100 (WebGL 1.0)        | glsl_version = "#version 100";
//
    glfwWindowHint      ( GLFW_CONTEXT_VERSION_MAJOR            , 2                         );
    glfwWindowHint      ( GLFW_CONTEXT_VERSION_MINOR            , 0                         );
    glfwWindowHint      ( GLFW_CLIENT_API                       , GLFW_OPENGL_ES_API        );
//
//
# elif defined(IMGUI_IMPL_OPENGL_ES3)       //  2.      GL ES 3.0 + GLSL 300 es (WebGL 2.0)     | glsl_version = "#version 300 es";
//
    glfwWindowHint      ( GLFW_CONTEXT_VERSION_MAJOR            , 3                         );
    glfwWindowHint      ( GLFW_CONTEXT_VERSION_MINOR            , 0                         );
    glfwWindowHint      ( GLFW_CLIENT_API                       , GLFW_OPENGL_ES_API        );
//
//
# elif defined(__APPLE__)                   //  3.      GL 3.2 + GLSL 150                       | glsl_version = "#version 150";
//
    glfwWindowHint      ( GLFW_CONTEXT_VERSION_MAJOR            , 3                         );
    glfwWindowHint      ( GLFW_CONTEXT_VERSION_MINOR            , 2                         );
    glfwWindowHint      ( GLFW_OPENGL_PROFILE                   , GLFW_OPENGL_CORE_PROFILE  );              //  3.2 + only.
    glfwWindowHint      ( GLFW_OPENGL_FORWARD_COMPAT            , GL_TRUE);                                 //  Required on macOS.
//
//
# else                                      //  4.      GL 3.0 + GLSL 130                       | glsl_version = "#version 130";
//
    glfwWindowHint      ( GLFW_CONTEXT_VERSION_MAJOR            , 3                         );
    glfwWindowHint      ( GLFW_CONTEXT_VERSION_MINOR            , 0                         );
//
//
#endif      //  IMGUI_IMPL_OPENGL_ES2  //

    return;
}


//  "glfwApplySecondaryWindowHints"
//
void glfwApplySecondaryWindowHints(void) noexcept
{
#if !defined( IMGUI_IMPL_OPENGL_ES2 )  &&  !defined( IMGUI_IMPL_OPENGL_ES3 )
//
//
//
    glfwWindowHint                  ( GLFW_SCALE_TO_MONITOR                 , GLFW_TRUE                     );      //  Honor per‑monitor content scaling.
    glfwWindowHint                  ( GLFW_TRANSPARENT_FRAMEBUFFER          , GLFW_TRUE                     );
    //glfwWindowHint                ( GLFW_COCOA_RETINA_FRAMEBUFFER         , GLFW_TRUE                     );
//
//
//
#endif  //  IMGUI_IMPL_OPENGL_ES2  &&  IMGUI_IMPL_OPENGL_ES3  //
    return;
}







//  "create_glfw_window_IMPL"
//
GLFWwindow * create_glfw_window_IMPL(int width, int height, const char * title, GLFWmonitor * monitor, GLFWwindow * share)
{
    return glfwCreateWindow(width, height, title, monitor, share);
}


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
//  1.6     MISC I/O FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

namespace StyleJson {
    // Convert ImU32 to "RRGGBBAA"
    inline std::string to_hex(ImU32 col) {
        std::ostringstream oss;
        oss << std::uppercase << std::hex << std::setw(8) << std::setfill('0') << col;
        return oss.str();
    }

    // Parse "RRGGBBAA" or "#RRGGBBAA" string or unsigned int to ImU32
    inline ImU32 from_json(const nlohmann::json &jval) {
        if (jval.is_string()) {
            std::string s = jval.get<std::string>();
            if (!s.empty() && s[0] == '#') s.erase(0, 1);
            return static_cast<ImU32>(std::stoul(s, nullptr, 16));
        }
        if (jval.is_number_unsigned()) {
            return jval.get<ImU32>();
        }
        return 0u;
    }
}



// *************************************************************************** //
//
//
//
//  1.6A-1      SAVING/WRITING FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "SaveImGuiStyleToDisk_IMPL"
//
bool SaveImGuiStyleToDisk_IMPL(const ImGuiStyle &style, const std::string &file_path) {
    using json = nlohmann::json;
    json j;

    // --- scalars ---
#define J_SCALAR(name) j[#name] = style.name
    J_SCALAR(Alpha);
    J_SCALAR(DisabledAlpha);
    J_SCALAR(WindowRounding);
    J_SCALAR(WindowBorderSize);
    J_SCALAR(WindowBorderHoverPadding);
    J_SCALAR(ChildRounding);
    J_SCALAR(ChildBorderSize);
    J_SCALAR(PopupRounding);
    J_SCALAR(PopupBorderSize);
    J_SCALAR(FrameRounding);
    J_SCALAR(FrameBorderSize);
    j["ItemSpacingX"] = style.ItemSpacing.x;
    j["ItemSpacingY"] = style.ItemSpacing.y;
    j["ItemInnerSpacingX"] = style.ItemInnerSpacing.x;
    j["ItemInnerSpacingY"] = style.ItemInnerSpacing.y;
    J_SCALAR(IndentSpacing);
    J_SCALAR(ColumnsMinSpacing);
    J_SCALAR(ScrollbarSize);
    J_SCALAR(ScrollbarRounding);
    J_SCALAR(GrabMinSize);
    J_SCALAR(GrabRounding);
    J_SCALAR(TabRounding);
    J_SCALAR(TabBorderSize);
    J_SCALAR(TabCloseButtonMinWidthSelected);
    J_SCALAR(TabCloseButtonMinWidthUnselected);
    J_SCALAR(TabBarBorderSize);
    J_SCALAR(TabBarOverlineSize);
    J_SCALAR(DockingSeparatorSize);
    J_SCALAR(MouseCursorScale);
    J_SCALAR(CurveTessellationTol);
    J_SCALAR(CircleTessellationMaxError);
    J_SCALAR(TreeLinesSize);
#undef J_SCALAR

    // --- vectors ---
    auto vec2 = [](const ImVec2 &v) { return json{v.x, v.y}; };
    j["WindowPadding"] = vec2(style.WindowPadding);
    j["WindowMinSize"] = vec2(style.WindowMinSize);
    j["WindowTitleAlign"] = vec2(style.WindowTitleAlign);
    j["FramePadding"] = vec2(style.FramePadding);
    j["CellPadding"] = vec2(style.CellPadding);
    j["TouchExtraPadding"] = vec2(style.TouchExtraPadding);
    j["DisplayWindowPadding"] = vec2(style.DisplayWindowPadding);
    j["DisplaySafeAreaPadding"] = vec2(style.DisplaySafeAreaPadding);

    // --- colors ---
    j["Colors"] = json::array();
    for (int i = 0; i < ImGuiCol_COUNT; ++i) {
        ImU32 packed = ImGui::ColorConvertFloat4ToU32(style.Colors[i]);
        j["Colors"].push_back(StyleJson::to_hex(packed));
    }

    // --- behaviors ---
    j["AntiAliasedLines"] = style.AntiAliasedLines;
    j["AntiAliasedLinesUseTex"] = style.AntiAliasedLinesUseTex;
    j["AntiAliasedFill"] = style.AntiAliasedFill;

    // Write file
    std::ofstream ofs(file_path);
    if (!ofs) return false;
    ofs << j.dump(4);
    return true;
}

//  "SaveImGuiStyleToDisk"
bool SaveImGuiStyleToDisk(const ImGuiStyle & style, const char * file_path)
    { return SaveImGuiStyleToDisk_IMPL(style, file_path); }

//  "SaveImGuiStyleToDisk"
bool SaveImGuiStyleToDisk(const ImGuiStyle & style, const std::string & file_path)
    { return SaveImGuiStyleToDisk_IMPL(style, file_path.c_str()); }

//  "SaveImGuiStyleToDisk"
bool SaveImGuiStyleToDisk(const ImGuiStyle & style, const std::string_view & file_path)
    { return SaveImGuiStyleToDisk_IMPL(style, std::string( file_path ).c_str()); }



//  1.6A-1      IMPLOT STYLE...
// *************************************************************************** //
// *************************************************************************** //

//  "SaveImPlotStyleToDisk_IMPL"
//
inline bool SaveImPlotStyleToDisk_IMPL(const ImPlotStyle & style, const std::string & file_path)
{
    using json = nlohmann::json;
    json j;

    // -- scalars --------------------------------------------------------------
#define J_SCALAR(name) j[#name] = style.name
    J_SCALAR(LineWeight);
    J_SCALAR(Marker);
    J_SCALAR(MarkerSize);
    J_SCALAR(MarkerWeight);
    J_SCALAR(FillAlpha);
    J_SCALAR(ErrorBarSize);
    J_SCALAR(ErrorBarWeight);
    J_SCALAR(DigitalBitHeight);
    J_SCALAR(DigitalBitGap);
    J_SCALAR(PlotBorderSize);
    J_SCALAR(MinorAlpha);
    //J_SCALAR(ReferenceScale);
    J_SCALAR(Colormap);
    J_SCALAR(UseLocalTime);
    J_SCALAR(UseISO8601);
    J_SCALAR(Use24HourClock);
#undef J_SCALAR

    // -- vec2 helper ----------------------------------------------------------
    auto vec2 = [](const ImVec2& v){ return json{v.x, v.y}; };

    // -- vectors --------------------------------------------------------------
    j["MajorTickLen"]       = vec2(style.MajorTickLen);
    j["MinorTickLen"]       = vec2(style.MinorTickLen);
    j["MajorTickSize"]      = vec2(style.MajorTickSize);
    j["MinorTickSize"]      = vec2(style.MinorTickSize);
    j["MajorGridSize"]      = vec2(style.MajorGridSize);
    j["MinorGridSize"]      = vec2(style.MinorGridSize);
    j["PlotPadding"]        = vec2(style.PlotPadding);
    j["LabelPadding"]       = vec2(style.LabelPadding);
    j["LegendPadding"]      = vec2(style.LegendPadding);
    j["LegendInnerPadding"] = vec2(style.LegendInnerPadding);
    j["LegendSpacing"]      = vec2(style.LegendSpacing);
    j["MousePosPadding"]    = vec2(style.MousePosPadding);
    j["AnnotationPadding"]  = vec2(style.AnnotationPadding);
    j["FitPadding"]         = vec2(style.FitPadding);
    j["PlotDefaultSize"]    = vec2(style.PlotDefaultSize);
    j["PlotMinSize"]        = vec2(style.PlotMinSize);

    // -- enums ----------------------------------------------------------------
    //j["LegendOrientation"] = static_cast<int>(style.LegendOrientation);
    //j["LegendLocation"]    = static_cast<int>(style.LegendLocation);

    // -- colors ---------------------------------------------------------------
    j["Colors"] = json::array();
    for (int i = 0; i < ImPlotCol_COUNT; ++i) {
        ImU32 packed = ImGui::ColorConvertFloat4ToU32(style.Colors[i]);
        j["Colors"].push_back(StyleJson::to_hex(packed));
    }

    // -- write file -----------------------------------------------------------
    std::ofstream ofs(file_path);
    if (!ofs) return false;
    ofs << j.dump(4);
    return true;
}

//  "SaveImPlotStyleToDisk"
bool SaveImPlotStyleToDisk(const ImPlotStyle & style, const char * file_path)
    { return SaveImPlotStyleToDisk_IMPL(style, file_path); }

//  "SaveImPlotStyleToDisk"
bool SaveImPlotStyleToDisk(const ImPlotStyle & style, const std::string & file_path)
    { return SaveImPlotStyleToDisk_IMPL(style, file_path.c_str()); }

//  "SaveImPlotStyleToDisk"
bool SaveImPlotStyleToDisk(const ImPlotStyle & style, const std::string_view & file_path)
    { return SaveImPlotStyleToDisk_IMPL(style, std::string( file_path ).c_str()); }












// *************************************************************************** //
//
//
//
//  1.6A-2      ASYNCHRONUC SAVING/WRITING FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "SaveImGuiStyleToDiskAsync"
bool SaveImGuiStyleToDiskAsync(const ImGuiStyle & style, const char * file_path) {
    //  ** CRITICAL **  Create a local, deep-copy of "style" object to prevent race condition if the main thread alters
    bool                status  =   false;      //  the "style" data while the worker thread is writing it's data to the file.
    const ImGuiStyle    copy(style);            //      Note:   We could also accomplish this by **PASSING THE ARG BY VALUE**.
    std::thread([&status, &copy, &file_path]() {
        status = SaveImGuiStyleToDisk_IMPL(copy, file_path);
    }).detach();

    return status;
}

//  "SaveImGuiStyleToDiskAsync"
//  bool SaveImGuiStyleToDiskAsync(ImGuiStyle style, const char * file_path) {
//      bool  status = false;
//      std::thread([&status, &style, &file_path]()     { status = SaveImGuiStyleToDisk_IMPL(style, file_path); }).detach();
//      return status;
//  }



//  "SaveImPlotStyleToDiskAsync"
bool SaveImPlotStyleToDiskAsync(const ImPlotStyle & style, const char * file_path) {
    //  ** CRITICAL **  Create a local, deep-copy of "style" object to prevent race condition if the main thread alters
    bool                status  =   false;      //  the "style" data while the worker thread is writing it's data to the file.
    const ImPlotStyle   copy(style);            //      Note:   We could also accomplish this by **PASSING THE ARG BY VALUE**.
    std::thread([&status, &copy, &file_path]() {
        status = SaveImPlotStyleToDisk_IMPL(copy, file_path);
    }).detach();
    
    return status;
}

//  "SaveImPlotStyleToDiskAsync"
//  bool SaveImPlotStyleToDiskAsync(ImPlotStyle style, const char * file_path) {
//      bool  status = false;
//      std::thread([&status, &style, &file_path]()     { status = SaveImPlotStyleToDisk_IMPL(style, file_path); }).detach();
//      return status;
//  }






// *************************************************************************** //
//
//
//
//  1.6B-1      LOADING FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "LoadImGuiStyleFromDisk_IMPL"
//
bool LoadImGuiStyleFromDisk_IMPL(ImGuiStyle & style, const char * file_path)
{
    using json = nlohmann::json;
    
    if (!std::filesystem::exists(file_path))    // Return false immediately if path doesn't exist
        return false;
        
    std::ifstream ifs(file_path);
    if (!ifs) return false;
    json j; ifs >> j;

    // --- helpers ---
    auto getf = [&](auto &dst, const char *key) {
        using T = std::remove_reference_t<decltype(dst)>;
        if (j.contains(key)) dst = j[key].get<T>();
    };
    auto get2 = [&](ImVec2 &v, const char *key) {
        if (j.contains(key) && j[key].is_array() && j[key].size() == 2) {
            v.x = j[key][0].get<float>();
            v.y = j[key][1].get<float>();
        }
    };
    auto getcol = [&](int i) {
        if (!j.contains("Colors")) return;
        const auto &arr = j["Colors"];
        if (!arr.is_array() || i >= (int)arr.size()) return;
        const auto &entry = arr[i];
        if (entry.is_string() || entry.is_number_unsigned()) {
            ImU32 packed = StyleJson::from_json(entry);
            style.Colors[i] = ImGui::ColorConvertU32ToFloat4(packed);
        } else if (entry.is_array() && entry.size() == 4) {
            style.Colors[i] = ImVec4(
                entry[0].get<float>(), entry[1].get<float>(),
                entry[2].get<float>(), entry[3].get<float>());
        }
    };

    // --- scalars ---
#define GET_SCALAR(name) getf(style.name, #name)
    GET_SCALAR(Alpha);
    GET_SCALAR(DisabledAlpha);
    GET_SCALAR(WindowRounding);
    GET_SCALAR(WindowBorderSize);
    GET_SCALAR(WindowBorderHoverPadding);
    GET_SCALAR(ChildRounding);
    GET_SCALAR(ChildBorderSize);
    GET_SCALAR(PopupRounding);
    GET_SCALAR(PopupBorderSize);
    GET_SCALAR(FrameRounding);
    GET_SCALAR(FrameBorderSize);
    getf(style.ItemSpacing.x, "ItemSpacingX");
    getf(style.ItemSpacing.y, "ItemSpacingY");
    getf(style.ItemInnerSpacing.x, "ItemInnerSpacingX");
    getf(style.ItemInnerSpacing.y, "ItemInnerSpacingY");
    GET_SCALAR(IndentSpacing);
    GET_SCALAR(ColumnsMinSpacing);
    GET_SCALAR(ScrollbarSize);
    GET_SCALAR(ScrollbarRounding);
    GET_SCALAR(GrabMinSize);
    GET_SCALAR(GrabRounding);
    GET_SCALAR(TabRounding);
    GET_SCALAR(TabBorderSize);
    GET_SCALAR(TabCloseButtonMinWidthSelected);
    GET_SCALAR(TabCloseButtonMinWidthUnselected);
    GET_SCALAR(TabBarBorderSize);
    GET_SCALAR(TabBarOverlineSize);
    GET_SCALAR(DockingSeparatorSize);
    GET_SCALAR(MouseCursorScale);
    GET_SCALAR(CurveTessellationTol);
    GET_SCALAR(CircleTessellationMaxError);
    GET_SCALAR(TreeLinesSize);
#undef GET_SCALAR

    // --- vectors ---
    get2(style.WindowPadding, "WindowPadding");
    get2(style.WindowMinSize, "WindowMinSize");
    get2(style.WindowTitleAlign, "WindowTitleAlign");
    get2(style.FramePadding, "FramePadding");
    get2(style.CellPadding, "CellPadding");
    get2(style.TouchExtraPadding, "TouchExtraPadding");
    get2(style.DisplayWindowPadding, "DisplayWindowPadding");
    get2(style.DisplaySafeAreaPadding, "DisplaySafeAreaPadding");

    // --- colors ---
    for (int i = 0; i < ImGuiCol_COUNT; ++i) getcol(i);

    // --- behaviors ---
    getf(style.AntiAliasedLines,       "AntiAliasedLines");
    getf(style.AntiAliasedLinesUseTex, "AntiAliasedLinesUseTex");
    getf(style.AntiAliasedFill,        "AntiAliasedFill");

    return true;
}

//  "LoadImGuiStyleFromDisk"
bool LoadImGuiStyleFromDisk(ImGuiStyle & style, const char * file_path)
    { return LoadImGuiStyleFromDisk_IMPL(style, file_path); }
    
//  "LoadImGuiStyleFromDisk"
bool LoadImGuiStyleFromDisk(ImGuiStyle & style, const std::string & file_path)
    { return LoadImGuiStyleFromDisk_IMPL(style, file_path.c_str() ); }

//  "LoadImGuiStyleFromDisk"
bool LoadImGuiStyleFromDisk(ImGuiStyle & style, const std::string_view & file_path)
    { return LoadImGuiStyleFromDisk_IMPL(style, std::string( file_path ).c_str() ); }






//  1.6B-1      IMPLOT STYLE...
// *************************************************************************** //
// *************************************************************************** //

//  "LoadImPlotStyleFromDisk_IMPL"
//
inline bool LoadImPlotStyleFromDisk_IMPL(ImPlotStyle & style, const char * file_path)
{
    using json = nlohmann::json;

    if (!std::filesystem::exists(file_path))
        return false;

    std::ifstream ifs(file_path);
    if (!ifs)
        return false;

    json j;
    try { ifs >> j; }
    catch (...) { return false; }

    // -------------------------------------------------------------------------
    // helpers
    // -------------------------------------------------------------------------
    auto get_scalar = [&](auto& dst, const char* key)
    {
        auto it = j.find(key);
        if (it != j.end() && it->is_primitive())          // skip if wrong type
            it->get_to(dst);
    };

    auto get_vec2 = [&](ImVec2& v, const char* key)
    {
        auto it = j.find(key);
        if (it != j.end() && it->is_array() && it->size() == 2) {
            v.x = (*it)[0].get<float>();
            v.y = (*it)[1].get<float>();
        }
    };

    auto get_color = [&](int i)
    {
        auto arr_it = j.find("Colors");
        if (arr_it == j.end() || !arr_it->is_array() || i >= static_cast<int>(arr_it->size()))
            return;

        const json& e = (*arr_it)[i];
        if (e.is_string() || e.is_number_unsigned()) {
            ImU32 packed = StyleJson::from_json(e);
            if (packed)
                style.Colors[i] = ImGui::ColorConvertU32ToFloat4(packed);
        }
        else if (e.is_array() && e.size() == 4) {
            style.Colors[i] = { e[0].get<float>(), e[1].get<float>(),
                                e[2].get<float>(), e[3].get<float>() };
        }
    };

    // -------------------------------------------------------------------------
    // scalars
    // -------------------------------------------------------------------------
#define LOAD_SCALAR(name) get_scalar(style.name, #name)
    LOAD_SCALAR(LineWeight);        LOAD_SCALAR(Marker);           LOAD_SCALAR(MarkerSize);
    LOAD_SCALAR(MarkerWeight);      LOAD_SCALAR(FillAlpha);        LOAD_SCALAR(ErrorBarSize);
    LOAD_SCALAR(ErrorBarWeight);    LOAD_SCALAR(DigitalBitHeight); LOAD_SCALAR(DigitalBitGap);
    LOAD_SCALAR(PlotBorderSize);    LOAD_SCALAR(MinorAlpha);       LOAD_SCALAR(Colormap);
    LOAD_SCALAR(UseLocalTime);      LOAD_SCALAR(UseISO8601);       LOAD_SCALAR(Use24HourClock);
#undef  LOAD_SCALAR

    // -------------------------------------------------------------------------
    // vectors
    // -------------------------------------------------------------------------
    get_vec2(style.MajorTickLen,        "MajorTickLen");
    get_vec2(style.MinorTickLen,        "MinorTickLen");
    get_vec2(style.MajorTickSize,       "MajorTickSize");
    get_vec2(style.MinorTickSize,       "MinorTickSize");
    get_vec2(style.MajorGridSize,       "MajorGridSize");
    get_vec2(style.MinorGridSize,       "MinorGridSize");
    get_vec2(style.PlotPadding,         "PlotPadding");
    get_vec2(style.LabelPadding,        "LabelPadding");
    get_vec2(style.LegendPadding,       "LegendPadding");
    get_vec2(style.LegendInnerPadding,  "LegendInnerPadding");
    get_vec2(style.LegendSpacing,       "LegendSpacing");
    get_vec2(style.MousePosPadding,     "MousePosPadding");
    get_vec2(style.AnnotationPadding,   "AnnotationPadding");
    get_vec2(style.FitPadding,          "FitPadding");
    get_vec2(style.PlotDefaultSize,     "PlotDefaultSize");
    get_vec2(style.PlotMinSize,         "PlotMinSize");

    // -------------------------------------------------------------------------
    // colors
    // -------------------------------------------------------------------------
    for (int i = 0; i < ImPlotCol_COUNT; ++i)
        get_color(i);

    return true;
}

//  "LoadImPlotStyleFromDisk"
bool LoadImPlotStyleFromDisk(ImPlotStyle & style, const char * file_path)
    { return LoadImPlotStyleFromDisk_IMPL(style, file_path); }
    
//  "LoadImPlotStyleFromDisk"
bool LoadImPlotStyleFromDisk(ImPlotStyle & style, const std::string & file_path)
    { return LoadImPlotStyleFromDisk_IMPL(style, file_path.c_str() ); }

//  "LoadImPlotStyleFromDisk"
bool LoadImPlotStyleFromDisk(ImPlotStyle & style, const std::string_view & file_path)
    { return LoadImPlotStyleFromDisk_IMPL(style, std::string( file_path ).c_str() ); }






// *************************************************************************** //
//
//
//
//  1.6B-1      IMGUI ".ini" STYLE...
// *************************************************************************** //
// *************************************************************************** //

//  "LoadIniSettingsFromDisk"
bool LoadIniSettingsFromDisk(const char * ini_filename) {
    size_t      file_data_size      = 0;
    char *      file_data           = (char*)ImFileLoadToMemory(ini_filename, "rb", &file_data_size);
    if (!file_data)
        return false;
        
    if (file_data_size > 0)     ImGui::LoadIniSettingsFromMemory(file_data, (size_t)file_data_size);
    IM_FREE(file_data);
    return true;
}








// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" :: "utl" NAMESPACE.






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

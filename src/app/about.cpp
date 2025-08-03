/***********************************************************************************
*
*       *********************************************************************
*       ****               A B O U T . C P P  ____  F I L E              ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      August 02, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/app.h"
#include "implot.h"
#include "implot_internal.h"
#include <random>
#include <algorithm>



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//  "MAKE_GMAIL_COMPOSE_URL_IMPL"
//
/// @def        MAKE_GMAIL_COMPOSE_URL_FULL(TO, CC, BCC, SUBJECT, BODY)
/// @brief      Helper to build a full Gmail “compose”-style URL with all recipient fields.
/// @note       USAGE:          MAKE_GMAIL_COMPOSE_URL_FULL("friend@example.com", "cc@example.com", "bcc@example.com",      \
///                                                         "Subject%20Of%20Email", "Body%20of%20email...")
///
/// @details    EXPANDS TO:     "https://mail.google.com/mail/?view=cm&fs=1&to=friend@example.com&cc=cc@example.com"        \
///                             "&bcc=bcc@example.com&su=Subject%20Of%20Email&body=Body%20of%20email..."
///
///             REQUIREMENTS:   •   All arguments must be C-string literals and URL-encoded:
///                             •   Use “%20” for spaces
///                             •   Encode any “&”, “=”, “?”, etc., in SUBJECT and BODY
//
#define MAKE_GMAIL_COMPOSE_URL_IMPL(TO, CC, BCC, SUBJECT, BODY)                                 \
    "https://mail.google.com/mail/?view=cm&fs=1"                                                \
    "&to="          TO                                                                          \
    "&cc="          CC                                                                          \
    "&bcc="         BCC                                                                         \
    "&su="          "CBApp " __CBAPP_VERSION__ " (Build "                                       \
                    __CBAPP_BUILD__ ") | "                                                      \
                    SUBJECT                                                                     \
    "&body="        BODY;


//  "MAKE_GMAIL_URL"
//
/// @def        MAKE_GMAIL_URL(TO, SUBJECT, BODY)
/// @brief      Shorthand helper of this macro that removed the CC and BCC arguments.
/// @see        MAKE_GMAIL_COMPOSE_URL_FULL(TO, CC, BCC, SUBJECT, BODY)
//
#define MAKE_GMAIL_URL(TO, SUBJECT, BODY)                            \
    MAKE_GMAIL_COMPOSE_URL_IMPL(TO, "", "", SUBJECT, BODY)
    
    
    
    
    
    
namespace about { //     BEGINNING NAMESPACE "about"...
// *************************************************************************** //
// *************************************************************************** //

/*
//  "AboutRow"
//
struct AboutRow {
    const char* label;        // points to static / literal text
    std::string value;        // non-static values we must build at run time
};


//  "AboutCache"
//
struct AboutCache               // lives only while the window is open
{
    std::vector<AboutRow>       build_settings;
    std::vector<AboutRow>       dep_versions;
    // add more sections here …



    // helper ----------------------------------------------------- //
    static AboutCache create()           // called on first open
    {
        AboutCache c;

        /* --- Build-settings section ---------------------------- //
#       ifdef __CBAPP_DEBUG__                     c.build_settings.push_back({"__CBAPP_DEBUG__", {}});
#endif
#       ifdef __CBLIB_RELEASE_WITH_DEBUG_INFO__   c.build_settings.push_back({"__CBLIB_RELEASE_WITH_DEBUG_INFO__", {}});
#endif
#       ifdef __CBAPP_RELEASE__                   c.build_settings.push_back({"__CBAPP_RELEASE__", {}});
#endif
#       ifdef __CBLIB_MIN_SIZE_RELEASE__          c.build_settings.push_back({"__CBLIB_MIN_SIZE_RELEASE__", {}});
#endif

        // --- Dependency-versions section ----------------------- //
        c.dep_versions = {
            { "Dear ImGui",  fmt::format("{} ({})", IMGUI_VERSION, IMGUI_VERSION_NUM) },
            { "ImPlot",      IMPLOT_VERSION },
            { "OpenGL",      about::OPENGL_VERSION },
            { "GLFW",        fmt::format("{}.{}.{}", GLFW_VERSION_MAJOR,
                                                       GLFW_VERSION_MINOR,
                                                       GLFW_VERSION_REVISION) }
        };
        return c;
    }
};





    //  "render_about_section"
    //
    [[maybe_unused]]
    static void render_about_section(const char* title, const std::vector<AboutRow>& rows)
    {
        static constexpr ImGuiTableFlags    FLAGS       = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit;
        
        
        ImGui::TextUnformatted(title);
        if ( !ImGui::BeginTable(title, 2, FLAGS) )  { return; }
        
        
        //  1.  BEGIN THE TABLE...
        {
            ImGui::TableSetupColumn("Label",    ImGuiTableColumnFlags_WidthStretch,     0.45f);
            ImGui::TableSetupColumn("Value",    ImGuiTableColumnFlags_WidthStretch,     0.55f);
            ImGui::TableHeadersRow();


            for (const auto& r : rows)
            {
                if ( !r.show )          { continue; }                // honour check-box filters
                ImGui::TableNextRow();
                ImGui::TableNextColumn(); ImGui::TextUnformatted(r.label);
                ImGui::TableNextColumn(); ImGui::TextUnformatted(r.value.c_str());
            }
            
            ImGui::EndTable();
        }
        
        
        ImGui::Spacing(); // visual air between sections
        
        
        return;
    }
    
    
*/

    
    

// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "about" NAMESPACE.














namespace about { //     BEGINNING NAMESPACE "about"...
// *************************************************************************** //
// *************************************************************************** //

// *************************************************************************** //
//
//          1.      ENUMS, TYPES, STATIC CONSTEXPR...
// *************************************************************************** //
// *************************************************************************** //

    //  "CopyFormatType"
    enum class CopyFormatType : uint8_t {
        None = 0, Github, COUNT
    };
    
    //  "get_imgui_color_order"
    //      Return "RGBA" (default) or "BGRA" when the build
    //      was compiled with IMGUI_USE_BGRA_PACKED_COLOR.
    //
    [[nodiscard]]
    inline static constexpr const char * get_imgui_color_order(void) noexcept
    {
        if constexpr (IM_COL32_R_SHIFT == 0 && IM_COL32_B_SHIFT == 16)
            { return "RGBA"; }           // packed as 0xAABBGGRR
        else
            { return "BGRA"; }           // packed as 0xAARRGGBB
    }

    //  "get_imgui_packing_format"
    //      Return the exact byte-pattern string used in ImU32.
    //
    [[nodiscard]]
    inline static constexpr const char * get_imgui_packing_format(void) noexcept
    {
        if constexpr (IM_COL32_R_SHIFT == 0 && IM_COL32_B_SHIFT == 16)
            { return "0xAABBGGRR"; }
        else
            { return "0xAARRGGBB"; }
    }



// *************************************************************************** //
//
//
//
//          2.      DATA MEMBERS...
// *************************************************************************** //
// *************************************************************************** //

    //
    //                              STATIC CONSTEXPR:
    static constexpr std::array<const char *, static_cast<size_t>( CopyFormatType::COUNT )>
                                        DEF_FORMAT_TYPE_NAMES       = { "Plain Text", "GitHub" };
    //
    static constexpr const char *       GMAIL_COMPOSE_URL           = MAKE_GMAIL_COMPOSE_URL_IMPL("collin.bond.d@gmail.com",    "",     "",                             "Subject",      "Your App sucks."   )
    static constexpr const char *       EDU_COMPOSE_URL             = MAKE_GMAIL_COMPOSE_URL_IMPL("collin23@pdx.edu",           "",     "collin.bond.d@gmail.com",      "Subject",      "Your App sucks."   )
    //
    static constexpr const char *       IMGUI_COLOR_ORDER           = get_imgui_color_order();
    static constexpr const char *       IMGUI_PACKING_FORMAT        = get_imgui_packing_format();
    //
    //                              CONSTANTS:
    static constexpr unsigned int       NUM_LINES                   = 18;
    static constexpr float              CHILD_SCALE_X               = 0.5f;
    static constexpr float              SUCCESS_MESSAGE_DURATION    = 5.0f;
    //
    //                              VARIABLES:
    static CopyFormatType               copy_format                 = CopyFormatType::Github;
    static bool                         copy_to_clipboard           = false;
    static float                        copy_time                   = -1.0f;
    //
    //
    //                              MUTABLE STATE STUFF:
    static bool                         show_verbose_info           = false;
    //
    static bool                         show_cbapp_info             = true;
    static bool                         show_build_info             = false;
    static bool                         show_config_info            = false;
    static bool                         show_imgui_info             = false;
    //
    //                              MUTABLE DIMENSION STUFF:
    static ImVec2                       BUTTON_SIZE                 = ImVec2(80.0f, -1);
    static ImVec2                       Avail                       = {};
    //
    //                              MUTABLE STRING STUFF:
    static std::string                  OPENGL_VERSION              = {};
    static std::string                  GLSL_VERSION                = {};



// *************************************************************************** //
//
//
//
//          3.      FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    //  "begin_log"
    inline static void begin_log(void)
    {
        ImGui::LogToClipboard();
        switch (about::copy_format) {
            case CopyFormatType::Github     : { ImGui::LogText("```\n"); break; }   //  Back quotes will make text appears without formatting when pasting on GitHub...
            default                         : { break; }
        }
        return;
    }

    //  "finish_log"
    inline static void finish_log(void)
    {
        switch (about::copy_format) {
            case CopyFormatType::Github     : { ImGui::LogText("\n```\n"); break; }
            default                         : { break; }
        }
        ImGui::LogFinish();
        copy_to_clipboard = false;
        return;
    }
    
    //  "first_frame_cache"
    //
    inline static void first_frame_cache(void)
    {
        BUTTON_SIZE                             = ImVec2( 1.8f * ImGui::CalcTextSize("More Info...").x, ImGui::GetFrameHeight() );
        OPENGL_VERSION                          = std::string( reinterpret_cast<const char *>( glGetString(GL_VERSION)                   )  );
        GLSL_VERSION                            = std::string( reinterpret_cast<const char *>( glGetString(GL_SHADING_LANGUAGE_VERSION)  )  );
        
        
        
        //  static constexpr const char *       IMGUI_COLOR_ORDER       = about::get_imgui_color_order();
        //  static constexpr const char *       IMGUI_PACKING_FORMAT    = about::get_imgui_packing_format();
        return;
    }
    
    

// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "about" NAMESPACE.






// *************************************************************************** //
// *************************************************************************** //

//  "ShowAboutWindow"
//
void App::ShowAboutWindow([[maybe_unused]]   const char *        uuid,
                          [[maybe_unused]]   bool *              p_open,
                          [[maybe_unused]]   ImGuiWindowFlags    flags)
{
    using           CopyFormatType          = about::CopyFormatType;
    static bool     first_frame             = true;
    int             copy_format_int         = static_cast<int>(about::copy_format);
    ImVec2          center                  = ImGui::GetMainViewport()->GetCenter();
    
    
    
    if (!*p_open)                        { return; }
    
    
    
    //  OPEN THE MAIN POP-UP "About" WINDOW...
    ImGui::OpenPopup(uuid);
    
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2( 0.5f, 0.5f ));
    if ( ImGui::BeginPopupModal(uuid, p_open, ImGuiWindowFlags_AlwaysAutoResize) )
    {
    
        //      0A.     COMPUTE CACHED INFO ON FIRST-FRAME...
        if (first_frame) [[unlikely]]                                       { about::first_frame_cache(); }
    

        //      0B.     If [ESC], CLOSE WINDOW...
        if ( !first_frame && ImGui::IsKeyPressed( ImGuiKey_Escape ) )       { *p_open = false; ImGui::EndPopup(); return; }     // { *p_open = false; }
        
        
        
        //      2.      "AUTHOR / HEADER"...
        ImGui::Text                     ("CBApp %s (Build %s, WIP)", __CBAPP_VERSION__, __CBAPP_BUILD__);
        ImGui::TextUnformatted          ("Developed by Collin Andrew Bond  (c)  2024-2025");
        //
        //
        //      3.      "CONTACT ME"...
        ImGui::NewLine();
        ImGui::TextUnformatted          ("Contact me:");
        ImGui::Indent();
        //
            ImGui::TextLinkOpenURL      ("collin.bond.d@gmail.com",     about::GMAIL_COMPOSE_URL);      ImGui::SameLine();
            ImGui::TextUnformatted      ("or");                                                         ImGui::SameLine();
            ImGui::TextLinkOpenURL      ("collin23@pdx.edu",            about::EDU_COMPOSE_URL);
        
            ImGui::TextLinkOpenURL      ("Repository",                  "https://github.com/collin-ballin/CBApp");
        //
        ImGui::Unindent();
        //
        //
        //      4.      "MORE INFO..." BUTTON...
        ImGui::NewLine();
        if ( !about::show_verbose_info )
        {
            about::Avail                        = ImGui::GetContentRegionAvail();
            
            ImGui::Dummy( { 0.5f * (about::Avail.x - about::BUTTON_SIZE.x), 0 } );      ImGui::SameLine(0.0f, 0.0f);
            
            if ( ImGui::Button( "More Info...", about::BUTTON_SIZE) )
                { about::show_verbose_info = true; }
            
            
            ImGui::NewLine();
           
        }
        else {
            if ( ImGui::Button("Less Info...", about::BUTTON_SIZE) )
                { about::show_verbose_info = false; }
            
        }
        
        
        
        //      5.      "MORE INFO" TEXTBOX WINDOW...
        if ( about::show_verbose_info )
        {
            ImGui::Separator();
            ImGui::Indent();
            
                ImGui::Checkbox("Show CBApp Information",           &about::show_cbapp_info);
                ImGui::Checkbox("Show Build Information",           &about::show_build_info);
                ImGui::Checkbox("Show Configuration Information",   &about::show_config_info);
                //
                if ( ImGui::Checkbox("Show Default ImGui Information",   &about::show_imgui_info) )
                {
                    about::show_cbapp_info = false;     about::show_build_info = false;     about::show_config_info = false;
                }
                if ( ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal) )
                { ImGui::SetTooltip("Use this option when posting a feature request, bug report, etc, to the Dear ImGui GitHub page."); }
                
                
            ImGui::Unindent();
        
            this->show_about_info();
        }


        if ( about::show_verbose_info )
        {
            ImGui::BeginDisabled(true);
                about::copy_to_clipboard    = false;
                if ( ImGui::Button("Copy-To-Clipboard") ) {
                    about::copy_to_clipboard    = true;
                }
            ImGui::EndDisabled();//  TO-DO:     Copy-To-Clipboard FAILS.
                                 //             The copy acts correctly, but when the user PASTES the data somewhere else, it crashes the application.
        
        
            ImGui::SameLine(0, 25);
            
            ImGui::TextUnformatted("Copy Format:");
            ImGui::SameLine();
            if ( ImGui::Combo("##App_About_CopyFormatType",             &copy_format_int,
                              about::DEF_FORMAT_TYPE_NAMES.data(),      static_cast<int>(CopyFormatType::COUNT)) )
            { about::copy_format  = static_cast<CopyFormatType>(copy_format_int); }
        }



        ImGui::EndPopup();
    }// END POP-UP.
    
    
    
    if (first_frame) [[unlikely]]       { first_frame = false; }
    
    return;
}



// *************************************************************************** //
// *************************************************************************** //










// *************************************************************************** //
// *************************************************************************** //

//  "show_about_info"
//
void App::show_about_info(void) const
{
    [[maybe_unused]] ImGuiIO &          io                  = ImGui::GetIO();
    [[maybe_unused]] ImGuiStyle &       style               = ImGui::GetStyle();
    //
    const ImVec2                        CHILD_SIZE          = ImVec2( about::CHILD_SCALE_X * ImGui::GetMainViewport()->Size.x,
                                                                      about::NUM_LINES * ImGui::GetTextLineHeightWithSpacing() );
    const float                         current_time        = ImGui::GetTime();

    
    //  0.  SHOW TOOLTIP TO INDICATE COPY-TO-CLIPBOARD SUCCESS...
    if ( about::copy_time > 0.0f && current_time - about::copy_time < about::SUCCESS_MESSAGE_DURATION )
    {
        ImGui::SetNextWindowPos(ImGui::GetMousePos());
        ImGui::BeginTooltip();
            ImGui::TextUnformatted("Copied info to clipboard!");
        ImGui::EndTooltip();
    }
    else { about::copy_time    = -1.0f; }








    //  1.  CREATE A CHILD WINDOW TO HOST ALL THE INFORMATION...
    ImGui::BeginChild( ImGui::GetID("cfg_info_1"), CHILD_SIZE, ImGuiChildFlags_FrameStyle );
    S.PushFont(Font::Small);
    //
    //
    //
    //  COPY INFORMATION TO CLIP-BOARD...
        if ( about::copy_to_clipboard ) {
            about::begin_log();
        }
        
        
        //  1.  CBAPP INFORMATION...
        if ( about::show_cbapp_info ) {
            this->get_cbapp_info();
        }
        //
        //  2.  BUILD INFORMATION...
        if ( about::show_build_info ) {
            this->get_build_info();
        }
        //
        //  3.  CONFIGURATION INFORMATION...
        if ( about::show_config_info ) {
            this->get_config_info();
        }
        //
        //  4.  DEFAULT IMGUI INFORMATION...
        if ( about::show_imgui_info ) {
            this->get_imgui_info();
        }
        

        //  END CHILD WINDOW.   COPY INFORMATION TO CLIPBOARD.
        if ( about::copy_to_clipboard ) {
            about::finish_log();
            about::copy_time    = ImGui::GetTime(); 
        }
    //
    //
    //
    S.PopFont();
    ImGui::EndChild();
    
    return;
}


//  "get_cbapp_info"
//
void App::get_cbapp_info(void) const
{

    //      1.      CBAPP INFO...
    ImGui::SeparatorText("CBApp Information...");
    //
    //
        //      1.1.    BUILD SETTINGS...
        ImGui::TextUnformatted("1.  Build Settings:");
        ImGui::Indent();
        //
        //
        #ifdef __CBAPP_DEBUG__
            ImGui::TextUnformatted(    "#define \t __CBAPP_DEBUG__"                             );
        #endif  //  __CBAPP_LOG__  //
        #ifdef __CBLIB_RELEASE_WITH_DEBUG_INFO__
            ImGui::TextUnformatted(    "#define \t __CBLIB_RELEASE_WITH_DEBUG_INFO__"           );
        #endif  //  __CBLIB_RELEASE_WITH_DEBUG_INFO__  //
        #ifdef __CBAPP_RELEASE__
            ImGui::TextUnformatted(    "#define \t __CBAPP_RELEASE__"                           );
        #endif  //  __CBAPP_RELEASE__  //
        #ifdef __CBLIB_MIN_SIZE_RELEASE__
            ImGui::TextUnformatted(    "#define \t __CBLIB_MIN_SIZE_RELEASE__"                  );
        #endif  //  __CBLIB_MIN_SIZE_RELEASE__  //
        //
        //
        ImGui::Unindent();


        //      1.2.    BUILD SETTINGS...
        ImGui::NewLine();
        ImGui::TextUnformatted("2.  Compile-Time Options:");
        ImGui::Indent();
        //
        //
        #ifdef __CBAPP_LOG__
            ImGui::TextUnformatted(    "#define \t __CBAPP_LOG__"                               );
        #endif  //  __CBAPP_LOG__  //
        //
        //
        ImGui::Unindent();
    //
    //
    //  END "CBAPP INFO"...
    
    
    
    //      2.      THIRD-PARTY DEPENDENCIES...
    ImGui::NewLine();
    ImGui::SeparatorText("Third-Party Dependencies...");
    //
    //
        //      2.1.    DEPENDENCY VERSIONS...
        ImGui::TextUnformatted("1.  Dependency Versions:");
        ImGui::Indent();
        //
        //
            ImGui::Text(                "Dear ImGui \t : %s (%d)",                  IMGUI_VERSION, IMGUI_VERSION_NUM                                );
            ImGui::Text(                "ImPlot \t \t : %s",                        IMPLOT_VERSION                                                  );
            ImGui::Text(                "OpenGL \t \t : %s",                        about::OPENGL_VERSION.c_str()                                   );
            ImGui::Text(                "GLSL \t \t : %s",                          about::GLSL_VERSION.c_str()                                     );
            ImGui::Text(                "GLFW \t \t : %d.%d.%d",                    GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION   );
            
            
            
        #ifdef __CBLIB_RELEASE_WITH_DEBUG_INFO__
            ImGui::TextUnformatted(    "#define \t __CBLIB_RELEASE_WITH_DEBUG_INFO__"           );
        #endif  //  __CBLIB_RELEASE_WITH_DEBUG_INFO__  //
        //
        //
        ImGui::Unindent();
    //
    //
    //  END "THIRD-PARTY"...
    
    
    
    
    
    
            
    
    ImGui::BulletText("Your current configuration is:");
    ImGui::Indent();
        ImGui::BulletText("ImDrawIdx: %d-bit", (int)(sizeof(ImDrawIdx) * 8));
        ImGui::BulletText("ImGuiBackendFlags_RendererHasVtxOffset: %s", (ImGui::GetIO().BackendFlags & ImGuiBackendFlags_RendererHasVtxOffset) ? "True" : "False");
    ImGui::Unindent();
    
    
    
    ImGui::Text     ("IMGUI_COLOR_ORDER:\t\t%s",            about::IMGUI_COLOR_ORDER        );
    ImGui::Text     ("IMGUI_PACKING_FORMAT:\t\t%s",         about::IMGUI_PACKING_FORMAT     );
    


    return;
}











//  "get_build_info"
//
void App::get_build_info(void) const
{
    using                               namespace       about;
    [[maybe_unused]] ImGuiIO &          io              = ImGui::GetIO();
    [[maybe_unused]] ImGuiStyle &       style           = ImGui::GetStyle();



    ImGui::Text("Dear ImGui %s (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);
    ImGui::Separator();
    ImGui::Text("sizeof(size_t): %d, sizeof(ImDrawIdx): %d, sizeof(ImDrawVert): %d", (int)sizeof(size_t), (int)sizeof(ImDrawIdx), (int)sizeof(ImDrawVert));
    ImGui::Text("define: __cplusplus=%d", (int)__cplusplus);
#ifdef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    ImGui::Text("define: IMGUI_DISABLE_OBSOLETE_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS
    ImGui::Text("define: IMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS
    ImGui::Text("define: IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_WIN32_FUNCTIONS
    ImGui::Text("define: IMGUI_DISABLE_WIN32_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_DEFAULT_SHELL_FUNCTIONS
    ImGui::Text("define: IMGUI_DISABLE_DEFAULT_SHELL_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_DEFAULT_FORMAT_FUNCTIONS
    ImGui::Text("define: IMGUI_DISABLE_DEFAULT_FORMAT_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_DEFAULT_MATH_FUNCTIONS
    ImGui::Text("define: IMGUI_DISABLE_DEFAULT_MATH_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_DEFAULT_FILE_FUNCTIONS
    ImGui::Text("define: IMGUI_DISABLE_DEFAULT_FILE_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_FILE_FUNCTIONS
    ImGui::Text("define: IMGUI_DISABLE_FILE_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_DEFAULT_ALLOCATORS
    ImGui::Text("define: IMGUI_DISABLE_DEFAULT_ALLOCATORS");
#endif
#ifdef IMGUI_USE_BGRA_PACKED_COLOR
    ImGui::Text("define: IMGUI_USE_BGRA_PACKED_COLOR");
#endif
#ifdef _WIN32
    ImGui::Text("define: _WIN32");
#endif
#ifdef _WIN64
    ImGui::Text("define: _WIN64");
#endif
#ifdef __linux__
    ImGui::Text("define: __linux__");
#endif
#ifdef __APPLE__
    ImGui::Text("define: __APPLE__");
#endif
#ifdef _MSC_VER
    ImGui::Text("define: _MSC_VER=%d", _MSC_VER);
#endif
#ifdef _MSVC_LANG
    ImGui::Text("define: _MSVC_LANG=%d", (int)_MSVC_LANG);
#endif
#ifdef __MINGW32__
    ImGui::Text("define: __MINGW32__");
#endif
#ifdef __MINGW64__
    ImGui::Text("define: __MINGW64__");
#endif
#ifdef __GNUC__
    ImGui::Text("define: __GNUC__=%d", (int)__GNUC__);
#endif
#ifdef __clang_version__
    ImGui::Text("define: __clang_version__=%s", __clang_version__);
#endif



#ifdef __EMSCRIPTEN__
    ImGui::Text("define: __EMSCRIPTEN__");
    ImGui::Text("Emscripten: %d.%d.%d", __EMSCRIPTEN_major__, __EMSCRIPTEN_minor__, __EMSCRIPTEN_tiny__);
#endif
#ifdef IMGUI_HAS_VIEWPORT
    ImGui::Text("define: IMGUI_HAS_VIEWPORT");
#endif
#ifdef IMGUI_HAS_DOCK
    ImGui::Text("define: IMGUI_HAS_DOCK");
#endif

    return;
}



//  "get_config_info"
//
void App::get_config_info(void) const
{
    using                               namespace       about;
    [[maybe_unused]] ImGuiIO &          io              = ImGui::GetIO();
    [[maybe_unused]] ImGuiStyle &       style           = ImGui::GetStyle();



    //  3.  IMGUI CONFIG STUFF...
    //
    ImGui::Text("io.BackendPlatformName: %s", io.BackendPlatformName ? io.BackendPlatformName : "NULL");
    ImGui::Text("io.BackendRendererName: %s", io.BackendRendererName ? io.BackendRendererName : "NULL");
    ImGui::Text("io.ConfigFlags: 0x%08X", io.ConfigFlags);
    if (io.ConfigFlags & ImGuiConfigFlags_NavEnableKeyboard)        ImGui::Text(" NavEnableKeyboard");
    if (io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad)         ImGui::Text(" NavEnableGamepad");
    if (io.ConfigFlags & ImGuiConfigFlags_NoMouse)                  ImGui::Text(" NoMouse");
    if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)      ImGui::Text(" NoMouseCursorChange");
    if (io.ConfigFlags & ImGuiConfigFlags_NoKeyboard)               ImGui::Text(" NoKeyboard");
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)            ImGui::Text(" DockingEnable");
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)          ImGui::Text(" ViewportsEnable");
    if (io.ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)  ImGui::Text(" DpiEnableScaleViewports");
    if (io.ConfigFlags & ImGuiConfigFlags_DpiEnableScaleFonts)      ImGui::Text(" DpiEnableScaleFonts");
    if (io.MouseDrawCursor)                                         ImGui::Text("io.MouseDrawCursor");
    if (io.ConfigViewportsNoAutoMerge)                              ImGui::Text("io.ConfigViewportsNoAutoMerge");
    if (io.ConfigViewportsNoTaskBarIcon)                            ImGui::Text("io.ConfigViewportsNoTaskBarIcon");
    if (io.ConfigViewportsNoDecoration)                             ImGui::Text("io.ConfigViewportsNoDecoration");
    if (io.ConfigViewportsNoDefaultParent)                          ImGui::Text("io.ConfigViewportsNoDefaultParent");
    if (io.ConfigDockingNoSplit)                                    ImGui::Text("io.ConfigDockingNoSplit");
    if (io.ConfigDockingWithShift)                                  ImGui::Text("io.ConfigDockingWithShift");
    if (io.ConfigDockingAlwaysTabBar)                               ImGui::Text("io.ConfigDockingAlwaysTabBar");
    if (io.ConfigDockingTransparentPayload)                         ImGui::Text("io.ConfigDockingTransparentPayload");
    if (io.ConfigMacOSXBehaviors)                                   ImGui::Text("io.ConfigMacOSXBehaviors");
    if (io.ConfigNavMoveSetMousePos)                                ImGui::Text("io.ConfigNavMoveSetMousePos");
    if (io.ConfigNavCaptureKeyboard)                                ImGui::Text("io.ConfigNavCaptureKeyboard");
    if (io.ConfigInputTextCursorBlink)                              ImGui::Text("io.ConfigInputTextCursorBlink");
    if (io.ConfigWindowsResizeFromEdges)                            ImGui::Text("io.ConfigWindowsResizeFromEdges");
    if (io.ConfigWindowsMoveFromTitleBarOnly)                       ImGui::Text("io.ConfigWindowsMoveFromTitleBarOnly");
    if (io.ConfigMemoryCompactTimer >= 0.0f)                        ImGui::Text("io.ConfigMemoryCompactTimer = %.1f", io.ConfigMemoryCompactTimer);
    ImGui::Text("io.BackendFlags: 0x%08X", io.BackendFlags);
    if (io.BackendFlags & ImGuiBackendFlags_HasGamepad)             ImGui::Text(" HasGamepad");
    if (io.BackendFlags & ImGuiBackendFlags_HasMouseCursors)        ImGui::Text(" HasMouseCursors");
    if (io.BackendFlags & ImGuiBackendFlags_HasSetMousePos)         ImGui::Text(" HasSetMousePos");
    if (io.BackendFlags & ImGuiBackendFlags_PlatformHasViewports)   ImGui::Text(" PlatformHasViewports");
    if (io.BackendFlags & ImGuiBackendFlags_HasMouseHoveredViewport)ImGui::Text(" HasMouseHoveredViewport");
    if (io.BackendFlags & ImGuiBackendFlags_RendererHasVtxOffset)   ImGui::Text(" RendererHasVtxOffset");
    if (io.BackendFlags & ImGuiBackendFlags_RendererHasViewports)   ImGui::Text(" RendererHasViewports");
    
    
    ImGui::Separator();
    
    
    ImGui::Text("io.Fonts: %d fonts, Flags: 0x%08X, TexSize: %d,%d", io.Fonts->Fonts.Size, io.Fonts->Flags, io.Fonts->TexWidth, io.Fonts->TexHeight);
    ImGui::Text("io.DisplaySize: %.2f,%.2f", io.DisplaySize.x, io.DisplaySize.y);
    ImGui::Text("io.DisplayFramebufferScale: %.2f,%.2f", io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    ImGui::Separator();
    ImGui::Text("style.WindowPadding: %.2f,%.2f", style.WindowPadding.x, style.WindowPadding.y);
    ImGui::Text("style.WindowBorderSize: %.2f", style.WindowBorderSize);
    ImGui::Text("style.FramePadding: %.2f,%.2f", style.FramePadding.x, style.FramePadding.y);
    ImGui::Text("style.FrameRounding: %.2f", style.FrameRounding);
    ImGui::Text("style.FrameBorderSize: %.2f", style.FrameBorderSize);
    ImGui::Text("style.ItemSpacing: %.2f,%.2f", style.ItemSpacing.x, style.ItemSpacing.y);
    ImGui::Text("style.ItemInnerSpacing: %.2f,%.2f", style.ItemInnerSpacing.x, style.ItemInnerSpacing.y);


    return;
}




//  "get_imgui_info"
//
void App::get_imgui_info(void) const
{
    using                               namespace       about;
    [[maybe_unused]] ImGuiIO &          io              = ImGui::GetIO();
    [[maybe_unused]] ImGuiStyle &       style           = ImGui::GetStyle();



    ImGui::Text("Dear ImGui %s (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);
    ImGui::Separator();
    ImGui::Text("sizeof(size_t): %d, sizeof(ImDrawIdx): %d, sizeof(ImDrawVert): %d", (int)sizeof(size_t), (int)sizeof(ImDrawIdx), (int)sizeof(ImDrawVert));
    ImGui::Text("define: __cplusplus=%d", (int)__cplusplus);
#ifdef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    ImGui::Text("define: IMGUI_DISABLE_OBSOLETE_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS
    ImGui::Text("define: IMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS
    ImGui::Text("define: IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_WIN32_FUNCTIONS
    ImGui::Text("define: IMGUI_DISABLE_WIN32_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_DEFAULT_SHELL_FUNCTIONS
    ImGui::Text("define: IMGUI_DISABLE_DEFAULT_SHELL_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_DEFAULT_FORMAT_FUNCTIONS
    ImGui::Text("define: IMGUI_DISABLE_DEFAULT_FORMAT_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_DEFAULT_MATH_FUNCTIONS
    ImGui::Text("define: IMGUI_DISABLE_DEFAULT_MATH_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_DEFAULT_FILE_FUNCTIONS
    ImGui::Text("define: IMGUI_DISABLE_DEFAULT_FILE_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_FILE_FUNCTIONS
    ImGui::Text("define: IMGUI_DISABLE_FILE_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_DEFAULT_ALLOCATORS
    ImGui::Text("define: IMGUI_DISABLE_DEFAULT_ALLOCATORS");
#endif
#ifdef IMGUI_USE_BGRA_PACKED_COLOR
    ImGui::Text("define: IMGUI_USE_BGRA_PACKED_COLOR");
#endif
#ifdef _WIN32
    ImGui::Text("define: _WIN32");
#endif
#ifdef _WIN64
    ImGui::Text("define: _WIN64");
#endif
#ifdef __linux__
    ImGui::Text("define: __linux__");
#endif
#ifdef __APPLE__
    ImGui::Text("define: __APPLE__");
#endif
#ifdef _MSC_VER
    ImGui::Text("define: _MSC_VER=%d", _MSC_VER);
#endif
#ifdef _MSVC_LANG
    ImGui::Text("define: _MSVC_LANG=%d", (int)_MSVC_LANG);
#endif
#ifdef __MINGW32__
    ImGui::Text("define: __MINGW32__");
#endif
#ifdef __MINGW64__
    ImGui::Text("define: __MINGW64__");
#endif
#ifdef __GNUC__
    ImGui::Text("define: __GNUC__=%d", (int)__GNUC__);
#endif
#ifdef __clang_version__
    ImGui::Text("define: __clang_version__=%s", __clang_version__);
#endif



#ifdef __EMSCRIPTEN__
    ImGui::Text("define: __EMSCRIPTEN__");
    ImGui::Text("Emscripten: %d.%d.%d", __EMSCRIPTEN_major__, __EMSCRIPTEN_minor__, __EMSCRIPTEN_tiny__);
#endif
#ifdef IMGUI_HAS_VIEWPORT
    ImGui::Text("define: IMGUI_HAS_VIEWPORT");
#endif
#ifdef IMGUI_HAS_DOCK
    ImGui::Text("define: IMGUI_HAS_DOCK");
#endif




    //  3.  IMGUI CONFIG STUFF...
    //
    ImGui::Separator();
    ImGui::Text("io.BackendPlatformName: %s", io.BackendPlatformName ? io.BackendPlatformName : "NULL");
    ImGui::Text("io.BackendRendererName: %s", io.BackendRendererName ? io.BackendRendererName : "NULL");
    ImGui::Text("io.ConfigFlags: 0x%08X", io.ConfigFlags);
    if (io.ConfigFlags & ImGuiConfigFlags_NavEnableKeyboard)        ImGui::Text(" NavEnableKeyboard");
    if (io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad)         ImGui::Text(" NavEnableGamepad");
    if (io.ConfigFlags & ImGuiConfigFlags_NoMouse)                  ImGui::Text(" NoMouse");
    if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)      ImGui::Text(" NoMouseCursorChange");
    if (io.ConfigFlags & ImGuiConfigFlags_NoKeyboard)               ImGui::Text(" NoKeyboard");
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)            ImGui::Text(" DockingEnable");
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)          ImGui::Text(" ViewportsEnable");
    if (io.ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)  ImGui::Text(" DpiEnableScaleViewports");
    if (io.ConfigFlags & ImGuiConfigFlags_DpiEnableScaleFonts)      ImGui::Text(" DpiEnableScaleFonts");
    if (io.MouseDrawCursor)                                         ImGui::Text("io.MouseDrawCursor");
    if (io.ConfigViewportsNoAutoMerge)                              ImGui::Text("io.ConfigViewportsNoAutoMerge");
    if (io.ConfigViewportsNoTaskBarIcon)                            ImGui::Text("io.ConfigViewportsNoTaskBarIcon");
    if (io.ConfigViewportsNoDecoration)                             ImGui::Text("io.ConfigViewportsNoDecoration");
    if (io.ConfigViewportsNoDefaultParent)                          ImGui::Text("io.ConfigViewportsNoDefaultParent");
    if (io.ConfigDockingNoSplit)                                    ImGui::Text("io.ConfigDockingNoSplit");
    if (io.ConfigDockingWithShift)                                  ImGui::Text("io.ConfigDockingWithShift");
    if (io.ConfigDockingAlwaysTabBar)                               ImGui::Text("io.ConfigDockingAlwaysTabBar");
    if (io.ConfigDockingTransparentPayload)                         ImGui::Text("io.ConfigDockingTransparentPayload");
    if (io.ConfigMacOSXBehaviors)                                   ImGui::Text("io.ConfigMacOSXBehaviors");
    if (io.ConfigNavMoveSetMousePos)                                ImGui::Text("io.ConfigNavMoveSetMousePos");
    if (io.ConfigNavCaptureKeyboard)                                ImGui::Text("io.ConfigNavCaptureKeyboard");
    if (io.ConfigInputTextCursorBlink)                              ImGui::Text("io.ConfigInputTextCursorBlink");
    if (io.ConfigWindowsResizeFromEdges)                            ImGui::Text("io.ConfigWindowsResizeFromEdges");
    if (io.ConfigWindowsMoveFromTitleBarOnly)                       ImGui::Text("io.ConfigWindowsMoveFromTitleBarOnly");
    if (io.ConfigMemoryCompactTimer >= 0.0f)                        ImGui::Text("io.ConfigMemoryCompactTimer = %.1f", io.ConfigMemoryCompactTimer);
    ImGui::Text("io.BackendFlags: 0x%08X", io.BackendFlags);
    if (io.BackendFlags & ImGuiBackendFlags_HasGamepad)             ImGui::Text(" HasGamepad");
    if (io.BackendFlags & ImGuiBackendFlags_HasMouseCursors)        ImGui::Text(" HasMouseCursors");
    if (io.BackendFlags & ImGuiBackendFlags_HasSetMousePos)         ImGui::Text(" HasSetMousePos");
    if (io.BackendFlags & ImGuiBackendFlags_PlatformHasViewports)   ImGui::Text(" PlatformHasViewports");
    if (io.BackendFlags & ImGuiBackendFlags_HasMouseHoveredViewport)ImGui::Text(" HasMouseHoveredViewport");
    if (io.BackendFlags & ImGuiBackendFlags_RendererHasVtxOffset)   ImGui::Text(" RendererHasVtxOffset");
    if (io.BackendFlags & ImGuiBackendFlags_RendererHasViewports)   ImGui::Text(" RendererHasViewports");
    ImGui::Separator();
    ImGui::Text("io.Fonts: %d fonts, Flags: 0x%08X, TexSize: %d,%d", io.Fonts->Fonts.Size, io.Fonts->Flags, io.Fonts->TexWidth, io.Fonts->TexHeight);
    ImGui::Text("io.DisplaySize: %.2f,%.2f", io.DisplaySize.x, io.DisplaySize.y);
    ImGui::Text("io.DisplayFramebufferScale: %.2f,%.2f", io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    ImGui::Separator();
    ImGui::Text("style.WindowPadding: %.2f,%.2f", style.WindowPadding.x, style.WindowPadding.y);
    ImGui::Text("style.WindowBorderSize: %.2f", style.WindowBorderSize);
    ImGui::Text("style.FramePadding: %.2f,%.2f", style.FramePadding.x, style.FramePadding.y);
    ImGui::Text("style.FrameRounding: %.2f", style.FrameRounding);
    ImGui::Text("style.FrameBorderSize: %.2f", style.FrameBorderSize);
    ImGui::Text("style.ItemSpacing: %.2f,%.2f", style.ItemSpacing.x, style.ItemSpacing.y);
    ImGui::Text("style.ItemInnerSpacing: %.2f,%.2f", style.ItemInnerSpacing.x, style.ItemInnerSpacing.y);

    return;
}














// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.






//  UNDEF THE MACROS WE USED EARLIER...
//
#ifdef MAKE_GMAIL_COMPOSE_URL_IMPL
#  undef MAKE_GMAIL_COMPOSE_URL_IMPL
#endif

#ifdef MAKE_GMAIL_URL
#  undef MAKE_GMAIL_URL
#endif



// *************************************************************************** //
// *************************************************************************** //  END.

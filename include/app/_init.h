//
//  app/init.h
//  CBApp
//
//  Created by Collin Bond on 4/15/25.
//
// *************************************************************************** //
// *************************************************************************** //
#ifndef _CBAPP_APP_INIT_H
#define _CBAPP_APP_INIT_H                  1

#include <filesystem>
#include <array>
#include <cstddef> // for std::size_t
#include <string_view>

//#include "imgui.h"
//#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"



//  1.  PRE-PROCESSOR DEFINED MACROS...
// *************************************************************************** //
// *************************************************************************** //

//
//  ...
//






// *************************************************************************** //
//
//
//  2.  GLOBAL-CONSTANTS AND NAMESPACE-DEFINED VARIABLES...
// *************************************************************************** //
// *************************************************************************** //

namespace cb { namespace app { //     BEGINNING NAMESPACE "cb" :: "app"...
// *************************************************************************** //
// *************************************************************************** //

//  0.  INLINE UTILITY FUNCTIONS...
//
//
//  "get_my_file_path"
//
inline const char * path_to_char(const std::filesystem::path & dir, const char * file) {
    static const std::string    path   = (dir / file).string();
    return path.c_str();
}



//  1.  APP CONSTANTS               | TITLES, GEOMETRIES, WINDOW DIMENSIONS, DEFAULT SETTINGS, ETC...
//
//
//      1.1         INFORMATION...
//
//                              A.  GENERAL APPLICATION STUFF.
inline constexpr float              DEF_MOUSEWHEEL_SCROLL_SPEED         = 0.50f;
// inline constexpr const char *       DEF_SIDEBAR_WIN_TITLE               = "Sidebar";
// inline constexpr const char *       DEF_MAIN_WIN_TITLE                  = "My Application";
// inline constexpr const char *       DEF_GRAPHING_APP_TITLE              = "Graphing Application";
//
//                              B.  PLOT AND DATA VISUALIZATIONS.
inline constexpr float              DEF_PLOT_LW                         = 0.50f;
//
//
//
//      1.2         DIMENSIONS & GEOMETRY...
//
//                              A.  ROOT WINDOW.
inline constexpr float              DEF_ROOT_WINDOW_SCALE               = 0.80f;
//
//                              B.  SIDEBAR.
inline constexpr float              DEF_SB_CLOSED_WIDTH                 = 0.20;    //  SIDEBAR WINDOW...
inline constexpr float              DEF_SB_OPEN_WIDTH                   = 0.20;
//
//                                  B1. Sidebae "Toggle" Button:
inline constexpr float              DEF_SB_TWIN_WINDOWBORDERSIZE        = 0.0f;
inline constexpr float              DEF_SB_TWIN_WINDOWROUNDING          = 4.0f;
//
//                                  B2. Settings:
inline constexpr bool               DEF_PERF_METRICS_STATE              = true;     //  begin program w/ perf. metrics panel open or closed.
inline constexpr bool               DEF_PERF_PLOTS_STATE                = false;    //  begin w/ perf. plots open or closed.
inline constexpr float              PERF_PLOTS_HISTORY_LENGTH           = 6.0f;    //  history length (in sec) of perf. plots
//
//
//
//      1.3         COLORS...
//                              A.  GENERAL COLORS.
inline constexpr ImVec4             DEF_APPLE_BLUE                      = ImVec4(0.244f,    0.467f,     0.847f,     1.000f);
inline constexpr ImVec4             DEF_APPLE_RED                       = ImVec4(1.000f,    0.271f,     0.227f,     1.000f);
inline constexpr ImVec4             DEF_INVISIBLE_COLOR                 = ImVec4(0.000f,    0.000f,     0.000f,     0.000f);
//
//                              B.  WINDOW COLORS.
inline constexpr ImVec4             DEF_ROOT_WIN_BG                     = ImVec4(0.000f,    0.000f,     0.000f,     0.000f);
inline constexpr ImVec4             DEF_SIDEBAR_WIN_BG                  = ImVec4(0.192f,    0.192f,     0.192f,     1.000f);
inline constexpr ImVec4             DEF_TITLEBAR_WIN_BG                 = ImVec4(0.999f,    0.999f,     0.999f,     0.500f);
inline constexpr ImVec4             DEF_MAIN_WIN_BG                     = ImVec4(0.125f,    0.133f,     0.141f,     1.000f);

  
  
  
  
  
//  2.  FILE-NAMES, DIRECTORY-NAMES, ETC    [COMPILE-TIME]...
//
//      2A.     DIRECTORY-NAMES...
inline constexpr const char *       assets_dirname                      = "assets";
//
//      2A.1    Sub-Directory Names...
inline constexpr const char *       config_dirname                      = "config";
inline constexpr const char *       fonts_dirname                       = "fonts";
//
//
//      2B.     FILENAMES...
inline constexpr const char *       ini_filename                        = "cb_app.ini";
inline constexpr const char *       style_filename                      = "imgui_style.json";






//  3.  FILE-PATHS, DIRECTORIES      | (FONT FAMILIES, SIZES, ETC)...
//
//      3A.     MAIN PROJECT DIRECTORIES    [RUN-TIME]...
//inline const std::filesystem::path  CURRENT_PATH            = std::filesystem::current_path();    //  EXE_DIR = std::filesystem::absolute(CURRENT_PATH);
inline constexpr const char *       EXE_DIR                             = "/";
inline constexpr const char *       BUILD_DIR                           = "../";
inline constexpr const char *       ROOT_DIR                            = "../../";
//
//      3A.2    Sub-Directories...
//                      1.  "Scripts"...
inline constexpr const char *       SCRIPTS_DIR                         = "../../scripts/";
inline constexpr const char *       PYTHON_DIR                          = "../../scripts/python/";
//
//
//                      2.  "Assets"...
inline constexpr const char *       ASSETS_DIR                          = "../../assets/";
//
inline constexpr const char *       CONFIG_DIR                          = "../../assets/config/";
inline constexpr const char *       FONTS_DIR                           = "../../assets/fonts/";
inline constexpr const char *       BIN_DIR                             = "../../assets/bin/";
//
//                      3.  "Data"...
inline constexpr const char *       DATA_DIR                            = "../../data/";
//
inline constexpr const char *       USER_DATA_DIR                       = "../../data/user_data/";
//
//
//
//
//      3B.     FILE-PATHS                  [RUN-TIME]...
inline constexpr const char *       INI_FILEPATH                        = "../../assets/config/cb_app.ini";
inline constexpr const char *       INI_BACKUP_FILEPATH                 = "../../assets/config/.backup.ini";     //  "../../assets/fonts/swap_cb_app.ini";

inline constexpr const char *       STYLE_FILEPATH                      = "../../assets/config/imgui_style.json";

inline constexpr const char *       PYTHON_FPGA_FILEPATH                = "../../scripts/python/fpga.py";
inline constexpr const char *       PYTHON_ECHO_FILEPATH                = "../../scripts/python/echo.py";
inline constexpr const char *       PYTHON_COUNTER_FILEPATH             = "../../scripts/python/counter.py";
inline constexpr const char *       PYTHON_DUMMY_FPGA_FILEPATH          = "../../scripts/python/counter_dummy.py";



//      3.3     Fonts.
//
inline constexpr const char * get_font_path(void) {
    #if defined(__APPLE__)
        return "/System/Library/Fonts/SFNS.ttf";
    # else
        return "../../assets/fonts/Roboto/static/Roboto-Regular.ttf";
    #endif      //  __APPLE__   //
}

inline constexpr const char *       DEF_FONT_PATH                       = get_font_path();
    


// *************************************************************************** //
//
//
//  FONTS...
// *************************************************************************** //
// *************************************************************************** //
inline constexpr float              DEF_FONT_SIZE                       = 13.0f;
inline constexpr float              DEF_SMALL_FONT_SIZE                 = 10.0f;
inline constexpr float              DEF_FOOTNOTE_FONT_SIZE              = 8.0f;


//  FUNCTIONAL MACRO TO DEFINE APPLICATION FONTS...
//
#if defined(__APPLE__)
    //  Fonts for MacOS Builds.
    #define _CBAPP_FONT_LIST(X)         \
        X(Main,             "/System/Library/Fonts/SFNS.ttf",                               DEF_FONT_SIZE)          \
        X(Small,            "/System/Library/Fonts/SFNS.ttf",                               DEF_SMALL_FONT_SIZE)    \
        X(FootNote,         "/System/Library/Fonts/SFNS.ttf",                               DEF_FOOTNOTE_FONT_SIZE) \
        X(Mono,             "/System/Library/Fonts/SFNSMono.ttf",                           DEF_FONT_SIZE)          \
        X(SanSerif,         "/System/Library/Fonts/NewYork.ttf",                            DEF_FONT_SIZE)          \
        X(SanSerifSmall,    "/System/Library/Fonts/NewYork.ttf",                            DEF_SMALL_FONT_SIZE)
# else
    //  Fonts for Windows, Linux, or other Builds.
    #define _CBAPP_FONT_LIST(X)         \
        X(Main,             "../../assets/fonts/Roboto/static/Roboto-Regular.ttf",          DEF_FONT_SIZE)          \
        X(Small,            "../../assets/fonts/Roboto/static/Roboto-Regular.ttf",          DEF_SMALL_FONT_SIZE)    \
        X(FootNote,         "../../assets/fonts/Roboto/static/Roboto-Regular.ttf",          DEF_FOOTNOTE_FONT_SIZE)
#endif  //  __APPLE__  //



// *************************************************************************** //

//  "Font_t"
//      - Enum to provide names for the index of each font...
enum class Font_t : int {
#define X(name, path, size) name,
    _CBAPP_FONT_LIST(X)
#undef X
    Count
};


//  "FontInfo"
//      - Simple struct to define the various properties of each font...
//#ifdef __APPLE__
struct FontInfo {
    std::string         path;
    float               size;
};
//  # else
//  struct FontInfo {
//      int                 rid;
//      float               size;
//  };
//#endif  //  __APPLE__  //



//  COMPILE-TIME ARRAY CONTAINING ALL APPLICATION_FONTS
//
inline static const std::array<FontInfo, int( Font_t::Count )>     APPLICATION_FONT_STYLES = {{
#define X(name, path, size) { path, size },
    _CBAPP_FONT_LIST(X)
#undef X
}};






// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" :: "app" NAMESPACE.







    



// *************************************************************************** //
//
//
//  9.  COMPILE-TIME DEFINED MESSAGES AND PROGRAM INFORMATION...
// *************************************************************************** //
// *************************************************************************** //
//
namespace cb {  //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//  1.  ERROR MESSAGING             | ASSERTION STATEMENTS, EXCEPTION MESSAGES, ETC...
//
namespace error { //     BEGINNING NAMESPACE "error"...
// *************************************************************************** //
// *************************************************************************** //

//  1.      GLFW ERRORS...
inline constexpr const char *       GLFW_INIT_ERROR                             =
    "A fatal error has occured during program initialization:\n\t"
    "Call to \"glfwInit()\" returned NULL.\n"
    "This may occur if your machine does not have the necessary graphics drivers for OpenGL installed.\n"
    "Ensure that your system is using the designated GPU / Graphics Card and NOT the on-board "
    "graphics that are build into the motherboard.";
    
inline constexpr const char *       GLFW_WINDOW_INIT_ERROR                      =
    "A fatal error has occured during program initialization:\n\t"
    "Call to \"glfwCreateWindow()\" returned NULL.\n";
//
//
//
//  2.      ASSERT STATEMENT ERRORS...
inline constexpr const char *       ASSERT_INVALID_WINDOW_RENDER_FUNCTIONS      =
    "Assert statement failure during initialization:\n\t"
    "One or more window render functions is NULL (app::WinInfo.render_fn).\n"
    "CHECK:\n"
    "\t(1.) \t \"dispatch_window_function()\" in \"src/app/init.cpp\".\n"
    "\t(2.) \t \"#define _CBAPP_WINDOW_LIST(X)\" in \"include/app/state/_state.h\".\n";
    
inline constexpr const char *       ASSERT_INVALID_PRIMARY_WINDOWS              =
    "Assert statement failure during initialization:\n\t"
    "\"Sidebar\" window cannot be an item inside \"this->m_primary_windows!\" variable.";



// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "error" NAMESPACE.






//  2.  GENERAL INFORMATION         | TOOL-TIPS, PROMPTS, PROGRAM INFORMATION, ETC...
//
namespace info { //     BEGINNING NAMESPACE "info"...
// *************************************************************************** //
// *************************************************************************** //

//
//  ...
//










//  "ANSI" ESCAPE KEYS...
//
/*
struct ANSI
{
    //      Basic colors.
    static constexpr std::string_view BLACK           = "\x1B[30m";
    static constexpr std::string_view BLACK_BG        = "\x1B[40m";
    static constexpr std::string_view BLACK_BOLD      = "\x1B[1m\x1B[30m";

    static constexpr std::string_view RED             = "\x1B[31m";
    static constexpr std::string_view RED_BG          = "\x1B[41m";
    static constexpr std::string_view RED_BOLD        = "\x1B[1m\x1B[31m";

    static constexpr std::string_view GREEN           = "\x1B[32m";
    static constexpr std::string_view BLUE_BG         = "\x1B[44m";
    static constexpr std::string_view BLUE_BOLD       = "\x1B[1m\x1B[32m";

    static constexpr std::string_view YELLOW          = "\x1B[33m";
    static constexpr std::string_view GREEN_BG        = "\x1B[42m";
    static constexpr std::string_view GREEN_BOLD      = "\x1B[1m\x1B[34m";

    static constexpr std::string_view BLUE            = "\x1B[34m";
    static constexpr std::string_view YELLOW_BG       = "\x1B[43m";
    static constexpr std::string_view YELLOW_BOLD     = "\x1B[1m\x1B[35m";

    static constexpr std::string_view MAGENTA         = "\x1B[35m";
    static constexpr std::string_view MAGENTA_BG      = "\x1B[45m";
    static constexpr std::string_view MAGENTA_BOLD    = "\x1B[1m\x1B[33m";

    static constexpr std::string_view CYAN            = "\x1B[36m";
    static constexpr std::string_view CYAN_BG         = "\x1B[46m";
    static constexpr std::string_view CYAN_BOLD       = "\x1B[1m\x1B[36m";

    static constexpr std::string_view WHITE           = "\x1B[37m";
    static constexpr std::string_view WHITE_BG        = "\x1B[47m";
    static constexpr std::string_view WHITE_BOLD      = "\x1B[1m\x1B[37m";
    //
    //
    //      Bright colors.
    static constexpr std::string_view BLACK_BRIGHT    = "\x1B[90m";
    static constexpr std::string_view BLACK_BB        = "\x1B[1m\x1B[90m";

    static constexpr std::string_view RED_BRIGHT      = "\x1B[91m";
    static constexpr std::string_view RED_BB          = "\x1B[1m\x1B[91m";

    static constexpr std::string_view GREEN_BRIGHT    = "\x1B[92m";
    static constexpr std::string_view GREEN_BB        = "\x1B[1m\x1B[92m";

    static constexpr std::string_view YELLOW_BRIGHT   = "\x1B[93m";
    static constexpr std::string_view YELLOW_BB       = "\x1B[1m\x1B[93m";

    static constexpr std::string_view BLUE_BRIGHT     = "\x1B[94m";
    static constexpr std::string_view BLUE_BB         = "\x1B[1m\x1B[94m";

    static constexpr std::string_view MAGENTA_BRIGHT  = "\x1B[95m";
    static constexpr std::string_view MAGENTA_BB      = "\x1B[1m\x1B[95m";

    static constexpr std::string_view CYAN_BRIGHT     = "\x1B[96m";
    static constexpr std::string_view CYAN_BB         = "\x1B[1m\x1B[96m";

    static constexpr std::string_view WHITE_BRIGHT    = "\x1B[97m";
    static constexpr std::string_view WHITE_BB        = "\x1B[1m\x1B[97m";
    //
    //
    //      Text styles.
    static constexpr std::string_view BOLD            = "\x1B[1m";
    static constexpr std::string_view RESET_BOLD      = "\x1B[22m";

    static constexpr std::string_view DIM             = "\x1B[2m";
    static constexpr std::string_view RESET_DIM       = "\x1B[22m";

    static constexpr std::string_view UNDERLINE       = "\x1B[4m";
    static constexpr std::string_view RESET_UNDERLINE = "\x1B[24m";

    static constexpr std::string_view BLINK           = "\x1B[5m";
    static constexpr std::string_view RESET_BLINK     = "\x1B[25m";

    static constexpr std::string_view INVERSE         = "\x1B[7m";
    static constexpr std::string_view RESET_INVERSE   = "\x1B[27m";
    //
    //
    //      Reset defaults.
    static constexpr std::string_view RESET_BG        = "\x1B[49";
    static constexpr std::string_view RESET_FG        = "\x1B[39";
    static constexpr std::string_view RESET           = "\x1B[0m";
    //
    //
    //      Convenience messages.
    static constexpr std::string_view PASS            = "\x1B[1m\x1B[92mPASSED.\x1B[0m\n";
    static constexpr std::string_view FAIL            = "\x1B[1m\x1B[91mFAILED.\x1B[0m\n";
};

*/

// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "info" NAMESPACE.






// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.




























#endif      //  _CBAPP_APP_INIT_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.

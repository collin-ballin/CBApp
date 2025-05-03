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
//      1.1                         INFORMATION...
inline constexpr const char *       DEF_ROOT_WIN_TITLE          = "Dear ImGui App (V0)";
inline constexpr const char *       DEF_SIDEBAR_WIN_TITLE       = "Sidebar";
inline constexpr const char *       DEF_MAIN_WIN_TITLE          = "My Application";
inline constexpr const char *       DEF_GRAPHING_APP_TITLE      = "Graphing Application";
//
//
//      1.2                         SETTINGS...
inline constexpr bool               DEF_PERF_METRICS_STATE      = true;     //  begin program w/ perf. metrics panel open or closed.
inline constexpr bool               DEF_PERF_PLOTS_STATE        = false;    //  begin w/ perf. plots open or closed.
inline constexpr float              PERF_PLOTS_HISTORY_LENGTH   = 10.0f;    //  history length (in sec) of perf. plots
//
//
//      1.3                         DIMENSIONS & GEOMETRY...
inline constexpr int                DEF_ROOT_WIN_WIDTH          = 1280;     //  GLFW / HOST WINDOW...
inline constexpr int                DEF_ROOT_WIN_HEIGHT         = 720;
inline constexpr float              DEF_ROOT_WINDOW_SCALE       = 0.80f;
//
inline constexpr float              DEF_SB_CLOSED_WIDTH         = 0.030f;    //  SIDEBAR WINDOW...
inline constexpr float              DEF_SB_OPEN_WIDTH           = 0.25;
//
//
//      1.4                         COLORS...
inline constexpr ImVec4             DEF_APPLE_BLUE              = ImVec4(0.244f,    0.467f,     0.847f,     1.000f);
inline constexpr ImVec4             DEF_APPLE_RED               = ImVec4(1.000f,    0.271f,     0.227f,     1.000f);
inline constexpr ImVec4             DEF_INVISIBLE_COLOR         = ImVec4(0.000f,    0.000f,     0.000f,     0.000f);

inline constexpr ImVec4             DEF_ROOT_WIN_BG             = ImVec4(0.000f,    0.000f,     0.000f,     0.000f); //ImVec4(0.45f,     0.55f,      0.60f,      1.0f);
inline constexpr ImVec4             DEF_SIDEBAR_WIN_BG          = ImVec4(0.192f,    0.192f,     0.192f,     1.000f);
inline constexpr ImVec4             DEF_MAIN_WIN_BG             = ImVec4(0.125f,    0.133f,     0.141f,     1.000f);

  
  
  
  
  
//  2.  FILE-NAMES, DIRECTORY-NAMES, ETC    [COMPILE-TIME]...
//
//      2A.     DIRECTORY-NAMES...
inline constexpr const char *       assets_dirname              = "assets";
//
//      2A.1    Sub-Directory Names...
inline constexpr const char *       config_dirname              = "config";
inline constexpr const char *       fonts_dirname               = "fonts";
//
//
//      2B.     FILENAMES...
inline constexpr const char *       ini_filename                = "cb_app.ini";
inline constexpr const char *       style_filename              = "imgui_style.json";






//  3.  FILE-PATHS, DIRECTORIES      | (FONT FAMILIES, SIZES, ETC)...
//
//      3A.     MAIN PROJECT DIRECTORIES    [RUN-TIME]...
//inline const std::filesystem::path  CURRENT_PATH            = std::filesystem::current_path();    //  EXE_DIR = std::filesystem::absolute(CURRENT_PATH);
inline constexpr const char *       EXE_DIR                     = "/";
inline constexpr const char *       BUILD_DIR                   = "../";
inline constexpr const char *       ROOT_DIR                    = "../../";
//
//      3A.2    Sub-Directories...
//                      1.  "Scripts"...
inline constexpr const char *       SCRIPTS_DIR                 = "../../scripts/";
inline constexpr const char *       PYTHON_DIR                  = "../../scripts/python/";
//
//
//                      2.  "Assets"...
inline constexpr const char *       ASSETS_DIR                  = "../../assets/";
//
inline constexpr const char *       CONFIG_DIR                  = "../../assets/config/";
inline constexpr const char *       FONTS_DIR                   = "../../assets/fonts/";
inline constexpr const char *       BIN_DIR                     = "../../assets/bin/";
//
//                      3.  "Data"...
inline constexpr const char *       DATA_DIR                    = "../../data/";
//
inline constexpr const char *       USER_DATA_DIR               = "../../data/user_data/";
//
//
//
//
//      3B.     FILE-PATHS                  [RUN-TIME]...
inline constexpr const char *       INI_FILEPATH                = "../../assets/config/cb_app.ini";
inline constexpr const char *       INI_BACKUP_FILEPATH         = "../../assets/config/.backup.ini";     //  "../../assets/fonts/swap_cb_app.ini";

inline constexpr const char *       STYLE_FILEPATH              = "../../assets/config/imgui_style.json";



//      3.3     Fonts.
//
inline constexpr const char * get_font_path(void) {
    #if defined(__APPLE__)
        return "/System/Library/Fonts/SFNS.ttf";
    # else
        return "../../assets/fonts/Roboto/static/Roboto-Regular.ttf";
    #endif      //  __APPLE__   //
}

inline constexpr const char *       DEF_FONT_PATH               = get_font_path();
inline constexpr float              DEF_FONT_SIZE               = 20.0f;
inline constexpr float              DEF_SMALL_FONT_SIZE         = 14.0f;
    


// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //

//  FUNCTIONAL MACRO TO DEFINE APPLICATION FONTS...
//
#if defined(__APPLE__)
    //  Fonts for MacOS Builds.
    #define _CBAPP_FONT_LIST(X)         \
        X(Main,             "/System/Library/Fonts/SFNS.ttf",                               20.0f)      \
        X(Small,            "/System/Library/Fonts/SFNS.ttf",                               14.0f)      \
        X(Mono,             "/System/Library/Fonts/SFNSMono.ttf",                           20.0f)      \
        X(SanSerif,         "/System/Library/Fonts/NewYork.ttf",                            20.0f)      \
        X(SanSerifSmall,    "/System/Library/Fonts/NewYork.ttf",                            14.0f)
# else
    //  Fonts for Windows, Linux, or other Builds.
    #define _CBAPP_FONT_LIST(X)         \
        X(Main,             "../../assets/fonts/Roboto/static/Roboto-Regular.ttf",          20.0f)       \
        X(Small,            "../../assets/fonts/Roboto/static/Roboto-Regular.ttf",          14.0f)
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
struct FontInfo {
    std::string_view    path;
    float               size;
};


//  COMPILE-TIME ARRAY CONTAINING ALL APPLICATION_FONTS
//
static constexpr std::array<FontInfo, int( Font_t::Count )>     APPLICATION_FONT_STYLES = {{
#define X(name, path, size) { path, size },
    _CBAPP_FONT_LIST(X)
#undef X
}};

    



// *************************************************************************** //
//
//
//  3.  GLOBAL-CONSTANTS AND NAMESPACE-DEFINED VARIABLES...
// *************************************************************************** //
// *************************************************************************** //










// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" :: "app" NAMESPACE.






#endif      //  _CBAPP_APP_INIT_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.

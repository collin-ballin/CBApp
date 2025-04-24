//
//  constants.h
//  CBApp
//
//  Created by Collin Bond on 4/15/25.
//
// *************************************************************************** //
// *************************************************************************** //
#ifndef _CBAPP_CONSTANTS_H
#define _CBAPP_CONSTANTS_H                  1

#include <filesystem>
#include <array>
#include <cstddef> // for std::size_t
#include <string_view>

//#include "imgui.h"
//#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"



// *************************************************************************** //
//
//
//  2.  GLOBAL-CONSTANTS AND NAMESPACE-DEFINED VARIABLES...
// *************************************************************************** //
// *************************************************************************** //



namespace cb { namespace utl { //     BEGINNING NAMESPACE "cb" :: "utl"...
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



//  1.  APP CONSTANTS               | TITLES, GEOMETRIES, WINDOW DIMENSIONS, ETC...
//
inline constexpr const char *       DEF_ROOT_WIN_TITLE      = "Dear ImGui App (V0)";
inline constexpr int                DEF_ROOT_WIN_WIDTH      = 1280;
inline constexpr int                DEF_ROOT_WIN_HEIGHT     = 720;
inline constexpr float              DEF_ROOT_WINDOW_SCALE   = 0.80f;
inline constexpr ImVec4             DEF_ROOT_WIN_BG         = ImVec4(0.00f,     0.00f,      0.00f,      0.00f); //ImVec4(0.45f,     0.55f,      0.60f,      1.0f);

inline constexpr ImVec4             DEF_APPLE_BLUE          = ImVec4(0.244f,    0.467f,     0.847f,     1.000f);
inline constexpr ImVec4             DEF_APPLE_RED           = ImVec4(1.000f,    0.271f,     0.227f,     1.000f);

  
//  2.  FILE-NAMES, DIRECTORY-NAMES, ETC    [COMPILE-TIME]...
//
//      2A.     DIRECTORY-NAMES...
inline constexpr const char *       assets_dirname          = "assets";
//
//      2A.1    Sub-Directory Names...
inline constexpr const char *       config_dirname          = "config";
inline constexpr const char *       fonts_dirname           = "fonts";
//
//
//      2B.     FILENAMES...
inline constexpr const char *       ini_filename            = "cb_app.ini";
inline constexpr const char *       style_filename          = "imgui_style.json";



//  3.  FILE-PATHS, DIRECTORIES      | (FONT FAMILIES, SIZES, ETC)...
//
//      3A.     MAIN PROJECT DIRECTORIES    [RUN-TIME]...
//inline const std::filesystem::path  CURRENT_PATH            = std::filesystem::current_path();    //  EXE_DIR = std::filesystem::absolute(CURRENT_PATH);
inline constexpr const char *       EXE_DIR                 = "/";
inline constexpr const char *       BUILD_DIR               = "../";
inline constexpr const char *       ROOT_DIR                = "../../";
//
//      3A.2    Sub-Directories...
inline constexpr const char *       ASSETS_DIR              = "../../assets/";
inline constexpr const char *       CONFIG_DIR              = "../../assets/config/";
inline constexpr const char *       FONTS_DIR               = "../../assets/fonts/";
//
//
//      3B.     FILE-PATHS                  [RUN-TIME]...
inline constexpr const char *       INI_FILEPATH            = "../../assets/fonts/cb_app.ini";
inline constexpr const char *       INI_BACKUP_FILEPATH     = "../../assets/fonts/.backup.ini";     //  "../../assets/fonts/swap_cb_app.ini";

inline constexpr const char *       STYLE_FILEPATH          = "../../assets/fonts/imgui_style.json";



//      3.3     Fonts.
//
inline constexpr const char * get_font_path(void) {
    #if defined(__APPLE__)
        return "/System/Library/Fonts/SFNS.ttf";
    # else
        return "../../assets/fonts/Roboto/static/Roboto-Regular.ttf";
    #endif      //  __APPLE__   //
}

inline constexpr const char *       DEF_FONT_PATH           = get_font_path();
inline constexpr float              DEF_FONT_SIZE           = 20.0f;
inline constexpr float              DEF_SMALL_FONT_SIZE     = 14.0f;
    

    



// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" NAMESPACE.






#endif      //  _CBAPP_CONSTANTS_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.

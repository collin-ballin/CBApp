/***********************************************************************************
*
*       ********************************************************************
*       ****         C B A P P _ C O N F I G . h  ____  F I L E         ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      May 23, 2025.
*
*       ********************************************************************
*                FILE:      [include/app/state/_state.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#pragma once
#include "version.hpp"



// *************************************************************************** //
//  1.  MACROS DEFINED BY "DEAR IMGUI" OR OTHER LIBRARIES]...
//
//
//      *****************************************************
//      ****           D O   N O T   T O U C H           ****
//      *****************************************************
//
// *************************************************************************** //
// *************************************************************************** //
#define     GL_SILENCE_DEPRECATION                      1               //  MACRO DEFINED BY "DEAR IMGUI" EXAMPLE PROGRAM.

//  #define     IMPLOT_DISABLE_OBSOLETE_FUNCTIONS           1
//  #define     IMGUI_DISABLE_OBSOLETE_FUNCTIONS            1

// IMGUI_DISABLE_DEBUG_TOOLS   //  Used to be named "IMGUI_DISABLE_METRICS_WINDOW"

// *************************************************************************** //
// *************************************************************************** //
//  END.






// *************************************************************************** //
//
//
//  2.  MY-OWN GLOBALLY DEFINED PRE-PROCESSOR MACROS...
// *************************************************************************** //
// *************************************************************************** //


//      2.1     CRITICAL BUILD SETTINGS...
// *************************************************************************** //
// *************************************************************************** //
//#define         __CBAPP_DEBUG__                             1                 //  Enable delay before starting int main (bug-fix for X-Code IDE issue).
//  #define         __CBAPP_BUILD_CCOUNTER_APP__                    1               //  BUILD FOR COINCIDENCE COUNTER...
//  #define         __CBAPP_BUILD_FDTD_APP__                        1               //  BUILD FOR FDTD APP...
#define         __CBAPP_BUILD_EDITOR_APP__                  1                   //  BUILD FOR EDITOR / "ILLUSTRATOR" APP...


//  ENFORCE THAT THERE ARE NOT MULTIPLE BUILDS ARE #DEFINED...
//
#if (  defined(__CBAPP_BUILD_CCOUNTER_APP__)    \
     + defined(__CBAPP_BUILD_FDTD_APP__)        \
     + defined(__CBAPP_BUILD_EDITOR_APP__)      \
    ) > 1
#   error "Cannot build with more than one __CBAPP_BUILD_APP__ macro defined (look in \"cbapp_config.h\")"
#endif



//      2.2     W-I-P APPLICATION OPTIONS...
// *************************************************************************** //
// *************************************************************************** //


//      2.3     DEBUGGING UTILITIES...
// *************************************************************************** //
// *************************************************************************** //
//  #define     CBAPP_USE_VIEWPORT                                  1           //  Debug util.  | Should be removed soon (as of 25th/04/2025).
//  #define     CBAPP_USE_DOCKSPACE                                 1           //  Debug util.  | Should be removed soon (as of 25th/04/2025).
#define     CBAPP_NEW_DOCKSPACE                             1



//      2.4     APP SETTINGS...
// *************************************************************************** //
// *************************************************************************** //
#define     CBAPP_ENABLE_MOVE_AND_RESIZE                        1           //  Allow user to moving / resizing core windows.
//  #define     CBAPP_DISABLE_SAVE_WINDOW_SIZE                      1           //  Prevent windows from saving/loading from "ini".     //  ImGuiWindowFlags_NoSavedSettings
#define     CBAPP_USE_FONTSCALE_DPI                         1           //  Use "S.m_dpi_fontscale" SEPERATE FROM "S.m_dpi_scale".

//#define     CBAPP_DISABLE_INI                               1           //  Prevent use of ".ini" file to save/recall data.
#define     CBAPP_LOAD_STYLE_FILE                           1           //  Prevent use of ".json" file to save/recall "ImGui" style data.

#if !( defined(_WIN32) || defined(MINGW) )
    # define     CBAPP_DISABLE_TERMINAL_COLORS              1           //  DISABLE ANSI Colors for LOGGER OUTPUT.
#endif      //  _WIN32 || MINGW  //




//      2.5     OTHER MACROS...
// *************************************************************************** //
// *************************************************************************** //

//              1.      META-DATA CAPTURE FOR LOGGING.
// *************************************************************************** //
#define CB_LOG(level, fmt, ...)                                             \
    Logger::instance().log_ex(                                              \
        fmt,                                                                \
        level,                                                              \
        __FILE__, __LINE__, __func__, std::this_thread::get_id()            \
        __VA_OPT__(, ) __VA_ARGS__)


//  clang-specific macros
// *************************************************************************** //
#if defined(__clang__)
    #define _CBAPP_WARN_UNUSED_PUSH                                         \
        _Pragma("clang diagnostic push")                                    \
        _Pragma("clang diagnostic warning \"-Wunused-private-field\"")
    #define _CBAPP_WARN_UNUSED_POP                                          \
        _Pragma("clang diagnostic pop")
// *************************************************************************** //
//
//
//
//  gcc (including mingw)-specific macros
// *************************************************************************** //
# elif defined(__GNUC__)
    #define _CBAPP_WARN_UNUSED_PUSH                                         \
        _Pragma("GCC diagnostic push")                                      \
        _Pragma("GCC diagnostic warning \"-Wunused-private-field\"")
    #define _CBAPP_WARN_UNUSED_POP                                          \
        _Pragma("GCC diagnostic pop")
//
//
// other compilers: no-ops
// *************************************************************************** //
# else
    #define _CBAPP_WARN_UNUSED_PUSH
    #define _CBAPP_WARN_UNUSED_POP

// *************************************************************************** //
//
//
//
// *************************************************************************** //
#endif  //  defined(__clang__)  //







// *************************************************************************** //
//
//
//  3.  DEBUG BUILD.                    | __CBAPP_DEBUG__ MACROS...
// *************************************************************************** //
// *************************************************************************** //

#ifdef __CBAPP_DEBUG__
// *************************************************************************** //
//
//


    //#define     CBAPP_ENABLE_CB_DEMO                    1               //  Enable the "CBDemo" window with the application.
    
    
//
//
// *************************************************************************** //
#endif  //  __CBAPP_DEBUG__  //



//      3A.     __CBAPP_DEBUG__  OR  __CBLIB_RELEASE_WITH_DEBUG_INFO__
// *************************************************************************** //
// *************************************************************************** //

#if defined(__CBAPP_DEBUG__) || defined(__CBLIB_RELEASE_WITH_DEBUG_INFO__)
// *************************************************************************** //
//
//
    #define __CBAPP_LOG__
//
//
// *************************************************************************** //
#endif  //  __CBAPP_DEBUG__  ||  __CBLIB_RELEASE_WITH_DEBUG_INFO__












// *************************************************************************** //
//
//
//  4.  RELEASE BUILD.                  | __CBAPP_RELEASE__ MACROS...
// *************************************************************************** //
// *************************************************************************** //

#ifdef __CBAPP_RELEASE__
// *************************************************************************** //
//
//

    //
    //      ...
    //
    
//
//
// *************************************************************************** //
#endif  //  __CBAPP_RELEASE__  //












// *************************************************************************** //
//
//
//  5.  RELEASE WITH DEBUG INFO.        | __CBLIB_RELEASE_WITH_DEBUG_INFO__
// *************************************************************************** //
// *************************************************************************** //

#ifdef __CBLIB_RELEASE_WITH_DEBUG_INFO__
// *************************************************************************** //
//
//

    //
    //      ...
    //
    
//
//
// *************************************************************************** //
#endif  //  __CBLIB_RELEASE_WITH_DEBUG_INFO__  //












// *************************************************************************** //
//
//
//  6.  MINIMUM SIZE RELEASE BUILD.     | __CBLIB_MIN_SIZE_RELEASE__ MACROS...
// *************************************************************************** //
// *************************************************************************** //

#ifdef __CBLIB_MIN_SIZE_RELEASE__
// *************************************************************************** //
//
//

    //
    //      ...
    //
    
//
//
// *************************************************************************** //
#endif  //  __CBLIB_MIN_SIZE_RELEASE__  //









// *************************************************************************** //
//
//
//      3.1     APPLICATION APIs AND INTERNAL TYPENAME ALIASES...
// *************************************************************************** //
// *************************************************************************** //
//  MEMBER VARIABLE NAME FOR "AppState" OBJECT IN EACH CLASS...
//      - Using this to consosolidate the lengthy member-accessor statements "this->m_state.my_object.my_data_member", etc ...
#define                 CBAPP_STATE_NAME                S



//  _CBAPP_APPSTATE_ALIAS_API           |   ** INTERNAL **
//
//      - "API" USED **EXCLUSIVELY** INSIDE "AppState" CLASS.
//      - CLASS-NESTED, PUBLIC TYPENAME ALIASES FOR "CBApp" CLASSES THAT UTILIZE AN "AppState" OBJECT...
//
#define                 _CBAPP_APPSTATE_ALIAS_API                                                               \
public:                                                                                                         \
    using               CBSignalFlags                   = std::uint32_t;                                        \
                                                                                                                \
    using               Window                          = app::Window_t;                                        \
    using               Applet                          = app::Applet_t;                                        \
    using               Font                            = app::Font_t;                                          \
    using               Cmap                            = app::Colormap_t;                                      \
    using               Timestamp_t                     = std::chrono::time_point<std::chrono::system_clock>;   \
                                                                                                                \
    using               ImFonts                         = utl::EnumArray<Font, ImFont *>;                       \
    using               ImWindows                       = utl::EnumArray<Window, WinInfo>;                      \
    using               Anchor                          = utl::Anchor;                                          \
                                                                                                                \
    using               Logger                          = utl::Logger;                                          \
    using               LogLevel                        = Logger::Level;                                        \
    using               Tab_t                           = utl::Tab_t;



//  "CBAPP_CBLIB_TYPES_API"             |   ** EXPORTED**
//      - Exported API to provide alias for types afforded by "cblib".
//
#define                 CBAPP_CBLIB_TYPES_API                                                   \
    template<typename T_>                                                                       \
    using               Param                           = cblib::math::Param<T_>;               \
    template<typename T_>                                                                       \
    using               Range                           = cblib::math::Range<T_>;



//  "CBAPP_APPSTATE_ALIAS_API"          |   ** EXPORTED**
//
//      - "API" USED BY "CBApp" DELEGATOR CLASSES THAT USE AN "AppState" OBJECT.
//      - CLASS-NESTED, PUBLIC TYPENAME ALIASES FOR "CBApp" CLASSES THAT UTILIZE AN "AppState" OBJECT...
//
#define                 CBAPP_APPSTATE_ALIAS_API                                                \
    using               AppState                        = app::AppState;                        \
    using               WinInfo                         = app::WinInfo;                         \
                                                        CBAPP_CBLIB_TYPES_API                   \
                                                        _CBAPP_APPSTATE_ALIAS_API



// *************************************************************************** //
// *************************************************************************** //
//
//
// *************************************************************************** //               //  END.












// *************************************************************************** //
// *************************************************************************** //
//
//  END.








 //       using                   Vertex                          = Vertex_t      <VertexID>                              ;
 //       using                   Point                           = Point_t       <PointID>                               ;
 //       using                   Line                            = Line_t        <LineID>                                ;
 //       using                   Path                            = Path_t        <PathID, VertexID>                      ;
 //       using                   Overlay                         = Overlay_t     <OverlayID>                             ;
 //       using                   Hit                             = Hit_t         <HitID>                                 ;
 //       using                   PathHit                         = PathHit_t     <PathID, VertexID>                      ;
 //       using                   Selection                       = Selection_t   <VertexID, PointID, LineID, PathID>     ;

/***********************************************************************************
*
*       ********************************************************************
*       ****         C B A P P _ C O N F I G . h  ____  F I L E         ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
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
//
//      2.      MY-OWN GLOBALLY DEFINED PRE-PROCESSOR MACROS...
// *************************************************************************** //
// *************************************************************************** //



//              2.0.    PREVENT USER FROM "-DMY_FLAG_NAME" FOR INTERNAL MACRO NAMES...
// *************************************************************************** //
// *************************************************************************** //

# ifdef CBAPP_ENABLE_DEBUG_WINDOWS
    #   error "Cannot define \"CBAPP_ENABLE_DEBUG_WINDOWS\" externally (this macro is defined by my own headers)."
#endif  //  CBAPP_ENABLE_DEBUG_WINDOWS  //

# ifdef CBAPP_ENABLE_OPTIONAL_WINDOWS
    #   error "Cannot define \"CBAPP_ENABLE_OPTIONAL_WINDOWS\" externally (this macro is defined by my own headers)."
#endif  //  CBAPP_ENABLE_OPTIONAL_WINDOWS  //
    
    
    



//      2.1     CRITICAL BUILD SETTINGS...
// *************************************************************************** //
// *************************************************************************** //
//  #define         CBAPP_ENABLE_FUNCTIONAL_TESTING             1                   //  Compiles with procedure to enact FUNCTIONAL TESTING...

//#define         __CBAPP_DEBUG__                             1                 //  Enable delay before starting int main (bug-fix for X-Code IDE issue).
//  #define         __CBAPP_BUILD_CCOUNTER_APP__                    1               //  BUILD FOR COINCIDENCE COUNTER...
//  #define         __CBAPP_BUILD_FDTD_APP__                        1               //  BUILD FOR FDTD APP...
#define         __CBAPP_BUILD_EDITOR_APP__                  1                   //  BUILD FOR EDITOR / "ILLUSTRATOR" APP...


//  ENFORCE THAT THERE ARE NOT MULTIPLE BUILDS ARE #DEFINED...
//
#define         __CBAPP_BUILD_APP_COUNT__                   (   \
          __CBAPP_BUILD_CCOUNTER_APP__                          \
        + __CBAPP_BUILD_FDTD_APP__                              \
        + __CBAPP_BUILD_EDITOR_APP__                            \
)
        
#if __CBAPP_BUILD_APP_COUNT__ > 1
#   error "Cannot build with more than one __CBAPP_BUILD_APP__ macro defined (look in \"cbapp_config.h\")"
#endif



//      2.2     W-I-P APPLICATION OPTIONS...
// *************************************************************************** //
// *************************************************************************** //


//      2.3     DEBUGGING UTILITIES...
// *************************************************************************** //
// *************************************************************************** //
//  #define     CBAPP_NEW_DOCKSPACE                             1
#define     CBAPP_USE_NEW_GLFW_CALLBACKS                    1           //  Aug. 15, 2025:  USING "APP REGISTRY" TO OBTAIN "this" PTR INSIDE GLFW CALLBACK FUNCs...



//      2.4     APP SETTINGS...
// *************************************************************************** //
// *************************************************************************** //

#define     CBAPP_ENABLE_MOVE_AND_RESIZE                    1           //  Allow user to moving / resizing core windows.
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
//
//      3.      DEBUG BUILD.                    | __CBAPP_DEBUG__ MACROS...
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



//      3A.     __CBAPP_DEBUG__  OR  __CBLIB_RELEASE_WITH_DEBUG_INFO__      [ Common to BOTH of these configurations ]...
// *************************************************************************** //
// *************************************************************************** //

#if defined(__CBAPP_DEBUG__) || defined(__CBLIB_RELEASE_WITH_DEBUG_INFO__)
// *************************************************************************** //
//
//
    #ifdef IMGUI_DISABLE_DEBUG_TOOLS
        #   error "Cannot create DEBUG build with \"IMGUI_DISABLE_DEBUG_TOOLS\" defined (check that you did NOT #define this inside \"my_imconfig.h\")."
    #endif  //  IMGUI_DISABLE_DEBUG_TOOLS  //
    //
    //
    //
    #define     __CBAPP_LOG__                       1
    #define     CBAPP_ENABLE_DEBUG_WINDOWS          1
//
//
// *************************************************************************** //
#endif  //  __CBAPP_DEBUG__  ||  __CBLIB_RELEASE_WITH_DEBUG_INFO__  //












// *************************************************************************** //
//
//
//
//      4.      RELEASE BUILD.                  | __CBAPP_RELEASE__ MACROS...
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
//
//      5.      RELEASE WITH DEBUG INFO.        | __CBLIB_RELEASE_WITH_DEBUG_INFO__
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
//
//      6.      MINIMUM SIZE RELEASE BUILD.     | __CBLIB_MIN_SIZE_RELEASE__ MACROS...
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
//
//      3.1.    MAIN APPLICATION APIs AND INTERNAL TYPENAME ALIASES...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//
//              3.1A.   "CBLIB" ALIASES...
// *************************************************************************** //
// *************************************************************************** //


//  "CB_FDTD_TYPES_API"                             |   ** EXPORTED**
//      - Typename aliases to define types used for the FDTD engine.
// *************************************************************************** //
#define                 CB_FDTD_TYPES_API                                                       \
    template<typename T_>                                                                       \
    using               Param                           = cblib::math::Param<T_>;               \
    template<typename T_>                                                                       \
    using               Range                           = cblib::math::Range<T_>;
//
// *************************************************************************** //   CB_FDTD_TYPES_API



//  "CBAPP_CBLIB_TYPES_API"                         |   ** EXPORTED**
//      - Exported API to provide alias for types afforded by "cblib".
// *************************************************************************** //
#define                 CBAPP_CBLIB_TYPES_API                                                   \
    template<typename T_>                                                                       \
    using               Param                           = cblib::math::Param<T_>;               \
    template<typename T_>                                                                       \
    using               Range                           = cblib::math::Range<T_>;
//
// *************************************************************************** //   CBAPP_CBLIB_TYPES_API



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "CBLIB" ALIASES.












// *************************************************************************** //
//
//              3.1B.   "MAIN APPLICATION" ALIASES...
// *************************************************************************** //
// *************************************************************************** //

//  "CBAPP_STATE_NAME
//
//      MEMBER VARIABLE NAME FOR "AppState" OBJECT IN EACH CLASS...
//          - Using this to consosolidate the lengthy member-accessor statements "this->m_state.my_object.my_data_member", etc ...
// *************************************************************************** //
#define                 CBAPP_STATE_NAME                S
//
// *************************************************************************** //



//  _CBAPP_APPSTATE_INTERNAL_ALIAS_API              |   ** INTERNAL **
//
//      - "API" USED **EXCLUSIVELY** INSIDE "AppState" CLASS.
//      - CLASS-NESTED, PUBLIC TYPENAME ALIASES FOR "CBApp" CLASSES THAT UTILIZE AN "AppState" OBJECT...
// *************************************************************************** //
#define                 _CBAPP_APPSTATE_INTERNAL_ALIAS_API                                                      \
public:                                                                                                         \
    using               CBSignalFlags                   = std::uint32_t;                                        \
    using               CBMenuCapabilityFlags           = std::uint64_t;                                        \
                                                                                                                \
    using               Window                          = app::Window_t;                                        \
    using               Applet                          = app::Applet_t;                                        \
    using               Font                            = app::Font_t;                                          \
    using               Cmap                            = app::Colormap_t;                                      \
    using               Timestamp_t                     = std::chrono::time_point<std::chrono::system_clock>;   \
                                                                                                                \
    using               LabelFn                         = std::function<void(const char *)>;                    \
                                                                                                                \
                                                                                                                \
    using               ImFonts                         = cblib::EnumArray<Font, ImFont *>;                     \
    using               ImWindows                       = cblib::EnumArray<Window, WinInfo>;                    \
    using               Anchor                          = utl::Anchor;                                          \
                                                                                                                \
    using               Logger                          = utl::Logger;                                          \
    using               LogLevel                        = Logger::Level;                                        \
    using               Tab_t                           = utl::Tab_t;
//
// *************************************************************************** //   _CBAPP_APPSTATE_INTERNAL_ALIAS_API



//  "CBAPP_APPSTATE_ALIAS_API"                      |   ** EXPORTED**
//
//      - "API" USED BY "CBApp" DELEGATOR CLASSES THAT USE AN "AppState" OBJECT.
//      - CLASS-NESTED, PUBLIC TYPENAME ALIASES FOR "CBApp" CLASSES THAT UTILIZE AN "AppState" OBJECT...
// *************************************************************************** //
#define                 CBAPP_APPSTATE_ALIAS_API                                                \
    using               AppState                        = app::AppState;                        \
    using               WinInfo                         = app::WinInfo;                         \
                                                        CBAPP_CBLIB_TYPES_API                   \
                                                        _CBAPP_APPSTATE_INTERNAL_ALIAS_API
//                                                      
// *************************************************************************** //   CBAPP_APPSTATE_ALIAS_API



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "MAIN APP" ALIASES.












// *************************************************************************** //
//
//              3.1C.   "EDITOR APPLICATION" ALIASES...
// *************************************************************************** //
// *************************************************************************** //

//  "_EDITOR_APP_AUXILIARY_API"                     |   ** INTERNAL **
//
//      ** ID-VALUE TEMPLATE CONVENTIONS **
//      ID Template Parameters MUST ALWAYS follow this order:
//              template< typename VertexID, typename PointID, typename LineID, typename PathID, typename ZID, typename HitID >=
// *************************************************************************** //
#define                 _EDITOR_APP_AUXILIARY_API                                                                                               \
public:                                                                                                                                         \
    /*                                                                                                                                  */      \
    /*      1.      CALLBACK TYPES...                                                                                                   */      \
    using                           LabelFn                     = std::function<void(const char *)>                                     ;       \
    /*                                                                                                                                  */      \
    using                           MappingFn                   = ImVec2 (&)(ImVec2) noexcept                                           ;       \
    /*  using                           GetVertexFn                 = const Vertex * (*)(void * , VertexID) ; */
    
//
// *************************************************************************** //   _EDITOR_APP_AUXILIARY_API



//  "_EDITOR_APP_INTERNAL_API"                      |   ** EXPORTED**
//
// *************************************************************************** //
//
//      - "API" USED **EXCLUSIVELY** INSIDE "AppState" CLASS.
//      - CLASS-NESTED, PUBLIC TYPENAME ALIASES FOR "CBApp" CLASSES THAT UTILIZE AN "AppState" OBJECT...
//
//      ** ID-VALUE TEMPLATE CONVENTIONS **
//      ID Template Parameters MUST ALWAYS follow this order:
//              template< typename VertexID, typename PointID, typename LineID, typename PathID, typename ZID, typename HitID >
//
// *************************************************************************** //
#define                 _EDITOR_APP_INTERNAL_API                                                                                                \
public:                                                                                                                                         \
    /*                                                                                                                                  */      \
    /*      0.      OTHER MACROS...                                                                                                     */      \
    _EDITOR_APP_AUXILIARY_API                                                                                                           \
    /*                                                                                                                                  */      \
    /*      1.      GENERIC TYPES...                                                                                                    */      \
    using                           Font                        = app::Font_t                                                           ;       \
    using                           Logger                      = utl::Logger                                                           ;       \
    using                           LogLevel                    = utl::LogLevel                                                         ;       \
                                                                                                                                        \
    using                           CBCapabilityFlags           = CBCapabilityFlags_                                                    ;       \
    using                           Anchor                      = BBoxAnchor                                                            ;       \
                                                                                                                                        \
    using                           PopupHandle                 = EditorPopupBits                                                       ;       \
    using                           CBEditorPopupFlags          = CBEditorPopupFlags_                                                   ;       \
    using                           PopupInfo                   = EditorPopupInfo                                                       ;       \
    /*                                                                                                                                  */      \
    /*      2.      ID/INDEX TYPES...                                                                                                   */      \
    template<typename T, typename Tag>                                                                                                  \
    using                           ID                          = cblib::utl::IDType<T, Tag>                                            ;       \
    using                           VertexID                    = uint32_t                                                              ;       \
    using                           HandleID                    = uint8_t                                                               ;       \
    using                           PointID                     = uint32_t                                                              ;       \
    using                           LineID                      = uint32_t                                                              ;       \
    using                           PathID                      = uint32_t                                                              ;       \
    using                           ZID                         = uint32_t                                                              ;       \
    using                           OverlayID                   = uint8_t                                                               ;       \
    using                           HitID                       = uint32_t                                                              ;       \
    /*                                                                                                                                  */      \
    /*      3.      OBJECT TYPES...                                                                                                     */      \
    using                           Vertex                      = Vertex_t          <VertexID>                                          ;       \
/*  using                           Handle                      = Handle_t          <HandleID>;                                         */      \
    using                           Point                       = Point_t           <PointID>                                           ;       \
    using                           Line                        = Line_t            <LineID, ZID>                                       ;       \
    using                           Path                        = Path_t            <PathID, VertexID, ZID>                             ;       \
    using                           PathKind                    = Path::PathKind                                                        ;       \
    using                           Payload                     = Path::Payload                                                         ;       \
    using                           Overlay                     = Overlay_t         <OverlayID>                                         ;       \
    using                           Hit                         = Hit_t             <HitID>                                             ;       \
    using                           PathHit                     = PathHit_t         <PathID, VertexID>                                  ;       \
    using                           EndpointInfo                = EndpointInfo_t    <PathID>                                            ;       \
    /*                                                                                                                                  */      \
    /*      4.      PRIMARY STATE OBJECTS...                                                                                            */      \
    using                           EditorRuntime               = EditorRuntime_t   <VertexID, PointID, LineID, PathID, ZID, HitID>     ;       \
    using                           EditorState                 = EditorState_t     <VertexID, PointID, LineID, PathID, ZID, HitID>     ;       \
    using                           RenderState                 = RenderState_t     <VertexID, PointID, LineID, PathID, ZID, HitID>     ;       \
    using                           BrowserState                = BrowserState_t    <VertexID, PointID, LineID, PathID, ZID, HitID>     ;       \
    using                           IndexState                  = IndexState_t      <VertexID, PointID, LineID, PathID, ZID, HitID>     ;       \
    /*                                                                                                                                  */      \
    /*      5.      SUBSIDIARY STATE OBJECTS...                                                                                         */      \
    using                           Clipboard                   = Clipboard_t       <Vertex, Point, Line, Path>                         ;       \
    using                           Selection                   = Selection_t       <VertexID, PointID, LineID, PathID, ZID, HitID>     ;       \
    /*                                                                                                                                  */      \
    /*      6.      TOOL STATE OBJECTS...                                                                                               */      \
    using                           PenState                    = PenState_t        <VertexID>                                          ;       \
    using                           ShapeState                  = ShapeState_t      <OverlayID>                                         ;       \
    using                           DebuggerState               = DebuggerState_t   <VertexID, PointID, LineID, PathID, ZID, HitID>     ;       \
    /*                                                                                                                                  */      \
    /*                                                                                                                                  */      \
    /*                                                                                                                                  */      \
    /*      7.      MORE CALLBACKS...                                                                                                   */      \
    using                           GetVertexFn                 = const Vertex* (&)(const std::vector<Vertex>&, VertexID) noexcept      ;       \
    /*                                                                                                                                  */      \
    /*      8.      OTHER OBJECT TYPES...                                                                                               */      \
    using                           OverlayManager              = OverlayManager_t  <OverlayID, MappingFn>                              ;       \
    using                           ResidentEntry               = ResidentEntry_t   <OverlayID>                                         ;       \
    using                           VertexStyle                 = VertexStyle       <MappingFn>                                         ;       \
    using                           VertexStyleType             = VertexStyle::StyleType                                                ;       \
    using                           RenderCTX                   = RenderCTX_t       <Vertex, MappingFn, GetVertexFn>                    ;
//
// *************************************************************************** //   _EDITOR_APP_INTERNAL_API



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "EDITOR APP" ALIASES.












// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //               //  END "NAMESPACE API MACROS".












// *************************************************************************** //
// *************************************************************************** //
//
//  END.

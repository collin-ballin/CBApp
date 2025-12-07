/***********************************************************************************
*
*       ********************************************************************
*       ****                S T A T E . h  ____  F I L E                ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      May 01, 2025.
*
*       ********************************************************************
*                FILE:      [include/app/state/_state.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_APP_STATE_H
#define _CBAPP_APP_STATE_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  1.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
#include "utility/utility.h"
#include "widgets/widgets.h"
//
#include "app/state/_sub_state.h"
//
#include "app/state/_init.h"
#include "app/state/_config.h"
#include "app/state/_types.h"
//  #include "app/_init.h"



//  1.2     STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <cstdlib>          // C-Headers...
#include <stdio.h>
#include <unistd.h>
#include <chrono>

#include <string>           //  <======| std::string, ...
#include <string_view>
#include <string_view>
#include <vector>           //  <======| std::vector, ...
#include <stdexcept>        //  <======| ...
#include <functional>
#include <limits.h>
#include <math.h>
#include <atomic>



//  1.3     "DEAR IMGUI" HEADERS...
#include "imgui.h"
#include "implot.h"
#include "imgui_internal.h"






namespace cb { namespace app { //     BEGINNING NAMESPACE "cb" :: "app"...
// *************************************************************************** //
// *************************************************************************** //






//  "AppState"
//      PLAIN-OLD-DATA (POD) ABSTRACTION/CONTAINER TO
//      FACILITATE MANUAL DEPENDENCY INJECTION DESIGN PATTERN...
//
class AppState
{
//      0.          CONSTANTS AND ALIASES...
// *************************************************************************** //
// *************************************************************************** //
public:

    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    _CBAPP_APPSTATE_INTERNAL_ALIAS_API       //  CLASS-DEFINED, NESTED TYPENAME ALIASES.
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    static constexpr float              ms_LABEL_WIDTH                              = 90.0f;
    static constexpr float              ms_WIDGET_WIDTH                             = 250.0f;
    //
    LabelFn                             ms_LeftLabel;
    LabelFn                             ms_TopLabel;
    LabelFn                             ms_LabelUnformat;
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      REFERENCES TO GLOBAL ARRAYS.
    // *************************************************************************** //
    
    //      std::array< std::string *, static_cast<size_t>(Applet_t::Count) >       //  No CONSTEXPR arr for this bc I want it to copy the
    //                                          m_applets                       = {};   //  window names EXACTLY in case we ever rename them.
    static constexpr auto &             ms_APP_COLOR_STYLE_NAMES        = APPLICATION_COLOR_STYLE_NAMES;        //  m_app_color_style_names
    static constexpr auto &             ms_PLOT_COLOR_STYLE_NAMES       = APPLICATION_PLOT_COLOR_STYLE_NAMES;   //  m_plot_color_style_names
    static constexpr auto &             ms_DOCK_LOCATION_NAMES          = APPLICATION_DOCK_LOCATION_NAMES;
    static constexpr AppleSystemColors_t
                                        SystemColor                     = {   };
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      STATIC CONSTEXPR VALUES.
    // *************************************************************************** //
    //                              WINDOW VARIABLES:
    //                                  ALL Windows.
    static constexpr size_t             ms_WINDOWS_BEGIN            = static_cast<size_t>(Window::Dockspace);
    static constexpr size_t             ms_RHS_WINDOWS_BEGIN        = static_cast<size_t>(Window::HomeApp);
    static constexpr size_t             ms_WINDOWS_END              = static_cast<size_t>(Window::Count);
    //
    //
    //                                  Main Application Windows.
    static constexpr size_t             ms_APP_WINDOWS_BEGIN        = static_cast<size_t>(Window::CCounterApp);
    static constexpr size_t             ms_APP_WINDOWS_END          = static_cast<size_t>(Window::ImGuiStyleEditor);
    //
    //                                  Basic Tool Windows.
    static constexpr size_t             ms_TOOL_WINDOWS_BEGIN       = static_cast<size_t>(Window::ImGuiStyleEditor);
    static constexpr size_t             ms_TOOL_WINDOWS_END         = static_cast<size_t>(Window::Logs);
    //
    //                                  Custom Tools Windows.
    static constexpr size_t             ms_MY_TOOLS_WINDOWS_BEGIN   = static_cast<size_t>(Window::Logs);
    static constexpr size_t             ms_MY_TOOLS_WINDOWS_END     = static_cast<size_t>(Window::AboutMyApp);
    //
    //                                  Misc Windows.
    static constexpr size_t             ms_MISC_WINDOWS_BEGIN       = static_cast<size_t>(Window::AboutMyApp);
#if defined( CBAPP_ENABLE_OPTIONAL_WINDOWS )
    static constexpr size_t             ms_MISC_WINDOWS_END         = static_cast<size_t>(Window::CBDemo);                  //  1.5A.   CB_DEMO IS ENABLED.
# elif defined( CBAPP_ENABLE_DEBUG_WINDOWS ) && !defined( CBAPP_ENABLE_OPTIONAL_WINDOWS )
    static constexpr size_t             ms_MISC_WINDOWS_END         = static_cast<size_t>(Window::ImGuiItemPickerTool);     //  1.5B.   CB_DEMO IS *DISABLED*
# else                                                                                                                      //          *AND* DEBUG TOOLS ARE ENABLED.
    static constexpr size_t             ms_MISC_WINDOWS_END         = static_cast<size_t>(Window::Count);                   //  DEFAULT CASE.
#endif  //  CBAPP_ENABLE_OPTIONAL_WINDOWS  ||  CBAPP_ENABLE_DEBUG_WINDOWS  //
    //
    //
    //                              DEBUGGER TOOLS:
# if defined(CBAPP_ENABLE_DEBUG_WINDOWS)
    static constexpr size_t             ms_DEBUG_WINDOWS_BEGIN      = static_cast<size_t>(Window::CBDebugger);
    #if defined( CBAPP_ENABLE_OPTIONAL_WINDOWS )
        static constexpr size_t             ms_DEBUG_WINDOWS_END        = static_cast<size_t>(Window::CBDemo);              //  2.1A.   CB_DEMO ENABLED.
    # else
        static constexpr size_t             ms_DEBUG_WINDOWS_END        = static_cast<size_t>(Window::Count);               //  2.1B.   CB_DEMO *DISABLED*.
    #endif  //  CBAPP_ENABLE_OPTIONAL_WINDOWS  //
#endif  //  CBAPP_ENABLE_DEBUG_WINDOWS  //
    //
    //
    //
    //                              EXTRA WINDOWS:
#if defined(CBAPP_ENABLE_OPTIONAL_WINDOWS)
    # if defined(CBAPP_ENABLE_DEBUG_WINDOWS)
        static constexpr size_t             ms_EXTRA_WINDOWS_BEGIN      = static_cast<size_t>(Window::CBDemo);      //  2.2A.   DEBUG TOOLS ARE ENABLED.
    # else
        static constexpr size_t             ms_EXTRA_WINDOWS_BEGIN      = static_cast<size_t>(Window::CBDemo);      //  2.2B.   DEBUG TOOLS ARE *DISABLED*.
    # endif     //  CBAPP_ENABLE_DEBUG_WINDOWS  //
    static constexpr size_t             ms_EXTRA_WINDOWS_END        = static_cast<size_t>(Window::Count);
#endif  //  CBAPP_ENABLE_OPTIONAL_WINDOWS  //
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "0.  CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//
//      1.          CLASS DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      MAIN OBJECTS.
    // *************************************************************************** //
    //                              GROUPS / SUB-CLASSES OF "APPSTATE":
    utl::Logger &                       m_logger;                                                   //  1.      LOGGER...
    LogLevel                            m_LogLevel;
    //
    //
    //                              WINDOW MANAGEMENT:
    ImWindows                           m_windows;                                                  //  2.      ARRAY OF *ALL* APPLICATION WINDOWS...
    //
    std::vector<WinInfo *>              m_home_windows                  = {   };                    //  2.1.        Home WINDOWS.
    std::vector<WinInfo *>              m_detview_windows               = {   };                    //  2.2.        DetView WINDOWS.
    //
    //
    //                              FONT / APPLICATION UI-SCALE:
    ImFonts                             m_fonts;                                                    //  3.      APPLICATION FONTS...
    UIScaler                            m_ui_scaler;                                                //  4.      GUI-SCALER OBJECT...
    //
    //
    //                              OTHER / SMALLER:
    std::vector< std::pair<Timestamp_t, std::string> >
                                        m_notes                         = {   };
    Timestamp_t                         m_timestamp_spawn;
    Timestamp_t                         m_timestamp_start;
    //
    CBAppSettings &                     m_settings                      = CBAppSettings::instance();
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      SUB-STATE INFORMATION.
    // *************************************************************************** //
    TaskState_t                         m_task_state;
    //
    //
#ifndef __CBAPP_BUILD_CCOUNTER_APP__
    AppColorStyle_t                     m_current_app_color_style       = AppColorStyle_t::Default;
# else
    AppColorStyle_t                     m_current_app_color_style       = AppColorStyle_t::Laser_410NM;
#endif  //  __CBAPP_BUILD_CCOUNTER_APP__  //
    PlotColorStyle_t                    m_current_plot_color_style      = PlotColorStyle_t::Default;
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      OTHER IMPORTANT DATA.
    // *************************************************************************** //
    cb::FileDialog::Initializer         m_dialog_settings               = {
    //
    //                              "OPEN" SETTINGS...                      //  |   "SAVE" SETTINGS...
        /* type               = */  cb::FileDialog::Type::Open,
        /* window_name        = */  "File Dialog",
        /* default_filename   = */  "",                                     //  |       "canvas_settings.json",
        /* required_extension = */  "",                                     //  |       "json",
        /* valid_extensions   = */  {".json", ".cbjson", ".txt"},           //  |       {".json", ".txt"}
        /* starting_dir       = */  std::filesystem::current_path()         //  |       std::filesystem::current_path(),
    //
    };
    cb::FileDialog                      m_file_dialog;
    bool                                m_dialog_queued                 = false;
    bool                                m_dialog_path                   = false;
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      REFERENCE TO CONSTEXPR GLOBAL ARRAYS.
    // *************************************************************************** //



    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      GLFW AND SYSTEM INFORMATION.
    // *************************************************************************** //
    //
    //                              SYSTEM INFO:
#ifdef _WIN32
    const DWORD                         m_process_id        ;
#else
    const pid_t                         m_process_id        ;
#endif  //  _WIN32  //
    const std::thread::id               m_main_thread_id    ;
    //
    //
    //                              GLFW AND HOST WINDOW STUFF:
    const char *                        m_glsl_version                  = nullptr;
    ImGuiConfigFlags                    m_io_flags                      = ImGuiConfigFlags_None | ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad | ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
    ImGuiViewport *                     m_main_viewport                 = nullptr;
    GLFWwindow *                        m_glfw_window                   = nullptr;
    int                                 m_glfw_interval                 = 1;    //  Use 1 for VSYNC ENABLED.


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      UI / STYLE / APPEARANCE INFORMATION.
    // *************************************************************************** //
    //
    //                              COLORS:
    //ImVec4                              m_glfw_bg                   = cb::app::DEF_ROOT_WIN_BG;
    //ImVec4                              m_dock_bg                   = cb::app::DEF_INVISIBLE_COLOR;
    //ImVec4                              m_main_bg                   = cb::app::DEF_MAIN_WIN_BG;
    //
    //ImVec4                              m_controlbar_bg             = cb::app::DEF_CONTROLBAR_WIN_BG;
    //
    //
    //                              BROWSER DIMENSIONS:
    ImVec2                              m_browser_item_spacing          = ImVec2(  7.0f     ,  7.0f );
    ImVec2                              m_browser_window_padding        = ImVec2( 10.0f     , 10.0f );
    //
    float                               m_browser_child_rounding        = 6.5f;
    float                               m_browser_child_bs              = 2.0f;
    //
    //
    //                              INFO DIMENSIONS:
    ImVec2                              m_info_item_spacing             = ImVec2(  2.0f     ,  2.0f );
    ImVec2                              m_info_window_padding           = ImVec2( 5.0f     , 5.0f );
    //
    float                               m_info_child_rounding           = 6.5f;
    float                               m_info_child_bs                 = 2.0f;
    //
    //
    //
    //
    //float                               m_browser_child_rounding    = 8.0f;
    //
    //                              DETAILVIEW COLORS:
    //ImVec4                              m_detview_bg                = cb::app::DEF_DETVIEW_WIN_BG;
    
    

    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      TRANSIENT APPLICATION STATE / DIMENSIONS.
    // *************************************************************************** //
    
    //                              SYSTEM:
    int                                 m_system_w                  = -1;       //  Sys. Display Dims.
    int                                 m_system_h                  = -1;
    float                               m_dpi_scale                 = 1.0f;
    float                               m_dpi_fontscale             = 1.0f;
    //
    //
    //                              MAIN UI:
    int                                 m_window_w                  = 1280;
    int                                 m_window_h                  = 720;
    //                              BOOLEANS:
    std::atomic<CBSignalFlags>          m_pending                   = { CBSignalFlags_None };
    std::atomic<bool>                   m_running                   = { true };
    bool                                m_rebuild_dockspace         = false;
    //
    //
    //                              DIFFERENT WINDOWS:
    bool                                m_show_controlbar_window    = true;
    bool                                m_show_browser_window       = true;
    bool                                m_show_detview_window       = true;
    


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      APPLICATION BEHAVIOR TOGGLES.
    // *************************************************************************** //
    bool                                m_show_system_preferences   = true;
    bool                                ms_FOCUS_ON_OPEN_WINDOW     = true;     //  Set focus after "Window > Show > Open_Window_Name"...
    

    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      SPECIFICS.
    // *************************************************************************** //
    //                              MAIN DOCKINGSPACE:
    const char *                        m_dock_name                 = "##RootDockspace";
    ImGuiID                             m_dockspace_id              = 0;
    //
    ImGuiID                             m_main_dock_id              = 0;
    ImGuiDockNodeFlags                  m_main_node_flags           = ImGuiDockNodeFlags_NoDockingOverOther | ImGuiDockNodeFlags_CentralNode | ImGuiDockNodeFlags_NoCloseButton; //ImGuiDockNodeFlags_NoDocking; ImGuiDockNodeFlags_NoDockingOverMe
    ImGuiDockNode *                     m_main_node                 = nullptr;
    //
    //                              CONTROL BAR:
    ImGuiID                             m_controlbar_dock_id        = 0;
    ImGuiDockNodeFlags                  m_controlbar_node_flags     = ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_HiddenTabBar | ImGuiDockNodeFlags_NoResize; //ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoCloseButton;      //  ImGuiDockNodeFlags_NoSplit
    ImGuiDockNode *                     m_controlbar_node           = nullptr;
    //
    float                               m_controlbar_ratio          = 0.02;
    //
    //                              BROWSER / SIDEBAR:
    ImGuiID                             m_browser_dock_id           = 0;
    ImGuiDockNodeFlags                  m_browser_node_flags        = ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoCloseButton;      //  ImGuiDockNodeFlags_NoSplit
    ImGuiDockNode *                     m_browser_node              = nullptr;
    //
    float                               m_browser_ratio             = app::DEF_BROWSER_RATIO;
    //
    //                              DETAIL VIEW:
    ImGuiID                             m_detview_dock_id           = 0;
    ImGuiDockNodeFlags                  m_detview_node_flags        = ImGuiDockNodeFlags_NoDockingOverOther | ImGuiDockNodeFlags_NoCloseButton;      //  ImGuiDockNodeFlags_NoSplit        //  ImGuiDockNodeFlags_NoUndocking
    ImGuiDockNode *                     m_detview_node              = nullptr;
    //
    float                               m_detview_ratio             = 0.45f;
    //
    //                              DETAIL VIEW DOCKSPACE:
    static constexpr const char *       m_detview_dockspace_uuid    = "##DetailViewDockspace";
    ImGuiID                             m_detview_dockspace_id      = 0;
    ImGuiDockNodeFlags                  m_detview_dockspace_flags   = ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoCloseButton;// | ImGuiDockNodeFlags_HiddenTabBar;      //  ImGuiDockNodeFlags_NoSplit
    //  ImGuiDockNodeFlags                  m_detview_window_flags      = ImGuiDockNodeFlags_HiddenTabBar; // ImGuiDockNodeFlags_NoTabBar;      //  ImGuiDockNodeFlags_NoSplit
    ImGuiDockNodeFlags                  m_detview_window_flags      = ImGuiDockNodeFlags_HiddenTabBar | ImGuiDockNodeFlags_NoDockingSplitOther; // ImGuiDockNodeFlags_NoTabBar;      //  ImGuiDockNodeFlags_NoSplit
    //
    //                              ADD MORE SHARED STATE DATA MEMBERS HERE:
    //                                  SubCategory.
    //
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "1.  CLASS DATA-MEMBERS".



// *************************************************************************** //
//
//
//      2.A.        MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
//
    // *************************************************************************** //
    //      CLASS INITIALIZATIONS.          |   "init.cpp" ...
    // *************************************************************************** //
    
    //  "instance"                          | Meyers-Style Singleton.       Created on first call, once.
    static inline AppState &            instance                    (void)
    { static AppState   single = AppState();     return single; }
    //
    //                      DELETED OPERATORS AND FUNCTIONS:
                                        AppState                    (const AppState &   )       = delete;   //  Copy. Constructor.
                                        AppState                    (AppState &&        )       = delete;   //  Move Constructor.
    AppState &                          operator =                  (const AppState &   )       = delete;   //  Assgn. Operator.
    AppState &                          operator =                  (AppState &&        )       = delete;   //  Move-Assgn. Operator.
    
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      PRIVATE INITIALIZATIONS.        |   "init.cpp" ...
    // *************************************************************************** //
protected:
                                        AppState                    (void);                 //  Default Constructor.
                                        ~AppState                   (void);                 //  Default Destructor.
    
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      LOADING FUNCTIONS.              |   "state.cpp" ...
    // *************************************************************************** //
public:
    //                              INITIALIZATION ORCHESTRATOR FUNCTIONS:
    void                                load                        (void);
    //
    //
protected:
    //                              PRIVATE INITIALIZATION FUNCTIONS:
    inline void                             _load                       (void);
    void                                    _load_ini                   (void);
    void                                    _load_imgui_style           (void);
    void                                    _load_implot_style          (void);
    
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      MAIN OPERATION FUNCTIONS.       |   "state.cpp" ...
    // *************************************************************************** //
public:
    void                                init_ui_scaler              (void);
    void                                RebuildFonts                (const float scale);


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      APPEARANCE HELPER FUNCTIONS.    |   "appearance.cpp" ...
    // *************************************************************************** //
    void                                SetAppColorStyle            (const AppColorStyle_t);
    void                                SetPlotColorStyle           (const PlotColorStyle_t);
    void                                SetDarkMode                 (void);
    void                                SetLightMode                (void);
    void                                LoadCustomColorMaps         (void);
    
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      APPEARANCE OPERATION FUNCTIONS. |   "appearance.cpp" ...
    // *************************************************************************** //
    //
    //                              APPLICATION UTILITIES:
    //                                  ...
    //
    //
    //                              SERIALIZATION ITEMS:
    //  std::optional<Applet>               m_hallpass                  = { std::nullopt };
    //
    void                                FileDialogHandler           ( void );
    template<typename Callback>
    bool                                SaveWithValidation          ( const std::filesystem::path & ,   Callback &,
                                                                      const std::string_view &      ,   const std::string_view & msg="callback function returned failure");
    
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      MISC. HELPER FUNCTIONS.         |   "appearance.cpp" ...
    // *************************************************************************** //
    void                                log_startup_info            (void) noexcept;
    void                                log_shutdown_info           (void) noexcept;
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "2A.  MEMBER FUNCS".

    
   
// *************************************************************************** //
//
//
//      2.B.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
    // *************************************************************************** //
    //      STATIC UTILITY FUNCTIONS.
    // *************************************************************************** //
    
    //  "ConditionalTextf"
    static inline bool                  ConditionalTextf            (const bool value, const char * fmt, ...) noexcept {
        if ( !value )       { return value; }
        va_list args;
        va_start(args, fmt);    ImGui::TextV(fmt, args);    va_end(args);
        return value;
    }
    


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      ORCHID FUNCTIONS.
    // *************************************************************************** //
    
    inline void                                     OrchidPush                  (OrchidAction && act) noexcept      { this->GetMenuState().orchid_push( std::move(act) );   }
    inline void                                     OrchidUndo                  (void) noexcept                     { this->GetMenuState().orchid_undo();                   }
    inline void                                     OrchidRedo                  (void) noexcept                     { this->GetMenuState().orchid_redo();                   }
    //
    [[nodiscard]] Orchid::size_type                 Orchid_undo_count           (void) noexcept                     { return this->GetMenuState().undo_count();             }
    [[nodiscard]] Orchid::size_type                 Orchid_redo_count           (void) noexcept                     { return this->GetMenuState().redo_count();             }
    
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      QUERY FUNCTIONS.
    // *************************************************************************** //
    
    //  "IsDetViewOpen"
    [[nodiscard]] inline bool                       IsDetViewOpen               (void) const noexcept   { return this->m_show_detview_window;       }
    [[nodiscard]] inline bool                       IsBrowserOpen               (void) const noexcept   { return this->m_show_browser_window;       }
    [[nodiscard]] inline bool                       IsControlBarOpen            (void) const noexcept   { return this->m_show_controlbar_window;    }
    
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      MAIN GETTER FUNCTIONS.
    // *************************************************************************** //
    
    //  "GetProcessID"
#ifdef _WIN32
    [[nodiscard]] inline DWORD                      GetProcessID                (void) const noexcept   { return this->m_process_id; }
#else
    [[nodiscard]] inline pid_t                      GetProcessID                (void) const noexcept   { return this->m_process_id; }
#endif  //  _WIN32  //
    [[nodiscard]] inline auto                       GetMainThreadID             (void) const noexcept   { return std::hash<std::thread::id>{}(this->m_main_thread_id); }
    
    
    
    //  "GetCurrentApplet"
    [[nodiscard]] inline Applet                     GetCurrentApplet            (void) const noexcept   { return this->m_task_state.m_current_task; }
    
    //  "GetCurrentAppletName"
    [[nodiscard]] inline const char *               GetCurrentAppletName        (void) noexcept         { return this->m_task_state.GetCurrentAppletName(); }
    
    
    //  "GetMainDockNodeID"
    [[nodiscard]] inline ImGuiDockNode *            GetMainDockNodeID           (void) noexcept         { return ImGui::DockBuilderGetNode( this->m_main_dock_id ); }
    [[nodiscard]] inline ImGuiDockNode const *      GetMainDockNodeID           (void) const noexcept   { return ImGui::DockBuilderGetNode( this->m_main_dock_id ); }
    
    //  "GetMainDockedWindowID"
    [[nodiscard]] inline ImGuiID                    GetMainDockedWindowID       (void) const noexcept   {
        const ImGuiDockNode *   node    = ImGui::DockBuilderGetNode( this->m_main_dock_id );    IM_ASSERT(node != nullptr);
        return ( (node->TabBar) ? node->TabBar->VisibleTabId  : 0 );
    }
    
    
    //  "GetDetViewDockNodeID"
    [[nodiscard]] inline ImGuiDockNode *            GetDetViewDockNodeID        (void) noexcept         { return ImGui::DockBuilderGetNode( this->m_detview_dockspace_id ); }
    [[nodiscard]] inline ImGuiDockNode const *      GetDetViewDockNodeID        (void) const noexcept   { return ImGui::DockBuilderGetNode( this->m_detview_dockspace_id ); }
    
    //  "GetDetViewDockedWindowID"
    [[nodiscard]] inline ImGuiID                    GetDetViewDockedWindowID    (void) const noexcept   {
        const ImGuiDockNode *   node    = ImGui::DockBuilderGetNode( this->m_detview_dockspace_id );    IM_ASSERT(node != nullptr);
        return ( (node->TabBar) ? node->TabBar->VisibleTabId  : 0 );
    }
    
    
    //  "GetNavWindowName"
    [[nodiscard]] inline const char *               GetNavWindowName            (void) noexcept         { return this->m_task_state.m_nav_window_name.c_str(); }
    
    //  "GetNavWindowStr"
    [[nodiscard]] inline const std::string &        GetNavWindowStr             (void) noexcept         { return this->m_task_state.m_nav_window_name; }
    
    //  "GetMenuState"
    [[nodiscard]] inline MenuState_t &              GetMenuState                (void) noexcept         { return this->m_task_state.m_current_menu_state.get(); }
    [[nodiscard]] inline const MenuState_t &        GetMenuState                (void) const noexcept   { return this->m_task_state.m_current_menu_state.get(); }
    
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      SECONDARY GETTER FUNCTIONS.
    // *************************************************************************** //
    
    //  "GetUIScaler"
    [[nodiscard]] inline UIScaler &                 GetUIScaler                 (void) noexcept         { return this->m_ui_scaler; }
    [[nodiscard]] inline const UIScaler &           GetUIScaler                 (void) const noexcept   { return this->m_ui_scaler; }
    
    //  "GetUIColor"
    [[nodiscard]] inline const ImVec4 &             GetUIColor                  (const UIColor col) const noexcept      { return CBAppSettings::ms_UI_COLORS[col]; }
    
    

    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      SETTER FUNCTIONS.
    // *************************************************************************** //
    
    //  "SetMenuState"
    inline void                         SetMenuState                (MenuState_t & src)     { this->m_task_state.m_current_menu_state = src; }
    
    //  "ResetMenuState"
    inline void                         ResetMenuState              (void)                  { this->m_task_state.m_current_menu_state = this->m_task_state.m_default_menu_state; }

    //  "current_app_color_style"
    inline const char *                 current_app_color_style     (void) const
    {  return this->ms_APP_COLOR_STYLE_NAMES[ static_cast<size_t>(this->m_current_app_color_style) ];  }

    //  "current_plot_color_style"
    inline const char *                 current_plot_color_style    (void) const
    {  return this->ms_PLOT_COLOR_STYLE_NAMES[ static_cast<size_t>(this->m_current_plot_color_style) ];  }
    
    

    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      SMALL HELPER FUNCTIONS.
    // *************************************************************************** //

    //  "HeaderSeparatorText"
    inline void                         HeadlineSeparatorText       (const char * text) noexcept {
        this->PushFont(Font::Main);     ImGui::PushStyleColor(ImGuiCol_Text, app::DEF_APPLE_BLUE);
            ImGui::SeparatorText(text);
        ImGui::PopStyleColor();         this->PopFont();
    }
    
    //  "DisabledSeparatorText"
    inline void                         DisabledSeparatorText       (const char * text) noexcept {
        ImGui::PushStyleColor( ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_TextDisabled) );
            ImGui::SeparatorText(text);
        ImGui::PopStyleColor();
    }

    //  "labelf"
    inline void                         labelf                      (const char * text, const float l_width=ms_LABEL_WIDTH, const float w_width=ms_WIDGET_WIDTH)
    {
        const ImVec2        padding             = ImVec2( l_width - ImGui::CalcTextSize(text).x,  0.0f );
        ImGui::AlignTextToFramePadding();       ImGui::SetNextItemWidth(l_width);
        ImGui::TextUnformatted(text);           ImGui::SameLine();
        ImGui::Dummy(padding);
        ImGui::SetNextItemWidth(w_width);       ImGui::SameLine();
        return;
    };
    //
    inline bool                         labelf                      (const bool vis, const char * text, const float l_width=ms_LABEL_WIDTH, const float w_width=ms_WIDGET_WIDTH)
    {
        if (!vis)   { return false; }
        const ImVec2        padding             = ImVec2( l_width - ImGui::CalcTextSize(text).x,  0.0f );
        ImGui::AlignTextToFramePadding();       ImGui::SetNextItemWidth(l_width);
        ImGui::TextUnformatted(text);           ImGui::SameLine();
        ImGui::Dummy(padding);
        ImGui::SetNextItemWidth(w_width);       ImGui::SameLine();
        return vis;
    };



    //  "_LeftLabel"
    inline void                         _LeftLabel                  (const char * text, const float l_width=ms_LABEL_WIDTH, const float w_width=ms_WIDGET_WIDTH)
    {
        const ImVec2        padding             = ImVec2( l_width - ImGui::CalcTextSize(text).x,  0.0f );
        ImGui::AlignTextToFramePadding();       ImGui::SetNextItemWidth(l_width);
        ImGui::TextUnformatted(text);           ImGui::SameLine();
        ImGui::Dummy(padding);
        ImGui::SetNextItemWidth(w_width);       ImGui::SameLine();
        return;
    };

    //  "_TopLabel"
    inline void                         _TopLabel                   (const char * text, const float l_width=ms_LABEL_WIDTH, const float w_width=ms_WIDGET_WIDTH)
    {
        ImGui::AlignTextToFramePadding();
        ImGui::SetNextItemWidth(l_width);
        ImGui::TextUnformatted(text);
        ImGui::SetNextItemWidth(w_width);
        return;
    };

    //  "_LabelUnformat"
    inline void                         _LabelUnformat              (const char * text)
    {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(text);
        return;
    };
    
    
    
    
    //  "column_label"
    //  inline void                         column_label                (const char * label)
    //  { this->PushFont(Font::FootNote);     ImGui::TextDisabled("%s", label);     this->PopFont(); }
    
    //  "column_label"
    inline void                         column_label                (const char * fmt, ...) {
        this->PushFont(Font::FootNote);
            va_list args;                       va_start(args, fmt);
            ImGui::TextDisabledV(fmt, args);    va_end(args);  // forward using the va_list version
        this->PopFont();
    }
    
    //  "print_TF"
    inline void                         print_TF                    (const bool value, const char * true_text = "ENABLED", const char * false_text = "DISABLED")
    {
        ImGui::TextColored(
            (value)     ? this->SystemColor.Green           : this->SystemColor.Red ,
            "%s",
            (value)     ? true_text                         : false_text
        );
        return;
    }
    
    //  "TFColoredText"
    inline void                         TFColoredText               (const bool value, const char * text) const noexcept
    {
        ImGui::TextColored(
              (value)   ? this->SystemColor.Green       : this->SystemColor.Red
            , "%s"
            , text
        );
        return;
    }
    
    //  "ConditionalText"
    inline bool                         ConditionalText             (const bool value, const char * text, const std::optional<ImVec4> & color = std::nullopt) noexcept
    {
        if (!value)     { return false; }
        ImGui::TextColored( color.value_or(this->SystemColor.White), "%s", text );
        return value;
    }
    


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      UTILITY FUNCTIONS.
    // *************************************************************************** //
    
    //  "_get_item_under_cursor"
    [[nodiscard]] inline utl::HoverItem             _get_item_under_cursor      (void) const noexcept
    {
        utl::HoverItem      out             {  };
        ImGuiContext *      ctx             = ImGui::GetCurrentContext();
        ImGuiID             hovered         = ctx->HoveredIdPreviousFrame;
        if ( !hovered )     { return out; }

        out.id          = hovered;
        out.window      = ctx->HoveredWindow;
        out.viewport    = (out.window && out.window->Viewport)
                            ? out.window->Viewport
                            : ImGui::FindViewportByID(ctx->IO.MouseHoveredViewport);

        // Exact bounding-box only available if this item was the LAST one
        // submitted in this frame (typical when you query inside the same UI).
        if (ctx->LastItemData.ID == hovered)
        {
            out.has_rect   = true;
            out.rect       = ctx->LastItemData.Rect;
            out.frame_stamp= ctx->FrameCount;
        }
        return out;
    }
    
    
    
    
    
    
    
    // *************************************************************************** //
    // *************************************************************************** //
/*
	inline void schedule_builtin_highlight(ImGuiID id, int frames = 2) noexcept
	{
		if (!id) return;
		ImGuiContext* ctx = ImGui::GetCurrentContext();
		if (!ctx) return;
		// Internal debug locate: core will draw the yellow box when the item with this ID is submitted.
		ctx->DebugLocateId      = id;
		ctx->DebugLocateFrames  = (ctx->DebugLocateFrames > frames ? ctx->DebugLocateFrames : frames);
	}




    inline bool highlight_hover_item(const utl::HoverItem & it,
	                                       float pad = 1.5f,
	                                       float thickness = 2.0f,
	                                       ImU32 color = IM_COL32(255,255,0,255),
	                                       bool show_clip_rect = false) noexcept
	{
		if (!it.id) return false;

		ImGuiContext* ctx = ImGui::GetCurrentContext();
		ImGuiViewport* vp = it.viewport ? it.viewport : ImGui::GetMainViewport();
		if (!vp) return false;

		// If the hovered item is also the last submitted item, we have an exact rect NOW.
		if (it.has_rect && ctx && it.frame_stamp == ctx->FrameCount)
		{
			ImDrawList* dl = ImGui::GetForegroundDrawList(vp);
			ImRect      r  = it.rect;
			r.Min.x -= pad; r.Min.y -= pad;
			r.Max.x += pad; r.Max.y += pad;

			dl->AddRect(r.Min, r.Max, color, 0.0f, 0, thickness);
			dl->AddLine(r.Min, r.Max, color, thickness * 0.5f);
			dl->AddLine(ImVec2(r.Min.x, r.Max.y), ImVec2(r.Max.x, r.Min.y), color, thickness * 0.5f);

			if (show_clip_rect && it.window)
				ImGui::GetForegroundDrawList(vp)->AddRect(it.window->InnerClipRect.Min, it.window->InnerClipRect.Max, IM_COL32(255,0,255,180));

			return true;
		}

		// No reliable rect this frame: schedule ImGui to highlight the exact item on the next frame(s).
		schedule_builtin_highlight(it.id, 2);
		return false;
	}
*/





    // *************************************************************************** //
    // *************************************************************************** //


    inline void hl_begin(void) noexcept {
        utl::highlight::begin_frame();
    }
    
    inline void hl_add(const ImGuiID & hovered) noexcept {
        utl::highlight::add(hovered);
    }
    
    inline void hl_remove(const ImGuiID & hovered) noexcept {
        utl::highlight::remove(hovered);
    }
    
    inline void hl_clear(void) noexcept {
        utl::highlight::clear();
    }


    //  "highlight_hover_item"
    //
    inline void highlight_hover_item(void) noexcept
    {
        enum class phase { idle, armed };
        ImGuiContext *  ctx         = ImGui::GetCurrentContext();
        ctx->DebugItemPickerActive  = false;
        static phase    s           = phase::idle;

        if (s == phase::idle)
        {
            //  ctx->DebugItemPickerActive = true;  // highlight during this frame's submissions
            //  s = phase::armed;
        }
        else // phase::armed
        {
            ctx->DebugItemPickerActive = false; // restore on next frame entry
            s = phase::idle;
        }
        ctx->DebugItemPickerActive = false;
        return;
    }

    // *************************************************************************** //
    // *************************************************************************** //



    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      ** PRIMARY FUNCTIONS. **
    // *************************************************************************** //

    //  "PerFrameCache"
    //
    //      TO-DO:
    //          - This needs to be a centralized function that will be called  ONE-TIME  at the beginning of  EACH FRAME.
    //          - It will ensure that each data-member of the App State is Up-To-Date and prepared for usage across any opteration
    //          that make take place across the coming frame.
    //
    inline void                         PerFrameCache               (void)  { return; }
    
    
    //  "update_current_task"
    [[nodiscard]] inline bool           update_current_task         (void)
    {
        //  return this->m_task_state.update_current_task( m_glfw_window );
        
        const bool updated = this->m_task_state.update_current_task( m_glfw_window );
        
        if ( updated )
        {
            return updated;
        }
        else
        {
            return updated;
        }
    }
    
    

    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //

    //  "PushFont"
    inline void                         PushFont                    ([[maybe_unused]] const Font & which)
    { ImGui::PushFont( this->m_fonts[which] ); return; }

    //  "PopFont"
    inline void                         PopFont                     (void)                  { ImGui::PopFont(); return; }




    //  "DockAtHome"
    inline void                         DockAtHome                  (const Window & idx) {
        app::WinInfo &w = this->m_windows[static_cast<Window>(idx)];
        if (w.open)     ImGui::DockBuilderDockWindow(w.uuid.c_str(), this->m_main_dock_id);
    }
    //
    inline void                         DockAtHome                  (const char * uuid)     { ImGui::DockBuilderDockWindow( uuid, m_main_dock_id ); }
    
    
    
    //  "DockAtDetView"
    inline void                         DockAtDetView               (const Window & idx) {
        app::WinInfo &  w = this->m_windows[static_cast<Window>(idx)];
        if (w.open)     ImGui::DockBuilderDockWindow(w.uuid.c_str(), this->m_detview_dockspace_id);
    }
    //
    inline void                         DockAtDetView               (const char * uuid)     { ImGui::DockBuilderDockWindow( uuid, m_detview_dockspace_id ); }
    
    
    
    // *************************************************************************** //
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.



// *************************************************************************** //
// *************************************************************************** //
};//	END "AppState" STRUCT INTERFACE.










// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" :: "app" NAMESPACE.






#endif      //  _CBAPP_APP_STATE_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.

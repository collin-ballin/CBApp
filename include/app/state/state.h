/***********************************************************************************
*
*       ********************************************************************
*       ****                S T A T E . h  ____  F I L E                ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
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
    _CBAPP_APPSTATE_ALIAS_API       //  CLASS-DEFINED, NESTED TYPENAME ALIASES.
// *************************************************************************** //
// *************************************************************************** //
public:
// *************************************************************************** //
//
//
//      1.  PUBLIC MEMBER FUNCTIONS...
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
    //
    // *************************************************************************** //
    //  1.2     PRIVATE STRUCT INITIALIZATION FUNCTIONS...
    // *************************************************************************** //
protected:
                                        AppState                    (void);                 //  Default Constructor.
                                        ~AppState                   (void);                 //  Default Destructor.
public:
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //  1.3A    APPEARANCE HELPER FUNCTIONS...
    // *************************************************************************** //
    void                                SetAppColorStyle            (const AppColorStyle_t);
    void                                SetPlotColorStyle           (const PlotColorStyle_t);
    void                                SetDarkMode                 (void);
    void                                SetLightMode                (void);
    void                                LoadCustomColorMaps         (void);
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //  1.3B    APPLICATION OPERATION HELPER FUNCTIONS...
    // *************************************************************************** //
    //
    //                              APPLICATION UTILITIES:
    void                                DockAtHome                  (const Window &);
    void                                DockAtHome                  (const char *);
    void                                DockAtDetView               (const Window &);
    void                                DockAtDetView               (const char *);
    //
    //                              SERIALIZATION ITEMS:
    template<typename Callback>
    bool                                SaveWithValidation          ( const std::filesystem::path & ,   Callback &,
                                                                      const std::string_view &      ,   const std::string_view & msg="callback function returned failure");
    
    
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //  1.3C    MISC. HELPER FUNCTIONS...
    // *************************************************************************** //
    void                                log_startup_info            (void) noexcept;
    void                                log_shutdown_info           (void) noexcept;
    
//
//
// *************************************************************************** //
// *************************************************************************** //   END "FUNCTIONS".

    
   
// *************************************************************************** //
//
//
//  1.4                 INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //                      PRIMARY INLINE FUNCTIONS...
    // *************************************************************************** //

    //  "PerFrameCache"
    //
    //      TO-DO:
    //          - This needs to be a centralized function that will be called  ONE-TIME  at the beginning of  EACH FRAME.
    //          - It will ensure that each data-member of the App State is Up-To-Date and prepared for usage across any opteration
    //          that make take place across the coming frame.
    //
    inline void                         PerFrameCache               (void)  { return; }
    
    

    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //                      GETTER FUNCTIONS...
    // *************************************************************************** //
    
    //  "current_task"
    inline Applet                       current_task                (void) const    { return this->m_task_state.m_current_task; }
    
    //  "current_task_name"
    inline const char *                 current_task_name           (void)          { return this->m_task_state.current_task_name(); }
    
    //  "GetNavWindowName"
    inline const char *                 GetNavWindowName            (void)          { return this->m_task_state.m_nav_window_name.c_str(); }
    
    //  "GetNavWindowStr"
    inline const std::string &          GetNavWindowStr             (void)          { return this->m_task_state.m_nav_window_name; }



    //  "current_app_color_style"
    inline const char *                 current_app_color_style     (void) const
    {  return this->m_app_color_style_names[ static_cast<size_t>(this->m_current_app_color_style) ];  }

    //  "current_plot_color_style"
    inline const char *                 current_plot_color_style    (void) const
    {  return this->m_plot_color_style_names[ static_cast<size_t>(this->m_current_plot_color_style) ];  }
    
    

    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //                      CENTRALIZED OPERATION FUNCTIONS...
    // *************************************************************************** //

    //  "PushFont"
    inline void                         PushFont                    ([[maybe_unused]] const Font & which)
    { ImGui::PushFont( this->m_fonts[which] ); return; }

    //  "PopFont"
    inline void                         PopFont                     (void)          { ImGui::PopFont(); return; }



    //  "GetDockNodeVisText"
    inline const char *                 GetDockNodeVisText          (const ImGuiDockNode * node)
    { return (node && node->VisibleWindow) ? node->VisibleWindow->Name : "NULL"; } // Same expression used inside DebugNodeDockNode()
    
    //  "update_current_task"
    inline void                         update_current_task         (void)
    {
        this->m_task_state.update_current_task( m_glfw_window );
        return;
    }
    
    
    
    // *************************************************************************** //
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.



// *************************************************************************** //
//
//
//  2.              CLASS DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //  2.1             CLASS DATA MEMBERS...
    // *************************************************************************** //
    //                      GROUPS / SUB-CLASSES OF "APPSTATE":
    utl::Logger &                       m_logger;                                                   //  1.      LOGGER...
    ImWindows                           m_windows;                                                  //  2.      APPLICATION WINDOW STATE...
    std::vector<WinInfo *>              m_detview_windows               = {};                       //  2.1     WINDOWS INSIDE DETAIL VIEW...
    ImFonts                             m_fonts;                                                    //  3.      APPLICATION FONTS...
    std::vector< std::pair<Timestamp_t, std::string> >
                                        m_notes                         = {};
    Timestamp_t                         m_timestamp_spawn;
    Timestamp_t                         m_timestamp_start;
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //  2.2             SUB-STATE INFORMATION...
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
    //
    // *************************************************************************** //
    //  2.21            OTHER IMPORTANT DATA...
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
    //
    // *************************************************************************** //
    //  2.3             REFERENCE TO CONSTEXPR GLOBAL ARRAYS.
    // *************************************************************************** //
    //      std::array< std::string *, static_cast<size_t>(Applet_t::Count) >       //  No CONSTEXPR arr for this bc I want it to copy the
    //                                          m_applets                       = {};   //  window names EXACTLY in case we ever rename them.
    static constexpr auto &             m_app_color_style_names         = APPLICATION_COLOR_STYLE_NAMES;
    static constexpr auto &             m_plot_color_style_names        = APPLICATION_PLOT_COLOR_STYLE_NAMES;
    static constexpr AppleSystemColors_t
                                        SystemColor                     = {};
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //  2.4             STATIC AND CONSTEXPR VARIABLES.
    // *************************************************************************** //
    //                      1.      Window Variables:
    //                      1.1         ALL Windows.
    static constexpr size_t             ms_WINDOWS_BEGIN            = static_cast<size_t>(Window::Dockspace);
    static constexpr size_t             ms_RHS_WINDOWS_BEGIN        = static_cast<size_t>(Window::MainApp);
    static constexpr size_t             ms_WINDOWS_END              = static_cast<size_t>(Window::Count);
    //
    //                      1.2         Main Application Windows.
    static constexpr size_t             ms_APP_WINDOWS_BEGIN        = static_cast<size_t>(Window::CCounterApp);
    static constexpr size_t             ms_APP_WINDOWS_END          = static_cast<size_t>(Window::ImGuiStyleEditor);
    //
    //                      1.3         Basic Tool Windows.
    static constexpr size_t             ms_TOOL_WINDOWS_BEGIN       = static_cast<size_t>(Window::ImGuiStyleEditor);
    static constexpr size_t             ms_TOOL_WINDOWS_END         = static_cast<size_t>(Window::ColorTool);
    //
    //                      1.4         "MY Tools" / Custom Tools Windows.
    static constexpr size_t             ms_MY_TOOLS_WINDOWS_BEGIN   = static_cast<size_t>(Window::ColorTool);
    static constexpr size_t             ms_MY_TOOLS_WINDOWS_END     = static_cast<size_t>(Window::ImGuiDemo);
    //
    //                      1.5         Demo Windows.
    static constexpr size_t             ms_DEMO_WINDOWS_BEGIN       = static_cast<size_t>(Window::ImGuiDemo);
#if defined(CBAPP_ENABLE_CB_DEMO)
    static constexpr size_t             ms_DEMO_WINDOWS_END         = static_cast<size_t>(Window::CBDemo);
#elif defined(CBAPP_ENABLE_FUNCTIONAL_TESTING)
    static constexpr size_t             ms_DEMO_WINDOWS_END         = static_cast<size_t>(Window::CBFunctionalTesting);
# else
    static constexpr size_t             ms_DEMO_WINDOWS_END         = static_cast<size_t>(Window::Count);
#endif  //  CBAPP_ENABLE_CB_DEMO) || defined(CBAPP_ENABLE_FUNCTIONAL_TESTING  //
    //
    //                      1.6         **Extra** Windows.
#if defined(CBAPP_ENABLE_CB_DEMO) || defined(CBAPP_ENABLE_FUNCTIONAL_TESTING)
    #if defined(CBAPP_ENABLE_CB_DEMO)
        static constexpr size_t         ms_EXTRA_WINDOWS_BEGIN      = static_cast<size_t>(Window::CBDemo);
    # elif defined(CBAPP_ENABLE_FUNCTIONAL_TESTING)
        static constexpr size_t         ms_EXTRA_WINDOWS_BEGIN      = static_cast<size_t>(Window::CBFunctionalTesting);
    #endif
    static constexpr size_t             ms_EXTRA_WINDOWS_END        = static_cast<size_t>(Window::Count);
#endif  //  CBAPP_ENABLE_CB_DEMO) || defined(CBAPP_ENABLE_FUNCTIONAL_TESTING  //


    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //  2.5             MISC. INFORMATION.
    // *************************************************************************** //
//
#if defined(__CBLIB_RELEASE_WITH_DEBUG_INFO__) || defined(__CBAPP_DEBUG__)
    LogLevel                            m_LogLevel                  = LogLevel::Debug;
# else
    LogLevel                            m_LogLevel                  = LogLevel::Warning;
#endif  //  __CBLIB_RELEASE_WITH_DEBUG_INFO__ || __CBAPP_DEBUG__  //


    //                      GLFW AND HOST WINDOW STUFF:
    const char *                        m_glsl_version              = nullptr;
    ImGuiConfigFlags                    m_io_flags                  = ImGuiConfigFlags_None | ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad | ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
    ImGuiViewport *                     m_main_viewport             = nullptr;
    GLFWwindow *                        m_glfw_window               = nullptr;
    int                                 m_glfw_interval             = 1;    //  Use 1 for VSYNC ENABLED.
    //
    //                      COLORS:
    ImVec4                              m_glfw_bg                   = cb::app::DEF_ROOT_WIN_BG;
    ImVec4                              m_dock_bg                   = cb::app::DEF_INVISIBLE_COLOR;
    ImVec4                              m_main_bg                   = cb::app::DEF_MAIN_WIN_BG;
    //
    ImVec4                              m_controlbar_bg             = cb::app::DEF_CONTROLBAR_WIN_BG;
    //
    //                      BROWSER COLORS:
    ImVec4                              m_browser_bg                = cb::app::DEF_BROWSER_WIN_BG;
    ImVec4                              m_browser_left_bg           = ImVec4(0.142f,    0.142f,     0.142f,     1.000f);//  #242424     (5% shade of browser bg)
    ImVec4                              m_browser_right_bg          = ImVec4(0.242f,    0.242f,     0.242f,     1.000f);//  ##3E3E3E    (5% tint of browser bg)
    float                               m_browser_child_rounding    = 8.0f;
    //
    //                      DETAILVIEW COLORS:
    ImVec4                              m_detview_bg                = cb::app::DEF_DETVIEW_WIN_BG;
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //  2.6             DIMENSIONS.
    // *************************************************************************** //
    //                      SYSTEM:
    int                                 m_system_w                  = -1;       //  Sys. Display Dims.
    int                                 m_system_h                  = -1;
    float                               m_dpi_scale                 = 1.0f;
    float                               m_dpi_fontscale             = 1.0f;
    //
    //                      MAIN UI:
    int                                 m_window_w                  = 1280;
    int                                 m_window_h                  = 720;
    //
    //                      BOOLEANS:
    std::atomic<CBSignalFlags>          m_pending                   = { CBSignalFlags_None };
    std::atomic<bool>                   m_running                   = { true };
    bool                                m_rebuild_dockspace         = false;
    //
    bool                                m_show_controlbar_window    = true;
    bool                                m_show_browser_window       = true;
    bool                                m_show_detview_window       = true;
    //
    bool                                m_show_system_preferences   = false;
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //  2.7             SPECIFICS.
    // *************************************************************************** //
    //                      MAIN DOCKINGSPACE:
    const char *                        m_dock_name                 = "##RootDockspace";
    ImGuiID                             m_dockspace_id              = 0;
    //
    ImGuiID                             m_main_dock_id              = 0;
    ImGuiDockNodeFlags                  m_main_node_flags           = ImGuiDockNodeFlags_NoDockingOverOther | ImGuiDockNodeFlags_CentralNode | ImGuiDockNodeFlags_NoCloseButton; //ImGuiDockNodeFlags_NoDocking; ImGuiDockNodeFlags_NoDockingOverMe
    ImGuiDockNode *                     m_main_node                 = nullptr;
    //
    //                      CONTROL BAR:
    ImGuiID                             m_controlbar_dock_id        = 0;
    ImGuiDockNodeFlags                  m_controlbar_node_flags     = ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_HiddenTabBar | ImGuiDockNodeFlags_NoResize; //ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoCloseButton;      //  ImGuiDockNodeFlags_NoSplit
    ImGuiDockNode *                     m_controlbar_node           = nullptr;
    //
    float                               m_controlbar_ratio          = 0.02;
    //
    //                      BROWSER / SIDEBAR:
    ImGuiID                             m_browser_dock_id           = 0;
    ImGuiDockNodeFlags                  m_browser_node_flags        = ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoCloseButton;      //  ImGuiDockNodeFlags_NoSplit
    ImGuiDockNode *                     m_browser_node              = nullptr;
    //
    float                               m_browser_ratio             = app::DEF_BROWSER_RATIO;
    //
    //                      DETAIL VIEW:
    ImGuiID                             m_detview_dock_id           = 0;
    ImGuiDockNodeFlags                  m_detview_node_flags        = ImGuiDockNodeFlags_NoDockingOverOther | ImGuiDockNodeFlags_NoCloseButton;      //  ImGuiDockNodeFlags_NoSplit
    ImGuiDockNode *                     m_detview_node              = nullptr;
    //
    float                               m_detview_ratio             = 0.45f;
    //
    //                      DETAIL VIEW DOCKSPACE:
    static constexpr const char *       m_detview_dockspace_uuid    = "##DetailViewDockspace";
    ImGuiID                             m_detview_dockspace_id      = 0;
    ImGuiDockNodeFlags                  m_detview_dockspace_flags   = ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoCloseButton;// | ImGuiDockNodeFlags_HiddenTabBar;      //  ImGuiDockNodeFlags_NoSplit
    ImGuiDockNodeFlags                  m_detview_window_flags      = ImGuiDockNodeFlags_HiddenTabBar; // ImGuiDockNodeFlags_NoTabBar;      //  ImGuiDockNodeFlags_NoSplit
    //
    //                      ADD MORE SHARED STATE DATA MEMBERS HERE:
    //
    //                              SubCategory.
    //                                  ...
    //
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //  3.              GENERIC CONSTANTS.
    // *************************************************************************** //
    //      std::string                         ms_IDLE_APPLET_NAME;
    //      std::string                         ms_UNDEFINED_APPLET_NAME;
    
    
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "CLASS DATA-MEMBERS".



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

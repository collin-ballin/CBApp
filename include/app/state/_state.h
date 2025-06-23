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
#include "app/state/_config.h"
#include "app/state/_types.h"
#include "app/_init.h"



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
    //  1.1             PUBLIC API...
    // *************************************************************************** //
    
    //  "instance"                      | Meyers-Style Singleton.       Created on first call, once.
    static inline AppState &            instance                    (void)
    { static AppState   single = AppState();     return single; }
                                        
    //  1.2             Deleted Operators, Functions, etc...
                                        AppState                    (const AppState &   )       = delete;   //  Copy. Constructor.
                                        AppState                    (AppState &&        )       = delete;   //  Move Constructor.
    AppState &                          operator =                  (const AppState &   )       = delete;   //  Assgn. Operator.
    AppState &                          operator =                  (AppState &&        )       = delete;   //  Move-Assgn. Operator.
    
    
// *************************************************************************** //
// *************************************************************************** //
protected:
// *************************************************************************** //
//  1.2     PRIVATE STRUCT INITIALIZATION FUNCTIONS...
// *************************************************************************** //
                                        AppState                    (void);                 //  Default Constructor.
                                        ~AppState                   (void);                 //  Default Destructor.
    
    
// *************************************************************************** //
// *************************************************************************** //
//
//
//
public:
    // *************************************************************************** //
    //  1.3     STRUCT UTILITY FUNCTIONS...
    // *************************************************************************** //
    void                                SetDarkMode                 (void);
    void                                SetLightMode                (void);
    void                                LoadCustomColorMaps         (void);
    
    
    void                                DockAtHome                  (const Window &);
    void                                DockAtHome                  (const char *);
    void                                DockAtDetView               (const Window &);
    void                                DockAtDetView               (const char *);
    void                                PushFont                    ([[maybe_unused]] const Font & );
    void                                PopFont                     (void);
    
    
    // *************************************************************************** //
    //  1.4     INLINE HELPER FUNCTIONS...
    // *************************************************************************** //

    //  "current_task"
    inline const char *                 current_task                (void) const
    {  return this->m_applets[ static_cast<size_t>(this->m_current_task) ]->c_str();  }

    //  "GetDockNodeVisText"
    inline const char *                 GetDockNodeVisText          (const ImGuiDockNode * node)
    {
        // Same expression used inside DebugNodeDockNode()
        return (node && node->VisibleWindow) ? node->VisibleWindow->Name : "NULL";
    }
    
    //  "update_current_task"
    inline void                         update_current_task         (void)
    {
        const char *    vis         = this->GetDockNodeVisText( this->m_main_node );
        const char *    name        = this->current_task();
        bool            match       = false;
        
        if ( strncmp(name, vis, 16) == 0 ) [[likely]] return; //  Bail out early if same applet is in use.
        
        
        for (size_t i = 0; !match && i < static_cast<size_t>(Applet::Count); ++i) {
            name    = m_applets[ static_cast<size_t>( i ) ]->c_str();
            match   = ( strncmp(name, vis, 16) == 0 );
            if (match)      this->m_current_task = static_cast<Applet>( i );
        }
        if (!match)     this->m_current_task = Applet::MainApp;
        
        return;
    }
    

    // *************************************************************************** //
    //  2.               CLASS DATA MEMBERS...
    // *************************************************************************** //
    
    //  0.      GROUPS / SUB-CLASSES OF "APPSTATE"...
    utl::Logger &                       m_logger;                                                   //  1.      LOGGER...
    ImWindows                           m_windows;                                                  //  2.      APPLICATION WINDOW STATE...
    std::vector<WinInfo *>              m_detview_windows           = {};                           //  2.1     WINDOWS INSIDE DETAIL VIEW...
    ImFonts                             m_fonts;                                                    //  3.      APPLICATION FONTS...
    std::vector< std::pair<Timestamp_t, std::string> >
                                        m_notes                     = {};
    //
    std::array< std::string *, static_cast<size_t>(Applet_t::Count) >
                                        m_applets;
    Applet                              m_current_task;
    
    
    
    //  1.      STATIC AND CONSTEXPR VARIABLES...
    //
    //          1.  Window Variables:
    //              1.1     ALL Windows.
    static constexpr size_t             ms_WINDOWS_BEGIN            = static_cast<size_t>(Window::Dockspace);
    static constexpr size_t             ms_RHS_WINDOWS_BEGIN        = static_cast<size_t>(Window::MainApp);
    static constexpr size_t             ms_WINDOWS_END              = static_cast<size_t>(Window::Count);
    //
    //              1.2     Main Application Windows.
    static constexpr size_t             ms_APP_WINDOWS_BEGIN        = static_cast<size_t>(Window::CCounterApp);
    static constexpr size_t             ms_APP_WINDOWS_END          = static_cast<size_t>(Window::ImGuiStyleEditor);
    //
    //              1.3     Basic Tool Windows.
    static constexpr size_t             ms_TOOL_WINDOWS_BEGIN       = static_cast<size_t>(Window::ImGuiStyleEditor);
    static constexpr size_t             ms_TOOL_WINDOWS_END         = static_cast<size_t>(Window::ColorTool);
    //
    //              1.4     "MY Tools" / Custom Tools Windows.
    static constexpr size_t             ms_MY_TOOLS_WINDOWS_BEGIN   = static_cast<size_t>(Window::ColorTool);
    static constexpr size_t             ms_MY_TOOLS_WINDOWS_END     = static_cast<size_t>(Window::ImGuiDemo);
    //
    //              1.5     Demo Windows.
    static constexpr size_t             ms_DEMO_WINDOWS_BEGIN       = static_cast<size_t>(Window::ImGuiDemo);
    static constexpr size_t             ms_DEMO_WINDOWS_END         = static_cast<size_t>(Window::Count);
    
    
    //  2.      MISC. INFORMATION...
    bool                                m_enable_vsync              = true;
    const char *                        m_glsl_version              = nullptr;
    LogLevel                            m_LogLevel                  = LogLevel::Debug;
    
    
    
    //  3.      GLFW AND HOST WINDOW STUFF...
    ImGuiConfigFlags                    m_io_flags                  = ImGuiConfigFlags_None | ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad | ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
    ImGuiViewport *                     m_main_viewport             = nullptr;
    GLFWwindow *                        m_glfw_window               = nullptr;
    
    
    //  4.      COLORS...
    ImVec4                              m_glfw_bg                   = cb::app::DEF_ROOT_WIN_BG;
    ImVec4                              m_dock_bg                   = cb::app::DEF_INVISIBLE_COLOR;
    ImVec4                              m_main_bg                   = cb::app::DEF_MAIN_WIN_BG;
    //
    ImVec4                              m_controlbar_bg             = cb::app::DEF_CONTROLBAR_WIN_BG;
    //
    //                              Browser Colors:
    ImVec4                              m_browser_bg                = cb::app::DEF_BROWSER_WIN_BG;
    ImVec4                              m_browser_left_bg           = ImVec4(0.142f,    0.142f,     0.142f,     1.000f);//  #242424     (5% shade of browser bg)
    ImVec4                              m_browser_right_bg          = ImVec4(0.242f,    0.242f,     0.242f,     1.000f);//  ##3E3E3E    (5% tint of browser bg)
    float                               m_browser_child_rounding    = 8.0f;
    //
    //                              DetailView Colors:
    ImVec4                              m_detview_bg                = cb::app::DEF_DETVIEW_WIN_BG;
    
    
    //  5.      DIMENSIONS...
    //
    //              6.1     System.
    int                                 m_system_w                  = -1;       //  Sys. Display Dims.
    int                                 m_system_h                  = -1;
    float                               m_dpi_scale                 = 1.0f;
    float                               m_dpi_fontscale             = 1.0f;
    //
    //              6.2     Main UI.
    //                              Main Window Dims.
    int                                 m_window_w                  = 1280;
    int                                 m_window_h                  = 720;
    
    
    //  6.      BOOLEANS...
    //
    std::atomic<CBSignalFlags>          m_pending                   = { CBSignalFlags_None };
    std::atomic<bool>                   m_running                   = { true };
    bool                                m_rebuild_dockspace         = false;
    //
    bool                                m_show_controlbar_window    = true;
    bool                                m_show_browser_window       = true;
    bool                                m_show_detview_window       = true;
    //
    bool                                m_show_system_preferences   = false;
    
    
    //  7.      SPECIFICS...
    //
    //              8.1     Main Dockingspace.
    const char *                        m_dock_name                 = "##RootDockspace";
    ImGuiID                             m_dockspace_id              = 0;
    //
    ImGuiID                             m_main_dock_id              = 0;
    ImGuiDockNodeFlags                  m_main_node_flags           = ImGuiDockNodeFlags_NoDockingOverOther | ImGuiDockNodeFlags_CentralNode | ImGuiDockNodeFlags_NoCloseButton; //ImGuiDockNodeFlags_NoDocking; ImGuiDockNodeFlags_NoDockingOverMe
    ImGuiDockNode *                     m_main_node                 = nullptr;
    //
    //              8.2     ControlBar.
    ImGuiID                             m_controlbar_dock_id        = 0;
    ImGuiDockNodeFlags                  m_controlbar_node_flags     = ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_HiddenTabBar | ImGuiDockNodeFlags_NoResize; //ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoCloseButton;      //  ImGuiDockNodeFlags_NoSplit
    ImGuiDockNode *                     m_controlbar_node           = nullptr;
    //
    float                               m_controlbar_ratio          = 0.02;
    //
    //              8.3     Sidebar.
    ImGuiID                             m_browser_dock_id           = 0;
    ImGuiDockNodeFlags                  m_browser_node_flags        = ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoCloseButton;      //  ImGuiDockNodeFlags_NoSplit
    ImGuiDockNode *                     m_browser_node              = nullptr;
    //
    float                               m_browser_ratio             = app::DEF_BROWSER_RATIO;
    //
    //              8.4     DetView.
    ImGuiID                             m_detview_dock_id           = 0;
    ImGuiDockNodeFlags                  m_detview_node_flags        = ImGuiDockNodeFlags_NoDockingOverOther | ImGuiDockNodeFlags_NoCloseButton;      //  ImGuiDockNodeFlags_NoSplit
    ImGuiDockNode *                     m_detview_node              = nullptr;
    //
    float                               m_detview_ratio             = 0.45f;
    //
    //
    //              9.1     DETVIEW DOCKSPACE.
    static constexpr const char *       m_detview_dockspace_uuid    = "##DetailViewDockspace";
    ImGuiID                             m_detview_dockspace_id      = 0;
    ImGuiDockNodeFlags                  m_detview_dockspace_flags   = ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoCloseButton;// | ImGuiDockNodeFlags_HiddenTabBar;      //  ImGuiDockNodeFlags_NoSplit
    ImGuiDockNodeFlags                  m_detview_window_flags      = ImGuiDockNodeFlags_HiddenTabBar; // ImGuiDockNodeFlags_NoTabBar;      //  ImGuiDockNodeFlags_NoSplit
    


    //  99.     ADD MORE SHARED STATE DATA MEMBERS HERE...
    //
    //              1.1     SubCategory.
    //                          ...
    //



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

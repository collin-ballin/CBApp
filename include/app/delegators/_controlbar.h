/***********************************************************************************
*
*       ********************************************************************
*       ****          _ C O N T R O L B A R . H  ____  F I L E          ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      June 05, 2025.
*
*       ********************************************************************
*                FILE:      [./ControlBar.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_APP_CONTROLBAR_H
#define _CBAPP_APP_CONTROLBAR_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  1.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
#include "utility/utility.h"
#include "widgets/widgets.h"
#include "app/_init.h"
#include "app/state/_state.h"



//  1.2     STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <cstdlib>          // C-Headers...
#include <stdio.h>
#include <unistd.h>

#include <string>           //  <======| std::string, ...
#include <string_view>
#include <filesystem>
#include <vector>           //  <======| std::vector, ...
#include <stdexcept>        //  <======| ...
#include <limits.h>
#include <math.h>



//  1.3     "DEAR IMGUI" HEADERS...
#include "imgui.h"
#include "implot.h"
# include "imgui_internal.h"






namespace cb { // BEGINNING NAMESPACE "cb"...
// *************************************************************** //
// *************************************************************** //



// *************************************************************** //
// *************************************************************** //
// 3.  PRIMARY CLASS INTERFACE
// @brief NO DESCRIPTION PROVIDED
// *************************************************************** //
// *************************************************************** //

class ControlBar
{
    CBAPP_APPSTATE_ALIAS_API                //  CLASS-DEFINED, NESTED TYPENAME ALIASES.
    friend class                App;
// *************************************************************************** //
// *************************************************************************** //
public:
    
    //  1               PUBLIC MEMBER FUNCTIONS...
    // *************************************************************************** //
    //  1.1                     Default Constructor, Destructor, etc.       [app/sidebar/sidebar.cpp]...
    explicit                    ControlBar                          (app::AppState & );                             //  Def. Constructor.
                                ~ControlBar                         (void);                                         //  Def. Destructor.
    
    //  1.2B                    Public API.
    void                        toggle_sidebar                      (void);
    void                        open_sidebar                        (void);
    void                        close_sidebar                       (void);
    void                        toggle_detview                      (void);
    void                        open_detview                        (void);
    void                        close_detview                       (void);
    
    
    //  1.2C                    Primary Class Interface.                    [app/sidebar/sidebar.cpp]...
    void                        Begin                               ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
    void                        initialize                          (void);


    //  1.3                     Deleted Operators, Functions, etc.
                                ControlBar                          (const ControlBar &    src)            = delete;   //  Copy. Constructor.
                                ControlBar                          (ControlBar &&         src)            = delete;   //  Move Constructor.
    ControlBar &                operator =                          (const ControlBar &    src)            = delete;   //  Assgn. Operator.
    ControlBar &                operator =                          (ControlBar &&         src)            = delete;   //  Move-Assgn. Operator.

    
    
// *************************************************************************** //
// *************************************************************************** //
protected:
    //  2.A             PROTECTED DATA-MEMBERS...
    // *************************************************************************** //
    
    //                          1.  BOOLEANS...
    bool                        m_initialized                       = false;
    bool                        m_first_frame                       = false;
    bool                        m_show_perf_metrics                 = app::DEF_PERF_METRICS_STATE;
    bool                        m_show_perf_plots                   = app::DEF_PERF_PLOTS_STATE;
    //
    //                          2.  DIMENSIONS...
    ImVec2                      ms_PLOT_SIZE                        = ImVec2(-1, 75);
    //                                      ...
    //
    //                          3.  WINDOW / GUI ITEMS...
    ImGuiWindowClass            m_window_class;
    //
    //                          4.  MISC INFORMATION...
    //                                      ...
    //
    //                          5.  IMPORTANT VARIABLES...
    AppState &                  CBAPP_STATE_NAME;
    
    
    //  2.B             PROTECTED MEMBER FUNCTIONS...
    // *************************************************************************** //
    
    //  2B.1                    Class Initializations.              [app/sidebar/sidebar.cpp]...
    void                        init                                (void);
    void                        load                                (void);
    void                        destroy                             (void);
    
    
    //  2B.2                    Secondary Class Methods.            [app/sidebar/sidebar.cpp]...
    void                        Display_Preferences_Menu            (void);
    //
    void                        draw_browser_button                 (void);
    void                        disp_appearance_mode                (void);     //  Other...
    void                        disp_font_selector                  (void);
    void                        disp_color_palette                  (void);
    void                        color_tool                          (void);
    void                        disp_ui_scale                       (void);
    void                        disp_performance_metrics            (void);
    
    
    
    
    
// *************************************************************************** //
// *************************************************************************** //
private:
    //  3.              PRIVATE MEMBER FUNCTIONS...
    // *************************************************************************** //
    
    //
    //  ...
    //


// *************************************************************************** //
// *************************************************************************** //
};//	END "Menubar" INLINE CLASS DEFINITION.






// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.












#endif      //  _CBAPP_SIDEBAR_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.

/***********************************************************************************
*
*       ********************************************************************
*       ****            _ T I T L E B A R . H  ____  F I L E            ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      May 07, 2025.
*
*       ********************************************************************
*                FILE:      [./TitleBar.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_APP_TITLEBAR_H
#define _CBAPP_APP_TITLEBAR_H  1



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
#include "app/_graphing_app.h"



//  1.2     STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <cstdlib>          // C-Headers...
#include <stdio.h>
#include <unistd.h>

#include <string>           //  <======| std::string, ...
#include <string_view>
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

class TitleBar
{
    CBAPP_APPSTATE_ALIAS_API                //  CLASS-DEFINED, NESTED TYPENAME ALIASES.
    friend class                App;
// *************************************************************************** //
// *************************************************************************** //
public:
    
    //  1               PUBLIC MEMBER FUNCTIONS...
    // *************************************************************************** //
    //  1.1                     Default Constructor, Destructor, etc.       [app/sidebar/sidebar.cpp]...
    explicit                    TitleBar                            (app::AppState & );                             //  Def. Constructor.
                                ~TitleBar                           (void);                                         //  Def. Destructor.
    
    //  1.2                     Primary Class Interface.                    [app/sidebar/sidebar.cpp]...
    void                        Begin                               ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
    void                        initialize                          (void);
    void                        toggle                              (void);
    void                        open                                (void);
    void                        close                               (void);

    //  1.3                     Deleted Operators, Functions, etc.
                                TitleBar                            (const TitleBar &    src)            = delete;   //  Copy. Constructor.
                                TitleBar                            (TitleBar &&         src)            = delete;   //  Move Constructor.
    TitleBar &                  operator =                          (const TitleBar &    src)            = delete;   //  Assgn. Operator.
    TitleBar &                  operator =                          (TitleBar &&         src)            = delete;   //  Move-Assgn. Operator.

    
    
// *************************************************************************** //
// *************************************************************************** //
protected:
    //  2.A             PROTECTED DATA-MEMBERS...
    // *************************************************************************** //
    
    //                                  1.  CONSTANTS...
    bool                                m_initialized                       = false;
    ImVec2                              m_scb_size                          = app::DEF_SIDEBAR_COLLAPSE_BUTTON_SIZE;
    const char *                        m_scb_uuid                          = "##SideBarCollapseButton";
    //
    //                                  2.  APPEARANCE...
    //                                          ...
    //
    //                                  3.  WINDOW / GUI ITEMS...
    //                                          ...
    //
    //                                      3.1     Sidebar-Button Toggle Window:
    bool                                m_invalid_cache                     = true;
    ImVec2                              m_win_size_cache                    = ImVec2(0.0f,      0.0f);
    ImVec2                              m_win_pos                           = ImVec2(-1.0f,     -1.0f);
    //
    //
    //                                  4.  MISC INFORMATION...
    //                                      ...
    //
    //                                  5.  IMPORTANT VARIABLES...
    AppState &                          CBAPP_STATE_NAME;
    
    
    //  2.B             PROTECTED MEMBER FUNCTIONS...
    // *************************************************************************** //
    
    //  2B.1                    Class Initializations.              [app/titlebar/titlebar.cpp]...
    void                        init                                (void);
    void                        destroy                             (void);
    
    
    //  2B.2                    Secondary Class Methods.            [app/titlebar/titlebar.cpp]...
    void                        ShoWTitleBarWindow                  (void);
    void                        ValidateCache                       (void);
    void                        compute_win_pos                     (void);
    
    
    
    
    
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

/***********************************************************************************
*
*       ********************************************************************
*       ****         _ D E T A I L _ V I E W . H  ____  F I L E         ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      June 05, 2025.
*
*       ********************************************************************
*                FILE:      [./DetailView.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_APP_DETAIL_VIEW_H
#define _CBAPP_APP_DETAIL_VIEW_H  1



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

class DetailView
{
    CBAPP_APPSTATE_ALIAS_API                //  CLASS-DEFINED, NESTED TYPENAME ALIASES.
    friend class                App;
// *************************************************************************** //
// *************************************************************************** //
public:
    
    //  1               PUBLIC MEMBER FUNCTIONS...
    // *************************************************************************** //
    //  1.1                     Default Constructor, Destructor, etc.       [app/sidebar/sidebar.cpp]...
    explicit                    DetailView                          (app::AppState & );                             //  Def. Constructor.
                                ~DetailView                         (void);                                         //  Def. Destructor.
    
    //  1.2B                    Public API.
    void                        open                                (void);
    void                        close                               (void);
    void                        toggle                              (void);
    
    
    //  1.2C                    Primary Class Interface.                    [app/sidebar/sidebar.cpp]...
    void                        Begin                               ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
    void                        initialize                          (void);


    //  1.3                     Deleted Operators, Functions, etc.
                                DetailView                          (const DetailView &    src)            = delete;   //  Copy. Constructor.
                                DetailView                          (DetailView &&         src)            = delete;   //  Move Constructor.
    DetailView &                operator =                          (const DetailView &    src)            = delete;   //  Assgn. Operator.
    DetailView &                operator =                          (DetailView &&         src)            = delete;   //  Move-Assgn. Operator.

    
    
// *************************************************************************** //
// *************************************************************************** //
protected:
    //  2.A             PROTECTED DATA-MEMBERS...
    // *************************************************************************** //
    
    //                          1.  BOOLEANS...
    bool                        m_initialized                       = false;
    bool                        m_first_frame                       = false;
    bool                        m_is_open                           = false;
    //
    //                          2.  DIMENSIONS...
    //                                      ...
    //
    //                          3.  WINDOW / GUI ITEMS...
    ImGuiWindowClass            m_window_class;
    //
    //                          4.  MISC INFORMATION...
    //                                      ...
    //
    //                          5.  IMPORTANT VARIABLES...
    std::vector<app::WinInfo *>      m_detview_children                  = { };
    AppState &                  CBAPP_STATE_NAME;
    
    
    //  2.B             PROTECTED MEMBER FUNCTIONS...
    // *************************************************************************** //
    
    //  2B.1                    Class Initializations.              [app/sidebar/sidebar.cpp]...
    void                        init                                (void);
    void                        load                                (void);
    void                        destroy                             (void);
    
    
    //  2B.2                    Secondary Class Methods.            [app/sidebar/sidebar.cpp]...
    void                        set_visibility_IMPL                 (const bool);
    void                        open_all                            (void);
    void                        close_all                           (void);
    //
    void                        add_child_window                    (app::WinInfo * );
    void                        remove_child_window                 (app::WinInfo * );
    void                        TestTabBar                          (void);
    
    
    
    
    
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

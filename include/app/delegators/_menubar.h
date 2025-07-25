/***********************************************************************************
*
*       ********************************************************************
*       ****              M E N U B A R . h  ____  F I L E              ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      May 01, 2025.
*
*       ********************************************************************
*                FILE:      [include/app/menubar/_menubar.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_APP_MENUBAR_H
#define _CBAPP_APP_MENUBAR_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  1.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
#include "utility/utility.h"
#include "widgets/widgets.h"
//  #include "app/_init.h"
#include "app/state/state.h"



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
#include "imgui_internal.h"





namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //

//  2.      UTILITY FUNCTIONS [NON-MEMBER FUNCTIONS]...
//                  ...






// *************************************************************************** //
// *************************************************************************** //
//          3.    PRIMARY CLASS INTERFACE.
//  Class to define the Top Menu-Bar for the main application.
// *************************************************************************** //
// *************************************************************************** //

class MenuBar
{
    CBAPP_APPSTATE_ALIAS_API        //  CLASS-DEFINED, NESTED TYPENAME ALIASES.
    friend class        App;
// *************************************************************************** //
// *************************************************************************** //
public:
    //  1               PUBLIC MEMBER FUNCTIONS...
    // *************************************************************************** //
    //  1.1             Default Constructor, Destructor, etc.       [app/menubar/menubar.cpp]...
    explicit            MenuBar                         (app::AppState & );                             //  Def. Constructor.
                        ~MenuBar                        (void);                                         //  Def. Destructor.
    
    //  1.2             Primary Class Interface.                    [app/menubar/menubar.cpp]...
    void                Begin                           ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
                                                         
    void                Display_Main_Menu_Bar           (void);

    //  1.3             Deleted Operators, Functions, etc.
                        MenuBar                         (const MenuBar &    src)            = delete;   //  Copy. Constructor.
                        MenuBar                         (MenuBar &&         src)            = delete;   //  Move Constructor.
    MenuBar &           operator =                      (const MenuBar &    src)            = delete;   //  Assgn. Operator.
    MenuBar &           operator =                      (MenuBar &&         src)            = delete;   //  Move-Assgn. Operator.
    
    
    

    
    
// *************************************************************************** //
// *************************************************************************** //
protected:
    //  2.A             PROTECTED DATA-MEMBERS...
    // *************************************************************************** //
    
    //                  1.  BOOLEANS...
    //  bool                m_running                       = true;
    //
    
    
    //                  2.  APPEARANCE...
    
    
    //                  3.  WINDOW / GUI ITEMS...           //  ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize
    //                                  ...
    
    
    //                  4.  MISC INFORMATION...
    
    
    //                  5.  IMPORTANT VARIABLES...
    AppState &          CBAPP_STATE_NAME;
    
    
    //  2.B             PROTECTED MEMBER FUNCTIONS...
    // *************************************************************************** //
    
    //  2B.1            Class Initializations.          [app/menubar/menubar.cpp]...
    void                init                            (void);
    void                load                            (void);
    void                destroy                         (void);
    //
    //  2B.2            Secondary Class Methods.        [app/menubar/menubar.cpp]...
    void                disp_file_menubar               (void);     //  MenuBar...
    void                disp_edit_menubar               (void);
    void                disp_view_menubar               (void);
    void                disp_window_menubar             (void);
    void                disp_show_windows_menubar       (void);
    void                disp_tools_menubar              (void);
    void                disp_help_menubar               (void);
    
    void                disp_imgui_submenu              (void);     //  Sub-MenuBar...
    
    
    
    
    
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
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.






#endif      //  _CBAPP_APP_MENUBAR_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.


/***********************************************************************************
*
*       ********************************************************************
*       ****           E D I T O R _ A P P . H  ____  F I L E           ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      June 14, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_EDITOR_APP_H
#define _CBAPP_EDITOR_APP_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "fdtd/fdtd.h"
#include "cblib.h"
#include "app/_init.h"
#include "widgets/widgets.h"
#include "utility/utility.h"

//  0.2     STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <cstdlib>          // C-Headers...
#include <stdio.h>
#include <unistd.h>
#include <random>
#include <tuple>
#include <utility>
#include <algorithm>

#include <string>           //  <======| std::string, ...
#include <string_view>
#include <vector>           //  <======| std::vector, ...
#include <stdexcept>        //  <======| ...
#include <limits.h>
#include <math.h>

//  0.3     "DEAR IMGUI" HEADERS...
#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_internal.h"
#include "implot.h"
#include "implot_internal.h"



//_CBAPP_WARN_UNUSED_PUSH
namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
// *************************************************************************** //
//                         EditorApp:
// 		        EditorApp Widget for Dear ImGui.
// *************************************************************************** //
// *************************************************************************** //

//  "EditorApp"
//
class EditorApp
{
    friend class        App;
    CBAPP_APPSTATE_ALIAS_API        //  CLASS-DEFINED, NESTED TYPENAME ALIASES.
// *************************************************************************** //
// *************************************************************************** //
public:
    //  0.               PUBLIC CLASS-NESTED ALIASES...
    // *************************************************************************** //
    
    //  1.               PUBLIC MEMBER FUNCTIONS...
    // *************************************************************************** //
    //  1.1             Default Constructor, Destructor, etc...
    explicit            EditorApp                    (app::AppState & );                                 //  Def. Constructor.
                        ~EditorApp                   (void);                                             //  Def. Destructor.
                        
    //  1.2             Public Member Functions...
    void                initialize                  (void);
    void                save                        (void);
    void                open                        (void);
    void                undo                        (void);
    void                redo                        (void);
    //
    void                Begin                       ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);


// *************************************************************************** //
// *************************************************************************** //
protected:
    //  2.A             PROTECTED DATA-MEMBERS...
    // *************************************************************************** //
    
    //  MISC APPLICATION STUFF...
    bool                                            m_initialized                   = false;
    
    //                                          3.  SUBSIDIARY WINDOWS...
    app::WinInfo                                    m_detview_window                = {
                                                        "Editor Controls",
                                                        ImGuiWindowFlags_None | ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY,
                                                        true,
                                                        nullptr
                                                    };
    
    //                                          3.  DOCKING SPACE...
    //                                              Main Dockspace:
    ImGuiWindowClass                                m_window_class;
    
    //                                          4.  IMPORTANT DATA...
    cb::Editor                                      m_editor;
    app::AppState &                                 CBAPP_STATE_NAME;
        
    
    //  2.B             PROTECTED MEMBER FUNCTIONS...
    // *************************************************************************** //
    
    //  2B.1        Class Initializations.          [init.cpp]...
    void                init                            (void);
    void                destroy                         (void);
    
    
    //  2C.         Core Functions.                 [graph_app.cpp]...
    void                display_plots                   (void);
    void                display_controls                (void);
    
    
    //  2C.1        Utility Functions...
    //                  ...
    
    

// *************************************************************************** //
// *************************************************************************** //
};//	END "EditorApp" CLASS PROTOTYPE.







// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.





#endif      //  _CBAPP_EDITOR_APP_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.


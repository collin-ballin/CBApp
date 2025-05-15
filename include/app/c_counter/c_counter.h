/***********************************************************************************
*
*       ********************************************************************
*       ****       _ C O I N C I D E N C E . C P P  ____  F I L E       ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      May 10, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_COUNTER_APP_H
#define _CBAPP_COUNTER_APP_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include "_config.h"
#include "cblib.h"
//#include "app/app.h"
#include "app/_init.h"
#include "widgets/widgets.h"
#include "utility/utility.h"
#include "utility/pystream/pystream.h"

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
#include "imgui_internal.h"
#include "implot.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



struct ChannelSpec { int idx; const char * name; };



// *************************************************************************** //
// *************************************************************************** //
//                         CCounterApp:
// 		        CCounterApp Widget for Dear ImGui.
// *************************************************************************** //
// *************************************************************************** //

class CCounterApp
{
    CBAPP_APPSTATE_ALIAS_API        //  CLASS-DEFINED, NESTED TYPENAME ALIASES.
// *************************************************************************** //
// *************************************************************************** //
public:
    //  0.               PUBLIC CLASS-NESTED ALIASES...
    // *************************************************************************** //
    
    //  1.               PUBLIC MEMBER FUNCTIONS...
    // *************************************************************************** //
    //  1.1             Default Constructor, Destructor, etc...
                        CCounterApp                 (void)                              = default;      //  Def. Constructor.
                        ~CCounterApp                (void);                                             //  Def. Destructor.
                        
    //  1.2             Public Member Functions...
    void                initialize                  (void);
    void                Begin                       ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
    //                      Data & Interaction.


    //  1.3             Deleted Operators, Functions, etc...
    //                     App                     (const App & src)               = delete;   //  Copy. Constructor.
    //                     App                     (App && src)                    = delete;   //  Move Constructor.
    // App &               operator =              (const App & src)               = delete;   //  Assgn. Operator.
    // App &               operator =              (App && src)                    = delete;   //  Move-Assgn. Operator.



// *************************************************************************** //
// *************************************************************************** //
protected:
    //  2.A             PROTECTED DATA-MEMBERS...
    // *************************************************************************** //
    
    //  CONSTANTS.
    ImVec2                                      COLORBAR_SIZE               = ImVec2(0, 0);
    ImVec2                                      SCALE_SIZE                  = ImVec2(0, 0);
    float                                       COUNTER_COL_WIDTH           = 120.0f;
    float                                       CONTROL_WIDTH               = 8.0f;
    
    //  MISC APPLICATION STUFF...
    bool                                        m_initialized               = false;
    
    //  INTERACTIVE WIDGET VARIABLES...
    ImPlotAxisFlags                             m_plot_flags                = ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoDecorations;
    ImPlotAxisFlags                             m_mst_plot_flags            = ImPlotAxisFlags_None | ImPlotAxisFlags_AutoFit;
    ImPlotAxisFlags                             m_mst_xaxis_flags           = ImPlotAxisFlags_None | ImPlotAxisFlags_AutoFit;                           // enable grid, disable decorations.
    ImPlotAxisFlags                             m_mst_yaxis_flags           = ImPlotAxisFlags_None | ImPlotAxisFlags_AutoFit;                           // enable grid, disable decorations.
    
    ImPlotLocation                              m_mst_legend_loc            = ImPlotLocation_NorthEast;                                                 // legend position.
    ImPlotLegendFlags                           m_mst_legend_flags          = ImPlotLegendFlags_None; //ImPlotLegendFlags_Outside; // | ImPlotLegendFlags_Horizontal;


    //  *************************************************************************** //
    //  ARRAY CONVENTIONS:
    //      ARRAY INDEX [ 0 ]       =   PLOT WINDOW...
    //      ARRAY INDEX [ 1 ]       =   CONTROL WINDOW...
    //
    //  *************************************************************************** //
    
    //                                  1.  APPEARANCE CONSTANTS...
    const float                             m_child_corner_radius       = 5.0f;
    std::pair<int,int>                      m_HEIGHT_LIMITS[2]          = { { 30, 30 }, { 30, 30 } };
    //
    //
    //                                  2.  CHILDREN PARAMETERS...
    std::array< const char *, 2 >           m_child_ids                 = { "PlotChild", "ControlChild" };
    ImGuiChildFlags                         m_child_flags[2]            = {
                                                ImGuiWindowFlags_None | ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY,
                                                ImGuiWindowFlags_None | ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY
                                            };
    ImGuiTabBarFlags                        m_tabbar_flags[2]           = {
                                                ImGuiTabBarFlags_None | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyResizeDown | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_TabListPopupButton,
                                                ImGuiTabBarFlags_None | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyResizeDown | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_TabListPopupButton
                                            };
    
    //                                  3.  SUBSIDIARY WINDOWS...
    std::array< const char *, 2 >           m_win_uuids                 = { "Visuals", "Controls" };
    ImGuiWindowFlags                        m_docked_win_flags[2]       = {
                                                ImGuiWindowFlags_None | ImGuiWindowFlags_NoNavFocus,    // ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground,
                                                ImGuiWindowFlags_None | ImGuiWindowFlags_NoNavFocus     //  ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground
                                            };
    
    //                                  3.  DOCKING SPACE...
    //
    //                                      Main Dockspace:
    float                                   m_dockspace_ratio           = 0.6f;
    static constexpr const char *           m_dockspace_name            = "DockHostSpace##CCApp";
    ImGuiDockNodeFlags                      m_dockspace_flags           = ImGuiDockNodeFlags_None;
    ImGuiID                                 m_dockspace_id              = 0;
    ImGuiWindowClass                        m_window_class;
    //
    //      Nested Dockspaces:
    ImGuiID                                 m_dock_ids[2]               = { 0, 0 };
    
    
    
    //                                  4.  PLOTTING STUFF...
    static constexpr std::array< const char *, 2 >
                                            ms_mst_axis_labels          = { "Time  [sec]",      "Counts  [Arb.]" };
    
    
    
    //                                  5.  IMPORTANT DATA...
    static constexpr size_t                 ms_NUM                      = 15;
    static constexpr ChannelSpec            ms_channels[ms_NUM]         = {
        { 8,  "A"   }, { 4,  "B"   }, { 2,  "C"   }, { 1,  "D"   },
        {12,  "AB"  }, {10,  "AC"  }, { 9,  "AD"  }, { 6,  "BC"  },
        { 5,  "BD"  }, { 3,  "CD"  }, {14,  "ABC" }, {13,  "ABD" },
        {11,  "ACD" }, { 7,  "BCD" }, {15,  "ABCD"}
    };
    
    std::array<utl::ScrollingBuffer, ms_NUM>
                                        m_buffers                   = { };
    float                               m_max_counts[ms_NUM]        = { };
    
    
    //                                  6.  DELAGATOR CLASSES...
    app::AppState                           CBAPP_STATE_NAME                = app::AppState();
    
        
    
    
    //  2.B             PROTECTED MEMBER FUNCTIONS...
    // *************************************************************************** //
    
    //  2B.1            Class Initializations.      [CCounterApp.cpp]...
    void                init                        (void);
    void                destroy                     (void);
    
    void                display_plots               (void);
    void                display_controls            (void);



// *************************************************************************** //
// *************************************************************************** //
};//	END "CCounterApp" CLASS PROTOTYPE.







// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.






#endif      //  _CBAPP_COUNTER_APP_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.


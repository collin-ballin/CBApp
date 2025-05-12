//
//  _graphing_app.h
//  CBApp
//
//  Created by Collin Bond on 4/16/25.
//
// *************************************************************************** //
// *************************************************************************** //
#ifndef _CBAPP_GRAPHING_APP_H
#define _CBAPP_GRAPHING_APP_H  1



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



// *************************************************************************** //
// *************************************************************************** //
//                         GraphingApp:
// 		        GraphingApp Widget for Dear ImGui.
// *************************************************************************** //
// *************************************************************************** //

class GraphingApp
{
// *************************************************************************** //
// *************************************************************************** //
public:
    //  0.               PUBLIC CLASS-NESTED ALIASES...
    // *************************************************************************** //
    using   value_type              = float;
    using   size_type               = std::size_t;
    using   dim_type                = float;
    using   tab_t                   = std::tuple<std::string, bool, ImGuiTabBarFlags>;
    
    //  1.               PUBLIC MEMBER FUNCTIONS...
    // *************************************************************************** //
    //  1.1             Default Constructor, Destructor, etc...
                        GraphingApp                 (void)                              = default;      //  Def. Constructor.
                        GraphingApp                 (const size_type,   const size_type);               //  Parametric Constructors.
                        GraphingApp                 (const char *,      const size_type,    const size_type);
                        ~GraphingApp                (void);                                             //  Def. Destructor.
                        
    //  1.2             Public Member Functions...
    void                initialize                  (void);
    void                Begin                       ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
    void                RebuildDockspace            (void);
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
    
    //  Constants.
    ImVec2                              COLORBAR_SIZE           = ImVec2(0, 0);
    ImVec2                              SCALE_SIZE              = ImVec2(0, 0);
    float                               CONTROL_WIDTH           = 0.0f;
    
    //  Main Window Stuff...
    bool                                m_initialized           = false;
    
    
    //  All Children Constants.
    ImGuiTabBarFlags                    m_def_tab_flags         = ImGuiTabItemFlags_None;   //  Assigned to all "new tabs" by default...
    
    
    //  Plot Panel.
    ImGuiChildFlags                     mc_plot_child_flags     = ImGuiWindowFlags_None | ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY;
    ImGuiTabBarFlags                    mc_plot_tabbar_flags    = ImGuiTabBarFlags_None | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyResizeDown | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_TabListPopupButton;
    const float                         mc_plot_corner_radius   = 5.0f;
    const char *                        mc_plot_child_id        = "PlotChild";
    const char *                        mc_plot_tab_bar_id      = "PlotTabBar";
    std::pair<int,int>                  MC_PLOT_HEIGHT_LIMIT    = {30, 30};  //  {y1, y2}    : "y1"  --- min height of child (no matter how many widgets are on it).     "y2"  --- max height the window will extend to WITHOUT adding scrollbar.
    
#ifdef CBAPP_ETCH_A_SKETCH
    std::vector<tab_t>                  MC_PLOT_TABS            = {
        {"Etch-A-Sketch",           true,       ImGuiTabItemFlags_None},
        {"Table",                   true,       ImGuiTabItemFlags_None}
    };
# else
    std::vector<tab_t>                  MC_PLOT_TABS            = {
        {"Main Plot",               true,       ImGuiTabItemFlags_None | ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_NoReorder},
        {"Etch-A-Sketch",           true,       ImGuiTabItemFlags_None},
        {"Table",                   true,       ImGuiTabItemFlags_None},
        {"Empty",                   true,       ImGuiTabItemFlags_None}
    };
#endif  //  CBAPP_ETCH_A_SKETCH  //
    
    
    //  Control Panel.
    ImGuiChildFlags                     mc_ctrl_child_flags     = ImGuiWindowFlags_None | ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY;
    ImGuiTabBarFlags                    mc_ctrl_tabbar_flags    = ImGuiTabBarFlags_None | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyResizeDown | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_TabListPopupButton;
    const float                         mc_ctrl_corner_radius   = 5.0f;
    const char *                        mc_ctrl_child_id        = "CtrlChild";
    const char *                        mc_ctrl_tab_bar_id      = "CtrlTabBar";
    std::pair<int,int>                  MC_CTRL_HEIGHT_LIMIT    = {30, 30};
#ifdef CBAPP_ETCH_A_SKETCH
    std::vector<tab_t>                  MC_CTRL_TABS            = {
        {"Main Controls",           true,       ImGuiTabItemFlags_None | ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_NoReorder},
        {"Etch-A-Sketch Controls",  true,       ImGuiTabItemFlags_None | ImGuiTabItemFlags_NoReorder}
    };
# else
    std::vector<tab_t>                  MC_CTRL_TABS            = {
        {"Main Controls",           true,       ImGuiTabItemFlags_None | ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_NoReorder},
        {"Secondary Controls",      true,       ImGuiTabItemFlags_None | ImGuiTabItemFlags_NoReorder}
    };
#endif  //  CBAPP_ETCH_A_SKETCH  //
    
    //  Docking Space.
    float                           m_dockspace_ratio           = 0.6f;
    static constexpr const char *   m_dockspace_name            = "GAppDockHostSpace";
    ImGuiDockNodeFlags              m_dockspace_flags           = ImGuiDockNodeFlags_None;
    ImGuiID                         m_dockspace_id              = 0;
    //
    //      Docked Window Stuff:
    ImGuiWindowClass                m_window_class;
    ImGuiWindowFlags                m_docked_win_flags          = ImGuiWindowFlags_None | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground; // ImGuiWindowFlags_NoScrollbar
    //
    //      "Plot" Dockspace:
    static constexpr const char *   m_plot_win_uuid             = "Visuals";
    ImGuiID                         m_plot_dock_id              = 0;
    //
    //      "Control" Dockspace:
    static constexpr const char *   m_ctrl_win_uuid             = "Controls";
    ImGuiID                         m_ctrl_dock_id              = 0;
    
    
    //  Appearance.
        
        
    //  Interactive Variables.
    
    
    //  IMPORTANT VARIABLES.
    cb::HeatMap                         m_heatmap;
        
    
    
    
    //  2.B             PROTECTED MEMBER FUNCTIONS...
    // *************************************************************************** //
    
    //  2B.1            Class Initializations.      [GraphingApp.cpp]...
    void                init                        (void);
    void                destroy                     (void);
    
    //  2B.2            Internal Functions.
    void                display_plot_panel          (void);
    void                display_control_panel       (void);
    
    void                etch_a_sketch               (void);
    void                sketch_controls             (void);
    
    void                test_table                  (void);



    
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
};//	END "GraphingApp" CLASS PROTOTYPE.







// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.






#endif      //  _CBAPP_GRAPHING_APP_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.


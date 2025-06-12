/***********************************************************************************
*
*       ********************************************************************
*       ****           _ G R A P H _ A P P . H  ____  F I L E           ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      May 18, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_GRAPH_APP_H
#define _CBAPP_GRAPH_APP_H  1


//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "fdtd/fdtd.h"
#include "cblib.h"
//#include "app/app.h"
#include "app/_init.h"
#include "widgets/widgets.h"
#include "utility/utility.h"
#include "utility/pystream/pystream.h"
#include "app/graph_app/fdtd.h"
#include "app/graph_app/_editor.h"

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
//                         GraphApp:
// 		        GraphApp Widget for Dear ImGui.
// *************************************************************************** //
// *************************************************************************** //

struct Playback {
    utl::Param<ImU64>       frame;
    float                   fps;
//
    bool                    ready           = false;
    bool                    playing         = false;
    double                  now             = 0.0f;
    double                  last_time       = 0.0f;
};


//  "GraphApp"
//
class GraphApp
{
    //  using               STATE_LABELS                = SketchWidget::STATE_LABELS;
    //  using               BRUSH_SHAPE_LABELS          = SketchWidget::BRUSH_SHAPE_LABELS;
    using               State                       = SketchWidget::State;
    using               BrushShape                  = SketchWidget::BrushShape;
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
    explicit            GraphApp                    (app::AppState & );                                 //  Def. Constructor.
                        ~GraphApp                   (void);                                             //  Def. Destructor.
                        
    //  1.2             Public Member Functions...
    void                initialize                  (void);
    void                Begin                       ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);


// *************************************************************************** //
// *************************************************************************** //
protected:
    //  2.A             PROTECTED DATA-MEMBERS...
    // *************************************************************************** //
    
    //  MISC CONSTANT VALUES...
    float                                           ms_SPACING                      = 60.0f;
    ImVec2                                          ms_COLLAPSE_BUTTON_SIZE         = ImVec2{15,    15};
    
    //  INDIVIDUAL PLOT STUFF...
    float                                           ms_I_PLOT_COL_WIDTH             = 80.0f;
    
    //  MISC APPLICATION STUFF...
    bool                                            m_initialized                   = false;
    bool                                            m_rebuild_dockspace             = false;
    
    //  VARIOUS FLAGS...
    ImPlotLineFlags                                 m_channel_flags                 = ImPlotLineFlags_None | ImPlotLineFlags_Shaded;
    ImPlotAxisFlags                                 m_plot_flags                    = ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoDecorations;
    ImPlotFlags                                     m_mst_PLOT_flags                = ImPlotFlags_None | ImPlotFlags_NoTitle;
    ImPlotAxisFlags                                 m_mst_plot_flags                = ImPlotAxisFlags_None | ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoSideSwitch;
    ImPlotAxisFlags                                 m_mst_xaxis_flags               = ImPlotAxisFlags_None | ImPlotAxisFlags_AutoFit;                           // enable grid, disable decorations.
    ImPlotAxisFlags                                 m_mst_yaxis_flags               = ImPlotAxisFlags_None | ImPlotAxisFlags_AutoFit;                           // enable grid, disable decorations.
    
    ImPlotLocation                                  m_mst_legend_loc                = ImPlotLocation_NorthWest;                                                 // legend position.
    ImPlotLegendFlags                               m_mst_legend_flags              = ImPlotLegendFlags_None | ImPlotLegendFlags_Outside; // | ImPlotLegendFlags_Horizontal;
    //
    //
    //
    //  2.B             FDTD TYPENAME ALIASES...
    // *************************************************************************** //
protected:
    static constexpr size_t                         NX                              = 401ULL;
    static constexpr std::size_t                    NT                              = 100ULL;
//
public:
    using                                           value_type                      = double;
    using                                           FDTD_t                          = cb::FDTD_1D<NX, NT, value_type>;
    using                                           NEW_FDTD_t                      = fdtd::FDTD_1D<value_type>;
    using                                           complex_t                       = FDTD_t::complex_t;
    using                                           re_array                        = FDTD_t::re_array;
    using                                           im_array                        = FDTD_t::im_array;
	using 	                                        re_frame 		                = FDTD_t::re_frame;
	using 	                                        im_frame 		                = FDTD_t::im_frame;
    //
    //
    //  2.C             FDTD DATA MEMBERS...
    // *************************************************************************** //
 protected:
    //                                  0.1     MISC FDTD STUFF...
    std::atomic<bool>                               data_ready                      = std::atomic<bool>(false);
    std::once_flag                                  init_once;
    //
    Playback			                            m_playback                      = {     { 0, { 0, NT-1 } },
                                                                                            120.0f
                                                                                    };
    bool                                            playing                         = false;      // start paused
    float                                           playback_fps                    = 120.0f;
    double                                          last_time                       = 0.0;
    int                                             current_frame                   = 0;
    //
    //                                  0.2     FDTD DATA-MEMBERS STUFF...
    fdtd::StepSizes<double>                         m_stepsizes                     = {
                                                        { 1.0f,     { 1e-9f,     10.0f}     },      //  dx
                                                        { 1.0f,     { 1e-9f,     10.0f}     },      //  dy
                                                        { 1.0f,     { 1e-9f,     10.0f}     },      //  dz
                                                        { 1.0f,     { 1e-9f,     10.0f}     },      //  dt
                                                        { 1.0f,     { 0.0f,       2.0f}     },      //  Sc
                                                    };
    //
    fdtd::Steps<ImU64>                              m_steps                         = {
                                                        //{ 200,      { 1,        1000}       },    //  NX
                                                        { 10,       { 1,        1000}       },      //  NX
                                                        { 5,        { 1,        1000}       },      //  NY
                                                        { 1,        { 1,        1000}       },      //  NZ
                                                        { 400,      { 1,        2000}       }       //  NT
                                                    };
    fdtd::Parameters<ImU64, double>                 m_params                        = {
                                                        { 20,       { 1,         100}       },      //  Wavelength
                                                        { 8,        { 1,         500}       },      //  Duration
                                                    };
    //
    FDTD_t                                          ms_model                        = cb::FDTD_1D<NX, NT, double>();
    NEW_FDTD_t		                                ms_simulation                   = NEW_FDTD_t();
    re_frame *                                      m_Ez_T                          = nullptr;



    //  *************************************************************************** //
    //  ARRAY CONVENTIONS:
    //      ARRAY INDEX [ 0 ]       =   PLOT WINDOW...
    //      ARRAY INDEX [ 1 ]       =   CONTROL WINDOW...
    //
    //  *************************************************************************** //
    //                                          1.  APPEARANCE CONSTANTS...
    const float                                     m_child_corner_radius           = 5.0f;
    std::pair<int,int>                              m_HEIGHT_LIMITS[2]              = { { 30, 30 }, { 5, 5 } };
    //
    //
    //                                          2.  CHILDREN PARAMETERS...
    bool                                            m_child_open[2]                 = {true, true};
    std::array< const char *, 2 >                   m_child_ids                     = { "PlotChild",            "ControlChild" };
    ImGuiChildFlags                                 m_child_flags[2]                = {
                                                        ImGuiWindowFlags_None | ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY,
                                                        ImGuiWindowFlags_None | ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY
                                                    };
    WinInfo                                         m_detview_window                = {
                                                        "ControlChild",
                                                        ImGuiWindowFlags_None | ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY,
                                                        true,
                                                        nullptr
                                                    };
    //
    std::array< const char *, 2 >                   m_tabbar_uuids                  = { "PlotTabBar##GApp",     "PlotTabBar##GApp" };
    ImGuiTabBarFlags                                m_tabbar_flags[2]               = {
                                                        ImGuiTabBarFlags_None | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyResizeDown | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_TabListPopupButton,
                                                        ImGuiTabBarFlags_None | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyResizeDown | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton
                                                    };
    
    
    //                                          3.  SUBSIDIARY WINDOWS...
    ImVec2                                          m_ctrl_size                     = ImVec2(-1, -1);
    std::array< const char *, 2 >                   m_win_uuids                     = { "Visuals",              "Controls" };
    ImGuiWindowFlags                                m_docked_win_flags[2]           = {
                                                        ImGuiWindowFlags_None | ImGuiWindowFlags_NoNavFocus,    // ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground,
                                                        ImGuiWindowFlags_None | ImGuiWindowFlags_NoNavFocus     //  ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground
                                                    };
    
    
    //                                          3.  DOCKING SPACE...
    //
    //                                              Main Dockspace:
    float                                           m_dockspace_ratio               = 0.85f;
    static constexpr const char *                   m_dockspace_name                = "DockHostSpace##GApp";
    ImGuiDockNodeFlags                              m_dockspace_flags               = ImGuiDockNodeFlags_None;
    ImGuiID                                         m_dockspace_id                  = 0;
    ImGuiWindowClass                                m_window_class[2]               = {};
    //
    //      Nested Dockspaces:
    ImGuiID                                         m_dock_ids[2]                   = { 0, 0 };
    
    
    //                                          4.  IMPORTANT DATA...
    std::vector<Tab_t>                              ms_PLOT_TABS                    = {};
    std::vector<Tab_t>                              ms_CTRL_TABS                    = {};
    //
    //                                              Table CFGs:
    utl::TableCFG<2>                                m_fdtd_table_CFG                = {};
    utl::TableCFG<2>                                m_stepsize_table_CFG            = {};
    utl::TableCFG<2>                                m_sources_table_CFG             = {};
    utl::TableCFG<2>                                m_editor_table_CFG              = {};
    utl::TableCFG<2>                                m_playback_table_CFG            = {};
    //
    //                                              Table Rows:
    std::vector<utl::WidgetRow>                     ms_FDTD_ROWS                    = {};
    std::vector<utl::WidgetRow>                     ms_STEPSIZE_ROWS                = {};
    std::vector<utl::WidgetRow>                     ms_SOURCES_ROWS                 = {};
    std::vector<utl::WidgetRow>                     ms_EDITOR_ROWS                  = {};
    std::vector<utl::WidgetRow>                     ms_PLAYBACK_ROWS                = {};
    
    
    //                                          5.  WIDGET VARIABLES...
    bool                                            m_running                       = false;
    
    //                                          6.  PLOTTING STUFF...
    static constexpr size_t                         m_NUM_COLORS                    = 3;
    std::vector<ImVec4>                             m_plot_colors                   = std::vector<ImVec4>(m_NUM_COLORS);
    
    
    //                                          7.  DELAGATOR CLASSES...
    cb::HeatMap                                     m_heatmap;
    utl::PlotCFG                                    fdtd_1D_time_cfg                = {
        {   "##1DFDTDTimePlot",         ImVec2(-1, -1),     ImPlotFlags_None | ImPlotFlags_NoTitle  },
        {
            { "x-Node Index    [m dx]",             ImPlotAxisFlags_None | ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_NoSideSwitch }, //    ImPlotAxisFlags_LockMin     ImPlotAxisFlags_AutoFit   ImPlotAxisFlags_NoSideSwitch
            { "Ez    [V / m]",                      ImPlotAxisFlags_None | ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_NoSideSwitch }   // ImPlotAxisFlags_AutoFit   ImPlotAxisFlags_PanStretch
        },
            { ImPlotLocation_NorthEast, ImPlotLegendFlags_None}
    };
    utl::PlotCFG                                    fdtd_1D_freq_cfg                = {
        {   "##1DFDTDFreqPlot",         ImVec2(-1, -1),     ImPlotFlags_None | ImPlotFlags_NoTitle  },
        {
            { "Frequency    [1 / (q * dt)]",        ImPlotAxisFlags_None | ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_NoSideSwitch | ImPlotAxisFlags_Lock }, //    ImPlotAxisFlags_LockMin     ImPlotAxisFlags_AutoFit   ImPlotAxisFlags_NoSideSwitch
            { "| Ez(w) |    [Norm.]",               ImPlotAxisFlags_None | ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_NoSideSwitch | ImPlotAxisFlags_Lock }   // ImPlotAxisFlags_AutoFit   ImPlotAxisFlags_PanStretch
        },
            { ImPlotLocation_NorthEast, ImPlotLegendFlags_None}
    };
    //
    //
    //
    utl::PlotCFG                                    editor_cfg                      = {
        {   "##FDTD_Editor",            ImVec2(-1, -1),     ImPlotFlags_None | ImPlotFlags_NoTitle  },
        {
            { "x-Node Index  [m dx]",   ImPlotAxisFlags_None | ImPlotAxisFlags_AutoFit },
            { "Ez  [V / m]",            ImPlotAxisFlags_None | ImPlotAxisFlags_AutoFit }
        }
    };
    bool                                            m_SHOW_PLOTS [2]                = { true, false };
    std::vector<SketchWidget::Channel>              m_channels                      = {
        {// Data.           Cmap.               paint val.      min.        max.            Map Title.
            {},             Cmap::Perm_E,       {   4.0f,       {1.0f,      16.0f} },       "Relative Permitivitty (Real)",
            nullptr,        "Epsilon-r ' ",         "%.2f",                     120.0f
        //  Map Units.      Scale Title.        Scale Units.                Scale Width.
        },
        {
            {},             Cmap::Perm_B,       {   2.0f,       {0.0f,      8.0f} },        "Relative Permitivitty (Imag)",
            nullptr,        "Epsilon-r '' ",        "%.2f",                     120.0f
        }
    };
    SketchWidget                                    m_editor                        = SketchWidget( editor_cfg, std::move(m_channels) );//m_channels);
    app::AppState                                   CBAPP_STATE_NAME;
    
        
    
    
    //  2.B             PROTECTED MEMBER FUNCTIONS...
    // *************************************************************************** //
    
    //  2B.1        Class Initializations.          [init.cpp]...
    void                init                            (void);
    void                init_ctrl_rows                  (void);
    void                destroy                         (void);
    void                dispatch_plot_function          (const std::string & );
    void                dispatch_ctrl_function          (const std::string & );
    
    
    //  2C.         Core Functions.                 [graph_app.cpp]...
    void                display_plots                   (void);
    void                display_controls                (void);
    
    
    //  2D.         Application Functions.          [tools.cpp]...
    //
    //                  1.  FDTD Functions:
    void                ShowEditor                      (void);
    //
    void                ShowEditorOLD                   (void);
    void                EditorUtility                   (void);
    void                ShowModelParameters             (void);
    void                ShowPlayback                    (void);
    void                ShowPlaybackControls            (void);
    //
    //void                InitializeData                  (void);
    void                StartDataInitAsync              (void);
    //
    //
    //                  2.  Sketch Functions:
    void                ShowSketch                      (void);
    void                ShowSketchControls              (void);
    
    
    
    
    
    
    //  2C.1            Utility Functions...
    void                RebuildDockspace                (void);
    void                AddNewPlotTab                   (void);
    void                AddNewCtrlTab                   (void);
    void                DefaultPlotTabRenderFunc        ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
    void                DefaultCtrlTabRenderFunc        ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
    Tab_t *             get_ctrl_tab                    (const std::string & , std::vector<Tab_t> & );
    void                test_table                      (void);



// *************************************************************************** //
// *************************************************************************** //
};//	END "GraphApp" CLASS PROTOTYPE.







// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.
//
//_CBAPP_WARN_UNUSED_POP





#endif      //  _CBAPP_GRAPH_APP_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.


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
#include CBAPP_USER_CONFIG
#include "cblib.h"
//#include "app/app.h"
//  #include "app/_init.h"
#include "widgets/widgets.h"
#include "utility/utility.h"
#include "utility/pystream/pystream.h"
#include "app/c_counter/_utility.h"


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



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
// *************************************************************************** //
//                         CCounterApp:
// 		        CCounterApp Widget for Dear ImGui.
// *************************************************************************** //
// *************************************************************************** //

//  "CCounterApp"
//
class CCounterApp
{
public:
    using                           buffer_type                     = cblib::RingBuffer<ImVec2, ccounter::BUFFER_SIZE>;     // utl::ScrollingBuffer;     // cblib::RingBuffer<int, 2000>
    using                           AvgMode                         = AvgMode;
    //
    friend class                    App;
    CBAPP_APPSTATE_ALIAS_API        //  CLASS-DEFINED, NESTED TYPENAME ALIASES.
//
//
// *************************************************************************** //
// *************************************************************************** //
public:
    //  1.              INITIALIZATION METHODS...
    // *************************************************************************** //



    // *************************************************************************** //
    //  1.2             PUBLIC API...
    // *************************************************************************** //
    void                            run                             (void);
    //  1.1             Default Constructor, Destructor, etc...
    explicit                        CCounterApp                     (app::AppState & ) noexcept;                        //  Def. Constructor.
                                    ~CCounterApp                    (void);                                             //  Def. Destructor.
                        
    //  1.2             Public Member Functions...
    void                            initialize                      (void);
    void                            save                            (void);
    void                            undo                            (void);
    void                            redo                            (void);
    //
    void                            Begin                           ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
    void                            ToggleAllPlots                  (const char * title_id);
    //
    //                      DELETED OPERATORS AND FUNCTIONS:
                                    CCounterApp                     (const CCounterApp &    )       = delete;   //  Copy. Constructor.
                                    CCounterApp                     (CCounterApp &&         )       = delete;   //  Move Constructor.
    CCounterApp &                   operator =                      (const CCounterApp &    )       = delete;   //  Assgn. Operator.
    CCounterApp &                   operator =                      (CCounterApp &&         )       = delete;   //  Move-Assgn. Operator.
    // *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END PUBLIC API.
    

    
// *************************************************************************** //
//
//
//      2.A             PROTECTED MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
protected:
//
    // *************************************************************************** //
    //      CLASS INITIALIZATIONS.          |   "init.cpp" ...
    // *************************************************************************** //
    void                            init                            (void);
    void                            destroy                         (void);
    void                            dispatch_plot_function          (const std::string & );
    void                            dispatch_ctrl_function          (const std::string & );
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      OPERATION FUNCTIONS.            |   "tools.cpp" ...
    // *************************************************************************** //
    void                            ShowCCPlots                     (void);
    void                            ShowCCControls                  (void);
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      UTILITY FUNCTIONS.              |   "main_application.cpp" ...
    // *************************************************************************** //
    
    
    
    void                            init_ctrl_rows                  (void);
    void                            display_plots                   (void);
    void                            display_controls                (void);
    void                            _draw_control_bar               (void);
    //
    void                            DefaultPlotTabRenderFunc        ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
    void                            DefaultCtrlTabRenderFunc        ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
    Tab_t *                         get_ctrl_tab                    (const std::string & , std::vector<Tab_t> & );
    float                           ComputeAverage                  (const buffer_type &, AvgMode , ImU64 , double , float ) const;
    void                            RebuildDockspace                (void);
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END PROTECTED FUNCTIONS.
    

    
// *************************************************************************** //
//
//
//      3.              PROTECTED DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //
protected:
//
    // *************************************************************************** //
    //      STATIC CONSTEXPR VARIABLES (USED FOR DEFINING CLASS MEMBERS)...
    // *************************************************************************** //
    static constexpr size_t                 ms_NUM                          = 15;
    static constexpr size_t                 ms_BUFFER_SIZE                  = 256;
    Param<ImU64>                            m_PLOT_LIMIT                    = { 0,     {0, ms_NUM-1}        };
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      APPLICATION STATE VARIABLES...
    // *************************************************************************** //
    
    //
    //  ...
    //
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      IMPORTANT DATA...
    // *************************************************************************** //
    std::array<buffer_type, ms_NUM>         m_buffers                       = { };
    std::array<buffer_type, ms_NUM>         m_avg_counts                    = { };
    float                                   m_max_counts[ms_NUM]            = { 0.0f };
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      WIDGET VARIABLES...
    // *************************************************************************** //
    
    //
    //      ...
    //
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      PLOTTING STUFF...
    // *************************************************************************** //
    
    //
    //      ...
    //
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      GENERIC CONSTANTS...
    // *************************************************************************** //
    
    //
    //      ...
    //
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      MISC / UNKNOWN...
    // *************************************************************************** //




    
    //  CONSTANTS.
    std::array<const char *, 2>                             ms_PLOT_UUIDs                   = { "##CCounterMasterPlot",     "##CCounterIndividualPlot"};
    std::array<const char *, 2>                             ms_TABLE_UUIDs                  = { "##IndividualPlotTable",    "##CCounterControlTable"};
    float                                                   m_mst_plot_slider_height        = 20.0f;
    float                                                   m_mst_plot_height               = 400.0f;
    
    //  INDIVIDUAL PLOT STUFF...
    ImGuiTableColumnFlags                                   ms_i_plot_table_flags           = ImGuiTableFlags_None | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoKeepColumnsVisible;
    ImGuiTableColumnFlags                                   ms_i_plot_column_flags          = ImGuiTableColumnFlags_WidthFixed;   //    ImGuiTableColumnFlags_WidthFixed;   ImGuiTableColumnFlags_None
    ImGuiTableColumnFlags                                   ms_i_plot_plot_flags            = ImGuiTableColumnFlags_WidthStretch;
    float                                                   ms_I_PLOT_COL_WIDTH             = 100.0f;
    float                                                   ms_I_PLOT_PLOT_WIDTH            = -1.0f;
    
    float                                                   ms_CENTER                       = 0.95f;
    
    //  MISC APPLICATION STUFF...
    bool                                                    m_toggle_mst_plots              = false;
    bool                                                    m_initialized                   = false;
    
    //  VARIOUS FLAGS...
    ImPlotLineFlags                                         m_channel_flags                 = ImPlotLineFlags_None | ImPlotLineFlags_Shaded;
    ImPlotAxisFlags                                         m_plot_flags                    = ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoDecorations;
    ImPlotFlags                                             m_mst_PLOT_flags                = ImPlotFlags_None | ImPlotFlags_NoTitle;
    ImPlotAxisFlags                                         m_mst_plot_flags                = ImPlotAxisFlags_None | ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoSideSwitch;
    ImPlotAxisFlags                                         m_mst_xaxis_flags               = ImPlotAxisFlags_None | ImPlotAxisFlags_AutoFit;                           // enable grid, disable decorations.
    ImPlotAxisFlags                                         m_mst_yaxis_flags               = ImPlotAxisFlags_None | ImPlotAxisFlags_AutoFit;                           // enable grid, disable decorations.
    
    ImPlotLocation                                          m_mst_legend_loc                = ImPlotLocation_NorthWest;                                                 // legend position.
    ImPlotLegendFlags                                       m_mst_legend_flags              = ImPlotLegendFlags_None; //ImPlotLegendFlags_Outside; // | ImPlotLegendFlags_Horizontal;


    
    //                                              1.  APPEARANCE CONSTANTS...
    float                                               m_dockspace_ratio               = 0.8f;
    const float                                         m_child_corner_radius           = 5.0f;
    float                                               ms_SPACING                      = 50.0f;
    std::pair<int,int>                                  m_HEIGHT_LIMITS[2]              = { { 30, 30 }, { 30, 30 } };
    //
    //
    //                                              2.  CHILDREN PARAMETERS...
    bool                                                m_child_open[2]                 = {true, true};
    std::array< const char *, 2 >                       m_child_ids                     = { "PlotChild",            "ControlChild" };
    ImGuiChildFlags                                     m_child_flags[2]                = {
        ImGuiWindowFlags_None | ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY,
        ImGuiWindowFlags_None | ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY
    };
    app::WinInfo                                             m_detview_window                = {
                                                            "ControlChild",
                                                            ImGuiWindowFlags_None | ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY,
                                                            true,
                                                            nullptr
                                                        };
    std::array< const char *, 2 >                       m_tabbar_uuids                  = { "PlotTabBar##GApp",     "PlotTabBar##GApp" };
    ImGuiTabBarFlags                                    m_tabbar_flags[2]               = {
        ImGuiTabBarFlags_None | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyResizeDown | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_TabListPopupButton,
        ImGuiTabBarFlags_None | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyResizeDown | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton
    };
    ImGuiWindowClass                                    m_window_class[2]               = {};
    
    
    //                                              3.  SUBSIDIARY WINDOWS...
    std::array< const char *, 2 >                       m_win_uuids                     = { "Visuals", "Controls" };
    ImGuiWindowFlags                                    m_docked_win_flags[2]           = {
        ImGuiWindowFlags_None | ImGuiWindowFlags_NoNavFocus,    // ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground,
        ImGuiWindowFlags_None | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoScrollbar     //  ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground
    };
    
    
    //                                              3.  DOCKING SPACE...
    //
    //                                                  Main Dockspace:
    static constexpr const char *                       m_dockspace_name                = "DockHostSpace##CCApp";
    ImGuiDockNodeFlags                                  m_dockspace_flags               = ImGuiDockNodeFlags_None;
    ImGuiID                                             m_dockspace_id                  = 0;
    bool                                                m_rebuild_dockspace             = false;
    //
    //      Nested Dockspaces:
    ImGuiID                                             m_dock_ids[2]                   = { 0, 0 };
    
    
    //                                              4.  IMPORTANT DATA...
    //
    ChannelSpec                                         ms_channels[ms_NUM]             = {
    //                  MASTER.                     SINGLE.                     AVERAGE.
        { 8,  "A",      {true, "##MasterA",         true, "##SingleA",          true, "##AvgA"      }      },
        { 4,  "B",      {true, "##MasterB",         true, "##SingleB",          true, "##AvgB"      }      },
        { 2,  "C",      {true, "##MasterC",         true, "##SingleC",          false, "##AvgC"      }      },
        { 1,  "D",      {true, "##MasterD",         true, "##SingleD",          false, "##AvgD"      }      },
    //
        {12,  "AB",     {true, "##MasterAB",        true, "##SingleAB",         true, "##AvgAB"     }      },
        {10,  "AC",     {true, "##MasterAC",        true, "##SingleAC",         true, "##AvgAC"     }      },
        { 9,  "AD",     {true, "##MasterAD",        true, "##SingleAD",         false, "##AvgAD"     }      },
        { 6,  "BC",     {true, "##MasterBC",        true, "##SingleBC",         false, "##AvgBC"     }      },
        { 5,  "BD",     {true, "##MasterBD",        true, "##SingleBD",         false, "##AvgBD"     }      },
        { 3,  "CD",     {true, "##MasterCD",        true, "##SingleCD",         false, "##AvgCD"     }      },
    //
        {14,  "ABC",    {false, "##MasterABC",      false, "##SingleABC",       false, "##AvgABC"    }      },
        {13,  "ABD",    {false, "##MasterABD",      false, "##SingleABD",       false, "##AvgABD"    }      },
        {11,  "ACD",    {false, "##MasterACD",      false, "##SingleACD",       false, "##AvgACD"    }      },
        { 7,  "BCD",    {false, "##MasterBCD",      false, "##SingleBCD",       false, "##AvgBCD"    }      },
    //
        {15,  "ABCD",   {false, "##MasterABCD",     false, "##SingleABCD",      false, "##AvgABCD"   }      }
    };
    
    
    //                                              4.  IMPORTANT DATA...
    std::vector<Tab_t>                                  ms_PLOT_TABS                    = { };
    std::vector<Tab_t>                                  ms_CTRL_TABS                    = { };
    std::vector<utl::WidgetRow>                         ms_CTRL_ROWS                    = { };
    std::vector<utl::WidgetRow>                         ms_APPEARANCE_ROWS              = { };
    utl::PyStream                                       m_python                        = utl::PyStream(app::PYTHON_DUMMY_FPGA_FILEPATH);
    
    
    //                                              5.  WIDGET VARIABLES...
    bool                                                m_process_running               = false;  
    bool                                                m_counter_running               = true;
    bool                                                m_smooth_scroll                 = false;   // checkbox toggled in UI
    float                                               m_last_packet_time              = 0.0f; // time of last data arrival
    float                                               m_stream_timeout                = 0.25f;    // seconds of silence → freeze
    bool                                                m_streaming_active              = false;    // derived each frame
    //
    bool                                                m_streaming_prev                = false;                            // true on previous frame if data arrived
    bool                                                m_xaxis_paused                  = true;                             // start paused until first sample
    float                                               m_freeze_xmin                   = 0.f;                              // cached limits when paused
    float                                               m_freeze_xmax                   = 0.f;
    float                                               m_freeze_now                    = 0.f;                              // reference time for sparklines when paused
    //
    //
    AvgMode                                             m_avg_mode                      = AvgMode::Samples;                 //  Current mode (UI‑controlled)
    Param<ImU64>                                        m_avg_window_samp               = { 10,     {1, 100}        };      //  N samples (for AvgMode::Samples)
    Param<double>                                       m_avg_window_sec                = { 30.0f,  {5.0f,   90.0}  };      //  Window length in seconds (for AvgMode::Seconds)

    Param<ImU64>                                        m_coincidence_window            = { 10,     {1, 100}        };
    Param<double>                                       m_integration_window            = { 1.00f,  {0.001f, 2.50f} };
    Param<double>                                       m_history_length                = { 30.0f,  {5.0f,   90.0}  };
    char                                                m_filepath[ms_BUFFER_SIZE]      = { };
    bool                                                m_use_mutex_count               = false;
    
    
    //                                              6.  PLOTTING STUFF...
    bool                                                m_colormap_cache_invalid        = true;
    ImPlotColormap                                      m_cmap                          = ImPlotColormap_Cool;
    std::vector<ImVec4>                                 m_plot_colors                   = std::vector<ImVec4>(ms_NUM);
    static constexpr std::array<const char *, 2>        ms_mst_axis_labels              = { "Time  [sec]",      "Counts  [Arb.]" };
    Param<float>                                        m_AVG_OPACITY                   = { 0.50f,      {0.0f,   1.00f}     };
    Param<float>                                        m_AVG_LINEWIDTH                 = { 10.0f,      {1.0f,   20.00f}    };
    
    
    //                                              7.  DELAGATOR CLASSES...
    app::AppState &                                     CBAPP_STATE_NAME;
    
        
    
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


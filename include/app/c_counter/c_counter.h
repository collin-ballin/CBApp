/***********************************************************************************
*
*       ********************************************************************
*       ****       _ C O I N C I D E N C E . C P P  ____  F I L E       ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
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
#include CBAPP_USER_CONFIG



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include "cblib.h"
#include "widgets/widgets.h"
#include "utility/utility.h"
#include "utility/pystream/pystream.h"
#include "app/c_counter/_internal.h"


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

#define     DEF_REFACTOR_CC     1

class CCounterApp
{
//      0.          CONSTANTS AND ALIASES...
// *************************************************************************** //
// *************************************************************************** //
public:

    // *************************************************************************** //
    //      0. |    NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    CBAPP_APPSTATE_ALIAS_API            //  *OR*    CBAPP_CBLIB_TYPES_API       //  FOR CBLIB...
    friend class                            App;
    //
    static constexpr size_t                 ms_BUFFER_SIZE                  = 512ULL;
    using                                   buffer_type                     = cblib::RingBuffer<ImVec2, ms_BUFFER_SIZE>;     // utl::ScrollingBuffer;     // cblib::RingBuffer<int, 2000>
    using                                   AvgMode                         = AvgMode;
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      0. |    STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    static constexpr size_t                 ms_NUM                          = 15;
    static constexpr size_t                 ms_MSG_BUFFER_SIZE              = 256;
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      0. |    REFERENCES TO GLOBAL ARRAYS.
    // *************************************************************************** //
    
//
//
// *************************************************************************** //
// *************************************************************************** //   END "0.  CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//
//      1.          CLASS DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //
protected:
    
    // *************************************************************************** //
    //      1. |    STATE VARIABLES.
    // *************************************************************************** //
    AppState &                              CBAPP_STATE_NAME;

    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      1. |    IMPORTANT DATA-MEMBERS.
    // *************************************************************************** //
    std::array<buffer_type, ms_NUM>         m_buffers                       = {   };
    std::array<buffer_type, ms_NUM>         m_avg_counts                    = {   };
    float                                   m_max_counts[ms_NUM]            = { 0.0f };
    std::vector<Tab_t>                      ms_PLOT_TABS                    = {   };
    std::vector<Tab_t>                      ms_CTRL_TABS                    = {   };
    std::vector<utl::WidgetRow>             ms_CTRL_ROWS                    = {   };
    std::vector<utl::WidgetRow>             ms_APPEARANCE_ROWS              = {   };
    //
    //
    //
    ChannelSpec                             ms_channels[ms_NUM]             = {
    //                  MASTER.                     SINGLE.                     AVERAGE.
        { 8,  "A",      {true, "##MasterA",         true, "##SingleA",          true, "##AvgA"          }      },
        { 4,  "B",      {true, "##MasterB",         true, "##SingleB",          true, "##AvgB"          }      },
        { 2,  "C",      {true, "##MasterC",         true, "##SingleC",          false, "##AvgC"         }      },
        { 1,  "D",      {true, "##MasterD",         true, "##SingleD",          false, "##AvgD"         }      },
    //
        {12,  "AB",     {true, "##MasterAB",        true, "##SingleAB",         true, "##AvgAB"         }      },
        {10,  "AC",     {true, "##MasterAC",        true, "##SingleAC",         true, "##AvgAC"         }      },
        { 9,  "AD",     {true, "##MasterAD",        true, "##SingleAD",         false, "##AvgAD"        }      },
        { 6,  "BC",     {true, "##MasterBC",        true, "##SingleBC",         false, "##AvgBC"        }      },
        { 5,  "BD",     {true, "##MasterBD",        true, "##SingleBD",         false, "##AvgBD"        }      },
        { 3,  "CD",     {true, "##MasterCD",        true, "##SingleCD",         false, "##AvgCD"        }      },
    //
        {14,  "ABC",    {false, "##MasterABC",      false, "##SingleABC",       false, "##AvgABC"       }      },
        {13,  "ABD",    {false, "##MasterABD",      false, "##SingleABD",       false, "##AvgABD"       }      },
        {11,  "ACD",    {false, "##MasterACD",      false, "##SingleACD",       false, "##AvgACD"       }      },
        { 7,  "BCD",    {false, "##MasterBCD",      false, "##SingleBCD",       false, "##AvgBCD"       }      },
    //
        {15,  "ABCD",   {false, "##MasterABCD",     false, "##SingleABCD",      false, "##AvgABCD"      }      }
    };


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      1. |    WIDGET DATA-MEMBERS.
    // *************************************************************************** //
    //                                  PYSTREAM:
    utl::PyStream                           m_python                            = utl::PyStream(app::PYTHON_DUMMY_FPGA_FILEPATH);
    char                                    m_filebuffer[ms_MSG_BUFFER_SIZE]    = {   };
    std::filesystem::path                   m_filepath                          = {"../../scripts/python/fpga_stream.py"};
    //
    //const char *       PYTHON_FPGA_FILEPATH                = "../../scripts/python/fpga.py";
    //const char *       PYTHON_ECHO_FILEPATH                = "../../scripts/python/echo.py";
    //const char *       PYTHON_COUNTER_FILEPATH             = "../../scripts/python/counter.py";
    //const char *       PYTHON_DUMMY_FPGA_FILEPATH          = "../../scripts/python/counter_dummy.py";
    //const char *       PYTHON_DUMMY_FPGA_FILEPATH          = "../../scripts/python/fpga_stream.py";
    //
    //
    float                                   m_stream_timeout                = 0.25f;        //  seconds of silence → freeze
    bool                                    m_streaming_active              = false;        //  derived each frame
    bool                                    m_streaming_prev                = false;        //  true on previous frame if data arrived
    bool                                    m_process_running               = false;
    bool                                    m_counter_running               = true;
    //
    //
    //                                  COINCIDENCE-COUNTER VARIABLES:
    Param<ImU64>                            m_coincidence_window            = { 10,     {1, 100}        };
    Param<double>                           m_integration_window            = { 1.00f,  {0.001f, 2.50f} };
    //
    //
    //
    //                                  PLOT-APPEARANCE STUFF:
    float                                   ms_CENTER                       = 0.95f;
    Param<double>                           m_history_length                = { 30.0f,  {5.0f,   90.0}  };
    float                                   m_last_packet_time              = 0.0f; // time of last data arrival
    float                                   m_freeze_xmin                   = 0.f;                              // cached limits when paused
    float                                   m_freeze_xmax                   = 0.f;
    float                                   m_freeze_now                    = 0.f;                              // reference time for sparklines when paused
    //
    bool                                    m_smooth_scroll                 = false;                            // checkbox toggled in UI
    bool                                    m_xaxis_paused                  = true;                             // start paused until first sample
    //
    //
    //                                  COMPUTATION STUFF:
    bool                                    m_use_mutex_count               = false;
    AvgMode                                 m_avg_mode                      = AvgMode::Samples;                 //  Current mode (UI‑controlled)
    Param<ImU64>                            m_avg_window_samp               = { 10,     {1, 100}        };      //  N samples (for AvgMode::Samples)
    Param<double>                           m_avg_window_sec                = { 30.0f,  {5.0f,   90.0}  };      //  Window length in seconds (for AvgMode::Seconds)
    //
    //
    //
    //
    //
    //
    //
    //
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      1. |    TRANSIENT STATE DATA.
    // *************************************************************************** //
    bool                                    m_initialized                   = false;
    bool                                    m_toggle_mst_plots              = false;
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      1. |    GENERIC DATA.
    // *************************************************************************** //
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      1. |    APPEARANCE DATA.
    // *************************************************************************** //
    float                                   m_mst_plot_slider_height        = 20.0f;
    float                                   m_mst_plot_height               = 400.0f;
    
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      1. |    RUN-TIME CONSTANT DATA.
    // *************************************************************************** //
    Param<ImU64>                            m_PLOT_LIMIT                    = { 0,     {0, ms_NUM-1}        };
    
    //  CONSTANTS.
    std::array<const char *, 2>             ms_PLOT_UUIDs                   = { "##CCounterMasterPlot",     "##CCounterIndividualPlot"};
    std::array<const char *, 2>             ms_TABLE_UUIDs                  = { "##IndividualPlotTable",    "##CCounterControlTable"};
    
    
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      MISC / UNKNOWN...
    // *************************************************************************** //




    
    //  INDIVIDUAL PLOT STUFF...
    ImGuiTableColumnFlags                                   ms_i_plot_table_flags           = ImGuiTableFlags_None | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoKeepColumnsVisible;
    ImGuiTableColumnFlags                                   ms_i_plot_column_flags          = ImGuiTableColumnFlags_WidthFixed;   //    ImGuiTableColumnFlags_WidthFixed;   ImGuiTableColumnFlags_None
    ImGuiTableColumnFlags                                   ms_i_plot_plot_flags            = ImGuiTableColumnFlags_WidthStretch;
    float                                                   ms_I_PLOT_COL_WIDTH             = 100.0f;
    float                                                   ms_I_PLOT_PLOT_WIDTH            = -1.0f;
    
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
    //
    //
    app::WinInfo                                             m_detview_window                = {
          "ControlChild"
        , ImGuiWindowFlags_None | ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY
        , true
        , nullptr
    };
                                                        
                                                        
                                                        
    std::array< const char *, 2 >                       m_tabbar_uuids                  = { "PlotTabBar##GApp",     "PlotTabBar##GApp" };
    ImGuiTabBarFlags                                    m_tabbar_flags[2]               = {
        ImGuiTabBarFlags_None | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyResizeDown | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_TabListPopupButton,
        ImGuiTabBarFlags_None | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyResizeDown | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton
    };
    ImGuiWindowClass                                    m_window_class[2]               = {   };
    
    
    //                                              3.  SUBSIDIARY WINDOWS...
    
    
    //                                              3.  DOCKING SPACE...
    //
    //                                                  Main Dockspace:
    
    
    //                                              4.  IMPORTANT DATA...
    //
    
    
    //                                              4.  IMPORTANT DATA...
    
    
    //                                              5.  WIDGET VARIABLES...
    
    
    //                                              6.  PLOTTING STUFF...
    bool                                                m_colormap_cache_invalid        = true;
    ImPlotColormap                                      m_cmap                          = ImPlotColormap_Cool;
    std::vector<ImVec4>                                 m_plot_colors                   = std::vector<ImVec4>(ms_NUM);
    static constexpr std::array<const char *, 2>        ms_mst_axis_labels              = { "Time  [sec]",      "Counts  [Arb.]" };
    Param<float>                                        m_AVG_OPACITY                   = { 0.50f,      {0.0f,   1.00f}     };
    Param<float>                                        m_AVG_LINEWIDTH                 = { 10.0f,      {1.0f,   20.00f}    };
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "1.  CLASS DATA-MEMBERS".



// *************************************************************************** //
//
//
//      2.A.        PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
public:
    
    // *************************************************************************** //
    //      2.A. |  INITIALIZATION METHODS.         |   "init.cpp" ...
    // *************************************************************************** //
    explicit                            CCounterApp             (app::AppState & ) noexcept;            //  Def. Constructor.
                                        ~CCounterApp            (void);
    //
    void                                initialize              (void);
    
    // *************************************************************************** //
    //      2.A. |  DELETED FUNCTIONS.              |   ...
    // *************************************************************************** //
                                        CCounterApp             (const CCounterApp &    src)        = delete;   //  Copy. Constructor.
                                        CCounterApp             (CCounterApp &&         src)        = delete;   //  Move Constructor.
    CCounterApp &                       operator =              (const CCounterApp &    src)        = delete;   //  Assgn. Operator.
    CCounterApp &                       operator =              (CCounterApp &&         src)        = delete;   //  Move-Assgn. Operator.
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.A. |  MAIN PUBLIC API.                |   "interface.cpp" ...
    // *************************************************************************** //
    void                                Begin                   ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
    void                                save                    (void);
    void                                undo                    (void);
    void                                redo                    (void);
    //
    void                                ToggleAllPlots          (const char * title_id);
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "2A.  PUBLIC MEMBER FUNCS".


    
// *************************************************************************** //
//
//
//      2.B.        PROTECTED MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
protected:

    // *************************************************************************** //
    //      2.B. |  "RULE-OF ..." FUNCTIONS.        |   "init.cpp" ...
    // *************************************************************************** //
    void                                init                                (void);
    void                                destroy                             (void);
    void                                dispatch_plot_function              (const std::string & );
    void                                dispatch_ctrl_function              (const std::string & );
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.B. |  MAIN UI FUNCTIONS.              |   "interface.cpp" ...
    // *************************************************************************** //
    //                              MAIN GUI FUNCTIONS:
    void                                handle_ui                           (void);
    //
    //                              UTILITIES FOR MAIN GUI FUNCTIONS:
    void                                _draw_selector                      (void);
    void                                _draw_inspector                     (void);
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.B. |  OPERATION FUNCTIONS.            |   "tools.cpp" ...
    // *************************************************************************** //
    void                                ShowCCPlots                         (void) noexcept;
    void                                TAB_Controls                        (void) noexcept;
    void                                TAB_Appearance                      (void) noexcept;
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.B. |  UTILITY FUNCTIONS.              |   "main_application.cpp" ...
    // *************************************************************************** //
    void                                init_ctrl_rows                      (void);
    void                                display_plots                       (void);
    void                                display_controls                    (void);
    void                                _draw_control_bar                   (void);
    //
    void                                DefaultPlotTabRenderFunc            ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
    void                                DefaultCtrlTabRenderFunc            ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
    Tab_t *                             get_ctrl_tab                        (const std::string & , std::vector<Tab_t> & );
    float                               ComputeAverage                      (const buffer_type &, AvgMode , ImU64 , double , float ) const;
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "2B.  PROTECTED" FUNCTIONS.

    
   
// *************************************************************************** //
//
//
//      2.C.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
protected:

    // *************************************************************************** //
    //      2.C. |  QUERY FUNCTIONS.
    // *************************************************************************** //
    
        //
        //  ...
        //


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.C. |  SETTER/GETTER FUNCTIONS.
    // *************************************************************************** //
    
        //
        //  ...
        //


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.C. |  CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    
        //
        //  ...
        //
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.C. |  MISC. UTILITY FUNCTIONS.
    // *************************************************************************** //
    
    //  "label"
    //  inline void                         label                               (const char * text)
    //  { utl::LeftLabel(text, this->ms_LABEL_WIDTH, this->ms_WIDGET_WIDTH); ImGui::SameLine(); };
    
    
    
    // *************************************************************************** //
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "2C.  INLINE" FUNCTIONS.






// *************************************************************************** //
// *************************************************************************** //
};//	END "CCounterApp" INLINE CLASS DEFINITION.












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


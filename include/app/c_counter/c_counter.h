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
    //      0. |    STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    static constexpr size_t                 ms_BUFFER_SIZE                  = 128ULL;           //  NUM. OF DATA-PACKETS FROM CCOUNTER.
    static constexpr size_t                 ms_NUM                          = 15;               //  NUM. OF CHANNELS.
    static constexpr size_t                 ms_CMD_MSG_SIZE                 = 512ULL;           //  BUFFER-SIZE FOR MESSAGE TO PYSTREAM.        //  formerly: "ms_MSG_BUFFER_SIZE"
    //
    static constexpr auto	                cv_DEF_COOLDOWN_DURATION	    = std::chrono::milliseconds(500);
    
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      0. |    NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    CBAPP_APPSTATE_ALIAS_API
    friend class                            App;
    //
    //
    using                                   buffer_type                     = cblib::RingBuffer<ImVec2, ms_BUFFER_SIZE>;    //  System-Wide Aliases.
    //  using                                   cblib::utl::anonClock						    = std::chrono::steady_clock;
    //  using                               LabelFn                         = std::function<void(const char *)>     ;
    //
    //
    using                                   PerFrame                        = ccounter::PerFrame_t;                         //  State POD structs.
    using                                   Packet                          = ccounter::CoincidencePacket;
    //
    using                                   ChannelSpec                     = ccounter::ChannelSpec;
    using                                   Style                           = ccounter::CCounterStyle;
    //
    using                                   PythonCMD                       = ccounter::PythonCMD;                          //  Enums.
    using                                   AvgMode                         = AvgMode;
    using                                   ChIndex                         = Packet::Index;
    
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      0. |    REFERENCES TO GLOBAL ARRAYS.
    // *************************************************************************** //
    static constexpr auto                   ms_CMD_STRINGS                  = ccounter::DEF_PYTHON_CMD_FMT_STRINGS;
    
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
    //      1. |    SUBSIDIARY STATE OBJECTS.
    // *************************************************************************** //
    AppState &                              CBAPP_STATE_NAME;
    Style                                   m_style                         = {   };
    PerFrame                                m_perframe                      = {   };


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      1. |    IMPORTANT DATA-MEMBERS.
    // *************************************************************************** //
    //
    //                                  COUNTER DATA:
    std::array<buffer_type, ms_NUM>         m_buffers                       = {   };        //  RAW-DATA values for each counter.
    std::array<buffer_type, ms_NUM>         m_avg_counts                    = {   };        //  AVERAGE values for each counter.
    float                                   m_max_counts[ms_NUM]            = { 0.0f };     //  MAXIMUM values for each counter.
    size_t                                  m_num_packets                   = 0ULL;
    //
    //
    //
    //                                  WIDGET ROWS:
    std::vector<Tab_t>                      ms_PLOT_TABS                    = {   };
    std::vector<Tab_t>                      ms_CTRL_TABS                    = {   };
    std::vector<utl::WidgetRow>             ms_CTRL_ROWS                    = {   };        //  vector that contains each widget for the CONTROL---TAB.
    std::vector<utl::WidgetRow>             ms_APPEARANCE_ROWS              = {   };        //  ...for the APPEARANCE---TAB.
    //
    //
    //
    ChannelSpec                             ms_channels [ms_NUM]            = {
    //                          MASTER---PLOT.                  SINGLE---PLOT.                      AVERAGE---PLOT.
          { 8   , "A"       , { true    , "##MasterA"           , true      , "##SingleA"           , true      , "##AvgA"          }     }
        , { 4   , "B"       , { true    , "##MasterB"           , true      , "##SingleB"           , true      , "##AvgB"          }     }
        , { 2   , "C"       , { true    , "##MasterC"           , true      , "##SingleC"           , false     , "##AvgC"          }     }
        , { 1   , "D"       , { true    , "##MasterD"           , true      , "##SingleD"           , false     , "##AvgD"          }     }
    //
        , {12   , "AB"      , { true    , "##MasterAB"          , true      , "##SingleAB"          , true      , "##AvgAB"         }     }
        , {10   , "AC"      , { true    , "##MasterAC"          , true      , "##SingleAC"          , true      , "##AvgAC"         }     }
        , { 9   , "AD"      , { true    , "##MasterAD"          , true      , "##SingleAD"          , false     , "##AvgAD"         }     }
        , { 6   , "BC"      , { true    , "##MasterBC"          , true      , "##SingleBC"          , false     , "##AvgBC"         }     }
        , { 5   , "BD"      , { true    , "##MasterBD"          , true      , "##SingleBD"          , false     , "##AvgBD"         }     }
        , { 3   , "CD"      , { true    , "##MasterCD"          , true      , "##SingleCD"          , false     , "##AvgCD"         }     }
    //
        , {14   , "ABC"     , { false   , "##MasterABC"         , false     , "##SingleABC"         , false     , "##AvgABC"        }     }
        , {13   , "ABD"     , { false   , "##MasterABD"         , false     , "##SingleABD"         , false     , "##AvgABD"        }     }
        , {11   , "ACD"     , { false   , "##MasterACD"         , false     , "##SingleACD"         , false     , "##AvgACD"        }     }
        , { 7   , "BCD"     , { false   , "##MasterBCD"         , false     , "##SingleBCD"         , false     , "##AvgBCD"        }     }
    //
        , {15   , "ABCD"    , { false   , "##MasterABCD"        , false     , "##SingleABCD"        , false     , "##AvgABCD"       }     }
    };


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      1. |    PROCESS-STREAMING DATA-MEMBERS.
    // *************************************************************************** //
    //
    //                                  PYSTREAM:
    utl::PyStream                           m_python                            = {   };    //  utl::PyStream(app::PYTHON_DUMMY_FPGA_FILEPATH);
    uint32_t                                m_child_pid                         = 0U;
    //
    //                                  PYTHON COMMUNICATION:
    char                                    m_py_message [ms_CMD_MSG_SIZE]      = { '\0' };
    char                                    m_filebuffer [ms_CMD_MSG_SIZE]      = { '\0' };
    //
    //                                  PYTHON PATHS:
    std::filesystem::path                   m_script_filepath                   = {"../../scripts/python/fpga_stream_v3.py"};
    std::filesystem::path                   m_output_filepath                   = {   };
    //
    //
    //
    //                                  COINCIDENCE-COUNTER VARIABLES:
    Param<ImU64>                            m_coincidence_window            = { 10,     {1          , 100   }   };
    Param<double>                           m_integration_window            = { 1.00f,  {0.001f     , 2.50f }   };


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      1. |    WIDGET DATA-MEMBERS.
    // *************************************************************************** //
    //
    //
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
    bool                                    m_process_recording             = false;
    bool                                    m_counter_running               = true;         //  <==| THIS IS SUPPOSED TO BE TRUE BY DEFAULT!!!  DO NOT TOUCH!
    //
    //
    //
    //                                  PLOT-APPEARANCE STUFF:
    float                                   ms_CENTER                       = 0.95f;
    Param<double>                           m_history_length                = { 30.0f,  {5.0f,   90.0}  };
    float                                   m_last_packet_time              = 0.0f;                             //  time of last data arrival
    float                                   m_freeze_xmin                   = 0.0f;                             //  cached limits when paused
    float                                   m_freeze_xmax                   = 0.0f;
    float                                   m_freeze_now                    = 0.0f;                             //  reference time for sparklines when paused
    //
    bool                                    m_smooth_scroll                 = false;                            //  checkbox toggled in UI
    bool                                    m_xaxis_paused                  = true;                             //  start paused until first sample
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
    //  float                                   m_mst_plot_slider_height        = 20.0f;
    //  float                                   m_mst_plot_height               = 400.0f;
    
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      1. |    RUN-TIME CONSTANT DATA.
    // *************************************************************************** //
    //  CONSTANTS.
    static constexpr const char *           ms_CMAP_SELECTION_MENU          = "CCounter_Cmap_SelectionMenu";
    std::array<const char *, 2>             ms_PLOT_UUIDs                   = { "##CCounterMasterPlot"      , "##CCounterIndividualPlot"    };
    std::array<const char *, 2>             ms_TABLE_UUIDs                  = { "##IndividualPlotTable"     , "##CCounterControlTable"      };
    
    
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      MISC / UNKNOWN...
    // *************************************************************************** //
    //
    //                                                  INDIVIDUAL PLOT STUFF...
    ImGuiTableColumnFlags                                   ms_i_plot_table_flags           = ImGuiTableFlags_None | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoKeepColumnsVisible;
    ImGuiTableColumnFlags                                   ms_i_plot_column_flags          = ImGuiTableColumnFlags_WidthFixed;   //    ImGuiTableColumnFlags_WidthFixed;   ImGuiTableColumnFlags_None
    ImGuiTableColumnFlags                                   ms_i_plot_plot_flags            = ImGuiTableColumnFlags_WidthStretch;
    float                                                   ms_I_PLOT_COL_WIDTH             = 100.0f;
    float                                                   ms_I_PLOT_PLOT_WIDTH            = -1.0f;
    
    //                                                  VARIOUS FLAGS...
    ImPlotLineFlags                                         m_channel_flags                 = ImPlotLineFlags_None | ImPlotLineFlags_Shaded;
    //  ImPlotAxisFlags                                         m_plot_flags                    = ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoDecorations;
    //ImPlotFlags                                             m_mst_PLOT_flags                = ImPlotFlags_None | ImPlotFlags_NoTitle;
    //ImPlotAxisFlags                                         m_mst_plot_flags                = ImPlotAxisFlags_None | ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoSideSwitch;


    
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
    bool                                                m_colormap_shuffled             = false;
    //
    //  static constexpr std::array<const char *, 2>        ms_mst_axis_labels              = { "Time  [sec]",      "Counts  [Arb.]" };
    //
    //
    //
    //                                              7.  PLOT APPEARANCE STUFF...
    ImPlotColormap                                      m_cmap                          = ImPlotColormap_Cool;
    //
    //                                              MASTER PLOTS.
    std::vector<ImVec4>                                 m_plot_colors                   = std::vector<ImVec4>(ms_NUM);
    Param<float>                                        m_plot_linewidth                = { 0.60f,      { 2.6f      , 4.00f     }       };
    //
    //                                              AVERAGE PLOTS.
    std::vector<ImVec4>                                 m_avg_colors                    = std::vector<ImVec4>(ms_NUM);
    Param<float>                                        m_avg_opacity                   = { 0.50f,      { 0.0f      , 1.00f     }       };
    Param<float>                                        m_avg_linewidth                 = { 14.0f,      { 4.0f      , 30.00f    }       };
    Param<float>                                        m_avg_color_shade               = { -0.45,      { -1.0f     , 1.00f     }       };
    //
    //
    //                                              INDIVIDUAL PLOTS.
    
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
    explicit                            CCounterApp                         (app::AppState & ) noexcept;            //  Def. Constructor.
                                        ~CCounterApp                        (void);
    //
    void                                initialize                          (void);
    
    
    // *************************************************************************** //
    //      2.A. |  DELETED FUNCTIONS.              |   ...
    // *************************************************************************** //
                                        CCounterApp                         (const CCounterApp &    src)        = delete;   //  Copy. Constructor.
                                        CCounterApp                         (CCounterApp &&         src)        = delete;   //  Move Constructor.
    CCounterApp &                       operator =                          (const CCounterApp &    src)        = delete;   //  Assgn. Operator.
    CCounterApp &                       operator =                          (CCounterApp &&         src)        = delete;   //  Move-Assgn. Operator.
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.A. |  MAIN PUBLIC API.                |   "interface.cpp" ...
    // *************************************************************************** //
    void                                Begin                               ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
    //
    void                                save                                (void);
    void                                undo                                (void);
    void                                redo                                (void);
    //
    void                                ToggleAllPlots                      (const char * title_id);
    
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
    //
    void                                init_ctrl_rows                      (void);                     //  initializes the rows of widgets.
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.B. |  MAIN USER-INTERFACE FUNCTIONS.  |   "c_counter.cpp" ...
    // *************************************************************************** //
    inline void                         _Begin_IMPL                         (void) noexcept;        //  MAIN UPDATE LOOP FOR THE APP  [ called by Begin(...) ]      //  formerly:   "display_plots"
    inline void                         _Begin_DetView_IMPL                 (void) noexcept;        //  MAIN FUNC. for the DETVIEW part of the app (in the bottom tab)...
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.B. |  CORE MECHANICS.                 |   "c_counter.cpp" ...
    // *************************************************************************** //
    //                              MAIN "_MECH" FUNCTIONS:
    inline void                         _MECH_per_frame_cache               (void) noexcept;
    inline void                         _MECH_draw_controls                 (void) noexcept;
    //
    //
    //                              "_MECH" HELPER FUNCTIONS:
    inline void                         _FetchData                          (void) noexcept;
    //
    inline float                        ComputeAverage                      (const buffer_type &, AvgMode , ImU64 , double , float ) const;
    //
    //
    //
    //                              DEPRICATED:
    void                                ShowCCPlots                         (void);                 //  [DEPRECATED]; just keeping a copy for now.  will delete soon.
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.B. |  PLOTTING FUNCTIONS.             |   "plots.cpp" ...
    // *************************************************************************** //
    //                              MAIN PLOTTING FUNCTIONS:
    void                                _PlotMaster                         (void) const noexcept;
    void                                _PlotSingles                        (void) const noexcept;
    //
    //                              PLOTTING UTILITIES:
    template<typename RB = buffer_type>
    inline void                         plot_sparkline                      (const RB & , const ImVec4 & , const ImVec2 , const float , const float , const float , const ImPlotAxisFlags) const noexcept;

        
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.B. |  USER-INTERFACE FUNCTIONS.       |   "interface.cpp" ...
    // *************************************************************************** //
    //                              MAIN USER-INTERFACE:
    void                                TAB_NewControls                     (void) noexcept;
    void                                TAB_Controls                        (void) noexcept;
    void                                TAB_Appearance                      (void) noexcept;
    //
    //
    //                              OTHER GUI FUNCTIONS:
    [[nodiscard]] bool                  _MENU_cmap_selection                (const ImPlotColormap ) noexcept;
    //
    //
    //                              TABBAR FUNCTIONS:
    void                                dispatch_plot_function              (const std::string & );     //  dispatch render_fn for the TOP-TABBAR.
    void                                dispatch_ctrl_function              (const std::string & );     //  dispatch render_fn for the BOTTOM-TABBAR.
    //
    void                                DefaultTabRenderFunc                ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
    Tab_t *                             get_ctrl_tab                        (const std::string & , std::vector<Tab_t> & );
    
    
    
    // *************************************************************************** //
    
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
    //      2.C. |  PYSTREAM COMMUNICATION FUNCTIONS.
    // *************************************************************************** //
        
    //  "_format_cmd_string"
    inline void                             _format_cmd_string                  (const PythonCMD type, const char * message) noexcept
    {
        return;
    }
     
     
    //  "_send_message"
    inline bool                             _send_message                       (void) noexcept
    {
        bool status = false;
        
        
        if ( this->m_process_running )
        {
            status = this->m_python.send(this->m_py_message);
        }
        else {
            //  this->S.m_logger.warning("[[CCounter]] failed to deliver message to PyStream -- process not running");
        }
        
        
        
        return status;
    }
    

    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.C. |  CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
        
    //  "_start_process"
    inline void                             _start_process                      (const bool run_and_rec=false) noexcept
    {
        IM_ASSERT(!this->m_process_running);    //  [[ TO-DO ]]:    Make this so that it only asserts script is not running if run_and_rec is NOT also true (ALLOW recording in middle of execution).
        IM_ASSERT( !(this->m_process_recording && run_and_rec) );
        
        
        
        if ( !this->m_process_running )
        {
            const bool      spawned     = this->_start_process_IMPL();
            
            if ( !spawned ) {
                this->_display_error_popup();
            }
            
            //      CASE 1 :    REQUEST TO RUN *AND* RECORD...
            if ( run_and_rec  &&  spawned )
            {
                const bool  recording       = this->_start_recording();
                
                
                //      ERROR : PROCESS PLAYBACK WAS BLOCKED BY RECORDING-ISSUE...
                if (!recording) {
                    this->m_process_running     = false;
                    this->m_process_recording   = false;
                    //  ui::ask_ok_cancel(   "Recording Error"
                    //                     , "An issue with the current record settings prevented Python from launching."
                    //                     , [ ]{   });
                }
                else {
                    this->m_process_recording   = true;
                }
            }
            
            
            //      CASE 2 :    FAILURE TO START PROCESS...
            if ( !m_process_running )
            {
                //  ui::ask_ok_cancel(   "Python Error"
                //                     , "Failure to launch Python child process."
                //                     , [ ]{   });
            }
            //
            //      CASE 2 :    PROCESS SUCCESSFULLY STARTED...
            else
            {
                m_max_counts[0]     = 0.0f;     // reset stats
            }
        }
        
        return;
    }
    
    //  "_stop_process"
    inline void                             _stop_process                       (const bool pause=false) noexcept
    {
        IM_ASSERT(this->m_process_running);
        
        //      CASE 1 :    *PAUSE* RECORDING  [ stop playback but leave recording status ].
        if (pause) {
            this->m_python.stop();
            this->m_process_running         = false;
            return;
        }
        //
        //      CASE 2 :    CEASE PLAYBACK *AND* FINALIZE RECORDING...
        else {
            this->m_python.stop();
            this->m_process_running         = false;
            this->_stop_recording();
        }
        
        return;
    }
    
    
    //  "_start_recording"
    inline bool                             _start_recording                    (void) noexcept {
    
        //  if (this->m_process_running) {
        //
        //  }
        
        this->m_process_recording = true;
        return true;
    }
    
    
    //  "_stop_recording"
    inline void                             _stop_recording                     (void) noexcept {
        this->m_process_recording = false;
        return;
    }
    
    
    //  "_clear_all"
    inline void                             _clear_all                          (void) noexcept
    {
        //      CASE 1 :    STOP RECORDING...
        if (this->m_process_recording) {
            this->_stop_process(false);
        }
        
        this->_clear_plot_data();
        this->_reset_max_values();
        this->_reset_average_values();
        this->m_num_packets     = 0ULL;
        
        
        //      2.      RESET AXES BOUNDS...
        this->m_perframe.clear();
        
        return;
    }
    
    
    
    //  "_clear_plot_data"
    inline void                             _clear_plot_data                    (void) noexcept
    {
        for (auto & b : this->m_buffers)      { b.clear(); }   //b.Erase();
        this->_reset_max_values();
        
        return;
    }
    
    
    //  "_reset_max_values"
    inline void                             _reset_max_values                   (void) noexcept {
        std::fill(std::begin(m_max_counts), std::end(m_max_counts), 0.0f);
        return;
    }
    
    
    //  "_reset_average_values"
    inline void                             _reset_average_values               (void) noexcept
    {
        for (auto & vec : m_avg_counts) {
            vec.clear();    //  b.Erase();
        }
        return;
    }
    
    
    //  "_validate_colormap_cache"
    inline void                             _validate_colormap_cache            (void) noexcept
    {
        const float     alpha           = this->m_avg_opacity.Value();
        const float     shade           = this->m_avg_color_shade.Value();
            
        
        if (this->m_colormap_cache_invalid)
        {
            if ( !this->m_colormap_shuffled ) {
                this->m_plot_colors         = cblib::utl::GetColormapSamples( ms_NUM, m_cmap );
            }
            else {
                this->m_plot_colors         = cblib::utl::GetHDRAdjacentColormapSamples( this->ms_NUM, this->m_cmap );
            }
            
            for (size_t i = 0; i < ms_NUM; ++i) {
                this->m_avg_colors[i]       = cblib::utl::compute_tint( m_plot_colors[i], shade );
                this->m_avg_colors[i].w     = alpha;
            }
        }
        
        this->m_colormap_cache_invalid      = false;
        return;
    }


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.C. |  INTERNAL FUNCTIONS.
    // *************************************************************************** //
        
    //  "_start_process_IMPL"
    //
    [[nodiscard]] inline bool               _start_process_IMPL                 (void) noexcept
    {
        namespace       cc          = ccounter;
        
        std::string &   header      = cc::s_last_error_title;
        std::string &   body        = cc::s_last_error_message;
        uint32_t        pid         = 0U;
        bool            spawned     = false;
        
        
        // OPTIONAL:    configure before start()
        //
        //      this->m_python.set_python_executable("/path/to/venv/bin/python");
        //      this->m_python.set_working_directory("/path/to/project");
        //      this->m_python.set_filepath("/path/to/script.py");
        //      this->m_python.set_args({ "-u", "--flag", "value" });

    
    
    
        //      1.      ATTEMPT TO BEGIN PROCESS...
        try {
            pid         = this->m_python.start();
            if (pid)    { spawned = true; }
        }
        //
        //      2.      CATCH-BLOCKS...
        catch (const std::invalid_argument &    e)      { body = std::format( "invalid_argument: {}\n"      , e.what()      );      }
        catch (const std::logic_error &         e)      { body = std::format( "logic_error: {}\n"           , e.what()      );      }
        catch (const std::system_error &        e) {
            body = std::format(
                  "system_error ({}): {}\n"
                , (int)e.code().value()
                , e.what()
            );
        }
        catch (const std::runtime_error &       e)      { body = std::format( "runtime_error: {}\n"         , e.what()      );      }
        catch (const std::exception &           e)      { body = std::format( "exception: {}\n"             , e.what()      );      }
        catch (...)                                     { body = std::format( "unknown exception"                           );      }



            
        //          CASE A :    PROCESS SUCCESSFULLY STARTED.
        if (spawned) {
            this->m_child_pid           = pid;
            this->m_process_running     = true;
            this->S.m_logger.info( std::format(
                  "[[CCounter]] spawned process pid={} (invocation: \"{}\")"
                , pid
                , this->m_python.get_invocation()
            ) );
        }
        //
        //          CASE B :    FAILURE TO BEGIN PROCESS.
        else {
            this->m_child_pid           = 0U;
            this->m_process_running     = false;
            this->S.m_logger.info( std::format("[[CCounter]] failed to spawn python process") );
            header                      = "ERROR";
        }

        return spawned;
    }
        
        
    //  "_display_error_popup"
    //
    inline void                             _display_error_popup                (void) noexcept
    {
        namespace       cc          = ccounter;
    
    
        ui::ask_ok_cancel(
              cc::s_last_error_title.c_str()
            , cc::s_last_error_message.c_str()
            , [this]{ /* this->_clear_all(); */ }
        );
        
        
        return;
    }
    
    
    
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
    // *************************************************************************** //
    //      2.C. |  STATIC INLINE FUNCTIONS.
    // *************************************************************************** //
        
    //
    //  ...
    //
    
    
    
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


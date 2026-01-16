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
#ifdef __CBAPP_DEBUG__
    static constexpr size_t                 ms_BUFFER_SIZE                  = 1024ULL;                          //  NUM. OF DATA-PACKETS FROM CCOUNTER.
#else
    static constexpr size_t                 ms_BUFFER_SIZE                  = 1024ULL;
#endif  //  __CBAPP_DEBUG__  //
//
    static constexpr size_t                 ms_NUM                          = ccounter::DEF_CHANNEL_COUNT;      //  NUM. OF CHANNELS.
    static constexpr size_t                 ms_CMD_MSG_SIZE                 = 512ULL;                           //  BUFFER-SIZE FOR MESSAGE TO PYSTREAM.        //  formerly: "ms_MSG_BUFFER_SIZE"
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
    using                                   buffer_type                     = cblib::ndRingBuffer<ImVec2>;    //  System-Wide Aliases.
    //  using                               cblib::utl::anonClock						    = std::chrono::steady_clock;
    //  using                               LabelFn                         = std::function<void(const char *)>     ;
    //
    //
    //                              C-COUNTER DATA PACKET ABSTRACTIONS:
    using                                   Counter_t                       = size_t;
    using                                   Frequency_t                     = size_t;
    using                                   Packet_t                        = ccounter::CoincidencePacket_t<Counter_t, Frequency_t>;
    using                                   PerFrame_t                      = ccounter::PerFrame_t<float, Counter_t>;                 //  State POD structs.
    //
    using                                   ChannelSpec                     = ccounter::ChannelSpec;
    //
    //
    //                              C-COUNTER APP ABSTRACTIONS:
//                                      Enum Types.
    using                                   AvgMode                         = ccounter::AvgMode;
    using                                   ChIndex                         = Packet_t::Index;
//                                      Larger Types.
    using                                   Style                           = ccounter::CCounterStyle;
    //
    //
    //                              PYTHON IPC ABSTRACTIONS:
    using                                   PacketType                      = ipc::PythonPacketType;                                //  Enums.
    using                                   PythonCMD                       = ipc::PythonCMD;
    using                                   IPCState                        = ipc::PythonIPC_t;
    
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      0. |    REFERENCES TO GLOBAL ARRAYS.
    // *************************************************************************** //
    static constexpr auto                   ms_PACKET_TYPE_NAMES            = ipc::DEF_PYTHON_PACKET_TYPE_NAMES;
    static constexpr auto                   ms_CMD_STRINGS                  = ipc::DEF_PYTHON_CMD_FMT_STRINGS;
    //
    inline static auto                      ms_channels                     = ccounter::DEF_CHANNEL_INFOS;
    //
    inline static auto                      ms_AVG_MODE_NAMES               = ccounter::DEF_AVG_MODE_NAMES;
    inline static auto                      ms_AVG_MODE_FMT_STRINGS         = ccounter::DEF_AVG_MODE_FMT_STRINGS;
    
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
    PerFrame_t                              m_perframe                      = {   };


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      1. |    IMPORTANT DATA-MEMBERS.
    // *************************************************************************** //
    //
    //                                  COUNTER DATA:
    std::array<buffer_type, ms_NUM>         m_buffers                       = {      };     //  RAW-DATA values for each counter.
    std::array<buffer_type, ms_NUM>         m_avg_counts                    = {      };     //  AVERAGE values for each counter.
    std::array<float      , ms_NUM>         m_max_counts                    = { 0.0f };     //  MAXIMUM values for each counter.
    size_t                                  m_num_packets                   = 0ULL;
    //
    //
    //
    //                                  WIDGET ROWS:
    std::vector<Tab_t>                      ms_PLOT_TABS                    = {   };
    std::vector<Tab_t>                      ms_CTRL_TABS                    = {   };
    std::vector<utl::WidgetRow>             ms_CTRL_ROWS                    = {   };        //  vector that contains each widget for the CONTROL---TAB.
    std::vector<utl::WidgetRow>             ms_APPEARANCE_ROWS              = {   };        //  ...for the APPEARANCE---TAB.


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      1. |    PROCESS-STREAMING DATA-MEMBERS.
    // *************************************************************************** //
    //
    //                                  PYSTREAM:
    IPCState		                        m_ipc_state                         = {   };
    utl::PyStream                           m_python                            = {   };    //  utl::PyStream(app::PYTHON_DUMMY_FPGA_FILEPATH);
    uint32_t                                m_child_pid                         = 0U;
    //
    //                                  PYTHON COMMUNICATION:
    char                                    m_py_message [ms_CMD_MSG_SIZE]      = { '\0' };
    char                                    m_filebuffer [ms_CMD_MSG_SIZE]      = { '\0' };
    //
    //                                  PYTHON PATHS:
    std::filesystem::path                   m_python_interpreter_path           = {    };//    {"~/.venvs/CBenv/"};
    std::filesystem::path                   m_script_filepath                   = {"../../scripts/python/fpga_stream_v3.py"};
    std::filesystem::path                   m_output_filepath                   = {   };
    //
    //
    //
    //                                  COINCIDENCE-COUNTER VARIABLES:
    Param<ImU64>                            m_coincidence_window                = { 10,     {1          , 100   }   };
    Param<double>                           m_integration_window                = { 1.00f,  {0.01f      , 2.50f }   };


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      1. |    GLOBAL APPLICATION-WIDE BEHAVIOR TOGGLES.
    // *************************************************************************** //
    //                                  BEHAVIOR-TOGGLES:
    //
    //                                      Data Aquisition.
    bool                                    m_use_mutex_count               = false;
    bool                                    m_recording_armed               = false;
    //
    //
    //                                      Plot Appearance.
    bool                                    m_use_relative_range            = true;         //  if (true):  Y-Range = [Y-MIN, Y-MAX].       if (false):     Y-Range [0, Y-MAX].
    bool                                    m_use_shuffled_colormap         = true;         //  (true):     Use optimized plot-colors.      (false):        Use naïve shuffled plot colors.
    //
    bool                                    m_smooth_scroll                 = false;        //  (true): Gradual, Smooth plot updates; continuous time.      (false): stop-motion, abrupt plot updates; discrete time.
    bool                                        m_xaxis_paused                  = true;     //  start paused until first sample
    //                                                                                          ^ [[TO-DO]]: DON'T TOUCH.  NEED TO REMOVE THIS.
    //
    //
    //                                  OTHER APP SETTINGS:
    AvgMode                                 m_avg_mode                      = AvgMode::Samples;
    ImPlotColormap                          m_cmap                          = ImPlotColormap_Cool;
    //

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
    //
    static constexpr float                  ms_TIMEOUT_DURATION             = 0.25f;        //  seconds of silence → freeze
    float                                   m_stream_timeout                = 0.25f;        //  ...
    bool                                    m_streaming_active              = false;        //  derived each frame
    bool                                    m_streaming_prev                = false;        //  true on previous frame if data arrived
    bool                                    m_process_running               = false;
    bool                                    m_process_recording             = false;
    bool                                    m_counter_running               = true;         //  <==| THIS IS SUPPOSED TO BE TRUE BY DEFAULT!!!  DO NOT TOUCH!
    //
    //
    //
    //                                  PLOT-APPEARANCE STUFF:
    double                                  ms_CENTER                       = 0.95f;            //  Rel. Position of the "Playhead" for most-recent data.
    double                                  ms_MARGIN                       = 1.10f;            //  Multiple of Y-Max value to add additional height to y-axis limits.
    Param<double>                           m_history_length                = { 30.0f,  {5.0f,   90.0}  };
    float                                   m_last_packet_time              = 0.0f;                             //  time of last data arrival
    float                                   m_freeze_xmin                   = 0.0f;                             //  cached limits when paused
    float                                   m_freeze_xmax                   = 0.0f;
    float                                   m_freeze_now                    = 0.0f;                             //  reference time for sparklines when paused
    //
    //
    //
    //                                  COMPUTATION STUFF:
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
    bool                                    m_initialized                   = false;    //  If the `Applet` has been initialized.
    bool                                    m_toggle_mst_plots              = false;
    //
    bool                                    m_colormap_cache_invalid        = true;     //  If the user made a change and the plot colors need to be updated.
    
    
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
    //
    //  static constexpr std::array<const char *, 2>        ms_mst_axis_labels              = { "Time  [sec]",      "Counts  [Arb.]" };
    //
    //
    //
    //                                              7.  PLOT APPEARANCE STUFF...
    //
    //                                              MASTER PLOTS.
    std::vector<ImVec4>                                 m_plot_colors                   = std::vector<ImVec4>(ms_NUM);
    Param<float>                                        m_plot_linewidth                = { 0.80f,      { 0.40f      , 4.00f     }       };
    //
    //                                              AVERAGE PLOTS.
    std::vector<ImVec4>                                 m_avg_colors                    = std::vector<ImVec4>(ms_NUM);
    Param<float>                                        m_avg_opacity                   = { 0.50f,      { 0.00f     , 1.00f     }       };
    Param<float>                                        m_avg_linewidth                 = { 14.0f,      { 4.0f      , 30.00f    }       };
    Param<float>                                        m_avg_color_shade               = { -0.45,      { -1.00f    , 1.00f     }       };
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
    void                                open                                (void);
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
        
    //  "_send_cmd"
    //
    inline bool                             _send_cmd                           (const PythonCMD type) noexcept
    {
        std::string     command     = {   };
        bool            received    = false;
    
        //  CASE 0 :    PROCESS IS NOT RUNNING...
        if ( !this->m_process_running )         { return received; }
        
        
        
        //      1.      DISPATCH EACH FORMAT-STRING COMMAND...
        switch (type)
        {
            case PythonCMD::IntegrationWindow : {
                command     = std::vformat(
                      CCounterApp::ms_CMD_STRINGS[type]
                    , std::make_format_args( this->m_coincidence_window.Value() )
                );
                break;
            }
            case PythonCMD::CoincidenceWindow : {
                command     = std::vformat(
                      CCounterApp::ms_CMD_STRINGS[type]
                    , std::make_format_args( this->m_coincidence_window.Value() )
                );
                break;
            }
            //
            //
            //
            default : {
                break;
            }
        }
        //
        //      2.      TRANSMIT MESSAGE TO THE PROCESS...
        received = this->m_python.send(command);
        
        
        
        return received;
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
                return;
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
                //  m_max_counts[0]     = 0.0f;     // reset stats
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
        
        this->m_ipc_state.ClearAll();
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
            if ( !this->m_use_shuffled_colormap ) {
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

    //  "_setup_pystream"
    //
    inline void                             _setup_pystream                     (void)
    {
        namespace           fs                              = std::filesystem;

    #ifdef _WIN32
        static constexpr const char *   cv_DEF_PY_EXE_REL   = ".venvs/CBenv/Scripts/python.exe";
        const char *                    home_cstr           = std::getenv("USERPROFILE");
    #else
        static constexpr const char *   cv_DEF_PY_EXE_REL   = ".venvs/CBenv/bin/python";
        const char *                    home_cstr           = std::getenv("HOME");
    #endif

        const fs::path                  home_path           = (home_cstr != nullptr) ? fs::path(home_cstr) : fs::path{};
        const fs::path                  def_py_exe          = (!home_path.empty()) ? (home_path / cv_DEF_PY_EXE_REL) : fs::path{};


        //  1.  PYTHON INTERPRETER...
        if (this->m_python_interpreter_path.empty() && !def_py_exe.empty())
        {
            this->m_python_interpreter_path                  = def_py_exe;
        }

        if ( !this->m_python_interpreter_path.empty() )
        {
            try {
                this->m_python.set_python_executable( this->m_python_interpreter_path );
                CB_LOG(
                      LogLevel::Debug
                    , "[[CCounter]] using python interpreter at filepath, \"{}\""
                    , this->m_python_interpreter_path.string()
                );
            }
            catch (...) {
                CB_LOG(
                      LogLevel::Warning
                    , "[[CCounter]] no python interpreter exists at default filepath, \"{}\""
                    , this->m_python_interpreter_path.string()
                );
                this->m_python_interpreter_path.clear();
            }
        }
        else
        {
            CB_LOG(
                  LogLevel::Warning
                , "[[CCounter]] unable to resolve a home directory; using default python interpreter"
            );
        }


        //  2.  PYTHON SCRIPT...
        if ( !this->m_script_filepath.empty() )
        {
            try {
                this->m_python.set_filepath( this->m_script_filepath );
                CB_LOG(
                      LogLevel::Debug
                    , "[[CCounter]] using python script at filepath, \"{}\""
                    , this->m_script_filepath.relative_path().string()
                );
            }
            catch (...) {
                CB_LOG(
                      LogLevel::Warning
                    , "[[CCounter]] no python script exists at default filepath, \"{}\""
                    , this->m_script_filepath.relative_path().string()
                );
            }
        }

        return;
    }

        
        
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
        try
        {
            pid         = this->m_python.start();
            
            if (pid)
            {
                spawned                             = true;
                //
                std::string     integration_str     = std::format( "integration_window {:.3f}\n"    , m_integration_window.Value()  );
                std::string     coincidence_str     = std::format( "coincidence_window %llu\n"      , m_coincidence_window.Value()  );
                
                m_python.send(integration_str);
                m_python.send(coincidence_str);
                
                
                //  Integration Window
                //      std::snprintf(cmd, ms_CMD_MSG_SIZE, "integration_window %.3f\n", m_integration_window.value);
                //      m_python.send(cmd);
                //
                //  Coincidence Window
                //      std::snprintf(cmd, ms_CMD_MSG_SIZE, "coincidence_window %llu\n", m_coincidence_window.value);
                //      m_python.send(cmd);
            }
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
            , std::string( "\n\t" + cc::s_last_error_message ).c_str()
            , []{ } // [this]{ /* this->_clear_all(); */ }
        );
        return;
    }
    
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.C. |  MISC. UTILITY FUNCTIONS.
    // *************************************************************************** //
        
    //  "_allocate_buffers"
    //
    inline void                             _allocate_buffers                   (const size_t buffer_size=ms_BUFFER_SIZE) noexcept
    {
        //      1.      ALLOCATE HEAP-MEMORY FOR EACH BUFFER...
        for (size_t i = 0ULL; i < CCounterApp::ms_NUM; ++i)
        {
            this->m_buffers[i]      .set_capacity(buffer_size);     //  1.  MAIN COUNTER DATA.
            this->m_avg_counts[i]   .set_capacity(buffer_size);     //  2.  AVG-VALUE DATA.
        }
        
        
        {
            const size_t    spb_bytes       = sizeof(buffer_type);       //  "spb" == "size-per-buffer"
            const size_t    buff_bytes      = 2ULL * ( CCounterApp::ms_NUM * spb_bytes );
            this->S.m_logger.info( std::format(
                  "[[CCounter]] allocated {} buffers with {} elements each ({} bytes)"
                , 2 * CCounterApp::ms_NUM
                , buffer_size
                , buff_bytes
            ));
        }
        return;
    }



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

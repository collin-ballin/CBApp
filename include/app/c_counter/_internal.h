/***********************************************************************************
*
*       ********************************************************************
*       ****            _ I N T E R N A L . H  ____  F I L E            ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      October 31, 2025.
*              MODULE:      CBAPP > CCOUNTER/           | _internal.h
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_COUNTER_INTERNAL_H
#define _CBAPP_COUNTER_INTERNAL_H  1
#include CBAPP_USER_CONFIG



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//      0.1.        ** MY **  HEADERS...
#include "cblib.h"
//  #include "widgets/widgets.h"
//  #include "utility/utility.h"
//  #include "utility/pystream/pystream.h"
//
#include "app/c_counter/_types.h"
#include "app/c_counter/_python_ipc.h"


//      0.2         STANDARD LIBRARY HEADERS...
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


//      0.3.        "DEAR IMGUI" HEADERS...
#include "json.hpp"
//
#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_internal.h"
#include "implot.h"
#include "implot_internal.h"





namespace cb { namespace ccounter { //     BEGINNING NAMESPACE "cb::ccounter"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//
//
//
//      0.      STATIC, INTERNAL, NAMESPACE-GLOBAL DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //

//  { //     BEGINNING NAMESPACE "cb::ccounter"...
//
//
// *************************************************************************** //
    inline static constexpr size_t      ms_CMD_BUFFER_SIZE              = 512;
    //
    static bool                         enter                           = false;
    static float                        row_height_px                   = 60.0f;
//
//
    static float                        delay_s                         = 1.0f;                 // command slider
    static char                         line_buf[ms_CMD_BUFFER_SIZE]    {   };                  // manual send box
//
    static std::string                  s_last_error_title              {   };                  // FOR ERRORS...
    static std::string                  s_last_error_message            {   };                  // FOR ERRORS...
//
//
    static ImGuiInputTextFlags          write_file_flags            = ImGuiInputTextFlags_None | ImGuiInputTextFlags_ElideLeft | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue;

// *************************************************************************** //
//
//
//  } //     BEGINNING NAMESPACE "cb::ccounter"...



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 0.  "STATIC NAMESPACE-GLOBALS." ]].












// *************************************************************************** //
//
//
//
//      2.      COINCIDENCE COUNTER ABSTRACTIONS...
// *************************************************************************** //
// *************************************************************************** //



    
// *************************************************************************** //
//      2A. TYPES |        LARGER ABSTRACTIONS.
// *************************************************************************** //

//  "PerFrame_t"
//
//      [[ TO-DO ]]:    REPLACE THIS!!!
//                      This is just a POD struct to carry the per-frame data we need throughout the CCounter class.
//
template <typename float_type, typename int_t>
struct PerFrame_t {
//
//
//
    std::string     raw                 {   };
    float_type      xmin                = float_type(0.00);     //  float_type(-1.0);
    float_type      xmax                = float_type(10.0);     //  float_type(-1.0);
    int_t           ymin                = int_t(0);
    int_t           ymax                = int_t(10);
    //
    ChannelID       current_min         = ChannelID::None;
    ChannelID       current_max         = ChannelID::None;
    //
    float           now                 = 0.0f;                 //  -1.0f;
    float           spark_now           = 0.0f;                 //  -1.0f;
//
    bool            got_packet          = false;
    bool            crawling            = false;
//
//
//
    //  "clear"
    //
    inline void clear(void) noexcept
    {
        this->raw             .clear();
        this->xmin            = float_type(0.00);     //  float_type(-1.0);
        this->xmax            = float_type(10.0);     //  float_type(-1.0);
        this->ymin            = int_t(0);
        this->ymax            = int_t(10);
        //
        this->now             = 0.00f;                  //  -1.0f;
        this->spark_now       = 0.00f;                  //  -1.0f;
        this->got_packet      = false;
        return;
    }
};


//  "CoincidencePacket_t"
//      Holds a single FPGA “packet”
//
template <typename T, typename U>
struct CoincidencePacket_t
{
    using                                   Index               = ChannelID;
    using                                   value_type          = T;
    using                                   frequency_type      = U;
//
//
//
    cblib::EnumArray<Index, value_type>     counts              = {   };
    frequency_type                          frequency           = frequency_type(0);
    //
    //
    //                                      CONVENIENCE ACCESSORS:
    [[nodiscard]] frequency_type                get_frequency       (void) const noexcept           {   return this->frequency;                     }
    //
    [[nodiscard]] value_type                    d                   (void) const noexcept           {   return this->counts[ Index::D       ];      }
    [[nodiscard]] value_type                    c                   (void) const noexcept           {   return this->counts[ Index::C       ];      }
    [[nodiscard]] value_type                    cd                  (void) const noexcept           {   return this->counts[ Index::CD      ];      }
    [[nodiscard]] value_type                    b                   (void) const noexcept           {   return this->counts[ Index::B       ];      }
    [[nodiscard]] value_type                    bd                  (void) const noexcept           {   return this->counts[ Index::BD      ];      }
    [[nodiscard]] value_type                    bc                  (void) const noexcept           {   return this->counts[ Index::BC      ];      }
    [[nodiscard]] value_type                    bcd                 (void) const noexcept           {   return this->counts[ Index::BCD     ];      }
    [[nodiscard]] value_type                    a                   (void) const noexcept           {   return this->counts[ Index::A       ];      }
    [[nodiscard]] value_type                    ad                  (void) const noexcept           {   return this->counts[ Index::AD      ];      }
    [[nodiscard]] value_type                    ac                  (void) const noexcept           {   return this->counts[ Index::AC      ];      }
    [[nodiscard]] value_type                    acd                 (void) const noexcept           {   return this->counts[ Index::ACD     ];      }
    [[nodiscard]] value_type                    ab                  (void) const noexcept           {   return this->counts[ Index::AB      ];      }
    [[nodiscard]] value_type                    abd                 (void) const noexcept           {   return this->counts[ Index::ABD     ];      }
    [[nodiscard]] value_type                    abc                 (void) const noexcept           {   return this->counts[ Index::ABC     ];      }
    [[nodiscard]] value_type                    abcd                (void) const noexcept           {   return this->counts[ Index::ABCD    ];      }
    //
    //                                      OVERLOADED ACCESSORS:
    [[nodiscard]] inline value_type &           operator []         (const Index id) noexcept       { return counts[id]; }
    [[nodiscard]] inline const value_type &     operator []         (const Index id) const noexcept { return counts[id]; }
};



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 2.  "CCOUNTER ABSTRACTIONS." ]].












// *************************************************************************** //
//
//
//
//      3.      OTHER...
// *************************************************************************** //
// *************************************************************************** //




// *************************************************************************** //
//      3A. OTHER |         CCOUNTER---STYLE.
// *************************************************************************** //

//  "CCounterStyle"
//      PLAIN-OLD-DATA (POD) STRUCT.
//
struct CCounterStyle
{
    // *************************************************************************** //
    //      0. |    NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    CBAPP_APPSTATE_ALIAS_API            //  *OR*    CBAPP_CBLIB_TYPES_API       //  FOR CBLIB...
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      0. |    STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    static constexpr float              ms_TOOLBAR_ICON_SCALE           = 1.5f;
    
//
// *************************************************************************** //
// *************************************************************************** //   END "0.  CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//      1.          DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      1. |    IMPORTANT DATA-MEMBERS.
    // *************************************************************************** //
    
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      1. |    UI---DIMENSION VARIABLES.
    // *************************************************************************** //
    //
    //                                  MASTER PLOT:
    float                                   m_mst_plot_slider_height        = 20.0f;
    float                                   m_mst_plot_height               = 400.0f;
    mutable ImVec2                          m_mst_avail                     = { -1.0f   , -1.0f };
    
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      1. |    MASTER PLOT STUFF.
    // *************************************************************************** //
    //  ImPlotLineFlags                                         m_channel_flags                 = ImPlotLineFlags_None | ImPlotLineFlags_Shaded;
    //  ImPlotAxisFlags                                         m_plot_flags                    = ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoDecorations;
    //  ImPlotFlags                                             m_mst_PLOT_flags                = ImPlotFlags_None | ImPlotFlags_NoTitle;
    //  ImPlotAxisFlags                                         m_mst_plot_flags                = ImPlotAxisFlags_None | ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoSideSwitch;
    //  ImPlotAxisFlags                                         m_mst_xaxis_flags               = ImPlotAxisFlags_None | ImPlotAxisFlags_AutoFit;                           // enable grid, disable decorations.
    //  ImPlotAxisFlags                                         m_mst_yaxis_flags               = ImPlotAxisFlags_None | ImPlotAxisFlags_AutoFit;                           // enable grid, disable decorations.
    
    //  ImPlotLocation                                          m_mst_legend_loc                = ImPlotLocation_NorthWest;                                                 // legend position.
    //  ImPlotLegendFlags                                       m_mst_legend_flags              = ImPlotLegendFlags_None; //ImPlotLegendFlags_Outside; // | ImPlotLegendFlags_Horizontal;
    
    
    ImPlotFlags                             mst_plot_flags                  =  ImPlotFlags_NoFrame | ImPlotFlags_NoBoxSelect | ImPlotFlags_NoTitle; // ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoDecorations;  //  ImPlotFlags_Equal | ImPlotFlags_NoFrame | ImPlotFlags_NoBoxSelect | ImPlotFlags_NoMenus | ImPlotFlags_NoLegend | ImPlotFlags_NoTitle;
    utl::AxisCFG                            mst_axes [2]                    = {
          { "Time  [sec]"       , ImPlotAxisFlags_None | ImPlotAxisFlags_NoHighlight                                                            }   //  ImPlotAxisFlags_Opposite
        , { "Counts  [Arb.]"    , ImPlotAxisFlags_None | ImPlotAxisFlags_Opposite | ImPlotAxisFlags_AutoFit  | ImPlotAxisFlags_NoHighlight      }
    };
    utl::LegendCFG                          legend                          = { ImPlotLocation_NorthWest, ImPlotLegendFlags_Outside | ImPlotLegendFlags_Horizontal /* ImPlotLegendFlags_Outside; // | ImPlotLegendFlags_Horizontal; */ };
    
    
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      1. |    INDIVIDUAL PLOT STUFF.
    // *************************************************************************** //
    ImPlotAxisFlags                         m_ind_pline_flags               = ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoDecorations;
    
    
    
    
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      1. |    PLOT---STYLE VARIABLES.
    // *************************************************************************** //
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      1. |    TRANSIENT / INTERNAL STATE VARIABLES.
    // *************************************************************************** //
    
    
//
// *************************************************************************** //
// *************************************************************************** //   END "1.  DATA-MEMBERS".



// *************************************************************************** //
//
//      2.A.        MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
    // *************************************************************************** //
    //      INITIALIZATION METHODS.         |   "init.cpp" ...
    // *************************************************************************** //
    //  explicit                        CCounterStyle           (app::AppState & );             //  Def. Constructor.
                                        CCounterStyle           (void) noexcept                 = default;
                                        ~CCounterStyle          (void)                          = default;
    
    // *************************************************************************** //
    //      DELETED FUNCTIONS.              |   ...
    // *************************************************************************** //
                                        CCounterStyle           (const CCounterStyle &    src)       = delete;   //  Copy. Constructor.
                                        CCounterStyle           (CCounterStyle &&         src)       = delete;   //  Move Constructor.
    CCounterStyle &                     operator =              (const CCounterStyle &    src)       = delete;   //  Assgn. Operator.
    CCounterStyle &                     operator =              (CCounterStyle &&         src)       = delete;   //  Move-Assgn. Operator.
    
//
// *************************************************************************** //
// *************************************************************************** //   END "2A.  MEMBER FUNCS".

    
   
// *************************************************************************** //
//
//      2.B.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      2.B. |  QUERY FUNCTIONS.
    // *************************************************************************** //
    //  "_no_op"
    inline void                         _no_op                              (void)      { return; };
    
    
//
// *************************************************************************** //
// *************************************************************************** //   END "2B.  INLINE" FUNCTIONS.



//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "CCounterStyle" INLINE STRUCT DEFINITION.






//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 3.  "OTHER" ]].












// *************************************************************************** //
//
//
//
//      4.      INTERNAL ARRAYS / CONSTANTS...
// *************************************************************************** //
// *************************************************************************** //

//  "DEF_CHANNEL_INFOS"
//
static constexpr size_t         DEF_CHANNEL_COUNT                           = 15ULL;
static constexpr ChannelSpec    DEF_CHANNEL_INFOS [ DEF_CHANNEL_COUNT ]     = {
//                          MASTER---PLOT.                  SINGLE---PLOT.                      AVERAGE---PLOT.
      { 8   , "A"       , { true    , "##MasterA"           , true      , "##SingleA"           , true      , "##AvgA"          }     }
    , { 4   , "B"       , { true    , "##MasterB"           , true      , "##SingleB"           , true      , "##AvgB"          }     }
    , { 2   , "C"       , { false   , "##MasterC"           , true      , "##SingleC"           , false     , "##AvgC"          }     }
    , { 1   , "D"       , { false   , "##MasterD"           , true      , "##SingleD"           , false     , "##AvgD"          }     }
//
    , {12   , "AB"      , { false   , "##MasterAB"          , false     , "##SingleAB"          , false     , "##AvgAB"         }     }
    , {10   , "AC"      , { false   , "##MasterAC"          , false     , "##SingleAC"          , false     , "##AvgAC"         }     }
    , { 9   , "AD"      , { false   , "##MasterAD"          , false     , "##SingleAD"          , false     , "##AvgAD"         }     }
    , { 6   , "BC"      , { false   , "##MasterBC"          , false     , "##SingleBC"          , false     , "##AvgBC"         }     }
    , { 5   , "BD"      , { false   , "##MasterBD"          , false     , "##SingleBD"          , false     , "##AvgBD"         }     }
    , { 3   , "CD"      , { false   , "##MasterCD"          , false     , "##SingleCD"          , false     , "##AvgCD"         }     }
//
    , {14   , "ABC"     , { false   , "##MasterABC"         , false     , "##SingleABC"         , false     , "##AvgABC"        }     }
    , {13   , "ABD"     , { false   , "##MasterABD"         , false     , "##SingleABD"         , false     , "##AvgABD"        }     }
    , {11   , "ACD"     , { false   , "##MasterACD"         , false     , "##SingleACD"         , false     , "##AvgACD"        }     }
    , { 7   , "BCD"     , { false   , "##MasterBCD"         , false     , "##SingleBCD"         , false     , "##AvgBCD"        }     }
//
    , {15   , "ABCD"    , { false   , "##MasterABCD"        , false     , "##SingleABCD"        , false     , "##AvgABCD"       }     }
};



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 4.  "INTERNAL ARRAYS" ]].












// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb::ccounter" NAMESPACE.












#endif      //  _CBAPP_COUNTER_INTERNAL_H  //
// *************************************************************************** //
// *************************************************************************** //   END FILE.

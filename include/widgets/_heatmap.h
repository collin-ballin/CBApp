/***********************************************************************************
*
*       ********************************************************************
*       ****             _ H E A T M A P . H  ____  F I L E             ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond
*               DATED:      April 26, 2025.
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_HEATMAP_H
#define _CBAPP_HEATMAP_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include "_config.h"
#include "cblib.h"


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
#include "implot.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
// *************************************************************************** //
//                         HeatMap:
// 		        HeatMap Widget for Dear ImGui.
// *************************************************************************** //
// *************************************************************************** //

class HeatMap
{
// *************************************************************************** //
// *************************************************************************** //
public:
    //  0.               PUBLIC CLASS-NESTED ALIASES...
    // *************************************************************************** //
    using   value_type              = float;
    using   size_type               = std::size_t;
    using   dim_type                = float;
    
    //  1.               PUBLIC MEMBER FUNCTIONS...
    // *************************************************************************** //
    //  1.1             Default Constructor, Destructor, etc...
                        HeatMap                 (void)                              = default;      //  Def. Constructor.
                        HeatMap                 (const size_type,   const size_type);               //  Parametric Constructors.
                        HeatMap                 (const char *,      const size_type,    const size_type);
                        ~HeatMap                (void);                                             //  Def. Destructor.
                        
    //  1.2             Public Member Functions...
    void                Begin                   (bool * p_open = nullptr);
    void                draw_controls           (void);
    void                draw_plots              (void);
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
    
    //  Main Window Stuff...
    const char *                        m_window_title          = "HeatMap";
    ImPlotHeatmapFlags                  m_window_flags          = ImGuiWindowFlags_None; //| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
    
    //  Appearance.
    ImPlotHeatmapFlags                  m_hm_flags              = 0;
    ImPlotAxisFlags                     m_axes_flags            = ImPlotAxisFlags_None | ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoDecorations;
    ImPlotFlags                         m_plot_flags            = ImPlotFlags_None | ImPlotFlags_NoLegend;
    ImPlotColormap                      m_cmap                  = ImPlotColormap_Viridis;
    const char *                        m_id                    = nullptr;
    
    
    //  Data.
    float                               m_time                  = 0.0f;
    value_type                          m_vmin                  = value_type(0);
    value_type                          m_vmax                  = value_type(1);
    std::vector<value_type>             m_data;
        
        
    //  Interactive Variables.
    ImVec2                              m_plot_dims             = ImVec2(650.0f, 650.0f);
    size_type                           m_rows                  = size_type(0);
    size_type                           m_cols                  = size_type(0);
    value_type                          m_amp                   = 1.0f;
    value_type                          m_freq                  = 1.0f;
    
    
    
    //  2.B             PROTECTED MEMBER FUNCTIONS...
    // *************************************************************************** //
    
    //  2B.1            Class Initializations.      [HeatMap.cpp]...
    void                init                    (void);
    void                destroy                 (void);
    
    //  2B.2            Internal Functions.
    // void                test_plots              (void);
    // void                test_controls           (void);



    
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
};//	END "HeatMap" CLASS PROTOTYPE.







// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.






#endif      //  _CBAPP_HEATMAP_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.


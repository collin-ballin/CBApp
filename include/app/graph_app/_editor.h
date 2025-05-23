/***********************************************************************************
*
*       ********************************************************************
*       ****              _ E D I T O R . H  ____  F I L E              ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      May 23, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_GRAPH_APP_EDITOR_H
#define _CBAPP_GRAPH_APP_EDITOR_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include "_config.h"
#include "cblib.h"
//#include "app/app.h"
#include "app/_init.h"
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
#include "implot_internal.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
// *************************************************************************** //
//                         SketchWidget:
// 		        SketchWidget Widget for Dear ImGui.
// *************************************************************************** //
// *************************************************************************** //

struct Channel {
    std::vector<float> data;   // heat‑map values
    int   cmap   = 0;          // ImPlot colormap index
    float vmin   = 0.0f;
    float vmax   = 1.0f;
    const char*  name  = "heat"; // label for PlotHeatmap/scale
};
        
enum Mode {
    Draw        = 0,
    Navigate    = 1
};
    
    
//  "SketchWidget"
//
class SketchWidget {
public:
    explicit SketchWidget(utl::PlotCFG & cfg);
    

    // Plot lifecycle ------------------------------------------------------
    bool Begin(int nx, int ny);
    void End();

    // Built‑in control block (optional)
    void ShowControls(float scale_width = 80.0f);

    // ---------------------------------------------------------------------
    //  Public variable accessors (return pointers so ImGui widgets can bind
    //  directly). Variables set via Begin(), constants, or internal caches
    //  are NOT exposed.
    // ---------------------------------------------------------------------
    inline int *       get_brush_size          (void)          { return &m_brush_size; }
    inline int *       get_brush_shape         (void)          { return &m_brush_shape; }
    inline float *     get_paint_value         (void)          { return &m_paint_value; }
    inline float *     get_vmin                (void)          { return &m_vmin; }
    inline float *     get_vmax                (void)          { return &m_vmax; }
    inline int *       get_cmap                (void)          { return &m_cmap; }

    //  Access to raw data buffer (read‑only)
    const std::vector<float>& get_data() const { return m_data; }

private:
    // helpers -------------------------------------------------------------
    void                    resize_buffers      (int nx, int ny);
    void                    clear               (void);
    void                    stamp               (int gx, int gy);
    void                    draw_line           (int x0, int y0, int x1, int y1);
    inline void             reset_prev          (void)          { m_prev_x = m_prev_y = -1; }
    inline Channel &        current(void)                       { return m_channels[m_active]; }



    // state ---------------------------------------------------------------
    std::vector<float>              m_data;
    
    
    int                             m_res_x                 = 0,
                                    m_res_y                 = 0;
    
    int                             m_brush_size            = 3;
    int                             m_brush_shape           = 0;
    float                           m_paint_value           = 1.0f;
    
    
    float                           m_vmin                  = 0.0f,
                                    m_vmax                  = 1.0f;
    
    int                             m_cmap                  = 0;

    bool                            m_drawing               = false;
    int                             m_prev_x                = -1,
                                    m_prev_y                = -1;



    Mode                            m_mode                  = Mode::Draw;
    std::vector<Channel>            m_channels{1};                              // single channel for now
    int                             m_active                = 0;                // active channel index
    //
    ImGuiID                         m_heatmap_id;
    utl::PlotCFG &                  m_cfg;

// *************************************************************************** //
// *************************************************************************** //
};//	END "SketchWidget" CLASS PROTOTYPE.







// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.






#endif      //  _CBAPP_GRAPH_APP_EDITOR_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.


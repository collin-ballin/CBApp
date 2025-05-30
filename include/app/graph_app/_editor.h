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
#include CBAPP_USER_CONFIG
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



namespace editor { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //

struct Channel {
    std::vector<float>      data;   // heat‑map values
    int                     cmap            = 0;            // ImPlot colormap index
    
    utl::Param<float>       paint_value     = { 0.5f, {0.0f, 1.0f}};
//
    const char *            map_title       = "Map";        // label for PlotHeatmap/scale
    const char *            map_units       = nullptr; 
//
    const char *            scale_title     = "Scale";
    const char *            scale_units     = "%.1f Arb.";
    const double            scale_width     = 100.0f;
};
        
enum class State : int {
    Default         = 0,
    Draw,
    Build,
    Count
};

enum class BrushShape : int {
    Square          = 0,
    Circle,
    Count
};



//  THIRD STATE THINGS...
//
struct DragPoint {
    int             id    = -1;              // stable identifier for ImPlot::DragPoint
    ImPlotPoint     pos   = {0.0, 0.0};      // plot‑space position
    bool            clicked = false,
                    hovered = false,
                    held    = false;          // supplied by ImPlot::DragPoint()
};
    
    
    
// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "editor" NAMESPACE.


    



// *************************************************************************** //
// *************************************************************************** //
//                         SketchWidget:
// 		        SketchWidget Widget for Dear ImGui.
// *************************************************************************** //
// *************************************************************************** //

//  "SketchWidget"
//
class SketchWidget {
public:
    using   Channel         = editor::Channel;
    using   State           = editor::State;
    using   BrushShape      = editor::BrushShape;
    using   CBDragPoint     = editor::DragPoint;
//
//  STATIC, CONSTEXPR CLASS VARIABLES...
    static constexpr std::array<const char *, static_cast<size_t>( State::Count)>
                                STATE_LABELS            = { "Default",  "Draw",  "Build" };
    static constexpr std::array<const char *, static_cast<size_t>(BrushShape::Count)>
                                BRUSH_SHAPE_LABELS      = { "Square",   "Circle" };
                        
    static constexpr float      k_scale_width           = 60.0f; // px reserved for color scale
    static constexpr int        k_max_points            = 64;



//  Initialization Methods.
    explicit            SketchWidget            (utl::PlotCFG & cfg);
    explicit            SketchWidget            (utl::PlotCFG& cfg, std::vector<Channel> channels);
    

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
    inline void             set_mode                (const int mode)    { this->m_mode = static_cast<State>(mode);                          }
    void                    set_cmap                (const int cmap);
    inline void             set_brush_shape         (const int brush)   { this->m_brush_shape = static_cast<BrushShape>(brush);             }
    //
    //
    int                     channel_count           (void)  const       { return static_cast<int>(m_channels.size());                       }
    inline void             set_active_channel      (int idx)           { m_active = std::clamp(idx, 0, channel_count()-1);                 }
    inline int *            get_active_channel      (void)              { return &m_active;                                                 }   // for ImGui combo-binding
    inline const Channel &  channel                 (int idx) const     { return m_channels.at(idx);                                        }
    //
    State *                 get_mode                (void)              { return std::addressof( m_mode );                                  }          // points into enum underlying int
    BrushShape *            get_brush_shape         (void)              { return std::addressof( m_brush_shape );                           }
    //
    inline int *            get_brush_size          (void)              { return std::addressof( m_brush_size );                            }
    inline float *          get_paint_value         (void)              { return std::addressof( current().paint_value.value );             }
    inline float*           get_vmin                (void)              { return std::addressof( current().paint_value.limits.min );        }
    inline float*           get_vmax                (void)              { return std::addressof( current().paint_value.limits.max );        }
    inline int*             get_cmap                (void)              { return std::addressof( current().cmap );                          }

    //  Access to raw data buffer (read‑only)
    const std::vector<float>& get_data(void) const                      { return m_data; }

private:
    // mode dispatch helpers ----------------------------------------------------
    inline void             default_mode_input      (void)              { return; }   // placeholder
    void                    draw_mode_input         (void);
    void                    build_mode_input        (void);
    
    // helpers -------------------------------------------------------------
    void                    resize_buffers          (int nx, int ny);
    void                    clear                   (void);
    void                    stamp                   (int gx, int gy);
    void                    draw_line               (int x0, int y0, int x1, int y1);
    inline void             reset_prev              (void)              {    m_prev_x = m_prev_y = -1; }
    
    
    inline Channel &        current                 (void)              { return m_channels[m_active];                          }



    // state ---------------------------------------------------------------
    std::vector<float>              m_data;
    
    
    int                             m_res_x                 = 0,
                                    m_res_y                 = 0;
    
    int                             m_brush_size            = 1;
    //  float                           m_vmin                  = 0.0f,
    //                                  m_vmax                  = 1.0f;
    
    //int                             m_cmap                  = 0;

    bool                            m_drawing               = false;
    int                             m_prev_x                = -1,
                                    m_prev_y                = -1;



    State                           m_mode                  = State::Default;               // underlying int of Mode
    BrushShape                      m_brush_shape           = BrushShape::Square;
    std::vector<Channel>            m_channels{1};                                          // single channel for now
    int                             m_active                = 0;                            // active channel index
    
    
    // Build‑mode drag points --------------------------------------------------
    std::vector<CBDragPoint>        m_points;   // automatically sized, <= k_max_points
    
    
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


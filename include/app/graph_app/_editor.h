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



namespace editor { //     BEGINNING NAMESPACE "cb" :: "editor"...
// *************************************************************************** //
// *************************************************************************** //



//      "DRAW" STATE STUFF...
// *************************************************************************** //
// *************************************************************************** //

struct      Channel {
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
        
enum class  State : int {
    Default     = 0,
    Draw,
    Build,
    Count
};

enum class  BrushShape : int {
    Square      = 0,
    Circle,
    Count
};



//      "BUILD"-STATE DEFINITIONS...
// *************************************************************************** //
// *************************************************************************** //

enum class MenuScope { Single, Multi };

enum class PointType    : uint8_t {
    Unassigned  = 0,
    TypeA,
    TypeB,
    TypeC,
    Count
};

enum class PopupAction  : uint8_t {
    None        = 0,
    Delete,        // remove (all) selected point(s)
    SetType,        // set point type for the selection
    Count
};



//  "DragPoint"
//
struct      DragPoint {
    int             id          = -1;              // stable identifier for ImPlot::DragPoint
    ImPlotPoint     pos         = {0.0, 0.0};      // plot‑space position
    bool            clicked     = false,
                    hovered     = false,
                    held        = false;          // supplied by ImPlot::DragPoint()
    PointType       type        = PointType::Unassigned;
};

//  "CBLine"
//
struct CBLine {
    int             id          = -1;      // unique line id
    int             a           = -1;      // index of first point in m_points
    int             b           = -1;      // index of second point
    PointType       type        = PointType::Unassigned;
};



//  "BuildCtx"
//
struct BuildCtx {
    bool         left_click;
    bool         right_click;
    bool         shift;
    bool         ctrl;
    bool         plot_hovered;
    ImPlotPoint  mouse_plot;   // mouse in plot coords
};

    

//  FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "make_build_ctx"
//
inline BuildCtx make_build_ctx(void) {
    const ImGuiIO& io = ImGui::GetIO();
    return {
        .left_click    = ImGui::IsMouseClicked(ImGuiMouseButton_Left),
        .right_click   = ImGui::IsMouseClicked(ImGuiMouseButton_Right),
        .shift         = io.KeyShift,
        .ctrl          = io.KeyCtrl,
        .plot_hovered  = ImPlot::IsPlotHovered(),
        .mouse_plot    = ImPlot::GetPlotMousePos()
    };
}
    
    
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
    using                           Channel                 = editor::Channel;
    using                           State                   = editor::State;
    using                           BrushShape              = editor::BrushShape;
    using                           CBDragPoint             = editor::DragPoint;
    using                           CBLine                  = editor::CBLine;
    using                           PopupAction             = editor::PopupAction;
    using                           PointType               = editor::PointType;
    using                           BuildCtx                = editor::BuildCtx;
    using                           MenuScope               = editor::MenuScope;
//
//  STATIC, CONSTEXPR CLASS VARIABLES...
    static constexpr std::array<const char *, static_cast<size_t>( State::Count)>
                                    STATE_LABELS            = { "Default",      "Draw",     "Build" };
    static constexpr std::array<const char *, static_cast<size_t>(BrushShape::Count)>
                                    BRUSH_SHAPE_LABELS      = { "Square",       "Circle" };
    static constexpr std::array<const char *, static_cast<size_t>( PointType::Count )>
                                    POINT_TYPE_LABELS       = { "Unassigned", "Type A", "Type B", "Type C" };
                        
    static constexpr float          k_scale_width           = 60.0f; // px reserved for color scale
    static constexpr int            k_max_points            = 64;



//  Initialization Methods.
    explicit                        SketchWidget                    (utl::PlotCFG & cfg);
    explicit                        SketchWidget                    (utl::PlotCFG & cfg, std::vector<Channel> channels);
    

    // Plot lifecycle ------------------------------------------------------
    bool                            Begin                           (int nx, int ny);
    void                            End                             (void);

    // Built‑in control block (optional)
    void                            ShowControls                    (float scale_width = 80.0f);

    // ---------------------------------------------------------------------
    //  Public variable accessors (return pointers so ImGui widgets can bind
    //  directly). Variables set via Begin(), constants, or internal caches
    //  are NOT exposed.
    // ---------------------------------------------------------------------
    void                            set_mode                        (const int mode);
    void                            set_cmap                        (const int cmap);
    inline void                     set_brush_shape                 (const int brush)   { this->m_brush_shape = static_cast<BrushShape>(brush);             }
    //
    //
    int                             channel_count                   (void)  const       { return static_cast<int>(m_channels.size());                       }
    inline void                     set_active_channel              (int idx)           { m_active = std::clamp(idx, 0, channel_count()-1);                 }
    inline int *                    get_active_channel              (void)              { return &m_active;                                                 }   // for ImGui combo-binding
    inline const Channel &          channel                         (int idx) const     { return m_channels.at(idx);                                        }
    //
    State *                         get_mode                        (void)              { return std::addressof( m_mode );                                  }          // points into enum underlying int
    BrushShape *                    get_brush_shape                 (void)              { return std::addressof( m_brush_shape );                           }
    //
    inline int *                    get_brush_size                  (void)              { return std::addressof( m_brush_size );                            }
    inline float *                  get_paint_value                 (void)              { return std::addressof( current().paint_value.value );             }
    inline float*                   get_vmin                        (void)              { return std::addressof( current().paint_value.limits.min );        }
    inline float*                   get_vmax                        (void)              { return std::addressof( current().paint_value.limits.max );        }
    inline int*                     get_cmap                        (void)              { return std::addressof( current().cmap );                          }

    //  Access to raw data buffer (read‑only)
    const std::vector<float> &      get_data(void) const                                { return m_data; }

private:
    // mode dispatch helpers ----------------------------------------------------
    inline void                     default_mode_input              (void)              { return; }   // placeholder
    void                            draw_mode_input                 (void);
    //
    //
    //
    //      BUILD MODE FUNCTIONS -------------------------------------------------------------
    void                            build_mode_input                (void);
    void                            delete_selected_points          (void);
    PopupAction                     show_point_popup                (CBDragPoint & pt);
    PopupAction                     show_multi_popup                (PointType & out_type);
    PopupAction                     show_selection_popup            (void);
    //
    //
    //                  NEW BUILD HELPERS...
    void                            build_add_new_point             (const BuildCtx&);
    void                            build_update_points             (const BuildCtx&, PopupAction&, PointType&);
    void                            build_handle_selection_popup    (const BuildCtx&, PopupAction&, PointType&);
    void                            build_apply_global_action       (PopupAction, PointType);
    void                            build_update_lasso              (const BuildCtx&);
    void                            build_nudge_selected            (const BuildCtx&);
    bool                            m_internal_click_on_pt          = false;
    //
    //                  LINE STUFF...
    void                            connect_points                  (void);
    void                            propagate_type                  (int idx, PointType new_type);
    //
    bool                            line_exists                     (int i, int j) const;          // order‑independent lookup
    void                            add_line                        (int i, int j);
    void                            remove_lines_with_point         (int idx);
    void                            render_lines                    (void);
    //
    //
    //
    //                  Build Mode Helpers.
    void                            begin_lasso                     (const ImPlotPoint& start, bool additive);
    void                            update_lasso_end                (const ImPlotPoint& curr);
    void                            finalize_lasso                  (void);
    void                            draw_lasso                      (void) const;
    bool                            m_lasso_active                  = false;
    bool                            m_lasso_additive                = false;
    ImPlotPoint                     m_lasso_start                   = ImPlotPoint();
    ImPlotPoint                     m_lasso_end                     = ImPlotPoint();
    //
    bool                            is_selected                     (int idx) const;
    void                            add_selection                   (int idx, bool additive);
    void                            clear_selection                 (void);
    // ---------------------------------------------------------------------------------------
    
    
    
    // helpers -------------------------------------------------------------
    void                            resize_buffers          (int nx, int ny);
    void                            clear                   (void);
    void                            stamp                   (int gx, int gy);
    void                            draw_line               (int x0, int y0, int x1, int y1);
    inline void                     reset_prev              (void)              {    m_prev_x = m_prev_y = -1; }
    
    
    inline Channel &                current                 (void)              { return m_channels[m_active]; }



    // state ---------------------------------------------------------------
    std::vector<float>              m_data;
    int                             m_selected_idx          = -1; // -1 = none
    
    
    int                             m_res_x                 = 0,
                                    m_res_y                 = 0;
    int                             m_brush_size            = 1;

    bool                            m_drawing               = false;
    int                             m_prev_x                = -1,
                                    m_prev_y                = -1;



    State                           m_mode                  = State::Default;               // underlying int of Mode
    BrushShape                      m_brush_shape           = BrushShape::Square;
    std::vector<Channel>            m_channels{1};                                          // single channel for now
    int                             m_active                = 0;                            // active channel index
    
    
    // Build‑mode drag points --------------------------------------------------
    std::vector<CBDragPoint>        m_points;       // automatically sized, <= k_max_points
    std::vector<int>                m_selected;     // indices of selected points
    std::vector<CBLine>             m_lines;
    PointType                       m_bulk_type             = PointType::Unassigned;
    
    
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


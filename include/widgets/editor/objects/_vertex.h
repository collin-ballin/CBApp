/***********************************************************************************
*
*       ********************************************************************
*       ****              _ V E R T E X . H  ____  F I L E              ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      September 7, 2025.
*              MODULE:      CBAPP > EDITOR > OBJECTS/       | _vertex.h
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_EDITOR_OBJECTS_VERTEX_H
#define _CBAPP_EDITOR_OBJECTS_VERTEX_H  1
#include CBAPP_USER_CONFIG



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include "cblib.h"
#include "utility/utility.h"
#include "widgets/editor/_constants.h"
#include "widgets/editor/objects/_internal.h"

//  0.2     STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <cstdlib>          // C-Headers...
#include <stdio.h>
#include <unistd.h>

#include <algorithm>        //  SPECIAL...
#include <optional>
#include <variant>
#include <tuple>

#include <string>           //  DATA STRUCTURES...      //  <======| std::string, ...
#include <string_view>
#include <vector>           //                          //  <======| std::vector, ...
#include <array>
#include <unordered_set>

#include <stdexcept>        //                          //  <======| ...
#include <limits.h>
#include <math.h>
#include <complex>
#include <utility>


//  0.3     "DEAR IMGUI" HEADERS...
#include "json.hpp"
//
#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_internal.h"
#include "implot.h"
#include "implot_internal.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//      0.      MISC ENUMS / UTILITIES...
// *************************************************************************** //
// *************************************************************************** //

//  "EndpointInfo"
//  template<typename PID>
//  struct EndpointInfo_t { PID path_idx; bool prepend; };   // prepend==true ↔ first vertex



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "MISC/UTILITY".












// *************************************************************************** //
//
//
//
//      1.      VERTEX STYLE AND BEZIER CONTROL POINT...
// *************************************************************************** //
// *************************************************************************** //


//  "VertexHoverState"
enum class VertexHoverState : uint8_t               { None = 0, Vertex, InHandle, OutHandle, COUNT };


//  "VertexStyleData"
//
struct VertexStyleData
{
    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    //  CBAPP_APPSTATE_ALIAS_API            //  *OR*    CBAPP_CBLIB_TYPES_API       //  FOR CBLIB...
    using                           HoverState                  = VertexHoverState;
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    static constexpr ImDrawFlags    ms_HANDLE_FLAGS             = ImDrawFlags_RoundCornersNone; //ImDrawFlags_RoundCornersAll;
    
//
// *************************************************************************** //
// *************************************************************************** //   END "CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//      1.          DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      STYLE VARIABLES.
    // *************************************************************************** //
    //                          Vertex Appearance:
    float                           vertex_radius               = 6.5f;
    ImU32                           vertex_color                = IM_COL32(0,       255,    0,      255);
    int                             vertex_tesselation          = 12;
    //
    //                          Bézier Handle "IN":
    ImU32                           handle_color                = IM_COL32(255,     215,    0,      255);
    float                           handle_size                 = 5.5f;
    float                           handle_rounding             = 0.0f;
    float                           handle_thickness            = 2.00f;
    //
    //                          Line-Style:
    ImU32                           line_color                  = IM_COL32(255,     215,    0,      170);
    float                           line_width                  = 1.0f;
    //
    //                          Additional Stuff:
    ImU32                           hovered_color               = IM_COL32(0,       0,    255,      255);
    float                           hovered_size                = 11.0f;
    
    // *************************************************************************** //
    
    

    // *************************************************************************** //
    //      STATE VARIABLES.
    // *************************************************************************** //
    mutable HoverState              hovered                     = HoverState::None;
    
    // *************************************************************************** //
    
//
// *************************************************************************** //
// *************************************************************************** //   END "DATA-MEMBERS".
    
//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "VertexStyleData" INLINE STRUCT DEFINITION.



//  "VertexStyleType"
//      Enum to define a HANDLE to access each PRESET STYLE of Vertex that is used by the Editor Application.
//
enum class VertexStyleType : uint8_t {
      Default = 0
    , Highlight
    , AltHighlight
    , Drag
    , COUNT
};
//
//
static constexpr cblib::EnumArray< VertexStyleType, VertexStyleData >
DEF_VERTEX_STYLES{
{
    VertexStyleData     //  0.  DEFAULT:
    {
        /*  vertex_radius       */ 6.00f,
        /*  vertex_color        */ IM_COL32(116,     140,    177,       255),
        /*  vertex_tesselation  */ 12,
        /*  handle_color        */ IM_COL32(215,     255,    0,         255),
        /*  handle_size         */ 5.50f,
        /*  handle_rounding     */ 0.00f,
        /*  handle_thickness    */ 2.00f,
        /*  line_color          */ IM_COL32(215,     255,    0,         170),
        /*  line_width          */ 1.50f,
        /*  hovered_color       */ IM_COL32(0,       0,    255,         255),
        /*  hovered_size        */ 10.0f
    },
//
    VertexStyleData     //  1.  HIGHLIGHT:
    {
        /*  vertex_radius       */ 6.50f,
        /*  vertex_color        */ IM_COL32(0,       255,    0,         240),
    //
        /*  vertex_tesselation  */ 15,
        /*  handle_color        */ IM_COL32(255, 215, 0, 255),
        /*  handle_size         */ 6.75f,
        /*  handle_rounding     */ 0.00f,
        /*  handle_thickness    */ 2.50f,
    //
        /*  line_color          */ IM_COL32(255, 215, 0, 255), //   cblib::utl::compute_shade( IM_COL32(255, 215, 0, 170), 0.15f ),
        /*  line_width          */ 2.00f,
        /*  hovered_color       */ IM_COL32(0,       255,       0,      255),
        /*  hovered_size        */ 11.0f
    },
//
    VertexStyleData     //  2.  ALTHIGHLIGHT (ALTERNATIVE HIGHLIGHT):
    {
        /*  vertex_radius       */ 8.00f,
        /*  vertex_color        */ IM_COL32(10,      255,    250,       240),
    //
        /*  vertex_tesselation  */ 15,
        /*  handle_color        */ IM_COL32(0, 0, 255, 255), //   cblib::utl::compute_tint( IM_COL32(255, 215, 0, 255), 0.30f ),
        /*  handle_size         */ 6.75f,
        /*  handle_rounding     */ 0.00f,
        /*  handle_thickness    */ 3.25f,
    //
        /*  line_color          */ IM_COL32(255, 215, 0, 255), //   cblib::utl::compute_shade( IM_COL32(255, 215, 0, 170), 0.15f ),
        /*  line_width          */ 2.50f,
        /*  hovered_color       */ IM_COL32(0,       255,       0,      255),
        /*  hovered_size        */ 12.0f
    },
//
    VertexStyleData     //  3.  DRAG:
    {
        /*  vertex_radius       */ 6.50f,
        /*  vertex_color        */ IM_COL32(255,       0,    0,         240),
        /*  vertex_tesselation  */ 12,
        /*  handle_color        */ IM_COL32(215,     255,    0,         255),
        /*  handle_size         */ 5.50f,
        /*  handle_rounding     */ 0.00f,
        /*  handle_thickness    */ 2.00f,
        /*  line_color          */ IM_COL32(215,     255,    0,         170),
        /*  line_width          */ 1.50f,
        /*  hovered_color       */ IM_COL32(0,       0,    255,         255),
        /*  hovered_size        */ 11.0f
    }
//
} };












//  "VertexStyle"
//
template <typename MappingFn>
    requires std::is_nothrow_invocable_r_v<ImVec2, MappingFn, ImVec2>
struct VertexStyle
{
    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    //  CBAPP_APPSTATE_ALIAS_API            //  *OR*    CBAPP_CBLIB_TYPES_API       //  FOR CBLIB...
    using                               StyleType                       = VertexStyleType;
    using                               StyleData                       = VertexStyleData;
    using                               HoverState                      = VertexStyleData::HoverState;
    
    // *************************************************************************** //
    //      STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    
//
// *************************************************************************** //
// *************************************************************************** //   END "CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//      1.          DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //
    MappingFn                           ws_to_px;
    mutable StyleData const *           data;
    mutable ImDrawList *                dl                              = nullptr;
    
//
// *************************************************************************** //
// *************************************************************************** //   END "DATA-MEMBERS".


    
   
// *************************************************************************** //
//
//      2.B.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    inline                              VertexStyle                         (MappingFn callback, const StyleData & data_) noexcept
                                        : ws_to_px(callback)    , data( std::addressof(data_) )   {   }
    
    //  "PushDL"
    inline void                         PushDL                              (ImDrawList * & dl_) const noexcept     { this->dl = dl_;           };
    inline void                         PopDL                               (void) const noexcept                   { this->dl = nullptr;       };
    
    
    //  "PushHoverState"
    inline void                         PushHoverState                      (const HoverState hover) const noexcept {
        IM_ASSERT( data != nullptr && "Call to \"VertexStyle.PushHoverState()\" with NULL data");
        (*this->data).hovered = hover;
        return;
    };
    inline void                         PopHoverState                       (void) const noexcept       = delete;   //  We automatically POP the hover state at the end of "Vertex_t::render()"...
    inline void                         _PopHoverState                      (void) const noexcept       { (*this->data).hovered = HoverState::None; }
    
//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.



//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "VertexStyle" INLINE STRUCT DEFINITION.



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "VertexStyle".












// *************************************************************************** //
//
//
//
//      2.      [DATA LAYER]    BEZIER CONTROL-POINT...
// *************************************************************************** //
// *************************************************************************** //

//  "BezierCurvatureType"
enum class BezierCurvatureType : uint8_t                { None = 0,     Quadratic,      Cubic,      Other,      COUNT   };
//
static constexpr cblib::EnumArray <BezierCurvatureType, const char *>
DEF_BEZIER_CURVATURE_TYPE_NAMES                         = { "None",     "Quadratic",    "Cubic" ,   "Other"             };



//  "BezierCurvatureState"
enum class BezierCurvatureState : uint8_t           { None = 0, In, Out, All, COUNT };



//  "BezierControl"
//      Defines a "control point" for the Vertex/Path curvature.
//
struct BezierControl
{
    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    using                               CurvatureType                           = BezierCurvatureType;
    using                               CurvatureState                          = BezierCurvatureState;
    using                               StyleData                               = VertexStyleData;
    using                               HoverState                              = StyleData::HoverState;
    
    // *************************************************************************** //
    //      STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    static constexpr float              ms_BEZIER_NUMERICAL_ERROR               = 1e-3;
    static constexpr const char *       ms_SLIDER_FMT                           = "%.3f";
    static constexpr ImGuiSliderFlags   ms_SLIDER_FLAGS                         = ImGuiSliderFlags_AlwaysClamp; //    ImGuiSliderFlags_None;
    
//
// *************************************************************************** //
// *************************************************************************** //   END "CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//      1.          DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      BéZIER HANDLE DATA.
    // *************************************************************************** //
    BezierCurvatureType                 kind                            = CurvatureType::None;
    ImVec2                              in_handle                       = ImVec2(0.0f, 0.0f);   // incoming Bézier handle (from previous vertex)
    ImVec2                              out_handle                      = ImVec2(0.0f, 0.0f);   // outgoing Bézier handle (to next vertex)

    // *************************************************************************** //
    //      TRANSIENT STATE VARIABLES.
    // *************************************************************************** //
    mutable CurvatureState              m_curvature_state               = CurvatureState::None;
    
//
// *************************************************************************** //
// *************************************************************************** //   END "DATA-MEMBERS".

    
   
// *************************************************************************** //
//
//      2.B.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
    // *************************************************************************** //
    //      DELETED FUNCTIONS.              |   ...
    // *************************************************************************** //
    
    inline void                         _set_in_handle                  (const ImVec2 & ) && noexcept   = delete;
    inline void                         _set_out_handle                 (const ImVec2 & ) && noexcept   = delete;


    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      STATIC INLINE FUNCTIONS.
    // *************************************************************************** //
    
    //  "_reset_curvature"
    //  inline void                         _reset_curvature                (void) noexcept       {
    //      this->m_curvature_state     = CurvatureState::None;
    //      this->in_handle             = {  };
    //      this->out_handle            = {  };
    //      return;
    //  }


    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      QUERY FUNCTIONS.
    // *************************************************************************** //
    
    //  "_is_linear"
    [[nodiscard]] inline bool           _is_linear                      (void) const noexcept           { return ( m_curvature_state == CurvatureState::None    );  }
    //  [[nodiscard]] inline bool           _is_in_linear                   (void) const noexcept           { return ( m_curvature_state != CurvatureState::In      );  }
    //  [[nodiscard]] inline bool           _is_out_linear                  (void) const noexcept           { return ( m_curvature_state != CurvatureState::Out     );  }
    [[nodiscard]] inline bool           _is_in_linear                   (void) const noexcept           { return ( m_curvature_state != CurvatureState::In  &&  m_curvature_state != CurvatureState::All  );    }
    [[nodiscard]] inline bool           _is_out_linear                  (void) const noexcept           { return ( m_curvature_state != CurvatureState::Out  &&  m_curvature_state != CurvatureState::All );    }
    
    //  "_is_quadratic"
    [[nodiscard]] inline bool           _is_quadratic                   (void) const noexcept           { return ( kind == CurvatureType::Quadratic );  }
    [[nodiscard]] inline bool           _is_cubic                       (void) const noexcept           { return ( kind != CurvatureType::Quadratic );  }


    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      SETTER / GETTER FUNCTIONS.
    // *************************************************************************** //
    
    //  "_set_curvature_type"
    inline void                         _set_curvature_type             (CurvatureType kind_) noexcept
    {
        if (kind_ == this->kind)    { return; }
        
        this->kind = kind_;
        this->_update_curvature_type();
        return;
    }
    
    
    //  "_set_in_handle"
    inline void                         _set_in_handle                  (const float  ih_x_, const float  ih_y_) noexcept       {  this->_set_in_handle( ImVec2{ih_x_, ih_y_} );  }
    inline void                         _set_in_handle                  (const double ih_x_, const double ih_y_) noexcept
        {  this->_set_in_handle( ImVec2{static_cast<float>(ih_x_), static_cast<float>(ih_y_)} );  }
    //
    inline void                         _set_in_handle                  (const ImVec2 & ih_) & noexcept
    {
        IM_ASSERT( this->kind != CurvatureType::Quadratic  &&  "Quadratic Bézier uses the \"OUT\" handle as the ONLY parameter" );
        switch (this->kind)
        {
            //      DEFAULT:    None.
            default : {
                this->in_handle = {
                    cblib::math::quantize( ih_.x,   this->ms_BEZIER_NUMERICAL_ERROR ),
                    cblib::math::quantize( ih_.y,   this->ms_BEZIER_NUMERICAL_ERROR )
                };
                break;
            }
        }
        this->_update_curvature_state();
        return;
    }
    
    
    
    //  "_set_out_handle"
    inline void                         _set_out_handle                 (const float  oh_x_, const float  oh_y_) noexcept       {  this->_set_out_handle( ImVec2{oh_x_, oh_y_} );  }
    inline void                         _set_out_handle                 (const double oh_x_, const double oh_y_) noexcept
        {  this->_set_out_handle( ImVec2{static_cast<float>(oh_x_), static_cast<float>(oh_y_)} );  }
    //
    inline void                         _set_out_handle                 (const ImVec2 & oh_) & noexcept
    {
        switch (this->kind)
        {
            //      DEFAULT:    None.
            default : {
                this->out_handle = {
                    cblib::math::quantize( oh_.x,   this->ms_BEZIER_NUMERICAL_ERROR ),
                    cblib::math::quantize( oh_.y,   this->ms_BEZIER_NUMERICAL_ERROR )
                };
                break;
            }
        }
        this->_update_curvature_state();
        return;
    }
    
    
    
    //  "_get_curvature_type"
    [[nodiscard]] inline CurvatureType  _get_curvature_type             (void) const noexcept           { return this->kind;            }
    
    //  "_get_in_handle"
    [[nodiscard]] inline ImVec2         _get_in_handle                  (void) const noexcept           { return this->in_handle;       }
    [[nodiscard]] inline ImVec2 &       _get_in_handle                  (void) noexcept                 { return this->in_handle;       }
    //
    //  "_get_out_handle"
    [[nodiscard]] inline ImVec2         _get_out_handle                 (void) const noexcept           { return this->out_handle;      }
    [[nodiscard]] inline ImVec2 &       _get_out_handle                 (void) noexcept                 { return this->out_handle;      }


    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    
    //  "_update_curvature_type"
    inline void                         _update_curvature_type          (void) const noexcept
    {
        this->_update_curvature_state();
        return;
    }
    
    
    //  "_update_curvature_state"
    inline CurvatureState               _update_curvature_state         (void) const noexcept
    {
        namespace           math            = cblib::math;
        const float &       GATE            = this->ms_BEZIER_NUMERICAL_ERROR;
        //
        const bool          in_is_set       = ( !math::is_close( this->in_handle.x,  0.0f, GATE)  ||  !math::is_close( this->in_handle.y,  0.0f, GATE)  );
        const bool          out_is_set      = ( !math::is_close( this->out_handle.x, 0.0f, GATE)  ||  !math::is_close( this->out_handle.y, 0.0f, GATE)  );
        const bool          both            = ( in_is_set  &&  out_is_set );
    
        switch (this->kind)
        {
            //  CASE 1 :    QUADRATIC BÉZIER CURVE...
            case CurvatureType::Quadratic :
            {
                if      ( out_is_set )      { this->m_curvature_state = CurvatureState::All;    }
                break;
            }
            //
            //  CASE 2 :    TREAT ALL OTHER CASES AS CUBIC BÉZIER...
            default :
            {
                if      ( both )            { this->m_curvature_state = CurvatureState::All;    }
                else {
                    if      ( in_is_set  )      { this->m_curvature_state = CurvatureState::In;     }
                    else if ( out_is_set )      { this->m_curvature_state = CurvatureState::Out;    }
                    else                        { this->m_curvature_state = CurvatureState::None;   }
                }
                break;
            }
        }
    
        return this->m_curvature_state;
    }
    
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      UTILITY FUNCTIONS.
    // *************************************************************************** //
    
    //  "_reset_curvature"
    inline void                         _reset_curvature                (void) noexcept       {
        this->m_curvature_state     = CurvatureState::None;
        this->in_handle             = {  };
        this->out_handle            = {  };
        return;
    }


    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      MAIN RENDERING FUNCTIONS.
    // *************************************************************************** //
    
    //  "render"
    template <typename VStyle>
    inline void                         render                          (const ImVec2 & origin, const VStyle & style) const noexcept
    {
        const ImVec2                pos             = style.ws_to_px(    {  origin.x                        , origin.y                          }   );
        const ImVec2                h_in            = style.ws_to_px(    {  origin.x + this->in_handle.x    , origin.y + this->in_handle.y      }   );
        const ImVec2                h_out           = style.ws_to_px(    {  origin.x + this->out_handle.x   , origin.y + this->out_handle.y     }   );
        
        this->_update_curvature_state();
        
        
        switch (this->kind)
        {
            //      1.      QUADRATIC BEZIER...
            case CurvatureType::Quadratic :
            {
                this->_render_QUADRATIC_IMPL                (pos, h_out, style);    //  USE "OUT" HANDLE AS THE CONTROL POINT FOR QUADRATIC BÉZIER...
                break;
            }
            //
            //
            //      X.      DEFAULT :   CUBIC BEZIER...
            default :
            {
                switch (style.data->hovered)
                {
                    //      CASE 1 :    EITHER "IN" / "OUT" HANDLE IS HOVERED.
                    case HoverState::InHandle  :
                    case HoverState::OutHandle :
                    {
                        this->_render_CUBIC_hovered_IMPL    (origin, h_in, h_out, style);
                        break;
                    }
                    //
                    //      CASE 2 :    NONE OF THE THREE ITEMS ARE HOVERED...
                    default :
                    {
                        this->_render_CUBIC_IMPL            (pos, h_in, h_out, style);
                        break;
                    }
                }
                break;
            }
        }
        
        return;
    }


    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      QUADRATIC RENDERING FUNCTIONS.
    // *************************************************************************** //
    
    //  "_render_QUADRATIC_IMPL"
    template <typename VStyle>
    inline void                         _render_QUADRATIC_IMPL              (const ImVec2 & pos, const ImVec2 & ctrl, const VStyle & style) const noexcept
    {
        const StyleData &           data            = *style.data;
        
        
        //      DISPATCH ACTION BASED ON BEZIER CURVATURE...
        switch (this->m_curvature_state)
        {
            default :
            {
                //  Draw guide line (anchor → control)
                style.dl->AddLine(pos, ctrl, data.line_color, data.line_width);

                //  Draw the unified handle box at the control point
                style.dl->AddRect(
                    ImVec2{ ctrl.x - data.handle_size, ctrl.y - data.handle_size },
                    ImVec2{ ctrl.x + data.handle_size, ctrl.y + data.handle_size },
                    data.handle_color,
                    data.handle_rounding,
                    data.ms_HANDLE_FLAGS,
                    data.handle_thickness
                );
                break;
            }
        }
        
        return;
    }


    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      CUBIC RENDERING FUNCTIONS.
    // *************************************************************************** //
    
    //  "_render_CUBIC_IMPL"
    //
    template <typename VStyle>
    inline void                         _render_CUBIC_IMPL                  (const ImVec2 & pos, const ImVec2 & h_in, const ImVec2 & h_out, const VStyle & style) const noexcept
    {
        const StyleData &           data            = *style.data;
        
        
        //      DISPATCH ACTION BASED ON BEZIER CURVATURE...
        switch (this->m_curvature_state)
        {
            case CurvatureState::In : {
                style.dl->AddLine( pos, h_in,  data.line_color, data.line_width );          //  2A.     LINE  V  |==>  IN.
                style.dl->AddRect(                                                          //  2B.     "IN" HANDLE.
                    { h_in.x - data.handle_size,       h_in.y - data.handle_size },
                    { h_in.x + data.handle_size,       h_in.y + data.handle_size },
                    data.handle_color,
                    data.handle_rounding,
                    data.ms_HANDLE_FLAGS,
                    data.handle_thickness
                );
                break;
            }
            case CurvatureState::Out : {
                style.dl->AddLine( pos, h_out, data.line_color, data.line_width );          //  3A.     LINE  V  |==>  OUT.
                style.dl->AddRect(                                                          //  3B.     "OUT" HANDLE.
                    { h_out.x - data.handle_size,      h_out.y - data.handle_size },
                    { h_out.x + data.handle_size,      h_out.y + data.handle_size },
                    data.handle_color,
                    data.handle_rounding,
                    data.ms_HANDLE_FLAGS,
                    data.handle_thickness
                );
                break;
            }
            case CurvatureState::All : {
                style.dl->AddLine( pos, h_in,  data.line_color, data.line_width );          //  1A.     LINE  V  |==>  IN.
                style.dl->AddLine( pos, h_out, data.line_color, data.line_width );          //  1B.     LINE  V  |==>  OUT.
                style.dl->AddRect(                                                          //  1C.     "IN" HANDLE.
                    { h_in.x - data.handle_size,       h_in.y - data.handle_size },
                    { h_in.x + data.handle_size,       h_in.y + data.handle_size },
                    data.handle_color,
                    data.handle_rounding,
                    data.ms_HANDLE_FLAGS,
                    data.handle_thickness
                );
                style.dl->AddRect(                                                          //  1D.     "OUT" HANDLE.
                    { h_out.x - data.handle_size,      h_out.y - data.handle_size },
                    { h_out.x + data.handle_size,      h_out.y + data.handle_size },
                    data.handle_color,
                    data.handle_rounding,
                    data.ms_HANDLE_FLAGS,
                    data.handle_thickness
                );
                break;
            }
            default : { break; }
        }
        
        return;
    }
    
    
    
    //  "_render_CUBIC_hovered_IMPL"
    //      Special case for one of three items being hovered.
    //      This is implemented as a second function to avoid the CONDITIONAL/TERNARY operations which will be unnessesary for overwhelming majority of the verticies.
    //
    template <typename VStyle>
    inline void                         _render_CUBIC_hovered_IMPL          (const ImVec2 & pos, const ImVec2 & h_in, const ImVec2 & h_out, const VStyle & style) const noexcept
    {
        const StyleData &           data            = *style.data;
        const bool                  ih_hovered      = ( data.hovered == HoverState::InHandle    );
        const bool                  oh_hovered      = ( data.hovered == HoverState::OutHandle   );
        const float &               ih_size         = (ih_hovered)  ? data.hovered_size     : data.handle_size;
        const ImU32 &               ih_color        = (ih_hovered)  ? data.hovered_color    : data.handle_color;
        const float &               oh_size         = (oh_hovered)  ? data.hovered_size     : data.handle_size;
        const ImU32 &               oh_color        = (oh_hovered)  ? data.hovered_color    : data.handle_color;
        
        //      DISPATCH ACTION BASED ON BEZIER CURVATURE...
        switch (this->m_curvature_state)
        {
            case CurvatureState::In : {
                style.dl->AddLine( pos, h_in,  data.line_color, data.line_width );          //  2A.     LINE  V  |==>  IN.
                style.dl->AddRect(                                                          //  2B.     "IN" HANDLE.
                    { h_in.x - ih_size,         h_in.y - ih_size },
                    { h_in.x + ih_size,         h_in.y + ih_size },
                    ih_color,
                    data.handle_rounding,
                    data.ms_HANDLE_FLAGS,
                    data.handle_thickness
                );
                break;
            }
            case CurvatureState::Out : {
                style.dl->AddLine( pos, h_out, data.line_color, data.line_width );          //  3A.     LINE  V  |==>  OUT.
                style.dl->AddRect(                                                          //  3B.     "OUT" HANDLE.
                    { h_out.x - oh_size,        h_out.y - oh_size },
                    { h_out.x + oh_size,        h_out.y + oh_size },
                    oh_color,
                    data.handle_rounding,
                    data.ms_HANDLE_FLAGS,
                    data.handle_thickness
                );
                break;
            }
            case CurvatureState::All : {
                style.dl->AddLine( pos, h_in,  data.line_color, data.line_width );          //  1A.     LINE  V  |==>  IN.
                style.dl->AddLine( pos, h_out, data.line_color, data.line_width );          //  1B.     LINE  V  |==>  OUT.
                style.dl->AddRect(                                                          //  1C.     "IN" HANDLE.
                    { h_in.x - ih_size,         h_in.y - ih_size },
                    { h_in.x + ih_size,         h_in.y + ih_size },
                    ih_color,
                    data.handle_rounding,
                    data.ms_HANDLE_FLAGS,
                    data.handle_thickness
                );
                style.dl->AddRect(                                                          //  1D.     "OUT" HANDLE.
                    { h_out.x - oh_size,        h_out.y - oh_size },
                    { h_out.x + oh_size,        h_out.y + oh_size },
                    oh_color,
                    data.handle_rounding,
                    data.ms_HANDLE_FLAGS,
                    data.handle_thickness
                );
                break;
            }
            default : { break; }
        }
        
        return;
    }
    
    // *************************************************************************** //
    
    
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.



//
//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "BezierControl" INLINE STRUCT DEFINITION.



//  "to_json"
//
inline void to_json(nlohmann::json & j, const BezierControl & obj)
{
    j = {
        { "in_handle",              { obj.in_handle.x,  obj.in_handle.y     }   },
        { "out_handle",             { obj.out_handle.x, obj.out_handle.y    }   },
        { "kind",                   obj.kind                                    },
        { "curvature_state",        obj.m_curvature_state                       }
    };

    return;
}


//  "from_json"
//
inline void from_json(const nlohmann::json & j, BezierControl & obj)
{
    j.at("kind")                    .get_to(obj.kind                  );
    j.at("curvature_state")         .get_to(obj.m_curvature_state     );

    auto    ih                      = j.at("in_handle"      );
    auto    oh                      = j.at("out_handle"     );
    obj.in_handle                   = { ih[0], ih[1] };
    obj.out_handle                  = { oh[0], oh[1] };
    
    return;
}

//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "BezierControl".



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "BEZIER CONTROL-POINT".












// *************************************************************************** //
//
//
//
//      3.      [DATA LAYER]    VERTEX...
// *************************************************************************** //
// *************************************************************************** //

//  "Vertex_t" / "Pos" / "Anchor Point"
//
//      A pure geometry node. `in_handle` and `out_handle` are offsets from
//      `pos`; when they are both zero the segment attached to this vertex is
//      straight. Handles live in world‑space units.
//
//  template<typename VID>
template<ObjectCFGTraits CFG>
struct Vertex_t
{
    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    //  USE_OBJECT_CFG_ALIASES(CFG);
    _USE_OBJECT_CFG_ALIASES
    //
    //                              LOCAL TYPES:
    using                               id_type                                 = vertex_id                         ;
    using                               CurvatureType                           = BezierControl::CurvatureType      ;
    using                               CurvatureState                          = BezierControl::CurvatureState     ;
    using                               StyleData                               = BezierControl::StyleData          ;
    using                               HoverState                              = BezierControl::HoverState         ;
    //
    //                              OBJECT TYPES:
    using                               Vertex                                  = Vertex_t<CFG>;
    
    
    
    // *************************************************************************** //
    //      STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    static constexpr ImPlotRect         ms_BEZIER_SLIDER_LIMITS                 = { -128.0f, 128.0f,     -128.0f, 128.0f };
    //
    static constexpr const char *       ms_DEF_VERTEX_TITLE_FMT_STRING          = "Vertex V%03d (ID #%06u)";
    static constexpr const char *       ms_DEF_VERTEX_SELECTOR_FMT_STRING       = "V%03u";
    static constexpr size_t             ms_MAX_VERTEX_NAME_LENGTH               = 10ULL;
    
//
// *************************************************************************** //
// *************************************************************************** //   END "CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//      1.          DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      IMPORTANT DATA-MEMBERS.
    // *************************************************************************** //
    id_type                             id                              = 0;
    float                               x                               = 0.0f,
                                        y                               = 0.0f,
                                        z                               = 0.0f;

    // *************************************************************************** //
    //      BéZIER HANDLE DATA.
    // *************************************************************************** //
    BezierControl                       m_bezier                        = {   };

    // *************************************************************************** //
    //      TRANSIENT STATE VARIABLES.
    // *************************************************************************** //
    
//
// *************************************************************************** //
// *************************************************************************** //   END "DATA-MEMBERS".

    
   
// *************************************************************************** //
//
//      2.A.        MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
    // *************************************************************************** //
    //      DELETED FUNCTIONS.              |   ...
    // *************************************************************************** //
    inline void                         SetXYPosition                   (const ImVec2 & ) && noexcept       = delete;
    inline void                         SetYZPosition                   (const ImVec2 & ) && noexcept       = delete;
    inline void                         SetXZPosition                   (const ImVec2 & ) && noexcept       = delete;
    //
    inline void                         SetInHandle                     (const ImVec2 & ) && noexcept       = delete;
    inline void                         SetOutHandle                    (const ImVec2 & ) && noexcept       = delete;
    
//
// *************************************************************************** //
// *************************************************************************** //   END "MEMBER FUNCTIONS".

    
   
// *************************************************************************** //
//
//      2.B.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      STATIC FUNCTIONS.
    // *************************************************************************** //
           
    //  "SegmentIsLinear"
    [[nodiscard]] static inline bool    SegmentIsLinear                 (const ImVec2 & P0, const ImVec2 & P1, const ImVec2 & P2, const ImVec2 & P3) noexcept
    {
        namespace                   math        = cblib::math;
        constexpr const float &     GATE        = BezierControl::ms_BEZIER_NUMERICAL_ERROR;
        return (
            math::is_close( P1.x, P0.x, GATE)  &&
            math::is_close( P1.y, P0.y, GATE)  &&
            math::is_close( P2.x, P3.x, GATE)  &&
            math::is_close( P2.y, P3.y, GATE)
        );
        //  return ( math::is_close( P1.x, P0.x, GATE)  &&  math::is_close( P1.y, P0.y, GATE)  &&
        //           math::is_close( P2.y, P3.y, GATE)  &&  math::is_close( P2.y, P3.y, GATE) );
    }
           
    //  "SegmentIsLinear"
    [[nodiscard]] static inline bool    SegmentIsLinear                 (const Vertex & a, const Vertex & b) noexcept
    {
        return ( a.IsOutLinear()  &&  b.IsInLinear() );
    }
           
    //  "SegmentIsCurved"
    [[nodiscard]] static inline bool    SegmentIsCurved                 (const Vertex & a, const Vertex & b) noexcept
    {
        return ( !a.IsOutLinear()  ||  !b.IsInLinear() );
    }

    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      QUERY FUNCTIONS.
    // *************************************************************************** //
    
    //  "IsLinear"
    [[nodiscard]] inline bool           IsLinear                        (void) const noexcept               { return m_bezier._is_linear();                 }
    [[nodiscard]] inline bool           IsInLinear                      (void) const noexcept               { return m_bezier._is_in_linear();              }
    [[nodiscard]] inline bool           IsOutLinear                     (void) const noexcept               { return m_bezier._is_out_linear();             }
    
    //  "IsQuadratic"
    [[nodiscard]] inline bool           IsQuadratic                     (void) const noexcept               { return m_bezier._is_quadratic();              }
    [[nodiscard]] inline bool           IsCubic                         (void) const noexcept               { return m_bezier._is_cubic();                  }
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      SETTER / GETTER FUNCTIONS.
    // *************************************************************************** //
    
    //  "SetCurvatureType"
    inline void                         SetCurvatureType                (CurvatureType kind_) noexcept      { return m_bezier._set_curvature_type(kind_);   }
    
    //  "SetPosition"
    inline void                         SetXYPosition                   (const ImVec2 & pos_) & noexcept                { 
        this->x = cblib::math::quantize(pos_.x, BezierControl::ms_BEZIER_NUMERICAL_ERROR);
        this->y = cblib::math::quantize(pos_.y, BezierControl::ms_BEZIER_NUMERICAL_ERROR);
    }
    inline void                         SetXYPosition                   (const float  x_, const float  y_) noexcept     {
        this->x = cblib::math::quantize(x_, BezierControl::ms_BEZIER_NUMERICAL_ERROR);
        this->y = cblib::math::quantize(y_, BezierControl::ms_BEZIER_NUMERICAL_ERROR);
    }
    inline void                         SetXYPosition                   (const double x_, const double y_) noexcept     {
        this->x = cblib::math::quantize(x_, BezierControl::ms_BEZIER_NUMERICAL_ERROR);
        this->y = cblib::math::quantize(y_, BezierControl::ms_BEZIER_NUMERICAL_ERROR);
    }
    //
    inline void                         SetYZPosition                   (const ImVec2 & pos_) & noexcept    { this->y = pos_.y; this->y = pos_.y;           }
    inline void                         SetXZPosition                   (const ImVec2 & pos_) & noexcept    { this->x = pos_.x; this->z = pos_.y;           }
    
    //  "SetInHandle"
    inline void                         SetInHandle                     (const ImVec2 & ih_) & noexcept                         { this->m_bezier._set_in_handle(ih_);               }
    inline void                         SetInHandle                     (const float  ih_x_, const float  ih_y_) noexcept       { this->m_bezier._set_in_handle(ih_x_, ih_y_);      }
    inline void                         SetInHandle                     (const double ih_x_, const double ih_y_) noexcept       { this->m_bezier._set_in_handle(ih_x_, ih_y_);      }
    
    //  "SetOutHandle"
    inline void                         SetOutHandle                    (const ImVec2 & oh_) & noexcept                         { this->m_bezier._set_out_handle(oh_);              }
    inline void                         SetOutHandle                    (const float  oh_x_, const float  oh_y_) noexcept       { this->m_bezier._set_out_handle(oh_x_, oh_y_);     }
    inline void                         SetOutHandle                    (const double oh_x_, const double oh_y_) noexcept       { this->m_bezier._set_out_handle(oh_x_, oh_y_);     }
    
    
    
    
    //  "GetCurvatureType"
    [[nodiscard]] inline CurvatureType  GetCurvatureType                (void) const noexcept               { return m_bezier._get_curvature_type();        }
    
    //  "GetXYPosition"
    [[nodiscard]] inline ImVec2         GetXYPosition                   (void) const noexcept               { return { this->x, this->y };                  }
    [[nodiscard]] inline ImVec2         GetYZPosition                   (void) const noexcept               { return { this->y, this->z };                  }
    [[nodiscard]] inline ImVec2         GetXZPosition                   (void) const noexcept               { return { this->x, this->z };                  }
    //
    //  "GetInHandle"
    [[nodiscard]] inline ImVec2         GetInHandle                     (void) const noexcept               { return this->m_bezier._get_in_handle();       }
    [[nodiscard]] inline ImVec2 &       GetInHandle                     (void) noexcept                     { return this->m_bezier._get_in_handle();       }
    //
    [[nodiscard]] inline ImVec2         GetOutHandle                    (void) const noexcept               { return this->m_bezier._get_out_handle();      }
    [[nodiscard]] inline ImVec2 &       GetOutHandle                    (void) noexcept                     { return this->m_bezier._get_out_handle();      }
    
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    
    //  "UpdateCurvatureState"
    inline CurvatureState               UpdateCurvatureState            (void) const noexcept               { return this->m_bezier._update_curvature_state(); }
    
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      VERTEX OPERATION FUNCTIONS.
    // *************************************************************************** //
    
    //  "EffectiveOutHandle"
    inline ImVec2                       EffectiveOutHandle              (void) const noexcept               { return ( this->IsOutLinear() )    ? ImVec2(0.0f, 0.0f)    : this->m_bezier.out_handle;    }
    inline ImVec2                       EffectiveInHandle               (void) const noexcept               { return ( this->IsInLinear()  )    ? ImVec2(0.0f, 0.0f)    : this->m_bezier.in_handle;     }
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      RENDERING FUNCTIONS.
    // *************************************************************************** //
    
    //  "render"
    template <typename VStyle>
    inline void                         render                          (const VStyle & style) const noexcept
    {
        const ImVec2        origin          = ImVec2({ this->x, this->y });
        switch (style.data->hovered) {
            case HoverState::Vertex : {
                this->_render_vertex_hovered    (origin, style);                //      Each func. will invoke the "style.ws_2_px" as needed...
                break;
            }
            default : {
                this->_render_vertex            (origin, style);
                break;
            }
        }
        
        style._PopHoverState();
        return;
    }
    
    //  "render_all"
    template <typename VStyle>
    inline void                         render_all                      (const VStyle & style) const noexcept
    {
        const ImVec2        origin          = ImVec2({ this->x, this->y });
    
        switch (style.data->hovered)
        {
            case HoverState::Vertex : {
                this->m_bezier.render           (origin, style);                //  Pass the RAW (x, y) to each function.
                this->_render_vertex_hovered    (origin, style);                //      Each func. will invoke the "style.ws_2_px" as needed...
                break;
            }
            default : {
                this->m_bezier.render           (origin, style);
                this->_render_vertex            (origin, style);
                break;
            }
        }
        
        style._PopHoverState();
        return;
    }
    
    //  "_render_vertex"
    template <typename VStyle>
    inline void                         _render_vertex                  (const ImVec2 & origin, const VStyle & style) const noexcept
    { style.dl->AddCircleFilled( style.ws_to_px(origin), style.data->vertex_radius, style.data->vertex_color, style.data->vertex_tesselation ); }
    
    //  "_render_vertex_IMPL"
    template <typename VStyle>
    inline void                         _render_vertex_hovered          (const ImVec2 & origin, const VStyle & style) const noexcept
    { style.dl->AddCircleFilled( style.ws_to_px(origin), style.data->vertex_radius, style.data->hovered_color, style.data->vertex_tesselation ); }
    


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      UTILITY FUNCTIONS.
    // *************************************************************************** //
    
    //  "ResetCurvature"
    inline void                         ResetCurvature                  (void) noexcept       { this->m_bezier._reset_curvature(); }
        
        
        
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      STATIC---INLINE FUNCTIONS.
    // *************************************************************************** //
    
    //  "_s_draw_vertex_slider"
    //
    static inline bool                  _s_draw_vertex_slider           (   const char *                label
                                                                          , double &                    value
                                                                          , const float                 speed
                                                                          , const double &              min
                                                                          , const double &              max
                                                                          , const char *                fmt     = BezierControl::ms_SLIDER_FMT
                                                                          , const ImGuiSliderFlags      flags   = BezierControl::ms_SLIDER_FLAGS
                                                                        ) noexcept
    {
        constexpr int                   N                   = 1;
        bool                            dirty               = false;
        dirty = ImGui::DragScalarN(    label
                                     , ImGuiDataType_Double
                                     , &value
                                     , N
                                     , speed
                                     , &min
                                     , &max
                                     , fmt
                                     , flags
        );
        return dirty;
    };
        
        
        
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      UI---HELPER FUNCTIONS.
    // *************************************************************************** //
    
    //  "ui_Position"
    inline bool                         ui_Position                         ( const double xmax, const double ymax, const double speedx, const double speedy ) noexcept
    {
        ImVec2              position        = this->GetXYPosition();
        double              pos_x           = static_cast<double>( position.x       );
        double              pos_y           = static_cast<double>( position.y       );
    
        const bool          dirty1          = this->_s_draw_vertex_slider( "##Vertex_Position_X", pos_x, speedx, -xmax, xmax );
        ImGui::SameLine();
        const bool          dirty2          = this->_s_draw_vertex_slider( "##Vertex_Position_Y", pos_y, speedy, -ymax, ymax );
        

        if ( dirty1 || dirty2 )             { this->SetXYPosition( pos_x, pos_y ); }        //  UPDATE VALUE.
        
        return (dirty1 || dirty2);
    }
    
    
    //  "ui_CurvatureType"
    inline bool                         ui_CurvatureType                    (void) noexcept
    {
        constexpr int   N           = static_cast<int>(CurvatureType::COUNT);
        int             type_idx    = static_cast<int>(this->m_bezier.kind);
        const bool      dirty       = ImGui::Combo( "##Path_UIInternal_Kind"
                                                    , &type_idx
                                                    , DEF_BEZIER_CURVATURE_TYPE_NAMES.data()
                                                    , N );
        //  UPDATE VALUE...
        if ( dirty ) {
            this->m_bezier._set_curvature_type( static_cast<CurvatureType>(type_idx) );
        }
        
        return dirty;
    }
    
    
    //  "ui_InHandle"
    inline bool                         ui_InHandle                         ( const double xmax, const double ymax, const double speedx, const double speedy ) noexcept
    {
        ImVec2          in_handle       = this->GetInHandle();
        double          ih_x            = static_cast<double>( in_handle.x      );
        double          ih_y            = static_cast<double>( in_handle.y      );
    
    
        ImGui::BeginDisabled( this->m_bezier.kind == CurvatureType::Quadratic );
        //
            const bool      dirty1          = this->_s_draw_vertex_slider( "##Vertex_InHandle_X", ih_x, speedx, -xmax, xmax );
            ImGui::SameLine();
            const bool      dirty2          = this->_s_draw_vertex_slider( "##Vertex_InHandle_Y", ih_y, speedy, -ymax, ymax );
        //
        ImGui::EndDisabled();
        
        if ( dirty1 || dirty2 )         { this->SetInHandle( ih_x, ih_y ); }    //      UPDATE VALUE.
        
        
        return (dirty1 || dirty2);
    }
    
    
    //  "ui_OutHandle"
    inline bool                         ui_OutHandle                        ( const double xmax, const double ymax, const double speedx, const double speedy ) noexcept
    {
        ImVec2          out_handle      = this->GetOutHandle();
        double          oh_x            = static_cast<double>( out_handle.x      );
        double          oh_y            = static_cast<double>( out_handle.y      );
    
    
        const bool      dirty1          = this->_s_draw_vertex_slider( "##Vertex_OutHandle_X", oh_x, speedx, -xmax, xmax );
        ImGui::SameLine();
        const bool      dirty2          = this->_s_draw_vertex_slider( "##Vertex_OutHandle_Y", oh_y, speedy, -ymax, ymax );
        
        
        if ( dirty1 || dirty2 )         { this->SetOutHandle( oh_x, oh_y ); }    //      UPDATE VALUE.
        
        return (dirty1 || dirty2);
    }
    
    
    
    
    
    
    
    //  "ui_properties"
    inline void                         ui_properties                       (void)
    {
            //  using namespace path;
            //
            //  //  2.  CALL THE "PROPERTIES UI" FOR THE
            //  std::visit([&](auto & pl)
            //  {
                //  using T = std::decay_t<decltype(pl)>;
                //  //  skip monostate (has no draw_ui)
                //  if constexpr (!std::is_same_v<T, std::monostate>)
                    //  { pl.ui_properties(); }          // every real payload implements this
            //  }, payload);
            //
            //  return;
    }
    
    
    
    // *************************************************************************** //
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.



//
//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "Vertex_t" INLINE STRUCT DEFINITION.






//  "to_json"
//
template <typename IdT>
inline void to_json(nlohmann::json & j, const Vertex_t<IdT> & v)
{
    j = {
        { "id",                     v.id                                    },
        { "x",                      v.x                                     },
        { "y",                      v.y                                     },
        { "z",                      v.z                                     },
    //
        { "bezier",                 v.m_bezier                              }
    };
    
    return;
}
//
//
//  "from_json"
template <typename IdT>
inline void from_json(const nlohmann::json & j, Vertex_t<IdT> & v)
{
    j.at("id")                      .get_to(v.id                    );
    j.at("x")                       .get_to(v.x                     );
    j.at("y")                       .get_to(v.y                     );
    j.at("z")                       .get_to(v.z                     );
//
    //  j.at("kind")                    .get_to(v.kind                  );
    //  j.at("curvature_state")         .get_to(v.m_curvature_state     );
    j.at("bezier")                  .get_to(v.m_bezier              );
    
    //  auto    ih                      = j.at("in_handle");
    //  auto    oh                      = j.at("out_handle");
    //  v.in_handle                     = { ih[0], ih[1] };
    //  v.out_handle                    = { oh[0], oh[1] };
    
    
    
    
    //  v.m_bezier.in_handle              = v.in_handle;
    //  v.m_bezier.out_handle             = v.out_handle;
    //  v.m_bezier.kind                   = v.kind;
    //  v.m_bezier.m_curvature_state      = v.m_curvature_state;
    
    
    return;
}

//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "Vertices".












// *************************************************************************** //
//
//
//
//      4.      [DATA LAYER]    POINTS...
// *************************************************************************** //
// *************************************************************************** //

//  "PointStyle"
//
struct PointStyle   {
    ImU32       color       = IM_COL32(0,255,0,255);
    float       radius      = 4.0f;
    bool        visible     = true;
};
//
//  "to_json"
inline void to_json(nlohmann::json & j, const PointStyle & s)
{
    j = nlohmann::json{
        { "color",  s.color   },
        { "radius", s.radius  },
        { "visible",s.visible }
    };
}
//
//  "from_json"
inline void from_json(const nlohmann::json& j, PointStyle& s)
{
    j.at("color" ).get_to(s.color );
    j.at("radius").get_to(s.radius);
    j.at("visible").get_to(s.visible);
}




//  "Point_t"
//
template <typename PtID>
struct Point_t        {
    PtID            v;
    PointStyle      sty{};
};
//
//  "to_json"
template <typename PtID>
inline void to_json(nlohmann::json & j, const Point_t<PtID> & p)
{
    j = {
        { "v",          p.v         },
        { "sty",        p.sty       }
    };
    
    return;
}
//
//  "from_json"
template <typename PtID>
inline void from_json(const nlohmann::json & j, Point_t<PtID> & p)
{
    j.at("v"  )         .get_to(p.v     );
    j.at("sty")         .get_to(p.sty   );
    
    return;
}

//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "POINTS".












// *************************************************************************** //
//
//
//
//      5.      [DATA LAYER]    LINES...
// *************************************************************************** //
// *************************************************************************** //

//  "Line_t"
//
template <typename LID, typename ZID>
struct Line_t         {
    [[nodiscard]] inline bool is_mutable(void) const noexcept
    { return visible && !locked; }
//
//
    LID         a, b;
    ImU32       color       = IM_COL32(255,255,0,255);
    float       thickness   = 2.0f;
//
    ZID         z_index     = Z_FLOOR_USER;
    bool        locked      = false;
    bool        visible     = true;
};
//
//  "to_json"
template <typename LID, typename ZID>
inline void to_json(nlohmann::json  j, const Line_t<LID, ZID> & l)
{
    j = { { "a",            l.a             },
          { "b",            l.b             },
          { "color",        l.color         },
          { "thickness",    l.thickness     },
          { "z_index",      l.z_index       },
          { "locked",       l.locked        },
          { "visible",      l.visible       }
    };
}
//
//  "from_json"
template <typename LID, typename ZID>
inline void from_json(const nlohmann::json & j, Line_t<LID, ZID>  & l)
{
    j.at("a"                ).get_to(l.a);
    j.at("b"                ).get_to(l.b);
    j.at("color"            ).get_to(l.color);
    j.at("z_index"          ).get_to(l.z_index);
    j.at("locked"           ).get_to(l.locked);
    j.at("visible"          ).get_to(l.visible);
}



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "LINES".

  















// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.












#endif      //  _CBAPP_EDITOR_OBJECTS_VERTEX_H  //
// *************************************************************************** //
// *************************************************************************** //   END FILE.

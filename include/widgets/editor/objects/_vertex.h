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
    Default = 0,
    Highlight,
    COUNT
};
//
static constexpr cblib::EnumArray< VertexStyleType, VertexStyleData >
DEF_VERTEX_STYLES{
{
    VertexStyleData     //  0.  DEFAULT:
    {
        /*  vertex_radius       */ 6.50f,
        /*  vertex_color        */ IM_COL32(0,       0,    255,      170),
        /*  vertex_tesselation  */ 12,
        /*  handle_color        */ IM_COL32(215,     255,    0,      255),
        /*  handle_size         */ 5.50f,
        /*  handle_rounding     */ 0.00f,
        /*  handle_thickness    */ 2.00f,
        /*  line_color          */ IM_COL32(215,     255,    0,      170),
        /*  line_width          */ 1.00f,
        /*  hovered_color       */ IM_COL32(0,       0,    255,      255),
        /*  hovered_size        */ 11.0f
    },
//
    VertexStyleData     //  1.  HIGHLIGHT:
    {
        /*  vertex_radius       */ 8.00f,
        /*  vertex_color        */ IM_COL32(0,       0,    255,      200),
    //
        /*  vertex_tesselation  */ 12,
        /*  handle_color        */ IM_COL32(255,     215,    0,      255),
        /*  handle_size         */ 6.50f,
        /*  handle_rounding     */ 0.00f,
        /*  handle_thickness    */ 2.50f,
    //
        /*  line_color          */ IM_COL32(255,     215,    0,      170),
        /*  line_width          */ 2.50f,
        /*  hovered_color       */ IM_COL32(0,       0,    255,      255),
        /*  hovered_size        */ 12.0f
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
    
    //  "_no_op"
    inline void                         PushDL                              (ImDrawList * & dl_) const noexcept     { this->dl = dl_;           };
    inline void                         PopDL                               (void) const noexcept                   { this->dl = nullptr;       };
    
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
enum class BezierCurvatureType : uint8_t            { Corner,     Smooth,     Symmetric,      COUNT };
//
static constexpr cblib::EnumArray <BezierCurvatureType, const char *>
DEF_BEZIER_CURVATURE_TYPE_NAMES                     = { "Corner",       "Smooth",       "Symmetric" };



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
    static constexpr float              ms_BEZIER_NUMERICAL_ERROR               = 1e-6;
    
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
    BezierCurvatureType                 kind                            = CurvatureType::Corner;
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
    //      SETTER / GETTER FUNCTIONS.
    // *************************************************************************** //
    
    //  "_is_linear"
    [[nodiscard]] inline bool           _is_linear                      (void) const noexcept           { return ( m_curvature_state == CurvatureState::None );     }
    [[nodiscard]] inline bool           _is_in_linear                   (void) const noexcept           { return ( m_curvature_state != CurvatureState::In  &&  m_curvature_state != CurvatureState::All );     }
    [[nodiscard]] inline bool           _is_out_linear                  (void) const noexcept           { return ( m_curvature_state != CurvatureState::Out  &&  m_curvature_state != CurvatureState::All );     }
    
    //  "_set_curvature_type"
    inline void                         _set_curvature_type             (CurvatureType kind_) noexcept
    {
        if (kind_ == this->kind)    { return; }
        
        this->kind = kind_;
        this->_update_curvature_type();
        return;
    }
    
    //  "_set_in_handle"
    inline void                         _set_in_handle                  (const ImVec2 & ih_) noexcept   { this->in_handle = ih_;    this->_update_curvature_state();   }
    inline void                         _set_out_handle                 (const ImVec2 & oh_) noexcept   { this->out_handle = oh_;   this->_update_curvature_state();   }
    
    
    
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
    
    
        if      ( both )            { this->m_curvature_state = CurvatureState::All;    }
        else if ( in_is_set )       { this->m_curvature_state = CurvatureState::In;     }
        else if ( out_is_set )      { this->m_curvature_state = CurvatureState::Out;    }
        else                        { this->m_curvature_state = CurvatureState::None;   }
    
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
        this->in_handle             = { };
        this->out_handle            = { };
        return;
    }


    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      RENDERING FUNCTIONS.
    // *************************************************************************** //
    
    //  "render"
    template <typename VStyle>
    inline void                         render                          (const ImVec2 & origin, VStyle & style) const noexcept
    {
        const ImVec2                pos             = style.ws_to_px(    {  origin.x                        , origin.y                          }   );
        const ImVec2                h_in            = style.ws_to_px(    {  origin.x + this->in_handle.x    , origin.y + this->in_handle.y      }   );
        const ImVec2                h_out           = style.ws_to_px(    {  origin.x + this->out_handle.x   , origin.y + this->out_handle.y     }   );
        
        this->_update_curvature_state();
        
        
        
        switch (style.data->hovered)
        {
            //      CASE 1 :    EITHER "IN" / "OUT" HANDLE IS HOVERED.
            case HoverState::InHandle  :
            case HoverState::OutHandle :
            {
                this->_render_hovered_IMPL      (origin, h_in, h_out, style);
                break;
            }
            //
            //      CASE 2 :    NONE OF THE THREE ITEMS ARE HOVERED...
            default :
            {
                this->_render_IMPL              (pos, h_in, h_out, style);
                break;
            }
        }
        
        return;
    }
    
    //  "_render_IMPL"
    //
    template <typename VStyle>
    inline void                         _render_IMPL                        (const ImVec2 & pos, const ImVec2 & h_in, const ImVec2 & h_out, VStyle & style) const noexcept
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
    
    
    
    //  "_render_hovered_IMPL"
    //      Special case for one of three items being hovered.
    //      This is implemented as a second function to avoid the CONDITIONAL/TERNARY operations which will be unnessesary for overwhelming majority of the verticies.
    //
    template <typename VStyle>
    inline void                         _render_hovered_IMPL                (const ImVec2 & pos, const ImVec2 & h_in, const ImVec2 & h_out, VStyle & style) const noexcept
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
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.



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
template<typename VID>
struct Vertex_t
{
    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    using                               id_type                                 = VID;
    using                               CurvatureType                           = BezierControl::CurvatureType;
    using                               CurvatureState                          = BezierControl::CurvatureState;
    using                               StyleData                               = BezierControl::StyleData;
    using                               HoverState                              = BezierControl::HoverState;
    
    
    
    // *************************************************************************** //
    //      STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    static constexpr ImPlotRect         ms_BEZIER_SLIDER_LIMITS                 = { -128.0f, 128.0f,     -128.0f, 128.0f };
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
    VID                                 id                              = 0;
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
//      2.B.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      SETTER / GETTER FUNCTIONS.
    // *************************************************************************** //
    
    //  "IsLinear"
    [[nodiscard]] inline bool           IsLinear                        (void) const noexcept           { return m_bezier._is_linear();                 }
    [[nodiscard]] inline bool           IsInLinear                      (void) const noexcept           { return m_bezier._is_in_linear();              }
    [[nodiscard]] inline bool           IsOutLinear                     (void) const noexcept           { return m_bezier._is_out_linear();             }
    
    //  "SetCurvatureType"
    inline void                         SetCurvatureType                (CurvatureType kind_) noexcept  { return m_bezier._set_curvature_type(kind_);   }
    
    //  "SetInHandle"
    inline void                         SetInHandle                     (const ImVec2 & ih_) noexcept   { this->m_bezier._set_in_handle(ih_);           }
    inline void                         SetOutHandle                    (const ImVec2 & oh_) noexcept   { this->m_bezier._set_out_handle(oh_);          }
    
    
    
    //  "GetCurvatureType"
    [[nodiscard]] inline CurvatureType  GetCurvatureType                (void) const noexcept           { return m_bezier._get_curvature_type();        }
    
    //  "GetXYPosition"
    [[nodiscard]] inline ImVec2         GetXYPosition                   (void) const noexcept           { return { this->x, this->y };                  }
    [[nodiscard]] inline ImVec2         GetYZPosition                   (void) const noexcept           { return { this->y, this->z };                  }
    [[nodiscard]] inline ImVec2         GetXZPosition                   (void) const noexcept           { return { this->x, this->z };                  }
    //
    //  "GetInHandle"
    [[nodiscard]] inline ImVec2         GetInHandle                     (void) const noexcept           { return this->m_bezier._get_in_handle();       }
    [[nodiscard]] inline ImVec2 &       GetInHandle                     (void) noexcept                 { return this->m_bezier._get_in_handle();       }
    //
    [[nodiscard]] inline ImVec2         GetOutHandle                    (void) const noexcept           { return this->m_bezier._get_out_handle();      }
    [[nodiscard]] inline ImVec2 &       GetOutHandle                    (void) noexcept                 { return this->m_bezier._get_out_handle();      }
    
    

    // *************************************************************************** //



    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    
    //  "UpdateCurvatureState"
    inline CurvatureState               UpdateCurvatureState            (void) const noexcept       { return this->m_bezier._update_curvature_state(); }
    
    // *************************************************************************** //



    // *************************************************************************** //
    //      RENDERING FUNCTIONS.
    // *************************************************************************** //
    
    //  "render"
    template <typename VStyle>
    inline void                         render                          (VStyle & style) const noexcept
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
        
        style.data->hovered = HoverState::None;
        return;
    }
    
    
    
    //  "_render_vertex"
    template <typename VStyle>
    inline void                         _render_vertex                  (const ImVec2 & origin, VStyle & style) const noexcept
    { style.dl->AddCircleFilled( style.ws_to_px(origin), style.data->vertex_radius, style.data->vertex_color, style.data->vertex_tesselation ); }
    
    //  "_render_vertex_IMPL"
    template <typename VStyle>
    inline void                         _render_vertex_hovered          (const ImVec2 & origin, VStyle & style) const noexcept
    { style.dl->AddCircleFilled( style.ws_to_px(origin), style.data->vertex_radius, style.data->hovered_color, style.data->vertex_tesselation ); }
    
    // *************************************************************************** //



    // *************************************************************************** //
    //      UTILITY FUNCTIONS.
    // *************************************************************************** //
    
    //  "ResetCurvature"
    inline void                         ResetCurvature                  (void) noexcept       { this->m_bezier._reset_curvature(); }

    // *************************************************************************** //
    
    
    
//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.



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

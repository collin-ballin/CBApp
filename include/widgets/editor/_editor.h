/***********************************************************************************
*
*       ********************************************************************
*       ****              _ E D I T O R . H  ____  F I L E              ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      July 13, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBWIDGETS_EDITOR_IMPL_H
#define _CBWIDGETS_EDITOR_IMPL_H  1


//      //  From the 2nd frame onward, test drag distance
//      //  if ( m_pen.pending_time >= PEN_DRAG_TIME_THRESHOLD && ImGui::IsMouseDragPastThreshold(ImGuiMouseButton_Left, PEN_DRAG_MOVEMENT_THRESHOLD) )
//      if ( m_pen.pending_time >= PEN_DRAG_TIME_THRESHOLD && ImGui::IsMouseDragPastThreshold(ImGuiMouseButton_Left, PEN_DRAG_MOVEMENT_THRESHOLD) )
//      {
//          _pen_begin_handle_drag( m_pen.pending_vid, /*out_handle=*/true, /*force_select=*/true );
//          if ( Vertex * v = find_vertex_mut(m_vertices, m_pen.pending_vid) )
//              v->in_handle = ImVec2(0,0);              // make handle visible
//
// Updated declarations for selection overlay functions (no origin argument)
// void _draw_selection_highlight(ImDrawList* dl) const;
// void _draw_selection_bbox(ImDrawList* dl) const;
// void _draw_selected_handles(ImDrawList* dl) const;

//
//          m_pen.pending_handle = false;                // hand-off to drag logic
//          m_pen.pending_time   = 0.0f;
//          _pen_update_handle_drag(it);                 // first update
//          return;
//      }
        


//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
#include "utility/utility.h"
#include "widgets/editor/_constants.h"
#include "widgets/editor/_types.h"
#include "widgets/editor/_overlays.h"

//  0.2     STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <cstdlib>          // C-Headers...
#include <stdio.h>
#include <unistd.h>
#include <random>
#include <tuple>
#include <utility>

#include <algorithm>
#include <array>
#include <unordered_set>
#include <optional>

#include <string>           //  <======| std::string, ...
#include <string_view>
#include <vector>           //  <======| std::vector, ...
#include <stdexcept>        //  <======| ...
#include <limits.h>
#include <math.h>

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



//  "EditorIMPL"
//
struct EditorIMPL {
//      0.          CONSTANTS AND ALIASES...
// *************************************************************************** //
// *************************************************************************** //
public:

    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    using                           Font                            = app::Font_t                           ;
    using                           Logger                          = utl::Logger                           ;
    using                           LogLevel                        = utl::LogLevel                         ;
    //
    using                           CBCapabilityFlags               = CBCapabilityFlags_                    ;
    using                           Anchor                          = BBoxAnchor                            ;
    //
    using                           PopupHandle                     = EditorPopupBits                       ;
    using                           CBEditorPopupFlags              = CBEditorPopupFlags_                   ;
    using                           PopupInfo                       = EditorPopupInfo                       ;
    //
    using                           LabelFn                         = std::function<void(const char *)>     ;
        
    // *************************************************************************** //
    //
    //
    //
    //
    // *************************************************************************** //
    //      ** ID-VALUE TEMPLATE CONVENTIONS **
    //      ID Template Parameters MUST ALWAYS follow this order:
    //
    //          template< typename VertexID, typename PointID, typename LineID, typename PathID >
    // *************************************************************************** //
    // *************************************************************************** //
    //
    //                      ID / INDEX TYPES:
        template<typename T, typename Tag>
        using                           ID                              = cblib::utl::IDType<T, Tag>;
    //
        using                           VertexID                        = uint32_t;     //    ID<std::uint32_t, Vertex_Tag>             ;
        using                           HandleID                        = uint8_t;      //    ID<std::uint32_t, Point_Tag>              ;
        using                           PointID                         = uint32_t;     //    ID<std::uint32_t, Point_Tag>              ;
        using                           LineID                          = uint32_t;     //    ID<std::uint32_t, Line_Tag>               ;
        using                           PathID                          = uint32_t;     //    ID<std::uint32_t, Path_Tag>               ;
        using                           ZID                             = uint32_t;     //    ID<std::uint32_t, Path_Tag>               ;
        using                           OverlayID                       = OverlayManager::OverlayID;
        using                           HitID                           = uint32_t;     //    ID<std::uint32_t, Hit_Tag>                ;
    //
    //                              OBJECT TYPE ALIASES (BASED ON INDEX TYPES):
        using                           Vertex                          = Vertex_t          <VertexID>                                  ;
        //  using                       Handle                          = Handle_t          <HandleID>                                  ;
        using                           Point                           = Point_t           <PointID>                                   ;
        using                           Line                            = Line_t            <LineID, ZID>                               ;
        using                           Path                            = Path_t            <PathID, VertexID, ZID>                     ;
        using                           Overlay                         = Overlay_t         <OverlayID>                                 ;
        using                           Hit                             = Hit_t             <HitID>                                     ;
        using                           PathHit                         = PathHit_t         <PathID, VertexID>                          ;
        using                           EndpointInfo                    = EndpointInfo_t    <PathID>                                    ;
    //
    //
    //
    //                              PRIMARY STATE OBJECTS:
        using                           EditorState                     = EditorState_t     <VertexID, PointID, LineID, PathID, ZID>    ;
        using                           BrowserState                    = BrowserState_t    <VertexID, PointID, LineID, PathID, ZID>    ;
        using                           IndexState                      = IndexState_t      <VertexID, PointID, LineID, PathID, ZID>    ;
    //
    //                              SUBSIDIARY STATE OBJECTS:
        using                           Clipboard                       = Clipboard_t       <Vertex, Point, Line, Path>                 ;
        using                           Selection                       = Selection_t       <VertexID, PointID, LineID, PathID>         ;
    //
    //                              TOOL STATE OBJECTS:
        using                           PenState                        = PenState_t        <VertexID>                                  ;
        using                           ShapeState                      = ShapeState_t      <OverlayID>                                 ;
        using                           DebuggerState                   = DebuggerState_t   <VertexID, PointID, LineID, PathID, ZID>    ;
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    using                                           version_type                    = cblib::SchemaVersion::value_type;
    static constexpr version_type                   ms_EDITOR_JSON_MAJ_VERSION      = 4;
    static constexpr version_type                   ms_EDITOR_JSON_MIN_VERSION      = 1;
    //
    static constexpr cblib::SchemaVersion           ms_EDITOR_SCHEMA                { ms_EDITOR_JSON_MAJ_VERSION, ms_EDITOR_JSON_MIN_VERSION };
    
//
//
// *************************************************************************** //
// *************************************************************************** //   END "CONSTANTS AND ALIASES".






//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "EditorIMPL".
};












// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.












#endif      //  _CBWIDGETS_EDITOR_IMPL_H  //
// *************************************************************************** //
// *************************************************************************** //   END FILE.

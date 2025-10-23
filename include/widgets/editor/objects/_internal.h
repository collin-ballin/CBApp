/***********************************************************************************
*
*       ********************************************************************
*       ****            _ I N T E R N A L . H  ____  F I L E            ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      September 8, 2025.
*              MODULE:      CBAPP > EDITOR > OBJECTS/       | _internal.h
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_EDITOR_OBJECTS_INTERNAL_H
#define _CBAPP_EDITOR_OBJECTS_INTERNAL_H  1
#include CBAPP_USER_CONFIG



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include "cblib.h"
#include "utility/utility.h"
#include "widgets/editor/_constants.h"

//  0.2     STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <cstdlib>          // C-Headers...
#include <cstdint>
#include <stdio.h>
#include <unistd.h>

#include <algorithm>        //  SPECIAL...
#include <concepts>
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
#include <utility>



//      0.3.        "DEAR IMGUI" HEADERS...
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



// *************************************************************************** //
//
//
//
//      1.      POLICY / CONFIGURATION TYPES...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//      1A. POLICY |        "OBJECT" POLICY  [ FIRST LAYER ].
// *************************************************************************** //

//  "ObjectCFG_t"
//
template < typename VertexID, typename PointID, typename LineID, typename PathID, typename ZID, typename HitID >
struct ObjectCFG_t
{
    using                           vertex_id                   = VertexID                                                              ;
    using                           point_id                    = PointID                                                               ;
    using                           line_id                     = LineID                                                                ;
    using                           path_id                     = PathID                                                                ;
    using                           z_id                        = ZID                                                                   ;
    using                           hit_id                      = HitID                                                                 ;
//
//
//
    //  using                           path_id                     = EndpointInfo_t    <PathID>                                            ;
    //  using                           vertex_id                   = Vertex_t          <VertexID>                                          ;
//
};


//  "ObjectCFGTraits"
//
template <class CFG>
concept ObjectCFGTraits =
    requires
    {
        typename CFG::vertex_id     ;
        typename CFG::point_id      ;
        typename CFG::line_id       ;
        typename CFG::path_id       ;
        typename CFG::z_id          ;
        typename CFG::hit_id        ;
    }                                                               &&
    std::unsigned_integral  < typename CFG::vertex_id       >       &&
    std::unsigned_integral  < typename CFG::point_id        >       &&
    std::unsigned_integral  < typename CFG::line_id         >       &&
    std::unsigned_integral  < typename CFG::path_id         >       &&
    std::unsigned_integral  < typename CFG::z_id            >       &&
    std::unsigned_integral  < typename CFG::hit_id          >;



// *************************************************************************** //
//      1B. POLICY |        "EDITOR" POLICY  [ SECOND LAYER ].
// *************************************************************************** //

//  "EditorCFG_t"
//
template < typename Vertex_, typename Point_, typename Line_, typename Path_, typename Hit_ >
struct EditorCFG_t
{
    using                           Vertex                  = Vertex_                                                               ;
    using                           Point                   = Point_                                                                ;
    using                           Line                    = Line_                                                                 ;
    using                           Path                    = Path_                                                                 ;
    using                           Hit                     = Hit_                                                                  ;
//
//
//
};


//  "EditorCFGTraits"
//
template <class CFG>
concept EditorCFGTraits =
    requires
    {
        typename CFG::vertex_id     ;
        typename CFG::point_id      ;
        typename CFG::line_id       ;
        typename CFG::path_id       ;
        typename CFG::z_id          ;
        typename CFG::hit_id        ;
    }                                                               &&
    std::unsigned_integral  < typename CFG::vertex_id       >       &&
    std::unsigned_integral  < typename CFG::point_id        >       &&
    std::unsigned_integral  < typename CFG::line_id         >       &&
    std::unsigned_integral  < typename CFG::path_id         >       &&
    std::unsigned_integral  < typename CFG::z_id            >       &&
    std::unsigned_integral  < typename CFG::hit_id          >;



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 1.  "CONFIGS" ]].






// *************************************************************************** //
//
//
//
//      2.      MISC / UTILITIES...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//      2A. MISC. |        GEOMETRY-OBJECT UTILITIES.
// *************************************************************************** //

//  "EndpointInfo"
//
template<typename PID>
struct EndpointInfo_t
{
//
    PID     path_idx;
    bool    prepend;
//
};   // prepend==true ↔ first vertex



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 2.  "MISC" ]].












// *************************************************************************** //
//
//
//
//      3.      RENDERING UTILITIES...
// *************************************************************************** //
// *************************************************************************** //
namespace render { //     BEGINNING NAMESPACE "render"...



// *************************************************************************** //
//      3A. RENDERING. |        TYPE---TRAITS.
// *************************************************************************** //

//  "ws_to_px_CallbackTT"
//
template<typename Func>
concept     ws_to_px_CallbackTT     =
    requires (Func tp, ImVec2 ws) {
        { tp(ws) } noexcept -> std::same_as<ImVec2>;
    }
    && std::is_nothrow_invocable_r_v<ImVec2, Func, ImVec2>;
    
   
//  "px_to_ws_CallbackTT"
//
template<typename Func>
concept     px_to_ws_CallbackTT     =
    requires (Func tp, ImVec2 ws) {
        { tp(ws) } noexcept -> std::same_as<ImVec2>;
    }
    && std::is_nothrow_invocable_r_v<ImVec2, Func, ImVec2>;


//  "get_vertex_CallbackTT"
//
template<class Func, class V>
concept     get_vertex_CallbackTT   =
    requires (const std::vector<V>& vec, typename V::id_type id) {
        { std::invoke(std::declval<std::decay_t<Func>>(), vec, id) }
            noexcept -> std::same_as<const V*>;
    } &&
    std::is_nothrow_invocable_r_v<
        const V*, std::decay_t<Func>, const std::vector<V>&, typename V::id_type>;






// *************************************************************************** //
//      3B. RENDERING. |        SUBSIDIARY TYPES IMPLEMENTATION.
// *************************************************************************** //

//  "RenderCallbacks"
//
template<
      typename VID_
    , typename MapFn                                        //  "MapFn"     =   Pixels -- To -- World.
    , typename PamFn                                        //  "PamFn"     =   World -- To -- Pixels.
    , typename GVertexFn
    , typename CGVertexFn
    , typename container_type=std::vector<VID_>
>
    requires ws_to_px_CallbackTT        <MapFn>                     &&
             px_to_ws_CallbackTT        <PamFn>                     &&
             get_vertex_CallbackTT      <CGVertexFn, VID_>
//
struct RenderCallbacks
{
//                                  COORDINATE FUNCTIONS.
    MapFn                               ws_to_px            ;       //  e.g., lambda:   [this](ImVec2 ws)               { return world_to_pixels(ws);   }
    PamFn                               px_to_ws            ;       //  e.g., lambda:   [this](ImVec2 ws)               { return world_to_pixels(ws);   }
//
//
//                                  DATA---FETCHING FUNCTIONS.
    GVertexFn                           get_vertex          ;
    CGVertexFn                          cget_vertex         ;       //  e.g., lambda:   [this](VID id) -> const V *     { return find_vertex(...);      }
//
//
//                                  REFERENCES TO DATA.
    container_type &                    vertices            ;       //  REFERENCE to the array of vertices...
//
//
//
    //  inline                              RenderCallbacks                     (MapFn ws_to_px_, GVertexFn get_vertex_) noexcept
    inline RenderCallbacks(
          MapFn                         ws_to_px_
        , PamFn                         px_to_ws_
        , GVertexFn                     get_vertex_
        , CGVertexFn                    cget_vertex_
        , container_type &              vertices_
    ) noexcept
        : ws_to_px(ws_to_px_)               , px_to_ws(px_to_ws_)
        , get_vertex(get_vertex_)           , cget_vertex(cget_vertex_)
        , vertices(vertices_)
    {   }
//
//
//
//  [ LATER... ]        more hooks (selection tint, masks) with additional concepts
//
};


//  "RenderCallbacks"
//
struct RenderFrameArgs
{
    ImDrawList *        dl                  ;               //  target draw list (already set to the channel)
    int                 bezier_fill_steps   ;               //  e.g., style.ms_BEZIER_FILL_STEPS
    int                 bezier_segments     ;               //  (for strokes later)
//
//
//
//  [ LATER... ]        const bool& aa, const float& width_scale, etc.
//
};



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [ 3A.  "Render" ].
}//   END OF "render" NAMESPACE.






// *************************************************************************** //
//      3C. RENDERING. |        MAIN "RenderCTX" IMPLEMENTATION.
// *************************************************************************** //

//  "RenderCTX_t"
//
template<
      typename V
    , typename MapFn
    , typename PamFn
    , typename GVertexFn
    , typename CGVertexFn
    , typename container_type=std::vector<V>
>
struct RenderCTX_t
{
    using                               Callbacks                           = render::RenderCallbacks<V, MapFn, PamFn, GVertexFn, CGVertexFn>;
    using                               Args                                = render::RenderFrameArgs;
    using                               Vertex                              = V;
    using                               VertexID                            = V::id_type;

    //  "RenderCTX_t"
    //
    inline RenderCTX_t(
          MapFn                     ws_to_px_
        , PamFn                     px_to_ws_
        , GVertexFn                 get_vertex_
        , CGVertexFn                cget_vertex_
        , container_type &          vertices_
    ) noexcept
        : callbacks( ws_to_px_      , px_to_ws_     , get_vertex_       , cget_vertex_      , vertices_ )
    {   }
//
//
//
    const Callbacks                     callbacks                   ;       //  persistent, safe to keep by ref
    Args                                args            {   }       ;       //  per-frame, safe during draw pass
};



// *************************************************************************** //
//      3D. RENDERING. |        OBJECT RENDER---CHANNEL ABSTRACTION.
// *************************************************************************** //

//  "CanvasDrawChannel"
//
enum class CanvasDrawChannel : uint8_t
{
    Grid = 0,               //  Grid-Lines, Guides, etc.
//
    Objects,                //  The "AREA"/"FILL"/"BODY" for each object (ONLY for CLOSED-PATHS).
    Highlights,             //  Selection or Browser glow/highlight to make an object STAND-OUT.  Must appear BELOW the FEATURES but ABOVE THE AREA of an object for "silhouette" effect to create contrast.
    Features,               //  FEATURES of each object.  E.g.: the STROKE/PERIMETER (MUST appear ON-TOP of an object's area/fill).
    Accents,                //  ACCENTUATING FEATURES ("Points"/"Vertices" MUST appear ON-TOP of PATH-STROKE, etc).
//
    Glyphs,                 //  Handles, Selectables, Snapping-Locations (not implemented yet), USER-INTERACTIBLES, etc.  Must appear ON-TOP of ALL OBJECT GEOMETRY.
    Top,                    //  TOP-MOST LAYER       (RESERVED FOR CURSORS, ETC).
//
    COUNT
};



//  "ChannelCTX"
//
struct ChannelCTX
{
    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    using                               Channel                         = CanvasDrawChannel;
    using                               CTX                             = ChannelCTX;
    
    // *************************************************************************** //
    //      INTERNAL TYPES.
    // *************************************************************************** //

    struct Scope
    {
        using           Channel         = CanvasDrawChannel;
        using           CTX             = ChannelCTX;
    //
        CTX &           manager;
        Channel         prev;
    //
    //
    //
                        Scope           (CTX & src, Channel ch)         : manager(src), prev( static_cast<Channel>( src.split._Current) )     { manager.Set(ch); }
                        ~Scope          (void)                          { manager.split.SetCurrentChannel( manager.dl, static_cast<int>( prev ) ); }
    //
                        Scope           (const Scope & )                = delete;
        Scope &         operator =      (const Scope & )                = delete;
    };
    
//
// *************************************************************************** //
// *************************************************************************** //   END "CONSTANTS AND ALIASES".
    
    
    
// *************************************************************************** //
//
//      1.          DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //
    ImDrawList *            dl              = nullptr;
    ImDrawListSplitter      split           {   };
    bool                    active          = false;
    
//
// *************************************************************************** //
// *************************************************************************** //   END "DATA-MEMBERS".



// *************************************************************************** //
//
//      2.A.        MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
    // *************************************************************************** //
    //      INITIALIZATION METHODS.         |   ...
    // *************************************************************************** //
    inline                              ChannelCTX              (void)                          = default;
    inline explicit                     ChannelCTX              (ImDrawList * dl_)              { Begin(dl_); }
    inline                              ~ChannelCTX             (void)                          { End(); }
    //
    //                              DELETED MEMBERS:
                                        ChannelCTX              (const ChannelCTX & )            = delete;
    ChannelCTX &                        operator =              (const ChannelCTX & )            = delete;
    


    // *************************************************************************** //
    //      CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //

    //  "Begin"
    inline void                         Begin                   (ImDrawList * dl_)
    {
        IM_ASSERT( !this->active  &&  "Call to \"CanvasChannels.Begin()\" while struct is already active" );
        IM_ASSERT( dl_ != nullptr );
        
        dl          = dl_;
        split.Split( dl, static_cast<int>(Channel::COUNT) );
        active      = true;
        return;
    }
    
    
    //  "End"
    inline void                         End                     (void)
    {
        if ( !this->active )        { return; }
        
        this->split     .Merge(dl);
        this->active    = false;
        this->dl        = nullptr;
        return;
    }
    
    
    //  "Set"
    inline void                         Set                     (Channel ch)
    {
        IM_ASSERT( this->active  &&  "Call to \"CanvasChannels.Set()\" before struct is active" );
        this->split.SetCurrentChannel( this->dl, static_cast<int>(ch) );
        return;
    }


    
//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.



//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "MyStruct" INLINE STRUCT DEFINITION.



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 3.  "RENDERING UTILITIES" ]].












// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.












#endif      //  _CBAPP_EDITOR_OBJECTS_INTERNAL_H  //
// *************************************************************************** //
// *************************************************************************** //   END FILE.

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



//      0.      MISC / UTILITIES...
// *************************************************************************** //
// *************************************************************************** //

//  "EndpointInfo"
template<typename PID>
struct EndpointInfo_t { PID path_idx; bool prepend; };   // prepend==true ↔ first vertex



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "MISC".






// *************************************************************************** //
//
//
//
//      1.      RENDERING UTILITIES...
// *************************************************************************** //
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
// *************************************************************************** //   END "RENDERING UTILITIES".












//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "MISC".

  















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

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






//  "CanvasDrawChannel"
//
enum class CanvasDrawChannel : uint8_t
{
    Grid = 0,               //  Grid-Lines, Guides, etc.
//
    Highlight,              //  selection glow, browser-hover glow
    Object,                 //  ENCLOSED-AREA / FILL for each object.
    Accent,                 //  Used for ACCENTUATING FEATURES (Top-Level "Points", etc).
    Glyph,                  //  Handles, Cursor-Hints, etc
//
    Top,                    //  TOP-MOST LAYER       (RESERVED).
//
    COUNT
};



//  "RenderCTX"
//
struct RenderCTX
{
    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    using                               Channel                         = CanvasDrawChannel;
    using                               CTX                             = RenderCTX;
    
    // *************************************************************************** //
    //      INTERNAL TYPES.
    // *************************************************************************** //

    struct Scope
    {
        using           Channel         = CanvasDrawChannel;
        using           CTX             = RenderCTX;
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
    inline                              RenderCTX               (void)                          = default;
    inline explicit                     RenderCTX               (ImDrawList * dl_)              { Begin(dl_); }
    inline                              ~RenderCTX              (void)                          { End(); }
    //
    //                              DELETED MEMBERS:
                                        RenderCTX               (const RenderCTX & )            = delete;
    RenderCTX &                         operator =              (const RenderCTX & )            = delete;
    


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
// *************************************************************************** //   END "MISC/UTILITY".












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

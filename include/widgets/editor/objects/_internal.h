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






//  "CanvasChannel"
//
enum class CanvasChannel : int
{
    Background = 0,         //  grid / guides
    Geometry,               //  paths/points (ordered by z_index inside)
    Highlights,             //  selection glow, browser-hover glow
    Gizmos,                 //  handles, cursor hints
    PlotTop,                //  must-be-top within the plot
//
    COUNT
};



//  "CanvasChannels"
//
struct CanvasChannels
{
    ImDrawList *        dl      = nullptr;
    ImDrawListSplitter  split   { };
    bool                active  = false;

    explicit CanvasChannels(ImDrawList * drawlist)
    : dl(drawlist)
    {
        split.Split(dl, static_cast<int>(CanvasChannel::COUNT));
        active = true;
    }

    ~CanvasChannels() { if (active) split.Merge(dl); }

    inline void set(CanvasChannel ch) {
        split.SetCurrentChannel(dl, static_cast<int>(ch));
    }

    struct Scope {
        CanvasChannels &    cm;
        int                 prev;
        Scope(CanvasChannels & mgr, CanvasChannel ch)
        : cm(mgr), prev(cm.split._Current) { cm.set(ch); }
        ~Scope() { cm.split.SetCurrentChannel(cm.dl, prev); }
    };
};






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

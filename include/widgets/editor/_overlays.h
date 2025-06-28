/***********************************************************************************
*
*       ********************************************************************
*       ****            _ O V E R L A Y S . H  ____  F I L E            ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      June 25, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBWIDGETS_EDITOR_OVERLAYS_H
#define _CBWIDGETS_EDITOR_OVERLAYS_H  1
   


//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
#include "utility/utility.h"
#include "widgets/editor/_types.h"

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
#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_internal.h"
#include "implot.h"
#include "implot_internal.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //





//  1.  STATIC / CONSTEXPR / CONSTANTS...
// *************************************************************************** //
// *************************************************************************** //






// *************************************************************************** //
// *************************************************************************** //
//                         Overlay:
//                 Overlay Widget for "Editor" Class.
// *************************************************************************** //
// *************************************************************************** //

class OverlayManager {
public:
        using                   OverlayID                       = uint32_t;
        using                   Overlay                         = Overlay_t<OverlayID>;
//
        using                   EndpointInfo                    = EndpointInfo;
//      CBAPP_APPSTATE_ALIAS_API        //  CLASS-DEFINED, NESTED TYPENAME ALIASES.
// *************************************************************************** //
//
//
//      1.          PUBLIC FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
public:
    OverlayID               create_overlay          (const OverlayCFG & cfg);
    void                    destroy_overlay         (OverlayID );
    //
    void                    Begin                   (const std::function<ImVec2(ImVec2)> & world_to_px, ImVec2 cursor_px, const ImRect& canvas_rect);


// *************************************************************************** //
//
//
//      2.          PROTECTED FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
protected:
    void                    _render_overlay         (Overlay & ov, const std::function<ImVec2(ImVec2)> & ,
                                                     ImVec2 cursor_px, const ImRect & );
    [[nodiscard]] std::pair<ImVec2, ImVec2>
                            _anchor_to_pos          (Overlay & ov, const std::function<ImVec2(ImVec2)> & ,
                                                     ImVec2 cursor_px, const ImRect & );
                        


// *************************************************************************** //
//
//
//      3.          PROTECTED DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //
private:
    OverlayID                   m_next_id           {1};
    std::vector<Overlay>        m_windows;




// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
};//    END "EDITOR" CLASS DEFINITION.












// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.






#endif      //  _CBWIDGETS_EDITOR_OVERLAYS_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.


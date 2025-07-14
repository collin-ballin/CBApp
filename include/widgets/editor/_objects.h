/***********************************************************************************
*
*       ********************************************************************
*       ****             _ O B J E C T S . H  ____  F I L E             ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      July 14, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBWIDGETS_EDITOR_OBJECTS_H
#define _CBWIDGETS_EDITOR_OBJECTS_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
#include "utility/utility.h"
#include "widgets/editor/_constants.h"

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



//      1.      ENUMS / UTILITIES...
// *************************************************************************** //
// *************************************************************************** //

//  "AnchorType"
//
enum class AnchorType : uint8_t     {
    Corner, Smooth, Symmetric,
    COUNT
};
//
static constexpr std::array<const char *, static_cast<size_t>(AnchorType::COUNT)>
ANCHOR_TYPE_NAMES                   = {
    "Corner",
    "Smooth",
    "Symmetric"
};
//
static constexpr std::array< ImColor, static_cast<size_t>(AnchorType::COUNT) >
ANCHOR_COLORS                       = { {
    /*  Corner          */      ImColor(255,       255,        0,      255     ),      //   yellow
    /*  Smooth          */      ImColor(  0,       255,        0,      255     ),      //   green
    /*  Symmetric       */      ImColor(  0,       200,        255,    255     )       //   cyan
} };


//  "EndpointInfo"
//
template<typename PID>
struct EndpointInfo_t { PID path_idx; bool prepend; };   // prepend==true ↔ first vertex



// *************************************************************************** //
// *************************************************************************** //   END "Enums".







// *************************************************************************** //
//
//
//
//      2.      [DATA LAYER]    VERTICES...
// *************************************************************************** //
// *************************************************************************** //

//  "Vertex_t" / "Pos" / "Anchor Point"
//
//      A pure geometry node. `in_handle` and `out_handle` are offsets from
//      `pos`; when they are both zero the segment attached to this vertex is
//      straight. Handles live in world‑space units.
//
template<typename VID>
struct Vertex_t {
    static constexpr const char *   ms_DEF_VERTEX_LABEL_FMT_STRING      = "Vertex V%03d (id=%u)";
    static constexpr const char *   ms_DEF_VERTEX_FMT_STRING            = "V%03u";
//
    uint32_t    id              = 0;
    float       x               = 0.0f,
                y               = 0.0f;
    //
    ImVec2      in_handle       = ImVec2(0.0f, 0.0f);   // incoming Bézier handle (from previous vertex)
    ImVec2      out_handle      = ImVec2(0.0f, 0.0f);   // outgoing Bézier handle (to next vertex)
    AnchorType  kind            = AnchorType::Corner;
};
//
//  "to_json"
template <typename IdT>
inline void to_json(nlohmann::json& j, const Vertex_t<IdT>& v)
{
    j = {
        { "id",          v.id },
        { "x",           v.x  },
        { "y",           v.y  },
        { "in_handle",   { v.in_handle.x,  v.in_handle.y } },
        { "out_handle",  { v.out_handle.x, v.out_handle.y } },
        { "kind",        v.kind }
    };
}
//
//  "from_json"
template <typename IdT>
inline void from_json(const nlohmann::json& j, Vertex_t<IdT>& v)
{
    j.at("id")  .get_to(v.id);
    j.at("x")   .get_to(v.x);
    j.at("y")   .get_to(v.y);
    auto ih = j.at("in_handle");  v.in_handle  = { ih[0], ih[1] };
    auto oh = j.at("out_handle"); v.out_handle = { oh[0], oh[1] };
    j.at("kind").get_to(v.kind);
}

// *************************************************************************** //
// *************************************************************************** //   END "Vertices".






// *************************************************************************** //
//
//
//
//      3.      [DATA LAYER]    POINTS...
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
inline void to_json(nlohmann::json& j, const PointStyle& s)
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
inline void to_json(nlohmann::json& j, const Point_t<PtID>& p)
{
    j = { { "v", p.v },
          { "sty", p.sty } };
}
//
//  "from_json"
template <typename PtID>
inline void from_json(const nlohmann::json& j, Point_t<PtID>& p)
{
    j.at("v"  ).get_to(p.v  );
    j.at("sty").get_to(p.sty);
}

// *************************************************************************** //
// *************************************************************************** //   END "POINTS".






// *************************************************************************** //
//
//
//
//      4.      [DATA LAYER]    LINES...
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

// *************************************************************************** //
// *************************************************************************** //   END "LINES".






// *************************************************************************** //
//
//
//
//      5.      [DATA LAYER]    PATH EXTENSIONS...
// *************************************************************************** //
// *************************************************************************** //






// *************************************************************************** //
//
//
//
//      PATH EXTENSIONS...
namespace path { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //

enum class PathKind : uint8_t {
    Default         = 0,
    Dielectric      = 1,
    // Boundary   = 2,
    // Source     = 3,
    // … add more as needed
};


//
struct DielectricExtra {
    double                eps_r {1.0};          // relative permittivity
    double                mu_r  {1.0};          // relative permeability
    std::array<double,2>  n     {1.0, 0.0};     // complex n = n + i·k
};

// using ExtraData = std::variant<std::monostate, DielectricExtra /*, BoundaryExtra, SourceExtra*/>;
                               


// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "path" NAMESPACE.

// *************************************************************************** //
// *************************************************************************** //   END "Path Extensions".






// *************************************************************************** //
//
//
//
//      6.      [DATA LAYER]    CORE PATH OBJECTS...
// *************************************************************************** //
// *************************************************************************** //

//  "PathStyle"
//
struct PathStyle {
    ImU32 stroke_color = IM_COL32(255,255,0,255);
    ImU32 fill_color   = IM_COL32(255,255,255,0);   // default: transparent white
    float stroke_width = 2.0f;
};


//  "to_json"
//
inline void to_json(nlohmann::json& j, const PathStyle& s)
{
    j = nlohmann::json{
        { "stroke_color", s.stroke_color },
        { "fill_color",   s.fill_color   },
        { "stroke_width", s.stroke_width }
    };
}


//  "from_json"
//
inline void from_json(const nlohmann::json& j, PathStyle& s)
{
    j.at("stroke_color").get_to(s.stroke_color);
    j.at("fill_color"  ).get_to(s.fill_color  );
    j.at("stroke_width").get_to(s.stroke_width);
}



//  "Path_t"  -- (polyline / spline / area).
//
//      A sequence of vertex IDs. `closed == true` means the last vertex connects
//      back to the first. Vertices carry their own Bézier handles; this struct
//      only stores ordering and style.
//
template<typename PID, typename VID, typename ZID>
struct Path_t {
// *************************************************************************** //
// *************************************************************************** //
    static constexpr size_t         ms_MAX_PATH_LABEL_LENGTH    = 64;
//
//
//      1.  MEMBER FUNCTIONS...
// *************************************************************************** //
    [[nodiscard]] inline bool       is_area             (void) const noexcept
    { return this->closed && this->verts.size() >= 3; }
//
    [[nodiscard]] inline bool       is_mutable          (void) const noexcept
    { return visible && !locked; }
//
    inline void                     set_label           (const char * src) noexcept
    { this->label = std::string(src); this->_truncate_label(); }
//
    inline void                     set_default_label   (const PID id_) noexcept
    { this->id = id_;   this->label = std::format("Path {:03}", id_);   this->_truncate_label(); }
//
    inline void                     _truncate_label     (void)
    { if (this->label.size() > ms_MAX_PATH_LABEL_LENGTH) { this->label.resize( ms_MAX_PATH_LABEL_LENGTH ); } }
//
//
//
    std::vector<VID>    verts;   // ordered anchor IDs
    PID                 id                                      = 0;
    bool                closed                                  = false;
    PathStyle           style                                   = PathStyle();
//
// ─────────── NEW ───────────
    ZID                 z_index                                 = Z_FLOOR_USER;
    bool                locked                                  = false;
    bool                visible                                 = true;
    std::string         label                                   = "";

// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
};//    END "Path_t" CLASS DEFINITION.


//  "to_json"
//
template<typename PID, typename VID, typename ZID>
inline void to_json(nlohmann::json & j, const Path_t<PID, VID, ZID> & p)
{
    j = { { "verts",        p.verts         },
          { "id",           p.id            },
          { "closed",       p.closed        },
          { "style",        p.style         },
          { "z_index",      p.z_index       },
          { "locked",       p.locked        },
          { "visible",      p.visible       },
          { "label",        p.label         } };
    return;
}


//  "from_json"
//
template<typename PID, typename VID, typename ZID>
inline void from_json(const nlohmann::json & j, Path_t<PID, VID, ZID> & p)
{
    if ( j.contains("id") )         { j.at("id").get_to(p.id);      }
    else                            { p.id = 0;                     }
    
    j.at("verts"    ).get_to(p.verts    );
    j.at("closed"   ).get_to(p.closed   );
    j.at("style"    ).get_to(p.style    );
    j.at("z_index"  ).get_to(p.z_index  );
    j.at("locked"   ).get_to(p.locked   );
    j.at("visible"  ).get_to(p.visible  );
    
    //  Handle old files that may lack "label"
    if ( j.contains("label") )      { j.at("label").get_to(p.label); }
    else                            { p.set_label("?"); }
        
    return;
}




// *************************************************************************** //
// *************************************************************************** //   END "PATH".

  















// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.












#endif      //  _CBWIDGETS_EDITOR_OBJECTS_H  //
// *************************************************************************** //
// *************************************************************************** //   END FILE.

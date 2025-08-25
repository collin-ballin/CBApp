/***********************************************************************************
*
*       ********************************************************************
*       ****               _ T Y P E S . H  ____  F I L E               ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      June 13, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBWIDGETS_EDITOR_TYPES_H
#define _CBWIDGETS_EDITOR_TYPES_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
#include "utility/utility.h"
#include "widgets/editor/_constants.h"
#include "widgets/editor/_objects.h"

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






// *************************************************************************** //
//
//
//
//  1.  [TYPE/ENUM LAYER]   TYPES AND ENUMERATIONS THAT WE USE FOR THE EDITOR...
// *************************************************************************** //
// *************************************************************************** //

//  "Mode"
//      - Enum type for each "tool" that we have in the application.
//
enum class Mode : int {
    Default = 0,        //  "Select"                (KEY: "V").
    Hand,               //  "Hand"                  (KEY: "H").
    Line,
    Point,              //  "Point"                 (KEY: "N").
    Pen,                //  "Pen"                   (KEY: "P").
    Scissor,            //  "Scissors"              (KEY: "C").
    Shape,              //  "Scissors"              (KEY: "C").
    AddAnchor,          //  "Add Anchor"            (KEY: "+").
    RemoveAnchor,       //  "Remove Anchor"         (KEY: "-").
    EditAnchor,         //  "Edit Anchor"           (KEY: "SHIFT" + "C").
//
    COUNT
};
//
//  "DEF_EDITOR_STATE_NAMES"
static constexpr std::array<const char *, static_cast<size_t>(Mode::COUNT)>
    DEF_EDITOR_STATE_NAMES  = { {
        "Default",                      "Hand",                     "Line",                 "Point",
        "Pen",                          "Scissor",                  "Shape",                "Add Anchor Point",
        "Remove Anchor Point",          "Edit Anchor"
} };






//  "Capability"
//      -Bitfield that defines what behaviors/capabilities each Editor State (Mode) has.
//
enum CBCapabilityFlags_ : uint64_t {
    CBCapabilityFlags_None                      = 0,                    //  No special behaviours...
//
    CBCapabilityFlags_Pan                       = 1u << 0,              //  Space + drag navigation
    CBCapabilityFlags_Zoom                      = 1u << 1,              //  mouse-wheel magnification
    CBCapabilityFlags_Select                    = 1u << 2,              //  click / lasso / move-drag
    CBCapabilityFlags_CursorHint                = 1u << 3,              //  hand or resize cursor
    CBCapabilityFlags_EnableMutableHotkeys      = 1u << 4,              //  Disables hotkeys that alter objects (like [ DEL ]).
    CBCapabilityFlags_EnableAdvancedHotkeys     = 1u << 5,              //  Enable EXTRA hotkeys (CMD+J to join, etc)
//
//
    CBCapabilityFlags_COUNT,                                            //  helper: number of bits
//
//
//
//  GROUPS OF FLAGS...
    CBCapabilityFlags_Navigation                = CBCapabilityFlags_Pan | CBCapabilityFlags_Zoom,
//
//
//  PRESETS...
    CBCapabilityFlags_ReadOnly                  = CBCapabilityFlags_Navigation,
//
    CBCapabilityFlags_Plain                     = CBCapabilityFlags_Navigation,
    CBCapabilityFlags_Default                   = CBCapabilityFlags_Plain | CBCapabilityFlags_CursorHint,
    CBCapabilityFlags_More                      = CBCapabilityFlags_Default | CBCapabilityFlags_Select | CBCapabilityFlags_EnableMutableHotkeys,
//
    CBCapabilityFlags_All                       = CBCapabilityFlags_More | CBCapabilityFlags_EnableAdvancedHotkeys
};
//
//
//
//
//  Per‑mode capability mask
static constexpr std::array< std::underlying_type_t<CBCapabilityFlags_>, static_cast<size_t>(Mode::COUNT)>
DEF_MODE_CAPABILITIES       = {
/*  Default             */      CBCapabilityFlags_All,
/*  Hand                */      CBCapabilityFlags_ReadOnly,
/*  Line                */      CBCapabilityFlags_Navigation | CBCapabilityFlags_CursorHint,
/*  Point               */      CBCapabilityFlags_Navigation | CBCapabilityFlags_Select | CBCapabilityFlags_CursorHint,
/*  Pen Tool,           */      CBCapabilityFlags_Navigation | CBCapabilityFlags_CursorHint,
/*  Scisssor Tool       */      CBCapabilityFlags_Plain,
/*  Shape               */      CBCapabilityFlags_Plain,
/*  Add Anchor          */      CBCapabilityFlags_Plain,
/*  Remove Anchor       */      CBCapabilityFlags_Plain,
/*  Edit Anchor         */      CBCapabilityFlags_Plain
};






//  "ObjectTrait"
//
enum class ObjectTrait : int {
    Properties = 0,         //  "Properties"    -- Fill color, line thickness, etc.
    Vertices,               //  "Vertices"      -- Bezier Control Points, etc...
    Payload,                //  "Payload"       -- Auxiliary Data.
//
    COUNT
};
//
//  "DEF_OBJECT_TRAIT_NAMES"
//static constexpr std::array<const char *, static_cast<size_t>(ObjectTrait::COUNT)>
static constexpr cblib::EnumArray< ObjectTrait, const char * >
    DEF_OBJECT_TRAIT_NAMES  = { {
        "Properties",       "Vertices",         "Payload"
} };




// *************************************************************************** //
//      SECONDARY ENUM TYPES...
// *************************************************************************** //

//  "IOResult"
//
enum class IOResult {
    Ok = 0,
    IoError,
    ParseError,
    VersionMismatch,
//
    COUNT
};
//
//  "DEF_IORESULT_NAMES"
static constexpr std::array<const char *, static_cast<size_t>(IOResult::COUNT)>
    DEF_IORESULT_NAMES  = { {
        "OK",   "IO Error",     "Parsing Error",    "Version Mismatch"
} };






// *************************************************************************** //
//
//
//
//      2.3.    HIT DETECTION.
// *************************************************************************** //
// *************************************************************************** //

//  "Hit_t"
//
template <typename HID>
struct Hit_t {
    enum class Type { Point, Line, Path, Handle };
    Type            type            = Type::Point;
    size_t          index           = 0;     // Point/Line/Path: original meaning
    bool            out             = false; // valid only when type == Handle
};


//  "PathHit_t"
//
template <typename PID, typename VID>
struct PathHit_t {
    size_t          path_idx        = 0;    // which Path in m_paths
    size_t          seg_idx         = 0;    // segment i   (verts[i] → verts[i+1])
    float           t               = 0.f;  // param along that segment
    ImVec2          pos_ws          {};     // exact split position (world-space)
};

// *************************************************************************** //
// *************************************************************************** //   END "HIT DETECTION"






// *************************************************************************** //
//
//
//      2.4.    INTERACTION INFORMATION/STATES.
// *************************************************************************** //
// *************************************************************************** //

//  "GridSettings"
//
struct GridSettings {
    float   snap_step       = 20.0f;   // quantize condition for grid-snapping.
    bool    visible         = true;
    bool    snap_on         = false;
};






// *************************************************************************** //
//              MENU FLAGS...
// *************************************************************************** //
    
//  "EditorPopupBits"
//
enum class EditorPopupBits : uint8_t {
    None = 0,
    Selection,
    Canvas,
    Browser,
    Settings,
    AskOkCancel,
//
    COUNT
};


//  "CBEditorPopupFlags_"
//
enum CBEditorPopupFlags_ : uint32_t {
    CBEditorPopupFlags_None                 = 0,               //  No popup / context menu is open
//
    CBEditorPopupFlags_Selection            = 1u << static_cast<unsigned>(  EditorPopupBits::Selection      ),
    CBEditorPopupFlags_Canvas               = 1u << static_cast<unsigned>(  EditorPopupBits::Canvas         ),
    CBEditorPopupFlags_Browser              = 1u << static_cast<unsigned>(  EditorPopupBits::Browser        ),
//
    CBEditorPopupFlags_Settings             = 1u << static_cast<unsigned>(  EditorPopupBits::Settings),
    CBEditorPopupFlags_AskOkCancel          = 1u << static_cast<unsigned>(  EditorPopupBits::AskOkCancel    ),
//
    CBEditorPopupFlags_COUNT                            // helper: number of bits
};
//
//      HELPERS...
static inline constexpr uint32_t                to_u        (CBEditorPopupFlags_    x)      { return static_cast<uint32_t>(x);              }
static inline constexpr CBEditorPopupFlags_     from_u      (uint32_t               x)      { return static_cast<CBEditorPopupFlags_>(x);   }
static inline constexpr uint32_t                bit_u       (size_t                 i)      { return (1u << static_cast<unsigned>(i));      }
//
//  "set_bit"
static inline void set_bit(CBEditorPopupFlags_ & m, size_t i, bool on)
{
	uint32_t    v   = to_u(m), b = bit_u(i);
	m               = from_u(on ? (v | b) : (v & ~b));
}


//  "EditorPopupWindowInfo"
//
struct EditorPopupInfo {
    const char *        uuid;
    const char *        name;
};



//  "DEF_EDITOR_POPUP_INFOS"
//
static constexpr std::array< EditorPopupInfo, static_cast<size_t>( EditorPopupBits::COUNT ) >
DEF_EDITOR_POPUP_INFOS      = { {
    /* None         */  { nullptr                           , "None"            },
    /* Selection    */  { "Editor_Selection_ContextMenu"    , "Selection"       },
    /* Canvas       */  { "Editor_Canvas_ContextMenu"       , "Canvas"          },
    /* Browser      */  { "Editor_Browser_ContextMenu"      , "Browser"         },
    /* Settings     */  { "Editor System Preferences"       , ""                },
    /* AskOkCancel  */  { nullptr                           , ""                }
} };


//  "k_valid_popup_bits"
constexpr uint32_t k_valid_popup_bits = []{
    constexpr size_t        N       = DEF_EDITOR_POPUP_INFOS.size();
    constexpr uint32_t      retval  = (N >= 32)
                                        ? 0xFFFF'FFFFu : ( (uint32_t{1} << N) - 1u );
    return retval;
}(    );


//  "popup_name_from_flag"
static inline const char * popup_name_from_flag(CBEditorPopupFlags_ single)
{
    uint32_t                    x           = to_u(single) & k_valid_popup_bits;
    
    if ( x == 0u || (x & (x - 1u)) )        { return nullptr; }                 // not single-bit
    
    unsigned                    i           = std::countr_zero(x);
    const EditorPopupInfo &     info        = DEF_EDITOR_POPUP_INFOS[i];
    return (info.uuid)      ? info.name    : nullptr;                          //     skip "None"
}

//  "GetMenuID"
static inline const char * GetMenuID(const EditorPopupBits & handle)
    { return DEF_EDITOR_POPUP_INFOS[ static_cast<size_t>( handle ) ].uuid; }





// *************************************************************************** //
//              INTERACTION...
// *************************************************************************** //

//  "EditorInteraction"
//
struct EditorInteraction
{
    using                           CBEditorPopupFlags              = CBEditorPopupFlags_;
    //
    //
//                              IMPLOT STATE:
                                    //  ...
//
//                              EDITOR STATE:
    //  bool                            empty_selection                 = false;
    //  bool                            single_obj_selection            = false;
//
//                              MENU STATE:
    CBEditorPopupFlags_             open_menus                      = CBEditorPopupFlags_None;
//
//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "EditorInteraction" INLINE CLASS DEFINITION.




//  "Interaction"
//
struct Interaction
{
// *************************************************************************** //
//                                  PREVIOUS DATA...
// *************************************************************************** //
    bool                            hovered                 {   };
    bool                            active                  {   };
    bool                            space                   {   };
//
//
//
//                              IMPLOT VARIABLES:
    ImVec2                          canvas                  {   };
    ImVec2                          origin                  {   };
    ImVec2                          tl                      {   };
//
//
//
//                              OTHER DATA:
    ImDrawList *                    dl                      {   };
//
//
//
//                              NEW DATA:
    EditorInteraction               obj                     {   };
//
//
//
// *************************************************************************** //
//                                  INLINE FUNCTIONS...
// *************************************************************************** //

    //  "BlockShortcuts"
    [[nodiscard]]
    inline bool                     BlockShortcuts              (void) const noexcept
    { return ( !hovered ); }
    //{ return ( (!hovered) || (obj.menus_open) ); }



// *************************************************************************** //
// *************************************************************************** //
};//	END "Interaction" INLINE CLASS DEFINITION.




// *************************************************************************** //
// *************************************************************************** //   END "INTERACTION STATES"






// *************************************************************************** //
//
//
//      SELECTION:
// *************************************************************************** //
// *************************************************************************** //

//  "Selection_t"
//
template<typename VID, typename PtID, typename LID, typename PID>
struct Selection_t {
    inline void                     clear           (void)          { vertices.clear(); points.clear(); lines.clear(); paths.clear();               }        // new
    inline bool                     empty           (void) const    { return vertices.empty() && points.empty() && lines.empty() && paths.empty();  }
    inline bool                     is_empty        (void) const    { return paths.empty();                                                         }
//
//
//
    std::unordered_set<uint32_t>    vertices                {};
    std::unordered_set<size_t>      points                  {};
    std::unordered_set<size_t>      lines                   {};
    std::unordered_set<size_t>      paths                   {};     // ← NEW
//
};
//
//  "to_json"
template<typename VID, typename PtID, typename LID, typename PID>
inline void to_json(nlohmann::json& j,
                    const Selection_t<VID,PtID,LID,PID>& s)
{
    j = { { "vertices",  std::vector<VID>  (s.vertices.begin(), s.vertices.end()) },
          { "points",    std::vector<PtID> (s.points  .begin(), s.points  .end()) },
          { "lines",     std::vector<LID>  (s.lines   .begin(), s.lines   .end()) },
          { "paths",     std::vector<PID>  (s.paths   .begin(), s.paths   .end()) } };
}
//
//  "from_json"
template<typename VID, typename PtID, typename LID, typename PID>
inline void from_json(const nlohmann::json& j,
                      Selection_t<VID,PtID,LID,PID>& s)
{
    std::vector<VID >  vs;  j.at("vertices").get_to(vs);
    std::vector<PtID>  ps;  j.at("points"  ).get_to(ps);
    std::vector<LID >  ls;  j.at("lines"   ).get_to(ls);
    std::vector<PID >  pa;  j.at("paths"   ).get_to(pa);

    s.vertices.clear();  s.vertices.insert(vs.begin(), vs.end());
    s.points  .clear();  s.points  .insert(ps.begin(), ps.end());
    s.lines   .clear();  s.lines   .insert(ls.begin(), ls.end());
    s.paths   .clear();  s.paths   .insert(pa.begin(), pa.end());
}

// *************************************************************************** //
// *************************************************************************** //   END "SELECTION".
    
    
    
    
    
    
    
    
    
    
    
    
// *************************************************************************** //
//
//
//
//      2.5.        TOOL STATE.
// *************************************************************************** //
// *************************************************************************** //

//  "PenState_t"
//
template<typename VID>
struct PenState_t {
    bool            active                  = false;
    size_t          path_index              = static_cast<size_t>(-1);
    VID             last_vid                = 0;

    bool            dragging_handle         = false;
    bool            dragging_out            = true;     // NEW: true → out_handle, false → in_handle
    VID             handle_vid              = 0;

    bool            prepend                 = false;
    bool            pending_handle          = false;        // waiting to see if user drags
    VID             pending_vid             = 0;            // vertex that may get a handle
    float           pending_time            = 0.0f;
    //int         pending_frames  = 0;        // NEW – counts frames since click
};




//  "ShapeKind"
//
enum class ShapeKind : uint32_t {
    Square,             Rectangle,
    Circle,             Oval,               Ellipse, /*, Polygon, Star, …*/
//
    COUNT
};
//
//  "APPLICATION_PLOT_COLOR_STYLE_NAMES"
//      COMPILE-TIME ARRAY CONTAINING THE NAME OF ALL STYLES.
static constexpr std::array<const char *, static_cast<size_t>(ShapeKind::COUNT)>
DEF_EDITOR_SHAPE_NAMES = {{
    "Square",           "Rectangle",
    "Circle",           "Oval",             "Ellipse"
}};


//  "ShapeState_t"
//
template<typename OID>
struct ShapeState_t {
    bool                active                  = false;            // true while user is click-dragging a preview
    OID                 overlay_id              = OID(0);           // contextual UI (0 ⇒ none)
    ShapeKind           kind                    = ShapeKind::Rectangle;
    float               radius                  = 25.0f;            // corner- or major-radius (placeholder)
    float               params[5]               = {0.0f};           // Array to hold multiple geometric descriptors for more complex shapes.
    //
    //
    //  Live-preview drag info (world-space)
    bool                dragging                = false;
    ImVec2              press_ws                {  };       // anchor at mouse-down
    ImVec2              cur_ws                  {  };       // current mouse pos each frame
    float               start_y                 = 0.0f;
    float               start_rad               = 0.0f;
};
//
//  "to_json"
template<typename OID>
inline void to_json(nlohmann::json & j, const ShapeState_t<OID> & obj)
{
    j = {
            { "kind",               obj.kind            },
            { "radius",             obj.radius          }
    };
    return;
}
//
//  "from_json"
template<typename OID>
inline void from_json(const nlohmann::json & j, ShapeState_t<OID> & o)
{
    j.at("kind"         ).get_to(o.kind         );
    j.at("radius"       ).get_to(o.radius       );
    return;
}




//  "BrowserState_t"
//
template<typename VID, typename PtID, typename LID, typename PID, typename ZID>
struct BrowserState_t {
// *************************************************************************** //
// *************************************************************************** //


    // *************************************************************************** //
    //      1.      NESTED TYPENAME ALIASES...
    // *************************************************************************** //
    using                       Path                                    = Path_t        <PID, VID, ZID>         ;
    using                       Vertex                                  = Vertex_t      <VID>                   ;

    // *************************************************************************** //
    //      2.      CONSTEXPR VALUES...
    // *************************************************************************** //
    static constexpr size_t     ms_MAX_PATH_TITLE_LENGTH                = Path::ms_MAX_PATH_LABEL_LENGTH + 64ULL;
    //
    static constexpr size_t     ms_MAX_VERTEX_TITLE_LENGTH              = Vertex:: ms_VERTEX_NAME_BUFFER_SIZE + 32ULL;

    // *************************************************************************** //
    //      3.      MEMBER FUNCTIONS...
    // *************************************************************************** //
    
    //  "reset"
    inline void                 reset(void) { this->clear(); }
    
    //  "clear"
    inline void                 clear(void)
    {
        //  Renaming "Active" State.
        m_renaming_layer            = false;
        m_renaming_obj              = false;


        //  Force filter to rebuild so the clipper sees an empty list.
        m_layer_filter              .Build();
        m_obj_filter                .Build();
        
        
        //  NEW STUFF...
        //
        //      (A)     LAYER.
            //  m_layer_rows                .clear();
            //  m_renaming_layer            = false;            //  "Dirty" Flags (to rebuild list of indices).
            //  m_layer_rows_paths_rev      = -1;
        //
        //      (B)     OBJECT.
        m_obj_rows                  .clear();
        m_renaming_obj              = false;            //  "Dirty" Flags (to rebuild list of indices).
        m_obj_rows_paths_rev        = -1;
        
        
        
        //  Rename Index.
        m_layer_rename_idx          = -1;
        m_obj_rename_idx            = -1;
        
        
          
          
          
        //  ??  Other  ??
        m_layer_browser_anchor      = -1;
        m_browser_anchor            = -1;
        m_inspector_vertex_idx      = -1;
        
        return;
    }
    
    // *************************************************************************** //


    // *************************************************************************** //
    //      3.      DATA MEMBERS...
    // *************************************************************************** //
    //                      MUTABLE STATE VARIABLES:
    bool                        m_show_default_properties                   = false;
    bool                        m_renaming_layer                            = false;    //  TRUE when user is in the middle of MODIFYING NAME.
    bool                        m_renaming_obj                              = false;    //
    //
    //
    //                      NEW STUFF:
        //      std::vector<int>            m_layer_rows                                {  };
        //      bool                        m_layer_filter_dirty                        = false;    //  Flag to queue Browser to RE-COMPUTE sorted items.
        //      int                         m_layer_rows_paths_rev                      = -1;
    //
    std::vector<int>            m_obj_rows                                  {  };
    bool                        m_obj_filter_dirty                          = false;    //
    int                         m_obj_rows_paths_rev                        = -1;
    //
    //
    //                      INDICES:
    int                         m_layer_browser_anchor                      = -1;       //  ?? currently selected LAYER ??
    int                         m_browser_anchor                            = -1;       //  ?? anchor index for Shift‑range select ??
    int                         m_inspector_vertex_idx                      = -1;       //  ...
    //
    //
    int                         m_layer_rename_idx                          = -1;       //  LAYER that is BEING RENAMED     (–1 = none)
    int                         m_obj_rename_idx                            = -1;       //  OBJECT that is BEING RENAMED    (–1 = none)
    //
    //
    //                      CACHE AND MISC. DATA:
    char                        m_name_buffer[ ms_MAX_PATH_TITLE_LENGTH ]   = {  };     //  scratch text
    //
    //
    //
    //                      OTHER DATA ITEMS:
    ImGuiTextFilter             m_layer_filter;                                         //  search box for "LAYER" browser.
    ImGuiTextFilter             m_obj_filter;                                           //  search box for "OBJECT" browser.
    
    
    
    // *************************************************************************** //

// *************************************************************************** //
// *************************************************************************** //   END "BrowserState_t".
};






//  "IndexState_t"
//
template<typename VID, typename PtID, typename LID, typename PID, typename ZID>
struct IndexState_t {
    VID                         next_vid                                = 0;    //  original:   "m_next_id"...
    PID                         next_pid                                = 0;    //  original:   "m_next_pid"...
//
    size_t                      m_paths_rev                             = 0ULL;
};






// *************************************************************************** //
// *************************************************************************** //   END "TOOL STATE"







// *************************************************************************** //
//
//
//
//      2.6.    SERIALIZATION STUFF...
// *************************************************************************** //
// *************************************************************************** //

//  "Clipboard_t"
//
template<typename Vertex, typename Point, typename Line, typename Path>
struct Clipboard_t {
    inline void                 clear       (void)                      { ref_ws = ImVec2(); vertices.clear(); points.clear(); lines.clear(); paths.clear(); return; }
    inline bool                 empty       (void) const noexcept       { return vertices.empty() && points.empty() && lines.empty() && paths.empty(); }
//
    ImVec2                      ref_ws                                  {  };       //  reference origin (top-left of bbox)
    std::vector<Vertex>         vertices;                                           //  geometry snapshots (id-field unused)
    std::vector<Point>          points;
    std::vector<Line>           lines;
    std::vector<Path>           paths;                                              //  verts[] hold vertex indices into vertices[]
};

// *************************************************************************** //
// *************************************************************************** //   END "SERIALIZATION"
  
  
  



// *************************************************************************** //
//
//
//
//      2.7.    OVERLAY STUFF...
// *************************************************************************** //
// *************************************************************************** //

//  "Resident"
//      Enums for each RESIDENT that will be default-assigned in the Editor class.
//
enum Resident: uint8_t {
    Debugger = 0,
    Selection,
    Shape,
//
    UITraits,
    UIObjects,
//
    COUNT
};


//  "BBoxAnchor"
//      Defiled in the order of unit circle angles (0deg = +x-axis) and DEFAULT = 0 = CENTER.
//
enum class BBoxAnchor : uint8_t {
    Center = 0,
    East, NorthEast, North, NorthWest, West, SouthWest, South, SouthEast,
//
    COUNT
};


//  "OverlayAnchor"
//
enum class OverlayAnchor : uint8_t {
    World = 0,      // anchor_ws interpreted in world-space → converts via world_to_pixels()
    Screen,         // anchor_ws is absolute screen coords (pixels)
    Cursor,         // anchor_ws is Δ offset from current cursor (pixels)
    COUNT
};


//  "OffscreenPolicy"
//
enum class OffscreenPolicy : uint8_t {
    Hide = 0,           // Overlay vanishes when anchor is outside canvas
    Clamp,              //  Overlay clamps to nearest edge (old behaviour)
    COUNT
};


//  "OverlayPlacement"
//
enum class OverlayPlacement : uint8_t {
    Custom = 0,         // anchor_px    = screen position (px)
    Cursor,             // anchor_px    = offset  (px)
    World,              // anchor_ws    = world‑space point
//
    CanvasPoint,
    CanvasTL,           // anchor_padpx = inset from top‑left  corner
    CanvasTR,           // anchor_padpx = inset from top‑right corner
    CanvasBL,           // anchor_padpx = inset from bot‑left  corner
    CanvasBR,           // anchor_padpx = inset from bot‑right corner
//
    CanvasLeft,
    CanvasRight,
    CanvasTop,
    CanvasBottom,
//
    COUNT
};


//  "OverlayWindowType"
//
enum class OverlayWindowType : uint8_t {
    Ephemeral = 0,      //  Ephemeral / Transient.
    Resident,
    Custom,
//
    COUNT
};



//  "OverlayCFG"
//
struct OverlayCFG {
    std::function<void()>       draw_fn;            //  Widgets callback.
    bool                        locked;             //  TRUE :  Allows user to modify Overlay settings (with right-click context menu).
//
    OverlayPlacement            placement           { OverlayPlacement::Custom };
    BBoxAnchor                  src_anchor          = BBoxAnchor::NorthWest;            //  top-centre of the window
    OffscreenPolicy             offscreen           { OffscreenPolicy::Clamp };         //  NEW
//
//
    ImVec2                      anchor_px           { 0.0f,     0.0f };         //  Pixel inset / offset
    //  ImGuiWindowFlags            override_flags      = ImGuiWindowFlags_None;
//
//
//  PRIVATE:
    ImVec2                      anchor_ws           { 0.0f,     0.0f };         //  World-space anchor
};


//  "OverlayStyle"
//
struct OverlayStyle {
    CBAPP_CBLIB_TYPES_API
//
    float                                   alpha                   = 0.65f;
    ImU32                                   bg                      = 0x000000FF;
    int                                     window_rounding         = 8;
//
    std::optional< Param<ImVec2> >          window_size             = { std::nullopt };      // {     {120.0f,    -1.0f},     { {80.0f,       1.0f},      {220.0f,    FLT_MAX} }   };
//
//
//
    //  Default Constructor.
    //  OverlayStyle                            (void)                                  = default;
      
    //  Deep Cooy Constructor.
    //OverlayStyle                            (const OverlayStyle & src)
    //    : alpha(src.alpha)
    //      , bg(src.bg)
    //      , window_rounding(src.window_rounding)
    //      , window_size( (src.window_size) ? std::make_unique< Param<ImVec2> >(*src.window_size) : nullptr )
    //{   }


    //  Move Constructor.
    //  OverlayStyle                            (OverlayStyle && ) noexcept             = default;
    
    
    //  Move Assignment Operator.
    //  OverlayStyle &           operator =      (OverlayStyle &&) noexcept             = default;
    
    
    //  "swap"
    friend void             swap            (OverlayStyle & src, OverlayStyle & dst) noexcept {
        using std::swap;
        swap    ( src.alpha,                dst.alpha               );  // intentional fallthrough to swap both
        swap    ( src.bg,                   dst.bg                  );
        swap    ( src.window_rounding,      dst.window_rounding     );
        
        return;
    }
//
//
//
};




//  "OverlayInfo_t"
//
template<typename OID = uint32_t>
struct OverlayInfo_t {
    using                                   OverlayID               = OID;
    using                                   WindowType              = OverlayWindowType;
//
//
    static constexpr ImGuiWindowFlags       ms_DEF_WINDOW_FLAGS     = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    static constexpr ImGuiWindowFlags       ms_CUSTOM_WINDOW_FLAGS  = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
//
//
    OverlayID                               id                      = 0;
    bool                                    visible                 = false;                 //     owner sets false to retire
    ImGuiWindowFlags                        flags                   = ms_DEF_WINDOW_FLAGS;
//
    std::string                             window_name             = {   };
    WindowType                              type                    = { WindowType::Ephemeral };
};



//  "Overlay_t"
//      MAIN "STATE" / "DATA" PACKET FOR EACH OVERLAY WINDOW...
//
template<typename OID = uint32_t>
struct Overlay_t {
    using                                   OverlayID               = OID;
    using                                   OverlayInfo             = OverlayInfo_t<OID>;
    using                                   WindowType              = OverlayWindowType;
//
    OverlayInfo                             info                    {   };
    OverlayCFG                              cfg                     {   };
    OverlayStyle                            style                   {   };
};






//  "OverlayState"
//      **DEPRECATED** > State for the OLD "HELPER" OVERLAY MENU / DEBUGGER MENU...
struct OverlayState {
    bool                open                = true;         // Editor toggles this; no “x” button
    bool                show_details        = true;         // extra diagnostics pane
    bool                verbose_detail      = false;        // extra diagnostics pane
    int                 location            = 3;            // 0-3 corners, −2 centre, −1 free
    float               alpha               = 0.65f;        // window translucency
    float               pad                 = 40.0f;        // margin from viewport edge
    ImGuiWindowFlags    base_flags          = ImGuiWindowFlags_NoDecoration       | ImGuiWindowFlags_NoDocking          | ImGuiWindowFlags_AlwaysAutoResize |
                                              ImGuiWindowFlags_NoSavedSettings    | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    ImGuiWindowFlags    flags               = 0;            // recomputed each frame
//
    std::string         log_msg;                            // last message (≤40 words)
    float               log_timer           = 0.0f;         // seconds until it disappears
};

// *************************************************************************** //
// *************************************************************************** //   END "OVERLAY"





    
// *************************************************************************** //
//
//
//
//      3.      OTHER STATE STRUCTURES.
// *************************************************************************** //
// *************************************************************************** //

//  "MoveDrag"
//
struct MoveDrag {
    bool                        active              = false;
    ImVec2                      anchor_ws           {0.0f,      0.0f};   // top-left of selection at mouse-press
    ImVec2                      press_ws            {0.0f,      0.0f};
    ImVec2                      cum_delta           {0.0f,      0.0f};   // accumulated world-space translation
    std::vector<uint32_t>       v_ids;               // selected vertex IDs
    std::vector<ImVec2>         v_orig;              // original positions (same order)
};


//  "BoxDrag"
//      struct (add fields for handle_ws0, orig_w, orig_h after mouse_ws0)
struct BoxDrag {
    bool                        active              = false;
    uint8_t                     handle_idx          = 0;
    ImVec2                      anchor_ws           = {0.0f,    0.0f};
    ImVec2                      bbox_tl_ws          = {0.0f,    0.0f};
    ImVec2                      bbox_br_ws          = {0.0f,    0.0f};
    std::vector<uint32_t>       v_ids;
    std::vector<ImVec2>         v_orig;
    ImVec2                      mouse_ws0           = {0.0f,    0.0f};
    ImVec2                      handle_ws0;                     // initial world‑space position of the dragged handle
    float                       orig_w              = 1.0f;          // original bbox width  (world units)
    float                       orig_h              = 1.0f;          // original bbox height (world units)
    bool                        first_frame         = true;
};


//  "Bounds"
//
struct Bounds {
    float min_x{0}, min_y{0}, max_x{0}, max_y{0};
};


//  "Camera"
//
struct Camera {
    ImVec2  pan             {0,0};      // world coords of viewport top-left
    float   zoom_mag        = 1.0f;     // 1 = fit, >1 zooms in, <1 zooms out

    // helpers --------------------------------------------------------
    inline float pixels_per_world(const ImVec2& view_sz, const Bounds& world) const {
        float w = world.max_x - world.min_x;
        float h = world.max_y - world.min_y;
        if (w <= 0) w = 1;              // avoid div-by-zero
        if (h <= 0) h = 1;
        float fit = std::min(view_sz.x / w, view_sz.y / h);
        return fit * zoom_mag;
    }
};





//  "DebuggerState_t"
//
template<typename VID, typename PtID, typename LID, typename PID, typename ZID>
struct DebuggerState_t
{
// *************************************************************************** //
//      0.      CONSTEXPR CONSTANTS...
// *************************************************************************** //
        static constexpr ImGuiChildFlags        ms_FLAGS        = ImGuiChildFlags_Borders;// | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY;


// *************************************************************************** //
//      1.      NESTED TYPES...
// *************************************************************************** //
    struct DebugItem {
        std::string                 uuid;
        bool                        open;
        ImGuiChildFlags             flags           = ms_FLAGS;
        std::function<void()>       render_fn       {   };
    };


// *************************************************************************** //
//      2.      DATA MEMBERS...
// *************************************************************************** //
    bool                            show_more_info      = false;
    std::vector<DebugItem>          windows             = {   };



// *************************************************************************** //
//      3.      MEMBER FUNCTIONS...
// *************************************************************************** //




// *************************************************************************** //
// *************************************************************************** //   END "DebuggerState_t"
};


  
// *************************************************************************** //
// *************************************************************************** //   END "OTHER STATES"











// *************************************************************************** //
//
//
//
//      99.     EDITOR SETTINGS / CONFIGS...
// *************************************************************************** //
// *************************************************************************** //

//  "EditorState_t"
//
template<typename VID, typename PtID, typename LID, typename PID, typename ZID>
struct EditorState_t
{
// *************************************************************************** //
//          OVERALL STATE...
// *************************************************************************** //
    ImPlotInputMap                      m_backup;
// *************************************************************************** //
//
//
//
// *************************************************************************** //
//          TRANSIENT STATE...
// *************************************************************************** //
    //
    //                              OVERALL STATE / ENABLED BEHAVIORS:
    bool                                m_show_grid                     = true;
    bool                                m_show_debug_overlay            = true;     //  Persistent/Resident Overlays.
    //
    bool                                m_show_ui_traits_overlay        = true;     //  UI-Overlays
    bool                                m_show_ui_objects_overlay       = true;
    //
    //
    //
    //                              TRANSIENT OBJECTS:
    ImPlotRect                          m_plot_limits                   = {   };
    //
    //                              TRANSIENT STATE:
    //
    //
    //
    //                              UTILITY:
    float                               m_bar_h                         = 0.0f;
    ImVec2                              m_avail                         = ImVec2(0.0f,      0.0f);
    ImVec2                              m_p0                            = ImVec2(0.0f,      0.0f);
    ImVec2                              m_p1                            = ImVec2(0.0f,      0.0f);
    
// *************************************************************************** //
//
//
//
// *************************************************************************** //
//          TOOL STATE (TODO: RE-HOME THESE INTO TOOL STATE OBJs)...
// *************************************************************************** //
    //
    //                              LASSO TOOL / SELECTION:
    bool                                m_show_sel_overlay              = false;
    //
    bool                                m_dragging                      = false;
    bool                                m_lasso_active                  = false;
    bool                                m_pending_clear                 = false;    //  pending click selection state ---
    //
    ImVec2                              m_lasso_start                   = ImVec2(0.f,       0.f);
    ImVec2                              m_lasso_end                     = ImVec2(0.f,       0.f);
    VID                                 m_next_id                       = 1;
    PID                                 m_next_pid                      = 1;        // counter for new path IDs
    //
    //
    //
    //                              PEN-TOOL STATE:
    bool                                m_drawing                       = false;
    bool                                m_dragging_handle               = false;
    bool                                m_dragging_out                  = true;
    VID                                 m_drag_vid                      = 0;
    
// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                                      MISC STATE...
// *************************************************************************** //

                                        //
                                        //  ...
                                        //

// *************************************************************************** //
//
//
//
// *************************************************************************** //
//          SERIALIZATION STUFF...
// *************************************************************************** //
    std::mutex                          m_task_mtx;
    std::vector<std::function<void()>>  m_main_tasks;
    std::atomic<bool>                   m_io_busy                       {false};
    IOResult                            m_io_last                       {IOResult::Ok};
    std::string                         m_io_msg                        {   };
    //
    std::atomic<bool>                   m_sdialog_open                  = {false};
    std::atomic<bool>                   m_odialog_open                  = {false};
    std::filesystem::path               m_filepath                      = {"../../assets/.cbapp/presets/editor/testing/editor-fdtd_v0.json"};   //    {"../../assets/.cbapp/presets/editor/testing/editor-fdtd_v0.json"};
    //  std::filesystem::path               m_filepath                      = {"../../assets/.cbapp/presets/editor/testing/editor-fdtd_v0.json"};
    
// *************************************************************************** //



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "EditorState"
};
//
//
//
//      EditorState     : "to_json"
//
template<typename VID, typename PtID, typename LID, typename PID, typename ZID>
inline void to_json(nlohmann::json & j, const EditorState_t<VID, PtID, LID, PID, ZID> & obj)
{
    //  j = {
    //      { "Z_EDITOR_BACK",          obj.Z_EDITOR_BACK       },
    //      { "Z_FLOOR_USER",           obj.Z_FLOOR_USER        },
    //      { "Z_EDITOR_FRONT",         obj.Z_EDITOR_FRONT      },
    //      { "Z_CEIL_USER",            obj.Z_CEIL_USER         },
    //      { "RENORM_THRESHOLD",       obj.RENORM_THRESHOLD    }
    //  };
    
    return;
}
//
//
//
//      EditorState     : "from_json"
//
template<typename VID, typename PtID, typename LID, typename PID, typename ZID>
inline void from_json(nlohmann::json & j, EditorState_t<VID, PtID, LID, PID, ZID> & obj)
{

    //  j.at("")        .get_to(obj.m_plot_limits);

    //  j.at("Z_EDITOR_BACK")           .get_to(obj.Z_EDITOR_BACK);
    //  j.at("Z_FLOOR_USER")            .get_to(obj.Z_FLOOR_USER);
    //  j.at("Z_EDITOR_FRONT")          .get_to(obj.Z_EDITOR_FRONT);
    //  j.at("Z_CEIL_USER")             .get_to(obj.Z_CEIL_USER);
    //  j.at("RENORM_THRESHOLD")        .get_to(obj.RENORM_THRESHOLD);
    
    return;
}






//  "BrowserStyle"
//
struct BrowserStyle
{
    CBAPP_CBLIB_TYPES_API
    
// *************************************************************************** //
//                                      BROWSER WINDOW STYLE...
// *************************************************************************** //
    //
    //                              BROWSER CHILD-WINDOW FLAGS:
    ImGuiChildFlags                     DYNAMIC_CHILD_FLAGS                         = ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX;
    ImGuiChildFlags                     STATIC_CHILD_FLAGS                          = ImGuiChildFlags_Borders;
    //
    //                              BROWSER CHILD-WINDOW DIMENSIONS:
    float                               ms_CHILD_WINDOW_SAMELINE                    = 4.0f;
    //
    Param<ImVec2>                       TRAIT_SELECTOR_DIMS                         = {     {120.0f,    -1.0f},     { {80.0f,       1.0f},      {220.0f,    FLT_MAX} }   };
    //
    Param<ImVec2>                       OBJ_SELECTOR_DIMS                           = {     {300.0f,    -1.0f},     { {250.0f,      1.0f},      {450.0f,    FLT_MAX} }   };
    Param<ImVec2>                       OBJ_PROPERTIES_INSPECTOR_DIMS               = {     {650.0f,    -1.0f},     { {600.0f,      1.0f},      {1200.0f,   FLT_MAX} }   };
    Param<ImVec2>                       VERTEX_SELECTOR_DIMS                        = {     {110.0f,    -1.0f},     { {90.0f,       1.0f},      {180.0f,    FLT_MAX} }   };
    //
    //
    //
    //                              BROWSER CHILD-WINDOW COLORS:
    ImVec4                              ms_TRAIT_INSPECTOR_FRAME_BG                 = ImVec4(0.188f,    0.203f,     0.242f,     0.750f);
    //
    ImVec4                              ms_VERTEX_SELECTOR_FRAME_BG                 = ImVec4(0.188f,    0.203f,     0.242f,     0.750f);
    ImVec4                              ms_VERTEX_INSPECTOR_FRAME_BG                = ImVec4(0.129f,    0.140f,     0.168f,     0.800f);
    //
    //
    //
    //
    //
    ImVec4                              ms_CHILD_FRAME_BG1                          = ImVec4(0.205f,    0.223f,     0.268f,     1.000f);//      ms_CHILD_FRAME_BG1      //   BASE = #343944
    ImVec4                              ms_CHILD_FRAME_BG1L                         = ImVec4(0.091f,    0.099f,     0.119f,     0.800f);//      ms_CHILD_FRAME_BG1L     //   #17191E
    ImVec4                              ms_CHILD_FRAME_BG1R                         = ImVec4(0.141f,    0.141f,     0.141f,     1.000f); // ImVec4(0.091f,    0.099f,     0.119f,     0.800f);//      ms_CHILD_FRAME_BG1R     //   #21242B
    
    ImVec4                              ms_CHILD_FRAME_BG2                          = ImVec4(0.149f,    0.161f,     0.192f,     1.000f);//      ms_CHILD_FRAME_BG2      // BASE = #52596B
    ImVec4                              ms_CHILD_FRAME_BG2L                         = ImVec4(0.188f,    0.203f,     0.242f,     0.750f);//      ms_CHILD_FRAME_BG2L     // ##353A46
    ImVec4                              ms_CHILD_FRAME_BG2R                         = ImVec4(0.129f,    0.140f,     0.168f,     0.800f); // ImVec4(0.250f,    0.271f,     0.326f,     0.750f);//      ms_CHILD_FRAME_BG2R     // #5B6377
    //
    //                              NEW COLORS:
    ImVec4                              ms_OBJ_INSPECTOR_FRAME_BG                   = ImVec4(0.129f,    0.140f,     0.168f,     0.800f);//      ms_CHILD_FRAME_BG1      //   BASE = #343944
    //
    //
    //                              BROWSER CHILD-WINDOW STYLE:
    float                               ms_VERTEX_SUBBROWSER_HEIGHT                 = 0.85f;    //  ms_VERTEX_SUBBROWSER_HEIGHT
    float                               ms_CHILD_BORDER1                            = 2.0f;     //  ms_CHILD_BORDER1
    float                               ms_CHILD_BORDER2                            = 1.0f;     //  ms_CHILD_BORDER2
    float                               ms_CHILD_ROUND1                             = 8.0f;     //  ms_CHILD_ROUND1
    float                               ms_CHILD_ROUND2                             = 4.0f;     //  ms_CHILD_ROUND2
    //
    //                              BROWSER CHILD-WINDOW DIMENSIONS:
    float                               OBJ_PROPERTIES_REL_WIDTH                    = 0.5f;     // Relative width of OBJECT PROPERTIES PANEL.
    float                               VERTEX_SELECTOR_REL_WIDTH                   = 0.075f;   // Rel. width of Vertex SELECTOR COLUMN.
    float                               VERTEX_INSPECTOR_REL_WIDTH                  = 0.0f;     // Rel. width of Vertex INSPECTOR COLUMN.

// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                                      GENERAL WIDGET STUFF...
// *************************************************************************** //
    //                              BROWSER **ALL** WIDGET STUFF:
    //  float                               ms_BROWSER_BUTTON_SEP                       = 8.0f;
    //  float                               ms_BROWSER_SELECTABLE_SEP                   = 16.0f;
    std::pair<float,float>              ms_BROWSER_ITEM_SEPS                        = { 8.0f,   16.0f };
    //
    //                              BROWSER PATH WIDGET STUFF:
    float                               ms_BROWSER_OBJ_LABEL_WIDTH                  = 55.0f;
    float                               ms_BROWSER_OBJ_WIDGET_WIDTH                 = 256.0f;
    //
    //                              BROWSER VERTEX WIDGET STUFF:
    float                               ms_BROWSER_VERTEX_LABEL_WIDTH               = 75.0f;
    float                               ms_BROWSER_VERTEX_WIDGET_WIDTH              = 196.0f;

// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                                      SPECIFIC WIDGET STUFF...
// *************************************************************************** //
    //                              DELETE BUTTON:
    static constexpr const char *       ms_DELETE_BUTTON_HANDLE                     = "x";
    static constexpr ImU32              ms_DELETE_BUTTON_COLOR                      = cblib::utl::ColorConvertFloat4ToU32_constexpr( ImVec4(   1.000f,     0.271f,     0.227f,     0.500f      )  );
    static constexpr size_t             ms_ACTION_DESCRIPTION_LIMIT                 = 256ULL;
    //
    //                              DRAG/DROP STUFF:
    static constexpr const char *       ms_DRAG_HANDLE_ICON                         = "=";

//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "DATA MEMBERS".






// *************************************************************************** //
//
//
//      2.C         INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      1.      UTILITY FUNCTIONS...
    // *************************************************************************** //
    
    //
    //  ...
    //
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.
    


// *************************************************************************** //
// *************************************************************************** //   END "EditorBrowserStyle"
};






//  "EditorStyle"
//
struct EditorStyle
{
// *************************************************************************** //
//                              STATIC CONSTEXPR CONSTANTS...
// *************************************************************************** //
    static constexpr size_t             DEF_HISTORY_CAP                             = 64;
    
// *************************************************************************** //
//                              SUBSIDIARY STYLE OBJECTS...
// *************************************************************************** //
    BrowserStyle                        browser_style                               = { };
    
    
    
// *************************************************************************** //
// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                              INTERACTION / RESPONSIVENESS CONSTANTS...
// *************************************************************************** //
    float                       GRID_STEP                       = 64.0f;
    float                       HIT_THRESH_SQ                   = 6.0f * 6.0f;
    float                       ms_ZOOM_RATE                    = 0.060f;
//
    int                         PEN_DRAG_TIME_THRESHOLD         = 0.05;                             //  PEN_DRAG_TIME_THRESHOLD         // seconds.
    float                       PEN_DRAG_MOVEMENT_THRESHOLD     = 4.0f;                             //  PEN_DRAG_MOVEMENT_THRESHOLD     // px  (was 2)
// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                              STANDARDIZED STYLE...
// *************************************************************************** //
    ImU32                       COL_POINT_DEFAULT               = IM_COL32(0,255,0,255);        // idle green
    ImU32                       COL_POINT_HELD                  = IM_COL32(255,100,0,255);      // while dragging
    ImU32                       COL_SELECTION_OUT               = IM_COL32(255,215,0,255);      // gold outline
    float                       DEFAULT_POINT_RADIUS            = 12.0f;                        // px
// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                              CANVAS  | USER-INTERFACE CONSTANTS...
// *************************************************************************** //
//                          HANDLES:
    ImU32                       ms_HANDLE_COLOR                 = IM_COL32(255, 215, 0, 255);       //  ms_HANDLE_COLOR             //  gold
    ImU32                       ms_HANDLE_HOVER_COLOR           = IM_COL32(255, 255, 0, 255);       //  ms_HANDLE_HOVER_COLOR       //  yellow
    float                       ms_HANDLE_SIZE                  = 3.0f;
    float                       HANDLE_BOX_SIZE                 = 4.f;                              //  ms_HANDLE_SIZE              //  px half-side
// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                              CANVAS  | PREVIEW APPEARANCE CONSTANTS...
// *************************************************************************** //
//                          VERTICES:
    float                       VERTEX_PREVIEW_RADIUS           = 4.0f;
    ImU32                       VERTEX_PREVIEW_COLOR            = IM_COL32(255, 255, 0, 160);
// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                              TOOL CONSTANTS...
// *************************************************************************** //
//
//                          STANDARDIZED STYLE:
//
//                          PEN-TOOL ANCHORS:
//                              //  ...
//
//                          LASSO TOOL:
    ImU32                       COL_LASSO_OUT                   = IM_COL32(255,215,0,255); // gold outline
    ImU32                       COL_LASSO_FILL                  = IM_COL32(255,215,0,40);  // translucent fill
//
//                          BOUNDING BOX:
    ImU32                       SELECTION_BBOX_COL              = IM_COL32(0, 180, 255, 153);   //  cyan-blue
    float                       SELECTION_BBOX_TH               = 1.5f;
// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                              CURSOR CONSTANTS...
// *************************************************************************** //
//                          PEN-TOOL CURSOR STUFF:
    float                       PEN_RING_RADIUS                 = 6.0f;                             //  PEN_RING_RADIUS     [px]
    float                       PEN_RING_THICK                  = 1.5f;                             //  PEN_RING_THICK      [px]
    int                         PEN_RING_SEGMENTS               = 32;                               //  NUMBER OF SEGMENTS TO DRAW OUTER RING.
    float                       PEN_DOT_RADIUS                  = 2.0f;                             //  PEN_DOT_RADIUS      // px
    int                         PEN_DOT_SEGMENTS                = 16;                               //  NUMBER OF SEGMENTS TO DRAW INNER DOT.
    //
    //
    ImU32                       PEN_COL_NORMAL                  = IM_COL32(255,255,0,255);          //  PEN_COL_NORMAL      // yellow
    ImU32                       PEN_COL_EXTEND                  = IM_COL32(  0,255,0,255);          //  PEN_COL_EXTEND      // green
// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                              BROWSER STYLE...
// *************************************************************************** //
    //                      BROWSER CHILD-WINDOW COLORS:
    //      ImVec4                      ms_CHILD_FRAME_BG1              = ImVec4(0.205f,    0.223f,     0.268f,     1.000f);//      ms_CHILD_FRAME_BG1      //   BASE = #343944
    //      ImVec4                      ms_CHILD_FRAME_BG1L             = ImVec4(0.091f,    0.099f,     0.119f,     0.800f);//      ms_CHILD_FRAME_BG1L     //   #17191E
    //      ImVec4                      ms_CHILD_FRAME_BG1R             = ImVec4(0.129f,    0.140f,     0.168f,     0.800f);//      ms_CHILD_FRAME_BG1R     //   #21242B
    //
    //      ImVec4                      ms_CHILD_FRAME_BG2              = ImVec4(0.149f,    0.161f,     0.192f,     1.000f);//      ms_CHILD_FRAME_BG2      // BASE = #52596B
    //      ImVec4                      ms_CHILD_FRAME_BG2L             = ImVec4(0.188f,    0.203f,     0.242f,     0.750f);//      ms_CHILD_FRAME_BG2L     // ##353A46
    //      ImVec4                      ms_CHILD_FRAME_BG2R             = ImVec4(0.250f,    0.271f,     0.326f,     0.750f);//      ms_CHILD_FRAME_BG2R     // #5B6377
    //      //
    //      //                      BROWSER CHILD-WINDOW STYLE:
    //      float                       ms_VERTEX_SUBBROWSER_HEIGHT     = 0.85f;    //  ms_VERTEX_SUBBROWSER_HEIGHT
    //      float                       ms_CHILD_BORDER1                = 2.0f;     //  ms_CHILD_BORDER1
    //      float                       ms_CHILD_BORDER2                = 1.0f;     //  ms_CHILD_BORDER2
    //      float                       ms_CHILD_ROUND1                 = 8.0f;     //  ms_CHILD_ROUND1
    //      float                       ms_CHILD_ROUND2                 = 4.0f;     //  ms_CHILD_ROUND2
    //      //
    //      //                      BROWSER CHILD-WINDOW DIMENSIONS:
    //      float                       OBJ_PROPERTIES_REL_WIDTH        = 0.5f;     // Relative width of OBJECT PROPERTIES PANEL.
    //      float                       VERTEX_SELECTOR_REL_WIDTH       = 0.075f;   // Rel. width of Vertex SELECTOR COLUMN.
    //      float                       VERTEX_INSPECTOR_REL_WIDTH      = 0.0f;     // Rel. width of Vertex INSPECTOR COLUMN.
    //      //
    //      //                      BROWSER **ALL** WIDGET STUFF:
    //      float                       ms_BROWSER_BUTTON_SEP           = 8.0f;
    //      float                       ms_BROWSER_SELECTABLE_SEP       = 16.0f;
    //      //
    //      //                      BROWSER PATH WIDGET STUFF:
    //      float                       ms_BROWSER_OBJ_LABEL_WIDTH      = 55.0f;
    //      float                       ms_BROWSER_OBJ_WIDGET_WIDTH     = 256.0f;
    //      //
    //      //                      BROWSER VERTEX WIDGET STUFF:
    //      float                       ms_BROWSER_VERTEX_LABEL_WIDTH   = 55.0f;
    //      float                       ms_BROWSER_VERTEX_WIDGET_WIDTH  = 196.0f;

// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                              WIDGETS / UI-CONSTANTS / APP-APPEARANCE...
// *************************************************************************** //
    //                      SYSTEM PREFERENCES WIDGETS:
    float                       ms_SETTINGS_LABEL_WIDTH         = 196.0f;
    float                       ms_SETTINGS_WIDGET_WIDTH        = 256.0f;
    
// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                              RENDERING CONSTANTS...
// *************************************************************************** //
    //                      VERTEX RENDERING:
    int                         ms_VERTEX_NUM_SEGMENTS          = 12;
    //
    //                      BEZIER RENDERING:
    int                         ms_BEZIER_SEGMENTS              = 0;        //  ms_BEZIER_SEGMENTS
    int                         ms_BEZIER_HIT_STEPS             = 20;       //  ms_BEZIER_HIT_STEPS
    int                         ms_BEZIER_FILL_STEPS            = 24;       //  ms_BEZIER_FILL_STEPS
    
// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                              UTILITY...
// *************************************************************************** //
                                //
                                //  ...
                                //
                                
// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                              MISC. / UNKNOWN CONSTANTS (RELOCATED FROM CODE)...
// *************************************************************************** //
    float                       TARGET_PX                       = 20.0f;                    //     desired screen grid pitch"  | found in "_update_grid"
    float                       PICK_PX                         = 6.0f;                     //     desired screen grid pitch"  | found in "_update_grid"

// *************************************************************************** //



// *************************************************************************** //
// *************************************************************************** //   END "EditorStyle"
};






//  "ZOrderCFG_t"
//
template<typename ZID>
struct ZOrderCFG_t {
    ZID         Z_EDITOR_BACK               = 1;                        //  Grid / background
    ZID         Z_FLOOR_USER                = 255;                      //  First user layer
    ZID         Z_EDITOR_FRONT              = UINT32_MAX - 2;           //  Overlays, guides
    ZID         Z_CEIL_USER                 = Z_EDITOR_FRONT - 1;       //  Max allowed for user items
    ZID         RENORM_THRESHOLD            = 10'000;                   //  Span triggering re-pack
};
//
//  "to_json"
template <typename ZID>
inline void to_json(nlohmann::json & j, const ZOrderCFG_t<ZID> & obj) {
    j = {
        { "Z_EDITOR_BACK",          obj.Z_EDITOR_BACK       },
        { "Z_FLOOR_USER",           obj.Z_FLOOR_USER        },
        { "Z_EDITOR_FRONT",         obj.Z_EDITOR_FRONT      },
        { "Z_CEIL_USER",            obj.Z_CEIL_USER         },
        { "RENORM_THRESHOLD",       obj.RENORM_THRESHOLD    }
    };
}
//
//  "from_json"
template <typename ZID>
inline void from_json(nlohmann::json & j, ZOrderCFG_t<ZID> & obj) {
    j.at("Z_EDITOR_BACK")           .get_to(obj.Z_EDITOR_BACK);
    j.at("Z_FLOOR_USER")            .get_to(obj.Z_FLOOR_USER);
    j.at("Z_EDITOR_FRONT")          .get_to(obj.Z_EDITOR_FRONT);
    j.at("Z_CEIL_USER")             .get_to(obj.Z_CEIL_USER);
    j.at("RENORM_THRESHOLD")        .get_to(obj.RENORM_THRESHOLD);
}
















// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.












#endif      //  _CBWIDGETS_EDITOR_TYPES_H  //
// *************************************************************************** //
// *************************************************************************** //   END FILE.

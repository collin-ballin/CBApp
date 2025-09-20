/***********************************************************************************
*
*       ********************************************************************
*       ****               _ T Y P E S . H  ____  F I L E               ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
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
//
#ifndef _CBAPP_EDITOR_OBJECTS_H
# include "widgets/editor/objects/objects.h"
#endif //  _CBAPP_EDITOR_OBJECTS_H  //

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
      Default = 0           //  "Select"                (KEY: "V").
    , Hand                  //  "Hand"                  (KEY: "H").
    , Pen                   //  "Pen"                   (KEY: "P").
    , Scissor               //  "Scissors"              (KEY: "C").
    , Shape                 //  "Scissors"              (KEY: "C").
    , AddAnchor             //  "Add Anchor"            (KEY: "+").
    , RemoveAnchor          //  "Remove Anchor"         (KEY: "-").
    , EditAnchor            //  "Edit Anchor"           (KEY: "SHIFT" + "C").
//
    , COUNT
};
//
//
//  "DEF_EDITOR_STATE_NAMES"
static constexpr cblib::EnumArray< Mode, const char * >
DEF_EDITOR_STATE_NAMES  = { {
      "Default"
    , "Hand"
    , "Pen"
    , "Scissor"
    , "Shape"
    , "Add Anchor"
    , "Remove Anchor"
    , "Edit Anchor"
} };
//
//
//  "DEF_EDITOR_STATE_ICONS"
static constexpr cblib::EnumArray< Mode, const char * >
DEF_EDITOR_STATE_ICONS  = { {
      ICON_FA_ARROW_POINTER
    , ICON_FA_HAND
    , ICON_FA_PEN_FANCY
    , ICON_FA_SCISSORS
    , ICON_FA_SHAPES
    , ICON_FA_PLUS
    , ICON_FA_MINUS
    , ICON_FA_PEN_NIB
} };



// *************************************************************************** //
//      EDITOR |        SELECTION BITS...
// *************************************************************************** //








// *************************************************************************** //
//      EDITOR |        CAPABILITY BITS...
// *************************************************************************** //

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
/*  Pen Tool,           */      CBCapabilityFlags_Navigation | CBCapabilityFlags_CursorHint,
/*  Scisssor Tool       */      CBCapabilityFlags_Plain,
/*  Shape               */      CBCapabilityFlags_Plain,
/*  Add Anchor          */      CBCapabilityFlags_Plain,
/*  Remove Anchor       */      CBCapabilityFlags_Plain,
/*  Edit Anchor         */      CBCapabilityFlags_None
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
//
//
//
//      3.      EDITOR "TOOL-STATE" INFORMATION...
// *************************************************************************** //
// *************************************************************************** //




//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "EDITOR TOOL STATE".






// *************************************************************************** //
//
//
//
//      4.      SECONDARY / MISC. ENUM TYPES...
// *************************************************************************** //
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
static constexpr cblib::EnumArray< IOResult, const char * >
    DEF_IORESULT_NAMES  = { {
        "OK",   "I/O Error",    "Parsing Error",    "Version Mismatch"
} };



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "SECONDARY ENUM TYPES".












// *************************************************************************** //
//
//
//
//      5.    HIT DETECTION.
// *************************************************************************** //
// *************************************************************************** //

//  "HitType"
//
enum class HitType : uint8_t {
    Handle = 0,
    Vertex,      Edge,       Surface,
    COUNT
};
//
//  "DEF_HIT_TYPE_NAMES"
static constexpr cblib::EnumArray< HitType, const char * >
    DEF_HIT_TYPE_NAMES  = { {
        "Handle",       "Vertex",       "Edge",         "Surface"
} };



//  "Hit_t"
//
template <typename HID>
struct Hit_t
{
    using Type = HitType;
//
//
    Type            type            = Type::Vertex;
//
    size_t          index           = 0;                //  Point/Line/Path: original meaning
    bool            out             = false;            //  valid only when type == Handle
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

//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "HIT DETECTION"









// *************************************************************************** //
//
//
//
//      6.      INTERACTION INFORMATION/STATES.
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
      None = 0
    , Selection
    , Canvas
    , ToolSelection
    , Browser
    , Settings
    , AskOkCancel
//
    , Other
//
    , COUNT
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
    CBEditorPopupFlags_Settings             = 1u << static_cast<unsigned>(  EditorPopupBits::Settings       ),
    CBEditorPopupFlags_AskOkCancel          = 1u << static_cast<unsigned>(  EditorPopupBits::AskOkCancel    ),
    CBEditorPopupFlags_Other                = 1u << static_cast<unsigned>(  EditorPopupBits::Other          ),
//
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
//
//  "set_bit"
static inline void set_bit(CBEditorPopupFlags_ & m, EditorPopupBits idx, bool on)
{
    const size_t    i   = static_cast<size_t>( idx );
	uint32_t        v   = to_u(m), b = bit_u(i);
	m                   = from_u(on ? (v | b) : (v & ~b));
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
    /* None             */  { nullptr                                   , "None"                },
    /* Selection        */  { "Editor_Selection_ContextMenu"            , "Selection"           },
    /* Canvas           */  { "Editor_Canvas_ContextMenu"               , "Canvas"              },
    /* ToolSelection    */  { "Editor_ControlBar_ToolSelectionMenu"     , "Tool Selection"      },
    /* Browser          */  { "Editor_Browser_ContextMenu"              , "Browser"             },
    /* Settings         */  { "Editor System Preferences"               , "Settings"            },
    /* AskOkCancel      */  { nullptr                                   , ""                    },
    /* Other            */  { nullptr                                   , "Other"               }
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
    bool                            other_windows_open              = false;
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
    mutable ImDrawList *            dl                      {   };
//
//
//
//                              NEW DATA:
    EditorInteraction               obj                     {   };
//
//
//
// *************************************************************************** //
//                                  MAIN INLINE FUNCTIONS...
// *************************************************************************** //

    //  "BlockShortcuts"
    [[nodiscard]]
    inline bool                     BlockShortcuts              (void) const noexcept
    //  { return ( !hovered ); }
    { return ( (!hovered) || (obj.open_menus) ); }

    //  "BlockInput"
    [[nodiscard]]
    inline bool                     BlockInput                  (void) const noexcept
    { return ( !hovered  &&  !obj.other_windows_open  &&  !ImGui::IsMouseDown(ImGuiMouseButton_Left) ); }   //  IsMouseDragging( ... )      IsMouseDragPastThreshold( ... )
//
//
//
// *************************************************************************** //
//                                  UTILITY FUNCTIONS...
// *************************************************************************** //

    //  "OtherWindowsOpen"
    [[nodiscard]]
    inline bool                     OtherWindowsOpen            (void) const noexcept
    { return (  ( obj.open_menus >> static_cast<CBEditorPopupFlags_>( EditorPopupBits::Other ) & 1u) != CBEditorPopupFlags_None  ); }



// *************************************************************************** //
// *************************************************************************** //
};//	END "Interaction" INLINE CLASS DEFINITION.



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "INTERACTION STATES"









// *************************************************************************** //
//
//
//
//      7.      SELECTION:
// *************************************************************************** //
// *************************************************************************** //

//  "Selection_t"
//
template< typename VertexID, typename PointID, typename LineID, typename PathID, typename ZID, typename HitID >
struct Selection_t
{
    using                               Hit                     = Hit_t         <HitID>                 ;
    using                               PathHit                 = PathHit_t     <PathID, VertexID>      ;
//
//
//
    inline void                         clear           (void)          { vertices.clear(); points.clear(); paths.clear();                  }        // new
    inline bool                         empty           (void) const    { return vertices.empty() && points.empty() && paths.empty();       }
    inline bool                         is_empty        (void) const    { return paths.empty();                                             }
//
//
//
    std::unordered_set<uint32_t>        vertices                {   };
    std::unordered_set<size_t>          points                  {   };
    std::unordered_set<size_t>          paths                   {   };     // ← NEW
//
//                                  CACHED ITEMS:
    mutable std::optional<Hit>          hovered                 { std::nullopt };
//
};
//
//  "to_json"
template<typename VID, typename PtID, typename LID, typename PID, typename ZID, typename HitID>
inline void to_json(nlohmann::json & j, const Selection_t<VID,PtID,LID,PID,ZID,HitID> & s)
{
    j = { { "vertices",  std::vector<VID>  (s.vertices.begin(), s.vertices.end()) },
          { "points",    std::vector<PtID> (s.points  .begin(), s.points  .end()) },
          { "paths",     std::vector<PID>  (s.paths   .begin(), s.paths   .end()) } };
}
//
//  "from_json"
template<typename VID, typename PtID, typename LID, typename PID, typename ZID, typename HitID>
inline void from_json(const nlohmann::json & j, Selection_t<VID,PtID,LID,PID,ZID,HitID> & s)
{
    std::vector<VID >  vs;  j.at("vertices").get_to(vs);
    std::vector<PtID>  ps;  j.at("points"  ).get_to(ps);
    std::vector<PID >  pa;  j.at("paths"   ).get_to(pa);

    s.vertices.clear();  s.vertices.insert(vs.begin(), vs.end());
    s.points  .clear();  s.points  .insert(ps.begin(), ps.end());
    s.paths   .clear();  s.paths   .insert(pa.begin(), pa.end());
}



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "SELECTION".









// *************************************************************************** //
//
//
//
//      8.      CURSOR STATE...
// *************************************************************************** //
// *************************************************************************** //

//  "Cursor_t"
//
template< typename VertexID, typename PointID, typename LineID, typename PathID, typename ZID, typename HitID >
struct Cursor_t
{
    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    //  CBAPP_APPSTATE_ALIAS_API            //  *OR*    CBAPP_CBLIB_TYPES_API       //  FOR CBLIB...
    //  using                               MyAlias                         = MyTypename_t;
    //  using                               IDType                          = uint32_t;
    //  using                               ObjectType                      = MyObject_t<IDType>;
    //  using                               State                           = cb::MyAppState;
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    //  static constexpr float              ms_MY_CONSTEXPR_VALUE           = 240.0f;
    
//
// *************************************************************************** //
// *************************************************************************** //   END "CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//      1.          DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //
    
    // *************************************************************************** //
    //      STATE VARIABLES.
    // *************************************************************************** //
    //  State                               m_state                         { State::None };

    // *************************************************************************** //
    //      IMPORTANT DATA-MEMBERS.
    // *************************************************************************** //
    //  mutable std::optional<Hit>          hovered                 { std::nullopt };
    
    // *************************************************************************** //
    //      GENERIC DATA.
    // *************************************************************************** //
    //  bool                                m_initialized                   = false;
    //  bool                                m_first_frame                   = false;
    
//
// *************************************************************************** //
// *************************************************************************** //   END "DATA-MEMBERS".



// *************************************************************************** //
//
//      2.A.        MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
    // *************************************************************************** //
    //      INITIALIZATION METHODS.         |   "init.cpp" ...
    // *************************************************************************** //
                                        Cursor_t                (void) noexcept                 = default;
                                        ~Cursor_t               (void)                          = default;
    
    // *************************************************************************** //
    //      DELETED FUNCTIONS.              |   ...
    // *************************************************************************** //
                                        Cursor_t                (const Cursor_t &    src)       = delete;   //  Copy. Constructor.
                                        Cursor_t                (Cursor_t &&         src)       = delete;   //  Move Constructor.
    Cursor_t &                          operator =              (const Cursor_t &    src)       = delete;   //  Assgn. Operator.
    Cursor_t &                          operator =              (Cursor_t &&         src)       = delete;   //  Move-Assgn. Operator.
    
//
// *************************************************************************** //
// *************************************************************************** //   END "MEMBER FUNCS".

    
   
// *************************************************************************** //
//
//      2.B.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    
    //  "clear"
    inline void                         clear                               (void) noexcept
    {
        return;
    };
    
    
    //  "_no_op"
    inline void                         _no_op                              (void)      { return; };
    
//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.



//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "Cursor_t" INLINE STRUCT DEFINITION.



//  "to_json"
template<typename VID, typename PtID, typename LID, typename PID, typename ZID, typename HitID>
inline void to_json([[maybe_unused]] nlohmann::json & j,
                    [[maybe_unused]] const Cursor_t<VID,PtID,LID,PID,ZID,HitID> & s)
{
    return;
}
//
//  "from_json"
template<typename VID, typename PtID, typename LID, typename PID, typename ZID, typename HitID>
inline void from_json([[maybe_unused]] const nlohmann::json & j,
                      [[maybe_unused]] Cursor_t<VID,PtID,LID,PID,ZID,HitID> & s)
{
    return;
}



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "Cursor_t".
    
    
    
    
    
    
    
    
    
    
    
    
// *************************************************************************** //
//
//
//
//      9.      TOOL STATE...
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
    float               radius                  = 10.0f;            // corner- or major-radius (placeholder)
    //  float               params[5]               = {0.0f};           // Array to hold multiple geometric descriptors for more complex shapes.
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
template<EditorCFGTraits CFG>
struct BrowserState_t {
// *************************************************************************** //
// *************************************************************************** //


    // *************************************************************************** //
    //      1.      NESTED TYPENAME ALIASES...
    // *************************************************************************** //
    _EDITOR_CFG_PRIMATIVES_ALIASES
    //  _EDITOR_CFG_OBJECTS_ALIASES
    //
    using                       Vertex                                  = Vertex_t      <CFG>                               ;
    using                       Path                                    = Path_t        <CFG, Vertex>                       ;

    // *************************************************************************** //
    //      2.      CONSTEXPR VALUES...
    // *************************************************************************** //
    static constexpr size_t     ms_MAX_PATH_TITLE_LENGTH                = Path::ms_MAX_PATH_LABEL_LENGTH + 64ULL;
    //
    static constexpr size_t     ms_MAX_VERTEX_TITLE_LENGTH              = Vertex::ms_VERTEX_NAME_BUFFER_SIZE + 32ULL;

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
        
        
        
        //  Hovered OBJECT in Browser.
        m_hovered_obj               = -1;           //  Hovered OBJECT in Browser-Selectable.
        m_hovered_canvas_obj        = -1;           //  Hovered OBJECT in Browser-Canvas.
        
        //  Hovered VERTEX in Browser.
        m_hovered_vertex            = { -1, -1 };   //  Hovered VERTEX in Browser.
        
          
          
          
        //  ??  Other  ??
        m_layer_browser_anchor      = -1;
        m_browser_anchor            = -1;
        m_inspector_vertex_idx      = -1;
        
        return;
    }
    
    // *************************************************************************** //

    // *************************************************************************** //
    //      3.1.    UTILITY FUNCTIONS...
    // *************************************************************************** //
    
    //  "HasAuxiliarySelection"
    [[nodiscard]] inline bool           HasAuxiliarySelection           (void) const noexcept {
        const bool obj_1    = !(this->m_hovered_obj    < 0);
        const bool obj_2    = ( !(this->m_hovered_vertex.first < 0)  &&  !(this->m_hovered_vertex.second < 0) );
        
        return ( obj_1  ||  obj_2 );
    }
    
    //  "ClearAuxiliarySelection"
    inline void                         ClearAuxiliarySelection         (void) const noexcept {
        this->m_hovered_obj             = -1;
        this->m_hovered_canvas_obj      = -1;
        
        this->m_hovered_vertex          = { -1, -1 };
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
    std::vector<int>            m_obj_rows                                  {   };
    bool                        m_obj_filter_dirty                          = false;    //
    int                         m_obj_rows_paths_rev                        = -1;
    //
    //
    //                      INDICES:
    int                         m_layer_browser_anchor                      = -1;       //  ?? currently selected LAYER ??
    //
    //
    int                         m_browser_anchor                            = -1;       //  ?? anchor index for Shift‑range select ??
    //
    mutable int                 m_hovered_obj                               = -1;
    mutable int                 m_hovered_canvas_obj                        = -1;
    //
    //
    int                         m_inspector_vertex_idx                      = -1;       //  ...
    mutable std::pair<int,int>  m_hovered_vertex                            = {-1, -1};
    //
    //
    int                         m_layer_rename_idx                          = -1;       //  LAYER that is BEING RENAMED     (–1 = none)
    int                         m_obj_rename_idx                            = -1;       //  OBJECT that is BEING RENAMED    (–1 = none)
    //
    //
    //                      CACHE AND MISC. DATA:
    char                        m_name_buffer[ ms_MAX_PATH_TITLE_LENGTH ]   = {   };    //  scratch text
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
template< typename VID, typename PtID, typename LID, typename PID, typename ZID, typename HID >
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
    const char *                            display_title           = nullptr;
//
    float                                   alpha                   = 0.65f;
    ImU32                                   bg                      = 0x000000FF;
    int                                     window_rounding         = 8;
//
    std::optional< Param<ImVec2> >          window_size             = { std::nullopt };      // {     {120.0f,    -1.0f},     { {80.0f,       1.0f},      {220.0f,    FLT_MAX} }   };
//
//
//                                      CACHE VALUES...
    bool                                    collapsed               = false;
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
    static constexpr ImGuiWindowFlags       ms_CUSTOM_WINDOW_FLAGS  = ImGuiWindowFlags_NoTitleBar  | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
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



// *************************************************************************** //
//
//      2.7B.       EDITOR "OVERLAY" DEFINITIONS.
// *************************************************************************** //
// *************************************************************************** //

//  "ResidentEntry_t"
//
template<typename OID = uint32_t>
struct ResidentEntry_t {
    using                       OverlayID       = OID;
    using                       Overlay         = Overlay_t<OID>;
//
    OverlayID                   id;             //  runtime ID (filled in ctor)
    Overlay *                   ptr;            //  Reference.
    OverlayCFG                  cfg;            //  compile-time defaults
    OverlayStyle                style;
};



//
//
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
//
struct BoxDrag
{
    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    
//
// *************************************************************************** //
// *************************************************************************** //   END "CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//      1.          DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //
    
    bool                        active              = false;
    uint8_t                     handle_idx          = 0;
    ImVec2                      anchor_ws           = {0.0f,    0.0f};
    ImVec2                      bbox_tl_ws          = {0.0f,    0.0f};
    ImVec2                      bbox_br_ws          = {0.0f,    0.0f};
    std::vector<uint32_t>       v_ids;
    std::vector<ImVec2>         v_orig;
    ImVec2                      mouse_ws0           = {0.0f,    0.0f};
    ImVec2                      handle_ws0;                                 //  initial world‑space position of the dragged handle
    float                       orig_w              = 1.0f;                 //  original bbox width  (world units)
    float                       orig_h              = 1.0f;                 //  original bbox height (world units)
    bool                        first_frame         = true;

    // *************************************************************************** //
    //      NEW STUFF.
    // *************************************************************************** //
    struct ViewCache {
        bool                    valid               = false;    // recompute if false or revs changed
        bool                    visible             = false;    // draw/hover only if true

        // Expanded bbox (world & pixels)
        ImVec2                  tl_ws               { 0.0f, 0.0f };
        ImVec2                  br_ws               { 0.0f, 0.0f };
        ImVec2                  tl_px               { 0.0f, 0.0f };
        ImVec2                  br_px               { 0.0f, 0.0f };

        // Handle anchors (world & pixels) and their pixel rects
        ImVec2                  handle_ws[8]        { };
        ImVec2                  handle_px[8]        { };
        ImRect                  handle_rect_px[8]   { };

        int                     hover_idx           = -1;       // -1 = none

        // Last-seen revision stamps (compare to Editor’s counters)
        uint64_t                sel_seen            = 0;
        uint64_t                geom_seen           = 0;
        uint64_t                cam_seen            = 0;
        uint64_t                style_seen          = 0;
    };
    ViewCache                   view                {   };
    
    
//
// *************************************************************************** //
// *************************************************************************** //   END "DATA-MEMBERS".



// *************************************************************************** //
//
//      2.A.        MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
//
// *************************************************************************** //
// *************************************************************************** //   END "MEMBER FUNCS".



    
//
//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "BoxDrag" INLINE CLASS DEFINITION.












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

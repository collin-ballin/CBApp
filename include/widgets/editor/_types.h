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
#include <bit>

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
//      0.      EDITOR STATE DATA   [TYPE/ENUM LAYER]...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//      0A. EDITOR |        MAIN ENUM TYPES.
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
//  "DEF_EDITOR_STATE_HOTKEY_NAMES"
static constexpr cblib::EnumArray< Mode, const char * >
DEF_EDITOR_STATE_HOTKEY_NAMES  = { {
      "V"
    , "H"
    , "P"
    , "C"
    , "S"
    , "+"
    , "-"
    , "SHIFT C"
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
    , ICON_FA_BEZIER_CURVE  //    ICON_FA_PEN_NIB
} };



//  "Action"
//      - Enum type for each "ACTION" that can be undertaken by the Editor.
//
enum class Action : uint8_t {
      None = 0              //  No Action (Idle).
    , PenDraw               //  Drawing with the PEN Tool.
    , ShapeDraw             //  Drawing with the SHAPE Tool.
//
//
    , LassoDrag             //  Dragging/Moving a LASSO SELECTION.
    , HandleDrag            //  Dragging/Moving a SINGLE VERTEX.
    , VertexDrag            //  Dragging/Moving a SINGLE VERTEX.
//
    , BBoxDrag              //  Moving an ENTIRE SELECTION BBOX.
    , BBoxScale             //  Scaling Selection BBox.
//
    , Invalid               //  Placeholder enum for debugging (Set if more-than-one action at a time).
    , COUNT
};
//
//
//  "DEF_ACTION_STATE_NAMES"
static constexpr cblib::EnumArray< Action, const char * >
DEF_ACTION_STATE_NAMES  = { {
    /*  None        */      "None"
    /*  PenDraw     */    , "Drawing Path"
    /*  ShapeDraw   */    , "Drawing Shape"
//
    /*  LassoDrag   */    , "Winding Lasso"
    /*  HandleDrag  */    , "Dragging Handle"
    /*  VertexDrag  */    , "Dragging Vertex"
//
    /*  BBoxDrag    */    , "Moving Selection"
    /*  BBoxScale   */    , "Scaling Selection"
//
//
    /*              */    , "INVALID"
} };



// *************************************************************************** //
//      0B. EDITOR |        MISC. ENUM TYPES.
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
    DEF_IORESULT_NAMES      = { { "OK",   "I/O Error",    "Parsing Error",    "Version Mismatch" } };



// *************************************************************************** //
//      0C. EDITOR |        CAPABILITY BITS.
// *************************************************************************** //

//  "Capability"
//      -Bitfield that defines what behaviors/capabilities each Editor State (Mode) has.
//
enum CBCapabilityFlags_ : uint64_t {
    CBCapabilityFlags_None                      = 0ULL,                     //  No special behaviours...
//
    CBCapabilityFlags_Pan                       = 1ULL << 0,                //  Space + drag navigation
    CBCapabilityFlags_Zoom                      = 1ULL << 1,                //  mouse-wheel magnification
    CBCapabilityFlags_Select                    = 1ULL << 2,                //  click / lasso / move-drag
    CBCapabilityFlags_CursorHint                = 1ULL << 3,                //  hand or resize cursor
    CBCapabilityFlags_EnableMutableHotkeys      = 1ULL << 4,                //  Disables hotkeys that alter objects (like [ DEL ]).
    CBCapabilityFlags_EnableAdvancedHotkeys     = 1ULL << 5,                //  Enable EXTRA hotkeys (CMD+J to join, etc)
//
//
    CBCapabilityFlags_COUNT,                                                //  helper: number of bits
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
/*  Edit Anchor         */      CBCapabilityFlags_Plain
};






// *************************************************************************** //
//      0D. EDITOR |        OBJECT TRAIT CATEGORIES...
// *************************************************************************** //

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
static constexpr cblib::EnumArray< ObjectTrait, const char * >
    DEF_OBJECT_TRAIT_NAMES  = { {
        "Properties",       "Vertices",         "Payload"
} };



// *************************************************************************** //
//      0E. EDITOR |        SELECTION GATING MECHANISMS.
// *************************************************************************** //

//  "EditorSelectionBits"
//
enum class EditorSelectionBits : uint8_t {
      Handles           = 0
    , Vertices          = 1
    , Edges             = 2
    , Surfaces          = 3
//
    , _UNUSED_1         = 4
    , _UNUSED_2         = 5
    , _UNUSED_3         = 6
//
//  INTERNAL...
    , Lock              = 7
    , COUNT             = 8
};


//  "CBEditorSelectionFlags_"
//
enum class CBEditorSelectionFlags : uint8_t {
      None              = 0
    , Handles           = 1u << static_cast<uint8_t>(  EditorSelectionBits::Handles     )
    , Vertices          = 1u << static_cast<uint8_t>(  EditorSelectionBits::Vertices    )
    , Edges             = 1u << static_cast<uint8_t>(  EditorSelectionBits::Edges       )
    , Surfaces          = 1u << static_cast<uint8_t>(  EditorSelectionBits::Surfaces    )
//
//
    , Lock              = 1u << static_cast<uint8_t>(  EditorSelectionBits::Lock        )
    , COUNT
};


//      BITWISE OPERATORS FOR ENUM CLASS...



//  "SelectionMask"
//
template<class MaskT = uint8_t, class TagE = EditorSelectionBits>
struct SelectionMask
{
    using underlying_tag_t = std::underlying_type_t<TagE>;
    static_assert( std::is_integral_v<MaskT> && std::is_unsigned_v<MaskT>,      "MaskT must be an unsigned integral type"   );
    static_assert( std::is_enum_v<TagE>,                                        "TagE must be an enum type"                 );


    MaskT value {0};

    // --- compile-time helpers
    static constexpr int  digits()               noexcept { return std::numeric_limits<MaskT>::digits; }
    static constexpr MaskT msb_mask()            noexcept { return MaskT(1) << (digits() - 1); }
    static constexpr bool is_lock(TagE e)        noexcept { return e == TagE::Lock; }

    // Map a Tag to its mask. Lock → MSB; others → 1 << index.
    static constexpr MaskT bit(TagE e) noexcept {
        if (is_lock(e)) return msb_mask();
        const auto idx = static_cast<underlying_tag_t>(e);
        // Guard: if someone defines a tag index beyond available user bits, map to 0.
        return (idx >= static_cast<underlying_tag_t>(digits() - 1))
                 ? MaskT(0)
                 : (MaskT(1) << idx);
    }

    // --- core ops
    constexpr void set(TagE e)                  noexcept { value |=  bit(e); }
    constexpr void clear(TagE e)                noexcept { value &= ~bit(e); }
    constexpr void toggle(TagE e)               noexcept { value ^=  bit(e); }
    constexpr bool test(TagE e)           const noexcept { return (value & bit(e)) != 0; }

    // bulk convenience
    constexpr void set_all(std::initializer_list<TagE> tags) noexcept {
        for (auto t : tags) set(t);
    }
    constexpr void clear_all(std::initializer_list<TagE> tags) noexcept {
        for (auto t : tags) clear(t);
    }

    // --- lock (gate) semantics
    constexpr void set_locked(bool on)          noexcept { on ? value |= msb_mask()
                                                             : value &= ~msb_mask(); }
    constexpr bool is_locked()            const noexcept { return (value & msb_mask()) != 0; }

    // When locked, return (fallback | MSB). Otherwise, return user value as-is.
    constexpr MaskT effective(MaskT fallback) const noexcept {
        return is_locked() ? MaskT(fallback | msb_mask()) : value;
    }

    // explicit masks for clarity (computed once)
    static constexpr MaskT user_bits_mask() noexcept {
        // All bits except MSB (lock)
        return (digits() == 0) ? MaskT(0) : MaskT(~msb_mask());
    }
    static constexpr MaskT lock_mask()     noexcept { return msb_mask(); }
};



// *************************************************************************** //
//      0F. EDITOR |        EDITOR POP-UP MENU LABELING, ETC.
// *************************************************************************** //
    
//  "EditorPopupBits"
//
enum class EditorPopupBits : uint8_t {
      None = 0
    , Selection
    , Canvas
    , ToolSelection
    , BrowserCTX
    , FilterCTX
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
    CBEditorPopupFlags_BrowserCTX           = 1u << static_cast<unsigned>(  EditorPopupBits::BrowserCTX     ),
    CBEditorPopupFlags_FilterCTX            = 1u << static_cast<unsigned>(  EditorPopupBits::FilterCTX      ),
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
    /* None             */  { nullptr                                   , "None"                            },
    /* Selection        */  { "Editor_Selection_ContextMenu"            , "Selection"                       },
    /* Canvas           */  { "Editor_Canvas_ContextMenu"               , "Canvas"                          },
    /* ToolSelection    */  { "Editor_ControlBar_ToolSelectionMenu"     , "Tool Selection"                  },
    /* FilterCTX        */  { "Browser_Filter_CTXMenu"                  , "Browser | Filter Context"        },
    /* BrowserCTX       */  { "Browser_Object_CTXMenu"                  , "Browser | Object Context"        },
    /* Settings         */  { "Editor System Preferences"               , "Settings"                        },
    /* AskOkCancel      */  { nullptr                                   , ""                                },
    /* Other            */  { nullptr                                   , "Other"                           }
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

//  "GetMenuName"
static inline const char * GetMenuName(const EditorPopupBits & handle)
    { return DEF_EDITOR_POPUP_INFOS[ static_cast<size_t>( handle ) ].name; }






// *************************************************************************** //
//      0G. EDITOR |        EDITOR WIDGET IDs / TOOL-TIP NAMES.
// *************************************************************************** //

//  "EditorTooltipKey"
//
enum class EditorTooltipKey : uint16_t
{
      None = 0
//
//
//      1.      MAIN CONTROLS.
    , ToolSelection             , ClearData                 , OpenSettings
//
//      2.      SELECTION STATE.
    , SSelectionSurface         , SelectionEdge             , SelectionVertex
//
//      3.      GRID CONTROLS.
    , GridSnap                  , GridPixelPerfect          , GridShow
    , GridDecrease              , GridIncrease              , GridDensityValue
//
//
//
    , COUNT
};
  
  
//  "DEF_TOOLTIP_INFOS"
//
static constexpr cblib::EnumArray< EditorTooltipKey, const char * >
DEF_TOOLTIP_INFOS  = { {
    /*  None                */        ""
//
//      1.      MAIN CONTROLS.
    /*  ToolSelection       */      , "Tool selection menu"
    /*  ClearData           */      , "Clear all data from the canvas"
    /*  OpenSettings        */      , "Open \"Editor\" system preferences menu  [CTRL ,]"
//
//      2.      SELECTION STATE.
    /*  SelectionSurface    */      , "Toggle SURFACE selection"
    /*  SelectionEdge       */      , "Toggle EDGE selection"
    /*  SelectionVertex     */      , "Toggle VERTEX selection"
//
//      3.      GRID CONTROLS.
    /*  GridSnap            */      , "Toggle Snap-To-Grid  [SHIFT G]"
    /*  GridPixelPerfect    */      , "Toggle Pixel-Perfect-Snapping (treat all coords. as integers)"
    /*  GridShow            */      , "Toggle visibility of the grid"
    /*  GridDecrease        */      , "Reduce grid resolution by ONE-HALF  [SHIFT -]"
    /*  GridIncrease        */      , "Increase grid resolution by DOUBLE  [SHIFT +]"
    /*  GridDensityValue    */      , "Snap-To-Grid resolution ( (x,y) quantization value )"
//
//
//
} };










    //      //  "DisplayIOStatus"
    //      inline void                         DisplayIOStatus                     (void) noexcept
    //      {
    //          ImGuiIO &           io                  = ImGui::GetIO();
    //          this->m_io_message_timer               -= io.DeltaTime;
    //
    //
    //          //      1.      DECREMENT TIMER...
    //          if ( this->m_io_message_timer <= 0.0f ) {
    //              this->m_io_message_timer = -1.0f;
    //              this->m_show_io_message.store(false, std::memory_order_release);
    //              return;
    //          }
    //          //
    //          //      2.      DISPLAY TOOL-TIP I/O MESSAGING...
    //          //  ImGui::SetNextWindowBgAlpha(0.75f); // optional: semi-transparent
    //          ImGui::BeginTooltip();
    //              ImGui::TextUnformatted( this->m_io_msg.c_str() );
    //          ImGui::EndTooltip();
    //
    //          return;
    //      }
    //      //  "SetLastIOStatus"
    //      inline void                         SetLastIOStatus                     (const IOResult result, std::string & message) noexcept
    //      {
    //          this->m_io_busy                 .store( false,   std::memory_order_release   );
    //          this->m_show_io_message         .store( true,    std::memory_order_release   );
    //          this->m_io_message_timer        = ms_IO_MESSAGE_DURATION;
    //          //
    //          this->m_io_last                 = result;
    //          this->m_io_msg                  = std::move( message );
    //
    //          return;
    //      }
    
    
    
    

//  "TooltipState"
//
template <typename Key, typename Value = const char *, typename Data = cblib::EnumArray<Key, Value> >
struct TooltipState
{

    // *************************************************************************** //
    //      STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    static constexpr ImGuiHoveredFlags  ms_TOOLTIP_HOVER_FLAGS          = ImGuiHoveredFlags_Stationary | ImGuiHoveredFlags_DelayShort; // ImGuiHoveredFlags_DelayNormal;
    static constexpr float              ms_TOOLTIP_RELEASE_TIME         = 0.250f;
    
//
// *************************************************************************** //
// *************************************************************************** //   END "CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//      1.          DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //
    const Data &                        word_bank;
    //
    mutable Key                         key                             = Key::None;
    ImGuiID                             key_id                          = 0;
    //
    std::atomic<bool>                   active                          { false };
    float                               timer                           = -1.0f;
    
//
// *************************************************************************** //
// *************************************************************************** //   END "DATA-MEMBERS".



// *************************************************************************** //
//
//      2.B.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      INITIALIZATION FUNCTIONS.
    // *************************************************************************** //
    //  Default Constructor.
    explicit                            TooltipState                    (const Data & bank) noexcept : word_bank(bank)      {   }

    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      MAIN FUNCTIONS.
    // *************************************************************************** //
    
    //  "UpdateTooltip"
    inline void                         UpdateTooltip                   (const Key key_) noexcept
    {
        //      CASE 0 :    IF STATUS IS ALREADY "ACTIVE", IGNORE INPUT...
        if ( this->active.load() )     { return; }
    
    
        //      CASE 1 :    STORE THE HOVERED ITEM.  SET STATUS TO "ACTIVE"...
        if ( ImGui::IsItemHovered(ms_TOOLTIP_HOVER_FLAGS) )
        {
            this->active    .store( true,    std::memory_order_release   );
            this->key       = key_;
            this->key_id    = ImGui::GetHoveredID();
            this->timer     = ImGui::GetHoveredID();
        }
        
        return;
    }
        
    //  "ShowTooltip"
    inline void                         ShowTooltip                     (void) noexcept
    {
        ImGuiIO &           io              = ImGui::GetIO();
        const ImGuiID       object_id       = ImGui::GetHoveredID();
        
        //      CASE 0 :    TOOL-TIP IS INACTIVE...
        if ( this->IsInactive() )       { return; }
        
        
        
        //      1A.     CURSOR REMAINS ON THE ORIGINAL WIDGET  [ RE-SET THE TIMER ]...
        if ( object_id == this->key_id )    {  this->_reset_timer();  }
        //      1B.     CURSOR HAS MOVED *OFF* THE ORIGINAL WIDGET  [ DECREMENT THE TIMER ]...
        else                                {  this->timer -= io.DeltaTime;  }
        
        
        //      2.      DRAW THE TOOLTIP...
        ImGui::BeginTooltip();
        //
            ImGui::Text             ( "%s",              this->word_bank[ this->key ]    );
            ImGui::SameLine();
            ImGui::TextDisabled     ( "Object ID: %d",   object_id                       );
        //
        ImGui::EndTooltip();
        
        
        
        //      3.      LASTLY, UPDATE "ACTIVE" STATUS...
        if ( this->timer < 0.0f )
        {
            this->_set_inactive();
        }
        
        return;
    }

    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      UTILITY FUNCTIONS.
    // *************************************************************************** //
    
    //  "HasTooltip"
    //[[nodiscard]] inline bool           HasTooltip                      (void) const noexcept   { return ( (this->key != Key::None)  &&  () );  }
    [[nodiscard]] inline bool           HasTooltip                      (void) const noexcept   { return ( (this->key != Key::None) );  }
    
    //  "IsActive"
    [[nodiscard]] inline bool           IsActive                        (void) const noexcept   { return ( this->active.load()  );   }
    [[nodiscard]] inline bool           IsInactive                      (void) const noexcept   { return ( !this->active.load() );   }
    
    
    //  "_reset_timer"
    inline void                         _reset_timer                    (void) noexcept         { this->timer = this->ms_TOOLTIP_RELEASE_TIME;  }
    
    
    //  "_set_active"
    inline void                         _set_active                     (const Key key_, const ImGuiID key_id_) noexcept
    {
        this->active    .store( true,    std::memory_order_release   );
        this->key       = key_;
        this->key_id    = key_id_;
        this->_reset_timer();
        return;
    }
    
    
    //  "_set_inactive"
    inline void                         _set_inactive                   (void) noexcept
    {
        this->active    .store( false,    std::memory_order_release   );
        this->key       = Key::None;
        this->key_id    = 0;
        this->timer     = -1.0f;
        return;
    }

//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "TooltipState" INLINE STRUCT DEFINITION.



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "0.  EDITOR STATE ITEMS".














// *************************************************************************** //
//
//
//
//      1.    MAIN OBJECT TYPES  [ REMAINDER THAT ARE NOT INSIDE "Vertex_t", "Path_t", etc ]...
// *************************************************************************** //
// *************************************************************************** //

//  "HitType"
//
enum class HitType : uint8_t {
    None = 0,
    Handle,
    Vertex,      Edge,       Surface,
    COUNT
};
//
//  "DEF_HIT_TYPE_NAMES"
static constexpr cblib::EnumArray< HitType, const char * >
    DEF_HIT_TYPE_NAMES  = { {
        "None",     "Handle",       "Vertex",       "Edge",         "Surface"
} };








//  "Hit_t"
//      PLAIN-OLD-DATA (POD) STRUCT.
//
template <typename HID>
struct Hit_t
{
    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    using                               Hit                             = Hit_t<HID>;
    using                               Type                            = HitType;
    
    // *************************************************************************** //
    //
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

    // *************************************************************************** //
    //      IMPORTANT DATA-MEMBERS.
    // *************************************************************************** //
    Type                                type                            = Type::None;
    size_t                              index                           = 0;                //  Point/Line/Path: original meaning
    bool                                out                             = false;            //  valid only when type == Handle
    
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
                                        //  Hit_t                   (void) noexcept                 = default;
                                        //  ~Hit_t                  (void)                          = default;
    
    // *************************************************************************** //
    //      DELETED FUNCTIONS.              |   ...
    // *************************************************************************** //
    //                                      Hit_t                   (const Hit_t &    src)          = delete;   //  Copy. Constructor.
    //                                      Hit_t                   (Hit_t &&         src)          = delete;   //  Move Constructor.
    //  Hit_t &                             operator =              (const Hit_t &    src)          = delete;   //  Assgn. Operator.
    //  Hit_t &                             operator =              (Hit_t &&         src)          = delete;   //  Move-Assgn. Operator.
    
//
// *************************************************************************** //
// *************************************************************************** //   END "MEMBER FUNCS".

    
   
// *************************************************************************** //
//
//      2.B.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      QUERY FUNCTIONS.
    // *************************************************************************** //
    
    //  "IsNonObjectHit"
    //
    //      POLICY :    I think we should consider an "empty" hover to be a NON-OBJECT.
    //      A.K.A. :    Our policy will be to only consider WHOLISTIC OBJECTS to be an OBJECT-HOVER.  All else will be Non-Object.
    //
    //          --- ?? This will allow us to treat an EMPTY HIT, HANDLE-HIT, or VERTEX-HIT as a different action than an OBJECT-HIT ??
    //
    [[nodiscard]] inline bool           IsNonObjectHit                      (void) const noexcept
    {
        switch (this->type) {
            case Type::Edge    :
            case Type::Surface :    { return false; }
            default :               { return true;  }
        }
    }
    //  "IsObjectHit"
    [[nodiscard]] inline bool           IsObjectHit                         (void) const noexcept
    {
        switch (this->type) {
            case Type::Edge    :
            case Type::Surface :    { return true; }
            default :               { return false; }
        }
        return false;
    }

    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    //  "_no_op"
    inline void                         _no_op                              (void)      { return; };
    
//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.



//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "Hit_t" INLINE STRUCT DEFINITION.






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
// *************************************************************************** //   END "1.  MAIN OBJECT TYPES".












// *************************************************************************** //
//
//
//
//      2.      AUXILIARY EDITOR TYPES / STATE VARIABLES...
// *************************************************************************** //
// *************************************************************************** //

//  "GridState"
//      PLAIN-OLD-DATA (POD) STRUCT.
//
struct GridState
{
    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    CBAPP_CBLIB_TYPES_API
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    //                                  CONSTANTS:
    static constexpr double                 ms_INITIAL_CANVAS_SIZE [4]          = { 0.0f, 256.0f, 0.0f, 256.0f };
    static constexpr double                 ms_INPUT_DOUBLE_INCREMENTS [2]      = { 1.0f, 10.0f };                      //  Snap value of "+" and "-" BUTTONS.
    
    static constexpr std::string_view       ms_GRID_LABEL_FMT                   = "{:.0f}";
    
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
    bool                                    visible                             = true;
    bool                                    snap_on                             = false;
    bool                                    pixel_perfect                       = false;
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //                  IMPLOT CANVAS INFORMATION...
    // *************************************************************************** //
    //                                          PERSISTENT STATE INFORMATION:
    std::array< Param<double>, 2>                   m_world_size                        = { {                                   //  MAXIMUM SIZE OF THE CANVAS (World Size).
                                                                                            { 512.0f,       { 10.0f,        1e4f } },
                                                                                            { 512.0f,       { 10.0f,        1e4f } }
                                                                                        } };
    std::array< Param<double>, 2>                   m_world_slop                        = { {                                   //  (Allow user to scroll a bit beyond canvas limits).
                                                                                            { 128.0f,       { 32.0f,        512.0f } },
                                                                                            { 128.0f,       { 32.0f,        512.0f } }
                                                                                        } };
    std::array< Param<double>, 2>                   m_zoom_size                         = { {                                   //  MAX + MIN "ZOOM" RESOLUTION OF THE CANVAS.
                                                                                            { 1024.0f,      { 1.0f,         2e4f } },
                                                                                            { 1024.0f,      { 1.0f,         2e4f } }
                                                                                        } };
    //
    //                                          GRID INFORMATION:
    std::array< Param<int>, 2>                      m_grid_density                      = { {                                   //  TOTAL # OF GRIDLINES.
                                                                                            { 16,           { 4,            128 } },
                                                                                            { 16,           { 4,            128 } }
                                                                                        } };
    std::array< std::vector<double>, 2 >            m_gridlines;                                                                //  POSITION OF THE "X" AND "Y" GRIDLINES.
    std::array< std::vector<std::string>, 2 >       m_grid_strings;
    std::array< std::vector<const char *>, 2 >      m_grid_labels;
    std::array< double, 2 >                         m_grid_spacing                      = { -1.0f,      -1.0f };                //  DIST. BETWEEN EACH GRIDLINE.
    //
    //
    //                                          TRANSIENT STATE INFORMATION:
    mutable ImPlotRect                              m_window_coords                     = {   };        //  DOMAIN + RANGE OF CURRENT CANVAS:   ( [X0, Xf], [Y0, Yf] ).
    std::array< double, 2>                          m_window_size                       = {   };
    //
    mutable ImVec2                                  m_plot_px_dims                      = {   };
    mutable ImRect                                  m_plot_bbox                         = {   };
    
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      *DATA THAT I'M MOVING FROM EDITOR-CLASS INTO HERE*.
    // *************************************************************************** //
    
    ImPlotFlags                             plot_flags                    = ImPlotFlags_Equal | ImPlotFlags_NoFrame | ImPlotFlags_NoBoxSelect | ImPlotFlags_NoMenus | ImPlotFlags_NoLegend | ImPlotFlags_NoTitle;
    utl::AxisCFG                            axes [2]                      = {
        {"##x-axis",    ImPlotAxisFlags_None | ImPlotAxisFlags_NoSideSwitch | ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_NoInitialFit | ImPlotAxisFlags_Opposite },
        {"##y-axis",    ImPlotAxisFlags_None | ImPlotAxisFlags_NoSideSwitch | ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_NoInitialFit  }
    };
    utl::LegendCFG                          legend                        = { ImPlotLocation_NorthWest, ImPlotLegendFlags_None };
    
    
    
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
                                        GridState               (void) noexcept
    {
        //      1.      ALLOCATE MEMORY FOR GRIDLINES...
        this->m_gridlines    [0]    .reserve( this->m_grid_density[0].Max() );      //  X-Axes.
        this->m_grid_strings [0]    .reserve( this->m_grid_density[0].Max() );
        this->m_grid_labels  [0]    .reserve( this->m_grid_density[0].Max() );
        
        this->m_gridlines    [1]    .reserve( this->m_grid_density[1].Max() );      //  Y-Axes.
        this->m_grid_strings [1]    .reserve( this->m_grid_density[1].Max() );
        this->m_grid_labels  [1]    .reserve( this->m_grid_density[1].Max() );
        
    
        this->_update_grid();
        return;
    }
                                        ~GridState              (void)                          = default;
    
    // *************************************************************************** //
    //      DELETED FUNCTIONS.              |   ...
    // *************************************************************************** //
                                        GridState               (const GridState &    src)      = delete;   //  Copy. Constructor.
                                        GridState               (GridState &&         src)      = delete;   //  Move Constructor.
    GridState &                         operator =              (const GridState &    src)      = delete;   //  Assgn. Operator.
    GridState &                         operator =              (GridState &&         src)      = delete;   //  Move-Assgn. Operator.
    
//
// *************************************************************************** //
// *************************************************************************** //   END "MEMBER FUNCS".

    
   
// *************************************************************************** //
//
//      2.B.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
    // *************************************************************************** //
    //      QUERY FUNCTIONS.
    // *************************************************************************** //
    
    //  "CanDecreaseGridSpacing"
    [[nodiscard]] inline bool               CanDecreaseGridSpacing              (void) const noexcept
    {
        const bool  decr_x  = ( this->m_grid_density[0].CanDecrement() );
        const bool  decr_y  = ( this->m_grid_density[1].CanDecrement() );
        return (decr_x  &&  decr_y);
    }
    //
    [[nodiscard]] inline bool               CanDecreaseGridSpacingX             (void) const noexcept
        { return this->m_grid_density[0].CanDecrement(); }
        
    [[nodiscard]] inline bool               CanDecreaseGridSpacingY             (void) const noexcept
        { return this->m_grid_density[1].CanDecrement(); }
    
    
    
    //  "CanIncreaseGridSpacing"
    [[nodiscard]] inline bool               CanIncreaseGridSpacing              (void) const noexcept
        { return ( this->CanIncreaseGridSpacingX()  &&  this->CanIncreaseGridSpacingY() ); }
    //
    //  {
    //      return ( this->m_grid_density[0].CanIncrement()   &&  this->m_grid_density[1].CanIncrement() )  &&     //  1.  Param<T> is at MAXIMUM VALUE.
    //             ( (this->m_grid_spacing[0] > 2.0f)         &&  (this->m_grid_spacing[1] > 2.0f) );               //  2.  DYADIC STEP-SIZE CONSTRAINTS.
    //  }
    //
    [[nodiscard]] inline bool               CanIncreaseGridSpacingX              (void) const noexcept
        { return (this->m_grid_density[0].CanIncrement()  &&  (this->m_grid_spacing[0] > 2.0f)); }

    [[nodiscard]] inline bool               CanIncreaseGridSpacingY              (void) const noexcept
        { return (this->m_grid_density[1].CanIncrement()  &&  (this->m_grid_spacing[1] > 2.0f)); }


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    
    //  "SetupImPlotGrid"
    inline void                             SetupImPlotGrid                     (void) const noexcept
    {
        ImPlot::SetupAxisTicks(
            /* axis             */    ImAxis_X1
            /* values           */  , this->m_gridlines    [0].data()
            /* n_ticks          */  , this->m_grid_density [0].Value()
            /* labels           */  , this->m_grid_labels  [0].data()
            /* keep_default     */  , false
        );
        
        ImPlot::SetupAxisTicks( 
            /* axis             */    ImAxis_Y1
            /* values           */  , this->m_gridlines    [1].data()
            /* n_ticks          */  , this->m_grid_density [1].Value()
            /* labels           */  , this->m_grid_labels  [1].data()
            /* keep_default     */  , false
        );



        //      // Sets the format of numeric axis labels via formater specifier (default="%g"). Formated values will be double (i.e. use %f).
        //      IMPLOT_API void SetupAxisFormat(ImAxis axis, const char* fmt);
        //      // Sets the format of numeric axis labels via formatter callback. Given #value, write a label into #buff. Optionally pass user data.
        //      IMPLOT_API void SetupAxisFormat(ImAxis axis, ImPlotFormatter formatter, void* data=nullptr);
        //      // Sets an axis' ticks and optionally the labels. To keep the default ticks, set #keep_default=true.
        //      IMPLOT_API void SetupAxisTicks(ImAxis axis, const double* values, int n_ticks, const char* const labels[]=nullptr, bool keep_default=false);
        //      // Sets an axis' ticks and optionally the labels for the next plot. To keep the default ticks, set #keep_default=true.
        //      IMPLOT_API void SetupAxisTicks(ImAxis axis, double v_min, double v_max, int n_ticks, const char* const labels[]=nullptr, bool keep_default=false);

        return;
    }

    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      IMPLOT GRID MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    
    //  "IncreaseGridSpacing"
    inline bool                             IncreaseGridSpacing                 (void) noexcept {
        if ( !this->CanIncreaseGridSpacing() )      { return false; }
    
        this->m_grid_density[0].SetValue            ( this->m_grid_density[0].value << 1 );
        this->m_grid_density[1].SetValue            ( this->m_grid_density[1].value << 1 );
        _update_grid();
        return true;
    }
    inline void                             IncreaseGridSpacingX                (void) noexcept {
        static_assert( true, "not implemented" );
        //  this->m_grid_density[0].SetValue( this->m_grid_density[0].value << 1 );
        //  _update_grid_x();
        return;
    }
    inline void                             IncreaseGridSpacingY                (void) noexcept {
        static_assert( true, "not implemented" );
        //  this->m_grid_density[1].SetValue( this->m_grid_density[1].value << 1 );
        //  this->_update_grid_y();
        return;
    }
    
    
    //  "DecreaseGridSpacing"
    inline bool                             DecreaseGridSpacing                 (void) noexcept {
        if ( !this->CanDecreaseGridSpacing() )      { return false; }
        
        this->m_grid_density[0].SetValue            ( this->m_grid_density[0].value >> 1 );
        this->m_grid_density[1].SetValue            ( this->m_grid_density[1].value >> 1 );
        _update_grid();
        return true;
    }
    inline void                             DecreaseGridSpacingX                (void) noexcept {
        static_assert( true, "not implemented" );
        //  this->m_grid_density[1].SetValue( this->m_grid_density[0].value >> 1 );
        //  this->_update_grid_x();
        return;
    }
    inline void                             DecreaseGridSpacingY                (void) noexcept {
        static_assert( true, "not implemented" );
        //  this->m_grid_density[1].SetValue( this->m_grid_density[1].value >> 1 );
        //  this->_update_grid_y();
        return;
    }
    
    
    //  "_compute_grid_spacing"
    inline void                             _compute_grid_spacing               (void) noexcept
    {
        return;
    }
    
    
    //  "_update_grid"
    inline void                             _update_grid                        (void) noexcept
    {
        const size_t        NX          = static_cast<size_t>( this->m_grid_density[0].Value() );
        const size_t        NY          = static_cast<size_t>( this->m_grid_density[1].Value() );
        const double        xmax        = this->m_world_size[0].Value();
        const double        ymax        = this->m_world_size[1].Value();
        //
        IM_ASSERT(          NX == NY    &&  "GridState::_update_grid: NX and NY must be equal" );
        
            
        //      1.      UPDATE DIST. BETWEEN EACH GRIDLINE...
        m_grid_spacing[0]               = xmax / static_cast<double>( NX );
        m_grid_spacing[1]               = ymax / static_cast<double>( NY );
        
        
        //      2.      RE-SIZE ARRAYS OF GRIDLINE POSITIONS...
        this->m_gridlines    [0]        .resize(NX);    //  X-Axis.
        this->m_grid_strings [0]        .resize(NX);
        this->m_grid_labels  [0]        .resize(NX);
        //
        this->m_gridlines    [1]        .resize(NY);    //  Y-Axis.
        this->m_grid_strings [1]        .resize(NY);
        this->m_grid_labels  [1]        .resize(NY);
        
        
        //      3.      POPULATE ARRAYS WITH NEW VALUES...
        //
        //              3A.         X-AXIS.
        this->_update_grid_array        (0,     NX);
        //
        //              3B.         Y-AXIS.
        this->_update_grid_array        (1,     NY);
    
    
    
        return;
    }
    
    
    //  "_compute_label_density"
    //      TO-DO:  Make this a LUT Array so O(1) lookup for value of j.
    //
    [[nodiscard]] static inline size_t      _compute_label_density              (const size_t N1) noexcept
    {
        constexpr size_t    N0              = 16ULL;
        constexpr size_t    n0              = 4ULL;
        const size_t        n1              = std::countr_zero(N1);
    
        //  1.  IF N <= 16 :    PLACE EVERY LABEL.
        //  2.  ELSE :          PLACE EVERY  j-th  LABEL, WHERE j = log_2( 16 ) - log_2( N )
        //                      A.K.A. --- EVERY  n-th  LABEL FOR EACH ADDITIONAL  n-value POWER-OF-TWO.
        return (N1 <= N0)   ? 1ULL  : (1ULL << ((n1 - n0) >> 1));
    }
    
    
    //  "_update_grid_array"
    inline void                             _update_grid_array                  (const size_t axis, const size_t N) noexcept
    {
        constexpr size_t        phase       = 0;    //  phase (optional): shift the pattern; label when (i - phase) % j == 0
        const size_t            j           = this->_compute_label_density(N);
        
        
        for (size_t i = 0; i < N; ++i)
        {
            const double    value               = static_cast<double>( i ) * m_grid_spacing[axis];
            const bool      skip                = (  ((i + N - (phase % j)) % j) == 0  );
            //
            this->m_gridlines    [axis][i]      = value;
            
            
            //      CASE 1A :   PLACE EVERY  *j-th*  LABEL.     SKIP EVERY  *non j-th*  LABEL...
            if ( skip )     { this->m_grid_strings [axis][i] = std::vformat( ms_GRID_LABEL_FMT, std::make_format_args(value) ); }
            else            { this->m_grid_strings [axis][i] = ""; }
            
            this->m_grid_labels  [axis][i]      = this->m_grid_strings[axis][i].c_str();
        }
    
        return;
    }
    
    
    
    
    
    // *************************************************************************** //
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.



//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "GridState" INLINE STRUCT DEFINITION.


//      GridState     : "to_json"
//
inline void to_json([[maybe_unused]] nlohmann::json & j, [[maybe_unused]] const GridState & obj)
{
    //  j ["m_world_size"]      = obj.m_world_size;
    //  j ["m_world_slop"]      = obj.m_world_slop;
    //  j ["m_zoom_size"]       = obj.m_zoom_size;
    //
    //  j = {
    //      { "m_world_size",          obj.m_world_size       }
    //  };
    return;
}


//      GridState     : "from_json"
//
inline void from_json([[maybe_unused]] nlohmann::json & j, [[maybe_unused]] GridState & obj)
{
    //  j.at("")        .get_to(obj.m_plot_limits);
    //  j.at("Z_EDITOR_BACK")           .get_to(obj.Z_EDITOR_BACK);
    //  j.at("Z_FLOOR_USER")            .get_to(obj.Z_FLOOR_USER);
    //  j.at("Z_EDITOR_FRONT")          .get_to(obj.Z_EDITOR_FRONT);
    //  j.at("Z_CEIL_USER")             .get_to(obj.Z_CEIL_USER);
    //  j.at("RENORM_THRESHOLD")        .get_to(obj.RENORM_THRESHOLD);
    return;
}






// *************************************************************************** //
//      2B. AUXILIARY |         PER-FRAME INTERACTIONS.
// *************************************************************************** //

//  "EditorInteraction"
//
struct EditorInteraction
{
    // *************************************************************************** //
    //      TYPENAME ALIASES AND CONSTANTS.
    // *************************************************************************** //
    using                           CBEditorPopupFlags              = CBEditorPopupFlags_;
    
    
    // *************************************************************************** //
    //      DATA MEMBERS.
    // *************************************************************************** //
    //                          EDITOR STATE:
    //  bool                            empty_selection                 = false;
    //  bool                            single_obj_selection            = false;
    //
    //                          MENU STATE:
    CBEditorPopupFlags_             open_menus                      = CBEditorPopupFlags_None;
    bool                            other_windows_open              = false;


    // *************************************************************************** //
    //      INLINE FUNCTIONS...
    // *************************************************************************** //
    //  inline void                         NoMenusOpen                         (void)      { return; };
    //  inline void                         _no_op                              (void)      { return; };



//
// *************************************************************************** //
// *************************************************************************** //
};//	END "EditorInteraction" INLINE CLASS DEFINITION.




//  "Interaction"
//
struct Interaction
{
// *************************************************************************** //
//          PREVIOUS DATA...
// *************************************************************************** //
//                              STATE VARIABLES:
    bool                            hovered                 {   };
    bool                            active                  {   };
    bool                            space                   {   };
//
//
//                              STATE VARIABLES:
    ImVec2                          mouse_pos               {   };      //  ImPlotPoint
//
//
//                              IMPLOT VARIABLES:
    ImVec2                          canvas                  {   };
    ImVec2                          origin                  {   };  //  Top-Left corner of the plot.
//
//
//                              OTHER DATA:
    mutable ImDrawList *            dl                      { nullptr };
//
//
//                              NEW DATA:
    EditorInteraction               obj                     {   };
//
//
//
// *************************************************************************** //
//          MAIN INLINE FUNCTIONS...
// *************************************************************************** //

    //  "BlockShortcuts"
    [[nodiscard]] inline bool           BlockShortcuts              (void) const noexcept
    //  { return ( !hovered ); }
    { return ( (!hovered) || (obj.open_menus) ); }

    //  "BlockInput"
    [[nodiscard]] inline bool           BlockInput                  (void) const noexcept
    { return ( !hovered  &&  !obj.other_windows_open  &&  !ImGui::IsMouseDown(ImGuiMouseButton_Left) ); }   //  IsMouseDragging( ... )      IsMouseDragPastThreshold( ... )
//
//
//
// *************************************************************************** //
//          UTILITY FUNCTIONS...
// *************************************************************************** //

    //  "OtherWindowsOpen"
    [[nodiscard]] inline bool           OtherWindowsOpen            (void) const noexcept
    { return (  ( obj.open_menus >> static_cast<CBEditorPopupFlags_>( EditorPopupBits::Other ) & 1u) != CBEditorPopupFlags_None  ); }
    
    //  "NoMenusOpen"
    [[nodiscard]] inline bool           NoMenusOpen                 (void) const noexcept   { return (this->obj.open_menus == CBEditorPopupFlags_None); }



// *************************************************************************** //
// *************************************************************************** //
};//	END "Interaction" INLINE CLASS DEFINITION.



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "2.  AUXILIARY INTERACTION TYPES".










    
    
    
    
    
    
    
    
    
    
    
// *************************************************************************** //
//
//
//
//      3.      PRIMARY TOOL STATES...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//      3A. TOOL STATES |       PEN---TOOL.
// *************************************************************************** //

//  "PenState_t"
//
template<typename VID>
struct PenState_t
{
    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    //  using                               State                           = cb::MyAppState;
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
    bool                                active                          = false;
    //
    bool                                prepend                         = false;
    bool                                pending_handle                  = false;        // waiting to see if user drags
    bool                                dragging_handle                 = false;
    bool                                dragging_out                    = true;         //  NEW: true → out_handle, false → in_handle

    // *************************************************************************** //
    //      IMPORTANT DATA-MEMBERS.
    // *************************************************************************** //
    std::optional<size_t>               path_index                      = std::nullopt;     //  ID of the current path.
    VID                                 last_vid                        = 0;
    VID                                 handle_vid                      = 0;

    // *************************************************************************** //
    //      GENERIC DATA.
    // *************************************************************************** //
    VID                                 pending_vid                     = 0;            // vertex that may get a handle
    float                               pending_time                    = 0.0f;
    
   
// *************************************************************************** //
//
//      2.B.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    
    //  "reset"
    inline void                         reset                               (void) noexcept
    {
        //      1.      STATE VARIABLES...
        active                  = false;
        prepend                 = false;
        pending_handle          = false;        // waiting to see if user drags
        dragging_handle         = false;
        dragging_out            = true;         //  NEW: true → out_handle, false → in_handle
    
        //      2.      DATA-MEMBERS...
        path_index              = std::nullopt;     //  ID of the current path.
        last_vid                = 0;
        handle_vid              = 0;
    
        //      3.      GENERIC DATA...
        pending_vid             = 0;            // vertex that may get a handle
        pending_time            = 0.0f;
        return;
    };
    
//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.



//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "PenState_t" INLINE STRUCT DEFINITION.






// *************************************************************************** //
//      3B. TOOL STATES |       SHAPE---TOOL.
// *************************************************************************** //

//  "ShapeKind"
enum class ShapeKind : uint8_t {
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






// *************************************************************************** //
//      3C. TOOL STATES |       SHAPE---TOOL.
// *************************************************************************** //





//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 3.  "PRIMARY TOOL STATES" ]].











    
// *************************************************************************** //
//
//
//
//      4.      AUXILIARY STATES...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//      4A. AUXILIARY |         INDEX STATE.
// *************************************************************************** //

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
//      4B. AUXILIARY |         SELECTION STATE.
// *************************************************************************** //

//  "Selection_t"
//      PLAIN-OLD-DATA (POD) STRUCT.
//
template <EditorCFGTraits CFG_>
struct Selection_t
{
    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    _USE_EDITOR_CFG_ALIASES
    
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
    std::unordered_set<vertex_id>       vertices                {   };
    std::unordered_set<point_id>        points                  {   };
    std::unordered_set<path_id>         paths                   {   };     // ← NEW
    
    // *************************************************************************** //
    //      CACHE DATA-MEMBERS
    // *************************************************************************** //
    mutable std::optional<Hit>          hovered                 { std::nullopt };
    
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
                                        Selection_t             (void) noexcept                 = default;
                                        ~Selection_t            (void)                          = default;
    
//
// *************************************************************************** //
// *************************************************************************** //   END "MEMBER FUNCS".

    
   
// *************************************************************************** //
//
//      2.B.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      STATIC FUNCTIONS.
    // *************************************************************************** //



    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      QUERY FUNCTIONS.
    // *************************************************************************** //
    
    inline bool                         empty                               (void) const    { return vertices.empty() && points.empty() && paths.empty();       }
    inline bool                         is_empty                            (void) const    { return ( this->paths.empty()  &&  this->vertices.empty() );       }
    //  inline bool                         is_empty                            (void) const    { return paths.empty();                                             }
    
    
    //  "IsHoveringNonObject"
    [[nodiscard]] inline bool           IsHoveringNonObject                 (void) const noexcept
        { return (this->hovered)  ? (*this->hovered).IsNonObjectHit() : false; }
    [[nodiscard]] inline bool           IsHoveringObject                    (void) const noexcept
        { return (this->hovered)  ? (*this->hovered).IsObjectHit() : false; }


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    
    //  "clear_hover"
    inline void                         clear_hover                         (void) noexcept         { this->hovered.reset(); }
    
    //  "clear"
    inline void                         clear                               (void) noexcept
    {
        this->clear_hover();
        this->vertices  .clear();
        this->points    .clear();
        this->paths     .clear();
        
        return;
    }


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      OPERATION FUNCTIONS.
    // *************************************************************************** //
    
    //  "AddPath"
    //  [[nodiscard]] inline bool           AddPath                             (const Path & p) noexcept
    inline void                         AddPath                             (const Path & p) noexcept
    {
            this->paths.insert( p.GetID() );

            //  for (VertexID vid : p.verts)        // include every vertex + its glyph index
            //  {
            //      m_sel.vertices.insert(vid);
            //      for (size_t gi = 0; gi < m_points.size(); ++gi)
            //      {
            //          if (m_points[gi].v == vid)      { m_sel.points.insert(gi); }
            //      }
            //  }
        
        return;
    }
    
//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.



//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "Selection_t" INLINE STRUCT DEFINITION.


//      Selection_t     : "to_json"
//
template <EditorCFGTraits CFG_>
inline void to_json(nlohmann::json & j, const Selection_t<CFG_> & s)
{
    using   sel         = Selection_t<CFG_>         ; 
    using   vertex_id   = sel::vertex_id            ;
    using   point_id    = sel::point_id             ;
    using   path_id     = sel::path_id              ;
    
    
    
    j = {
        { "vertices",  std::vector<vertex_id>   ( s.vertices.begin()    , s.vertices.end() )  },
        { "points",    std::vector<point_id>    ( s.points  .begin()    , s.points  .end() )  },
        { "paths",     std::vector<path_id>     ( s.paths   .begin()    , s.paths   .end() )  }
    };
          
    return;
}


//      Selection_t     : "from_json"
//
template <EditorCFGTraits CFG_>
inline void from_json(const nlohmann::json & j, Selection_t<CFG_> & s)
{
    using   sel         = Selection_t<CFG_>         ;
    using   vertex_id   = sel::vertex_id            ;
    using   point_id    = sel::point_id             ;
    using   path_id     = sel::path_id              ;
    
    

    std::vector<vertex_id>      vs  ;       j.at("vertices")    .get_to(vs)                     ;
    std::vector<point_id>       ps  ;       j.at("points"  )    .get_to(ps)                     ;
    std::vector<path_id >       pa  ;       j.at("paths"   )    .get_to(pa)                     ;

    s.vertices  .clear()            ;       s.vertices          .insert(vs.begin(), vs.end())   ;
    s.points    .clear()            ;       s.points            .insert(ps.begin(), ps.end())   ;
    s.paths     .clear()            ;       s.paths             .insert(pa.begin(), pa.end())   ;
    
    return;
}






// *************************************************************************** //
//      4C. AUXILIARY |         BOX-DRAG STATE.
// *************************************************************************** //

//  "DragState"
//
enum class DragState : uint8_t {
      None = 0
    , Lasso
    , Moving
    , Scaling
    , COUNT
};
//
//  "DEF_DRAGSTATE_NAMES"
static constexpr cblib::EnumArray< DragState, const char * >
    DEF_DRAGSTATE_NAMES  = { {
        "None",     "Lasso",        "Moving",       "Scaling"
} };




//  "BoxDrag_t"
//      struct (add fields for handle_ws0, orig_w, orig_h after mouse_ws0)
//
template <EditorCFGTraits CFG_>
struct BoxDrag_t
{
    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    _USE_EDITOR_CFG_ALIASES
    template<typename T_, typename E_>
    using                               EnumArray                       = cblib::EnumArray<T_, E_>;
    using                               Anchor                          = utl::Anchor;
    //
    using                               PathHit                         = PathHit_t     <path_id, vertex_id>    ;
    
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      0. |    STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    static constexpr auto &             ms_DRAGSTATE_NAMES              = DEF_DRAGSTATE_NAMES;
    
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      STATIC INLINE HELPER FUNCTIONS.
    // *************************************************************************** //
    
    //  "AnchorToCursor"            FORMERLY: "..."
    [[nodiscard]]
    static inline ImGuiMouseCursor      AnchorToCursor                      (const Anchor which) noexcept
    {
        switch (which)
        {
            case Anchor::NorthWest  : case Anchor::SouthEast :      { return ImGuiMouseCursor_ResizeNWSE;   }       //  NW  , SE
            case Anchor::NorthEast  : case Anchor::SouthWest :      { return ImGuiMouseCursor_ResizeNESW;   }       //  NE  , SW
            case Anchor::North      : case Anchor::South     :      { return ImGuiMouseCursor_ResizeNS ;    }       //  N   , S
            case Anchor::East       : case Anchor::West      :      { return ImGuiMouseCursor_ResizeEW;     }       //  E   , W
            //
            default:                                                { return ImGuiMouseCursor_Arrow;        }
        }
    }

    //  "AnchorToWorldPos"          FORMERLY: "_bbox_handle_pos_ws"
    [[nodiscard]] static inline ImVec2  AnchorToWorldPos                    (const Anchor which, const ImVec2 & tl, const ImVec2 & br) noexcept
    {
        const ImVec2    c   { (tl.x + br.x) * 0.5f, (tl.y + br.y) * 0.5f };     //  Compute Center.
        switch (which)
        {
            case Anchor::NorthWest :        { return { tl.x    , tl.y };    }   //  NW
            case Anchor::North     :        { return { c.x     , tl.y };    }   //  N
            case Anchor::NorthEast :        { return { br.x    , tl.y };    }   //  NE
            case Anchor::East      :        { return { br.x    , c.y  };    }   //  E
            case Anchor::SouthEast :        { return { br.x    , br.y };    }   //  SE
            case Anchor::South     :        { return { c.x     , br.y };    }   //  S
            case Anchor::SouthWest :        { return { tl.x    , br.y };    }   //  SW
            case Anchor::West      :        { return { tl.x    , c.y  };    }   //  W (7)
            //
            default                :        { return {   };                 }   //  DEFAULT.
        }
    }
    
    //  "OppositePivot"             FORMERLY: "_bbox_pivot_opposite"
    [[nodiscard]] static inline ImVec2  OppositePivot                       (const Anchor which, const ImVec2 & tl, const ImVec2 & br) noexcept
    {
        const ImVec2    c   { (tl.x + br.x) * 0.5f, (tl.y + br.y) * 0.5f };
        switch (which)
        {
            case Anchor::NorthWest :        { return { br.x   , br.y };     }   //  NW → pivot SE
            case Anchor::North     :        { return { c.x    , br.y };     }   //  N  → pivot S-mid
            case Anchor::NorthEast :        { return { tl.x   , br.y };     }   //  NE → pivot SW
            case Anchor::East      :        { return { tl.x   , c.y  };     }   //  E  → pivot W-mid
            case Anchor::SouthEast :        { return { tl.x   , tl.y };     }   //  SE → pivot NW
            case Anchor::South     :        { return { c.x    , tl.y };     }   //  S  → pivot N-mid
            case Anchor::SouthWest :        { return { br.x   , tl.y };     }   //  SW → pivot NE
            case Anchor::West      :        { return { br.x   , c.y  };     }   //  W  → pivot E-mid
            //
            default                :        { return {   };                 }   //  DEFAULT.
        }
    }
    
    //  "OppositeAnchor"
    [[nodiscard]] static inline Anchor  OppositeAnchor                      (const Anchor which) noexcept
    {
        switch (which) {
            case Anchor::NorthWest :        { return Anchor::SouthEast;     }
            case Anchor::North     :        { return Anchor::South;         }
            case Anchor::NorthEast :        { return Anchor::SouthWest;     }
            case Anchor::East      :        { return Anchor::West;          }
            case Anchor::SouthEast :        { return Anchor::NorthWest;     }
            case Anchor::South     :        { return Anchor::North;         }
            case Anchor::SouthWest :        { return Anchor::NorthEast;     }
            case Anchor::West      :        { return Anchor::East;          }
            default                :        { return which;                 }
        }
    }

    //  "AnchorToSign"
    [[nodiscard]] static inline ImVec2  AnchorToSign                        (const Anchor which) noexcept
    {
        switch (which) {
            case Anchor::NorthWest :        { return { -1.0f,   -1.0f };    }
            case Anchor::North     :        { return {  0.0f,   -1.0f };    }
            case Anchor::NorthEast :        { return {  1.0f,   -1.0f };    }
            case Anchor::East      :        { return {  1.0f,    0.0f };    }
            case Anchor::SouthEast :        { return {  1.0f,    1.0f };    }
            case Anchor::South     :        { return {  0.0f,    1.0f };    }
            case Anchor::SouthWest :        { return { -1.0f,    1.0f };    }
            case Anchor::West      :        { return { -1.0f,    0.0f };    }
            default                :        { return { -1.0f,    0.0f };    }
        }
    }
    
    //  "SafeDiv"
    template <typename T> requires std::floating_point<T>
    static inline T                     SafeDiv                             (T num, T den) noexcept
        { constexpr T eps = cblib::math::numerics::cv_default_rel_tol_v<T>();  return (std::fabs(den) > eps) ? (num / den) : T(1); }
    
    //  "IsDiagonal"
    [[nodiscard]] static inline bool    IsDiagonal                          (const Anchor handle) noexcept {
        switch (handle) {
            case Anchor::NorthWest :    case Anchor::NorthEast :
            case Anchor::SouthEast :    case Anchor::SouthWest :    { return true;  }
            default :                                               { return false; }
        }
    }
    
    //  "IsVertical"
    [[nodiscard]] static inline bool    IsVertical                          (const Anchor handle) noexcept {
        switch (handle) {
            case Anchor::North :    case Anchor::South :    { return true;  }
            default :                                       { return false; }
        }
    }
    
    //  "IsHorizontal"
    [[nodiscard]] static inline bool    IsHorizontal                        (const Anchor handle) noexcept {
        switch (handle) {
            case Anchor::East :     case Anchor::West :     { return true;  }
            default :                                       { return false; }
        }
    }
    
    
    
//
// *************************************************************************** //
// *************************************************************************** //   END "CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//      1.          DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //
    //
    //                          STATE-VARIABLES.
    DragState                       state                               = DragState::None;
    //
    //
    Anchor                          handle_idx                          = static_cast<Anchor>( 0 );
    ImVec2                          anchor_ws                           = { 0.0f,    0.0f };
    ImVec2                          bbox_tl_ws                          = { 0.0f,    0.0f };
    ImVec2                          bbox_br_ws                          = { 0.0f,    0.0f };
    //
    //                          DATA.
    std::vector<vertex_id>          v_ids;               //     selected vertex IDs
    std::vector<ImVec2>             v_orig;              //     original positions (same order)
    //
    //
    //                          MIGRATED FROM "MoveDrag_t".
#ifdef _EDITOR_REDUCE_REDUNDANCY
    //  ImVec2                          anchor_ws                           {0.0f,      0.0f};   // top-left of selection at mouse-press
    ImVec2                          press_ws                            {0.0f,      0.0f};
    ImVec2                          cum_delta                           {0.0f,      0.0f};   // accumulated world-space translation
//
#endif  //  _EDITOR_REDUCE_REDUNDANCY  //
    //
    //
    //                          EXISTING STUFF.
    ImVec2                          mouse_ws0                           = {0.0f,    0.0f};
    ImVec2                          handle_ws0;                                 //  initial world‑space position of the dragged handle
    float                           orig_w                              = 1.0f;                 //  original bbox width  (world units)
    float                           orig_h                              = 1.0f;                 //  original bbox height (world units)
    bool                            first_frame                         = true;

    // *************************************************************************** //
    //      1. |    NEW STUFF.
    // *************************************************************************** //
    struct ViewCache {
        bool                            valid                               = false;    // recompute if false or revs changed
        bool                            visible                             = false;    // draw/hover only if true

        // NEW.
        ImVec2                          tl_tight_ws                         { 0.0f,     0.0f },
                                        br_tight_ws                         { 0.0f,     0.0f };  // NEW: content (tight) bounds in W
        //
        // Expanded bbox (world & pixels)
        ImVec2                          tl_ws                               { 0.0f,     0.0f };
        ImVec2                          br_ws                               { 0.0f,     0.0f };
        ImVec2                          tl_px                               { 0.0f,     0.0f };
        ImVec2                          br_px                               { 0.0f,     0.0f };

        // Handle anchors (world & pixels) and their pixel rects
        EnumArray<Anchor, ImVec2>       handle_ws                           {   };
        EnumArray<Anchor, ImVec2>       handle_px                           {   };
        EnumArray<Anchor, ImRect>       handle_rect_px                      {   };
        //
        std::optional<Anchor>           hover_idx                           = std::nullopt;     // -1 = none

        // Last-seen revision stamps (compare to Editor’s counters)
        uint64_t                        sel_seen                            = 0;
        uint64_t                        geom_seen                           = 0;
        uint64_t                        cam_seen                            = 0;
        uint64_t                        style_seen                          = 0;
    };
    ViewCache                       view                                {   };
    
    // *************************************************************************** //
    //      1. |    MIGRATED FROM "Selection_t".
    // *************************************************************************** //
    std::unordered_set<vertex_id>   vertices                            {   };
    std::unordered_set<point_id>    points                              {   };
    std::unordered_set<path_id>     paths                               {   };     // ← NEW
    mutable std::optional<Hit>      hovered                             { std::nullopt };
    
    
//
// *************************************************************************** //
// *************************************************************************** //   END "DATA-MEMBERS".

    
   
// *************************************************************************** //
//
//      2.B.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      QUERY FUNCTIONS.
    // *************************************************************************** //
    
    
    //  "IsMoving"
    [[nodiscard]] inline bool           IsMoving                            (void) const noexcept   { return ( this->state == DragState::Moving );      }
    [[nodiscard]] inline bool           IsScaling                           (void) const noexcept   { return ( this->state == DragState::Scaling );     }
    [[nodiscard]] inline bool           IsActive                            (void) const noexcept   { return ( this->state != DragState::None );        }
    [[nodiscard]] inline bool           IsIdle                              (void) const noexcept   { return ( this->state == DragState::None );        }
    
    
    //  "IsScaling"
    //  [[nodiscard]] inline bool           IsScaling                           (void) const noexcept   { return ( this->active  &&  this->view.hover_idx.has_value() ); }
    




    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      SETTER / GETTER FUNCTIONS...
    // *************************************************************************** //
    
    //  "StartMoving"
    inline void                         StartMoving                         (void) noexcept         { this->state = DragState::Moving;      }
    inline void                         StopMoving                          (void) noexcept         { this->state = DragState::None;        }
    
    //  "StartScaling"
    inline void                         StartScaling                        (void) noexcept         { this->state = DragState::Scaling;     }
    inline void                         StopScaling                         (void) noexcept         { this->state = DragState::None;        }
    
    
    
    //  "GetDragState"
    [[nodiscard]] inline DragState      GetDragState                        (void) const noexcept   { return this->state;                               }
    [[nodiscard]] inline const char *   GetDragStateName                    (void) const noexcept   { return this->ms_DRAGSTATE_NAMES[ this->state ];   }



    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    
    //  "clear"
    inline void                         clear                               (void) noexcept { this->reset(); }
    
    //  "reset"
    inline void                         reset                               (void) noexcept
    {
        //      1.      STATE...
        this->first_frame       = true;
        state                   = DragState::None;
        //
        this->handle_idx        = static_cast<Anchor>( 0 );
        this->anchor_ws         = { 0.0f    , 0.0f };
        this->bbox_tl_ws        = { 0.0f    , 0.0f };
        this->bbox_br_ws        = { 0.0f    , 0.0f };


        //      2.      DATA...
        this->v_ids             .clear();
        this->v_orig            .clear();
        //
        this->vertices          .clear();
        this->points            .clear();
        this->paths             .clear();
        this->hovered           .reset();
        //
        this->view              = {   };


        //      3.      MIGRATED FROM "MoveDrag_t"...
    #ifdef _EDITOR_REDUCE_REDUNDANCY
        this->press_ws          = { 0.0f    , 0.0f };
        this->cum_delta         = { 0.0f    , 0.0f };
    #endif  //  _EDITOR_REDUCE_REDUNDANCY  //


        //      4.      EXISTING STUFF...
        this->mouse_ws0         = { 0.0f    , 0.0f };
        this->handle_ws0        = { 0.0f    , 0.0f };
        this->orig_w            = 1.0f;
        this->orig_h            = 1.0f;
        
    
        return;
    }



    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      OPERATION FUNCTIONS.
    // *************************************************************************** //

    //  "_selection_bounds"
    template <typename RenderCTX>
    inline bool                         _selection_bounds                   (ImVec2 & tl, ImVec2 & br, const RenderCTX & ctx) const
    {
    /*
        using           cblib::math::is_close;
        namespace       bez             = cblib::math::bezier;
        bool            have_any        = false;
        //
        auto            add_pt          = [&](const ImVec2 & p) {
            if (!have_any) { tl = br = p; have_any = true; }
            else {
                tl.x = std::min(tl.x, p.x); tl.y = std::min(tl.y, p.y);
                br.x = std::max(br.x, p.x); br.y = std::max(br.y, p.y);
            }
        };
        auto            add_box         = [&](const ImVec2& btl, const ImVec2& bbr) { add_pt(btl); add_pt(bbr); };



        // A) Vertex anchors
        for (vertex_id vid : this->vertices)
        {
            if ( const Vertex* v = find_vertex(m_vertices, vid) )       { add_pt(ImVec2{v->x, v->y}); }
        }

        // B) Per-path segment AABBs (tight, degree-aware)
        for (path_id pidx : m_sel.paths)
        {
            if ( static_cast<size_t>(pidx) >= m_paths.size() )      { continue; }
            
            const Path &    p   = m_paths[pidx];
            const size_t    N   = p.size();
            
            if ( N < 2 )                    { continue; }

            const bool      is_area         = p.IsArea();
            const size_t    seg_cnt         = N - (is_area ? 0u : 1u);

            bool            first           = true;         // per-path accumulator flag
            ImVec2          p_tl            {   },
                            p_br            {   };          // per-path bbox


            for (size_t si = 0; si < seg_cnt; ++si)
            {
                const vertex_id     a_id        = p.verts[si];
                const vertex_id     b_id        = p.verts[(si + 1) % N];

                const Vertex *      a           = ctx.callbacks.get_vertex(ctx.callbacks.vertices, a_id);
                const Vertex *      b           = ctx.callbacks.get_vertex(ctx.callbacks.vertices, b_id);
                if ( !a || !b )                 { continue; }

                const ImVec2        P0          {a->x, a->y};
                const ImVec2        P3          {b->x, b->y};


                const ImVec2        out_eff     = a->EffectiveOutHandle();
                const ImVec2        in_eff      = b->EffectiveInHandle();


                const bool          out_zero    = ( is_close(out_eff.x, 0.0f)  &&  is_close(out_eff.y, 0.0f) );
                const bool          in_zero     = ( is_close(in_eff.x , 0.0f)  &&  is_close(in_eff.y , 0.0f) );
                const bool          linear      = ( out_zero  &&  in_zero );


                //      CASE 1 :    SPLINE / LINEAR SEGMENT:        Use Endpoints ONLY.
                if (linear)
                {=
                    ImVec2          seg_tl      { std::min(P0.x, P3.x), std::min(P0.y, P3.y) };
                    ImVec2          seg_br      { std::max(P0.x, P3.x), std::max(P0.y, P3.y) };
                    if (first) { p_tl = seg_tl; p_br = seg_br; first = false; }
                    else {
                        p_tl.x = std::min(p_tl.x, seg_tl.x); p_tl.y = std::min(p_tl.y, seg_tl.y);
                        p_br.x = std::max(p_br.x, seg_br.x); p_br.y = std::max(p_br.y, seg_br.y);
                    }
                }
                //      CASE 2 :    QUADRATIC SEGMENT:              C = A + out_eff.
                else if ( a->IsQuadratic() )
                {
                    const ImVec2        C               { P0.x + out_eff.x, P0.y + out_eff.y };
                    auto                [q_tl, q_br]    = bez::bbox_quadratic_tight<ImVec2, float>(P0, C, P3);
                    
                    if (first) { p_tl = q_tl; p_br = q_br; first = false; }
                    else {
                        p_tl.x = std::min(p_tl.x, q_tl.x); p_tl.y = std::min(p_tl.y, q_tl.y);
                        p_br.x = std::max(p_br.x, q_br.x); p_br.y = std::max(p_br.y, q_br.y);
                    }
                }
                //
                //      CASE 3 :    CUBIC SEGMENT:                  Pass path accumulators directly (matches bbox_cubic_tight signature)
                else {
                    const ImVec2        P1          { P0.x + out_eff.x, P0.y + out_eff.y };
                    const ImVec2        P2          { P3.x + in_eff.x , P3.y + in_eff.y  };
                    bez::bbox_cubic_tight<ImVec2, float>(P0, P1, P2, P3, p_tl, p_br, first);
                }
            }
            if ( !first )       { add_box(p_tl, p_br); }
        }

        return have_any;
        */
        return false;
    }



    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      UTILITY FUNCTIONS.
    // *************************************************************************** //

    //  "TightWSPos"
    [[nodiscard]] inline ImVec2         TightWSPos                          (Anchor h, ImVec2 handle_ws, const float margin) noexcept
    {
        const ImVec2    hsign   = this->AnchorToSign(h);
        return { handle_ws.x - hsign.x * margin,  handle_ws.y - hsign.y * margin };
    }



//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.



    
//
//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "BoxDrag" INLINE CLASS DEFINITION.






//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 4.  "AUXILIARY STATES" ]].












// *************************************************************************** //
//
//
//
//      5.    SERIALIZATION STUFF...
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



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 5.  "SERIALIZATION" ]].







  
  



// *************************************************************************** //
//
//
//
//      5.    OVERLAY AND UI STUFF...
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
    utl::Anchor                 src_anchor          = utl::Anchor::NorthWest;            //  top-centre of the window
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
//      5B.       EDITOR "OVERLAY" DEFINITIONS.
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
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 5.  "OVERLAY" ]].












// *************************************************************************** //
//
//
//
//      6.      DEBUGGER STATE STUFF...
// *************************************************************************** //
// *************************************************************************** //

//  "DebuggerState_t"
//
template< typename VID, typename PtID, typename LID, typename PID, typename ZID, typename HID >
struct DebuggerState_t
{
// *************************************************************************** //
//      0.      CONSTEXPR CONSTANTS...
// *************************************************************************** //
        static constexpr ImGuiChildFlags        ms_FLAGS        = ImGuiChildFlags_Borders;// | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY;



// *************************************************************************** //
//
//
//      1.          CLASS DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      IMPORTANT DATA-MEMBERS.
    // *************************************************************************** //
    struct DebugItem {
        std::string                 uuid;
        bool                        open;
        ImGuiChildFlags             flags               = ms_FLAGS;
        std::function<void()>       render_fn           {   };
    };
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      GENERIC DATA.
    // *************************************************************************** //
    bool                            show_more_info      = false;
    std::vector<DebugItem>          windows             = {   };
    
//
// *************************************************************************** //
// *************************************************************************** //   END "CLASS DATA-MEMBERS".

    
   
// *************************************************************************** //
//
//
//      2.C.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
    
    
    // *************************************************************************** //
    
//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.


    
//
//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "DebuggerState_t" INLINE CLASS DEFINITION.



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 6.  "DEBUGGER" ]].












// *************************************************************************** //
//
//
//
//      7.      UNDO/REDO STATE...
// *************************************************************************** //
// *************************************************************************** //









//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 6.  "UNDO/REDO" ]].
























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

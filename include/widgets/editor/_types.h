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
    , ICON_FA_PEN_NIB
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
    DEF_IORESULT_NAMES  = { {
        "OK",   "I/O Error",    "Parsing Error",    "Version Mismatch"
} };



// *************************************************************************** //
//      0C. EDITOR |        CAPABILITY BITS.
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
    , GridSnap                  , GridShow                  , GridDecrease              , GridIncrease          , GridDensityValue
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
    /*  SelectionSurface    */      , "Toggle ability to select SURFACES"
    /*  SelectionEdge       */      , "Toggle ability to select EDGES"
    /*  SelectionVertex     */      , "Toggle ability to select VERTICES"
//
//      3.      GRID CONTROLS.
    /*  GridSnap            */      , "Toggle Snap-To-Grid  [SHIFT G]"
    /*  GridShow            */      , "Toggle visibility of the grid"
    /*  GridDecrease        */      , "Halve the grid resolution  [CTRL -]"
    /*  GridIncrease        */      , "Double the grid resolution  [CTRL +]"
    /*  GridDensityValue    */      , "Grid resolution (quantization value)"
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
    static constexpr ImGuiHoveredFlags  ms_TOOLTIP_HOVER_FLAGS          = ImGuiHoveredFlags_Stationary | ImGuiHoveredFlags_DelayNormal;
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
    [[nodiscard]] inline bool           IsActive                        (void) const noexcept   { return ( this->active.load  (std::memory_order_release)  );   }
    [[nodiscard]] inline bool           IsInactive                      (void) const noexcept   { return ( !this->active.load (std::memory_order_release)  );   }
    
    
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
//
template <typename HID>
struct Hit_t {
    using           Type            = HitType;
//
//
    Type            type            = Type::None;
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
// *************************************************************************** //   END "1.  MAIN OBJECT TYPES".












// *************************************************************************** //
//
//
//
//      2.      AUXILIARY EDITOR TYPES / STATE VARIABLES...
// *************************************************************************** //
// *************************************************************************** //



//  "GridSettings"
struct GridSettings {
    float   snap_step       = 20.0f;   // quantize condition for grid-snapping.
    bool    visible         = true;
    bool    snap_on         = false;
};



// *************************************************************************** //
//      2B. AUXILIARY |         PER-FRAME INTERACTIONS.
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







// *************************************************************************** //
//      2C. AUXILIARY |         SELECTION STATE.
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
    j = {
        { "vertices",  std::vector<VID>  (s.vertices.begin(), s.vertices.end()) },
        { "points",    std::vector<PtID> (s.points  .begin(), s.points  .end()) },
        { "paths",     std::vector<PID>  (s.paths   .begin(), s.paths   .end()) }
    };
          
    return;
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
    
    return;
}




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
struct PenState_t {
    bool            active                  = false;
//
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
// *************************************************************************** //   END "3.  PRIMARY TOOL STATES".









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
//
//
//
//      4.      AUXILIARY STATES...
// *************************************************************************** //
// *************************************************************************** //

//  "MoveDrag_t"
//
template <ObjectCFGTraits CFG, typename Vertex>
struct MoveDrag_t
{
    //  USE_OBJECT_CFG_ALIASES(CFG);
    _USE_OBJECT_CFG_ALIASES
//
//
//
    bool                        active              = false;
//
    ImVec2                      anchor_ws           {0.0f,      0.0f};   // top-left of selection at mouse-press
    ImVec2                      press_ws            {0.0f,      0.0f};
    ImVec2                      cum_delta           {0.0f,      0.0f};   // accumulated world-space translation
//
//
    std::vector<vertex_id>      v_ids;               //     selected vertex IDs
    std::vector<ImVec2>         v_orig;              //     original positions (same order)
};





//  "BBoxAnchor"
//      Defiled in the order of unit circle angles (0deg = +x-axis) and DEFAULT = 0 = CENTER.
//
enum class BBoxAnchor : uint8_t {
      Center = 0
    , East, NorthEast, North, NorthWest, West, SouthWest, South, SouthEast
//
    , COUNT
};



//  "BoxDrag_t"
//      struct (add fields for handle_ws0, orig_w, orig_h after mouse_ws0)
//
template <ObjectCFGTraits CFG, typename Vertex>
struct BoxDrag_t
{
    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    _USE_OBJECT_CFG_ALIASES
    template<typename T_, typename E_>
    using                               EnumArray                       = cblib::EnumArray<T_, E_>;
    using                               Anchor                          = BBoxAnchor;
    
//
// *************************************************************************** //
// *************************************************************************** //   END "CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//      1.          DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //
    
    bool                        active                              = false;
    Anchor                      handle_idx                          = static_cast<Anchor>( 0 );
    ImVec2                      anchor_ws                           = { 0.0f,    0.0f };
    ImVec2                      bbox_tl_ws                          = { 0.0f,    0.0f };
    ImVec2                      bbox_br_ws                          = { 0.0f,    0.0f };
//
    std::vector<vertex_id>      v_ids;
    std::vector<ImVec2>         v_orig;
//
    ImVec2                      mouse_ws0                           = {0.0f,    0.0f};
    ImVec2                      handle_ws0;                                 //  initial world‑space position of the dragged handle
    float                       orig_w                              = 1.0f;                 //  original bbox width  (world units)
    float                       orig_h                              = 1.0f;                 //  original bbox height (world units)
    bool                        first_frame                         = true;

    // *************************************************************************** //
    //      NEW STUFF.
    // *************************************************************************** //
    struct ViewCache {
        bool                            valid                               = false;    // recompute if false or revs changed
        bool                            visible                             = false;    // draw/hover only if true

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
    ViewCache                           view                                {   };
    
    
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
    
    //  "IsScaling"
    [[nodiscard]] inline bool           IsScaling                           (void) const noexcept   { return ( this->active  &&  this->view.hover_idx.has_value() ); }

    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    
    //  "AnchorToCursor"            FORMERLY: "..."
    [[nodiscard]]
    static inline ImGuiMouseCursor      AnchorToCursor                      (const Anchor which) noexcept
    {
        switch (which)
        {
            case Anchor::NorthWest  : case Anchor::SouthEast :      { return ImGuiMouseCursor_ResizeNWSE;   }       //  NW  , SE
            case Anchor::NorthEast  : case Anchor::SouthWest :      { return ImGuiMouseCursor_ResizeNESW;   }       //  NE  , SW
            case Anchor::North      : case Anchor::South     :      { return ImGuiMouseCursor_ResizeEW ;    }       //  N   , S
            case Anchor::East       : case Anchor::West      :      { return ImGuiMouseCursor_ResizeNS;     }       //  E   , W
            //
            default:                                                { return ImGuiMouseCursor_Arrow;        }
        }
    }

    //  "AnchorToWorldPos"          FORMERLY: "_bbox_handle_pos_ws"
    [[nodiscard]]
    static inline ImVec2                AnchorToWorldPos                    (const Anchor which, const ImVec2 & tl, const ImVec2 & br) noexcept
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
    [[nodiscard]]
    static inline ImVec2                OppositePivot                       (const Anchor which, const ImVec2 & tl, const ImVec2 & br) noexcept
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

    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      SETTER / GETTER FUNCTIONS...
    // *************************************************************************** //
    
    //  "SafeDiv"
    //  static inline T                     SafeDiv                             (T num, T den) noexcept
    //      { constexpr T eps = cblib::math::default_rel_tol<T>();;  return (std::fabs(den) > eps) ? (num / den) : T(1); }

    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      STATIC INLINE HELPER FUNCTIONS.
    // *************************************************************************** //
    
    //  "SafeDiv"
    template <typename T> requires std::floating_point<T>
    static inline T                     SafeDiv                             (T num, T den) noexcept
        { constexpr T eps = cblib::math::default_rel_tol<T>();;  return (std::fabs(den) > eps) ? (num / den) : T(1); }
    
    
    
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
// *************************************************************************** //   END "3.  TOOL STATES".












// *************************************************************************** //
//
//
//
//      4.    SERIALIZATION STUFF...
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
// *************************************************************************** //   END "4.  SERIALIZATION"







  
  



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
// *************************************************************************** //   END "5.  OVERLAY"












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
// *************************************************************************** //   END "6.  DEBUGGER" STATE.











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

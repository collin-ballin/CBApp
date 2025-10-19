/***********************************************************************************
*
*       ********************************************************************
*       ****     _ S T A T E _ A N D _ S T Y L E . H  ____  F I L E     ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      September 19, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBWIDGETS_EDITOR_STATE_AND_STYLE_H
#define _CBWIDGETS_EDITOR_STATE_AND_STYLE_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
#include "utility/utility.h"
#include "widgets/editor/_constants.h"
//
//
//  #ifndef _CBAPP_EDITOR_OBJECTS_H
//  # include "widgets/editor/objects/objects.h"
//  #endif //  _CBAPP_EDITOR_OBJECTS_H  //
//
//
#ifndef _CBWIDGETS_EDITOR_TYPES_H
# include "widgets/editor/_types.h"
#endif //  _CBWIDGETS_EDITOR_TYPES_H  //



//  0.2     STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <cstdlib>          // C-Headers...
#include <stdio.h>
#include <unistd.h>
#include <cassert>
#include <random>
#include <tuple>
#include <utility>

#include <algorithm>
#include <array>
#include <unordered_set>
#include <optional>
#include <variant>

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
//      1.      OTHER...
// *************************************************************************** //
// *************************************************************************** //

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


//  "to_json"
//
template <typename ZID>
inline void to_json(nlohmann::json & j, const ZOrderCFG_t<ZID> & obj) {
    j = {
        { "Z_EDITOR_BACK",          obj.Z_EDITOR_BACK       },
        { "Z_FLOOR_USER",           obj.Z_FLOOR_USER        },
        { "Z_EDITOR_FRONT",         obj.Z_EDITOR_FRONT      },
        { "Z_CEIL_USER",            obj.Z_CEIL_USER         },
        { "RENORM_THRESHOLD",       obj.RENORM_THRESHOLD    }
    };
    
    return;
}


//  "from_json"
//
template <typename ZID>
inline void from_json(nlohmann::json & j, ZOrderCFG_t<ZID> & obj)
{
    j.at("Z_EDITOR_BACK")           .get_to(obj.Z_EDITOR_BACK       );
    j.at("Z_FLOOR_USER")            .get_to(obj.Z_FLOOR_USER        );
    j.at("Z_EDITOR_FRONT")          .get_to(obj.Z_EDITOR_FRONT      );
    j.at("Z_CEIL_USER")             .get_to(obj.Z_CEIL_USER         );
    j.at("RENORM_THRESHOLD")        .get_to(obj.RENORM_THRESHOLD    );
    
    return;
}



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "4.  REMAINDER".












// *************************************************************************** //
//
//
//
//      1.      "DRAG/DROP" PAYLOAD STATE...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//      1B. DRAG/DROP PAYLOADS  |       ACTUAL PAYLOAD DEFINITIONS.
// *************************************************************************** //

namespace ddrop {   //     BEGINNING NAMESPACE "ddrop"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//      1.      GLOBAL STUFF.
// *************************************************************************** //

static constexpr size_t         s_IMGUI_PAYLOAD_TYPE_STRING_SIZE    = 32;
static constexpr size_t         s_IMGUI_PAYLOAD_DATA_SIZE           = 64;


//  "ItemInfo"
//
struct ItemInfo
{
    const char *    name       ;
    const char *    type       ;
};




// *************************************************************************** //
//      2.      DEFINITION OF EACH PAYLOAD---TYPE.
// *************************************************************************** //

//  "ItemType"
//
enum class ItemType  : uint8_t {
      None = 0
    , PathRow
    , Empty
    , COUNT
};


//  "DEF_ITEM_INFOS"
//
static constexpr cblib::EnumArray< ItemType, ItemInfo >
DEF_ITEM_INFOS = { {
    /*  None        */        ItemInfo{ "None"             , nullptr           }
    /*  PathRow     */      , ItemInfo{ "Path Row"         , "PATH_ROW"        }
    /*  Empty     */        , ItemInfo{ "Empty"            , "EMPTY"           }   //  placeholder for now (unused).
} };


    

//      ENUM #1 :   "PathRow"...
//
//                  1A.     ENFORCE  *NAME*  RESTRICTIONS FOR DRAG/DROP PAYLOAD...
static_assert(
    DEF_ITEM_INFOS[ItemType::PathRow].name                                                       &&      //  "name" is NOT NULL.
    sizeof(DEF_ITEM_INFOS[ItemType::PathRow].name)       != 0                                            //  "name" is NOT empty-string.
);
//                  1B.     ENFORCE  *TYPE*  RESTRICTIONS FOR DRAG/DROP PAYLOAD...
static_assert(
    DEF_ITEM_INFOS[ItemType::PathRow].type                                                       &&      //  "type" is NOT NULL.
    sizeof(DEF_ITEM_INFOS[ItemType::PathRow].type)       != 0                                    &&      //  "type" is NOT empty-string.
    sizeof(DEF_ITEM_INFOS[ItemType::PathRow].type)       <= s_IMGUI_PAYLOAD_TYPE_STRING_SIZE     &&      //  ImGui DRAG/DROP Type-Strings have MAX. SIZE of 32-characters.
    DEF_ITEM_INFOS[ItemType::PathRow].type[0]            != '_'                                          //  Type-Strings beginning with "_" are reserved for ImGui-Types.
);
//
//      ENUM #2 :   "Empty"...
static_assert(
    DEF_ITEM_INFOS[ItemType::Empty].name                                                         &&
    sizeof(DEF_ITEM_INFOS[ItemType::Empty].name)         != 0
);
static_assert(
    DEF_ITEM_INFOS[ItemType::Empty].type                                                         &&
    sizeof(DEF_ITEM_INFOS[ItemType::Empty].type)         != 0                                    &&
    sizeof(DEF_ITEM_INFOS[ItemType::Empty].type)         <= s_IMGUI_PAYLOAD_TYPE_STRING_SIZE     &&
    DEF_ITEM_INFOS[ItemType::Empty].type[0]              != '_'
);







[[nodiscard]] inline const char *   GetTypeString               (const ItemType k) noexcept   { return DEF_ITEM_INFOS[k].type; };
[[nodiscard]] inline const char *   GetypeName                  (const ItemType k) noexcept   { return DEF_ITEM_INFOS[k].name; };
    


//  "ParcelHeader"
//
struct ParcelHeader {
    static constexpr uint32_t       ms_MAGIC            = 0x44525044;   // 'DRPD'
    static constexpr uint16_t       ms_VERSION          = 1;
//
    uint32_t                        magic               = ms_MAGIC;
    uint16_t                        version             = ms_VERSION;
    uint16_t                        _pad                = 0;
};






// *************************************************************************** //
//      3.      DEFINE DATA-PAYLOADS (FOR EACH PAYLOAD-TYPE TO CARRY)...
// *************************************************************************** //

//  "PathRowParcel"
//      PLAIN-OLD-DATA (POD) STRUCT.
//
struct PathRowParcel
{
    // *************************************************************************** //
    //      0. |    NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    struct Payload
    {
        int                 src_row                 = -1;       //  visual row index when drag began
        int                 src_index               = -1;       //  backing m_paths index when drag began
    };
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      0. |    STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    static constexpr ItemType               type                            = ItemType::PathRow;
    static constexpr const char *           type_name                       = DEF_ITEM_INFOS[ type ].name;
    static constexpr const char *           type_string                     = DEF_ITEM_INFOS[ type ].type;
    static constexpr ImU32                  ms_PATH_DRAG_HINT_COLOR         = 0xFFBF661A;
    static constexpr float                  ms_PATH_DRAG_HINT_WIDTH         = 4.0f;
    
    
    
//
// *************************************************************************** //
// *************************************************************************** //   END "0.  CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//      1.          DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      1. |    IMPORTANT DATA-MEMBERS.
    // *************************************************************************** //
    Payload                             payload                         = {   };
    
//
// *************************************************************************** //
// *************************************************************************** //   END "1.  DATA-MEMBERS".



// *************************************************************************** //
//
//      2.A.        MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
    // *************************************************************************** //
    //      INITIALIZATION METHODS.         |   "init.cpp" ...
    // *************************************************************************** //
                                        PathRowParcel           (void) noexcept                 = default;
                                        ~PathRowParcel          (void)                          = default;
                                        
    //  In-Place Construction.
    explicit                            PathRowParcel           (Payload && p) noexcept
        : payload( std::move(p) )
    {   }
    
//
// *************************************************************************** //
// *************************************************************************** //   END "2A.  MEMBER FUNCS".

    
   
// *************************************************************************** //
//
//      2.B.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      2.B. |  CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //

    //  "GetPayload"
    [[nodiscard]] inline Payload *      GetPayload                          (void) noexcept         { return std::addressof( this->payload ); }
    [[nodiscard]] static inline size_t  GetPayloadSize                      (void) noexcept         { return sizeof( Payload ); }

    
    //  "ui_properties"
    inline void                         ui_properties                       (void)      { return; }
    
    //  "RenderDragHint"
    inline void                         RenderDragHint                      (ImDrawList * dl) const noexcept 
    {
        ImVec2          r_min           = ImGui::GetItemRectMin();
        ImVec2          r_max           = ImGui::GetItemRectMax();
        float           mid_y           = 0.5f * (r_min.y + r_max.y);
        float           mouse_y         = ImGui::GetIO().MousePos.y;
        bool            drop_above      = (mouse_y < mid_y);
        float           y               = drop_above ? r_min.y : r_max.y;

        dl->AddLine(ImVec2(r_min.x, y), ImVec2(r_max.x, y), this->ms_PATH_DRAG_HINT_COLOR, this->ms_PATH_DRAG_HINT_WIDTH);
        return;
    }
    
    //  "RenderDragTooltip"
    inline void                         RenderDragTooltip                   (ImDrawList * dl) const noexcept    { return; }
    
//
// *************************************************************************** //
// *************************************************************************** //   END "2B.  INLINE" FUNCTIONS.



//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "PathRowParcel" INLINE STRUCT DEFINITION.
//
//  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PathRowParcel, x, y, data, meta)




//  "EmptyParcel"
//      PLAIN-OLD-DATA (POD) STRUCT.
//
struct EmptyParcel
{
    // *************************************************************************** //
    //      0. |    NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    struct Payload
    {
        int32_t             src_row                 = -1;       //  visual row index when drag began
    };
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      0. |    STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    static constexpr ItemType               type                            = ItemType::Empty;
    static constexpr const char *           type_name                       = DEF_ITEM_INFOS[ type ].name;
    static constexpr const char *           type_string                     = DEF_ITEM_INFOS[ type ].type;
    
    
    
//
// *************************************************************************** //
// *************************************************************************** //   END "0.  CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//      1.          DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      1. |    IMPORTANT DATA-MEMBERS.
    // *************************************************************************** //
    Payload                             payload                         = {   };
    
//
// *************************************************************************** //
// *************************************************************************** //   END "1.  DATA-MEMBERS".



// *************************************************************************** //
//
//      2.A.        MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
    // *************************************************************************** //
    //      INITIALIZATION METHODS.         |   "init.cpp" ...
    // *************************************************************************** //
    //  explicit                        MyStruct                (app::AppState & );             //  Def. Constructor.
                                        EmptyParcel             (void) noexcept                 = default;
                                        ~EmptyParcel            (void)                          = default;
    
    // *************************************************************************** //
    //      DELETED FUNCTIONS.              |   ...
    // *************************************************************************** //
    
//
// *************************************************************************** //
// *************************************************************************** //   END "2A.  MEMBER FUNCS".

    
   
// *************************************************************************** //
//
//      2.B.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      2.B. |  CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //

    //  "GetPayload"
    [[nodiscard]] inline Payload *      GetPayload                          (void) noexcept         { return std::addressof( this->payload ); }
    [[nodiscard]] static inline size_t  GetPayloadSize                      (void) noexcept         { return sizeof( Payload ); }

    
    //  "ui_properties"
    inline void                         ui_properties                       (void)              { return; }
    
    //  "RenderDragHint"
    inline void                         RenderDragHint                      (ImDrawList * dl) const noexcept    { return; }
    
    //  "RenderDragTooltip"
    inline void                         RenderDragTooltip                   (ImDrawList * dl) const noexcept    { return; }
    
//
// *************************************************************************** //
// *************************************************************************** //   END "2B.  INLINE" FUNCTIONS.



//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "EmptyParcel" INLINE STRUCT DEFINITION.
//
//  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(EmptyParcel, x, y, data, meta)






// *************************************************************************** //
//      3.      DEFINE  "PARCEL"  STD::VARIANT ALIAS TYPE...
// *************************************************************************** //

//      1.      Ensure each Payload is POD Type...
static_assert(
      std::is_trivially_copyable_v<PathRowParcel::Payload>                      &&
      std::is_trivially_copyable_v<EmptyParcel::Payload>
    , "Each payload-type must be Plain-Old-Data struct (POD)"
);
//
//      2.      Ensure each Payload is small-size...
static_assert(
      ( sizeof(PathRowParcel::Payload)      <= s_IMGUI_PAYLOAD_DATA_SIZE )      &&
      ( sizeof(EmptyParcel::Payload)        <= s_IMGUI_PAYLOAD_DATA_SIZE )
    , "Each payload must be, at most, 64-bytes in memory"   //  ImGui DEEP-COPIES the Payload Data...
);


//  "Parcel"
//
using Parcel = std::variant<
      std::monostate
    , PathRowParcel
    , EmptyParcel
>;


//  "ParcelOf"      Type-Specific "Getter" Function.
//
template <ItemType K>
struct ParcelOf;

template <>
struct ParcelOf<ItemType::PathRow>      { using type = ddrop::PathRowParcel; };
template <>
struct ParcelOf<ItemType::Empty>        { using type = ddrop::EmptyParcel;   };



    
    
    
//  "parcel_type"
//
template <class T>
concept parcel_type =
    std::is_default_constructible_v<T>  &&
    requires(T t)
{
    //      1.      REQUIRED DATA---MEMBERS...
    typename T::Payload;                                                        //  nested payload type
    requires std::is_trivially_copyable_v<typename T::Payload>;
    //
    T::type;                                                                    //  Enum Type-Tag for the Parcel.
    T::type_name;                                                               //  Human-Readable Type-Name.
    T::type_string;                                                             //  Type-String for ImGui Payload.
    //
    //      2.      REQUIRED MEMBER---FUNCTIONS...
    { t.GetPayload() }          -> std::same_as<typename T::Payload*>;          //  pointer getter
    //  { t.GetPayloadSize() }      -> std::same_as<size_t>;                    //  sizeof(payload)
    //
    t.RenderDragHint    ( static_cast<ImDrawList*>(nullptr) );                  //  RenderDragHint(dl)
    t.RenderDragTooltip ( static_cast<ImDrawList*>(nullptr) );                  //  RenderDragTooltip(dl)
};


//  "parcel_type"
/*
template <class T>
concept parcel_type =
    //
    //      1.      TYPE-TRAITS FOR PARCEL-TYPE     (Typename T)...
    std::default_initializable<T>           &&
    std::is_default_constructible_v<T>      &&
    //
    //      2.      TYPE-TRAITS FOR PAYLOAD-TYPE    (Typename T::Payload)...
    requires
    {
        typename T::Payload;
        requires std::is_trivially_copyable_v<typename T::Payload>      ;
        requires std::default_initializable<T>                          ;
        requires std::is_default_constructible_v<T>                     ;
    }
    &&
    //
    //      3.      REQUIRED MEMBER-FUNCTIONS...
    requires        // static function presence/signature/noexcept
    {
        { T::GetPayloadSize() } noexcept -> std::same_as<size_t>;
    }
    &&
    requires (T t)
    {
        T::type; T::type_name; T::type_string;
        { t.GetPayload() } -> std::same_as<typename T::Payload*>;
        t.render(static_cast<ImDrawList*>(nullptr));
    };*/
    


//  "ExtractPayloadData"
//
template <parcel_type T, typename U = T::Payload>
[[nodiscard]] static inline bool ExtractPayloadData(const ImGuiPayload * src, U & dst) noexcept
{
    constexpr const size_t &    length      = s_IMGUI_PAYLOAD_TYPE_STRING_SIZE;
    const size_t                size        = T::GetPayloadSize();
    const bool                  valid       = (
        ( src != nullptr )                                              &&
        ( std::strncmp(src->DataType, T::type_string, length) == 0 )    &&
        ( static_cast<size_t>( src->DataSize ) == size )
    );

    if (!valid)     { return false; }   //  CASE 0 :    INVALID IMGUI PAYLOAD  [ Wrong---Type-Str,  Null---Ptr,  Wrong---Size ].
    
    
    std::memcpy( std::addressof(dst), src->Data, size );
    return true;
}





    


// *************************************************************************** //
//      4.      DEFINE "JSON" HELPERS FOR PAYLOADS...
// *************************************************************************** //





// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}// END NAMESPACE "ddrop".












// *************************************************************************** //
//      1C. DRAG/DROP PAYLOADS  |       PRIMARY STATE OBJECT.
// *************************************************************************** //

//  "ItemDDropper_t"
//      PLAIN-OLD-DATA (POD) STRUCT.
//
struct ItemDDropper_t
{
    // *************************************************************************** //
    //      0. |    NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    using                                   ItemType                        = ddrop::ItemType;
    //
    using                                   PathRowParcel                   = ddrop::PathRowParcel;
    using                                   EmptyParcel                     = ddrop::EmptyParcel;
    using                                   Parcel                          = ddrop::Parcel;
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      0. |    STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    static constexpr auto &                 ms_ITEM_INFOS                   = ddrop::DEF_ITEM_INFOS;
    //
    static constexpr float                  ms_DRAGDROP_DRAG_THRESHOLD      = 2.0f;
    
//
// *************************************************************************** //
// *************************************************************************** //   END "0.  CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//      1.          DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      1. |    PAYLOAD DATA-MEMBERS.
    // *************************************************************************** //
    ItemType                                m_type                          = { ItemType::None };
    Parcel                                  m_parcel                        {   };



    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      1. |    STATE DATA-MEMBERS.
    // *************************************************************************** //
    bool                                    m_is_dragging                   = false;    //  NEW: TRUE when DRAGGING.
    
//
// *************************************************************************** //
// *************************************************************************** //   END "1.  DATA-MEMBERS".



// *************************************************************************** //
//
//      2.A.        MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
    // *************************************************************************** //
    //      INITIALIZATION METHODS.         |   "init.cpp" ...
    // *************************************************************************** //
    //  explicit                        ItemDDropper_t                      (app::AppState & );             //  Def. Constructor.
                                        ItemDDropper_t                      (void)                          = default;
                                        ~ItemDDropper_t                     (void)                          = default;
    
    // *************************************************************************** //
    //      DELETED FUNCTIONS.              |   ...
    // *************************************************************************** //
    //                                      ItemDDropper_t                  (const ItemDDropper_t &     src)         = delete;   //  Copy. Constructor.
    //                                      ItemDDropper_t                  (ItemDDropper_t &&          src)         = delete;   //  Move Constructor.
    //  ItemDDropper_t &                    operator =                      (const ItemDDropper_t &     src)         = delete;   //  Assgn. Operator.
    //  ItemDDropper_t &                    operator =                      (ItemDDropper_t &&          src)         = delete;   //  Move-Assgn. Operator.
    
//
// *************************************************************************** //
// *************************************************************************** //   END "2A.  MEMBER FUNCS".

    
   
// *************************************************************************** //
//
//      2.B.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      2.B. |  STATIC INLINE MEMBER FUNCTIONS.
    // *************************************************************************** //
    
    //  "s_make_default_payload"
    //
    static inline Parcel                s_make_default_payload              (const ItemType k)
    {
        switch (k)
        {
            case ItemType::PathRow              : { return ddrop::PathRowParcel     {   };      }   //  default-constructed
            case ItemType::Empty                : { return ddrop::EmptyParcel       {   };      }
        //
            default                             : { return std::monostate           {   };      }   //  PayloadType::None or unknown
        }
    }
    
    
    //  "has_payload_v"
    //      Helpers to get the active parcel’s Payload address/size (for ImGui bytes)
    template <class TParcel>
    static constexpr bool               has_payload_v                       = requires(TParcel p)   { sizeof(typename TParcel::Payload); p.payload; };
    


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.B. |  PAYLOAD FUNCTIONS.
    // *************************************************************************** //
    
    //  "BeginDragDropSource"
    //
    inline bool                         BeginDragDropSource                 (const ImGuiDragDropFlags flags = ImGuiDragDropFlags_SourceNoPreviewTooltip) noexcept
    {
        IM_ASSERT( (this->m_type != ItemType::None) && "ItemDDropper_t: \"SetDragDropPayload()\" called without valid payload" );
        const char *    type_str        = this->GetItemTypeString();
        bool            accepted        = ImGui::BeginDragDropSource(flags);    //  Only when ImGui says a source begins this frame.
        
        std::visit([&](auto & parcel)
        {
            using P = std::decay_t<decltype(parcel)>;
            if constexpr ( !std::is_same_v<P, std::monostate>  &&  has_payload_v<P> )
            {
                accepted = ImGui::SetDragDropPayload(type_str, parcel.GetPayload(), parcel.GetPayloadSize());;
                
                //  if ( accepted ) {
                //      parcel.RenderDragTooltip();
                //  }
                this->m_is_dragging = accepted;
                //  IM_ASSERT( accepted && "ItemDDropper_t: ImGui failed to accept DragDropSource" );
            }
        }, this->m_parcel);

        return accepted;
    }
    
    
    //  "GetPayloadIfValid"
    //      Call inside an active ImGui::BeginDragDropTarget() scope.
    //
    template < ddrop::parcel_type T, typename U = T::Payload >
    [[nodiscard]] inline typename std::optional<U>
                                        GetPayloadIfValid                    (const ImGuiDragDropFlags flags = ImGuiDragDropFlags_None) noexcept
    {
        using                   namespace           ddrop;
        
        //      1.      OBTAIN PAYLOAD FROM IMGUI...
        U                       payload             = {   };
        const ImGuiPayload *    received            = ImGui::AcceptDragDropPayload(T::type_string, flags);
        
        if ( received  &&  ExtractPayloadData<T>(received, payload) )
        {
            return payload;
        }

        return std::nullopt;
    }
    
    
    //  "SetDragDropPayload"
    //      PREPARE the dropper with a parcel (copy or move), but DO NOT send to ImGui yet.
    //
    template <ddrop::parcel_type T>
    inline bool                         SetDragDropPayload                  (T && parcel)
    {
        //      CASE 0 :    FIRST-FRAME OF DRAG/DROP...
        if ( this->m_type == ItemType::None )       { this->m_type = T::type;   }
        else if ( T::type != this->m_type )         { return false;             }   //  IM_ASSERT( (this->m_type != T::type)  &&  "ItemDDropper_t: \"SetDragDropPayload()\" invoked while a payload of different-type is still active");
        
        
        this->m_parcel                  = std::forward<T>( parcel );   // stores into the std::variant
        const bool      sent            = ImGui::SetDragDropPayload( T::type_string, parcel.GetPayload(), parcel.GetPayloadSize() );
        return sent;
    }



    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.B. |  SETTER/GETTER FUNCTIONS.
    // *************************************************************************** //
    
    //  "SetItemType"
    inline void                         SetItemType                         (const ItemType type) noexcept
    {
        this->m_type        = type;
        this->m_parcel      = this->s_make_default_payload(type);
        return;
    }
        
    //  "GetItemTypeString"
    [[nodiscard]] inline const char *   GetItemTypeString                   (void) const noexcept   { return ms_ITEM_INFOS[this->m_type].type; };
    [[nodiscard]] inline const char *   GetItemTypeName                     (void) const noexcept   { return ms_ITEM_INFOS[this->m_type].name; };



    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.B. |  QUERY FUNCTIONS.
    // *************************************************************************** //
    
    //  "IsPayloadSet"
    [[nodiscard]] inline bool           IsPayloadSet                        (void) const noexcept
        { return (m_parcel.index() != 0); }
        //  { return std::holds_alternative<std::monostate>(m_parcel); }   //  *OR:*   return m_parcel.index() != 0; // assuming monostate is the first alternative

    //  "IsDragging"
    [[nodiscard]] inline bool           IsDragging                          (void) const noexcept   { return this->m_is_dragging; }
    
    
    //  "IsDragging"
    [[nodiscard]] inline bool           IsDragging                          (const ItemType type) const noexcept
    {
        return ( (this->m_type == type)  &&  (ImGui::IsDragDropActive()) );
    }



    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.B. |  OPERATION FUNCTIONS.
    // *************************************************************************** //
    
    //  "RenderDragHint"
    inline void                         RenderDragHint                      (ImDrawList * dl) const noexcept
    {
        std::visit( [&](auto & payload)
        {
            using T = std::decay_t<decltype(payload)>;
            if constexpr (!std::is_same_v<T, std::monostate>)   { payload.RenderDragHint(dl); }     //  Skip monostate (has no draw_ui).
        }, this->m_parcel);
        
        return;
    }



    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.B. |  CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    
    //  "StopDragging"
    inline void                         StopDragging                        (void) noexcept         { this->reset(); }
    
    //  "reset"
    inline void                         reset                               (void) noexcept
    {
        //      1.      BOOLEAN-STATE VALUES...
        this->m_is_dragging         = false;
        
        //      2.      PAYLOAD VALUES...
        this->m_type                = ItemType::None;
        this->m_parcel              = this->s_make_default_payload( ItemType::None );
        
        return;
    }
    
    
    
//
// *************************************************************************** //
// *************************************************************************** //   END "2B.  INLINE" FUNCTIONS.



//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "ItemDDropper_t" INLINE STRUCT DEFINITION.






//
//
//
// *************************************************************************** //
// *************************************************************************** //   [[ END "1.  DRAG/DROP" ]].












// *************************************************************************** //
//
//
//
//      2.      "BROWSER" STATE...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//      2B. BROWSER STATE |        BROWSER---STATE.
// *************************************************************************** //

//  "BrowserState_t"
//      PLAIN-OLD-DATA (POD) STRUCT.
//
template<ObjectCFGTraits CFG>
struct BrowserState_t
{
    // *************************************************************************** //
    //      0. |    NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    //  USE_OBJECT_CFG_ALIASES(CFG);
    _USE_OBJECT_CFG_ALIASES
    using                               Vertex                                  = Vertex_t      <CFG>                               ;
    using                               Path                                    = Path_t        <CFG, Vertex>                       ;
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      0. |    STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    static constexpr size_t             ms_MAX_PATH_TITLE_LENGTH                = Path::ms_MAX_PATH_LABEL_LENGTH + 64ULL;
    static constexpr size_t             ms_MAX_VERTEX_TITLE_LENGTH              = Vertex::ms_VERTEX_NAME_BUFFER_SIZE + 32ULL;
    
//
// *************************************************************************** //
// *************************************************************************** //   END "0.  CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//      1.          DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //
    
    // *************************************************************************** //
    //      1. |    STATE VARIABLES.
    // *************************************************************************** //

    // *************************************************************************** //
    //      1. |    IMPORTANT DATA-MEMBERS.
    // *************************************************************************** //
    
    // *************************************************************************** //
    //      1. |    GENERIC DATA.
    // *************************************************************************** //
    //                              MUTABLE STATE VARIABLES:
    bool                                m_show_default_properties                   = false;
    bool                                m_renaming_layer                            = false;    //  TRUE when user is in the middle of MODIFYING NAME.
    bool                                m_renaming_obj                              = false;    //
    //
    //
    //                              NEW STUFF:
        //      std::vector<int>            m_layer_rows                                {  };
        //      bool                        m_layer_filter_dirty                        = false;    //  Flag to queue Browser to RE-COMPUTE sorted items.
        //      int                         m_layer_rows_paths_rev                      = -1;
    //
    std::vector<int>                    m_obj_rows                                  {   };
    bool                                m_obj_filter_dirty                          = false;    //
    int                                 m_obj_rows_paths_rev                        = -1;
    //
    //
    //                              INDICES:
    int                                 m_layer_browser_anchor                      = -1;       //  ?? currently selected LAYER ??
    //
    //
    int                                 m_browser_anchor                            = -1;       //  ?? anchor index for Shift‑range select ??
    //
    mutable int                         m_hovered_obj                               = -1;
    mutable int                         m_hovered_canvas_obj                        = -1;
    //
    //
    int                                 m_inspector_vertex_idx                      = -1;       //  ...
    mutable std::pair<int,int>          m_hovered_vertex                            = {-1, -1};
    //
    //
    int                                 m_layer_rename_idx                          = -1;       //  LAYER that is BEING RENAMED     (–1 = none)
    int                                 m_obj_rename_idx                            = -1;       //  OBJECT that is BEING RENAMED    (–1 = none)
    //
    //
    //                              CACHE AND MISC. DATA:
    char                                m_name_buffer[ ms_MAX_PATH_TITLE_LENGTH ]   = {   };    //  scratch text
    //
    //
    //
    //                              OTHER DATA ITEMS:
    ImGuiTextFilter                     m_layer_filter;                                         //  search box for "LAYER" browser.
    ImGuiTextFilter                     m_obj_filter;                                           //  search box for "OBJECT" browser.
    
//
// *************************************************************************** //
// *************************************************************************** //   END "1.  DATA-MEMBERS".



// *************************************************************************** //
//
//      2.A.        MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
    // *************************************************************************** //
    //      INITIALIZATION METHODS.         |   "init.cpp" ...
    // *************************************************************************** //
    //  explicit                        BrowserState_t          (app::AppState & );             //  Def. Constructor.
                                        BrowserState_t          (void) noexcept                 = default;
                                        ~BrowserState_t         (void)                          = default;
    
    // *************************************************************************** //
    //      DELETED FUNCTIONS.              |   ...
    // *************************************************************************** //
                                        BrowserState_t          (const BrowserState_t &    src)       = delete;   //  Copy. Constructor.
                                        BrowserState_t          (BrowserState_t &&         src)       = delete;   //  Move Constructor.
    BrowserState_t &                    operator =              (const BrowserState_t &    src)       = delete;   //  Assgn. Operator.
    BrowserState_t &                    operator =              (BrowserState_t &&         src)       = delete;   //  Move-Assgn. Operator.
    
//
// *************************************************************************** //
// *************************************************************************** //   END "2A.  MEMBER FUNCS".

    
   
// *************************************************************************** //
//
//      2.B.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
    // *************************************************************************** //
    //      2.B. |  QUERY FUNCTIONS.
    // *************************************************************************** //
    //  "_has_query"
    //  [[nodiscard]] inline bool           _has_query                          (void) const noexcept   { return; };



    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.B. |  CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    //  "reset"
    inline void                         reset                           (void) noexcept                 { this->clear(); }
    
    //  "clear"
    inline void                         clear                           (void) noexcept
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
    //
    //
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
    
//
// *************************************************************************** //
// *************************************************************************** //   END "2B.  INLINE" FUNCTIONS.



//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "BrowserState_t" INLINE STRUCT DEFINITION.






// *************************************************************************** //
//      2B. BROWSER STYLE |         BROWSER---STYLE.
// *************************************************************************** //

//  "BrowserStyle"
//      PLAIN-OLD-DATA (POD) STRUCT.
//
struct BrowserStyle
{
    // *************************************************************************** //
    //      0. |    NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    CBAPP_CBLIB_TYPES_API
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      0. |    STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    static constexpr float              ms_MY_CONSTEXPR_VALUE           = 240.0f;
    
//
// *************************************************************************** //
// *************************************************************************** //   END "0.  CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//      1.          DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //
    
    // *************************************************************************** //
    //      1. |    STATE VARIABLES.
    // *************************************************************************** //

    // *************************************************************************** //
    //      1. |    IMPORTANT DATA-MEMBERS.
    // *************************************************************************** //
    
    // *************************************************************************** //
    //      1. |    WIDGET---APPEARANCE DATA.
    // *************************************************************************** //
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
    
//
// *************************************************************************** //
// *************************************************************************** //   END "1.  DATA-MEMBERS".



// *************************************************************************** //
//
//      2.A.        MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
    // *************************************************************************** //
    //      INITIALIZATION METHODS.         |   "init.cpp" ...
    // *************************************************************************** //
    //  explicit                        BrowserStyle                (app::AppState & );             //  Def. Constructor.
                                        BrowserStyle                (void) noexcept                 = default;
                                        ~BrowserStyle               (void)                          = default;
    
    // *************************************************************************** //
    //      DELETED FUNCTIONS.              |   ...
    // *************************************************************************** //
                                        BrowserStyle                (const BrowserStyle &    src)       = delete;   //  Copy. Constructor.
                                        BrowserStyle                (BrowserStyle &&         src)       = delete;   //  Move Constructor.
    BrowserStyle &                      operator =                  (const BrowserStyle &    src)       = delete;   //  Assgn. Operator.
    BrowserStyle &                      operator =                  (BrowserStyle &&         src)       = delete;   //  Move-Assgn. Operator.
    
//
// *************************************************************************** //
// *************************************************************************** //   END "2A.  MEMBER FUNCS".

    
   
// *************************************************************************** //
//
//      2.B.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      2.B. |  QUERY FUNCTIONS.
    // *************************************************************************** //
    //  "_has_query"
    //  [[nodiscard]] inline bool           _has_query                          (void) const noexcept   { return; };

    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.B. |  CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    //  "_no_op"
    //  inline void                         _no_op                              (void)      { return; };
    
//
// *************************************************************************** //
// *************************************************************************** //   END "2B.  INLINE" FUNCTIONS.



//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "BrowserStyle" INLINE STRUCT DEFINITION.



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 2.  "BROWSER" ]].











// *************************************************************************** //
//
//
//
//      3.      "EDITOR" STATE...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//      3A. EDITOR STATES |         EDITOR---RUNTIME.
// *************************************************************************** //

//  "EditorRuntime_t"
//      Contains the NON-COPYABLE, NON-MOVABLE DATA that pertains to the Editor State.
//
template< typename VID, typename PtID, typename LID, typename PID, typename ZID, typename HID >
struct EditorRuntime_t
{
    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    
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
    std::mutex                              m_task_mtx                      {   };
    std::vector< std::function<void()> >    m_main_tasks                    {   };
    
    // *************************************************************************** //
    //      STATE VARIABLES.
    // *************************************************************************** //
    std::atomic<bool>                       m_io_busy                       { false };
    std::atomic<bool>                       m_sdialog_open                  { false };
    std::atomic<bool>                       m_odialog_open                  { false };
    std::atomic<bool>                       m_show_io_message               { false };
    
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
                                        EditorRuntime_t             (void) noexcept                     = default;
                                        ~EditorRuntime_t            (void)                              = default;
    
    // *************************************************************************** //
    //      DELETED FUNCTIONS.              |   ...
    // *************************************************************************** //
                                        EditorRuntime_t             (const EditorRuntime_t &    src)    = delete;   //  Copy. Constructor.
                                        EditorRuntime_t             (EditorRuntime_t &&         src)    = delete;   //  Move Constructor.
    EditorRuntime_t &                   operator =                  (const EditorRuntime_t &    src)    = delete;   //  Assgn. Operator.
    EditorRuntime_t &                   operator =                  (EditorRuntime_t &&         src)    = delete;   //  Move-Assgn. Operator.
    
//
// *************************************************************************** //
// *************************************************************************** //   END "PUBLIC MEMBER FUNCS".

    
   
// *************************************************************************** //
//
//      2.B.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.

//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "EditorRuntime" INLINE STRUCT DEFINITION.






// *************************************************************************** //
//      4B. EDITOR STATES |         EDITOR---STATE.
// *************************************************************************** //

//  "EditorState_t"
//
template< typename VID, typename PtID, typename LID, typename PID, typename ZID, typename HID >
struct EditorState_t
{
//      0.          CONSTANTS AND ALIASES...
// *************************************************************************** //
// *************************************************************************** //
public:

    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    CBAPP_CBLIB_TYPES_API
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    static constexpr float                  ms_IO_MESSAGE_DURATION          = 5.0f;
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      REFERENCES TO GLOBAL ARRAYS.
    // *************************************************************************** //
    //                              //  ...
    
//
//
// *************************************************************************** //
// *************************************************************************** //   END "CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//
//      1.          CLASS DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //                  OVERALL STATE...
    // *************************************************************************** //
    ImPlotInputMap                          m_backup;
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //                  PERSISTENT STATE INFORMATION...
    // *************************************************************************** //
    //                                  OVERALL STATE / ENABLED BEHAVIORS:
    mutable bool                            m_block_overlays                = false;
    //
    bool                                    m_show_debug_overlay            = true;     //  Persistent UI Resident Overlays.
    bool                                    m_show_ui_traits_overlay        = true;
    bool                                    m_show_ui_objects_overlay       = true;
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //                  IMPLOT SETTINGS INFORMATION...
    // *************************************************************************** //
    //                                  PERSISTENT STATE INFORMATION:
    Param<float>                            m_mousewheel_zoom_rate              = { 0.050f,     { 0.010f,   0.350f } };
    //
    //
    //                                  TRANSIENT STATE INFORMATION:
    //                                      ...
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //                  TRANSIENT STATE INFORMATION...
    // *************************************************************************** //
    //                                  UTILITY:
    float                                   m_bar_h                             = 0.0f;
    ImVec2                                  m_avail                             = ImVec2(0.0f,      0.0f);
    ImVec2                                  m_p0                                = ImVec2(0.0f,      0.0f);
    ImVec2                                  m_p1                                = ImVec2(0.0f,      0.0f);
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //                  TODO: RE-HOME THESE INTO TOOL STATE OBJs...
    // *************************************************************************** //
    //
    //                                  LASSO TOOL / SELECTION:
    bool                                    m_show_sel_overlay                  = false;
    //
    bool                                    m_dragging                          = false;
    bool                                    m_lasso_active                      = false;
    bool                                    m_pending_clear                     = false;    //  pending click selection state ---
    //
    ImVec2                                  m_lasso_start                       = ImVec2(0.f,       0.f);
    ImVec2                                  m_lasso_end                         = ImVec2(0.f,       0.f);
    VID                                     m_next_id                           = 1;
    PID                                     m_next_pid                          = 1;        // counter for new path IDs
    //
    //
    //                                  PEN-TOOL STATE:
    bool                                    m_drawing                           = false;
    bool                                    m_dragging_handle                   = false;
    bool                                    m_dragging_out                      = true;
    VID                                     m_drag_vid                          = 0;
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //                  MISC STATE...
    // *************************************************************************** //

                                            //
                                            //  ...
                                            //

    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //                  SERIALIZATION STUFF...
    // *************************************************************************** //
    std::mutex                              m_task_mtx;
    std::vector<std::function<void()>>      m_main_tasks;
    //
    std::atomic<bool>                       m_io_busy                       { false };
    IOResult                                m_io_last                       { IOResult::Ok };
    std::string                             m_io_msg                        {   };
    //
    std::atomic<bool>                       m_sdialog_open                  = { false };
    std::atomic<bool>                       m_odialog_open                  = { false };
    std::string                             m_project_name                  = {   };
    std::filesystem::path                   m_filepath                      = {"../../assets/.cbapp/presets/editor/testing/editor-default.json"};   //    {"../../assets/.cbapp/presets/editor/testing/editor-fdtd_v0.json"};
    //      std::filesystem::path               m_filepath                      = {"../../assets/.cbapp/presets/editor/testing/editor-fdtd_v0.json"};
    //
    //
    //                                  PEN-TOOL STATE:
    std::atomic<bool>                       m_show_io_message               { false };
    float                                   m_io_message_timer              = -1.0f;
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "CLASS DATA-MEMBERS".



// *************************************************************************** //
//
//
//      2.A.        PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
public:
    
    // *************************************************************************** //
    //      INITIALIZATION METHODS.         |   "init.cpp" ...
    // *************************************************************************** //
                                        EditorState_t           (void) noexcept                 = default;
                                        ~EditorState_t          (void)                          = default;
    
    // *************************************************************************** //
    //      DELETED FUNCTIONS.              |   ...
    // *************************************************************************** //
                                        EditorState_t           (const EditorState_t &    src)  = default;      //  Copy. Constructor.
                                        EditorState_t           (EditorState_t &&         src)  = default;      //  Move Constructor.
    EditorState_t &                     operator =              (const EditorState_t &    src)  = default;      //  Assgn. Operator.
    EditorState_t &                     operator =              (EditorState_t &&         src)  = default;      //  Move-Assgn. Operator.
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "PUBLIC MEMBER FUNCS".


   
// *************************************************************************** //
//
//
//      2.B.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      MAIN FUNCTIONS...
    // *************************************************************************** //
    
    //  "DisplayIOStatus"
    inline void                         DisplayIOStatus                     (void) noexcept
    {
        ImGuiIO &           io                  = ImGui::GetIO();
        this->m_io_message_timer               -= io.DeltaTime;
        
        
        //      1.      DECREMENT TIMER...
        if ( this->m_io_message_timer <= 0.0f ) {
            this->m_io_message_timer = -1.0f;
            this->m_show_io_message.store(false, std::memory_order_release);
            return;
        }
        //
        //      2.      DISPLAY TOOL-TIP I/O MESSAGING...
        //  ImGui::SetNextWindowBgAlpha(0.75f); // optional: semi-transparent
        ImGui::BeginTooltip();
            ImGui::TextUnformatted( this->m_io_msg.c_str() );
        ImGui::EndTooltip();
    
        return;
    }
    
    

    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      QUERY FUNCTIONS.
    // *************************************************************************** //
    


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    
    //  "SetLastIOStatus"
    inline void                         SetLastIOStatus                     (const IOResult result, std::string & message) noexcept
    {
        this->m_io_busy                 .store( false,   std::memory_order_release   );
        this->m_show_io_message         .store( true,    std::memory_order_release   );
        this->m_io_message_timer        = ms_IO_MESSAGE_DURATION;
        //
        this->m_io_last                 = result;
        this->m_io_msg                  = std::move( message );
        
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
//
// *************************************************************************** //
// *************************************************************************** //   END "EditorState"
};



//      EditorState     : "to_json"
//
template<typename VID, typename PtID, typename LID, typename PID, typename ZID, typename HitID>
inline void to_json(nlohmann::json & j, const EditorState_t<VID, PtID, LID, PID, ZID, HitID> & obj)
{
    j ["m_world_size"]      = obj.m_world_size;
    j ["m_world_slop"]      = obj.m_world_slop;
    j ["m_zoom_size"]       = obj.m_zoom_size;

    //  j = {
    //      { "m_world_size",          obj.m_world_size       }
    //  };
    
    return;
}


//      EditorState     : "from_json"
//
template<typename VID, typename PtID, typename LID, typename PID, typename ZID, typename HitID>
inline void from_json([[maybe_unused]] nlohmann::json & j, [[maybe_unused]] EditorState_t<VID, PtID, LID, PID, ZID, HitID> & obj)
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
//      4C. EDITOR STYLE |         EDITOR---STYLE.
// *************************************************************************** //

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
//
//
// *************************************************************************** //
//                              UI-APPEARANCE CONSTANTS...
// *************************************************************************** //
    float                       ms_TOOLBAR_ICON_SCALE           = 1.5f;
    
// *************************************************************************** //
//
//
// *************************************************************************** //
//                              INTERACTION / RESPONSIVENESS CONSTANTS...
// *************************************************************************** //
    float                       GRID_STEP                       = 64.0f;
    float                       HIT_THRESH_SQ                   = 6.0f * 6.0f;
    float                       DRAG_START_DIST_PX              = 4.0f;
//
    int                         PEN_DRAG_TIME_THRESHOLD         = 0.05;                             //  PEN_DRAG_TIME_THRESHOLD         // seconds.
    float                       PEN_DRAG_MOVEMENT_THRESHOLD     = 4.0f;                             //  PEN_DRAG_MOVEMENT_THRESHOLD     // px  (was 2)
    
// *************************************************************************** //
//
//
// *************************************************************************** //
//                              STANDARDIZED STYLE...
// *************************************************************************** //
    ImU32                       COL_POINT_DEFAULT               = IM_COL32(0,255,0,255);        // idle green
    ImU32                       COL_POINT_HELD                  = IM_COL32(255,100,0,255);      // while dragging
    ImU32                       COL_SELECTION_OUT               = IM_COL32(255,215,0,170);      // gold outline
    float                       DEFAULT_POINT_RADIUS            = 12.0f;                        // px
    //
    //
    //  SEL BLUE    = IM_COL32(0,       180,    255,    153);
    //  GOLD        = IM_COL32(255,     215,    0,      127);
    //  GOOD BLUE   = IM_COL32(97,      145,    224,    170);
    //
    //
    ImU32                       HIGHLIGHT_COLOR                 = IM_COL32(0,       180,    255,    153);
    float                       HIGHLIGHT_WIDTH                 = 4.0f;
    ImU32                       AUX_HIGHLIGHT_COLOR             = IM_COL32(97,      145,    224,    170);
    float                       AUX_HIGHLIGHT_WIDTH             = 8.0f;
    //
    //
    //  ImU32                       AUX_HIGHLIGHT_COLOR             = IM_COL32(255,215,0,255);      // gold outline
    //  float                       AUX_HIGHLIGHT_WIDTH             = 6.0f;
    
// *************************************************************************** //
//
//
// *************************************************************************** //
//                              CANVAS  | USER-INTERFACE CONSTANTS...
// *************************************************************************** //
//                          HANDLES:
    ImU32                       ms_HANDLE_COLOR                 = IM_COL32(255, 215, 0, 170);       //  ms_HANDLE_COLOR             //  gold    IM_COL32(255, 215, 0, 255)
    ImU32                       ms_HANDLE_HOVER_COLOR           = cblib::utl::compute_tint( IM_COL32(255, 215, 0, 255), 0.30f );       //  ms_HANDLE_HOVER_COLOR       //  yellow  IM_COL32(255, 255, 0, 255)
    float                       ms_HANDLE_SIZE                  = 5.5f;
    float                       HANDLE_BOX_SIZE                 = 5.5f;                              //  ms_HANDLE_SIZE              //  px half-side
    
// *************************************************************************** //
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
    float                       SELECTION_BBOX_MARGIN_PX        = 8.0f; //    12.0f;
    ImU32                       SELECTION_BBOX_COL              = IM_COL32(0, 180, 255, 153);   //  cyan-blue
    float                       SELECTION_BBOX_TH               = 2.5f;
    
// *************************************************************************** //
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
// *************************************************************************** //
//                              WIDGETS / UI-CONSTANTS / APP-APPEARANCE...
// *************************************************************************** //
    //                      SYSTEM PREFERENCES WIDGETS:
    float                       ms_SETTINGS_LABEL_WIDTH             = 196.0f;
    float                       ms_SETTINGS_WIDGET_AVAIL            = 256.0f;
    float                       ms_SETTINGS_INDENT_SPACING_CACHE    = 0.0f;
    float                       ms_SETTINGS_WIDGET_WIDTH            = -1.0f;
    float                       ms_SETTINGS_SMALL_LINE_HEIGHT       = -1.0f;
    
// *************************************************************************** //
//
//
// *************************************************************************** //
//                              RENDERING CONSTANTS...
// *************************************************************************** //
    //                      VERTEX RENDERING:
    int                         ms_VERTEX_NUM_SEGMENTS          = 8;    //    12;
    //
    //                      BEZIER RENDERING:
    int                         ms_BEZIER_SEGMENTS              = 0;                    //  ms_BEZIER_SEGMENTS
    int                         ms_BEZIER_HIT_STEPS             = 15;   //    20;       //  ms_BEZIER_HIT_STEPS
    int                         ms_BEZIER_FILL_STEPS            = 15;   //    24;       //  ms_BEZIER_FILL_STEPS
    
// *************************************************************************** //
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
// *************************************************************************** //
//                              MISC. / UNKNOWN CONSTANTS (RELOCATED FROM CODE)...
// *************************************************************************** //
    float                       TARGET_PX                       = 20.0f;                    //     desired screen grid pitch"  | found in "_update_grid"
    float                       PICK_PX                         = 6.0f;                     //     desired screen grid pitch"  | found in "_update_grid"

// *************************************************************************** //

    
   
// *************************************************************************** //
//
//
//      2.C.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    
    //  "PushSettingsWidgetW"
    inline void                         PushSettingsWidgetW                 (const uint8_t mult = 1) noexcept
    { this->ms_SETTINGS_WIDGET_WIDTH = this->ms_SETTINGS_WIDGET_AVAIL - static_cast<float>( mult ) * ms_SETTINGS_INDENT_SPACING_CACHE; }
    
    //  "PopSettingsWidgetW"
    inline void                         PopSettingsWidgetW                  (void) noexcept
    { this->ms_SETTINGS_WIDGET_WIDTH = this->ms_SETTINGS_WIDGET_AVAIL; }
    
    //  "SmallNewLine"
    inline void                         SmallNewLine                        (void) noexcept
    { ImGui::Dummy({ 0.0f, this->ms_SETTINGS_SMALL_LINE_HEIGHT}); return; }
    
    
    
    // *************************************************************************** //
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.



// *************************************************************************** //
// *************************************************************************** //
};//	END "MyClass" INLINE CLASS DEFINITION.



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 3.  "EDITOR" ]].


















// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.












#endif      //  _CBWIDGETS_EDITOR_STATE_AND_STYLE_H  //
// *************************************************************************** //
// *************************************************************************** //   END FILE.

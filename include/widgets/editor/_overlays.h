/***********************************************************************************
*
*       ********************************************************************
*       ****            _ O V E R L A Y S . H  ____  F I L E            ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
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

#define     _USE_INTERNAL_CALLBACK      1

template <typename OID, typename MappingFn>
    requires std::is_nothrow_invocable_r_v<ImVec2, MappingFn, ImVec2>
class OverlayManager_t
{
//      0.          CONSTANTS AND ALIASES...
// *************************************************************************** //
// *************************************************************************** //
public:

    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    using                               OverlayID                       = OID;
    using                               Overlay                         = Overlay_t<OverlayID>;
    using                               OverlayInfo                     = Overlay::OverlayInfo;
    using                               WindowType                      = OverlayInfo::WindowType;
    using                               Anchor                          = utl::Anchor;
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    static constexpr size_t             ms_INVALID_ID                   = cblib::maximum_value_of_type<OID>();
    
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
private:

    // *************************************************************************** //
    //      IMPORTANT DATA-MEMBERS.
    // *************************************************************************** //
    MappingFn                           ws_to_px;
    OverlayID                           m_next_id                   { 1 };
    //
    std::vector<Overlay>                m_windows;                              //  dynamic overlays (current m_windows)
    std::vector<Overlay>                m_residents;                            //  resident overlays never erased; their `visible` flag is toggled
    //
    //
    //                              CACHE VARIABLES FOR RE-SIZE:
    ImVec2                              m_pos_cache                 = { 0.0f,   0.0f };
    ImVec2                              m_pivot_cache               = { 0.0f,   0.0f };
    std::optional<OverlayID>            m_current_ctx_menu          = {std::nullopt};
    std::optional<OverlayID>            m_request_update            = {std::nullopt};
    
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
    //      INITIALIZATION METHODS.         |   ...
    // *************************************************************************** //
    inline                              OverlayManager_t        (MappingFn callback) noexcept   : ws_to_px(callback) {   }
    
    // *************************************************************************** //
    //      DELETED FUNCTIONS.              |   ...
    // *************************************************************************** //
                                        OverlayManager_t        (const OverlayManager_t &    src)        = delete;   //  Copy. Constructor.
                                        OverlayManager_t        (OverlayManager_t &&         src)        = delete;   //  Move Constructor.
    OverlayManager_t &                  operator =              (const OverlayManager_t &    src)        = delete;   //  Assgn. Operator.
    OverlayManager_t &                  operator =              (OverlayManager_t &&         src)        = delete;   //  Move-Assgn. Operator.
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      MAIN API.                       |   "interface.cpp" ...
    // *************************************************************************** //
    void                                Begin                               (ImVec2 , const ImRect & );
    void                                Begin                               (const std::function<ImVec2(ImVec2)> & world_to_px, ImVec2 cursor_px, const ImRect & plot_rect);
    //
    //                              OVERLAY API:
    OverlayID                           create_overlay                      (const OverlayCFG & cfg);
    void                                destroy_overlay                     (OverlayID );
    //
    //                              UTILITIES:
    Overlay *                           resident                            (OverlayID id);
    OverlayID                           add_resident                        (const OverlayCFG & cfg);
    OverlayID                           add_resident                        (const OverlayCFG &, const OverlayStyle &);
    OverlayID                           add_resident                        (const OverlayCFG &, const OverlayStyle &, const OverlayInfo &);
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      INLINE PUBLIC API.              |   "..."
    // *************************************************************************** //
    
    //  "lookup"
    Overlay *               lookup                  (OverlayID id) {
        for (auto& ov : m_windows) {
            if (ov.info.id == id)   { return &ov; }
        }
        return nullptr;                               // not found
    }
    
    //  "lookup_resident"
    Overlay *               lookup_resident         (OverlayID id) {
        for (auto& ov : m_residents) {
            if (ov.info.id == id)   { return &ov; }
        }
        return nullptr;                               // not found
    }
    
    //  "get_resident"
    inline Overlay *        get_resident            (OverlayID id)
    {
        for (auto & ov : m_residents) {
            if (ov.info.id == id)   { return std::addressof( ov ); }
        }
        IM_ASSERT(false && "get_resident: id not found");           // debug-time catch
        return std::addressof( m_residents.front() );                // fallback (never hit in release)
    }

    // *************************************************************************** //
    
    
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "PUBLIC MEMBER FUNCS".


    
// *************************************************************************** //
//
//
//      2.B.        PROTECTED MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
protected:
#ifdef _USE_INTERNAL_CALLBACK
    void                                _render_default_overlay             ( Overlay & ov, ImVec2 cursor_px, const ImRect & );
    void                                _render_custom_overlay              ( Overlay & ov, ImVec2 cursor_px, const ImRect & );
//
# else
//
    void                                _render_default_overlay             ( Overlay & ov, const std::function<ImVec2(ImVec2)> & ,
                                                                              ImVec2 cursor_px, const ImRect & );
    void                                _render_custom_overlay              ( Overlay & ov, const std::function<ImVec2(ImVec2)> & ,
                                                                              ImVec2 cursor_px, const ImRect & );
#endif  //  _USE_INTERNAL_CALLBACK  //
    //
    void                                _draw_context_menu                  (Overlay & );
    void                                _draw_custom_context_menu           (Overlay & );
    //
    //
    [[nodiscard]] std::pair<ImVec2, ImVec2>
                                        _anchor_to_pos                      ( Overlay & , ImVec2 cursor, const ImRect & plot);
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "PROTECTED" FUNCTIONS.

    
   
// *************************************************************************** //
//
//
//      2.C.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    
    //  "anchor_to_pivot"
    inline static ImVec2    anchor_to_pivot         (const Anchor a) {
        switch (a) {
            case Anchor::North:         return {0.5f, 0.0f};
            case Anchor::NorthEast:     return {1.0f, 0.0f};
            case Anchor::East:          return {1.0f, 0.5f};
            case Anchor::SouthEast:     return {1.0f, 1.0f};
            case Anchor::South:         return {0.5f, 1.0f};
            case Anchor::SouthWest:     return {0.0f, 1.0f};
            case Anchor::West:          return {0.0f, 0.5f};
            case Anchor::NorthWest:     return {0.0f, 0.0f};
            case Anchor::Center:
            default:                    return {0.5f, 0.5f};
        }
    }
                       
    //  "_render_overlay"
    inline void                 _render_overlay                 ( Overlay & ov, ImVec2 cursor_px, const ImRect & rect )
    {
        IM_ASSERT( ov.cfg.draw_fn  &&  "Overlay draw_fn must not be null" );
        if ( !ov.info.visible )         { return; }
        
    
        switch (ov.info.type)
        {
            //      1.      DEFAULT RESIDENTS.
            case WindowType::Resident   : {
                _render_default_overlay         (ov,    cursor_px,  rect);
                break;
            }
            
            //      2.      CUSTOM RESIDENTS.
            case WindowType::Custom     : {
                _render_custom_overlay          (ov,    cursor_px,  rect);
                break;
            }
            
            //      0.      EPHENERAL OVERLAY WINDOWS.
            default                     : {
                IM_ASSERT( true && "ephemeral overlays are NOT implemented yet" );
            }
        }
        
        return;
    }
                       
    //  "_render_overlay"
# ifndef _USE_INTERNAL_CALLBACK
    inline void                 _render_overlay                 ( Overlay & ov, const std::function<ImVec2(ImVec2)> & render_fn,
                                                                  ImVec2 cursor_px, const ImRect & rect )
    {
        IM_ASSERT( ov.cfg.draw_fn  &&  "Overlay draw_fn must not be null" );
        if ( !ov.info.visible )         { return; }
        
    
        switch (ov.info.type)
        {
            //      1.      DEFAULT RESIDENTS.
            case WindowType::Resident   : {
                _render_default_overlay         (ov,    render_fn,  cursor_px,  rect);
                break;
            }
            
            //      2.      CUSTOM RESIDENTS.
            case WindowType::Custom     : {
                _render_custom_overlay          (ov,    render_fn,  cursor_px,  rect);
                break;
            }
            
            //      0.      EPHENERAL OVERLAY WINDOWS.
            default                     : {
                IM_ASSERT( true && "ephemeral overlays are NOT implemented yet" );
            }
        }
        
        return;
    }
#endif     //  _USE_INTERNAL_CALLBACK  //
               
    //  "_init_resident_overlay"
    inline void                 _init_resident_overlay              (Overlay & ov)
    {
        const bool  promote_to_custom   = ov.style.window_size.has_value();
    
        if ( promote_to_custom )        { ov.info.type = WindowType::Custom; }
        else                            { ov.info.type = WindowType::Resident; }
        
        
        switch (ov.info.type)
        {
            //      1.      DEFAULT RESIDENTS.
            case WindowType::Resident   : {
                ov.info.id              = m_next_id++;
                ov.info.window_name     = "##DefaultResident_" + std::to_string(ov.info.id);
                break;
            }
            
            //      2.      CUSTOM RESIDENTS.
            case WindowType::Custom     : {
                ov.info.id              = m_next_id++;
                ov.info.window_name     = "##CustomResident_" + std::to_string(ov.info.id);
                break;
            }
            
            //      0.      EPHENERAL OVERLAY WINDOWS.
            default :           { IM_ASSERT( true && "ephemeral overlays are NOT implemented yet" ); }
        }
        
        return;
    }
    
    // *************************************************************************** //
   
   
   
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.






// *************************************************************************** //
// *************************************************************************** //
};//	END "OverlayManager_t" INLINE CLASS DEFINITION.






//  DEFINE THE TYPES THAT WE SHIP...
//
extern template class       OverlayManager_t   < int8_t,           ImVec2 (&)(ImVec2) noexcept >;
extern template class       OverlayManager_t   < int16_t,          ImVec2 (&)(ImVec2) noexcept >;
extern template class       OverlayManager_t   < int32_t,          ImVec2 (&)(ImVec2) noexcept >;
extern template class       OverlayManager_t   < int64_t,          ImVec2 (&)(ImVec2) noexcept >;
//
extern template class       OverlayManager_t   < uint8_t,          ImVec2 (&)(ImVec2) noexcept >;
extern template class       OverlayManager_t   < uint16_t,         ImVec2 (&)(ImVec2) noexcept >;
extern template class       OverlayManager_t   < uint32_t,         ImVec2 (&)(ImVec2) noexcept >;
extern template class       OverlayManager_t   < uint64_t,         ImVec2 (&)(ImVec2) noexcept >;








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


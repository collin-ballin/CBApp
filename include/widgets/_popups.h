/***********************************************************************************
*
*       ********************************************************************
*       ****              _ P O P U P S . H  ____  F I L E              ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond
*               DATED:      July 12, 2025.
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_WIDGETS_POPUPS_H
#define _CBAPP_WIDGETS_POPUPS_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
#include "utility/utility.h"


//  0.2     STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <cstdlib>          // C-Headers...
#include <stdio.h>
#include <unistd.h>
#include <random>
#include <tuple>
#include <utility>
#include <algorithm>

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



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



namespace popup { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //

//---------------------------------------------------------------------
// forward decl. + type aliases
//---------------------------------------------------------------------
struct Context;                                   // per-modal frame context

using BeginFn  = std::function<void(Context&)>;   // runs every frame inside modal
using CloseFn  = std::function<void(bool)>;       // fired once on close (true = OK)




//  "Modal"
//
struct Modal {
    std::string         id;
    BeginFn             begin;
    CloseFn             on_close;
    ImGuiWindowFlags    flags           = ImGuiWindowFlags_AlwaysAutoResize;
//
    ImVec2              pos             {  };              // placement
    ImVec2              size            {  };
    ImVec2              pivot           { 0.5f,     0.5f };
    ImGuiID             viewport        = 0;
    ImGuiCond           cond            = ImGuiCond_Appearing;
//
    bool                open            = true;
    bool                accepted        = false;
};


//  "Context"
//
struct Context {
    Modal &         modal;
//
//
    void            close       (bool ok = false)       { modal.open = false; modal.accepted = ok; }
    bool            accepted    (void) const            { return modal.accepted; }
};


//  CLASS:  "Manager"
//
class Manager {
public:
    //
    //      1.      ASK-OK-CANCEL MENU:
    static constexpr ImVec2             ASKOKCANCEL_WINDOW_SIZE         = ImVec2( 400.0f,        200.0f );
    static constexpr ImGuiWindowFlags   ASKOKCANCEL_WINDOW_FLAGS        = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration;
    static constexpr ImVec2             ASKOKCANCEL_WINDOW_PIVOT        = ImVec2( 0.5f,          1.0f/3.0f );
    //
    //
    //      2.      SYSTEM PREFERENCES MENU:
    static constexpr ImVec2             PREFERENCES_WINDOW_SIZE         = ImVec2( 668.0f,        520.0f + 120.0f );
    static constexpr ImGuiWindowFlags   PREFERENCES_WINDOW_FLAGS        = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse;
    static constexpr ImVec2             PREFERENCES_WINDOW_PIVOT        = ImVec2( 0.5f,          0.0f );
    //
    //
    //      X.      OTHER WIDGETS...
    static constexpr float              ASKOKCANCEL_BUTTON_W            = 120.0f;
//
//
//
public:


    //  "Open"
    //      flags defaults to AlwaysAutoResize
    //
    void Open( const char *         id,
               BeginFn              body,
               ImGuiWindowFlags     flags           = ImGuiWindowFlags_AlwaysAutoResize,
               CloseFn              on_close        = {  },
               ImVec2               pos             = {  },
               ImVec2               size            = {  },
               ImVec2               pivot           = {0.5f,    0.5f},
               ImGuiID              viewport        = 0,
               ImGuiCond            cond            = ImGuiCond_Appearing )
    {
        queued_.push_back( Modal{
            /*  id          */      id,
            /*  begin       */      std::move(body),
            /*  on_close    */      std::move(on_close),
            /*  flags       */      flags,
            /*  pos         */      pos,
            /*  size        */      size,
            /*  pivot       */      pivot,
            /*  viewport    */      viewport,
            /*  cond        */      cond
            //
            //  ...open / accepted use default members
        } );
        
        return;
    }


    //  "Draw"
    //
    inline void Draw(void)
    {
        //  1.  PROMOTE ANY FRESHLY-QUEUED MODAL WINDOWS TO ACTIVE LIST...
        for (Modal & m : queued_) {
            ImGui::OpenPopup(m.id.c_str());
            active_.push_back(std::move(m));
        }
        queued_.clear();


        //  2.  ITERATE THROUGH OPENED POP-UPS...
        for (auto it = active_.begin(); it != active_.end(); /*increment inside*/)
        {
            Modal &     m   = *it;

            // ── Apply caller-supplied placement just before BeginPopupModal ──────────
            ImGui::SetNextWindowSize     (m.size,  m.cond);
            ImGui::SetNextWindowPos      (m.pos,   m.cond, m.pivot);
            if ( m.viewport )           { ImGui::SetNextWindowViewport(m.viewport); }


            //      2.1.    BEGIN MODAL WINDOW:
            bool visible = ImGui::BeginPopupModal(m.id.c_str(), &m.open, m.flags);

            if (visible)
            {
                Context     ctx         {m};
                m.begin(ctx);   // draw user content

                // Fallback: manual Esc close if default handler missed it
                if ( ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
                     ImGui::IsKeyPressed(ImGuiKey_Escape, /*repeat*/false) )
                {
                    ctx.close(false);                  // treat as Cancel
                }

                ImGui::EndPopup();
            }


            //  3.  REMOVE THE MODAL WINDOW IF:  (A) ImGui closed it.  (B) User closes it by "x" or "[ESC]"...
            if ( !visible || !m.open )
            {
                if (m.on_close)     { m.on_close(m.accepted); }     //  true ===>   OK/Accept
                it = active_.erase(it);                             //  modal lifetime ends...
            }
            else    { ++it; }
        }
        
        return;
    }


private:
    std::vector<Modal>      queued_;
    std::vector<Modal>      active_;
};



// *************************************************************************** //
//
//
//      GLOBAL WRAPPERS...
// *************************************************************************** //
// *************************************************************************** //

//  "manager"
//
inline Manager &        manager     (void)      { static Manager inst; return inst; }


//  "Draw"
//
inline void             Draw        (void)      { manager().Draw(); }


//  "Open"
//
inline void Open(const char*        id,
                 BeginFn            body,
                 ImGuiWindowFlags   flags     = ImGuiWindowFlags_AlwaysAutoResize,
                 CloseFn            on_close  = {},
                 ImVec2             pos       = {},
                 ImVec2             size      = {},
                 ImVec2             pivot     = {0.5f, 0.5f},
                 ImGuiID            viewport  = 0,
                 ImGuiCond          cond      = ImGuiCond_Appearing)
{
    manager().Open(id,
                   std::move(body),
                   flags,
                   std::move(on_close),
                   pos,
                   size,
                   pivot,
                   viewport,
                   cond);
                   
    return;
}



// *************************************************************************** //
// *************************************************************************** //
}//   END OF "popup" NAMESPACE.






namespace ui { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //

/*  ask_ok_cancel
 *  -------------
 *  Opens a centred modal that shows `message` and “Cancel / OK” buttons.
 *  If the user presses **OK**, `on_accept()` runs exactly once.
 *
 *  Usage:
 *      ui::ask_ok_cancel("Erase everything?",
 *                        []{ clear_all(); });
 */
inline void ask_ok_cancel( const char *             uuid,
                           const char *             message,
                           std::function<void()>    on_ok           = {  },
                           ImGuiViewport *          vp              = nullptr,
                           ImVec2                   size            = popup::Manager::ASKOKCANCEL_WINDOW_SIZE,
                           ImGuiWindowFlags         flags           = popup::Manager::ASKOKCANCEL_WINDOW_FLAGS,
                           ImVec2                   pivot           = popup::Manager::ASKOKCANCEL_WINDOW_PIVOT,
                           ImGuiCond                cond            = ImGuiCond_Appearing)
{
    using namespace popup;


    //  CASE 0 :    USE DEFAULT VIEWPORT...
    if ( !vp ) {
        ImGuiWindow *   win     = ImGui::GetCurrentWindow();
        if (win)        { vp = win->Viewport;               }
        else            { vp = ImGui::GetMainViewport();    }
    }


    //  1.      COMPUTE ANCHOR POINT AND POP-UP POSITIONING...
    ImVec2      anchor{
        vp->Pos.x + vp->Size.x * pivot.x,
        vp->Pos.y + vp->Size.y * pivot.y
    };


    // 3. Queue modal
    popup::Open(
        /*  ID          */      uuid,
        /*  body        */      [msg = std::string(message)](popup::Context & ctx)
        {
            constexpr float         BTN_W           = popup::Manager::ASKOKCANCEL_BUTTON_W;
            const float             BTN_H           = ImGui::GetFrameHeight();

            //------------------------------------------------------------------
            // 1.  Wrapped message
            //------------------------------------------------------------------
            const float             wrap_pos        = ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x; // right edge
            ImGui::PushTextWrapPos(wrap_pos);
            ImGui::TextWrapped("%s", msg.c_str());
            ImGui::PopTextWrapPos();

            //------------------------------------------------------------------
            // 2.  Bottom-aligned & symmetric buttons
            //------------------------------------------------------------------
            const ImGuiStyle &      s               = ImGui::GetStyle();
            const ImVec2            win_sz          = ImGui::GetWindowSize();

            const float             btn_y           = win_sz.y - s.WindowPadding.y - BTN_H;        // bottom
            const float             x_left          = s.WindowPadding.x;                           // left pad
            const float             x_right         = win_sz.x - s.WindowPadding.x - BTN_W;        // right pad

            //      2A.     DRAW "Cancel" BUTTON (on the left)...
            ImGui::SetCursorPos({x_left, btn_y});
            bool                    cancel          = ImGui::Button("Cancel", {BTN_W, BTN_H});

            //      2B.     DRAW "OK" BUTTON (on the right)...
            ImGui::SetCursorPos({x_right, btn_y});
            bool                    ok              = ImGui::Button("OK", {BTN_W, BTN_H});

            //------------------------------------------------------------------
            // 3.  Result handling
            //------------------------------------------------------------------
            if ( !(cancel && ok) ) {
                if (cancel)         { ctx.close(false); }  //   Cancel
                else if ( ok )      { ctx.close(true);  }  //   OK / Accept
            }
            
            return;
        },
        /*  flags       */      flags,
        /*  on_close    */      [cb = std::move(on_ok)](bool accepted){ if (accepted && cb) cb(); },
        /*  pos         */      anchor,                 // anchor matches pivot
        /*  size        */      size,
        /*  pivot       */      pivot,                  // (0.5,0.5)
        /*  viewport    */      vp->ID,
        /*  cond        */      cond
    );
    
    return;
}
//
//
//
/*
inline void ask_ok_cancel( const char *             uuid,
                           const char *             message,
                           std::function<void()>    on_ok,
                           ImGuiViewport *          vp          = nullptr,
                           ImVec2                   size        = popup::Manager::ASKOKCANCEL_WINDOW_SIZE,
                           ImGuiWindowFlags         flags       = popup::Manager::ASKOKCANCEL_WINDOW_FLAGS,
                           ImVec2                   pivot       = popup::Manager::ASKOKCANCEL_WINDOW_PIVOT )
{
    using               namespace       popup;
    ImGuiWindow *       window          = ImGui::GetCurrentWindow();
    ImVec2              pos             = {  };
        
    //  CASE 0 :    ASSIGN DEFAULT VIEWPORT TO CURRENT WINDOW...
    if ( nullptr == nullptr )           { vp = window->Viewport; }     // ImGui::GetMainViewport();
    vp = window->Viewport;
    
    //------------------------------------------------------------------
    //  1.  Fixed size & centred position
    //------------------------------------------------------------------
    pos     = { vp->Pos.x + (vp->Size.x - size.x) * 0.5f,
                vp->Pos.y + (vp->Size.y - size.y) * 0.5f };




    //------------------------------------------------------------------
    // 2. Queue modal
    //------------------------------------------------------------------
    ImGui::SetNextWindowSize        (   size,       ImGuiCond_Appearing                 );
    ImGui::SetNextWindowPos         (   pos,        ImGuiCond_Appearing,    pivot       );
    ImGui::SetNextWindowViewport    (   vp->ID                                          );
    //
    popup::Open(
        //  Title       //          uuid,
        //  BeginFn     //          [msg = std::string(message)](popup::Context & ctx)
        {
            static constexpr float  scale           = 2.05f;
            ImGuiStyle              style           = ImGui::GetStyle();
            const float             BUTTON_H        = ImGui::GetFrameHeight();
            const float             SPACING_W       = ImGui::GetWindowWidth() - Manager::ASKOKCANCEL_BUTTON_W - ImGui::GetStyle().ItemSpacing.x;
            float                   SPACING_H       = ImGui::GetContentRegionAvail().y - scale * ( BUTTON_H + style.WindowPadding.y );
            
            ImGui::PushTextWrapPos();            // wrap at window inner width
                ImGui::TextWrapped("%s", msg.c_str());
            ImGui::PopTextWrapPos();

            if ( SPACING_H > 0.0f )                 { ImGui::Dummy({0, SPACING_H}); }

            const bool              cancel          = ImGui::Button( "Cancel",  {Manager::ASKOKCANCEL_BUTTON_W, BUTTON_H} );
            ImGui::SameLine( SPACING_W );
            const bool              ok              = ImGui::Button( "OK",      {Manager::ASKOKCANCEL_BUTTON_W, BUTTON_H} );

            //  HANDLE USER'S SELECTION (Gaurd against both OK && CANCEL)...
            if ( !(cancel && ok) ) {
                if ( cancel )                       { ctx.close(false); }
                else if ( ok )                      { ctx.close(true);  }
            }
        },
        //
        //  window flags    //      flags,
        //  on_close        //      [cb = std::move(on_ok)](bool accepted)      { if (accepted && cb) { cb(); } }
    );
    
    return;
}*/


//  "open_preferences_popup"
//  Opens a macOS-style Preferences window and calls `body(ctx)` each frame.
//
inline void open_preferences_popup( const char *        uuid,
                                    popup::BeginFn      body,
                                    ImGuiViewport *     vp          = nullptr,
                                    ImVec2              size        = popup::Manager::PREFERENCES_WINDOW_SIZE,
                                    ImGuiWindowFlags    flags       = popup::Manager::PREFERENCES_WINDOW_FLAGS,
                                    ImVec2              pivot       = popup::Manager::PREFERENCES_WINDOW_PIVOT,
                                    ImGuiCond           cond        = ImGuiCond_Appearing )
{
    //  CASE 0 :    USE DEFAULT VIEWPORT...
    if ( !vp ) {
        ImGuiWindow *   win     = ImGui::GetCurrentWindow();
        if (win)        { vp = win->Viewport;               }
        else            { vp = ImGui::GetMainViewport();    }
    }


    //  1.      COMPUTE ANCHOR POINT AND POP-UP POSITIONING...
    ImVec2      anchor{
        vp->Pos.x + vp->Size.x * 0.5f,   // centre-x (pivot.x = 0.5)
        vp->Pos.y + 120.0f               // top edge offset (pivot.y = 0)
    };


    //  3.      Queue modal
    popup::Open(
        /*  ID          */      uuid,
        /*  body        */      std::move(body),
        /*  flags       */      flags,
        /*  on_close    */      {  },
        /*  pos         */      anchor,             //    anchor matches pivot
        /*  size        */      size,
        /*  pivot       */      pivot,              //    (0.5,0.0)
        /*  viewport    */      vp->ID,
        /*  cond        */      cond );
        
        
    return;
}



// *************************************************************************** //
// *************************************************************************** //
}//   END OF "ui" NAMESPACE.













// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.






#endif      //  _CBAPP_WIDGETS_POPUPS_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.


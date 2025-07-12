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
    std::string       id;
    BeginFn           begin;
    CloseFn           on_close;
    ImGuiWindowFlags  flags     = ImGuiWindowFlags_AlwaysAutoResize;
    bool              open      = true;
    bool              accepted  = false;
};


//  "Context"
//
struct Context {
    Modal& modal;
    void close(bool ok = false) { modal.open = false; modal.accepted = ok; }
    bool accepted() const       { return modal.accepted; }
};


//  CLASS:  "Manager"
//
class Manager {
public:
    //
    //  FLAGS:
    static constexpr ImGuiWindowFlags   ASKOKCANCEL_WINDOW_FLAGS        = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration;
    static constexpr ImGuiWindowFlags   PREFERENCES_WINDOW_FLAGS        = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse;
    //
    //  SIZES:
    static constexpr ImVec2             ASKOKCANCEL_WINDOW_SIZE         = ImVec2(400.0f,        200.0f);
    static constexpr ImVec2             PREFERENCES_WINDOW_SIZE         = ImVec2(668.0f,        520.0f);
    static constexpr float              ASKOKCANCEL_BUTTON_W            = 120.0f;
//
//
//
public:
    //  "Open"
    //      flags defaults to AlwaysAutoResize
    //
    void Open(const char *          id,
              BeginFn               body,
              ImGuiWindowFlags      flags = ImGuiWindowFlags_NoResize,// ImGuiWindowFlags_AlwaysAutoResize,
              CloseFn               on_close  = {})
    {
        queued_.push_back(
            Modal{ id, std::move(body), std::move(on_close), flags });
    }

    //  "Draw"
    //
    inline void Draw(void)
    {
        //------------------------------------------------------------------ 1
        // Promote any queued modals → active list and tell ImGui to show them
        //------------------------------------------------------------------
        for (auto& m : queued_) {
            ImGui::OpenPopup(m.id.c_str());
            active_.push_back(std::move(m));
        }
        queued_.clear();

        //------------------------------------------------------------------ 2
        // Iterate through live pop-ups
        //------------------------------------------------------------------
        for (auto it = active_.begin(); it != active_.end(); ) {
            Modal& m = *it;

            // *** Pass &m.open so the title-bar “×” and Esc toggle it ***  ★
            bool visible = ImGui::BeginPopupModal(
                               m.id.c_str(),
                               &m.open,                    // <- this pointer is required
                               m.flags);                   // (AlwaysAutoResize by default)

            if (visible) {
                Context ctx{m};
                m.begin(ctx);                             // user-supplied body

                // ----------------------------------------------------------------
                // Explicit Esc key fallback: if the window is focused but the
                // automatic mechanism didn't fire, close manually.           ★
                // ----------------------------------------------------------------
                if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
                    ImGui::IsKeyPressed(ImGuiKey_Escape, /*repeat*/false))
                {
                    ctx.close(false);                    // treat as “Cancel”
                }

                ImGui::EndPopup();
            }

            // --------------------------------------------------------------------
            // Remove modal if    (a) ImGui closed it (× / Esc)   OR
            //                    (b) user code called ctx.close()
            // --------------------------------------------------------------------
            if (!visible || !m.open) {
                if (m.on_close) m.on_close(m.accepted);  // accepted==true only on OK
                it = active_.erase(it);                  // destroy modal
            } else {
                ++it;
            }
        }
    }

private:
    std::vector<Modal> queued_;
    std::vector<Modal> active_;
};

// global wrappers -----------------------------------------------------------
inline Manager& manager() { static Manager inst; return inst; }

inline void Open(const char* id,
                 BeginFn     body,
                 ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize,
                 CloseFn     on_close  = {})
{
    manager().Open(id, std::move(body), flags, std::move(on_close));
}

inline void Draw() { manager().Draw(); }


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
inline void ask_ok_cancel(const char *              id,
                          const char *              message,
                          std::function<void()>     on_ok,
                          ImGuiWindowFlags          flags       = popup::Manager::ASKOKCANCEL_WINDOW_FLAGS)
{
    using namespace popup;
    
    //------------------------------------------------------------------
    //  1.  Fixed size & centred position
    //------------------------------------------------------------------
    const ImGuiViewport *   vp      = ImGui::GetMainViewport();
    const ImVec2            pos     { vp->Pos.x + (vp->Size.x - Manager::ASKOKCANCEL_WINDOW_SIZE.x) * 0.5f,
                                      vp->Pos.y + (vp->Size.y - Manager::ASKOKCANCEL_WINDOW_SIZE.y) * 0.5f };

    //------------------------------------------------------------------
    // 2. Queue modal
    //------------------------------------------------------------------
    ImGui::SetNextWindowSize(Manager::ASKOKCANCEL_WINDOW_SIZE,  ImGuiCond_Appearing);
    ImGui::SetNextWindowPos (pos,                               ImGuiCond_Appearing);
    popup::Open(
        /*  Title       */          id,
        /*  BeginFn     */          [msg = std::string(message)](popup::Context & ctx)
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
        /*  window flags    */      flags,
        /*  on_close        */      [cb = std::move(on_ok)](bool accepted)      { if (accepted && cb) { cb(); } }
    );
    
    return;
}


//  "open_preferences_popup"
//  Opens a macOS-style Preferences window and calls `body(ctx)` each frame.
//
inline void open_preferences_popup(const char *             id,
                                   popup::BeginFn           body,
                                   ImVec2                   size    = popup::Manager::PREFERENCES_WINDOW_SIZE,
                                   ImGuiWindowFlags         flags   = popup::Manager::PREFERENCES_WINDOW_FLAGS)
{
    const ImGuiViewport *   vp      = ImGui::GetMainViewport();
    ImVec2                  pos{ vp->Pos.x + (vp->Size.x - size.x)*0.5f, vp->Pos.y + 120.0f };

    ImGui::SetNextWindowSize(size, ImGuiCond_Appearing);
    ImGui::SetNextWindowPos (pos,  ImGuiCond_Appearing);

    popup::Open( id, std::move(body), flags );
    
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


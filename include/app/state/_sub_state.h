/***********************************************************************************
*
*       ********************************************************************
*       ****           _ S U B _ S T A T E . h  ____  F I L E           ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      August 09, 2025.
*
*       ********************************************************************
*                FILE:      [include/app/state/_state.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_APP_SUB_STATE_H
#define _CBAPP_APP_SUB_STATE_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  1.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
#include "utility/utility.h"
#include "widgets/widgets.h"
//
#include "app/state/_init.h"
#include "app/state/_config.h"
#include "app/state/_types.h"



//  1.2     STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <cstdlib>          // C-Headers...
#include <stdio.h>
#include <unistd.h>
#include <chrono>

#include <string>           //  <======| std::string, ...
#include <string_view>
#include <string_view>
#include <vector>           //  <======| std::vector, ...
#include <stdexcept>        //  <======| ...
#include <functional>
#include <limits.h>
#include <math.h>
#include <atomic>



//  1.3     "DEAR IMGUI" HEADERS...
#include "imgui.h"
#include "implot.h"
#include "imgui_internal.h"






namespace cb { namespace app { //     BEGINNING NAMESPACE "cb" :: "app"...
// *************************************************************************** //
// *************************************************************************** //








//  "MenuState_t"
//
struct MenuState_t {
// *************************************************************************** //
// *************************************************************************** //
//
//  0.              CONSTANTS AND ALIASES...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    _CBAPP_APPSTATE_ALIAS_API       //  CLASS-DEFINED, NESTED TYPENAME ALIASES.

    // *************************************************************************** //
    //      PUBLIC API.
    // *************************************************************************** //
    //                              DELETED OPERATORS AND FUNCTIONS:
                                        MenuState_t                 (const MenuState_t &    )       = delete;   //  Copy. Constructor.
                                        MenuState_t                 (MenuState_t &&         )       = delete;   //  Move Constructor.
    MenuState_t &                       operator =                  (const MenuState_t &    )       = delete;   //  Assgn. Operator.
    MenuState_t &                       operator =                  (MenuState_t &&         )       = delete;   //  Move-Assgn. Operator.
    


    // *************************************************************************** //
    //      STATIC AND CONSTEXPR VARIABLES.
    // *************************************************************************** //
    
    // *************************************************************************** //
    //      GENERIC CONSTANTS.
    // *************************************************************************** //



// *************************************************************************** //
//
//
//  1.              CLASS DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //                      STATE INFORMATION.
    // *************************************************************************** //
    //
    //                                  MAIN STATE INFORMATION:
    CBMenuCapabilityFlags                   m_capabilities                  = CBMenuCapabilityFlags_None;
    
    //
    //                                  SECONDARY STATE INFORMATION:
    int                                     m_undo_count                    = 0;
    int                                     m_redo_count                    = 0;
    bool                                    m_dirty                         = 0;
    //
    //
    //                                  FILE I/O:
    std::filesystem::path                   m_filepath                      = {  };
    //
    //
    //                                  SUB-STATE INFORMATION:
    MenuCallbacks                           m_callbacks                     = {  };


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //                      FUTURE STUFF...
    // *************************************************************************** //
    //      std::atomic<bool>                       m_running                       = { true };
    //      std::atomic<bool>                       m_closeable                     = { true };
    
    //
    //
    //                                          UNDO / REDO STACK:
    //      std::vector< UnReDo<T> >                m_undo_stack                    = { true };     //  Vector of UNDO/REDO Data.
    //      std::vector< UnReDo<T> >::size_type     m_undo_top                      = 0;            //  Index of the current UNDO/REDO Item.
    //
    //
    //                                          FILE I/O:
    //      std::filesystem::path                   m_filepath                      = {  };
    //      std::filesystem::path                   m_last_dir                      = {  };     //  Track the last directory we visited in file navigator.
    //      std::vector< std::filesystem::path >    m_recent_files                  = {  };     //  Vector to store the recently opened files...
    
    
    // *************************************************************************** //
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "CLASS DATA-MEMBERS".





   
// *************************************************************************** //
//
//
//  2.C                 INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //                      INITIALIZATION FUNCTIONS.
    // *************************************************************************** //
    
    //  Default Constructor.
    inline MenuState_t                                              (void)      {   }
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //                      UTILITY FUNCTIONS...
    // *************************************************************************** //
    
    [[nodiscard]] inline bool           can_undo                    (void)      { return false; }
    [[nodiscard]] inline bool           can_redo                    (void)      { return false; }
    //
    //
    [[nodiscard]] inline bool           has_assigned_file           (void)      { return false; }
    
 
 
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //                      PRIMARY OPERATION FUNCTIONS...
    // *************************************************************************** //
    

    

//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.







// *************************************************************************** //
// *************************************************************************** //
};//	END "MenuState_t" STRUCT INTERFACE.












//  "TaskState_t"
//
struct TaskState_t {
// *************************************************************************** //
// *************************************************************************** //
//
//  0.              CONSTANTS AND ALIASES...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    _CBAPP_APPSTATE_ALIAS_API       //  CLASS-DEFINED, NESTED TYPENAME ALIASES.

    // *************************************************************************** //
    //      PUBLIC API.
    // *************************************************************************** //
    //                              DELETED OPERATORS AND FUNCTIONS:
                                        TaskState_t                 (const TaskState_t &    )       = delete;   //  Copy. Constructor.
                                        TaskState_t                 (TaskState_t &&         )       = delete;   //  Move Constructor.
    TaskState_t &                       operator =                  (const TaskState_t &    )       = delete;   //  Assgn. Operator.
    TaskState_t &                       operator =                  (TaskState_t &&         )       = delete;   //  Move-Assgn. Operator.
    


    // *************************************************************************** //
    //      STATIC AND CONSTEXPR VARIABLES.
    // *************************************************************************** //
    static constexpr size_t             ms_TASK_NAME_LIMIT              = 64;
    //
    std::array< std::string *, static_cast<size_t>(Applet_t::Count) >                       //  No CONSTEXPR arr for this bc I want it to copy the
                                        m_applets                       = {};               //  window names EXACTLY in case we ever rename them.
    
    // *************************************************************************** //
    //      GENERIC CONSTANTS.
    // *************************************************************************** //
    std::string                         ms_IDLE_APPLET_NAME;
    std::string                         ms_UNDEFINED_APPLET_NAME;



// *************************************************************************** //
//
//
//  1.              CLASS DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //                      STATE INFORMATION.
    // *************************************************************************** //
    //                              MAIN STATE INFORMATION:
    Applet                              m_current_task                  = Applet::None;
    ImGuiWindow *                       m_nav_window                    = nullptr;
    std::string                         m_und_task_name                 = {  };
    //
    ImGuiViewport *                     focused_viewport                = nullptr; // Which viewport (if any)
    //
    //                              SECONDARY STATE INFORMATION:
    bool                                has_focus                       = false;   // Any ImGui platform window focused by OS?
    bool                                reliable                        = false;   // True if determined via Platform_GetWindowFocus
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //                      SUB-STATE.
    // *************************************************************************** //
    MenuState_t                         m_default_menu_state            = {  };
    MenuState_t *                       m_current_menu_state            = { &m_default_menu_state };
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //                      ACTION / EVENT STUFF.
    // *************************************************************************** //
    std::atomic<CBSignalFlags>          m_pending                       = { CBSignalFlags_None };
    bool                                m_dialog_queued                 = false;
    bool                                m_rebuild_dockspace             = false;


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //                      FUTURE STUFF...
    // *************************************************************************** //
    //      std::atomic<bool>                   m_running                       = { true };
    //      std::atomic<bool>                   m_closeable                     = { true };
    
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //                      DATA...
    // *************************************************************************** //
    //      GLFWwindow *                        m_glfw_window                   = nullptr;
    
    
    
    // *************************************************************************** //
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "CLASS DATA-MEMBERS".





   
// *************************************************************************** //
//
//
//  2.C                 INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //                      INITIALIZATION FUNCTIONS.
    // *************************************************************************** //
    
    //  Default Constructor.
    inline TaskState_t                                              (void)
        : ms_IDLE_APPLET_NAME("Idle"),
          ms_UNDEFINED_APPLET_NAME("---")
    {
        //  ...
    }
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //                      UTILITY FUNCTIONS...
    // *************************************************************************** //
    
    //  "current_task_name"
    inline const char *                 current_task_name           (void) {
        if ( this->m_current_task == Applet::Undefined )
        {
            if ( this->m_und_task_name.empty() )
            { this->m_und_task_name   = std::string( (this->m_nav_window) ? this->m_nav_window->Name : "NULL"); }
            
            return this->m_und_task_name.c_str();
        }

        return this->m_applets[ static_cast<size_t>(this->m_current_task) ]->c_str();
    }
    
    
    //  "get_nav_window"
    [[nodiscard]] inline ImGuiWindow *  get_nav_window              (void) noexcept {
        //  Text("NavWindow: '%s'", g.NavWindow ? g.NavWindow->Name : "NULL");
		ImGuiContext *	    g	    = ImGui::GetCurrentContext();
		return ( g )    ? g->NavWindow      : nullptr;
	}
 
 
    //  "StateHasIO"
    [[nodiscard]] inline bool           StateHasIO                  (void) noexcept {
        switch ( this->m_current_task ) {
            case Applet::Undefined      : { return false;   }
            default                     : { return true;    }
        }
    }
 
 
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //                      PRIMARY OPERATION FUNCTIONS...
    // *************************************************************************** //
    
    //  "update_current_task"
    //
    inline void                         update_current_task         (GLFWwindow * window)
    {
        static constexpr size_t     CACHE_COMPARE_NUM       = 32;
        static constexpr size_t     LOOP_COMPARE_NUM        = 16;
        //
        //
        const bool                  app_is_idle             = ( glfwGetWindowAttrib(window, GLFW_FOCUSED) == 0 );
        //
        //
        ImGuiWindow *               vis_win                 = this->get_nav_window();
        const char *                vis                     = (vis_win) ? vis_win->Name     : nullptr;
        //
        //  const char *                vis                     = this->GetDockNodeVisText( this->m_main_node );
        const char *                name                    = this->current_task_name();
        bool                        match                   = false;
        
        
        
        
        this->m_nav_window                                  = vis_win;
        this->_update_task_state();
        
        
        
        
        //  CASE 0 :    ENTIRE APPLICATION IS UN-FOCUSED...
        if ( !vis )     {
            this->m_current_task = Applet::None;
            this->m_und_task_name.clear();
        }
        else
        {
            //  CASE 1 :    CURRENT WINDOW HAS NOT CHANGED SINCE OUR LAST FRAME (NO NEED TO CHANGE)...
            if ( strncmp(name, vis, CACHE_COMPARE_NUM) != 0 ) [[unlikely]]     //  Bail out early if same applet is in use.
            {
                //  CASE 2 :    COMPARE THE CURRENT WINDOW NAME TO THE NAME OF EACH THE APPLET...
                for (size_t i = 0; !match && i < static_cast<size_t>(Applet::Count); ++i) {
                    name    = m_applets[ static_cast<size_t>( i ) ]->c_str();
                    match   = ( strncmp(name, vis, LOOP_COMPARE_NUM) == 0 );
                    if (match)          {
                        this->m_current_task = static_cast<Applet>( i );
                        this->m_und_task_name.clear();
                    }
                }
                
                //  CASE 3 :    NO MATCH---A NON-NAMED WINDOW IS OPEN...
                if (!match) {
                    this->m_current_task        = Applet::Undefined;
                    this->m_und_task_name       = std::string(cblib::utl::fmt_imgui_string( vis_win->Name, ms_TASK_NAME_LIMIT ));
                }
            }
        }
        
        
        
        return;
    }
    
    
    //  "_update_task_state"
    //
    inline void                         _update_task_state          (void) noexcept
	{
		ImGuiContext*           ctx     = ImGui::GetCurrentContext();
        
		if ( !ctx )                     { return; }

		ImGuiPlatformIO &       pio     = ctx->PlatformIO;

		// Preferred: query the backend for OS focus across all platform windows (multi-viewport safe).
		if ( pio.Platform_GetWindowFocus )
		{
			for (ImGuiViewportP * vp : ctx->Viewports)
			{
				if  (!vp->PlatformWindowCreated )       { continue; }       //  Skip viewports that don't own a platform window
                
				if ( pio.Platform_GetWindowFocus(vp) ) {                    //  Backend says this viewport is focused by the OS
					this->has_focus           = true;
					this->focused_viewport    = vp;
					break;
				}
			}
			this->reliable = true;
			return;
		}

		// Fallback (heuristic): use the IO focus-lost event and assume main viewport.
		// Note: io.AppFocusLost is typically set on the frame focus is lost (event-style).
		ImGuiIO &   io              = ctx->IO;
		this->has_focus             = !io.AppFocusLost;        // May be transient; treat as best-effort when backend lacks the callback
		this->focused_viewport      = ImGui::GetMainViewport();
		this->reliable              = false;
		return;
	}


    

//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.







// *************************************************************************** //
// *************************************************************************** //
};//	END "TaskState_t" STRUCT INTERFACE.































// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" :: "app" NAMESPACE.






#endif      //  _CBAPP_APP_SUB_STATE_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.

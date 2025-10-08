/***********************************************************************************
*
*       ********************************************************************
*       ****           _ S U B _ S T A T E . h  ____  F I L E           ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
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

//  1.1.        STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <cstdlib>          // C-Headers...
#include <stdio.h>
#include <unistd.h>
#include <chrono>
#include <type_traits>

#include <string>           //  <======| std::string, ...
#include <string_view>
#include <string_view>
#include <vector>           //  <======| std::vector, ...
#include <stdexcept>        //  <======| ...
#include <functional>
#include <limits.h>
#include <math.h>
#include <atomic>


//  1.2.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
#include "utility/utility.h"
#include "widgets/widgets.h"
//
#include "app/state/_init.h"
#include "app/state/_config.h"
#include "app/state/_types.h"


//  1.3.        "DEAR IMGUI" HEADERS...
#include "imgui.h"
#include "implot.h"
#include "imgui_internal.h"






namespace cb { namespace app { //     BEGINNING NAMESPACE "cb" :: "app"...
// *************************************************************************** //
// *************************************************************************** //



//  "MenuState_t"
//      PLAIN-OLD-DATA (POD) STRUCT.
//
struct MenuState_t
{
    // *************************************************************************** //
    //      0. |    NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    _CBAPP_APPSTATE_INTERNAL_ALIAS_API       //  CLASS-DEFINED, NESTED TYPENAME ALIASES.
    
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      0. |    STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    
    //  "_MakeOrchid"
    [[nodiscard]] static inline Orchid &    _MakeOrchid                     (void) noexcept {
        static Orchid s_orchid{};   return s_orchid;
    }
    
//
// *************************************************************************** //
// *************************************************************************** //   END "0.  CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//
//      1.          DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //
    
    // *************************************************************************** //
    //      1. |    STATE VARIABLES.
    // *************************************************************************** //
    //                                  MAIN STATE INFORMATION:
    CBMenuCapabilityFlags                   m_capabilities                  = CBMenuCapabilityFlags_Default;
    //
    //                                  FILE I/O:
    std::filesystem::path                   m_filepath                      = {   };
    std::vector<std::filesystem::path>      m_recent_files                  = {   };
    //
    //                                  SUB-STATE INFORMATION:
    MenuCallbacks                           m_callbacks                     = {   };
    //
    inline static Orchid &                  m_orchid                        = _MakeOrchid();
    


    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      1. |    IMPORTANT DATA-MEMBERS.
    // *************************************************************************** //
    
    
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      1. |    MUTABLE STATE INFO.
    // *************************************************************************** //
    //                                  MUTABLE STATE DATA:
    bool                                    m_dirty                         = 0;
    
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      1. |    FUTURE STUFF.
    // *************************************************************************** //
    //      std::atomic<bool>                       m_running                       = { true };
    //      std::atomic<bool>                       m_closeable                     = { true };
    //
    //                                          UNDO / REDO STACK:
    //      std::vector< UnReDo<T> >                m_undo_stack                    = { true };     //  Vector of UNDO/REDO Data.
    //      std::vector< UnReDo<T> >::size_type     m_undo_top                      = 0;            //  Index of the current UNDO/REDO Item.
    //
    //                                          FILE I/O:
    //      std::filesystem::path                   m_filepath                      = {  };
    //      std::filesystem::path                   m_last_dir                      = {  };     //  Track the last directory we visited in file navigator.
    //      std::vector< std::filesystem::path >    m_recent_files                  = {  };     //  Vector to store the recently opened files...
    
    
    
    // *************************************************************************** //
    
//
// *************************************************************************** //
// *************************************************************************** //   END "1.  DATA-MEMBERS".



// *************************************************************************** //
//
//
//      2.A.        MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
    // *************************************************************************** //
    //      INITIALIZATION METHODS.         |   "init.cpp" ...
    // *************************************************************************** //
    //  explicit                        MenuState_t                 (app::AppState & );             //  Def. Constructor.
    inline                              MenuState_t                 (void)                          {   }
                                        ~MenuState_t                (void)                          = default;
    
    // *************************************************************************** //
    //      DELETED FUNCTIONS.              |   ...
    // *************************************************************************** //
                                        MenuState_t                 (const MenuState_t &    src)       = delete;   //  Copy. Constructor.
                                        MenuState_t                 (MenuState_t &&         src)       = delete;   //  Move Constructor.
    MenuState_t &                       operator =                  (const MenuState_t &    src)       = delete;   //  Assgn. Operator.
    MenuState_t &                       operator =                  (MenuState_t &&         src)       = delete;   //  Move-Assgn. Operator.
    
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
    [[nodiscard]] inline bool                   has_capability          (CBMenuCapabilityFlags f) const noexcept    { return (m_capabilities & f) == f; }
 	[[nodiscard]] inline bool                   supports_any            (CBMenuCapabilityFlags f) const noexcept    { return (m_capabilities & f) != 0; }
	[[nodiscard]] inline bool                   supports_all            (CBMenuCapabilityFlags f) const noexcept    { return (m_capabilities & f) == f; }
    

    // *************************************************************************** //
    //      2.B. |  ORCHID FUNCTIONS.
    // *************************************************************************** //
	[[nodiscard]] inline Orchid::size_type      get_orchid_uuid         (void) const noexcept       { return this->m_orchid.get_uuid(); }
    
    [[nodiscard]] inline bool                   can_undo                (void) const noexcept       { return this->m_orchid.can_undo(); }
	[[nodiscard]] inline bool                   can_redo                (void) const noexcept       { return this->m_orchid.can_redo(); }
    
    [[nodiscard]] Orchid::size_type             undo_count              (void) const noexcept       { return this->m_orchid.undo_count(); }
	[[nodiscard]] Orchid::size_type             redo_count              (void) const noexcept       { return this->m_orchid.redo_count(); }
    
    [[nodiscard]] inline auto                   get_undo_label          (void) const noexcept       { return this->m_orchid.get_undo_label(); }
	[[nodiscard]] inline auto                   get_redo_label          (void) const noexcept       { return this->m_orchid.get_redo_label(); }
    
    
    //  "Orchid" Operation Functions...
    inline void                                 orchid_push             (OrchidAction && act) noexcept      { this->m_orchid.push( std::move(act) );    }
    inline void                                 orchid_undo             (void) noexcept                     { this->m_orchid.undo();                    }
    inline void                                 orchid_redo             (void) noexcept                     { this->m_orchid.redo();                    }
 


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.B. |  CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    
    //  "DrawCustomMenus"
    inline void                         DrawCustomMenus             (void) const noexcept
    {
        for ( const auto & menu : this->m_callbacks.custom_menus )
        {
            ImGui::BeginMenu( menu.label.c_str() );
                menu.render_fn();
            ImGui::EndMenu();
        }
        return;
    }


    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.B. |  UTILITY FUNCTIONS.
    // *************************************************************************** //
	[[nodiscard]] inline bool           has_assigned_file           (void) const noexcept       { namespace fs = std::filesystem; return fs::exists(m_filepath); }
	[[nodiscard]] inline bool           has_open_recent             (void) const noexcept       { return m_recent_files.empty(); }
    
    
    inline void                         enable_capability           (CBMenuCapabilityFlags flags) noexcept      { m_capabilities |= flags;  }       // force-on bits in 'flags'
    inline void                         set_capability              (CBMenuCapabilityFlags flags) noexcept      { m_capabilities |= flags;  }


    inline void                         disable_capability          (CBMenuCapabilityFlags flags) noexcept      { m_capabilities &= ~flags; }       // force-off bits in 'flags'
    inline void                         unset_capability            (CBMenuCapabilityFlags flags) noexcept      { m_capabilities &= ~flags; }


    inline void                         toggle_capability           (CBMenuCapabilityFlags flags) noexcept      { m_capabilities ^= flags;  }       // flip bits in 'flags'
    
    
    
    // *************************************************************************** //
    
//
//
// *************************************************************************** //
// *************************************************************************** //   END "2B.  INLINE" FUNCTIONS.



//
//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "MenuState_t" INLINE STRUCT DEFINITION.












//  "TaskState_t"
//      PLAIN-OLD-DATA (POD) STRUCT.
//
struct TaskState_t
{
    // *************************************************************************** //
    //      0. |    NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    _CBAPP_APPSTATE_INTERNAL_ALIAS_API          //  CLASS-DEFINED, NESTED TYPENAME ALIASES.
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      0. |    STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    static constexpr size_t             ms_TASK_NAME_LIMIT              = 20;
    //
    std::array< std::string *, static_cast<size_t>(Applet_t::COUNT) >                       //  No CONSTEXPR arr for this bc I want it to copy the
                                        m_applets                       = {   };            //  window names EXACTLY in case we ever rename them.
    
//
// *************************************************************************** //
// *************************************************************************** //   END "0.  CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//      1.          DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //                      STATE INFORMATION.
    // *************************************************************************** //
    //                                  MAIN STATE INFORMATION:
    Applet                                  m_current_task;
    ImGuiWindow *                           m_nav_window                    = nullptr;
    std::string                             m_und_task_name                 = {   };
    std::string                             m_nav_window_name               = {   };
    //
    ImGuiViewport *                         focused_viewport                = nullptr; // Which viewport (if any)
    //
    //                                  SECONDARY STATE INFORMATION:
    bool                                    has_focus                       = false;   // Any ImGui platform window focused by OS?
    bool                                    reliable                        = false;   // True if determined via Platform_GetWindowFocus

    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      1. |    SUB-STATE.
    // *************************************************************************** //
    MenuState_t                             m_default_menu_state            = {   };
    std::reference_wrapper<MenuState_t>     m_current_menu_state            = { m_default_menu_state };
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      1. |    GENERIC DATA.
    // *************************************************************************** //
    std::atomic<CBSignalFlags>              m_pending                       = { CBSignalFlags_None };
    bool                                    m_dialog_queued                 = false;
    bool                                    m_rebuild_dockspace             = false;

    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      1. |    FUTURE STUFF.
    // *************************************************************************** //
    //      std::atomic<bool>                   m_running                       = { true };
    //      std::atomic<bool>                   m_closeable                     = { true };
    
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      1. |    DATA.
    // *************************************************************************** //
    //      GLFWwindow *                        m_glfw_window                   = nullptr;
    
    
    
    // *************************************************************************** //
    
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
    //  explicit                        TaskState_t                (app::AppState & );             //  Def. Constructor.
                                        //  TaskState_t                (void) noexcept                 = default;
                                        //  ~TaskState_t               (void)                          = default;
    
    // *************************************************************************** //
    //      DELETED FUNCTIONS.              |   ...
    // *************************************************************************** //
                                        TaskState_t                (const TaskState_t &    src)       = delete;   //  Copy. Constructor.
                                        TaskState_t                (TaskState_t &&         src)       = delete;   //  Move Constructor.
    TaskState_t &                       operator =                  (const TaskState_t &    src)       = delete;   //  Assgn. Operator.
    TaskState_t &                       operator =                  (TaskState_t &&         src)       = delete;   //  Move-Assgn. Operator.
    
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



    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.B. |  SETTER/GETTER FUNCTIONS.
    // *************************************************************************** //
    
    //  "GetCurrentAppletName"
    inline const char *                 GetCurrentAppletName                (void)
    { return this->m_applets[ static_cast<size_t>(this->m_current_task) ]->c_str(); }



    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.B. |  CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    
    //  Default Constructor.
    inline                              TaskState_t                         (void)
    {
        //  ...
    }



    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.B. |  UTILITY FUNCTIONS.
    // *************************************************************************** //
    
    //  "_OLD_current_task_name"
    inline const char *                 _OLD_current_task_name              (void) {
        //  if ( this->m_current_task == Applet::Undefined )
        //  {
        //      if ( this->m_nav_window_name.empty() )
        //      { this->m_nav_window_name   = std::string( (this->m_nav_window) ? this->m_nav_window->Name : "NULL"); }
        //
        //      return this->m_nav_window_name.c_str();
        //  }

        return this->m_applets[ static_cast<size_t>(this->m_current_task) ]->c_str();
    }

    //  "get_dock_node_vis_text"
    inline const char *                 get_dock_node_vis_text              (const ImGuiDockNode * node)
    { return (node && node->VisibleWindow) ? node->VisibleWindow->Name : "NULL"; } // Same expression used inside DebugNodeDockNode()
    
    //  "get_nav_window"
    [[nodiscard]] inline ImGuiWindow *  get_nav_window                      (void) noexcept {
        //  Text("NavWindow: '%s'", g.NavWindow ? g.NavWindow->Name : "NULL");
		ImGuiContext *	    g	    = ImGui::GetCurrentContext();
		return ( g )    ? g->NavWindow      : nullptr;
	}



    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      2.B. |  PRIMARY OPERATION FUNCTIONS.
    // *************************************************************************** //
    
    //  "update_current_task"
    //
    [[nodiscard]] inline bool           update_current_task                 ([[maybe_unused]] GLFWwindow * window)
    {
        static constexpr size_t     CACHE_COMPARE_NUM       = 32;
        static constexpr size_t     LOOP_COMPARE_NUM        = 16;
        //
        //
        //  const bool                  app_is_idle             = ( glfwGetWindowAttrib(window, GLFW_FOCUSED) == 0 );
        //
        //
        ImGuiWindow *               vis_win                 = this->get_nav_window();
        const char *                vis                     = (vis_win) ? vis_win->Name     : nullptr;
        //
        // const char *                vis                     = this->get_dock_node_vis_text( this->m_main_node );
        //
        const char *                name                    = this->GetCurrentAppletName();
        bool                        match                   = false;
        bool                        need_to_update          = false;
        
        
        this->m_nav_window                                  = vis_win;
        this->_update_task_state();
        
        
        //  CASE 0 :    ENTIRE APPLICATION IS UN-FOCUSED...
        if ( !vis )     {
            this->m_nav_window_name.clear();
        }
        else
        {
            //  CASE 1 :    CURRENT WINDOW HAS NOT CHANGED SINCE OUR LAST FRAME (NO NEED TO CHANGE)...
            if ( strncmp(name, vis, CACHE_COMPARE_NUM) != 0 ) [[unlikely]]     //  Bail out early if same applet is in use.
            {
                //  CASE 2 :    COMPARE THE CURRENT WINDOW NAME TO THE NAME OF EACH THE APPLET...
                for ( size_t i = 0; !match && i < static_cast<size_t>(Applet::COUNT); ++i )
                {
                    name    = m_applets[ static_cast<size_t>( i ) ]->c_str();
                    match   = ( strncmp(name, vis, LOOP_COMPARE_NUM) == 0 );
                    
                    if (match)          {
                        need_to_update          = true;
                        this->m_current_task    = static_cast<Applet>( i );
                        this->m_und_task_name.clear();
                    }
                }
                
                //  CASE 3 :    NO MATCH---A NON-NAMED WINDOW IS OPEN...
                if ( !match ) {
                    this->m_nav_window_name       = std::string(cblib::utl::fmt_imgui_string( vis_win->Name, ms_TASK_NAME_LIMIT ));
                }
            }
        }
        
        return need_to_update;
    }
    
    
    //  "_update_task_state"
    //
    inline void                         _update_task_state                  (void) noexcept
	{
		ImGuiContext*           ctx     = ImGui::GetCurrentContext();
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
 
 
 
    // *************************************************************************** //
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "2B.  INLINE" FUNCTIONS.



//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "TaskState_t" INLINE STRUCT DEFINITION.












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

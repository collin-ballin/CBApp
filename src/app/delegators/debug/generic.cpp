/***********************************************************************************
*
*       ********************************************************************
*       ****        C B _ D E B U G G E R . C P P  ____  F I L E        ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond.
*               DATED:      August 28, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/app.h"
#include "app/delegators/_detail_view.h"
#include <random>
#include <algorithm>



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //












// *************************************************************************** //
//
//
//
//      3.      "GENERIC TESTING FUNCTIONS"...
// *************************************************************************** //
// *************************************************************************** //


// *************************************************************************** //
//      3A. GENERIC.    |     SMALL_INTERNAL_NAMESPACE
// *************************************************************************** //
//
namespace anon { //     BEGINNING NAMESPACE "anon"...

    
    //      0.0.        ALIASES...
    //
    using TimePoint = cblib::utl::anon::time_point;
    
    
    //      0.1.        TYPES...
    //
    struct DockedWindowInfo {
		ImGuiID								window_id;
		std::string						    name;

		ImGuiID								dockspace_id;
		ImGuiID								dock_node_id;
		ImGuiID								viewport_id;

		bool							    is_visible;			// active & not hidden
		bool							    is_active_tab;		// active tab in its dock node
		bool							    is_focused;			// NavWindow == this window
		bool							    is_collapsed;

		ImVec2								pos;
		ImVec2								size;

		short							    dock_order;			// order within dock node
	};
    //
    //
	struct DockspaceState
	{
		std::uint64_t				        composition_signature	    = 0;
		std::vector<std::string>		    window_names                ;

		cblib::utl::anon::time_point	    last_update_time            {   };     // Timestamp of last composition update (i.e., when we rebuilt window_names).
	};
 
 
 
    //      0.2.        DATA...
    //
    // Per-dockspace memoized state (names + composition + timing).
    static std::unordered_map<ImGuiID, DockspaceState> s_dockspaces;
    //
    // Scratch buffers reused every frame to avoid reallocating.
    static std::vector<ImGuiWindow*>        s_scratch_windows           ;
    static std::vector<ImGuiID>             s_scratch_ids               ;
    
    
    
    


    //  "collect_windows_recursive"
    //      Internal helper: recursively collect ImGuiWindow* from a dock node tree.
    //
    static void collect_windows_recursive(ImGuiDockNode*             node,
                                          std::vector<ImGuiWindow*>& out_windows) noexcept(false)
    {
        if (!node)
            return;

        for (int i = 0; i < node->Windows.Size; ++i)
            out_windows.push_back(node->Windows[i]);

        collect_windows_recursive(node->ChildNodes[0], out_windows);
        collect_windows_recursive(node->ChildNodes[1], out_windows);
    }
    
    // -------------------------------------------------------------------------
    // Compute an order-insensitive signature for the set of window IDs.
    //
    // Strategy: collect all ImGuiID values, sort them, then hash the sorted
    // array using FNV-1a. This way, any permutation of the same set yields
    // the same signature.
    // -------------------------------------------------------------------------
    static std::uint64_t compute_composition_signature(
        const std::vector<ImGuiWindow*>& windows) noexcept(false)
    {
        s_scratch_ids.clear();
        s_scratch_ids.reserve(windows.size());

        for (ImGuiWindow* w : windows)
        {
            if (w)
                s_scratch_ids.push_back(w->ID);
        }

        std::sort(s_scratch_ids.begin(), s_scratch_ids.end());

        // FNV-1a over sorted IDs.
        std::uint64_t signature = 1469598103934665603ull;     // offset basis
        for (ImGuiID id : s_scratch_ids)
        {
            std::uint64_t value = static_cast<std::uint64_t>(id);
            signature ^= value;
            signature *= 1099511628211ull;                    // prime
        }

        return signature;
    }


    //  "update_dockspace_window_cache"
    //      Update memoized *composition* (just names/IDs) for a given dockspace.
	//      Only rebuild list when the set of docked windows (by ID) changes.
    //
    void update_dockspace_window_cache(ImGuiID dockspace_id) noexcept(false)
    {
        // 0 is "no dockspace" / invalid.
        if (dockspace_id == 0)
        {
            s_dockspaces.erase(0);
            return;
        }

        ImGuiDockNode* root_node = ImGui::DockBuilderGetNode(dockspace_id);
        if (!root_node)
        {
            // If the dockspace no longer exists, drop its cache.
            s_dockspaces.erase(dockspace_id);
            return;
        }

        s_scratch_windows.clear();
        s_scratch_windows.reserve(16);
        collect_windows_recursive(root_node, s_scratch_windows);

        const std::uint64_t signature = compute_composition_signature(s_scratch_windows);

        DockspaceState& state = s_dockspaces[dockspace_id];

        // If composition didn't change, keep cached names & timestamp.
        if (signature == state.composition_signature)
            return;

        // Composition changed: rebuild names and stamp update time.
        state.composition_signature = signature;
        state.window_names.clear();
        state.window_names.reserve(s_scratch_windows.size());

        for (ImGuiWindow* window : s_scratch_windows)
        {
            if (window)
                state.window_names.emplace_back(window->Name);
        }

        // Record the time of this cache rebuild using your cooldown API.
        cblib::utl::update_cooldown_timer(state.last_update_time);
    }


    //  "get_dockspace_window_names"
    //      Access cached window names for a dockspace.
	//      Returns an empty vector reference if the dockspace is unknown or empty.
    //
    [[nodiscard]] const std::vector<std::string>&
    get_dockspace_window_names(ImGuiID dockspace_id) noexcept
    {
        static const std::vector<std::string> s_empty;

        const auto it = s_dockspaces.find(dockspace_id);
        if (it == s_dockspaces.end())
            return s_empty;

        return it->second.window_names;
    }
 
 
 	// -------------------------------------------------------------------------
	// Get the last-update timestamp for a given dockspace.
	// Returns true on success and writes into 'out_time'.
	// Returns false if the dockspace_id has no cached state yet.
	// -------------------------------------------------------------------------
    [[nodiscard]] bool
    get_dockspace_last_update_time(ImGuiID dockspace_id,
                                   TimePoint&            out_time) noexcept
    {
        const auto it = s_dockspaces.find(dockspace_id);
        if (it == s_dockspaces.end())
            return false;

        out_time = it->second.last_update_time;
        return true;
    }
 
 
 	// -------------------------------------------------------------------------
	// Compute the number of seconds since the last cache update for a dockspace.
	//
	// Returns:
	//   - 0.0 if the dockspace has no record yet (never updated / unknown).
	//   - otherwise, a positive double representing elapsed seconds.
	// -------------------------------------------------------------------------
    [[nodiscard]] double
    get_dockspace_seconds_since_last_update(ImGuiID dockspace_id) noexcept
    {
        const auto it = s_dockspaces.find(dockspace_id);
        if (it == s_dockspaces.end())
            return 0.0;

        const TimePoint& last = it->second.last_update_time;

        // If never stamped (defensive), treat as "no time elapsed".
        if (last == TimePoint{})
            return 0.0;

        const auto now   = cblib::utl::get_cooldown_timer();
        const auto delta = now - last;

        using seconds_f = std::chrono::duration<double>;
        return std::chrono::duration_cast<seconds_f>(delta).count();
    }
 
 
 
 	// -------------------------------------------------------------------------
	// Build rich descriptors for all windows currently docked in a dockspace.
	//
	// NOTE:
	//  - This *always* reads current-frame state (visibility, focus, etc.).
	//  - It reuses `out_infos` capacity; call it every frame if you want live data.
	//  - The heavier "names + composition" part is still memoized above.
	// -------------------------------------------------------------------------
    void gather_docked_window_info(ImGuiID                          dockspace_id,
                                   std::vector<DockedWindowInfo> &  out_infos) noexcept(false)
    {
        ImGuiDockNode *     root_node       = nullptr;
        out_infos           .clear();


        if (dockspace_id == 0)      { return; }
        root_node                           = ImGui::DockBuilderGetNode(dockspace_id);
        if (!root_node)             { return; }


        s_scratch_windows   .clear();
        s_scratch_windows   .reserve(16);
        collect_windows_recursive   (root_node, s_scratch_windows);
        out_infos           .reserve(s_scratch_windows.size());

        ImGuiContext *      ctx             = ImGui::GetCurrentContext();
        ImGuiWindow *       nav_window      = ctx   ? ctx->NavWindow    : nullptr;


        for (ImGuiWindow * window : s_scratch_windows)
        {
            if (!window)            { continue; }

            ImGuiDockNode *     node            = window->DockNode;
            ImGuiID             dock_node_id    = node      ? node->ID      : 0;


            bool                is_active_tab   = false;                                    //  Active tab: either the node's visible window or DockIsActive flag.
            
            if (node && node->VisibleWindow == window)      { is_active_tab = true; }
            else if (window->DockIsActive)                  { is_active_tab = true; }

            const bool          is_visible      = (window->Active && !window->Hidden);      //  Visibility: ImGui internal notion (active & not hidden).
            const bool          is_focused      = (nav_window == window);                   //  Focus: use NavWindow as proxy for "keyboard focus".

            ImGuiViewport *     viewport        = window->Viewport;
            ImGuiID             viewport_id     = viewport  ? viewport->ID  : 0;


            //  CREATE INFO OBJECT...
            DockedWindowInfo    info            {   };
            info.window_id                      = window->ID;
            info.name                           = window->Name;
            //
            info.dockspace_id                   = dockspace_id;
            info.dock_node_id                   = dock_node_id;
            info.viewport_id                    = viewport_id;
            //
            info.is_visible                     = is_visible;
            info.is_active_tab                  = is_active_tab;
            info.is_focused                     = is_focused;
            info.is_collapsed                   = window->Collapsed;
            //
            info.pos                            = window->Pos;
            info.size                           = window->Size;
            //
            info.dock_order                     = window->DockOrder;

            out_infos.emplace_back(std::move(info));
        }
        return;
    }
    
 
 
    //  "clear_dockspace_cache"
	//      Optional helpers for manual control over caches.
    //
	void	clear_dockspace_cache(ImGuiID dockspace_id) noexcept
	{
		s_dockspaces.erase(dockspace_id);
	}


    //  "clear_all_dockspace_caches"
    //
	void clear_all_dockspace_caches(void) noexcept
	{
		s_dockspaces.clear();
	}



//
//
// *************************************************************************** //
// *************************************************************************** //   END [ 1.0.  "TYPES" ].



// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}// END NAMESPACE "anon".






// *************************************************************************** //
//      3B. GENERIC.    |     ORCHESTRATOR FUNCTIONS.
// *************************************************************************** //
static inline void          draw_docked_window_cache_stats          (ImGuiID );
static inline void          draw_docked_window_verbose_debug        (ImGuiID );
static inline void          draw_docked_window_list                 (ImGuiID );



//  "TestGeneric"
//
void CBDebugger::TestGeneric(void) noexcept
{
    static bool     show_main_dockspace     = false;
    static bool     show_main_windows       = false;
    static bool     show_detview_windows    = false;
    static bool     show_browser_windows    = false;
    
    
    //      2.      INTERACTION WIDGETS...
    //
    ImGui::Checkbox("Show Dockspace Windows"    , &show_main_dockspace      );
    ImGui::Checkbox("Show Main Windows"         , &show_main_windows        );
    ImGui::Checkbox("Show DetView Windows"      , &show_detview_windows     );
    ImGui::Checkbox("Show Browser Windows"      , &show_browser_windows     );
    
    
    
    ImGui::NewLine();
    ImGui::NewLine();
    
    
    
    //      3.      DEBUG DISPLAYS...
    //
    //              3A.      MAIN DOCKSPACE:
    if (show_main_dockspace)
    {
        ImGui::SeparatorText                ("Dockspace:");
        draw_docked_window_cache_stats      (this->S.m_dockspace_id);
        this->draw_docked_window_debug      (this->S.m_dockspace_id);
    }
    //              3B.      MAIN DOCKSPACE:
    if (show_main_windows)
    {
        if (show_main_dockspace)    { ImGui::NewLine(); }
        
        ImGui::SeparatorText                ("Main Dockspace:");
        draw_docked_window_cache_stats      (this->S.m_main_dock_id);
        this->draw_docked_window_debug      (this->S.m_main_dock_id);
    }
    //
    //              3C.      DETVIEW DOCKSPACE:
    if (show_detview_windows)
    {
        if (show_main_dockspace || show_main_dockspace)
            { ImGui::NewLine(); }
        
        
        ImGui::SeparatorText                ("DetView Dockspace:");
        draw_docked_window_cache_stats      (this->S.m_detview_dockspace_id);
        this->draw_docked_window_debug      (this->S.m_detview_dockspace_id);
    }
    //
    //              3D.      DETVIEW DOCKSPACE:
    if (show_browser_windows)
    {
        if (show_main_dockspace || show_main_dockspace || show_detview_windows)
            { ImGui::NewLine(); }
        
        
        ImGui::SeparatorText                ("Browser:");
        draw_docked_window_cache_stats      (this->S.m_browser_dock_id);
        this->draw_docked_window_debug      (this->S.m_browser_dock_id);
    }
    
    
    
    return;
}



// *************************************************************************** //
//      3C. GENERIC.    |     SPECIFIC FUNCTIONS.
// *************************************************************************** //

//  "draw_docked_window_cache_stats"
//
static inline void draw_docked_window_cache_stats(ImGuiID dockspace_id)
{
    anon::update_dockspace_window_cache(dockspace_id);

    const auto& names        = anon::get_dockspace_window_names(dockspace_id);
    const double seconds     = anon::get_dockspace_seconds_since_last_update(dockspace_id);

    ImGui::Text("Dockspace 0x%08X", dockspace_id);
    ImGui::BulletText("Cached windows: %d", (int)names.size());
    ImGui::BulletText("Seconds since last cache update: %.3f", seconds);
}



//  "draw_docked_window_debug"
//

inline void CBDebugger::draw_docked_window_debug(ImGuiID dockspace_id) const noexcept
{
	static std::vector<anon::DockedWindowInfo>	s_infos;

	anon::gather_docked_window_info(dockspace_id, s_infos);

	for (const auto & info : s_infos)
	{
        const bool      highlight       = (info.is_visible  /*&&  info.is_active_tab  &&  info.is_focused*/  &&  !info.is_collapsed );
        const bool      active          = (info.is_visible  &&  info.is_active_tab  &&  info.is_focused  &&  !info.is_collapsed );
        
        if (active) {
            ImGui::TextColored( this->S.SystemColor.Blue, "%s", info.name.c_str() );
        }
        else {
            this->S.TFColoredText( highlight, info.name.c_str() );
        }
        
		ImGui::BulletText("ID: 0x%08X, DockNode: 0x%08X, Viewport: 0x%08X",
		                  info.window_id, info.dock_node_id, info.viewport_id);
                          
                          
		ImGui::BulletText("Visible: %d, ActiveTab: %d, Focused: %d, Collapsed: %d",
		                  info.is_visible, info.is_active_tab, info.is_focused, info.is_collapsed);
                          
                          
		ImGui::BulletText("DockOrder:%d", info.dock_order);
	}

    return;
}






//  "draw_docked_window_verbose_debug"
//
static inline void draw_docked_window_verbose_debug(ImGuiID dockspace_id)
{
	static std::vector<anon::DockedWindowInfo>	s_infos;

	anon::gather_docked_window_info(dockspace_id, s_infos);

	for (const auto & info : s_infos)
	{
		ImGui::Text("Window: %s", info.name.c_str());
		ImGui::BulletText("ID: 0x%08X, DockNode: 0x%08X, Viewport: 0x%08X",
		                  info.window_id, info.dock_node_id, info.viewport_id);
                          
		ImGui::BulletText("Visible: %d, ActiveTab: %d, Focused: %d, Collapsed: %d",
		                  info.is_visible, info.is_active_tab, info.is_focused, info.is_collapsed);
                          
		ImGui::BulletText("Pos:(%.1f,%.1f)  Size:(%.1f,%.1f)  DockOrder:%d",
		                  info.pos.x, info.pos.y, info.size.x, info.size.y, info.dock_order);
	}

    return;
}



    

//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 3.  "GENERIC FUNCTIONS" ]].












// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.






// *************************************************************************** //
// *************************************************************************** //
//
//  END.

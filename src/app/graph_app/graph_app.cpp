/***********************************************************************************
*
*       ********************************************************************
*       ****          G R A P H _ A P P . C P P  ____  F I L E          ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond
*               DATED:      May 18, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/graph_app/graph_app.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//  1B.     PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "Begin"
//
void GraphApp::Begin([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
{
    static bool                     init                    = true;
    static constexpr const char *   controls_uuid           = "Controls";



    //  1.  CREATING THE HOST WINDOW...
    ImGui::Begin(uuid, p_open, flags);
        //
        //  Generate a persistent dockspace ID
        m_dockspace_id          = ImGui::GetID(m_dockspace_name);
        
        ImGui::DockSpace(m_dockspace_id,    ImVec2(0.0f, 0.0f),     m_dockspace_flags);
        if (init) [[unlikely]] {
            init = false;
            ImGui::DockBuilderRemoveNode    (m_dockspace_id); // clear any previous layout
            ImGui::DockBuilderAddNode       (m_dockspace_id, ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize   (m_dockspace_id, ImVec2(800, 600));

            // Example split: left and right
            m_dock_ids[1]   = ImGui::DockBuilderSplitNode( m_dockspace_id, ImGuiDir_Down, m_dockspace_ratio,
                                                           nullptr, &m_dock_ids[0] );

            ImGui::DockBuilderDockWindow(m_win_uuids[0],    m_dock_ids[0]);
            ImGui::DockBuilderDockWindow(m_win_uuids[1],    m_dock_ids[1]);
            ImGui::DockBuilderFinish(m_dockspace_id);
        }
        
        if (this->m_rebuild_dockspace) [[unlikely]] {
            this->m_rebuild_dockspace   = false;
            this->RebuildDockspace();
        }
        
    ImGui::End();
    
    
    //  2.  CREATE TOP WINDOW FOR PLOTS...
    ImGui::SetNextWindowClass(&this->m_window_class[0]);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, app::DEF_LIVE_LIGHTBLUE);
    ImGui::Begin(m_win_uuids[0], nullptr, m_docked_win_flags[0]);
        this->display_plots();
    ImGui::End();
    ImGui::PopStyleColor();
    
    
    //  3.  CREATE BOTTOM WINDOW FOR CONTROLS...
    if (m_child_open[1]) {
        ImGui::SetNextWindowClass(&this->m_window_class[1]);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, app::DEF_LIVE_DARKBLUE);
        ImGui::Begin(m_win_uuids[1], nullptr, m_docked_win_flags[1]);
            this->display_controls();
        ImGui::End();
        ImGui::PopStyleColor();
    }
    
    
    //  4.  DEFINE COLLAPSE/EXPAND TOOLBAR BUTTON...
    static toolbar::config  ctrl_toolbar{
    //  PARENT UUID.      PARENT ANCHOR.        HOST UUID.      HOST ANCHOR.                PARENT OFFSET.      HOST OFFSET.
        m_win_uuids[1], utl::Anchor::NorthWest, uuid,           utl::Anchor::SouthWest,     {0.0f, 0.0f},       {0.0f, -45.0f}
    };
    
    if ( toolbar::begin("##sidebar_toolbar", ctrl_toolbar) )
    {
        
        if ( utl::DirectionalButton("##toggle", m_child_open[1] ? Anchor::South : Anchor::North, ImVec2{20,20}) )
        {
            m_child_open[1]     = !m_child_open[1];
            //sidebar_ratio       = show_sidebar ? 0.60 : 0.05f;
        }
    }
    toolbar::end();
    
    
    
    return;
}




// *************************************************************************** //
//
//
//  2.  PRIMARY "PLOT" AND "CONTROL" FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "display_plots"
//
void GraphApp::display_plots(void)
{
    //  DEFINE MISC. VARIABLES...
    static size_t                   N                           = 0;
    static bool                     SHOW_HELP_TABS              = true;
    static bool                     ENABLE_ADDING_TABS          = true;
    

    //  BEGIN THE TAB BAR...
    if ( ImGui::BeginTabBar(m_tabbar_uuids[0], m_tabbar_flags[0]) )
    {
        //      1.      DRAW HELP-MENU TAB-BUTTON ITEM ("?")...
        if (SHOW_HELP_TABS) {
            if (ImGui::TabItemButton("?", ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_NoTooltip))
                ImGui::OpenPopup("MyHelpMenu");
        }
        if (ImGui::BeginPopup("MyHelpMenu")) {
            ImGui::Selectable("Hello!");
            ImGui::EndPopup();
        }


        //      2.      DRAW THE "ADD-TAB" BUTTON ("+")...
        //  if (ENABLE_ADDING_TABS) {
        //      if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip))
        //          { AddNewTabFunc(); /* Add new tab */ }
        //  }



        //      2.3     DRAW EACH OF THE TAB ITEMS...
        for (auto & tab : this->ms_PLOT_TABS)
        {
            if ( ImGui::BeginTabItem( tab.get_uuid(), (tab.no_close) ? nullptr : &tab.open, tab.flags ) )
            {
                if (tab.render_fn) {
                    tab.render_fn( tab.get_uuid(), &tab.open, tab.flags );
                }
                else {
                    this->DefaultTabRenderFunc(tab.get_uuid(), &tab.open, tab.flags);
                }
                
            ImGui::EndTabItem();
            }// END "BeginTabItem".
        
        } // END "for auto & tab".


    ImGui::EndTabBar();
        
    } // END "BeginTabBar".

    return;
}




//  "display_controls"
//
void GraphApp::display_controls(void)
{
    //  DEFINE MISC. VARIABLES...
    static bool                     ENABLE_ADDING_TABS          = true;
    

    //  BEGIN THE TAB BAR...
    if ( ImGui::BeginTabBar(m_tabbar_uuids[1], m_tabbar_flags[1]) )
    {
        //      1.      DRAW THE CLOSE-CONTROL-BAR BUTTON...
        if ( ImGui::TabItemButton( (this->m_child_open[1]) ? ms_TABBAR_OPEN_TEXT : ms_TABBAR_CLOSED_TEXT,
             ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_NoTooltip) )
        {
            this->m_child_open[1]       = !this->m_child_open[1];
            m_HEIGHT_LIMITS[1].first    = (this->m_child_open[1]) ? 0 : 30 * S.m_dpi_scale ;
            
            //m_docked_win_flags[1]  |= ImGuiWindowFlags_AlwaysAutoResize;
            
            
            m_dockspace_ratio           = (this->m_child_open[1]) ?  0.60f : 0.05f;
            this->m_rebuild_dockspace   = true;
        }
        
        
        //      2.      QUIT EARLY IF CONTROL TAB-BAR IS NOT OPEN...
        if (!this->m_child_open[1])
        {
            ImGui::EndTabBar();
            return;
        }
        

        //      2.3     DRAW EACH OF THE TAB ITEMS...
        for (auto & tab : this->ms_CTRL_TABS)
        {
            if ( ImGui::BeginTabItem( tab.get_uuid(), (tab.no_close) ? nullptr : &tab.open, tab.flags ) )
            {
                if (tab.render_fn) {
                    tab.render_fn( tab.get_uuid(), &tab.open, tab.flags );
                }
                else {
                    this->DefaultTabRenderFunc(tab.get_uuid(), &tab.open, tab.flags);
                }
                
            ImGui::EndTabItem();
            }// END "BeginTabItem".
        
        
        } // END "for auto & tab".
    ImGui::EndTabBar();
        
    
    } // END "BeginTabBar".

    return;
}
























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

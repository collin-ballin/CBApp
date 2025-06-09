/***********************************************************************************
*
*       ********************************************************************
*       ****       _ C O I N C I D E N C E . C P P  ____  F I L E       ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond
*               DATED:      May 10, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/c_counter/c_counter.h"




namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//  1B.     PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

void CCounterApp::toggle(void)     { this->m_child_open[1] = !m_child_open[1]; }

void CCounterApp::open(void)       { this->m_child_open[1] = true; }

void CCounterApp::close(void)      { this->m_child_open[1] = false; }


//  "Begin"
//
void CCounterApp::Begin([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
{
    //  1.  CREATING THE HOST WINDOW...
    ImGui::Begin(uuid, p_open, flags);
        this->display_plots();
    ImGui::End();
    
    
    
    //  2.  CREATE TOP WINDOW FOR PLOTS...
    if (m_detview_window.open) {
        ImGui::SetNextWindowClass(&this->m_window_class[1]);
        ImGui::Begin( m_detview_window.uuid.c_str(), nullptr, m_detview_window.flags );
            this->display_controls();
        ImGui::End();
    }
    
    

    //  //  3.  CREATE BOTTOM WINDOW FOR CONTROLS...
    //  if (m_child_open[1]) {
    //      ImGui::SetNextWindowClass(&this->m_window_class[1]);
    //      ImGui::PushStyleColor(ImGuiCol_WindowBg, app::DEF_LIVE_DARKBLUE);
    //      ImGui::Begin(m_win_uuids[1], nullptr, m_docked_win_flags[1]);
    //          this->display_controls();
    //      ImGui::End();
    //      ImGui::PopStyleColor();
    //  }
    
    
    
    

    
    return;
}






// *************************************************************************** //
//
//
//  2.      PROTECTED MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //


//  "display_plots"
//
void CCounterApp::display_plots(void)
{
    //  DEFINE MISC. VARIABLES...
    static bool                     SHOW_HELP_TABS              = true;
    static bool                     ENABLE_ADDING_TABS          = true;
    

    //  BEGIN THE TAB BAR...
    if ( ImGui::BeginTabBar(m_tabbar_uuids[0], m_tabbar_flags[0]) )
    {
        //  1.      DRAW HELP-MENU TAB-BUTTON ITEM ("?")...
        if (SHOW_HELP_TABS) {
            if (ImGui::TabItemButton("?", ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_NoTooltip))
                ImGui::OpenPopup("MyHelpMenu");
        }
        if (ImGui::BeginPopup("MyHelpMenu")) {
            ImGui::Selectable("Here's a help message!  The PLOTS go here, on the top.");
            ImGui::EndPopup();
        }


        if (ENABLE_ADDING_TABS)     //  2.      DRAW THE "ADD-TAB" BUTTON ("+")...
        {
            if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip))
            {   ImGui::OpenPopup("add_plot_tab_popup");     }
            //
            if (ImGui::BeginPopup("add_plot_tab_popup")) {  //          - NEW PLOTTING TAB POP-UP...
                //
                //  AddNewPlotTab();
                //
                ImGui::EndPopup();
            }// END "Popup".
        }



        //      2.3     DRAW EACH OF THE TAB ITEMS...
        for (auto & tab : this->ms_PLOT_TABS)
        {
            if ( ImGui::BeginTabItem( tab.get_uuid(), (tab.no_close) ? nullptr : &tab.open, tab.flags ) )
            {
                if (tab.render_fn) {
                    tab.render_fn( tab.get_uuid(), &tab.open, tab.flags );
                }
                else {
                    this->DefaultPlotTabRenderFunc(tab.get_uuid(), &tab.open, tab.flags);
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
void CCounterApp::display_controls(void)
{
    //  DEFINE MISC. VARIABLES...
    static  ImGuiTabItemFlags       SPACING_FLAGS   = ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_NoTooltip | ImGuiTabItemFlags_NoReorder;
    
    
    //  BEGIN THE TAB BAR...
    if ( ImGui::BeginTabBar(m_tabbar_uuids[1], m_tabbar_flags[1]) )
    {
        //      2.3     DRAW EACH OF THE TAB ITEMS...
        for (auto & tab : this->ms_CTRL_TABS)
        {
            //  PLACE TAB-BAR ITEM SPACING...
            ImGui::PushID("##CtrlTabbarSpacing");
            ImGui::TabItemSpacing("##CtrlTabbarSpacing", SPACING_FLAGS, ms_SPACING);
            ImGui::PopID();
        
            //  PLACING EACH TAB...
            if ( ImGui::BeginTabItem( tab.get_uuid(), (tab.no_close) ? nullptr : &tab.open, tab.flags ) )
            {
                if (tab.render_fn) {
                    tab.render_fn( tab.get_uuid(), &tab.open, tab.flags );
                }
                else {
                    this->DefaultCtrlTabRenderFunc(tab.get_uuid(), &tab.open, tab.flags);
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
//  ?.      UTILITY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "get_tab"
//
utl::Tab_t * CCounterApp::get_ctrl_tab(const std::string & uuid, std::vector<Tab_t> & tabs) {
    bool            match       = false;
    const size_t    N           = tabs.size();
    size_t          idx         = N + 1;       //   Default:    if (NO MATCH):  "N < idx"
    
    //  1.  FIND THE INDEX AT WHICH THE TAB WITH NAME "uuid" IS FOUND...
    for (size_t i = 0; i < N && !match; ++i) {
        match = ( uuid == tabs[i].uuid );
        if (match) idx = i;
    }
    
    if (!match)
        return nullptr;
    
    return std::addressof( tabs[idx] );
}


//  "RebuildDockspace"
//
void CCounterApp::RebuildDockspace(void)
{
    //ImGui::DockBuilderRemoveNode    (this->S.m_dockspace_id);
    //m_dockspace_id          = ImGui::GetID(m_dockspace_name);
    
    //  ImGui::DockSpace(m_dockspace_id,    ImVec2(0.0f, 0.0f),     m_dockspace_flags);
    //  ImGui::DockBuilderRemoveNode    (m_dockspace_id); // clear any previous layout
    //  ImGui::DockBuilderAddNode       (m_dockspace_id, ImGuiDockNodeFlags_DockSpace);
    
    ImGui::DockBuilderSetNodeSize   (m_dockspace_id, ImVec2(800, 600));

        // Example split: left and right
        m_dock_ids[1]   = ImGui::DockBuilderSplitNode( m_dockspace_id, ImGuiDir_Down, m_dockspace_ratio,
                                                       nullptr, &m_dock_ids[0] );

        ImGui::DockBuilderDockWindow(m_win_uuids[0],    m_dock_ids[0]);
        ImGui::DockBuilderDockWindow(m_win_uuids[1],    m_dock_ids[1]);
        ImGui::DockBuilderFinish(m_dockspace_id);

    return;
}











// *************************************************************************** //
//
//
//  3.      PRIVATE MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//--------------------------------------------------------------------
// Helper to compute localized average
//--------------------------------------------------------------------
float CCounterApp::ComputeAverage(const buffer_type &buf, AvgMode mode,
                                  ImU64 N_samples, double seconds, float now) const
{
    const std::size_t sz = buf.size();
    if (sz == 0) return 0.f;

    float sum = 0.f;
    std::size_t cnt = 0;

    if (mode == AvgMode::Samples) {
        const ImU64 N = std::min<ImU64>(N_samples, sz);
        const std::size_t start = sz - static_cast<std::size_t>(N);
        for (std::size_t i = start; i < sz; ++i) {
            sum += buf[i].y;
            ++cnt;
        }
    } else { // AvgMode::Seconds
        for (std::size_t i = sz; i-- > 0; ) {
            const float dt = now - buf[i].x;
            if (dt > seconds) break;
            sum += buf[i].y;
            ++cnt;
        }
    }
    return cnt ? sum / static_cast<float>(cnt) : 0.f;
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

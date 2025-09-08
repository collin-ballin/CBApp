/***********************************************************************************
*
*       ********************************************************************
*       ****          G R A P H _ A P P . C P P  ____  F I L E          ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond.
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
                AddNewPlotTab();
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
void GraphApp::display_controls(void)
{
    //  BEGIN THE TAB BAR...
    if ( ImGui::BeginTabBar(m_tabbar_uuids[1], m_tabbar_flags[1]) )
    {
        //      2.3     DRAW EACH OF THE TAB ITEMS...
        for (auto & tab : this->ms_CTRL_TABS)
        {
        
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

//  "AddNewPlotTab"
//
void GraphApp::AddNewPlotTab(void)
{
    constexpr size_t            N           = 3;
    const static std::string    OPTIONS[]   = {
        "1D Plot", "Heatmap", "Other"
    };
    Tab_t               new_tab     = { "",     true,   false,      0U,     nullptr };
    
    bool                selected    = false;
    short               plot_type   = -1;
    const size_t        N_tabs      = this->ms_PLOT_TABS.size() + 1;
    std::string         uuid        = "Tab #" + std::to_string(N);
    
    
    ImGui::SeparatorText("Add New Plot...");
    ImGui::Separator();
    
    
    
    //  2.  DEFINE PLOT TYPE SELECTIONS...
    for (size_t i = 0; i < N; ++i)
    {
        if ( ImGui::Selectable( OPTIONS[i].c_str() ) ) {
            selected        = true;
            plot_type       = static_cast<short>(i);
            new_tab.uuid    = OPTIONS[i] + " #" + std::to_string(N_tabs);
        }
    }
         

    //  3.  ADD THE NEW PLOT TYPE...
    if (selected)
    {
        switch (plot_type) {
            case 0 : {          //  CASE "0" :      ???
                //
                break;
            }
            case 1 : {          //  CASE "1" :      ???
                //
                break;
            }
            //
            //
            default : {         //  DEFAULT :       GENERIC TAB.
                //
                break;
            }
        }
        this->ms_PLOT_TABS.push_back(new_tab);
    }
        
        
        
    return;
}


//  "AddNewCtrlTab"
//
void GraphApp::AddNewCtrlTab(void)      { return; }


//  "DefaultPlotTabRenderFunc"
//
void GraphApp::DefaultPlotTabRenderFunc([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags) {
    if (!p_open)
        return;
        
    ImGui::Text("Window Tab \"%s\".  Here is some default text dispatched by \"DefaultPlotTabRenderFunc()\".", uuid);
    return;
}



//  "DefaultCtrlTabRenderFunc"
//
void GraphApp::DefaultCtrlTabRenderFunc([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags) {
    if (!p_open)
        return;
        
    ImGui::BulletText("Tab UUID: \"%s\".", uuid);
        
    ImGui::Text("Window Tab \"%s\".  Here is some default text dispatched by \"DefaultCtrlTabRenderFunc()\".", uuid);
    return;
}



//  "get_tab"
//
utl::Tab_t * GraphApp::get_ctrl_tab(const std::string & uuid, std::vector<Tab_t> & tabs) {
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



// *************************************************************************** //
//
//
//  ?.      NEW SKETCH / EDITOR STUFF...
// *************************************************************************** //
// *************************************************************************** //















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

/***********************************************************************************
*
*       ********************************************************************
*       ****               I N I T . C P P  ____  F I L E               ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond
*               DATED:      May 19, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/graph_app/graph_app.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//  1.      INITIALIZATION  | DEFAULT CONSTRUCTOR, DESTRUCTOR, ETC...
// *************************************************************************** //
// *************************************************************************** //

//  Parametric Constructor 1.
//
GraphApp::GraphApp(app::AppState & src)
    : S(src)
{
    this->m_heatmap     = cb::HeatMap(256, 256);
}


//  Destructor.
//
GraphApp::~GraphApp(void)     { this->destroy(); }


//  "destroy"
//
void GraphApp::destroy(void)
{
    return;
}






// *************************************************************************** //
//
//
//  1B.     SECONDARY INITIALIZATION...
// *************************************************************************** //
// *************************************************************************** //

//  "initialize"
//
void GraphApp::initialize(void)
{
    if (this->m_initialized)
        return;
        
    this->init();
    return;
}


//  "init"
//
void GraphApp::init(void)
{
    ms_I_PLOT_COL_WIDTH                            *= S.m_dpi_scale;
    
    
    //  1.  ASSIGN THE CHILD-WINDOW CLASS PROPERTIES...
    m_window_class[0].DockNodeFlagsOverrideSet      = 0;    //  ImGuiDockNodeFlags_HiddenTabBar; //ImGuiDockNodeFlags_NoTabBar;
    m_window_class[1].DockNodeFlagsOverrideSet      = ImGuiDockNodeFlags_NoTabBar; //ImGuiDockNodeFlags_HiddenTabBar; //ImGuiDockNodeFlags_NoTabBar;
    
    
    //  2.  TABS FOR PLOT WINDOW...
    ms_PLOT_TABS                                    = {
    //          TAB NAME.               OPEN.           CLOSE-ABLE.         FLAGS.                          CALLBACK.
        Tab_t(  "Etch-A-Sketch",        true,           true,               ImGuiTabItemFlags_None,         nullptr),
        Tab_t(  "Tab 2",                true,           true,               ImGuiTabItemFlags_None,         nullptr)
    };
    

    //  3.  TABS FOR CONTROL WINDOW...
    ms_CTRL_TABS                                    = {
    //          TAB NAME.               OPEN.           CLOSE-ABLE.         FLAGS.                          CALLBACK.
        Tab_t(  "Tab 1",                true,           true,               ImGuiTabItemFlags_None,         nullptr),
        Tab_t(  "Tab 2",                true,           true,               ImGuiTabItemFlags_None,         nullptr)
    };
    
    
    //  4.  ASSIGN THE CALLBACK RENDER FUNCTIONS FOR EACH TAB...
    for (std::size_t i = 0; i < ms_PLOT_TABS.size(); ++i)
    {
        auto &      tab                     = ms_PLOT_TABS[i];
        ms_PLOT_TABS[i].render_fn           = [this, tab]([[maybe_unused]] const char * id, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
                                              { this->dispatch_plot_function( tab.uuid ); };
    }
    
    
    //  END INITIALIZATION...
    this->m_initialized                             = true;
    return;
}


//  "dispatch_plot_function"
//
void GraphApp::dispatch_plot_function(const std::string & uuid)
{
    bool            match       = false;
    const size_t    N           = ms_PLOT_TABS.size();
    size_t          idx         = N + 1;       //   Default:    if (NO MATCH):  "N < idx"
    
    
/*
    //  1.  FIND THE INDEX AT WHICH THE TAB WITH NAME "uuid" IS FOUND...
    for (size_t i = 0; i < N && !match; ++i) {
        match = ( uuid == ms_PLOT_TABS[i].uuid );
        if (match) idx = i;
    }
    
    
    if (!match) return;
    
    
    auto &          T       = ms_PLOT_TABS[idx];
    
    
    //  DISPATCH EACH RENDER FUNCTION FOR EACH WINDOW OF THE APPLICATION...
    switch (idx)
    {
        //      1.  ...
        case 0:         {
            this->ShowDockspace(         T.uuid.c_str(),     nullptr,        w.flags);
            break;
        }
        //
        //
        //
        default: {
            break;
        }
    }
    
*/
    return;
}




// *************************************************************************** //
//
//
//  ?.      UTILITY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "RebuildDockspace"
//
void GraphApp::RebuildDockspace(void)
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



//  "DefaultTabRenderFunc"
//
void GraphApp::DefaultTabRenderFunc([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags) {
    if (!p_open)
        return;
        
    ImGui::Text("Here is some text on a window with no render function.");
    return;
}



//  "get_tab"
//
Tab_t * GraphApp::get_ctrl_tab(const std::string & uuid, std::vector<Tab_t> & tabs) {
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
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.















// *************************************************************************** //
// *************************************************************************** //
//
//  END.

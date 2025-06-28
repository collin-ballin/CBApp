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
        //ImGui::SetNextWindowClass(&this->m_window_class[1]);
        ImGui::Begin( m_detview_window.uuid.c_str(), nullptr, m_detview_window.flags );
            this->display_controls();
        ImGui::End();
    }

    
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
    const ImGuiWindowFlags          CHILD_FLAGS     = ImGuiWindowFlags_AlwaysVerticalScrollbar;
    
    
    this->_draw_control_bar();
    
    
    //  BEGIN THE TAB BAR...
    if ( ImGui::BeginTabBar(m_tabbar_uuids[1], m_tabbar_flags[1]) )
    {
        //      2.3     DRAW EACH OF THE TAB ITEMS...
        for (auto & tab : this->ms_CTRL_TABS)
        {
        
            //  PLACING EACH TAB...
            if ( ImGui::BeginTabItem( tab.get_uuid(), (tab.no_close) ? nullptr : &tab.open, tab.flags ) )
            {
                ImGui::BeginChild("##Control_TabBar_ChildWindow", ImVec2(0,0), /*border=*/false, CHILD_FLAGS);
            
                    if (tab.render_fn) {
                        tab.render_fn( tab.get_uuid(), &tab.open, tab.flags );
                    }
                    else {
                        this->DefaultCtrlTabRenderFunc(tab.get_uuid(), &tab.open, tab.flags);
                    }
                
                
            ImGui::EndChild();          // <- scrollable
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
//  ?.      NEW...
// *************************************************************************** //
// *************************************************************************** //

void CCounterApp::_draw_control_bar(void)
{
    static constexpr const char *   uuid            = "##Editor_Controls_Columns";
    static constexpr int            NC              = 8;
    static ImGuiOldColumnFlags      COLUMN_FLAGS    = ImGuiOldColumnFlags_None;
    static ImVec2                   WIDGET_SIZE     = ImVec2( -1,  32 );
    static ImVec2                   BUTTON_SIZE     = ImVec2( 32,   WIDGET_SIZE.y );
    //
    constexpr ImGuiButtonFlags      BUTTON_FLAGS    = ImGuiOldColumnFlags_NoPreserveWidths;
    //int                             mode_i          = static_cast<int>(m_mode);
    
    
    const auto                      Label           = [this](const char * text)
    { S.PushFont(Font::Small); ImGui::TextUnformatted(text); S.PopFont(); };

   
   
    //  BEGIN COLUMNS...
    //
    ImGui::Columns(NC, uuid, COLUMN_FLAGS);
    //
    //
    //
        //  1.  BEGIN PYTHON SCRIPT...
        if (!m_process_running)
        {
            if ( ImGui::Button("Start Process") ) {
                m_process_running         = m_python.start();
                if ( !m_process_running )       { ImGui::OpenPopup("launch_error"); }
                else                            { m_max_counts[0] = 0.f; } // reset stats
            }
        }
        //  CASE 2 :    SCRIPT  **IS**  RUNNING...
        else
        {
            ImGui::PushStyleColor(ImGuiCol_Button,          ImVec4(0.800f, 0.216f, 0.180f, 1.00f) );
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   app::DEF_APPLE_RED );
            if ( ImGui::Button("Stop Process") ) {
                m_python.stop();
                m_process_running = false;
            }
            ImGui::PopStyleColor(2);
        }
        
        
        
        
        //  2.  Plot Crawling.
        ImGui::NextColumn();    Label("Plot Crawling:");
        ImGui::Checkbox("##CCounterControls_PlotCrawling",              &m_smooth_scroll);
    
    
    
        
        //  3.  Mutex Counts.
        ImGui::NextColumn();    Label("Use Mutex Counts:");
        ImGui::SetNextItemWidth( BUTTON_SIZE.x );
        ImGui::Checkbox("##CCounterControls_UseMutexCounts",           &m_use_mutex_count);
            
                
        
        
        //  4.  Reset Averages.
        ImGui::NextColumn();
        if ( ImGui::Button("Reset Averages") ) {
            for (auto & vec : m_avg_counts)
                vec.clear(); //b.Erase();
        }
        
        
        
        //  5.  Clear Plot.
        ImGui::NextColumn();
        if ( ImGui::Button("Clear Plot") ) {
            for (auto & b : m_buffers) {
                b.clear(); //b.Erase();
            }
            std::fill(std::begin(m_max_counts), std::end(m_max_counts), 0.f);
        }
    
    

    //
    //
    //if ( ImGui::Button("Reset Averages") ) {
    //    for (auto & vec : m_avg_counts)
    //        vec.clear(); //b.Erase();
    //}
    //
    //ImGui::SameLine();
    //
    //if ( ImGui::Button("Reset Max") )
    //    std::fill(std::begin(m_max_counts), std::end(m_max_counts), 0.f);
    //
    //ImGui::SameLine();
    //ImGui::Checkbox("Plot Crawling", &m_smooth_scroll);
                

    
    
    
    
    
    
    
    
    
    
    
    
    
    
/*
        //  2.  GRID VISIBILITY...
        ImGui::NextColumn();
        ImGui::Text("Show Grid:");
        //
        ImGui::SetNextItemWidth( BUTTON_SIZE.x );
        ImGui::Checkbox("##Editor_Controls_ShowGrid",           &m_grid.visible);



        //  3.  SNAP-TO-GRID...
        ImGui::NextColumn();
        ImGui::Text("Snap-To-Grid:");
        //
        ImGui::Checkbox("##Editor_Controls_SnapToGrid",         &m_grid.snap_on);
        
        
        
        //  4.  GRID-LINE DENSITY...
        ImGui::NextColumn();
        ImGui::Text("Grid Density:");
        //
        //
        //
        if ( ImGui::ArrowButtonEx("##Editor_Controls_GridDensityDown",      ImGuiDir_Down,
                          BUTTON_SIZE,                                      BUTTON_FLAGS) )
        {
            m_grid.snap_step *= 2.f;
        }
        //
        ImGui::SameLine(0.0f, 0.0f);
        //
        if ( ImGui::ArrowButtonEx("##Editor_Controls_GridDensityUp",        ImGuiDir_Up,
                          BUTTON_SIZE,                                      BUTTON_FLAGS) )
        {
            m_grid.snap_step = std::max(ms_GRID_STEP_MIN, m_grid.snap_step * 0.5f);
        }
        //
        ImGui::SameLine();
        //
        ImGui::Text("(%.1f)", m_grid.snap_step);



        //  5.  CANVAS SETTINGS...
        ImGui::NextColumn();
        //ImGui::Text("##Editor_Controls_CanvasSettings");
        ImGui::Text("");
        //
        //
        if ( ImGui::Button("Canvas Settings", WIDGET_SIZE) ) {
            ImGui::OpenPopup("Editor_CanvasSettingsPopup");
        }
        if ( ImGui::BeginPopup("Editor_CanvasSettingsPopup") ) {
            this->_display_canvas_settings();
            ImGui::EndPopup();
        }


        
        //  6.  EMPTY SPACES FOR LATER...
        for (int i = ImGui::GetColumnIndex(); i < NC - 1; ++i) {
            ImGui::Dummy( ImVec2(0,0) );    ImGui::NextColumn();
        }



        //  X.  CLEAR ALL...
        //ImGui::NextColumn();
        //ImGui::TextUnformatted("##Editor_Controls_ClearCanvas");
        ImGui::Text("");
        if ( ImGui::Button("Clear", WIDGET_SIZE) ) {
            _clear_all();
        }
    */
    //
    //
    //
    ImGui::Columns(1);      //  END COLUMNS...
    
    
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

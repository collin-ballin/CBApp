//
//  graphing_app.cpp
//  CBApp
//
//  Created by Collin Bond on 4/29/25.
//
// *************************************************************************** //
// *************************************************************************** //
#include "app/_graphing_app.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//  0.      STATIC VARIABLES...
// *************************************************************************** //
// *************************************************************************** //

//  STATIC MEMBERS...
using dim_type      = GraphingApp::dim_type;
using value_type    = GraphingApp::value_type;

static constexpr dim_type                               ms_control_width    = dim_type(225);
static constexpr dim_type                               ms_scale_width      = dim_type(90);
static constexpr std::pair<dim_type, dim_type>          ms_width_range      = { dim_type(500),          dim_type(1200) };
static constexpr std::pair<dim_type, dim_type>          ms_height_range     = { dim_type(500),          dim_type(1200) };
static constexpr std::pair<value_type, value_type>      ms_vlimits          = { value_type(-10),        value_type(10) };






// *************************************************************************** //
//
//
//  1.      INITIALIZATION  | DEFAULT CONSTRUCTOR, DESTRUCTOR, ETC...
// *************************************************************************** //
// *************************************************************************** //

//  Parametric Constructor 1.
//
GraphingApp::GraphingApp(const size_type rows_, const size_type cols_)
    : m_heatmap(rows_, cols_)
{
    //this->m_heatmap     = cb::HeatMap(rows_, cols_);
    //this->init();
}


//  "initialize"
//
void GraphingApp::initialize(void)
{
    if (this->m_initialized)
        return;
        
    this->init();
    return;
}


//  "init"
//
void GraphingApp::init(void)
{
    CONTROL_WIDTH               = ImGui::GetFontSize() * 8;
    
    this->m_initialized         = true;
    return;
}



//  Destructor.
//
GraphingApp::~GraphingApp(void)     { this->destroy(); }


//  "destroy"
//
void GraphingApp::destroy(void)
{
    return;
}





// *************************************************************************** //
//
//
//  1B.     PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "Begin"
//
void GraphingApp::Begin([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
{
    ImGuiIO &                       io                      = ImGui::GetIO(); (void)io;
    ImGuiStyle &                    style                   = ImGui::GetStyle();
    
    
    
    //  1.  CREATE THE WINDOW AND BEGIN APPENDING WIDGETS INTO IT...
    ImGui::Begin(uuid, p_open, flags);
    
    
    
    static float                    SPACING             = 0.0f;
    static float                    DEF_SPACING         = 1.5 * ImGui::GetTextLineHeightWithSpacing();
    static float                    TEXT_SPACING        = 8.0 * ImGui::GetTextLineHeightWithSpacing();
    //static float                    PLOT_SPACING        = 2.5 * PLOT_SIZE[1];
    
    
    //SPACING                     =   (this->m_show_perf_metrics)                             ? TEXT_SPACING : DEF_SPACING;
    //SPACING                    +=   (this->m_show_perf_plots && this->m_show_perf_metrics)  ? PLOT_SPACING : 0;
    
    
    
        this->display_plot_panel();
    
    
        //ImGui::Dummy( ImVec2(0.0f, ImGui::GetContentRegionAvail().y - SPACING) );
        
        
        this->display_control_panel();
    


    ImGui::End();
    return;
}



// *************************************************************************** //
//
//
//  2.      PROTECTED MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //



//  2A.     CHILD WINDOW FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "display_plot_panel"
//
void GraphingApp::display_plot_panel(void)
{
    static bool                 show_help_tab           = true;     //  Enable / Disable the "?" Tab Button.
    static bool                 enable_adding_tabs      = true;     //  Enable / Disable the "+" Add-Tab Button.
    
    
    
    //  CHILD WINDOW WIDGET VARIABLES...
    static int                  draw_lines              = 3;
    static ImVec2               win_dims_min, win_dims_max;
    static ImVec2               size_arg                = ImVec2(-FLT_MIN,      0.0f);
      
      
    win_dims_min    = ImVec2(0.0f,          ImGui::GetTextLineHeightWithSpacing() * MC_PLOT_HEIGHT_LIMIT.first);
    win_dims_max    = ImVec2(FLT_MAX,       ImGui::GetTextLineHeightWithSpacing() * MC_PLOT_HEIGHT_LIMIT.second);
    
    
    
    
      
    //  1.  DRAWING CHILD-WINDOW...
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding,            mc_plot_corner_radius);
    ImGui::SetNextWindowSizeConstraints(    win_dims_min,       win_dims_max );
    ImGui::BeginChild(mc_plot_child_id,     size_arg,           mc_plot_child_flags);
    
    
        //  2.  DRAWING THE TAB-BAR INSIDE THE CHILD WINDOW...
        if (ImGui::BeginTabBar(mc_plot_tab_bar_id, mc_plot_tabbar_flags))
        {
            //      2.3     DRAW THE MAIN TAB ITEMS.
            std::size_t     n   = 0ULL;
            for (auto & tab : MC_PLOT_TABS) {
            
                //  DRAW THE TAB...                               (n<2) ? nullptr : &std::get<1>(tab)   ===> Prevent first 2 tabs from being closed.
                if (ImGui::BeginTabItem(std::get<0>(tab).c_str(), (n<2) ? nullptr : &std::get<1>(tab), std::get<2>(tab)) ) {
                    switch (n)
                    {
                        case 0 : {      //      TAB 1 :     MAIN PLOT...
                            this->m_heatmap.draw_plots();
                            break;
                        }
                        
                        default : {     //      DEFAULT :   ALL REMAINING MC_PLOT_TABS...
                            ImGui::Text("This is the %zu tab.", n);
                            break;
                        }
                        
                    }// END "switch".
                    ImGui::EndTabItem();
                }
                    
            
                //  2.4     IF THE USER DELETED A TAB...
                if (!std::get<1>(tab))      MC_PLOT_TABS.erase(MC_PLOT_TABS.begin() + n);
                else                        ++n;
                    
            }// END "for (auto & tab : MC_PLOT_TABS).


            ImGui::EndTabBar();
        }// END "BeginTabBar".
    
    
    
    //  END "Child Window".
    ImGui::EndChild();
    ImGui::PopStyleVar();





    return;
}







//  "display_control_panel"
//
void GraphingApp::display_control_panel(void)
{
    //  TAB-BAR VARIABLES...
    static bool                 show_help_tab           = true;     //  Enable / Disable the "?" Tab Button.
    static bool                 enable_adding_tabs      = true;     //  Enable / Disable the "+" Add-Tab Button.
    
    
    
    //  CHILD WINDOW WIDGET VARIABLES...
    static std::pair<int,int>   MC_CTRL_HEIGHT_LIMIT   = {9, 15};  //  {y1, y2}    : "y1"  --- min height of child (no matter how many widgets are on it).     "y2"  --- max height the window will extend to WITHOUT adding scrollbar.
    static int                  draw_lines              = 3;
    static ImVec2               win_dims_min, win_dims_max;
    static ImVec2               size_arg                = ImVec2(-FLT_MIN,      0.0f);
      
      
    win_dims_min    = ImVec2(0.0f,          ImGui::GetTextLineHeightWithSpacing() * MC_CTRL_HEIGHT_LIMIT.first);
    win_dims_max    = ImVec2(FLT_MAX,       ImGui::GetTextLineHeightWithSpacing() * MC_CTRL_HEIGHT_LIMIT.second);
    
    
    
    
      
    //  1.  DRAWING CHILD-WINDOW...
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding,            mc_ctrl_corner_radius);
    ImGui::SetNextWindowSizeConstraints(    win_dims_min,       win_dims_max );
    ImGui::BeginChild("ControlChild",       size_arg,           mc_ctrl_child_flags);
    
    
    
        //  2.  DRAWING THE TAB-BAR INSIDE THE CHILD WINDOW...
        if (ImGui::BeginTabBar("ControlTabBar", mc_ctrl_tabbar_flags))
        {
            //      2.1     DRAW HELP-MENU TAB ITEM "?".
            if (show_help_tab)
            {
                if (ImGui::TabItemButton("?", ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_NoTooltip))
                    ImGui::OpenPopup("MyHelpMenu");
            }
            if (ImGui::BeginPopup("MyHelpMenu"))
            {
                ImGui::Selectable("Hello!");
                ImGui::EndPopup();
            }


            //      2.2     DRAW TRAILING "+" ADD-TAB BUTTON.
            if (enable_adding_tabs)
            {
                if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip))
                {
                    //const auto      tab_idx     = MC_CTRL_TABS.size() + 1;
                    //tab_t           new_tab     = { std::string("Tab #") + std::to_string(tab_idx), true, m_def_tab_flags };
                    MC_CTRL_TABS.push_back({std::string("Tab #") + std::to_string(MC_CTRL_TABS.size()+1), true, m_def_tab_flags}); // Add new tab
                }
            }


            //      2.3     DRAW THE MAIN TAB ITEMS.
            std::size_t     n   = 0ULL;
            for (auto & tab : MC_CTRL_TABS)
            {
                //  DRAW THE TAB...                               (n<2) ? nullptr : &std::get<1>(tab)   ===> Prevent first 2 tabs from being closed.
                if (ImGui::BeginTabItem(std::get<0>(tab).c_str(), (n<2) ? nullptr : &std::get<1>(tab), std::get<2>(tab)) )
                {
                    switch (n)
                    {
                        //      TAB 1 :     MAIN CONTROLS...
                        case 0:
                        {
                            this->m_heatmap.draw_controls();
                            break;
                        }
                        
                        //      DEFAULT :   ALL REMAINING TABS...
                        default:
                        {
                            ImGui::Text("This is the %zu tab.", n);
                            break;
                        }
                        
                    }// END "switch".
                    ImGui::EndTabItem();
                }
                    
            
                //  2.4     IF THE USER DELETED A TAB...
                if (!std::get<1>(tab))      MC_CTRL_TABS.erase(MC_CTRL_TABS.begin() + n);
                else                        ++n;
                    
            }// END "for (auto & tab : MC_CTRL_TABS).


            ImGui::EndTabBar();
        }// END "BeginTabBar".
    
    
    
    //  END "Child Window".
    ImGui::EndChild();
    ImGui::PopStyleVar();





    return;
}





// *************************************************************************** //
//
//
//  3.      PRIVATE MEMBER FUNCTIONS...
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

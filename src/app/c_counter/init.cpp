/***********************************************************************************
*
*       ********************************************************************
*       ****                  I N I T  ____  F I L E                   ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond
*               DATED:      May 24, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/c_counter/c_counter.h"




namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //






// *************************************************************************** //
//
//
//  1.      INITIALIZATION  | DEFAULT CONSTRUCTOR, DESTRUCTOR, ETC...
// *************************************************************************** //
// *************************************************************************** //

CCounterApp::CCounterApp(app::AppState & src)
    : S(src)                        { }


//  "initialize"
//
void CCounterApp::initialize(void)
{
    if (this->m_initialized)
        return;
        
    this->init();
    return;
}


//  "init"
//
void CCounterApp::init(void)
{
    ms_I_PLOT_COL_WIDTH                            *= S.m_dpi_scale;
    ms_SPACING                                     *= S.m_dpi_scale;
    ms_COLLAPSE_BUTTON_SIZE.x                      *= S.m_dpi_scale;
    ms_COLLAPSE_BUTTON_SIZE.y                      *= S.m_dpi_scale;
    m_cmap                                          = ImPlot::GetColormapIndex("CCounter_Map");
    
    
    //  1.  ASSIGN THE CHILD-WINDOW CLASS PROPERTIES...
    m_window_class[0].DockNodeFlagsOverrideSet      = ImGuiDockNodeFlags_HiddenTabBar;    //  ImGuiDockNodeFlags_HiddenTabBar; //ImGuiDockNodeFlags_NoTabBar;
    m_window_class[1].DockNodeFlagsOverrideSet      = ImGuiDockNodeFlags_NoTabBar; //ImGuiDockNodeFlags_HiddenTabBar; //ImGuiDockNodeFlags_NoTabBar;
    
    
    std::snprintf(m_filepath, ms_BUFFER_SIZE, "%s", app::PYTHON_DUMMY_FPGA_FILEPATH);
    
        
    
    //  2.  TABS FOR PLOT WINDOW...
    ms_PLOT_TABS                                    = {
    //          TAB NAME.                   OPEN.           NOT CLOSE-ABLE.     FLAGS.                          CALLBACK.
        Tab_t(  "C-Counter Plots",          true,           true,               ImGuiTabItemFlags_None,         nullptr)
    };
    

    //  3.  TABS FOR CONTROL WINDOW...
    ms_CTRL_TABS                                    = {
    //          TAB NAME.                   OPEN.           NOT CLOSE-ABLE.     FLAGS.                          CALLBACK.
        Tab_t(  "C-Counter Controls",       true,           true,               ImGuiTabItemFlags_None,         nullptr)
    };
    
    
    //  4.  ASSIGN THE CALLBACK RENDER FUNCTIONS FOR EACH PLOT TAB...
    for (std::size_t i = 0; i < ms_PLOT_TABS.size(); ++i) {
        auto &      tab                     = ms_PLOT_TABS[i];
        ms_PLOT_TABS[i].render_fn           = [this, tab]([[maybe_unused]] const char * id, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
                                              { this->dispatch_plot_function( tab.uuid ); };
    }
    //  5.  ASSIGN CALLBACKS TO EACH CTRL TAB...
    for (std::size_t i = 0; i < ms_CTRL_TABS.size(); ++i) {
        auto &      tab                     = ms_CTRL_TABS[i];
        ms_CTRL_TABS[i].render_fn           = [this, tab]([[maybe_unused]] const char * id, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
                                              { this->dispatch_ctrl_function( tab.uuid ); };
    }
    
    
    //  6.  DEFINE FDTD STUFF...
    //m_plot_colors                           = cb::utl::GetColormapSamples( m_NUM_COLORS, m_cmap );
    this->init_ctrl_rows();
    
    
    
    //  END INITIALIZATION...
    this->m_initialized                         = true;
    return;
}



//  Destructor.
//
CCounterApp::~CCounterApp(void)     { this->destroy(); }


//  "destroy"
//
void CCounterApp::destroy(void)
{
    return;
}






// *************************************************************************** //
//
//
//  2.  TAB FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "dispatch_plot_function"
//
void CCounterApp::dispatch_plot_function(const std::string & uuid)
{
    bool            match       = false;
    const size_t    N           = ms_PLOT_TABS.size();
    size_t          idx         = N + 1;       //   Default:    if (NO MATCH):  "N < idx"

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
        //      0.  Model PARAMETERS...
        case 0:         {
            this->ShowCCPlots();
            break;
        }
        //
        //
        //
        default: {
            break;
        }
    }
    return;
}


//  "dispatch_ctrl_function"
//
void CCounterApp::dispatch_ctrl_function(const std::string & uuid)
{
    bool            match       = false;
    const size_t    N           = ms_CTRL_TABS.size();
    size_t          idx         = N + 1;       //   Default:    if (NO MATCH):  "N < idx"

    //  1.  FIND THE INDEX AT WHICH THE TAB WITH NAME "uuid" IS FOUND...
    for (size_t i = 0; i < N && !match; ++i) {
        match = ( uuid == ms_CTRL_TABS[i].uuid );
        if (match) idx = i;
    }
    if (!match) return;
    
    
    auto &          T       = ms_PLOT_TABS[idx];
    
    
    //  DISPATCH EACH RENDER FUNCTION FOR EACH WINDOW OF THE APPLICATION...
    switch (idx)
    {
        //      0.  Model PARAMETERS...
        case 0:         {
            this->ShowCCControls();
            break;
        }
        //
        //
        //
        default: {
            break;
        }
    }
    return;
}


//  "DefaultPlotTabRenderFunc"
//
void CCounterApp::DefaultPlotTabRenderFunc([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags) {
    if (!p_open)
        return;
        
    ImGui::Text("Window Tab \"%s\".  Here is some default text dispatched by \"DefaultPlotTabRenderFunc()\".", uuid);
    return;
}


//  "DefaultCtrlTabRenderFunc"
//
void CCounterApp::DefaultCtrlTabRenderFunc([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags) {
    if (!p_open)
        return;
        
    ImGui::BulletText("Tab UUID: \"%s\".", uuid);
        
    ImGui::Text("Window Tab \"%s\".  Here is some default text dispatched by \"DefaultCtrlTabRenderFunc()\".", uuid);
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

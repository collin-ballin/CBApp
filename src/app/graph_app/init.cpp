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
    //  1.      ASSIGN THE CHILD-WINDOW CLASS PROPERTIES...
    m_window_class[0].DockNodeFlagsOverrideSet                  = ImGuiDockNodeFlags_None;// ImGuiDockNodeFlags_HiddenTabBar;      //  ImGuiDockNodeFlags_HiddenTabBar; //ImGuiDockNodeFlags_NoTabBar;
    m_window_class[1].DockNodeFlagsOverrideSet                  = ImGuiDockNodeFlags_None;// ImGuiDockNodeFlags_HiddenTabBar;      //    ImGuiDockNodeFlags_NoTabBar; //ImGuiDockNodeFlags_HiddenTabBar; //ImGuiDockNodeFlags_NoTabBar;
    
    //  this->m_detview_window.render_fn    = std::bind_front(&GraphApp::display_controls, this);
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
//
//  1A.     PUBLIC API...
// *************************************************************************** //
// *************************************************************************** //

//  "save"
//
void GraphApp::save(void) {
    CB_LOG( LogLevel::Info, "GraphApp--save" );
    return;
}


//  "undo"
//
void GraphApp::undo(void) {
    S.m_logger.info("GraphApp--undo");
    return;
}


//  "redo"
//
void GraphApp::redo(void) {
    S.m_logger.info("GraphApp--redo");
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
    //ms_I_PLOT_COL_WIDTH                                        *= S.m_dpi_scale;
    //ms_SPACING                                                 *= S.m_dpi_scale;
        
    //  2.      DEFAULT TAB OPTIONS...
    static ImGuiTabItemFlags        ms_DEF_PLOT_TAB_FLAGS       = ImGuiTabItemFlags_None;
    static ImGuiTabItemFlags        ms_DEF_CTRL_TAB_FLAGS       = ImGuiTabItemFlags_None;
    
    
    //  3A.     TABS FOR PLOT WINDOW...
    ms_PLOT_TABS                                                = {
    //          TAB NAME.                   OPEN.           NOT CLOSE-ABLE.     FLAGS.                          CALLBACK.
        Tab_t(  "FDTD Editor",              true,           true,               ms_DEF_PLOT_TAB_FLAGS,          nullptr),
        Tab_t(  "Simulation Playback",      true,           true,               ms_DEF_PLOT_TAB_FLAGS,          nullptr),
        //  Tab_t(  "Etch-A-Sketch",            true,           true,               ImGuiTabItemFlags_None,         nullptr),
        //  Tab_t(  "Tab 2",                    true,           false,              ImGuiTabItemFlags_None,         nullptr)
    };
    

    
    //  3B.     TABS FOR CONTROL WINDOW...
    ms_CTRL_TABS                                                = {
    //          TAB NAME.                   OPEN.           NOT CLOSE-ABLE.     FLAGS.                          CALLBACK.
        Tab_t(  "Model Parameters",         true,           true,               ms_DEF_CTRL_TAB_FLAGS,          nullptr),
        Tab_t(  "Playback Controls",        true,           true,               ms_DEF_CTRL_TAB_FLAGS,          nullptr),
        //  Tab_t(  "Sketch Controls",          true,           true,               ImGuiTabItemFlags_None,         nullptr),
        //  Tab_t(  "Tab 2",                    true,           false,              ImGuiTabItemFlags_None,         nullptr)
    };
    
    
    //  4A.     ASSIGN THE CALLBACK RENDER FUNCTIONS FOR EACH PLOT TAB...
    for (std::size_t i = 0; i < ms_PLOT_TABS.size(); ++i) {
        auto &      tab                     = ms_PLOT_TABS[i];
        ms_PLOT_TABS[i].render_fn           = [this, tab]([[maybe_unused]] const char * id, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
                                              { this->dispatch_plot_function( tab.uuid ); };
    }
    //  4B.     ASSIGN CALLBACKS TO EACH CTRL TAB...
    for (std::size_t i = 0; i < ms_CTRL_TABS.size(); ++i) {
        auto &      tab                     = ms_CTRL_TABS[i];
        ms_CTRL_TABS[i].render_fn           = [this, tab]([[maybe_unused]] const char * id, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
                                              { this->dispatch_ctrl_function( tab.uuid ); };
    }
    
    
    
    //  6.  DEFINE FDTD STUFF...
    this->init_ctrl_rows();
    
    
    
    //  END INITIALIZATION...
    this->m_initialized                     = true;
    return;
}


//  "dispatch_plot_function"
//
void GraphApp::dispatch_plot_function(const std::string & uuid)
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
    
    
    //  DISPATCH EACH RENDER FUNCTION FOR EACH WINDOW OF THE APPLICATION...
    switch (idx)
    {
        //      0.  Model PARAMETERS...
        case 0:         {
            this->ShowEditor();
            break;
        }
        //      1.  1D FDTD...
        case 1:         {
            this->ShowPlayback();
            break;
        }
        //
        //      2.  ETCH-A-SKETCH...
        //      case 2:         {
        //          this->ShowSketch();
        //          //this->ShowSketch();
        //          break;
        //      }
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
void GraphApp::dispatch_ctrl_function(const std::string & uuid)
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
    
    
    
    
    //  DISPATCH EACH RENDER FUNCTION FOR EACH WINDOW OF THE APPLICATION...
    switch (idx)
    {
        //      0.  Model PARAMETERS...
        case 0:         {
            this->ShowModelParameters();
            break;
        }
        //      1.  FDTD...
        case 1:         {
            this->ShowPlaybackControls();
            break;
        }
        //
        //      2.  ETCH-A-SKETCH...
        //      case 2:         {
        //          this->ShowSketchControls();
        //          break;
        //      }
        //
        //
        //
        default: {
            break;
        }
    }
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

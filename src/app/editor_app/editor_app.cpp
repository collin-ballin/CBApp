/***********************************************************************************
*
*       ********************************************************************
*       ****         E D I T O R _ A P P . C P P  ____  F I L E         ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond.
*               DATED:      June 14, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/editor_app/editor_app.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //


//  1.      INITIALIZATION  | DEFAULT CONSTRUCTOR, DESTRUCTOR, ETC...
// *************************************************************************** //
// *************************************************************************** //

//  Parametric Constructor 1.
//
EditorApp::EditorApp(app::AppState & src)
    : S(src), m_editor(src)
{
    //  1.      ASSIGN THE CHILD-WINDOW CLASS PROPERTIES...
    m_window_class.DockNodeFlagsOverrideSet                  = ImGuiDockNodeFlags_None;      //    ImGuiDockNodeFlags_NoTabBar; //ImGuiDockNodeFlags_HiddenTabBar; //ImGuiDockNodeFlags_NoTabBar;
}


//  Destructor.
//
EditorApp::~EditorApp(void)     { this->destroy(); }


//  "destroy"
//
void EditorApp::destroy(void)
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

//  "GetMenuState"
[[nodiscard]] app::MenuState_t & EditorApp::GetMenuState(void) const noexcept       { return m_editor.GetMenuState(); }

//  "GetMenuState"
[[nodiscard]] app::MenuState_t & EditorApp::GetMenuState(void) noexcept             { return m_editor.GetMenuState(); }






//  "save"
//
void EditorApp::save(void)
{
    m_editor.save();
    return;
}


//  "open"
//
void EditorApp::open(void)
{
    m_editor.open();
    return;
}


//  "undo"
//
void EditorApp::undo(void)
{
    m_editor.undo();
    return;
}


//  "redo"
//
void EditorApp::redo(void)
{
    m_editor.redo();
    return;
}






// *************************************************************************** //
//
//
//
//  1B.     SECONDARY INITIALIZATION...
// *************************************************************************** //
// *************************************************************************** //

//  "initialize"
//
void EditorApp::initialize(void)
{
    if (this->m_initialized)
        return;
        
    this->init();
    return;
}


//  "init"
//
void EditorApp::init(void)
{

    //
    //  ...
    //


    //  END INITIALIZATION...
    this->m_initialized                     = true;
    return;
}






// *************************************************************************** //
//
//
//
//  1B.     PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "Begin"
//
void EditorApp::Begin([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
{
    //  1.  CREATING THE HOST WINDOW...
    ImGui::Begin(uuid, p_open, flags);
        this->display_plots();
    ImGui::End();
    
    
    
    //  2.  CREATE TOP WINDOW FOR PLOTS...
    if (m_detview_window.open) {
        ImGui::SetNextWindowClass(&this->m_window_class);
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
void EditorApp::display_plots(void)
{
    this->m_editor.Begin();
    return;
}


//  "display_controls"
//
void EditorApp::display_controls(void)
{
    m_editor.DrawBrowser();
    return;
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

/***********************************************************************************
*
*       ********************************************************************
*       ****            T O O L B A R . C P P  ____  F I L E            ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond
*               DATED:      June 05, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/app.h"
#include "app/toolbar/_toolbar.h"
#include <random>
#include <algorithm>



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//  1.      INITIALIZATION  | DEFAULT CONSTRUCTOR, DESTRUCTOR, ETC...
// *************************************************************************** //
// *************************************************************************** //

//  Default Constructor.
//
ToolBar::ToolBar(app::AppState & src)
    : S(src)                        { }


//  "initialize"
//
void ToolBar::initialize(void)
{
    if (this->m_initialized)
        return;
        
    this->init();
    return;
}


//  "init"          | protected
//
void ToolBar::init(void) {
    this->ms_PLOT_SIZE.y                           *= S.m_dpi_scale;
    
    this->m_window_class.DockNodeFlagsOverrideSet   = ImGuiDockNodeFlags_NoTabBar;
    return;
}


//  Destructor.
//
ToolBar::~ToolBar(void)             { this->destroy(); }


//  "destroy"       | protected
//
void ToolBar::destroy(void)         { }




// *************************************************************************** //
//
//
//  1B.     PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "Begin"
//
void ToolBar::Begin([[maybe_unused]] const char *       uuid,
                    [[maybe_unused]] bool *             p_open,
                    [[maybe_unused]] ImGuiWindowFlags   flags)
{
    [[maybe_unused]] ImGuiIO &      io              = ImGui::GetIO(); (void)io;
    [[maybe_unused]] ImGuiStyle &   style           = ImGui::GetStyle();
    
    
    //  1.  CREATE THE WINDOW AND BEGIN APPENDING WIDGETS INTO IT...
    ImGui::PushStyleColor(ImGuiCol_WindowBg, S.m_toolbar_bg);   // Push before ImGui::Begin()
    ImGui::SetNextWindowClass(&this->m_window_class);
    
    ImGui::Begin(uuid, p_open, flags);
        ImGui::PopStyleColor();
        

        //  3.  DETERMINE WINDOW COLLAPSE...
        if (this->S.m_show_toolbar_window)
        {
            
        }
        else
        {
            //  ...
        }
        
        
    
 
    ImGui::End();
    
    return;
}





// *************************************************************************** //
//
//
//  2.      PROTECTED MEMBER FUNCTIONS...
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

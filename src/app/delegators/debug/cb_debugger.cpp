/***********************************************************************************
*
*       ********************************************************************
*       ****        C B _ D E B U G G E R . C P P  ____  F I L E        ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond.
*               DATED:      August 28, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/app.h"
#include "app/delegators/_detail_view.h"
#include <random>
#include <algorithm>



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //






// *************************************************************************** //
//
//
//
//      1.      INITIALIZATION  | DEFAULT CONSTRUCTOR, DESTRUCTOR, ETC...
// *************************************************************************** //
// *************************************************************************** //

//  Default Constructor.
//
CBDebugger::CBDebugger(app::AppState & src)
    : S(src)                        {   }


//  "initialize"
//
void CBDebugger::initialize(void)
{
    if ( this->m_initialized )    { return; }
        
    this->init();
    return;
}


//  "init"          | protected
//
void CBDebugger::init(void) {
    this->m_window_class.DockNodeFlagsOverrideSet   = this-> m_docknode_override_flags;
    
    
    return;
}


//  Destructor.
//
CBDebugger::~CBDebugger(void)               { this->destroy(); }


//  "destroy"       | protected
//
void CBDebugger::destroy(void)              {   }



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 1A.  "INITIALIZATION" ]].












// *************************************************************************** //
//
//
//
//      1B.     PUBLIC API FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "Begin"
/*
void CBDebugger::Begin([[maybe_unused]] const char *        uuid,
                       [[maybe_unused]] bool *              p_open,
                       [[maybe_unused]] ImGuiWindowFlags    flags)
{
    [[maybe_unused]] ImGuiIO &      io              = ImGui::GetIO();
    [[maybe_unused]] ImGuiStyle &   style           = ImGui::GetStyle();

    
    
    //      2.      CREATE THE WINDOW AND BEGIN APPENDING WIDGETS INTO IT...
    //  this->m_window_class.DockNodeFlagsOverrideSet   = S.m_detview_window_flags;
    ImGui::SetNextWindowClass( &this->m_window_class );
    //
    ImGui::Begin(uuid, p_open, flags);
    //
    //
        this->Begin_IMPL();
    //
    //
    ImGui::End();
    

    
    return;
}
*/



//  "Begin"
//
void CBDebugger::Begin([[maybe_unused]] const char *        uuid,
                       [[maybe_unused]] bool *              p_open,
                       [[maybe_unused]] ImGuiWindowFlags    flags)
{
    [[maybe_unused]] ImGuiIO &      io              = ImGui::GetIO();
    [[maybe_unused]] ImGuiStyle &   style           = ImGui::GetStyle();

    
    //      2.      CREATE THE WINDOW AND BEGIN APPENDING WIDGETS INTO IT...
    ImGui::SetNextWindowClass( &this->m_window_class );
    ImGui::Begin(uuid, p_open, flags);
    //
    //
        this->Begin_IMPL();
    //
    //
    ImGui::End();
    

    
    return;
}










//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 1B.  "PUBLIC API" ]].












// *************************************************************************** //
//
//
//
//      2.      "MAIN UI FUNCTIONS"...
// *************************************************************************** //
// *************************************************************************** //

//  "Begin_IMPL"
//
inline void CBDebugger::Begin_IMPL(void)
{
    constexpr float     cv_combo_width      = 0.40f;
    constexpr float     cv_label_sep        = 50.0f;
    //
    static ImVec2       Avail               = ImVec2(-1.0f, -1.0f);
    static int          menu_idx            = static_cast<int>( this->m_menu_selection );



    //      1.      MENU - TYPE SELECTOR...
    //
    Avail       = ImGui::GetContentRegionAvail();
    
    ImGui::TextUnformatted  ( "Menu Type:"                  );
    ImGui::SameLine         ( 0.0f,     cv_label_sep        );
    ImGui::SetNextItemWidth ( cv_combo_width * Avail.x      );
    //
    if ( ImGui::Combo("##CBDebuggerr_MenuTypeSelection"     , &menu_idx     , ms_MENU_TYPE_NAMES.data()     , static_cast<int>( MenuType::COUNT )) )
    {
        this->m_menu_selection  = static_cast<MenuType>( menu_idx );
    }
    //
    ImGui::Separator();
    ImGui::NewLine();
    
    
    
    //      2.      DISPATCHING EACH FUNCTION...
    //
    switch ( this->m_menu_selection )
    {
        //
        //          2A.     "CB DEMO" Menu:
        case MenuType::CBDemo :
        {
            this->_MENU_CBDemo();
            break;
        }
        //
        //          2B.     "Unit Testing" Menu:
        case MenuType::UnitTesting :
        {
            this->_MENU_UnitTesting();
            break;
        }
        //
        //          2C.     "Generic" Menu:
        case MenuType::Generic :
        {
            this->_MENU_Generic();
            break;
        }
        //
        //
        //          2X.     DEFAULT...
        case MenuType::None :
        default :
        {
            break;
        }
    }
    
    
    
    
    
    //  this->TestOrchid();
    //
    //  this->TestndRingBuffer();

    return;
}


    

//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 2.  "MAIN UI FUNCTIONS" ]].












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

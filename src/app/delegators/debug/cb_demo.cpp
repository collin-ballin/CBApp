/***********************************************************************************
*
*       ********************************************************************
*       ****        C B _ D E M O . C P P  ____  F I L E        ****
*       ********************************************************************
*              AUTHOR:      Collin A. Bond.
*               DATED:      January 15, 2026.
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
//      0.      "INTERNAL NAMESPACE"...
// *************************************************************************** //
// *************************************************************************** //


// *************************************************************************** //
//      0A. INTERNAL.   |   SMALL_INTERNAL_NAMESPACE
// *************************************************************************** //
//
namespace anon { //     BEGINNING NAMESPACE "anon"...



// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}// END NAMESPACE "anon".






// *************************************************************************** //
//      0B. INTERNAL.   |   ORCHESTRATOR FUNCTIONS.
// *************************************************************************** //




// *************************************************************************** //
//      0C. INTERNAL.   |   SPECIFIC FUNCTIONS.
// *************************************************************************** //




    

//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 0.  "INTERNAL NAMESPACE" ]].












// *************************************************************************** //
//
//
//
//      1.      "MAIN UI FUNCTIONS"...
// *************************************************************************** //
// *************************************************************************** //

//  "_MENU_CBDemo"
//
void CBDebugger::_MENU_CBDemo(void) noexcept
{
    static bool     s_show_one              = false;
    static bool     s_show_two              = false;
    
    
    
    //      1.      INTERACTION WIDGETS...
    ImGui::Checkbox ( "Show One:"                   , &s_show_one           );
    ImGui::Checkbox ( "Show Two:"                   , &s_show_two           );
    //
    ImGui::NewLine();
    ImGui::NewLine();
    
    
    
    //      2.      DISPATCH EACH FUNCTION... DEBUG DISPLAYS...
    //
    if ( s_show_one )
    {
        _CBDemo_show_one();
    }
    if ( s_show_two )
    {
        _CBDemo_show_two();
    }
    
    
    

    return;
}


    

//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 1.  "MAIN UI FUNCTIONS" ]].












// *************************************************************************** //
//
//
//
//      2.      "CB_DEMO" FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_CBDemo_show_one"
//
inline void CBDebugger::_CBDemo_show_one(void) noexcept
{
    using                               IconAnchor                  = utl::icon_widgets::Anchor;
    using                               Padding                     = utl::icon_widgets::PaddingPolicy;
    //
    static constexpr const char *       s_uuid                      = "##CBDemo_S1_Columns";
    static constexpr int                cv_NC                       = 4;        //  # columns at BEGINNING.
    static constexpr int                cv_NE                       = 2;        //  # cols at END (*AFTER* the spacer/empty columns).
    //
    static ImGuiOldColumnFlags          s_COLUMN_FLAGS              = ImGuiOldColumnFlags_None;
    static ImVec2                       s_WIDGET_SIZE               = ImVec2( -1        ,  32               );
    static ImVec2                       s_BUTTON_SIZE               = ImVec2( 32        ,  s_WIDGET_SIZE.y  );
    //
    //
    //
    static float                        s_scale                     = 1.50f;
    //
    //
    static bool                         b1                          = false;
    static bool                         b2                          = false;



   
   
    //      BEGIN COLUMNS...
    //
    this->S.PushFont    ( Font::Small                               );
    ImGui::Columns      ( cv_NC     , s_uuid    , s_COLUMN_FLAGS    );
    //
    //
    //
        ImGui::PushItemWidth( s_BUTTON_SIZE.x );
    
    
        //          1.          FIRST GROUP:
        {
        //
        //
            //                  1A.     G1. First Button.
            b1          = utl::IconButton(
                /*  const char *            id          */   "##CBDemo_S1_G1B1"
                /*  ImVec4                  color       */ , this->S.SystemColor.Orange
                /*  char *                  icon_utf8   */ , "one"
                /*  float                   scale       */ , s_scale
                /*  ImVec2 &                size        */ //, ImVec2(32.0f, 32.0f)
                /*  icon_w::Anchor          anchor      */ , IconAnchor::TextBaseline    // TextBaseline    South   Center
                /*  icon_w::PaddingPolicy   pad         */ , Padding::Tight
                /*  ImVec2 &                nudge       */ , ImVec2(0.0f, 0.0f)
            );
        //
        //
        }
    
    
    
        //          2.          SECOND BUTTON:
        ImGui::NextColumn();
        {
        //
        //
            //                  2A.     G1. First Button.
            b2          = utl::IconButton(
                /*  const char *            id          */   "##CBDemo_S1_G2B1"
                /*  ImVec4                  color       */ , this->S.SystemColor.Orange
                /*  char *                  icon_utf8   */ , "avg"
                /*  float                   scale       */ , s_scale
                /*  ImVec2 &                size        */ //, ImVec2(32.0f, 32.0f)
                /*  icon_w::Anchor          anchor      */ , IconAnchor::TextBaseline    // TextBaseline    South   Center
                /*  icon_w::PaddingPolicy   pad         */ , Padding::Tight
                /*  ImVec2 &                nudge       */ , ImVec2(0.0f, 0.0f)
            );
        //
        //
        }
    
    
    
    



        //          X.0.        EMPTY SPACES FOR LATER...
        for (int i = ImGui::GetColumnIndex(); i < cv_NC - cv_NE; ++i) {
            ImGui::Dummy( ImVec2(0,0) );    ImGui::NextColumn();
        }



        ImGui::PopItemWidth();
    //
    //
    //
    ImGui::Columns(1);      //  END COLUMNS...
    this->S.PopFont();
    
    
    
    
    
    
    //      3.      RESPOND TO EACH BUTTON ACTIVATION...
    //
    if ( b1 )
    {
        //  this->_reset_average_values();
    }
    //
    if ( b2 )
    {
        //  this->_reset_average_values();
    }
            
            

    return;
}


//  "_CBDemo_show_two"
//
inline void CBDebugger::_CBDemo_show_two(void) noexcept
{

    return;
}


    

//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 2.  "CB_DEMO FUNCTIONS" ]].












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

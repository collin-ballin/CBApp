/***********************************************************************************
*
*       *********************************************************************
*       ****          S H O R T C U T S . C P P  ____  F I L E           ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond..
*               DATED:      September 2, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "widgets/editor/editor.h"



namespace cb {  //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //


// *************************************************************************** //
//
//
//
//      1.      MAIN "SHORTCUT" HANDLER...
// *************************************************************************** //
// *************************************************************************** //

//  "_MECH_query_shortcuts"
//
void Editor::_MECH_query_shortcuts([[maybe_unused]] const Interaction & it)
{
    this->_selection_no_selection_shortcuts     ( it );             //  0.  HOT-KEYS FOR NO-SELECTION STATUS...



    this->_selection_read_only_shortcuts        ( it );             //  1.  READ-ONLY HOTKEYS...
    
    
    //  CASE 1 :    If NO SELECTION -- There is no need for the additional hotkeys.
    if ( m_sel.empty() )                    { return; }
    

    if ( _mode_has(CBCapabilityFlags_EnableMutableHotkeys) )        //  2.  MUTABLE HOTKEYS...
        { this->_selection_mutable_shortcuts( it ); }
        
    if ( _mode_has(CBCapabilityFlags_EnableAdvancedHotkeys) )       //  3.  ADVANCED HOTKEYS...
        { this->_selection_advanced_shortcuts( it ); }
    
    
    return;
}

//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "MAIN SHORTCUT HANDLER".






// *************************************************************************** //
//
//
//
//      2.      SUBSIDIARY SHORTCUT HANDLERS...
// *************************************************************************** //
// *************************************************************************** //


//  "_selection_no_selection_shortcuts"
//
inline void Editor::_selection_no_selection_shortcuts([[maybe_unused]] const Interaction & it)
{
    ImGuiIO &           io      = ImGui::GetIO();
    EditorState &       ES      = this->m_editor_S;


    //      1.1.    GRID SHORTCUTS.
    this->_selection_grid_shortcuts(it);

    //      1.2.    HIDE ALL OVERLAYS.      [ SHIFT + H ].
    if ( io.KeyShift && ImGui::IsKeyPressed(ImGuiKey_H) )       { ES.m_block_overlays = !ES.m_block_overlays; return; }   //  [SHIFT + H]       HIDE ALL OVERLAYS...
    
    //      1.3.    PASTE.                  [ CTRL + V ].
    if ( io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_V) )        { /* this->copy_to_clipboard(); */      return;     }



    return;
}

//  "_grid_handle_shortcuts"
//
//      I think our policy should be to RETURN OUT after implementing a SINGLE hotkey.
//      In other words, only allow ONE hotkey activation per frame.  It seems problematic to allow
//      the user to perform both a CTRL+ AND CTRL- (inverse operations) in the same frame.
//
inline void Editor:: _selection_grid_shortcuts([[maybe_unused]] const Interaction & it) noexcept
{
    ImGuiIO &           io      = ImGui::GetIO();
    //  EditorState &       ES      = this->m_editor_S;
    GridState &         GS      = this->m_grid;
   
   
   
    //      1.      EARLY EXIT IF [ SHIFT ] IS NOT PRESSED...
    if ( io.KeyShift )
    {
        //              1.1.    TOGGLE SNAP-TO-GRID.            [ SHIFT G ]
        if ( io.KeyShift && ImGui::IsKeyPressed(ImGuiKey_G) )   { m_grid.snap_on = !m_grid.snap_on; return; }
           
        //              1.2.    DECREASE GRID SPACING.          [ SHIFT – ]
        if ( ImGui::IsKeyPressed(ImGuiKey_Minus) )              { GS.DecreaseGridSpacing();     return; }
        
        //              1.3.    INCREASE GRID SPACING.          [ SHIFT + ]
        if ( ImGui::IsKeyPressed(ImGuiKey_Equal) )              { GS.IncreaseGridSpacing();     return; }
    }
       
   
   
    //      2.      EARLY EXIT IF  [ CTRL ]  IS NOT PRESSED...
    if ( io.KeyCtrl )
    {
        //              2.1.    ZOOM---OUT CANVAS.              [ CTRL – ]
        if ( ImGui::IsKeyPressed(ImGuiKey_Minus) )              { /* GS.DecreaseWindowSize(); */    return; }
        
        //              2.2.    ZOOM---IN CANVAS.               [ CTRL + ]
        if ( ImGui::IsKeyPressed(ImGuiKey_Equal) )              { /* GS.IncreaseWindowSize(); */    return; }
        
        //              2.3.    RESET VIEW OF CANVAS.           [ CTRL KEYPAD-0 ]
        if ( ImGui::IsKeyPressed(ImGuiKey_Keypad0) )            { this->_utl_set_canvas_window();   return; }
    }
            
        
        
    return;
}



//  "_selection_read_only_shortcuts"
//
inline void Editor::_selection_read_only_shortcuts([[maybe_unused]] const Interaction & it)
{
    ImGuiIO &           io      = ImGui::GetIO();
    
    
    
    //      1.1.    COPY.                   [ CTRL + C ].
    if ( io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_C) )        { this->copy_to_clipboard();    return;     }
    
    
    //      CASE 2 :    THE HOTKEYS BELOW SHOULD BE  **BLOCKED**  BY  "it.BlockShortcuts()" ...
    if ( it.BlockShortcuts() )              { return; }
    
   
    //      2.1.    CLEAR SELECTION.        [ ESC ].
    if ( ImGui::IsKeyPressed(ImGuiKey_Escape) )                 { this->reset_selection();      return;     }   //  [ESC]       CANCEL SELECTION...
    
    
    
    return;
}


//  "_selection_mutable_shortcuts"
//
inline void Editor::_selection_mutable_shortcuts([[maybe_unused]] const Interaction & it)
{
    ImGuiIO &               io              = ImGui::GetIO();
    const EditorState &     ES              = this->m_editor_S;
    GridState &             GS              = this->m_grid;
    
    if ( m_sel.empty() || it.BlockShortcuts() )             { return; }     //  CASE 0 :    NO SELECTION  *OR*  HOTKEYS ARE BLOCKED...
    
    
    const bool              groupable       = ( this->m_sel.points.size() + this->m_sel.paths.size() > 1 );
    const bool              ctrl            = io.KeyCtrl;
    float                   step_x          = 0.0f;
    float                   step_y          = 0.0f;
    //
    {
        constexpr float         s_SHIFT_SCALER      = 2.0f;
        const bool              shift               = io.KeyShift;
        const bool              should_snap         = this->want_snap();
            
        //  CASE 1A :   If SNAP-TO-GRID is *ON* :   [ No-Shift ] = Grid-Spacing.  [ Shift ] = 2 x Grid-Spacing.
        if ( this->m_grid.snap_on ) {
            step_x          = ( shift )         ? s_SHIFT_SCALER * GS.m_grid_spacing[0]     : GS.m_grid_spacing[0];
            step_y          = ( shift )         ? s_SHIFT_SCALER * GS.m_grid_spacing[1]     : GS.m_grid_spacing[1];
        }
        //  CASE 1B :   If SNAP-TO-GRID is *ON* :   [ No-Shift ] = Grid-Spacing.  [ Shift ] = 10 x Grid-Spacing.
        else {
            step_x          = ( should_snap )   ? GS.m_grid_spacing[0]                      : 1.0f;
            step_y          = ( should_snap )   ? GS.m_grid_spacing[1]                      : 1.0f;
        }
    }
    
    
    



    //      1.      ARROW KEYS.
    if ( ImGui::IsKeyPressed(ImGuiKey_LeftArrow   , true) )       { this->move_selection     ( -step_x     , 0.0f      );    return;   }
    if ( ImGui::IsKeyPressed(ImGuiKey_RightArrow  , true) )       { this->move_selection     ( step_x      , 0.0f      );    return;   }
    if ( ImGui::IsKeyPressed(ImGuiKey_UpArrow     , true) )       { this->move_selection     ( 0.0f        , step_y    );    return;   }
    if ( ImGui::IsKeyPressed(ImGuiKey_DownArrow   , true) )       { this->move_selection     ( 0.0f        , -step_y   );    return;   }


    //      2.      DELETE KEY.
    if ( ImGui::IsKeyPressed(ImGuiKey_Delete)  ||  ImGui::IsKeyPressed(ImGuiKey_Backspace) )
        { delete_selection(); return; }                    // selection cleared, bail-out early


    //      3.      JOIN.                   [ CTRL + J ].
    if ( ctrl  &&  ImGui::IsKeyPressed(ImGuiKey_J) )            { _join_selected_open_path();   return;     }   //  JOINING CLOSED PATHS...


    //      X.      CREATE GROUP.           [ CTRL + G ].
    if ( ctrl  &&  ImGui::IsKeyPressed(ImGuiKey_G)  &&  groupable )
    {
        //
        //  TODO: group_selection();
        //
    }


    return;
}


//  "_selection_advanced_shortcuts"
//
inline void Editor::_selection_advanced_shortcuts([[maybe_unused]] const Interaction & it)
{
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

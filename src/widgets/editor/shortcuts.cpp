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



    //      1.1.    HIDE ALL OVERLAYS.      [ SHIFT + H ].
    if ( io.KeyShift && ImGui::IsKeyPressed(ImGuiKey_H) )       { ES.m_block_overlays = !ES.m_block_overlays; return; }   //  [SHIFT + H]       HIDE ALL OVERLAYS...
    
    //      1.2.    PASTE.                  [ CTRL + V ].
    if ( io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_V) )        { /* this->copy_to_clipboard(); */      return;     }



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
    ImGuiIO &       io      = ImGui::GetIO();
    const float     step    = m_grid.snap_step * ( (io.KeyShift) ? 10.0f : 1.0f );         //  Always one “grid unit”.
    
    
    if ( m_sel.empty() || it.BlockShortcuts() )             { return; }                     //  Nothing selected  OR  Not In-Focus  ===> nothing to do.



    //      1.      ARROW KEYS.
    if ( ImGui::IsKeyPressed(ImGuiKey_LeftArrow,  true) )    { this->move_selection(-step,  0.0f); }
    if ( ImGui::IsKeyPressed(ImGuiKey_RightArrow, true) )    { this->move_selection( step,  0.0f); }
    if ( ImGui::IsKeyPressed(ImGuiKey_UpArrow,    true) )    { this->move_selection( 0.0f,  step); }
    if ( ImGui::IsKeyPressed(ImGuiKey_DownArrow,  true) )    { this->move_selection( 0.0f, -step); }



    //      2.      DELETE KEY.
    if ( ImGui::IsKeyPressed(ImGuiKey_Delete) || ImGui::IsKeyPressed(ImGuiKey_Backspace) )
        { delete_selection(); return; }                    // selection cleared, bail


    //      3.      JOIN.                   [ CTRL + J ].
    if ( io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_J) )        { _join_selected_open_path();   return;     }   //  JOINING CLOSED PATHS...


    //      X.      CREATE GROUP.           [ CTRL + G ].
    if ( io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_G) &&
         ( m_sel.points.size() + m_sel.paths.size() ) > 1 )
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

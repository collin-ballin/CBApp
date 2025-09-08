/***********************************************************************************
*
*       *********************************************************************
*       ****              M E N U S . C P P  ____  F I L E               ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      August 20, 2025.
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
//      1.      CONTEXT MENU ORCHESTRATORS... 
// *************************************************************************** //
// *************************************************************************** //

//  "dispatch_selection_context_menus"
//
void Editor::dispatch_selection_context_menus([[maybe_unused]] const Interaction & it)
{
    const bool                  rmb_click           = it.hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right);


    //      1.      Handle the click (selection update + OpenPopup)
    if (rmb_click)
    {
        if ( m_sel.empty() )
        {
            if ( auto h = _hit_any(it) ) {      // pick under cursor
                this->reset_selection();        // m_sel.clear();
                add_hit_to_selection(*h);
            }
        }

        //  Decide which popup to open based on current selection state
        if ( m_sel.empty() )    { ImGui::OpenPopup( GetMenuID(PopupHandle::Canvas)      );      }       // empty → canvas menu
        else                    { ImGui::OpenPopup( GetMenuID(PopupHandle::Selection)   );      }       // non-empty → selection menu
    }

    //      2.      Render both popups every frame; BeginPopup() returns false if not open
    _show_selection_context_menu    (it,    GetMenuID(PopupHandle::Selection)       );
    _show_canvas_context_menu       (it,    GetMenuID(PopupHandle::Canvas)          );
    
    return;
}

//
//
// *************************************************************************** //
// *************************************************************************** //   END "ORCHESTRATORS".






// *************************************************************************** //
//
//
//      2.      CANVAS CONTEXT MENU...
// *************************************************************************** //
// *************************************************************************** //

//  "_show_canvas_context_menu"
//      When user RIGHT-CLICKS on an EMPTY POINT ON THE CANVAS (Allows them to PASTE at this location).
//
inline void Editor::_show_canvas_context_menu([[maybe_unused]] const Interaction & it, const char * popup_id)
{
    const bool              can_paste       = !m_clipboard.empty();
        
        
    //  Jump-out early if NO POPUP WINDOW...
    if ( !ImGui::BeginPopup(popup_id) ) return;
    
        //  0.  TEMPORARY LABEL...
        ImGui::TextDisabled("Canvas Context Menu");
        ImGui::Separator();
        
    
        //  1.  PASTE BUTTON...
        ImGui::BeginDisabled( !can_paste );
        //
            if ( ImGui::MenuItem("Paste", nullptr, false, can_paste) )
            { paste_from_clipboard( pixels_to_world(ImGui::GetIO().MousePos) ); }
        //
        ImGui::EndDisabled();
    
    
        //if ( ImGui::MenuItem("Paste", nullptr, false, can_paste) )
        //{ paste_from_clipboard(pixels_to_world(ImGui::GetIO().MousePos)); }



    ImGui::EndPopup();
    return;
}

//
//
// *************************************************************************** //
// *************************************************************************** //   END "CANVAS".







// *************************************************************************** //
//
//
//      3.      SELECTION CONTEXT MENU...
// *************************************************************************** //
// *************************************************************************** //

//  "_show_selection_context_menu"
//      Main manager for selection context menu---When user RIGHT-CLICKS on a selection.
//
inline void Editor::_show_selection_context_menu([[maybe_unused]] const Interaction & it, const char * popup_id)
{
    //const size_t    total_items     = m_sel.points.size() + m_sel.lines.size() + m_sel.paths.size();
    const size_t    total_items     = m_sel.paths.size();


    //  Jump-out early if NO POPUP WINDOW...
    if ( !ImGui::BeginPopup(popup_id) ) return;
    
    

    //  1.  CONTEXT MENU FOR SPECIALIZED SELECTIONS (Single vs. Multi.)...
    if (total_items == 1) {
#ifdef __CBAPP_DEBUG__
        ImGui::TextDisabled("Single");
#endif  //  __CBAPP_DEBUG__  //
        _selection_context_single(it);
    }
    else {
#ifdef __CBAPP_DEBUG__
        ImGui::TextDisabled("Multi.");
#endif  //  __CBAPP_DEBUG__  //
        _selection_context_multi(it);
    }


    //  2.  DEFAULT FUNCTIONS ENABLED FOR *ALL* SELECTIONS...
    ImGui::Separator();
#ifdef __CBAPP_DEBUG__
    ImGui::TextDisabled("Primative");
#endif  //  __CBAPP_DEBUG__  //
    _selection_context_primative(it);



    ImGui::EndPopup();
    return;
}




// *************************************************************************** //
//      SUBSIDIARY FUNCTIONS FOR "SELECTION" MENU:
// *************************************************************************** //

//  "_selection_context_primative"
//      Functions that can operate on ANY set of selected items.
//
inline void Editor::_selection_context_primative([[maybe_unused]] const Interaction & it)
{
    
    //  1.  REORDER Z-ORDER OF OBJECTS ON CANVAS...
    if ( ImGui::BeginMenu("Arrange") ) {
        //
        if ( ImGui::MenuItem("Bring to Front")  )       { this->bring_selection_to_front();     }
        if ( ImGui::MenuItem("Bring Forward")   )       { this->bring_selection_forward();      }
        ImGui::Separator();
        if ( ImGui::MenuItem("Send Backwards")  )       { this->send_selection_backward();      }
        if ( ImGui::MenuItem("Send to Back")    )       { this->send_selection_to_back();       }
        //
        ImGui::EndMenu();
    }
    
    
    
    
    ImGui::Separator();
    
    
    


    //  2.  COPY SELECTION...
    if ( ImGui::MenuItem("Cut") )
    {
        // TODO: implement copy/duplicate behaviour
        this->copy_to_clipboard();
    }


    //  3.  COPY SELECTION...
    const bool          copy_menu       = ImGui::BeginMenu("Copy");
    const bool          copy_clicked    = ImGui::IsItemClicked(ImGuiMouseButton_Left);
    if ( copy_menu )
    {
        if ( ImGui::MenuItem("Copy") || copy_clicked ) {
            this->copy_to_clipboard();
            if (copy_clicked)   { ImGui::CloseCurrentPopup(); }     //  If we clicked on the "Copy" menu drop-down itself, close the Pop-up.
        }
        //
        ImGui::Separator();
        ImGui::BeginDisabled(true);
        //
            if ( ImGui::MenuItem("Copy Properties")     )       { /* this->copy_to_clipboard(); */ }
            if ( ImGui::MenuItem("Copy Payload")        )       { /* this->copy_to_clipboard(); */ }
        //
        ImGui::EndDisabled();
        ImGui::EndMenu();
    }


    //  4.  DELETE SELECTION...
    if ( ImGui::MenuItem("Delete") )        { this->delete_selection(); }


    
    return;
}


//  "_selection_context_single"
//      Functions that operate ONLY ON SINGLE-ITEM SELECTIONS.
//
inline void Editor::_selection_context_single([[maybe_unused]] const Interaction & it)
{
    const size_t                    sel_idx             = *m_sel.paths.begin();   // only element
    Path &                          path                = m_paths[sel_idx];
    
    
    //  1.  PROPERTIES...
    if ( ImGui::BeginMenu("Payload") ) {
        //
        if ( path.ui_kind() )                   { /*    changed the path kind.      */ }
        path.ui_properties();
        //
        ImGui::EndMenu();
    }
    
    
    

    //  2.  TRANSFORM...
    if ( ImGui::BeginMenu("Transform") )
    {
        ImGui::BeginDisabled(true);
        //
            if ( ImGui::MenuItem("Move")            )       { /*  TODO:  */     }
            if ( ImGui::MenuItem("Scale")           )       { /*  TODO:  */     }
            if ( ImGui::MenuItem("Rotate")          )       { /*  TODO:  */     }
            if ( ImGui::MenuItem("Reflect")         )       { /*  TODO:  */     }
            //
            ImGui::Separator();
            if ( ImGui::MenuItem("Quantize")        )       { /*  TODO:  */     }
            if ( ImGui::MenuItem("Smooth")          )       { /*  TODO:  */     }
        //
        ImGui::EndDisabled();
        ImGui::EndMenu();
    }
    
    
    //  Example: Rename path, convert to outline, etc.
    //      if (ImGui::MenuItem("Reverse Path Direction"))
    //      {
    //          // TODO: implement
    //      }
    
    
    
    return;
}


//  "_selection_context_multi"
//      Functions that operate ONLY WHEN MULTIPLE ITEMS ARE SELECTED.
//
inline void Editor::_selection_context_multi([[maybe_unused]] const Interaction & it)
{
    ImGui::BeginDisabled(true);
    
    
    if ( ImGui::MenuItem("Create Group") )
    {
        // TODO: implement grouping
    }
    //  if (ImGui::MenuItem("Align Vertices"))
    //  {
    //      // TODO: implement alignment helpers
    //  }
    
    
    ImGui::EndDisabled();
    return;
}

//
//
// *************************************************************************** //
// *************************************************************************** //   END "SELECTION".
























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

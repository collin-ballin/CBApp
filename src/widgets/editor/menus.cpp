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
#include <ranges>



namespace cb {  //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //






// *************************************************************************** //
//
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
//
//      2.      CANVAS CONTEXT MENU...
// *************************************************************************** //
// *************************************************************************** //

//  "_show_canvas_context_menu"
//      When user RIGHT-CLICKS on an EMPTY POINT ON THE CANVAS (Allows them to PASTE at this location).
//
inline void Editor::_show_canvas_context_menu([[maybe_unused]] const Interaction & it, const char * popup_id)
{
    using                   cblib::utl::strcat_literals_cx;
    static constexpr auto   s_paste_label       = strcat_literals_cx( ICON_FA_PASTE,                "  ", "Paste"       );
    const bool              can_paste           = !m_clipboard.empty();
        
        
    //      CASE 0 :    Jump-out early if NO POPUP WINDOW...
    if ( !ImGui::BeginPopup(popup_id) )     { return; }
    //
    //
        //      0.      TEMPORARY LABEL...
        ImGui::TextColored(this->S.SystemColor.Gray, "Canvas Context Menu");
        ImGui::Separator();
    
        //      1.      PASTE BUTTON...
        ImGui::BeginDisabled( !can_paste );
        //
            if ( ImGui::MenuItem(s_paste_label.data(), nullptr, false, can_paste) )
                { paste_from_clipboard( pixels_to_world(ImGui::GetIO().MousePos) ); }
        //
        ImGui::EndDisabled();
    
        //if ( ImGui::MenuItem("Paste", nullptr, false, can_paste) )
        //{ paste_from_clipboard(pixels_to_world(ImGui::GetIO().MousePos)); }
    //
    //
    ImGui::EndPopup();
    return;
}

//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "CANVAS".







// *************************************************************************** //
//
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
    if ( !ImGui::BeginPopup(popup_id) )     { return; }
    
    

    //      1.      CONTEXT MENU FOR SPECIALIZED SELECTIONS (Single vs. Multi.)...
    if (total_items == 1) {
#ifdef __CBAPP_DEBUG__
        ImGui::TextColored(this->S.SystemColor.Gray, "Single");
#endif  //  __CBAPP_DEBUG__  //
        _selection_context_single(it);
    }
    else {
#ifdef __CBAPP_DEBUG__
        ImGui::TextColored(this->S.SystemColor.Gray, "Multi.");
#endif  //  __CBAPP_DEBUG__  //
        _selection_context_multi(it);
    }


    //      2.      DEFAULT FUNCTIONS ENABLED FOR *ALL* SELECTIONS...
    ImGui::Separator();
    ImGui::NewLine();
    ImGui::Separator();
#ifdef __CBAPP_DEBUG__
        ImGui::TextColored(this->S.SystemColor.Gray, "Primative");
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
    using                       cblib::utl::strcat_literals_cx;
    static constexpr auto       s_bring_to_label        = strcat_literals_cx( ICON_FA_LAYER_GROUP,          "  ", "Bring To New Layer"  );  //  ICON_FA_LAYER_GROUP     ICON_FA_ROUTE
    static constexpr auto       s_arrange_label         = strcat_literals_cx( ICON_FA_RETWEET,              "  ", "Arrange"             );  //  ICON_FA_SQUARE_BINARY
    static constexpr auto       s_cut_label             = strcat_literals_cx( ICON_FA_CLIPBOARD_CHECK,      "  ", "Cut"                 );
    static constexpr auto       s_copy_label            = strcat_literals_cx( ICON_FA_COPY,                 "  ", "Copy"                );
    static constexpr auto       s_paste_label           = strcat_literals_cx( ICON_FA_PASTE,                "  ", "Paste"               );
    static constexpr auto       s_delete_label          = strcat_literals_cx( ICON_FA_XMARK,                "  ", "Delete"              );
    
    
    //      1.      MOVE-TO NEW LAYER...
    ImGui::BeginDisabled(true);
    if ( ImGui::BeginMenu(s_bring_to_label.data()) ) {
        //
        if ( ImGui::MenuItem("...")  )                  { /*  TO-DO...  */ }
        //
        ImGui::EndMenu();
    }
    ImGui::EndDisabled();
    
    
    
    //      2.      REORDER Z-ORDER OF OBJECTS ON CANVAS...
    if ( ImGui::BeginMenu(s_arrange_label.data()) ) {
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
    ImGui::NewLine();
    ImGui::Separator();
    ImGui::TextColored(this->S.SystemColor.Gray, "Basic");

    //      3.      CUT SELECTION...
    if ( ImGui::MenuItem(s_cut_label.data()) )
    {
        // TODO: implement copy/duplicate behaviour
        this->copy_to_clipboard();
    }



    //      4.      COPY SELECTION...
    const bool          copy_menu       = ImGui::BeginMenu(s_copy_label.data());
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


    //      5.      PASTE SELECTION...
    ImGui::BeginDisabled(true);
    const bool          paste_menu       = ImGui::BeginMenu(s_paste_label.data());
    if ( paste_menu )
    {
        ImGui::Separator();
        ImGui::BeginDisabled(true);
        //
            if ( ImGui::MenuItem("Paste Properties")     )       { /* this->paste_from_clipboard(); */ }
            if ( ImGui::MenuItem("Paste Payload")        )       { /* this->paste_from_clipboard(); */ }
        //
        ImGui::EndDisabled();
        ImGui::EndMenu();
    }
    ImGui::EndDisabled();
    

    //      6.      DELETE SELECTION...
    ImGui::Separator();
    if ( ImGui::MenuItem(s_delete_label.data()) )       { this->delete_selection(); }
    
    
 
    return;
}


//  "_selection_context_single"
//      Functions that operate ONLY ON SINGLE-ITEM SELECTIONS.
//
inline void Editor::_selection_context_single([[maybe_unused]] const Interaction & it) noexcept
{
    using                   cblib::utl::strcat_literals_cx;
    static constexpr auto   s_payload_label     = strcat_literals_cx( ICON_FA_CART_FLATBED_SUITCASE,        "  ", "Payload"                 ); //  ICON_FA_SQUARE_BINARY
    static constexpr auto   s_transform_label   = strcat_literals_cx( ICON_FA_TEXT_WIDTH,                   "  ", "Transform"               );
    //
    static constexpr auto   s_move_label        = strcat_literals_cx( ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT,    "  ", "Move"                    );
    static constexpr auto   s_scale_label       = strcat_literals_cx( ICON_FA_MAXIMIZE,                     "  ", "Scale"                   );  //  ICON_FA_EXPAND
    static constexpr auto   s_rotate_label      = strcat_literals_cx( ICON_FA_ROTATE_LEFT,                  "  ", "Rotate"                  );
    static constexpr auto   s_reflect_label     = strcat_literals_cx( ICON_FA_ARROWS_LEFT_RIGHT_TO_LINE,    "  ", "Reflect"                 );
    //
    static constexpr auto   s_smooth_label      = strcat_literals_cx( ICON_FA_MOUND,                        "  ", "Smooth"                  );
    static constexpr auto   s_pixel_label       = strcat_literals_cx( ICON_FA_RULER_COMBINED,               "  ", "Make Pixel-Perfect"      );
    static constexpr auto   s_quantize_label    = strcat_literals_cx( ICON_FA_STAIRS,                       "  ", "Quantize"                );
    //
    const size_t            sel_idx             = *m_sel.paths.begin();   // only element
    Path &                  path                = m_paths[sel_idx];
    
    
    
    //      1.      PROPERTIES...
    if ( ImGui::BeginMenu(s_payload_label.data()) ) {
        //
        if ( path.ui_payload_type() )           { /*    changed the path kind.      */ }
        path.ui_properties();
        //
        ImGui::EndMenu();
    }
    
    
    
    //      2.      TRANSFORM...
    if ( ImGui::BeginMenu(s_transform_label.data()) )
    {
        //          2.1.    BASIC OPERATIONS...
        ImGui::BeginDisabled(true);
        //
            if ( ImGui::MenuItem(s_move_label       .data() )   )     { /*  TODO:  */     }   //  Move
            if ( ImGui::MenuItem(s_scale_label      .data() )   )     { /*  TODO:  */     }   //  Scale
            if ( ImGui::MenuItem(s_rotate_label     .data() )   )     { /*  TODO:  */     }   //  Rotate
            if ( ImGui::MenuItem(s_reflect_label    .data() )   )     { /*  TODO:  */     }   //  Reflect
            //
            ImGui::Separator();
            if ( ImGui::MenuItem(s_smooth_label     .data() )   )     { /*  TODO:  */     }   //  Smooth
            if ( ImGui::MenuItem(s_pixel_label      .data() )   )     { /*  TODO:  */     }   //  Make Pixel-Perfect
            if ( ImGui::MenuItem(s_quantize_label   .data() )   )     { /*  TODO:  */     }   //  Quantize
        //
        ImGui::EndDisabled();
        
        
        //          2.2.    ADVANCED OPERATIONS...
        this->_selection_context_single_advanced    ( it,   path );
        
        
        ImGui::EndMenu();
    }
    
    
    
    return;
}


//  "_selection_context_single_advanced"
//
inline void Editor::_selection_context_single_advanced([[maybe_unused]] const Interaction & /*it*/, Path & /*path*/) noexcept
{
    using                   cblib::utl::strcat_literals_cx;
    static constexpr auto   s_discretize_label  = strcat_literals_cx( ICON_FA_STAMP                    , "  ", "Stamp To Grid"     );           //  ICON_FA_BURST
    static constexpr auto   s_interpolate_label = strcat_literals_cx( ICON_FA_FEATHER_POINTED          , "  ", "Interpolate Perimeter"  );      //  ICON_FA_CIRCLE_HALF_STROKE
    
    
    //      0.      SEPARATOR...
    ImGui::Separator();
    ImGui::NewLine();
    ImGui::Separator();
    
    
    
    //      1.      TRANSFORM...
    ImGui::BeginDisabled(true);
    {
    
        //  Ice-Cube Tray.      Stamp
        if ( ImGui::MenuItem(s_discretize_label     .data() )   )     { /*  TODO:  */     }   //  Discretize To-Grid        [ "Conform" to Grid ]   [ Melt and Cure ]   [ Cast ]
        if ( ImGui::MenuItem(s_interpolate_label    .data() )   )     { /*  TODO:  */     }   //  Interpolate               [ "Feather" Edges ]
        
    }
    ImGui::EndDisabled();
    
    
    
    return;
}






//  "_selection_context_multi"
//      Functions that operate ONLY WHEN MULTIPLE ITEMS ARE SELECTED.
//
inline void Editor::_selection_context_multi([[maybe_unused]] const Interaction & it)
{
    using                   cblib::utl::strcat_literals_cx;
    static constexpr auto   s_group_label       = strcat_literals_cx( ICON_FA_OBJECT_GROUP,                 "  ", "Create Group"    );

    ImGui::BeginDisabled(true);
    //
    //
        if ( ImGui::MenuItem(s_group_label.data()) )
        {
            // TODO: implement grouping
        }
        //  if (ImGui::MenuItem("Align Vertices"))
        //  {
        //      // TODO: implement alignment helpers
        //  }
    //
    //
    ImGui::EndDisabled();
    
    return;
}

//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "SELECTION".






// *************************************************************************** //
//
//
//
//      4.      OTHER CONTEXT MENUS...
// *************************************************************************** //
// *************************************************************************** //

//  "_MENU_tool_selection"
//
bool Editor::_MENU_tool_selection(const Mode current_tool) noexcept
{
    //  using                               IconStyle           = utl::icon_widgets::Style;
    using                               IconAnchor          = utl::icon_widgets::Anchor;
    using                               Padding             = utl::icon_widgets::PaddingPolicy;
    //
    constexpr ImGuiTableFlags           FLAGS               = ImGuiTableFlags_SizingFixedFit; //    ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Resizable;
    constexpr ImGuiSelectableFlags      SELECTABLE_FLAGS    = ImGuiSelectableFlags_SpanAllColumns; //    ImGuiSelectableFlags_SpanAllColumns; ImGuiSelectableFlags_None
    constexpr const char *              HOTKEY_FMT          = "[ %s ]";
    //
    //
    EditorStyle &                       Style               = this->m_style;
    constexpr size_t                    N                   = static_cast<size_t>( Mode::COUNT );
    //
    //
    static ImVec2                       rect_size           = {-1, -1};
    static float                        col1_width          = -1;
    static float                        col2_width          = -1;
    //
    Mode                                idx                 = static_cast<Mode>( 0 );
    bool                                close               = false;
    const ImVec2                        table_dims          = { col1_width + col2_width + rect_size.x,  -1 };

    
    //      1.      CACHE THE WIDTH OF THE LARGEST ITEM IN THE LIST-OF-NAMES...
    if ( ImGui::IsWindowAppearing() )
    {
        auto    width_of        = [](const char * s) -> float   {  return ImGui::CalcTextSize(s ? s : "", nullptr, true).x;    };
        auto    tool_name       = this->ms_EDITOR_STATE_NAMES        | std::views::transform(width_of);     //  Find  "const char *"  with longest length.
        auto    hotkey_name     = this->ms_EDITOR_STATE_HOTKEY_NAMES | std::views::transform(width_of);
        //
        col1_width              = 1.60f * ( *std::ranges::max_element(tool_name) );
        col2_width              = 1.60f * ( *std::ranges::max_element(hotkey_name) );
    }



    //      CASE 0 :    IF NO TABLE, EXIT EARLY...
    ImGui::SeparatorText( GetMenuName(PopupHandle::ToolSelection) );
    //  ImGui::NewLine();
    //
    if ( !ImGui::BeginTable("ToolSelectionTable", 3, FLAGS, table_dims) )    { return false; }
    
    
    //  BEGIN THE TABLE...
    {
        ImGui::TableSetupColumn("##ToolIconColumn"      , ImGuiTableColumnFlags_WidthFixed                  );
        ImGui::TableSetupColumn("##ToolLabelColumn"     , ImGuiTableColumnFlags_WidthStretch        , 1     );
        ImGui::TableSetupColumn("##ToolIconHotkey"      , ImGuiTableColumnFlags_WidthStretch        , 1     );
        //  ImGui::TableHeadersRow();
    
    
    
        //      1.      ITERATE THROUGH EACH TOOL...
        for (size_t i = 0; (!close) && (i < N);  ++i, idx = static_cast<Mode>( i ))
        {
            ImGui::TableNextRow(ImGuiTableRowFlags_None, rect_size.y );
            bool      selected    = (current_tool == idx);
            
            
            //          1.1.    DRAW ICON (SELECTABLE) FOR THE TOOL...
            ImGui::TableSetColumnIndex(0);
            ImGui::PushID( static_cast<int>(i) );
            const bool      dirty1      = utl::IconButton(   "##ToolIcon"
                                                           , (selected)
                                                                ? this->S.SystemColor.Blue      : this->S.SystemColor.White
                                                           , this->ms_EDITOR_STATE_ICONS[ idx ]
                                                           , Style.ms_TOOLBAR_ICON_SCALE
                                                           , IconAnchor::TextBaseline    // TextBaseline    South   Center
                                                           , Padding::Default );
            rect_size                   = ImGui::GetItemRectSize();
            ImGui::PopID();
            
            
            
            //          1.2.    DRAW "LABEL" FOR THE TOOL...
            ImGui::TableSetColumnIndex(1);
            const bool      dirty2      = ImGui::Selectable( this->ms_EDITOR_STATE_NAMES[ idx ], selected, SELECTABLE_FLAGS, {0.0f, rect_size.x} );
            
            
            //
            //
            //  const bool dirty1 = false;
            //  ImGui::PushID( static_cast<int>(i) );
            //  //
            //  const bool      dirty2      = utl::IconSelectable(   "##ToolSelectable"
            //                                                     , selected
            //                                                     , SELECTABLE_FLAGS
            //                                                     , (selected)
            //                                                          ? this->S.SystemColor.Blue      : this->S.SystemColor.White
            //                                                     , this->ms_EDITOR_STATE_ICONS[ idx ]
            //                                                     , IconStyle(
            //                                                            Style.ms_TOOLBAR_ICON_SCALE
            //                                                          , IconAnchor::West
            //                                                          , Padding::Default
            //                                                      )
            //                              );
            //  rect_size                   = ImGui::GetItemRectSize();
            //  ImGui::PopID();
            
            
        
            //          1.3.    ADD A "LABEL" TO DISPLAY TOOL'S HOTKEY...
            ImGui::TableSetColumnIndex(2);
            ImGui::Text(HOTKEY_FMT, this->ms_EDITOR_STATE_HOTKEY_NAMES[ idx ]);
            //
            close                       = (dirty1 || dirty2);
        
        
        
            //          1.3.    IF ITEM IS SELECTED, UPDATE THE NEW TOOL STATE...
            if ( close  &&  !selected )
            {
                this->m_mode = idx;
            }
            
            
        }//----END "FOR-LOOP".
        
        
        
        ImGui::EndTable();
    }//----END TABLE.
        
        
        
    //  #ifndef _TEMP_USE_SELECTABLE
    //  //
    //      ImGui::AlignTextToFramePadding();
    //      if (!selected) {
    //          ImGui::Text( "%s", this->ms_EDITOR_STATE_NAMES[ idx ] );
    //      }
    //      else {
    //          ImGui::TextColored( this->S.SystemColor.Blue, "%s", this->ms_EDITOR_STATE_NAMES[ idx ] );
    //      }
    //      close                       = dirty1;
    //  //
    //  # else
    //  //
    //      const bool      dirty2      = ImGui::Selectable( this->ms_EDITOR_STATE_NAMES[ idx ], selected );
    //      close                       = (dirty1 || dirty2);
    //  //
    //  #endif  //  _TEMP_USE_SELECTABLE  //
    
    
    
    
    
    
    
    return !close;  //  Return TRUE if window should CLOSE...
}



//  "_MENU_filter_selection"
//
bool Editor::_MENU_filter_selection(void) noexcept
{
    ImGui::SeparatorText( GetMenuName(PopupHandle::FilterCTX) );
    ImGui::NewLine();
    
    return true;
}



//  "_MENU_object_browser_item"
//
bool Editor::_MENU_object_browser_item(void) noexcept
{
    ImGui::SeparatorText( GetMenuName(PopupHandle::BrowserCTX) );
    ImGui::NewLine();
    
    return true;
}






//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "MISC." CONTEXT MENUS.












// *************************************************************************** //
//
//
//
//      5.      CUSTOM MENU-BAR MENUS...
// *************************************************************************** //
// *************************************************************************** //

//  "_MENUBAR_object_menu"
//
void Editor::_MENUBAR_object_menu(void) noexcept
{
    return;
}



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "5.  MENUBAR".

















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

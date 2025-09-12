/***********************************************************************************
*
*       *********************************************************************
*       ****           R E S I D E N T . C P P  ____  F I L E            ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      July 10, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "widgets/editor/editor.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //




// *************************************************************************** //
//
//
//
//      4.  RESIDENT OVERLAY WINDOWS...
// *************************************************************************** //
// *************************************************************************** //






// *************************************************************************** //
//
//
//
//          4.1.    DEBUGGER OVERLAY.
// *************************************************************************** //
// *************************************************************************** //

//  "_draw_debugger_resident"
//
void Editor::_draw_debugger_resident(void)
{
    using                       DebugItem       = DebuggerState::DebugItem;
    static DebuggerState &      debug           = this->m_debugger;
    //
    //
    const size_t                N               = debug.windows.size();
    int                         Nopen           = 0;
    
    
    
    //      1.      HEADER CONTENT...
    S.PushFont(Font::Small);
    //
    //
    //  //          1.A.        RESIDENT TITLE.
        ImGui::BeginDisabled(true);
            ImGui::SeparatorText("Debugger Resident");
        ImGui::EndDisabled();
    //
    //
    S.PopFont();//  END "HEADER".
    
    
    
    
    //      2.      BODY ENTRIES...
    S.PushFont(Font::FootNote);
    {
        //          2A.     COUNT NUM. VISIBLE WINDOWS.
        for (const DebugItem & item : debug.windows) {
            Nopen   += static_cast<int>( (item.open == true) );
        }
    
    
        //          2B.     DISPLAY EACH WINDOW.
        for (size_t i = 0; i < N; ++i)
        {
            DebugItem &     item        = debug.windows.at(i);
            const bool      same_line   = (  (1 < Nopen) && (i != 0)  );
            
            //      2B-1.       SAME-LINE AS NEXT WINDOW (IF MORE WINDOWS EXIST).
            if (same_line) {
                //  ImGui::SameLine();
            }
            
            //      2B-2.       DISPLAY WINDOW.
            if ( item.open ) {
                ImGui::SetNextWindowBgAlpha(0.5f);
                item.render_fn();
            }
            
        }
    }//  END "BODY".
        
    
    
    //      3.      CONTROL BAR...
    {
    //
        ImGui::NewLine();
        ImGui::Separator();
        this->_debugger_draw_controlbar(/*Nopen*/);
    //
    }// END "CONTROLS".



    S.PopFont();
    return;
}




//  "_debugger_draw_controlbar"
//
void Editor::_debugger_draw_controlbar(void)
{
    static constexpr const char *   uuid                    = "##Editor_Debugger_Controls_Columns";
    //
    static ImGuiOldColumnFlags      COLUMN_FLAGS            = ImGuiOldColumnFlags_None;
    //
    const ImVec2                    WIDGET_SIZE             = ImVec2( -1,               1.2f * ImGui::GetTextLineHeight()               );
    const ImVec2                    BUTTON_SIZE             = ImVec2( WIDGET_SIZE.y,    WIDGET_SIZE.y                                   );
    //
    //
    using                           DebugItem               = DebuggerState::DebugItem;
    static DebuggerState &          debug                   = this->m_debugger;
    const int                       NC                      = static_cast<int>( debug.windows.size() );
    const int                       NE                      = 0;
    const int                       N                       = NC + NE;
    


    //      1.      BEGIN COLUMNS...
    ImGui::PushItemWidth( BUTTON_SIZE.x );
    ImGui::Columns(N, uuid, COLUMN_FLAGS);
    //
    //
        //      DRAW A TOGGLE-SWITCH FOR EACH DEBUGGER SECTION...
        for (int i = 0; i < NC; ++i)
        {
            DebugItem &     item    = debug.windows.at(i);
            
            if ( i != 0 ) {
                ImGui::NextColumn();
            }
            this->S.column_label( item.uuid.c_str() );
            
            ImGui::PushID(i);
                ImGui::Checkbox("##DebugSectionToggle",     &item.open);
            ImGui::PopID();
        }
        //
        //
        //      ?.      EMPTY SPACES FOR LATER...
        for (int i = ImGui::GetColumnIndex(); i < N; ++i) {
            ImGui::Dummy( ImVec2(0,0) );    ImGui::NextColumn();
        }
    //
    //
    ImGui::Columns(1);      //  END COLUMNS...
    ImGui::PopItemWidth();
    
    
    return;
}



// *************************************************************************** //
//      HELPERS...
// *************************************************************************** //

//  "_debugger_hit_detection"
//
void Editor::_debugger_hit_detection(void)
{
    const float                         LABEL_W             = 0.6f * m_style.ms_SETTINGS_LABEL_WIDTH;
    const float &                       WIDGET_W            = m_style.ms_SETTINGS_WIDGET_WIDTH;
    EditorState &                       ES                  = this->m_editor_S;
    [[maybe_unused]] Interaction &      it                  = *this->m_it;
    //
    //
    S.PushFont(Font::Small);
    
    
    //      1.      CURRENT CANVAS WINDOW DIMENSIONS...
    this->S.labelf("Canvas Window:", LABEL_W, WIDGET_W);
    ImGui::Text( "X: (%.0f, %.0f).  Y: (%.0f, %.0f)",       ES.m_window_size.X.Min,     ES.m_window_size.X.Max,
                                                            ES.m_window_size.Y.Min,     ES.m_window_size.Y.Max  );

    //      2.      CURRENT "ZOOM" DIMENSIONS...
    this->S.labelf("Canvas Zoom:", LABEL_W, WIDGET_W);
    ImGui::Text( "X: (%.0f, %.0f).  Y: (%.0f, %.0f)",       ES.m_window_size.X.Min,     ES.m_window_size.X.Max,
                                                            ES.m_window_size.Y.Min,     ES.m_window_size.Y.Max  );
    
    

    //      3.      HOVERED ITEM...
    this->S.labelf("Hovered:", LABEL_W, WIDGET_W);
    //
    S.print_TF(
        ( m_sel.hovered.has_value() ),
        (m_sel.hovered)
            ? ms_HIT_TYPE_NAMES[ (*m_sel.hovered).type ]
            : "None",
        "None"
    );
    
    //  S.print_TF( !it.BlockShortcuts() );




    S.PopFont();
    return;
}


//  "_debugger_interaction"
//
void Editor::_debugger_interaction(void)
{
    const float                     LABEL_W                 = 0.6f * m_style.ms_SETTINGS_LABEL_WIDTH;
    const float &                   WIDGET_W                = m_style.ms_SETTINGS_WIDGET_WIDTH;
    //
    Interaction &                   it                      = *this->m_it;
    //
    
    
    //      1.      SHORTCUTS ENABLED/DISABLED...
    this->S.labelf("Shortcuts:", LABEL_W, WIDGET_W);
    S.print_TF( !it.BlockShortcuts() );
    
    
    //      2.      INPUTS ENABLED/DISABLED...
    this->S.labelf("Inputs:",       LABEL_W, WIDGET_W);
    S.print_TF( !it.BlockInput() );
    
    

    return;
}


//  "_debugger_more_info"
//
void Editor::_debugger_more_info(void)
{
    constexpr size_t                N_POPUPS                = ms_POPUP_INFOS.size();
    const float                     LABEL_W                 = 0.6f * m_style.ms_SETTINGS_LABEL_WIDTH;
    const float &                   WIDGET_W                = m_style.ms_SETTINGS_WIDGET_WIDTH;
    //
    Interaction &                   it                      = *this->m_it;
    const CBEditorPopupFlags &      open_menus              = it.obj.open_menus;
    const bool                      no_menus_open           = (open_menus == CBEditorPopupFlags_None);
    //
    static CBEditorPopupFlags       open_menus_cache        = it.obj.open_menus;
    static std::string              menu_buffer             = "";
    
    
    
    //      2.      ANY POP-UP MENUS OPEN?...
    if ( open_menus_cache != open_menus )     //  UPDATE CACHE...
    {
        open_menus_cache            = open_menus;
        
        //          2A.     NO MENUS OPEN...
        if ( no_menus_open ) {
            menu_buffer.clear();
        }
        //
        //          2B.     ADD THE NAME OF EACH OPEN-WINDOW INTO A SINGLE STRING (CSV)...
        else
        {
            for (size_t i = 0, n = N_POPUPS; i < n; ++i)
            {
                if ( (static_cast<CBEditorPopupFlags>(open_menus) >> i) & 1u )
                {
                    const PopupInfo & info = ms_POPUP_INFOS[i];
                    
                    /*      Append each "info.name" into a char [512] BUFFER in a COMMA-SEPARATED FORMAT        */
                    if ( info.name /* info.uuid*/ )
                    {
                        menu_buffer += info.name;
                        if ( (i != 0) && (i != (n-1)) )     { menu_buffer += ", ";  }
                        //  if ( i == (n-1) )                   { menu_buffer += ".";   }
                    }
                }
            }
        }
    }
    
    
    this->S.labelf("Open Menus:", LABEL_W, WIDGET_W);
    S.print_TF( !no_menus_open, menu_buffer.c_str(), "NONE" );



    return;
}




// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "DEBUGGER RESIDENT".













// *************************************************************************** //
//          4.2.    SELECTION OVERLAY.
// *************************************************************************** //

//  "_draw_selection_resident"
//
void Editor::_draw_selection_resident(void)
{
    const size_t        N_vertices      = m_sel.vertices.size();
    const size_t        N_points        = m_sel.points.size();
    const size_t        N_paths         = m_sel.paths.size();
    
    
    //      1.      HEADER CONTENT...
    S.PushFont(Font::FootNote);
    //
    //
        ImGui::TextDisabled("Selection:");
        //
        ImGui::SameLine(0.0f, 8.0f);
        //
        ImGui::Text("Paths: %zu.  Points: %zu.  Vertices: %zu.", N_paths, N_points, N_vertices);
    //
    //
    S.PopFont();
    
    
    

    //      2.      BODY CONTENT...
    ImGui::Separator();
    S.PushFont(Font::Small);
    //
        //
        //              ...
        //
    //
    S.PopFont();
    
    
    return;
}



// *************************************************************************** //
//          4.3.    "SHAPE-TOOL" OVERLAY.
// *************************************************************************** //

//  "_draw_shape_resident"
//
void Editor::_draw_shape_resident(void)
{
    int kind_idx = static_cast<int>(m_shape.kind);
    S.PushFont(Font::Small);


    ImGui::BeginDisabled(true);
        ImGui::SeparatorText("Shape Tool");
    ImGui::EndDisabled();
    //
    //
    //
    //  1.  DROP-DOWN FOR SHAPE TYPE.
    ImGui::Separator();
    if (ImGui::Combo( "##Editor_Shape_ShapeType",
                      &kind_idx,
                      this->ms_SHAPE_NAMES.data(),                          //  Names table
                      static_cast<int>(ShapeKind::COUNT)) )                 //  Item count
    {
        m_shape.kind = static_cast<ShapeKind>(kind_idx);
    }
    //
    //  2.  SLIDER FOR SHAPE RADIUS...
    switch (m_shape.kind)
    {
        //  2.1.    Shapes with only ONE Variable (Radius).
        case ShapeKind::Square      :
        case ShapeKind::Circle      :
        case ShapeKind::Oval        :
        case ShapeKind::Ellipse     : {
            this->_draw_shape_resident_default();
            break;
        }
        
        case ShapeKind::Rectangle   : {
            break;
        }
        
        default                     : {
            break;
        }
    
    }
    ImGui::SliderFloat("Radius", &m_shape.radius, 1.0f, 100.0f, "%.2f");
           

    S.PopFont();
    return;
}


//  "_draw_shape_default_resident"
//
void Editor::_draw_shape_resident_default(void)
{
    ImGui::BeginDisabled(true);
        ImGui::SliderFloat("Radius", &m_shape.radius, 1.0f, 100.0f, "%.2f");
    ImGui::EndDisabled();
    return;
}


//  "_draw_shape_resident_multi"
//      For drawing shapes that require more than one variable to describe.
//
void Editor::_draw_shape_resident_multi(void)
{
    ImGui::BeginDisabled(true);
        ImGui::SliderFloat("R", &m_shape.radius, 1.0f, 100.0f, "%.2f");
        ImGui::SliderFloat("S", &m_shape.radius, 1.0f, 100.0f, "%.2f");
    ImGui::EndDisabled();
    return;
}



//  "_draw_shape_resident_custom"
//      For drawing shapes that require more than one variable to describe.
//
void Editor::_draw_shape_resident_custom(void)
{
    ImGui::BeginDisabled(true);
        ImGui::SliderFloat("R", &m_shape.radius, 1.0f, 100.0f, "%.2f");
        ImGui::SliderFloat("S", &m_shape.radius, 1.0f, 100.0f, "%.2f");
    ImGui::EndDisabled();
    return;
}











// *************************************************************************** //
//          4.4.    UI-TRAITS OVERLAY...
// *************************************************************************** //

//  "_draw_ui_traits_resident"
//
void Editor::_draw_ui_traits_resident(void)
{
    BrowserStyle &      BStyle      = this->m_style.browser_style;
    const ImVec2        Avail       = ImGui::GetContentRegionAvail();
    int                 trait_i     = static_cast<int>( this->m_trait_overlay );
    
    
    this->S.PushFont(Font::Small);
    //
    //
        ImGui::BeginDisabled(true);
            ImGui::SeparatorText("Traits");
        ImGui::EndDisabled();
        
        
        ImGui::SetNextItemWidth( Avail.x );
        //ImGui::SetNextItemWidth( -1.0f );
        if ( ImGui::Combo("##UITraits_TraitSelector",           &trait_i,
                          ms_OBJECT_TRAIT_NAMES.data(),         static_cast<int>(ObjectTrait::COUNT)) )
        {
            this->m_trait_overlay = static_cast<ObjectTrait>(trait_i);
        }
        
        
        
        
        
        ImGui::BeginChild("##UITraits_Inspector",    {-1.0f, 0.0f},     BStyle.STATIC_CHILD_FLAGS);
            _dispatch_trait_inspector( /*which_menu = */this->m_trait_overlay, /*top_label = */this->S.ms_TopLabel );
        ImGui::EndChild();
    //
    //
    this->S.PopFont();
    
    
    
    return;
}


//  "_draw_ui_objects_resident"
//
void Editor::_draw_ui_objects_resident(void)
{
    BrowserStyle &      BStyle      = this->m_style.browser_style;
    
    
    this->S.PushFont(Font::Small);
    //
    //
        ImGui::BeginDisabled(true);
            ImGui::SeparatorText("Objects");
        ImGui::EndDisabled();
        
        
        
        ImGui::BeginChild("##UIObjects_Inspector",    {-1.0f, 0.0f},     BStyle.STATIC_CHILD_FLAGS);
            _draw_obj_selector_table(/*top_label = this->S.ms_TopLabel*/);
        ImGui::EndChild();
    //
    //
    this->S.PopFont();
    
    
    
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

/***********************************************************************************
*
*       *********************************************************************
*       ****           R E S I D E N T . C P P  ____  F I L E            ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond
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
    const size_t                N               = debug.windows.size();
    
    
    
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
    //
    S.PopFont();//  END "HEADER".
        
    
    
    //      2.      CONTROL BAR...
    S.PushFont(Font::FootNote);
    {
    //
        this->_debugger_draw_controlbar();
        //
        ImGui::NewLine();
        ImGui::Separator();
    //
    }// END "CONTROLS".
    
    
    
    
    //      3.      BODY ENTRIES...
    {
    //
    //
        for (size_t i = 0; i < N; ++i)
        {
            DebugItem & item    = debug.windows.at(i);
            
            //      3A.     DISPLAY WINDOW.
            if ( item.open ) {
                
                ImGui::SetNextWindowBgAlpha(0.0f);
                item.render_fn(); 
            
            }
            //
            //      3B.     SAME-LINE AS NEXT WINDOW (IF MORE WINDOWS EXIST).
            if ( i != (N-1) ) {
                ImGui::SameLine();
            }
            
        }
    //
    //
    }
    S.PopFont();//  END "BODY".



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
    const int                       NE                      = 1;
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

//  "_debugger_interaction"
//
void Editor::_debugger_interaction(void)
{
    const float                     LABEL_W             = 0.6f * m_style.ms_SETTINGS_LABEL_WIDTH;
    const float &                   WIDGET_W            = m_style.ms_SETTINGS_WIDGET_WIDTH;
    Interaction &                   it                  = *this->m_it;
    //
    auto                            PrintTF             = [&](const bool value, const char * true_text = "ENABLED", const char * false_text = "DISABLED" ) -> void {
        ImGui::TextColored(
            (value)     ? S.SystemColor.Green       : S.SystemColor.Red ,
            "%s",
            (value)     ? true_text                 : false_text
        );
    };
    
    
    //      1.      SHORTCUTS ENABLED/DISABLED...
    left_label("Shortcuts:", LABEL_W, WIDGET_W);
    //
    PrintTF( !it.BlockShortcuts() );
    
    
    
    //      2.      ANY POP-UP MENUS OPEN?...
    left_label("Context Menus Open:", LABEL_W, WIDGET_W);
    PrintTF( it.obj.menus_open, "YES", "NO" );
    





    return;
}


//  "_debugger_more_info"
//
void Editor::_debugger_more_info(void)
{

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
    const size_t        N_lines         = m_sel.lines.size();
    const size_t        N_paths         = m_sel.paths.size();
    
    
    //      1.      HEADER CONTENT...
    S.PushFont(Font::FootNote);
    //
    //
        ImGui::TextDisabled("Selection:");
        //
        ImGui::SameLine(0.0f, 8.0f);
        //
        ImGui::Text("Paths: %zu.  Lines: %zu.  Points: %zu.  Vertices: %zu.", N_paths, N_lines, N_points, N_vertices);
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

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
    
    
    
    //      1.      BODY ENTRIES...
    S.PushFont(Font::Small);
    {
        //          1A.     COUNT NUM. VISIBLE WINDOWS.
        for (const DebugItem & item : debug.windows) {
            Nopen   += static_cast<int>( (item.open == true) );
        }
    
    
        //          1B.     DISPLAY EACH WINDOW.
        for (size_t i = 0; i < N; ++i)
        {
            DebugItem &     item        = debug.windows.at(i);
            const bool      same_line   = ( (1 < Nopen)  &&  (i != 0) );
            
            //      1B-1.       SAME-LINE AS NEXT WINDOW (IF MORE WINDOWS EXIST).
            if (same_line) {
                //  ImGui::SameLine();
            }
            
            //      1B-2.       DISPLAY WINDOW.
            if ( item.open ) {
                ImGui::SetNextWindowBgAlpha(0.5f);
                item.render_fn();
            }
        }
    //
    }//  END "BODY".
        
    
    
    //      2.      CONTROL BAR...
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
    const int                       NE                      = 1;
    const int                       N                       = NC + NE;
    


    //      1.      BEGIN COLUMNS...
    ImGui::Columns(N, uuid, COLUMN_FLAGS);
    //
    //
        //      DRAW A TOGGLE-SWITCH FOR EACH DEBUGGER SECTION...
        ImGui:: PushItemWidth( BUTTON_SIZE.x );
        for (int i = 0; i < NC; ++i)
        {
            DebugItem &     item    = debug.windows.at(i);
            
            if ( i != 0 )   { ImGui::NextColumn(); }
            this->S.column_label( item.uuid.c_str() );
            
            ImGui::PushID(i);
                ImGui::Checkbox("##DebugSectionToggle",     &item.open);
            ImGui::PopID();
        }
        ImGui::PopItemWidth();
        //
        //
        //      ?.      EMPTY SPACES FOR LATER...
        for (int i = ImGui::GetColumnIndex(); i < N; ++i)       { ImGui::Dummy( ImVec2(0,0) );  ImGui::NextColumn(); }
    //
    //
    ImGui::Columns(1);      //  END COLUMNS...
    
    
    return;
}



// *************************************************************************** //
//      HELPERS...
// *************************************************************************** //

//  "_DEBUGGER_state"
//
void Editor::_DEBUGGER_state(void) const noexcept
{
    const float                 LABEL_W             = 0.6f * m_style.ms_SETTINGS_LABEL_WIDTH;
    const float &               WIDGET_W            = m_style.ms_SETTINGS_WIDGET_WIDTH;
    //  const EditorState &         ES                  = this->m_editor_S;
    const Interaction &         it                  = *(this->m_it);
    //
    static std::string          menu_names          = { };
    //
    //
    //
    const char *                current_action      = this->ms_ACTION_STATE_NAMES[ this->m_action ];
    const bool                  action_color        = ( (this->m_action != Action::None)  &&  (this->m_action != Action::Invalid) );
    const bool                  menus_open          = this->GetOpenMenuNames(menu_names);
    //
    const bool                  no_shortcuts        = it.BlockShortcuts();
    const bool                  no_inputs           = it.BlockInput();
    const bool                  show_interactions   = ( no_shortcuts  ||  no_inputs );



    //      1.      MAIN ITEMS...
    this->S.PushFont(Font::Main);
    {
        //          1.1.    CURRENT ACTION.
        this->S.labelf("Action:", LABEL_W, WIDGET_W);
        S.print_TF( (action_color), current_action, current_action );
        //
        //  if ( this->m_action != Action::None ) {
        //      this->S.labelf("Action:", LABEL_W, WIDGET_W);
        //      S.print_TF( (this->m_action != Action::Invalid), current_action, current_action );
        //  }
        
        
        //          1.2.    HOVERED ITEM.
        if ( this->m_sel.hovered.has_value() ) {
            this->S.labelf("Hovered:", LABEL_W, WIDGET_W);
            ImGui::TextColored( this->S.SystemColor.Green, "%s", this->ms_HIT_TYPE_NAMES[ (*m_sel.hovered).type ] );
        };
        
    }
    this->S.PopFont();
    
    
    //      3.      GENERAL INTERACTIONS...
    if (show_interactions)
    {
        this->S.labelf("Disabled Interactions:", LABEL_W, WIDGET_W);
        //
        //          3.1.    SHORTCUTS ENABLED/DISABLED...
        S.ConditionalText( no_shortcuts,    "Shortcuts",        this->S.SystemColor.Red );
        //
        if ( no_inputs )   { ImGui::SameLine(); }
        //
        //          3.2.    INPUTS ENABLED/DISABLED...
        S.ConditionalText( no_inputs,       "Inputs",           this->S.SystemColor.Red );
    }
    
    
    //      4.      DRAGGING STATES...
    this->_DEBUGGER_state_dragging( LABEL_W, WIDGET_W );
 
        
    //      5.      OPEN MENUS...
    if (menus_open)
    {
        this->S.labelf("Open Menus:", LABEL_W, WIDGET_W);
        S.ConditionalText( menus_open,    menu_names.c_str(),        this->S.SystemColor.Green );
    }
    
    return;
}


//  "_DEBUGGER_state_dragging"
//
inline void Editor::_DEBUGGER_state_dragging(const float LABEL_W, const float WIDGET_W) const noexcept
{
    static constexpr const char *                       uuid            = "##Debugger_State_DraggingColumns";
    static ImGuiOldColumnFlags                          COLUMN_FLAGS    = ImGuiOldColumnFlags_None;
    //
    constexpr int                                       N               = 3;
    constexpr int                                       M               = 1;
    static std::array< bool , N >                       s_bools         = { false };
    static constexpr std::array< const char * , N >     s_labels        = {
          "m_dragging"
        , "m_dragging_handle"
        , "m_drawing"
    };
    
    
    
    s_bools[0]          = this->m_dragging;
    s_bools[1]          = this->m_dragging_handle;
    s_bools[2]          = this->m_drawing;
    //
    uint8_t     count   = 0;
    size_t      idx     = 0;
    
    
    {
        this->S.labelf("Dragging State:", LABEL_W, WIDGET_W);

        //      1.      BEGIN COLUMNS...
        ImGui::NewLine();
        ImGui::Columns(M, uuid, COLUMN_FLAGS);
        //
        //
        for ( int i = 0; i < N; ++i, idx = static_cast<size_t>(i) )
        {
            const bool      enabled     = s_bools[idx];
            
            if ( enabled )
            {
                if ( count > 0 )    { ImGui::NextColumn(); }
                ImGui::TextColored( (enabled) ? S.SystemColor.Green : S.SystemColor.Red,  "%s", s_labels[idx] );
                ++count;
            }
        }
        //
        //
        ImGui::Columns(1);      //  END COLUMNS...
    }
    
    
        
/*
                                    m_dragging                      = false;
    bool                                m_lasso_active                  = false;
    bool                                m_pending_clear                 = false;    //  pending click selection state ---
    //
    //                              PEN-TOOL STATE:
    bool                                m_drawing                       = false;
    bool                                m_dragging_handle               = false;
    bool                                m_dragging_out                  = true;
    VertexID                            m_drag_vid                      = 0;
*/
   
   
   
    return;
}
    
    

//  "_DEBUGGER_canvas"
//
void Editor::_DEBUGGER_canvas(void) const noexcept
{
    static float                LABEL_W                 = 0.6f * m_style.ms_SETTINGS_LABEL_WIDTH;
    static const float &        WIDGET_W                = m_style.ms_SETTINGS_WIDGET_WIDTH;
    const GridState &           GS                      = this->m_grid;
    const Interaction &         it                      = *this->m_it;
    
    
    
    //      1.      GRID...
    //
    //              1.1.      CURRENT CANVAS WINDOW DIMENSIONS...
    this->S.labelf("Window:", LABEL_W, WIDGET_W);
    ImGui::Text(
          "X: (%.0f, %.0f), Y: (%.0f, %.0f).    [ %.0f, %.0f ]."
        , GS.m_window_coords.X.Min           , GS.m_window_coords.X.Max
        , GS.m_window_coords.Y.Min           , GS.m_window_coords.Y.Max
        , GS.m_window_size[0]                , GS.m_window_size[1]
    );
    //
    //              1.2.      CURRENT "ZOOM" DIMENSIONS...
    this->S.labelf("Zoom:", LABEL_W, WIDGET_W);
    ImGui::Text(
          "X: (%.0f, %.0f).     Y: (%.0f, %.0f)"
        , GS.m_window_coords.X.Min           , GS.m_window_coords.X.Max
        , GS.m_window_coords.Y.Min           , GS.m_window_coords.Y.Max
    );
    //
    //              1.3.      PLOT MOUSE POSITION...
    this->S.labelf("Mouse Pos [PLOT]:", LABEL_W, WIDGET_W);
    ImGui::Text(
          "(%.0f, %.0f)."
        , it.mouse_pos.x                    , it.mouse_pos.y
    );
    

    return;
}


//  "GetOpenMenuNames"
//
inline bool Editor::GetOpenMenuNames(std::string & menu_buffer) const noexcept
{
    constexpr size_t                N_POPUPS                = ms_POPUP_INFOS.size();
    //
    Interaction &                   it                      = *this->m_it;
    static CBEditorPopupFlags       open_menus_cache        = it.obj.open_menus;
    //
    const CBEditorPopupFlags &      open_menus              = it.obj.open_menus;
    const bool                      no_menus_open           = (open_menus == CBEditorPopupFlags_None);
    const bool                      update_cache            = (open_menus_cache != open_menus);
    
    
    //  CASE 0 :    EARLY EXIT IF NO CACHE UPDATE...
    if ( !update_cache )            { return (open_menus != CBEditorPopupFlags_None); } //  UPDATE CACHE...
    
    
    open_menus_cache            = open_menus;   //      2.      ANY POP-UP MENUS OPEN?...
    
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
                }
            }
        }
    }
    
    
    return (open_menus != CBEditorPopupFlags_None);
}



// *************************************************************************** //
//      MISC DEBUGGER...
// *************************************************************************** //

//  "_DEBUGGER_misc"
//
void Editor::_DEBUGGER_misc(void) const noexcept
{
    static float                LABEL_W                 = 0.85f * m_style.ms_SETTINGS_LABEL_WIDTH;
    static const float &        WIDGET_W                = m_style.ms_SETTINGS_WIDGET_WIDTH;
    static bool                 misc_1                  = true;
    static bool                 misc_2                  = false;
    static bool                 misc_3                  = false;
    
    
    
    
    ImGui::Checkbox("Misc 1##Debugger_Misc_ShowMisc1",     &misc_1);
    ImGui::SameLine();
    ImGui::Checkbox("Misc 2##Debugger_Misc_ShowMisc1",     &misc_2);
    ImGui::SameLine();
    ImGui::Checkbox("Misc 3##Debugger_Misc_ShowMisc1",     &misc_3);
    
    ImGui::Separator();
                
                
    //  const bool      drag_vid    = ( this->m_drag_vid > 0 );
    //  if (drag_vid) {
    //      this->S.labelf("drag_vid:", LABEL_W, WIDGET_W);             //  1.1A.   this->m_dragging.
    //      ImGui::Text("%u", this->m_drag_vid );
    //  }
                
                
                
                
    if (misc_1)     { this->_DEBUGGER_misc_1      (LABEL_W, WIDGET_W); }
    if (misc_2)     { this->_DEBUGGER_misc_2      (LABEL_W, WIDGET_W); }
    if (misc_3)     { this->_DEBUGGER_misc_3      (LABEL_W, WIDGET_W); }
    

    return;
}


// *************************************************************************** //
//      MISC UTILITIES...
// *************************************************************************** //

//  "_DEBUGGER_misc_1"
//
inline void Editor::_DEBUGGER_misc_1(const float LABEL_W, const float WIDGET_W) const noexcept
{
    //              1.1.      "this"...
    ImGui::TextDisabled("This...");
    ImGui::Indent();
    //
        this->S.labelf("this->m_dragging:", LABEL_W, WIDGET_W);             //  1.1A.   this->m_dragging.
        S.print_TF( this->m_dragging );
    //
    ImGui::Unindent();
    
    
    
    //              1.3.      "m_boxdrag" OBJECT...
    ImGui::TextDisabled("MoveDrag / m_movedrag...");
    ImGui::Indent();
    //
        //
        S.ConditionalText( this->m_movedrag.active,    "active",        this->S.SystemColor.Green );
    //
    ImGui::Unindent();
    
    
    
    //              1.4.      "m_boxdrag" OBJECT...
    ImGui::TextDisabled("BoxDrag / m_boxdrag...");
    ImGui::Indent();
    //
        S.ConditionalText( this->m_boxdrag.active,    "active",        this->S.SystemColor.Green );
        //
        //  this->S.labelf(".view.hover_idx.has_value():", LABEL_W, WIDGET_W);
        //  S.print_TF( this->m_boxdrag.view.hover_idx.has_value() );
    //
    ImGui::Unindent();
    
    

    return;
}


//  "_DEBUGGER_misc_2"
//
inline void Editor::_DEBUGGER_misc_2(const float LABEL_W, const float WIDGET_W) const noexcept
{
    //      1.      QUERY...
    //
    //              1.1.      "this"...
    ImGui::TextDisabled("this");
    //
    this->S.labelf("this->m_dragging:", LABEL_W, WIDGET_W);             //  1.1A.   this->m_dragging.
    S.print_TF( this->m_dragging );
    //
    this->S.labelf("this->m_drag_vid > 0:", LABEL_W, WIDGET_W);         //  1.1B.   this->m_drag_vid > 0.
    S.print_TF( (this->m_drag_vid > 0) );
    //
    this->S.labelf("this->m_dragging_handle:", LABEL_W, WIDGET_W);      //  1.1C.   this->m_dragging_handle.
    S.print_TF( this->m_dragging_handle );
    
    
    
    //              1.3.      "m_boxdrag" OBJECT...
    ImGui::TextDisabled("m_movedrag");
    //
    this->S.labelf(".active:", LABEL_W, WIDGET_W);
    S.print_TF( this->m_movedrag.active );
    
    
    
    //              1.4.      "m_boxdrag" OBJECT...
    ImGui::TextDisabled("m_boxdrag");
    //
    this->S.labelf(".view.hover_idx.has_value():", LABEL_W, WIDGET_W);
    S.print_TF( this->m_boxdrag.view.hover_idx.has_value() );
    
    

    return;
}





//  "_DEBUGGER_misc_3"
//
inline void Editor::_DEBUGGER_misc_3(const float LABEL_W, const float WIDGET_W) const noexcept
{
    //      1.      PEN-TOOL STATE...
    ImGui::TextDisabled("PenState");
    //
    this->S.labelf("active:",               LABEL_W, WIDGET_W);         //  1.1.      active.
    S.print_TF( this->m_pen.active );
    //
    this->S.labelf("dragging_handle:",      LABEL_W, WIDGET_W);         //  1.2.      dragging_handle.
    S.print_TF( this->m_pen.dragging_handle );
    //
    
    
    this->S.labelf("path_index:",           LABEL_W, WIDGET_W);         //  1.2.      path_index.
    if ( this->m_pen.path_index.has_value() ) {
        ImGui::Text( "%zu",     *this->m_pen.path_index );
    }
    else    { ImGui::TextDisabled( "%s",     "None" ); }
    //
    this->S.labelf("last_vid:",             LABEL_W, WIDGET_W);         //  1.3.      last_vid.
    ImGui::Text( "%u",      this->m_pen.last_vid );
    
    
    //ImGui::NewLine();
    
    
    //this->S.labelf("pending_vid:",      LABEL_W, WIDGET_W);             //  2.1.      pending_vid.
    //S.print_TF( this->m_pen.pending_vid );
    
    
    
    
    
/*

struct PenState_t {
    bool            active                  = false;
//
    size_t          path_index              = static_cast<size_t>(-1);
    VID             last_vid                = 0;

    bool            dragging_handle         = false;
    bool            dragging_out            = true;     // NEW: true → out_handle, false → in_handle
    VID             handle_vid              = 0;

    bool            prepend                 = false;
    bool            pending_handle          = false;        // waiting to see if user drags
    VID             pending_vid             = 0;            // vertex that may get a handle
    float           pending_time            = 0.0f;
};


*/

    return;
}


// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "NEW DEBUGGER".












// *************************************************************************** //
//
//
//      OLD DEBUGGER STUFF...
// *************************************************************************** //
// *************************************************************************** //

//  "_debugger_hit_detection"
//
void Editor::_debugger_hit_detection(void)
{
    const float                         LABEL_W             = 0.6f * m_style.ms_SETTINGS_LABEL_WIDTH;
    const float &                       WIDGET_W            = m_style.ms_SETTINGS_WIDGET_WIDTH;
    const GridState &                   GS                  = this->m_grid;
    [[maybe_unused]] Interaction &      it                  = *this->m_it;
    //
    //
    //  S.PushFont(Font::Small);
    
    
    //      1.      CURRENT CANVAS WINDOW DIMENSIONS...
    this->S.labelf("Canvas Window:", LABEL_W, WIDGET_W);
    ImGui::Text( "X: (%.0f, %.0f).  Y: (%.0f, %.0f)",       GS.m_window_coords.X.Min,   GS.m_window_coords.X.Max,
                                                            GS.m_window_coords.Y.Min,   GS.m_window_coords.Y.Max    );
    //
    this->S.labelf("Window Size:",  LABEL_W, WIDGET_W);
    ImGui::Text( "[%.0f x %.0f]",                           GS.m_window_size[0],        GS.m_window_size[1]         );
    //
    //
    //
    //      2.      CURRENT "ZOOM" DIMENSIONS...
    this->S.labelf("Canvas Zoom:", LABEL_W, WIDGET_W);
    ImGui::Text( "X: (%.0f, %.0f).  Y: (%.0f, %.0f)",       GS.m_window_coords.X.Min,   GS.m_window_coords.X.Max,
                                                            GS.m_window_coords.Y.Min,   GS.m_window_coords.Y.Max    );
    
    

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




    //  S.PopFont();
    return;
}


//  "_debugger_interaction"
//
void Editor::_debugger_interaction(void)
{
    const float                     LABEL_W                 = 0.6f * m_style.ms_SETTINGS_LABEL_WIDTH;
    const float &                   WIDGET_W                = m_style.ms_SETTINGS_WIDGET_WIDTH;
    //
    Interaction &                   it                      = *(this->m_it);
    //
    //
    //
    const char *                    current_action          = this->ms_ACTION_STATE_NAMES[ this->m_action ];
    const bool                      action_color            = ( (this->m_action != Action::None)  &&  (this->m_action != Action::Invalid) );
    //
    
    
    //      1.      CURRENT ACTION...
    this->S.labelf("Action:", LABEL_W, WIDGET_W);
    S.print_TF( action_color, current_action, current_action );
    
    
    //      2.      SHORTCUTS ENABLED/DISABLED...
    this->S.labelf("Shortcuts:", LABEL_W, WIDGET_W);
    S.print_TF( !it.BlockShortcuts() );
    
    
    //      3.      INPUTS ENABLED/DISABLED...
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
    const size_t        N_paths         = m_sel.paths       .size();
    const size_t        N_vertices      = m_sel.vertices    .size();
    const size_t        N_points        = m_sel.points      .size();
    //
    const bool          has_paths       = ( N_paths > 0     );
    const bool          has_vertices    = ( N_vertices > 0  );
    const bool          has_points      = ( N_points > 0    );
    const uint8_t       total_count     = ( static_cast<uint8_t>(has_paths + has_vertices + has_points) );
    
    S.PushFont(Font::Small);
    
    
    
    //      1.      HEADER CONTENT...
    ImGui::TextColored( this->S.SystemColor.Blue, ICON_FA_OBJECT_GROUP );
    ImGui::SameLine(0.0f, 12.0f);
    
    
    
    //      2.      SELECTION INFO CONTENT...
    //                  CASE 1 :    PATHS...
    if ( has_paths ) {
        ImGui::Text("%zu %s", N_paths,      (N_paths == 1)      ? "Path"    : "Paths"       );
        if ( total_count > 1 )      { ImGui::SameLine(); }
    }
    //
    //                  CASE 2 :    VERTICES...
    if ( has_paths ) {
        ImGui::Text("%zu %s", N_vertices,   (N_vertices == 1)   ? "Vertex"  : "Vertices"    );
        if ( has_points )           { ImGui::SameLine(); }
    }
    //
    //                  CASE 3 :    POINTS...
    if ( has_points ) {
        ImGui::Text("%zu %s", N_points,     (N_points == 1)     ? "Point"   : "Points"      );
    }
    
    
    

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

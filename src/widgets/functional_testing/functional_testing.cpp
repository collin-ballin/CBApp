/***********************************************************************************
*
*       **************************************************************************
*       ****    F U N C T I O N A L _ T E S T I N G . C P P  ____  F I L E    ****
*       **************************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      July 20, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "widgets/functional_testing/functional_testing.h"


namespace cb { namespace ui { //     BEGINNING NAMESPACE "cb::ui"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//
//
//
//      2.      "ActionComposer" IMPLEMENTATION...
// *************************************************************************** //
// *************************************************************************** //




// *************************************************************************** //
//          INITIALIZATIONS...
// *************************************************************************** //

//  Default Constructor.
//
ActionComposer::ActionComposer(GLFWwindow * window)     : m_glfw_window(window)
{
    m_actions = &m_compositions.front().actions;   // initial seat
}






// *************************************************************************** //
//
//
//
// *************************************************************************** //
//          MAIN PUBLIC API FUNCTIONS...
// *************************************************************************** //

//  "Begin"
//
void ActionComposer::Begin(void)
{
    //  1.  DRAW THE "CONTROL-BAR" UI-INTERFACE...
    this->_update_capture();
    this->_draw_controlbar();
    
    //  2.  DRAW THE "BROWSER" UI-INTERFACE...
    this->draw_all();
    
    //  3.  DRIVE EXECUTION OF THE ACTION COMPOSITION...
    this->_drive_execution();
    this->_draw_overlay();
    this->_draw_renderer_visuals();
    
    return;
}
    
    
//  "draw_all"
//
void ActionComposer::draw_all(void)
{
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize,  ms_CHILD_BORDER1);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding,    ms_CHILD_ROUND1);
    ImGui::PushStyleColor(ImGuiCol_ChildBg,             ms_CHILD_FRAME_BG1L);
        
        //  1.  DRAW THE TOP-MOST "COMPOSITION" BAR...
        ImGui::BeginChild("##ActionComposer_CompositionSelector", {ms_COMPOSITION_COLUMN_WIDTH, 0.0f}, ImGuiChildFlags_Borders);
            this->_draw_composition_selector();
        ImGui::EndChild();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);  //  ImGuiStyleVar_ChildBorderSize, ImGuiStyleVar_ChildRounding


    ImGui::SameLine();
    
    
    //  2.  DRAW THE "BROWSER" UI-INTERFACE...
    ImGui::BeginChild("##ActionComposer_Action_Browser",  {0, 0}, ImGuiChildFlags_Borders);
        //
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize,  ms_CHILD_BORDER1);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding,    ms_CHILD_ROUND1);
        ImGui::PushStyleColor(ImGuiCol_ChildBg,             ms_CHILD_FRAME_BG1L);
        //
        //
        //
            ImGui::BeginChild("##ActionComposer_Action_Selector",  {ms_SELECTOR_COLUMN_WIDTH, 0}, ImGuiChildFlags_Borders);
                this->_draw_action_selector();
            ImGui::EndChild();
            ImGui::PopStyleColor();
            
            ImGui::SameLine();
            
            ImGui::PushStyleColor(ImGuiCol_ChildBg,         ms_CHILD_FRAME_BG1R);
            ImGui::BeginChild("##ActionComposer_Action_Inspector", {0,0},     ImGuiChildFlags_Borders);
                this->_draw_action_inspector();
            ImGui::EndChild();
        //
        //
        //
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);  //  ImGuiStyleVar_ChildBorderSize, ImGuiStyleVar_ChildRounding
        //
    ImGui::EndChild();
    


    return;
}






// *************************************************************************** //
//
//
//
// *************************************************************************** //
//          MAIN UI FUNCTIONS...
// *************************************************************************** //

//  "_draw_composition_selector"
//
void ActionComposer::_draw_composition_selector(void)
{
    //  1.  ADD A NEW COMPOSITION...
    if ( ImGui::Button("+ Add Composition") ) {
    
        m_compositions.emplace_back();
        m_comp_sel = static_cast<int>(m_compositions.size()) - 1;
        _load_actions_from_comp();     // already resets pointers & executor
    }
    
    ImGui::SameLine();
    
    ImGui::BeginDisabled(m_compositions.size() < 1);
        if ( ImGui::Button("- Delete") ) {
            m_compositions.erase(m_compositions.begin() + m_comp_sel);
            m_comp_sel = std::clamp(m_comp_sel, 0, (int)m_compositions.size() - 1);
            _load_actions_from_comp();
        }
    ImGui::EndDisabled();

    ImGui::Separator();


    // listbox
    for (int i = 0; i < (int)m_compositions.size(); ++i)
    {
        ImGui::PushID(i);
        bool selected = (i == m_comp_sel);
        if (ImGui::Selectable(m_compositions[i].name.c_str(), selected))
        {
            // store previous comp, then load new //
            _save_actions_to_comp();
            m_comp_sel = i;
            _load_actions_from_comp();
        }
        ImGui::PopID();
    }
    
    auto &      comp    = m_compositions[m_comp_sel];


    //  2.  COMPOSITION NAME...
    {
        ImGui::TextUnformatted("Name:");
        ImGui::SetNextItemWidth( -FLT_MIN );
        if ( ImGui::InputText("##Composition_Name", &comp.name) )
        {
            if ( comp.name.size() > ms_COMPOSITION_NAME_LIMIT )                  { comp.name.resize(ms_COMPOSITION_NAME_LIMIT); }
        }
    }

    //  3.  COMPOSITION DESCRIPTION...
    {
        ImGui::TextUnformatted("Description:");
        if ( ImGui::InputTextMultiline("##Composition_Description",     &comp.description, { -FLT_MIN, ms_COMPOSITION_DESCRIPTION_FIELD_HEIGHT }) )
        {
            if ( comp.description.size() > ms_COMPOSITION_DESCRIPTION_LIMIT )    { comp.description.resize(ms_COMPOSITION_DESCRIPTION_LIMIT); }
        }
    }
    
    
    
    return;
}


//  "_draw_action_selector"
//
void ActionComposer::_draw_action_selector(void)
{
    //  1.  FILTER SEARCH BOX...
    //
    ImGui::BeginDisabled(true);
        ImGui::SetNextItemWidth(-FLT_MIN);
        if ( ImGui::InputTextWithHint("##flt", "filter", m_filter.InputBuf, IM_ARRAYSIZE(m_filter.InputBuf)) )
            { m_filter.Build(); }
    ImGui::EndDisabled();
        
        
    ImGui::Separator();


    //  2.  TOOL-BAR WIDGETS...
    if ( ImGui::Button("+ Add") ) {
        m_actions->emplace_back();
        m_sel = static_cast<int>(m_actions->size()) - 1;
    }
    
    ImGui::SameLine();
    
    ImGui::BeginDisabled(m_sel <= 0);
        if ( ImGui::Button("Prev") )        { m_sel = std::max(m_sel - 1, 0); }
    ImGui::EndDisabled();
    
    ImGui::SameLine(0.0f, ms_TOOLBAR_SELECTABLE_SEP);
    
    ImGui::BeginDisabled( m_sel < 0 || m_sel >= (int)m_actions->size() - 1 );
        if ( ImGui::Button("Next") )        { m_sel = std::min(m_sel + 1, (int)m_actions->size() - 1); }
    ImGui::EndDisabled();

    ImGui::Separator();
    ImGui::Separator();
    
    
    this->_draw_selector_table();
    
    
    return;
}


//  "_draw_selector_table"
//
inline void ActionComposer::_draw_selector_table(void)
{
    static constexpr ImGuiTableFlags            TABLE_FLAGS         = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg;
    static constexpr ImGuiTableColumnFlags      C0_FLAGS            = ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_WidthFixed;
    static constexpr ImGuiTableColumnFlags      C1_FLAGS            = ImGuiTableColumnFlags_WidthStretch;
    static constexpr ImGuiTableColumnFlags      C2_FLAGS            = ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_WidthFixed;
    //
    static constexpr ImGuiSelectableFlags       SELECTABLE_FLAGS    = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap;
    ImGuiListClipper                            clip;

    // ── 3. list body (table) ────────────────────────────────────────
    //      fixed: [handle] | stretchy name | fixed delete
    //
    if ( ImGui::BeginTable( "##actions",   3,    TABLE_FLAGS,    ImVec2(0, -1)) )           // full remaining height
    {
        ImGui::TableSetupColumn( "H",       C0_FLAGS,     ImGui::GetFrameHeight()   );      // ~ square
        ImGui::TableSetupColumn( "Name",    C1_FLAGS                                );
        ImGui::TableSetupColumn( "Del",     C2_FLAGS,     ImGui::GetFrameHeight()   );
        //  ImGui::TableHeadersRow();        // optional – draws thin top line

        clip.Begin(static_cast<int>(m_actions->size()));
        while ( clip.Step() )
        {
            for (int i = clip.DisplayStart; i < clip.DisplayEnd; ++i)
            {
                const bool  selected    = (m_sel == i);
                
                if ( !m_filter.PassFilter( (*m_actions)[i].name.c_str() ) )      { continue; }


                ImGui::PushID(i);
                ImGui::TableNextRow();

                //      3.1.    DRAG / DROP HANDLE...
                ImGui::TableSetColumnIndex(0);
                //if ( ImGui::SmallButton(this->ms_DRAG_DROP_HANDLE) ) {}   // larger hit-box
                utl::SmallCButton(this->ms_DRAG_DROP_HANDLE, 0x00000000);
                
                if ( ImGui::IsItemActive() && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoPreviewTooltip) )
                {
                    ImGui::SetDragDropPayload("ACTION_ROW", &i, sizeof i);
                    ImGui::EndDragDropSource();
                }
                if ( ImGui::BeginDragDropTarget() )
                {
                    if ( const ImGuiPayload * p = ImGui::AcceptDragDropPayload("ACTION_ROW") )
                        { _reorder(*(int*)p->Data, i); }
                    ImGui::EndDragDropTarget();
                }


                //      3.2.    SELECTIBLE WIDGET...
                ImGui::TableSetColumnIndex(1);
                if ( ImGui::Selectable((*m_actions)[i].name.c_str(), selected, SELECTABLE_FLAGS) )     { m_sel = i; }


                //      3.3.    DELETE BUTTON...
                ImGui::TableSetColumnIndex(2);
                if ( utl::SmallCButton(this->ms_DELETE_BUTTON_HANDLE) ) //  if ( ImGui::SmallButton("X") )
                {
                    m_actions->erase(m_actions->begin() + i);
                    if ( m_sel >= i )       { m_sel = std::max(m_sel - 1, (int)m_actions->size() - 1); }
                    ImGui::PopID();
                    clip.End();          // ← ensure clipper is closed
                    ImGui::EndTable();   // close the table
                    return;              // abort safely after mutation
                }
            
            
            

                ImGui::PopID();
            }
        }
        clip.End();
        ImGui::EndTable();
    }

        
    return;
}


//  "_draw_action_inspector"
//
void ActionComposer::_draw_action_inspector(void)
{
    if ( m_sel < 0 || m_sel >= m_actions->size() )         { ImGui::TextDisabled("No Selection..."); return; }
    
    Action &        a           = (*m_actions)[m_sel];
    int             type_int    = static_cast<int>( a.type );

    
    //  1.  ACTION NAME...
    {
        this->label("Name:");
        if ( ImGui::InputText("##Action_Name", &a.name, ImGuiInputTextFlags_None) )
        {
            if ( a.name.size() > ms_ACTION_NAME_LIMIT )     { a.name.resize(ms_ACTION_NAME_LIMIT); }
        }
    }
    
    //  2.  ACTION DESCRIPTION...
    {
        this->label("Description:");
        if ( ImGui::InputTextMultiline("##Action_Description", &a.descr, { -FLT_MIN, ms_ACTION_DESCRIPTION_FIELD_HEIGHT }) )
        {
            if ( a.descr.size() > ms_ACTION_DESCRIPTION_LIMIT )     { a.descr.resize(ms_ACTION_DESCRIPTION_LIMIT); }
        }
    }
    
    //  3.  ACTION TYPE...
    {
        this->label("Type:");
        if ( ImGui::Combo("##type", &type_int, ms_ACTION_TYPE_NAMES.data(), static_cast<int>( ActionType::COUNT )) )
            { a.type    = static_cast<ActionType>(type_int); }
    }


    ImGui::Separator();
    
    
    //  4.  DRAW THE TYPE-SPECIFIC UI-WIDGETS...
    this->_dispatch_action_ui(a);
    
    
    
    return;
}


//  "_draw_renderer_visuals"
//
void ActionComposer::_draw_renderer_visuals(void)
{
    ImDrawList *        dl                  = ImGui::GetForegroundDrawList();
    int                 wx{},   wy{};
    auto                draw_rect_marker    = [&](ImVec2 p) {
        ImVec2 p_min = { p.x - ms_VIS_RECT_HALF, p.y - ms_VIS_RECT_HALF };
        ImVec2 p_max = { p.x + ms_VIS_RECT_HALF, p.y + ms_VIS_RECT_HALF };
        dl->AddRect(p_min, p_max, ms_VIS_COLOR, 0.0f, 0, ms_VIS_THICK);
    };


    //  CASE 0 :    EARLY OUT IF:   (1) VISIBILITY IS DISABLED.     (2) NO ACTION SELECTED...
    //
    if ( !m_render_visuals )                                             { return; }
    if ( m_sel < 0 || m_sel >= static_cast<int>(m_actions->size()) )    { return; }


    //  CASE 1 :    EARLY OUT IF ACTION IS NOT A CURSOR-MOTION...
    const Action &      act             = (*m_actions)[m_sel];
    if ( act.type != ActionType::CursorMove )                           { return; }



    //  1.  CONVERT GUI-COORDS. TO SCREEN-COORDS.
    glfwGetWindowPos(m_glfw_window, &wx, &wy);
    ImVec2              begin           = { act.cursor.first.x + wx,    act.cursor.first.y + wy };
    ImVec2              end             = { act.cursor.last.x  + wx,    act.cursor.last.y  + wy };
        

    //  2.  RENDER THE VISUALS...
    dl->AddLine( begin, end, ms_VIS_COLOR, ms_VIS_THICK );
    //
    draw_rect_marker( begin );
    draw_rect_marker( end   );
    
    
    return;
}






// *************************************************************************** //
//
//
//
// *************************************************************************** //
//          SECONDARY UI FUNCTIONS...
// *************************************************************************** //

//  "_draw_controlbar"
//
void ActionComposer::_draw_controlbar(void)
{
    static constexpr const char *   uuid            = "##Editor_Controls_Columns";
    static constexpr int            NC              = 8;
    static ImGuiOldColumnFlags      COLUMN_FLAGS    = ImGuiOldColumnFlags_None;
    static ImVec2                   WIDGET_SIZE     = ImVec2( -1,  32 );
    static ImVec2                   BUTTON_SIZE     = ImVec2( 22,   WIDGET_SIZE.y );
    //
    constexpr ImGuiButtonFlags      BUTTON_FLAGS    = ImGuiOldColumnFlags_NoPreserveWidths;
    
    //this->S.PushFont( Font::Small );
   
    
    
   
   
    //  BEGIN COLUMNS...
    //
    ImGui::Columns(NC, uuid, COLUMN_FLAGS);
    //
    //
    //
        //  1.  PLAY / PAUSE...
        ImGui::TextDisabled("Controls:");
        //
        ImGui::SetNextItemWidth( WIDGET_SIZE.x );
        ImGui::BeginDisabled( m_actions->empty() );
            if ( !this->is_running() )
            {
                if ( ImGui::Button("Run All", WIDGET_SIZE) ) {
                    m_play_index        = (m_sel >= 0           ? m_sel             : 0);
                    m_is_running        = !m_actions->empty();
                    m_state             = (m_actions->empty())   ? State::Idle       : State::Run;
                }
            }
            else
            {
                if ( utl::CButton("Stop", 0xFF453AFF, WIDGET_SIZE) ) {
                    this->reset_all();
                }
            }
        ImGui::EndDisabled();
    
    
    
        //  2.  RUN ONCE...
        ImGui::NextColumn();        ImGui::NewLine();
        ImGui::SetNextItemWidth( WIDGET_SIZE.x );
        ImGui::BeginDisabled( !this->is_running() && m_actions->empty() && (m_sel < 0) );
            if ( ImGui::Button("Run Once", WIDGET_SIZE) ) {
                m_play_index = m_sel;
                m_step_req   = true;             // drive exactly one action
            }
        ImGui::EndDisabled();
        


    
        //  3.  STEP BUTTON...
        ImGui::NextColumn();        ImGui::NewLine();
        ImGui::BeginDisabled( !this->is_running() || m_actions->size() < 1 );
            if ( ImGui::Button("Step", WIDGET_SIZE) )
            {
                m_play_index    = (m_play_index + 1) % (int)m_actions->size();
                m_sel           = m_play_index;          // highlight row that will run next
            }
        ImGui::EndDisabled();



        //  4.  [TOGGLE]    OVERVIEW...
        ImGui::NextColumn();
        ImGui::TextDisabled("Overlay:");
        //
        ImGui::SetNextItemWidth( BUTTON_SIZE.x );
        ImGui::Checkbox("##ActionComposer_OverlayToggle",           &m_show_overlay);



        //  5.  [TOGGLE]    RENDER VISUALS...
        ImGui::NextColumn();
        ImGui::TextDisabled("Render Visuals:");
        //
        ImGui::SetNextItemWidth( BUTTON_SIZE.x );
        ImGui::Checkbox("##ActionComposer_RenderVisualsToggle",     &m_render_visuals);



        //  6.  INFO...
        ImGui::NextColumn();
        ImGui::TextDisabled("Info:");
        //
        if ( this->is_running() )       { ImGui::Text("running: %d / %zu", m_play_index + 1, m_actions->size()); }
        else                            { ImGui::Text("idle"); }
        
        



        
        //  7.  EMPTY SPACES FOR LATER...
        for (int i = ImGui::GetColumnIndex(); i < NC - 1; ++i) {
            ImGui::Dummy( ImVec2(0,0) );    ImGui::NextColumn();
        }


        //  X.  MOUSE COORDINATES...
        ImGui::TextDisabled("Mouse Position:");
        //ImVec2 mpos = ImGui::GetMousePos();
        ImVec2 mpos = this->get_cursor_pos();
        
        ImGui::Text("(%.1f , %.1f)",    mpos.x, mpos.y);     //  Live cursor read-out in the same units we feed to glfwSetCursorPos...
    //
    //
    //
    ImGui::Columns(1);      //  END COLUMNS...
    
    
    //this->S.PopFont();
   
    return;
}


//  "_draw_toolbar"
//
void ActionComposer::_draw_toolbar(void)
{
    return;
}


//  "_draw_overlay"
//
void ActionComposer::_draw_overlay(void)
{
    static constexpr ImGuiWindowFlags       flags           = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    ImVec2                                  mpos            = ImGui::GetMousePos();
    ImVec2                                  pos             = mpos + ms_OVERLAY_OFFSET;

    if ( !m_show_overlay )                                          { return; }
    if ( glfwGetWindowAttrib(m_glfw_window, GLFW_FOCUSED) == 0 )    { return; }


    //  1.  CACHE CURRENT MONITOR DATA...
    _refresh_monitor_cache(mpos);


    //  2.  CLAMP INSIDE CACHED MONITOR WORKSPACE...
    pos.x       = std::clamp( pos.x,        m_monitor_bounds.Min.x,       m_monitor_bounds.Max.x - m_overlay_size.x );
    pos.y       = std::clamp( pos.y,        m_monitor_bounds.Min.y,       m_monitor_bounds.Max.y - m_overlay_size.y );



    ImGui::SetNextWindowBgAlpha(ms_OVERLAY_ALPHA);
    ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    //  ImGuiViewport * vp = ImGui::GetMainViewport();
    //  ImGui::SetNextWindowViewport(vp->ID);               //  pin to main viewport
        if ( ImGui::Begin("##ac_overlay", nullptr, flags) )
        {
            this->_dispatch_overlay_content();
            m_overlay_size  = ImGui::GetWindowSize();
        }
        
    ImGui::End();
    return;
}


//  "_dispatch_overlay_content"
inline void ActionComposer::_dispatch_overlay_content(void)
{
    this->_overlay_ui_none();

    switch (m_state)
    {
        //  1.  "Run" STATE...
        case State::Run :  {
            this->_overlay_ui_run();
            break;
        }
        
        //  2.  "Capture" STATE...
        case State::Capture :  {
            this->_overlay_ui_capture();
            break;
        }
        
        //  DEFAULT...
        default :   { break; }
    }
    
    return;
}

//  "_overlay_ui_none"
inline void ActionComposer::_overlay_ui_none(void)
{
    static constexpr const char *   FMT_STRING              = "State: %s. \t Mouse: (%.0f, %.0f)";
    //
    static const char *             state_str               = nullptr;
    static State                    state_cache             = static_cast<State>( static_cast<int>(this->m_state) - 1 );
    //const ImVec2                    mpos                    = ImGui::GetMousePos();
    const ImVec2                    mpos                    = this->get_cursor_pos();
            
    if ( state_cache != this->m_state ) {
        state_cache     = static_cast<State>( this->m_state );
        state_str       = ms_COMPOSER_STATE_NAMES[ static_cast<size_t>(this->m_state) ];
    }
    
    ImGui::Text(FMT_STRING, state_str, mpos.x, mpos.y);
    ImGui::Separator();
    
    return;
}

//  "_overlay_ui_run"
inline void ActionComposer::_overlay_ui_run(void)
{
    ImGui::TextUnformatted("Running test... (press ESC to CANCEL)");
    return;
}

//  "_overlay_ui_capture"
inline void ActionComposer::_overlay_ui_capture(void)
{
    ImGui::TextUnformatted("Capturing... (press ESC to ENTER)");
    return;
}






// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                                  GENERAL FUNCTIONS...
// *************************************************************************** //

//  "_drive_execution"
//
void ActionComposer::_drive_execution(void)
{
    //  1.  ADVANCE TO THE CURRENT ACTION...
    m_executor.update();

    //  2.  CHECK IF WE ARE READY TO PROCEED...
    if ( !m_executor.busy() && this->is_running() )
    {
        if ( m_play_index < 0 || m_play_index >= static_cast<int>(m_actions->size()) ) {
            m_state         = State::Idle;
            m_is_running    = false;
            m_step_req      = false;
            m_play_index    = -1;
            return;
        }

        Action &    act     = (*m_actions)[m_play_index];
        this->_dispatch_execution(act);


        //  3.  PREPARE FOR THE NEXT INDEX...=
        //  ++m_play_index;
        //
        if ( !this->is_running() )      { m_play_index = -1;    }               //  RUNNING ONLY ONCE...
        else                            { ++m_play_index;       }               //  RUNNING **ALL** ACTIONS...
            
        if ( this->is_running() && m_play_index >= static_cast<int>(m_actions->size()) ) {
            m_state         = State::Idle;
            m_is_running    = false;
            m_play_index    = -1;
        }
        m_step_req = false;
        m_sel      = m_play_index;
    }
    
    return;
}


//  "_dispatch_execution"
//
inline void ActionComposer::_dispatch_execution(Action & act)
{
    
    switch (act.type)
    {
        //  1.  CURSOR MOVEMENT...
        case ActionType::CursorMove: {
            m_executor.start_cursor_move(
                m_glfw_window,
                act.cursor.first,           // <- use stored begin coordinate
                act.cursor.last,
                act.cursor.duration
            );
            break;
        }
        
        //  2.  SINGLE MOUSE CLICK...
        case ActionType::MouseClick: {
            break;
        }
        
        //  3.  MOUSE PRESS...
        case ActionType::MousePress: {
            m_executor.start_mouse_press( act.press.left_button );
            break;
        }
        
        //  4.  MOUSE RELEASE...
        case ActionType::MouseRelease: {
            m_executor.start_mouse_release( act.release.left_button );
            break;
        }
        
        //  5.  MOUSE DRAG...
        case ActionType::MouseDrag: {
            break;
        }

        //  6.  HOTKEY PRESS...
        case ActionType::Hotkey: {
            m_executor.start_button_action(
                m_glfw_window,
                act.hotkey.key,
                act.hotkey.ctrl,
                act.hotkey.shift,
                act.hotkey.alt
            );
            break;
        }

        //  ?.  DEFAULT...
        default: {
            break;
        }
    }
        
    
    return;
}






// *************************************************************************** //
//
//
//
// *************************************************************************** //
//      ACTION-UI FUNCTIONS...
// *************************************************************************** //

//  "_dispatch_action_ui"
//
inline void ActionComposer::_dispatch_action_ui(Action & a)
{
    
    switch(a.type)
    {
        case ActionType::CursorMove:        { this->_ui_cursor_move(a);         break;       }
        //
        case ActionType::MouseClick:        { this->_ui_mouse_click(a);         break;       }
        case ActionType::MousePress:        { this->_ui_mouse_press(a);         break;       }
        case ActionType::MouseRelease:      { this->_ui_mouse_release(a);       break;       }
        case ActionType::MouseDrag:         { this->_ui_mouse_drag(a);          break;       }
        //
        case ActionType::Hotkey:            { this->_ui_hotkey(a);              break;       }
        default:                            { break; }
    }
    
    
    
    
    return;
}
    
    
//  "_ui_cursor_move"
//
inline void ActionComposer::_ui_cursor_move(Action & a)
{

    this->label("Begin:");
    ImGui::PushID("ActionComposed_CursorMove_Begin");
        ImGui::DragFloat2   ("##init",          (float*)&a.cursor.first,        1,          0,          FLT_MAX,        "%.f");
        ImGui::SameLine();
        if ( ImGui::SmallButton("auto") )           { _begin_cursor_capture(&a.cursor.first); }     // start capture for Begin
    ImGui::PopID();
    
    
    this->label("End:");
    ImGui::PushID("ActionComposed_CursorMove_End");
        ImGui::DragFloat2   ("##final",         (float*)&a.cursor.last,         1,          0,          FLT_MAX,        "%.f");
        ImGui::SameLine();
        if ( ImGui::SmallButton("auto") )           { _begin_cursor_capture(&a.cursor.last); }     // start capture for Begin
    ImGui::PopID();
    
    
    this->label("Duration:");
    ImGui::DragFloat    ("##duration",      &a.cursor.duration,             0.05f,      0.0f,       10,             "%.2f s");
    
    
    
    return;
}


//  "_ui_mouse_click"
//
inline void ActionComposer::_ui_mouse_click(Action & a)
{
    label("Button:");
    const char * btn_names[]{"Left", "Right"};
    int b = a.click.left_button ? 0 : 1;
    if (ImGui::Combo("##btn", &b, btn_names, 2))
        a.click.left_button = (b == 0);

    return;
}


//  "_ui_mouse_press"
//
inline void ActionComposer::_ui_mouse_press(Action & a)
{
    label("Button:");
    const char* names[]{"Left","Right"};
    int b = a.press.left_button ? 0 : 1;
    if (ImGui::Combo("##press_btn", &b, names, 2))
        a.press.left_button = (b==0);
}


//  "_ui_mouse_release"
//
inline void ActionComposer::_ui_mouse_release(Action & a)
{
    label("Button:");
    const char* names[]{"Left","Right"};
    int b = a.release.left_button ? 0 : 1;
    if (ImGui::Combo("##rel_btn", &b, names, 2))
        a.release.left_button = (b==0);
}


//  "_ui_mouse_drag"
//
inline void ActionComposer::_ui_mouse_drag(Action & a)
{
    label("Begin:");
    ImGui::DragFloat2("##drag_from", (float*)&a.drag.from, 1.f, 0.f, FLT_MAX, "%.0f");
    ImGui::SameLine();
    if (ImGui::SmallButton("Auto##drag_from"))
        _begin_cursor_capture(&a.drag.from);

    label("End:");
    ImGui::DragFloat2("##drag_to", (float*)&a.drag.to, 1.f, 0.f, FLT_MAX, "%.0f");
    ImGui::SameLine();
    if (ImGui::SmallButton("Auto##drag_to"))
        _begin_cursor_capture(&a.drag.to);

    label("Duration:");
    ImGui::DragFloat("##drag_dur", &a.drag.duration, 0.05f, 0.f, 10.f, "%.2f s");

    label("Button:");
    const char* btn[]{"Left", "Right"};
    int b = a.drag.left_button ? 0 : 1;
    if (ImGui::Combo("##drag_btn", &b, btn, 2))
        a.drag.left_button = (b == 0);
}


//  "_ui_hotkey"
//
inline void ActionComposer::_ui_hotkey(Action & a)
{
    ImGui::Text(        "Key: %d",      a.hotkey.key);      //  Quick placeholder
    //
    ImGui::Checkbox(    "Ctrl",         &a.hotkey.ctrl);    ImGui::SameLine();
    ImGui::Checkbox(    "Shift",        &a.hotkey.shift);   ImGui::SameLine();
    ImGui::Checkbox(    "Alt",          &a.hotkey.alt);
    
    return;
}






// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                      OTHER UTILITY FUNCTIONS...
// *************************************************************************** //

//  "_begin_cursor_capture"
//
inline bool ActionComposer::_begin_cursor_capture(ImVec2 * destination)
{
    if (m_state == State::Run)  { return false; }
    
    m_state             = State::Capture;
    m_capture_dest      = destination;
    return true;
}


//  "_update_capture"
//
inline void ActionComposer::_update_capture(void)
{
    ImVec2          mpos        = ImVec2(-1.0f, -1.0f);
    ImGuiIO &       io          = ImGui::GetIO();
    
    if ( m_state != State::Capture || m_capture_dest == nullptr )   { return; }


    //  ESC key ends capture...
    if ( ImGui::IsKeyPressed(ImGuiKey_Escape) )
    {
        m_state             = State::Idle;
        m_capture_dest      = nullptr;
        return;
    }

    // Query GLFW for window-relative cursor coordinates --------------------//
    mpos                        = this->get_cursor_pos();
    m_capture_dest->x           = static_cast<float>(mpos.x);
    m_capture_dest->y           = static_cast<float>(mpos.y);
    return;
}


//  "_refresh_monitor_cache"
//
//      Update m_active_monitor / m_monitor_bounds when the cursor enters a new
//      monitor.  Expects |global| = screen-space cursor coordinates.
//
inline void ActionComposer::_refresh_monitor_cache(ImVec2 global)
{
    int mon_count;
    GLFWmonitor** monitors = glfwGetMonitors(&mon_count);

    for (int i = 0; i < mon_count; ++i)
    {
        int mx, my, mw, mh;
        glfwGetMonitorWorkarea(monitors[i], &mx, &my, &mw, &mh);
        if (global.x >= mx && global.x < mx + mw &&
            global.y >= my && global.y < my + mh)
        {
            if (monitors[i] != m_active_monitor)      // changed monitor
            {
                m_active_monitor = monitors[i];
                m_monitor_bounds = ImRect({(float)mx,        (float)my},
                                          {(float)(mx+mw),   (float)(my+mh)});
            }
            return;
        }
    }
    
    /* fallback: first monitor if none matched */
    if (m_active_monitor == nullptr && mon_count > 0)
    {
        int mx, my, mw, mh;
        glfwGetMonitorWorkarea(monitors[0], &mx, &my, &mw, &mh);
        m_active_monitor = monitors[0];
        m_monitor_bounds = ImRect({(float)mx,        (float)my},
                                  {(float)(mx+mw),   (float)(my+mh)});
    }
}




// 
// 
//
// *************************************************************************** //
// *************************************************************************** //   END "ActionExecutor".














// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" "ui" NAMESPACE.











// *************************************************************************** //
// *************************************************************************** //
//
//  END.

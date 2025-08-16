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
#include "app/state/state.h"
#include "app/state/_types.h"



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
ActionComposer::ActionComposer(app::AppState & src)
    : CBAPP_STATE_NAME(src)
    , m_detview_window( std::make_unique<app::WinInfo>() )
{
    m_actions       = &m_compositions.front().actions;   // initial seat
}


//  "initialize"
//
void ActionComposer::initialize(void)
{
    namespace           fs          = std::filesystem;
    app::WinInfo &      win_info    = *m_detview_window;
    
    if ( this->m_initialized )          { return;                       }
    else                                { this->m_initialized = true;   }
    
    
    //  1.  CREATE WinInfo OBJECT TO STORE WINDOW IN DETVIEW...
    win_info                        = {
        "Functional Testing",
        ImGuiWindowFlags_None | ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY,
        true,
        nullptr
    };
    
    
    //  2.  LOAD DEFAULT TESTS FROM FILE...
    if ( !this->m_filepath.empty() && fs::exists(this->m_filepath) && fs::is_regular_file(this->m_filepath) )
    {
        this->S.m_logger.debug( std::format("ActionComposer | loading from default file, \"{}\"", this->m_filepath.string()) );
        this->load_from_file(this->m_filepath);
    }
    else {
        this->S.m_logger.debug( std::format("ActionComposer | unable to load default file, \"{}\"", this->m_filepath.string()) );
    }
    
    return;
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
void ActionComposer::Begin([[maybe_unused]] const char * ,     [[maybe_unused]] bool * ,    [[maybe_unused]] ImGuiWindowFlags )
{

    if ( m_detview_window->open ) {
        ImGui::Begin( m_detview_window->uuid.c_str(), nullptr, m_detview_window->flags );
            this->Begin_IMPL();
        ImGui::End();
    }
    
    return;
}


//  "Begin_IMPL"
//
void ActionComposer::Begin_IMPL(void)
{
    const bool      busy    = m_saving || m_loading;
    
    
    //  1.  UPDATE INPUT-QUERY FUNCTIONS...
    switch (this->m_state)
    {
        case State::Idle : {                  //  1.  IDLE...
            break;
        }
    
        case State::Run : {                   //  2.  RUNNING...
            m_executor.m_key_manager.Begin();
            break;
        }
    
        case State::MouseCapture : {          //  3.  MOUSE-CAPTURING...
            this->_update_mouse_capture();
            break;
        }
    
        case State::KeyCapture : {            //  4.  KEY-CAPTURING...
            this->_update_key_capture();
            break;
        }
        
        //  DEFAULT CASE...
        default : { break; }
    }
    
    //  print m_executor.m_key_manager.m_active
    //  print m_key_manager.m_active
    
    //  this->_update_mouse_capture();
    //  this->_update_key_capture();
    
    
    
    //  2.  DRAW THE "CONTROL-BAR" UI-INTERFACE...
    this->_draw_controlbar();
    
    
    //  3.  DRAW THE "BROWSER" UI-INTERFACE...
    this->draw_all();
    
    
    //  4.  DRIVE EXECUTION OF THE ACTION COMPOSITION...
    if (m_state == State::Run) {
        this->_drive_execution();
    }
    this->_draw_overlay();
    this->_draw_renderer_visuals();
    
    
    //  5.  QUERY FILE DIALOG MENUS...
    if ( busy && (this->m_state != State::Run) ) {
        this->_file_dialog_handler();
    }
    
    //  S.highlight_hover_item();
    
    return;
}
    
    
//  "draw_all"
//
void ActionComposer::draw_all(void)
{
    using                       Font                            = app::AppState::Font;
    const bool                  allow_resize                    = ImGui::GetCurrentWindow()->DockTabIsVisible;
    
    
    
    //  0.  ALLOW / PREVENT RESIZING CHILD WINDOWS...
    if (allow_resize) {
        ms_COMPOSER_SELECTOR_FLAGS      = ms_COMPOSER_SELECTOR_FLAGS    | ImGuiChildFlags_ResizeX;
        ms_ACTION_SELECTOR_FLAGS        = ms_ACTION_SELECTOR_FLAGS      | ImGuiChildFlags_ResizeX;
    }
    else {
        ms_COMPOSER_SELECTOR_FLAGS      = ms_COMPOSER_SELECTOR_FLAGS    & ~ImGuiChildFlags_ResizeX;
        ms_ACTION_SELECTOR_FLAGS        = ms_ACTION_SELECTOR_FLAGS      & ~ImGuiChildFlags_ResizeX;
    }
    
    
    
    //  1.  COMPOSITION SELECTOR COLUMN...
    if ( m_show_composition_browser )
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize,  ms_CHILD_BORDER1);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding,    ms_CHILD_ROUND1);
        ImGui::PushStyleColor(ImGuiCol_ChildBg,             ms_CHILD_FRAME_BG1L);
            
            //  1.  DRAW THE TOP-MOST "COMPOSITION" BAR...
            ImGui::SetNextWindowSizeConstraints( ms_COMPOSER_SELECTOR_DIMS.limits.min, ms_COMPOSER_SELECTOR_DIMS.limits.max );
            ImGui::BeginChild("##ActionComposer_CompositionSelector", ms_COMPOSER_SELECTOR_DIMS.value, ms_COMPOSER_SELECTOR_FLAGS);
                this->_draw_composition_selector();
                ms_COMPOSER_SELECTOR_DIMS.value.x     = ImGui::GetItemRectSize().x;
            ImGui::EndChild();

        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);  //  ImGuiStyleVar_ChildBorderSize, ImGuiStyleVar_ChildRounding

        ImGui::SameLine();
    }
    
    
    
    //  2.  DRAW THE "BROWSER" UI-INTERFACE...
    S.PushFont(Font::Small);
    ImGui::BeginChild("##ActionComposer_Action_Browser",  {0, 0}, ImGuiChildFlags_Borders);
        //
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize,  ms_CHILD_BORDER1);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding,    ms_CHILD_ROUND1);
        ImGui::PushStyleColor(ImGuiCol_ChildBg,             ms_CHILD_FRAME_BG1L);
        //
        //
        //
            ImGui::SetNextWindowSizeConstraints( ms_ACTION_SELECTOR_DIMS.limits.min, ms_ACTION_SELECTOR_DIMS.limits.max );
            ImGui::BeginChild("##ActionComposer_Action_Selector",  ms_ACTION_SELECTOR_DIMS.value, ms_ACTION_SELECTOR_FLAGS);
                this->_draw_action_selector();
                ms_ACTION_SELECTOR_DIMS.value.x       = ImGui::GetItemRectSize().x;
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
    S.PopFont();
    
    
    return;
}






// *************************************************************************** //
//
//
//
// *************************************************************************** //
//          MAIN UI FUNCTIONS...
// *************************************************************************** //



// *************************************************************************** //
//          COMPOSITION UI...
// *************************************************************************** //

//  "_draw_composition_selector"
//
void ActionComposer::_draw_composition_selector(void)
{
    if ( m_comp_sel < 0 )       { m_show_composition_inspector = false; }



    //  3A.     SWITCH FROM  "INSPECTOR"    ===>    "TABLE"...
    if (m_show_composition_inspector)
    {
        if ( ImGui::Button("Back To Table") )
        {
            m_show_composition_inspector = false;
        }
    }
    //
    //
    //  3B.     SWITCH FROM  "TABLE"    ===>    "INSPECTOR"...
    else
    {
        //  1.  ADD A NEW COMPOSITION...
        if ( ImGui::Button("+ Add Composition") ) {
            _save_actions_to_comp();                       // persist current comp
            m_compositions.emplace_back();                 // push default
            int new_idx = static_cast<int>(m_compositions.size()) - 1;
            _load_actions_from_comp(new_idx);              // select & reset state
        }
        //
        //  ImGui::SameLine();
        //
        //  2.  DELETE CURRENT COMPOSITION...
        //      ImGui::BeginDisabled(m_compositions.size() < 1);
        //          if ( ImGui::Button("- Delete") )
        //          {
        //              _save_actions_to_comp();                       // persist before erase
        //              m_compositions.erase(m_compositions.begin() + m_comp_sel);
        //              int next = std::clamp( m_comp_sel, 0, static_cast<int>(m_compositions.size()) - 1 );
        //              _load_actions_from_comp(next);
        //          }
        //      ImGui::EndDisabled();
        //
        ImGui::SameLine();
        
        //  3.  EDIT THE CURRENT COMPOSITION...
        ImGui::BeginDisabled( m_comp_sel < 0 );
            if ( ImGui::Button("Edit") )
            {
                m_show_composition_inspector = true;
            }
        ImGui::EndDisabled();
    
    }
    ImGui::Separator();



    //  2.  COMPOSITION INSPECTOR...
    if ( m_show_composition_inspector )
    {
        auto &      comp    = m_compositions[m_comp_sel];
        this->_draw_composition_inspector(comp);
    }
    //
    //  3.  COMPOSITION TABLE...
    else {
        _draw_composition_table();
    }

    
    
    return;
}


//  "_draw_composition_table"
//
void ActionComposer::_draw_composition_table(void)
{
    static constexpr const char *           COMPOSITION_DRAGDROP_ID     = "COMP_PAYLOAD";
    
    /// column & table flags
    static constexpr ImGuiTableFlags        TABLE_FLAGS                 = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg;
    static constexpr ImGuiTableColumnFlags  COL_HANDLE                  = ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_WidthFixed;
    static constexpr ImGuiTableColumnFlags  COL_NAME                    = ImGuiTableColumnFlags_WidthStretch;
    static constexpr ImGuiTableColumnFlags  COL_DELETE                  = ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_WidthFixed;
    static constexpr ImGuiSelectableFlags   SELECT_FLAGS                = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap;



    if ( !ImGui::BeginTable("##ActionComposer_Composition_Table", 3, TABLE_FLAGS, ImVec2(0, -1)) )      { return; }


    ImGui::TableSetupColumn( "Handle",      COL_HANDLE, ImGui::GetFrameHeight()     );
    ImGui::TableSetupColumn( "Name",        COL_NAME                                );
    ImGui::TableSetupColumn( "Delete",      COL_DELETE, ImGui::GetFrameHeight()     );

    ImGuiListClipper clip;
    clip.Begin( static_cast<int>(m_compositions.size()) );
    while ( clip.Step() )
    {
        for (int row = clip.DisplayStart; row < clip.DisplayEnd; ++row)
        {
            const bool is_selected = (row == m_comp_sel);

            ImGui::PushID(row);
            ImGui::TableNextRow();

            //  1.  DRAG/DROP HANDLE...
            ImGui::TableSetColumnIndex(0);
            utl::SmallCButton(ms_DRAG_DROP_HANDLE, 0x00000000);
            //
            if ( ImGui::IsItemActive() && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoPreviewTooltip) ) {
                ImGui::SetDragDropPayload(COMPOSITION_DRAGDROP_ID, &row, sizeof row);
                ImGui::EndDragDropSource();
            }
            if ( ImGui::BeginDragDropTarget() ) {
                if ( const ImGuiPayload* p = ImGui::AcceptDragDropPayload(COMPOSITION_DRAGDROP_ID) )
                {
                    _reorder_composition(*static_cast<const int*>(p->Data), row);

                    /* ── abort drawing after the vector changed ────────────────── */
                    ImGui::EndDragDropTarget();
                    ImGui::PopID();
                    clip.End();
                    ImGui::EndTable();
                    return;                      // ← exit the function safely
                }
                ImGui::EndDragDropTarget();
            }


            //  2.  COMPOSITION SELECTABLE...
            ImGui::TableSetColumnIndex(1);
            if ( ImGui::Selectable(m_compositions[row].name.c_str(), is_selected, SELECT_FLAGS) )
            {
                _save_actions_to_comp();
                _load_actions_from_comp(row);
            }

            //  3.  RIGHT-CLICK CONTEXT MENU...
            if ( ImGui::BeginPopupContextItem("##ActionComposer_Composition_ContextMenu") )
            {
                //  3A.     "DUPLICATE" BUTTON.
                if ( ImGui::MenuItem("Duplicate") )
                {
                    _save_actions_to_comp();
                    m_compositions.insert(m_compositions.begin() + row + 1,
                                          m_compositions[row]);          // deep copy
                    _load_actions_from_comp(row + 1);
                    ImGui::EndPopup();
                    ImGui::PopID();
                    clip.End();
                    ImGui::EndTable();
                    return;
                }
                ImGui::EndPopup();
            }

            //  3.  DELETE BUTTON...
            ImGui::TableSetColumnIndex(2);
            if ( utl::SmallCButton(ms_DELETE_BUTTON_HANDLE, this->ms_DELETE_BUTTON_COLOR) )
            {
                _save_actions_to_comp();
                m_compositions.erase(m_compositions.begin() + row);
                _load_actions_from_comp(std::clamp(
                    row, 0, static_cast<int>(m_compositions.size()) - 1));
                ImGui::PopID();
                clip.End();
                ImGui::EndTable();
                return;
            }

            ImGui::PopID();
        }
    }

    clip.End();
    ImGui::EndTable();
    
    
    return;
}


//  "_draw_composition_inspector"
//
void ActionComposer::_draw_composition_inspector(Composition & comp)
{

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



// *************************************************************************** //
//          ACTION UI...
// *************************************************************************** //

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
    
    
    this->_draw_action_table();
    
    
    return;
}


//  "_draw_action_table"
//
inline void ActionComposer::_draw_action_table(void)
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

        clip.Begin( static_cast<int>(m_actions->size()) );
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
                utl::SmallCButton(this->ms_DRAG_DROP_HANDLE, 0x00000000);
                if ( ImGui::IsItemActive() && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoPreviewTooltip) ) {
                    ImGui::SetDragDropPayload("ACTION_ROW", &i, sizeof i);
                    ImGui::EndDragDropSource();
                }
                if ( ImGui::BeginDragDropTarget() ) {
                    if ( const ImGuiPayload * p = ImGui::AcceptDragDropPayload("ACTION_ROW") )
                        { _reorder(*(int*)p->Data, i); }
                    ImGui::EndDragDropTarget();
                }


                //      3.2A.   SELECTIBLE WIDGET...
                ImGui::TableSetColumnIndex(1);
                if ( ImGui::Selectable((*m_actions)[i].name.c_str(), selected, SELECTABLE_FLAGS) )     { m_sel = i; }
                //
                //      3.2B.   RIGHT-CLICK CONTEXT MENU...
                if ( ImGui::BeginPopupContextItem("##ActionComposer_ActionRowContextMenu") ) {        // opens on RMB...
                    if ( ImGui::MenuItem("Duplicate") ) {   // duplicate current element and insert right below...
                        Action copy               = (*m_actions)[i];           // deep copy
                        m_actions->insert(m_actions->begin() + i + 1, copy);
                        m_sel                     = i + 1;                     // highlight new row
                    }
                    ImGui::EndPopup();
                }
                

                //      3.3.    DELETE BUTTON...
                ImGui::TableSetColumnIndex(2);
                if ( utl::SmallCButton(ms_DELETE_BUTTON_HANDLE, this->ms_DELETE_BUTTON_COLOR) )
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
    if ( m_sel < 0 || m_sel >= static_cast<int>( m_actions->size() ) )      { ImGui::TextDisabled("No Selection..."); return; }
    
    Action &        a           = (*m_actions)[m_sel];
    int             type_int    = static_cast<int>( a.type );

    
    //  1.  ACTION TYPE...
    {
        this->label("Type:");
        if ( ImGui::Combo("##type", &type_int, ms_ACTION_TYPE_NAMES.data(), static_cast<int>( ActionType::COUNT )) )
            { a.type    = static_cast<ActionType>(type_int); }
    }
    
    //  2.  ACTION NAME...
    {
        this->label("Name:");
        if ( ImGui::InputText("##Action_Name", &a.name, ImGuiInputTextFlags_None) )
        {
            if ( a.name.size() > ms_ACTION_NAME_LIMIT )     { a.name.resize(ms_ACTION_NAME_LIMIT); }
        }
    }
    
    //  3.  ACTION DESCRIPTION...
    {
        this->label("Description:");
        if ( ImGui::InputTextMultiline("##Action_Description", &a.descr, { -FLT_MIN, ms_ACTION_DESCRIPTION_FIELD_HEIGHT }) )
        {
            if ( a.descr.size() > ms_ACTION_DESCRIPTION_LIMIT )     { a.descr.resize(ms_ACTION_DESCRIPTION_LIMIT); }
        }
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
    //  "viewport_for"
    auto                    viewport_for        = [](ImVec2 p) -> ImGuiViewport *
    {
        const ImGuiPlatformIO& pio = ImGui::GetPlatformIO();
        for (ImGuiViewport* vp : pio.Viewports)
            if (p.x >= vp->Pos.x && p.x < vp->Pos.x + vp->Size.x &&
                p.y >= vp->Pos.y && p.y < vp->Pos.y + vp->Size.y)
                return vp;
        return ImGui::GetMainViewport();
    };
    //
    //  "draw_rect"
    auto                    draw_rect           = [&] (ImDrawList * dl, ImVec2 c, const ImU32 color)
    {
        dl->AddRect({c.x - ms_VIS_RECT_HALF, c.y - ms_VIS_RECT_HALF},
                    {c.x + ms_VIS_RECT_HALF, c.y + ms_VIS_RECT_HALF}, color, 0.0f, 0, ms_VIS_THICK);
    };
    
    
    
    //  CASE 0 :    NOTHING-TO-SEE / NOTHING-TO-DRAW...
    if ( !m_render_visuals || S.m_glfw_window == nullptr )                              { return; }
    if ( m_sel < 0 || m_sel >= static_cast<int>(m_actions->size()) )                    { return; }

    const Action &          act                 = (*m_actions)[m_sel];
    const ImGuiID           first_ID            = act.cursor.first_pos.widget.id;
    const ImGuiID           last_ID             = act.cursor.last_pos.widget.id;
    
    
    if ( act.type != ActionType::CursorMove && act.type != ActionType::MouseDrag )      { return; }



    //  1.  FIGURE OUT LOCAL POINTS : A / B...
    int                     wx{},               wy{};
    ImVec2                  local_a             {};
    ImVec2                  local_b             {};
    ImVec2                  global_a{},         global_b{};
    ImGuiViewport *         vp_a                = nullptr;
    ImGuiViewport *         vp_b                = nullptr;
    ImDrawList *            dl_a                = nullptr;
    ImDrawList *            dl_b                = nullptr;
    const ImVec2 *          cap_ptr             = m_mouse_capture.dest;           // convenience alias


    if ( act.type == ActionType::CursorMove || act.type == ActionType::MouseDrag )
    {
        local_a = (&act.cursor.first  == cap_ptr && m_mouse_capture.active)
                    ? m_mouse_capture.live_local
                    : act.cursor.first;

        local_b = (&act.cursor.last   == cap_ptr && m_mouse_capture.active)
                    ? m_mouse_capture.live_local
                    : act.cursor.last;
    }


    //  2.  CONVERT     LOCAL   ===>    GLOBAL      COORDINATES...
    glfwGetWindowPos(S.m_glfw_window, &wx, &wy);
    global_a        = { local_a.x + wx, local_a.y + wy };
    global_b        = { local_b.x + wx, local_b.y + wy };
    
    
    //  3.  PICK VIEWPORTS AND GET DRAW-LISTS...
    vp_a            = viewport_for(global_a);
    vp_b            = viewport_for(global_b);
    dl_a            = ImGui::GetForegroundDrawList(vp_a);
    dl_b            = (vp_b == vp_a) ? dl_a : ImGui::GetForegroundDrawList(vp_b);


    //  4.  RENDER LINES AND RECTANGLES...
    dl_a->AddLine(global_a, global_b, ms_VIS_LINE_COLOR, ms_VIS_THICK);
    draw_rect(dl_a, global_a, ms_VIS_COLOR_A);
    
    if ( dl_b != dl_a )         { draw_rect(dl_b, global_b, ms_VIS_COLOR_B); }
    else                        { draw_rect(dl_a, global_b, ms_VIS_COLOR_B); }
    
    
    //  5.  DRAW WIDGET BOX...
    //  if ( !(0 > first_ID) )      { S.highlight_hover_item( act.cursor.first_pos.widget );    }
    //  if ( !(0 > last_ID) )       { S.highlight_hover_item( act.cursor.last_pos.widget );     }
    
    
    
    return;
}










// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                      CAPTURE FUNCTIONS...
// *************************************************************************** //


// *************************************************************************** //
//      1.  CURSOR CAPTURE...
// *************************************************************************** //

//  "get_cursor_pos"
//
ImVec2 ActionComposer::get_cursor_pos(void) {
    double cx = -1.0f;  double cy = -1.0f;
    glfwGetCursorPos(S.m_glfw_window, &cx, &cy);
    return ImVec2(cx, cy);
}
    
    
//  "_begin_mouse_capture"
//
bool ActionComposer::_begin_mouse_capture([[maybe_unused]] Action & act, WPos * dst, ImVec2 * destination)
{
    if ( m_state == State::Run )    { return false; }

    m_state                         = State::MouseCapture;
    m_mouse_capture.reset();                      // reset all fields
    
    GLFWwindow * hovered            = glfwGetCurrentContext();
    m_mouse_capture.active          = true;
    //
    //
    m_mouse_capture.dst             = dst;
    //
    //
    m_mouse_capture.dest            = destination;
    m_mouse_capture.target_window   = hovered ? hovered : S.m_glfw_window;
    
    if ( act.type == ActionType::CursorMove || act.type == ActionType::MouseDrag ) {
        m_mouse_capture.alt_dest    = (destination == &act.cursor.first)    ? &act.cursor.last     : &act.cursor.first;
    }
    
    m_mouse_capture.backup          = *destination;         // save for cancel
    return true;
}


//  "_update_mouse_capture"
//
inline void ActionComposer::_update_mouse_capture(void)
{
    if ( m_state != State::MouseCapture || !m_mouse_capture.active )   { return; }
    
    
    //  1.  INPUT-BLOCKER WINDOW...
    if ( this->m_allow_input_blocker )          { _draw_input_blocker(); }
    
    
    //  2.  CAPTURE DATA...
    want_capture_mouse_next_frame();                //  ← NEW (blocks UI input)
    m_mouse_capture.live_local                  = get_cursor_pos();
    //
    m_mouse_capture.dst_live_local.pos          = get_cursor_pos();
    m_mouse_capture.dst_live_local.widget       = S._get_item_under_cursor();
        
    
    //  3.  USE TAB TO SWITCH ENDPOINTS...
    if ( ImGui::IsKeyPressed(ImGuiKey_Tab, false) && m_mouse_capture.alt_dest ) {
        //  1.  commit current pos to the endpoint we were editing
        *m_mouse_capture.dest               = m_mouse_capture.live_local;

        //  2.  swap pointers so the other endpoint is now active
        std::swap( m_mouse_capture.dest, m_mouse_capture.alt_dest );


        //  3.  update backup for Esc‑cancel on the new endpoint
        m_mouse_capture.backup              = *m_mouse_capture.dest;

        //  4.  fall through: continue real‑time capture on new point
        
        
        
        //  ** NEW **
        m_mouse_capture.dst->pos            = m_mouse_capture.live_local;
        //
        std::swap( m_mouse_capture.dst, m_mouse_capture.dst_alt );
        //
        m_mouse_capture.dst_backup          = *m_mouse_capture.dst;
    }


    //  4.  ACCEPT...
    if ( ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsMouseClicked(ImGuiMouseButton_Left) )
    {
        *m_mouse_capture.dest           = m_mouse_capture.live_local;
        
        m_mouse_capture.dst->pos        = m_mouse_capture.live_local;
        m_mouse_capture.dst->widget     = S._get_item_under_cursor();
        
        //  const utl::HoverItem & hover    = m_mouse_capture.dst->widget;
        //
        //  std::cout << "ID:               : "     << hover.id                     << ".\n"
        //            << "Window            : "     << hover.window                 << ".\n"
        //            << "Viewport          : "     << hover.viewport               << ".\n"
        //            << "On Main Viewport  : "     << hover.on_main_viewport       << ".\n"
        //            << "Has-Rect          : "     << hover.has_rect               << ".\n"
        //            << "Clip-Min          : "     << "(" << hover.clip_min.x << ", " << hover.clip_min.y << ").\n"
        //            << "Clip-Max          : "     << "(" << hover.clip_max.x << ", " << hover.clip_max.y << ").\n"
        //            << std::endl;
        
        
        
        m_mouse_capture.active          = false;
        m_state                         = State::Idle;
        
        return;
    }
    

    //  5.  CANCEL...
    if ( ImGui::IsKeyPressed(ImGuiKey_Escape) ) {
        *m_mouse_capture.dest       = m_mouse_capture.backup;
        *m_mouse_capture.dst        = m_mouse_capture.dst_backup;
        m_mouse_capture.active      = false;
        m_state                     = State::Idle;
        return;
    }
    
    return;
}









// *************************************************************************** //
//      KEYBOARD INPUT CAPTURE...
// *************************************************************************** //

//  "_begin_key_capture"
//
bool ActionComposer::_begin_key_capture(HotkeyParams * dest)
{
    if (m_state == State::Run)      { return false; }
    
    m_state                 = State::KeyCapture;
    m_key_capture           = {};          // reset all
    m_key_capture.active    = true;
    m_key_capture.dest      = dest;
    m_key_capture.backup    = *dest;      // keep previous binding
    ImGui::SetNextFrameWantCaptureKeyboard(true);
    return true;
}


//  "_update_key_capture"
//
inline void ActionComposer::_update_key_capture(void)
{
    if (!m_key_capture.active)      { return; }

    ImGuiIO & io = ImGui::GetIO();
    
    
    //want_capture_keyboard_next_frame();

    //  Detect first key press (ignore repeats)
    for ( ImGuiKey k = ImGuiKey_NamedKey_BEGIN; k < ImGuiKey_NamedKey_END; k = (ImGuiKey)(k + 1) )
    {
        if ( ImGui::IsKeyPressed(k, false) )
        {
            m_key_capture.key_current = k;
            m_key_capture.ctrl  = io.KeyCtrl;
            m_key_capture.shift = io.KeyShift;
            m_key_capture.alt   = io.KeyAlt;
            m_key_capture.super = io.KeySuper;
        }
    }

    //  Accept when that key goes UP or user hits Enter
    if ( (m_key_capture.key_current != ImGuiKey_None    &&
         !ImGui::IsKeyDown(m_key_capture.key_current))  ||
         ImGui::IsKeyPressed(ImGuiKey_Enter) )
    {
        _accept_key_capture();
    }
    //
    //  Cancel on Esc
    else if (ImGui::IsKeyPressed(ImGuiKey_Escape))
    {
        _cancel_key_capture();
    }
    
    return;
}


//  "_accept_key_capture"
//
inline void ActionComposer::_accept_key_capture(void)
{
    if (m_key_capture.dest && m_key_capture.key_current != ImGuiKey_None)
    {
        m_key_capture.dest->key   = m_key_capture.key_current;
        m_key_capture.dest->ctrl  = m_key_capture.ctrl;
        m_key_capture.dest->shift = m_key_capture.shift;
        m_key_capture.dest->alt   = m_key_capture.alt;
    }
    m_key_capture.active    = false;
    m_state                 = State::Idle;
    return;
}


//  "_cancel_key_capture"
//
inline void ActionComposer::_cancel_key_capture(void)
{
    if ( m_key_capture.dest )   { *m_key_capture.dest = m_key_capture.backup; }
    m_key_capture.active    = false;
    m_state                 = State::Idle;
    return;
}






// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                      SERIALIZER FUNCTIONS...
// *************************************************************************** //

//  SERIALIZER ASSERTIONS...
//
//      1.      ImVec2.
static_assert( cblib::has_from_json<ImVec2>::value,                 "ImVec2 \"from_json\" NOT visible."                 );
static_assert( cblib::has_to_json<ImVec2>::value,                   "ImVec2 \"to_json\" NOT visible."                   );
//
//      2.      CursorMoveParams.
static_assert( cblib::has_from_json<CursorMoveParams>::value,       "CursorMoveParams \"from_json\" NOT visible."       );
static_assert( cblib::has_to_json<CursorMoveParams>::value,         "CursorMoveParams \"to_json\" NOT visible."         );
//
//      3.      Action.
static_assert( cblib::has_from_json<Action>::value,                 "Action \"from_json\" NOT visible."                 );
static_assert( cblib::has_to_json<Action>::value,                   "Action \"to_json\" NOT visible."                   );
//
//      4.      Composition_t.
static_assert( cblib::has_from_json<Composition_t>::value,          "Composition_t \"from_json\" NOT visible."          );
static_assert( cblib::has_to_json<Composition_t>::value,            "Composition_t \"to_json\" NOT visible."            );
               
               
               
//  "_file_dialog_handler"
//
void ActionComposer::_file_dialog_handler(void)
{
    namespace                   fs              = std::filesystem;
    std::optional<fs::path>     filepath        = std::nullopt;
    
    if ( m_saving )
    {
        //      DIALOG IS OPEN...
        if ( S.m_file_dialog.is_open() )        { return; }
        //
        //      FINISHED SAVING...
        else {
            m_saving        = false;
            filepath        = S.m_file_dialog.get_last_path().value_or("");
            if ( filepath )   {
                S.m_logger.info( std::format("Saved to file \"{}\"", filepath->string()) );
                this->save_to_file( filepath.value() );
            }
            else    { S.m_logger.warning("Failed to save file."); }
        }
    }
    
    if ( m_loading )
    {
        //      DIALOG IS OPEN...
        if ( S.m_file_dialog.is_open() )        { return; }
        //
        //      FINISHED SAVING...
        else {
            m_loading           = false;
            filepath            = S.m_file_dialog.get_last_path().value_or("");
            if ( filepath )   {
                S.m_logger.info( std::format("Loaded from file \"{}\"", filepath->string()) );
                this->load_from_file( filepath.value() );
            }
            else    { S.m_logger.warning("Failed to open file."); }
        }
    }
    

    return;
}
              



//  "save_to_file"
//
bool ActionComposer::save_to_file(const std::filesystem::path & path) const
{
    std::ofstream       f(path);
    nlohmann::json      j = { {"compositions", m_compositions} };
    
    //  CASE 0 :    FAILURE TO OPEN  "ifstream" OBJECT...
    if ( !f ) {
        S.m_logger.error( std::format("ActionComposer | failed to save JSON file: failed to create std::ofstream object from provided path \"{}\".", path.string()) );
        return false;
    }
    
    f << j.dump(2);
    return true;
}


//  "load_from_file"
//
bool ActionComposer::load_from_file(const std::filesystem::path & path)
{
    std::ifstream   f(path);
    
    //  CASE 0 :    FAILURE TO OPEN  "ifstream" OBJECT...
    if ( !f ) {
        S.m_logger.error( std::format("ActionComposer | failed to load JSON file: failed to create std::ifstream object from provided path \"{}\".", path.string()) );
        return false;
    }

    try {
        nlohmann::json j; f >> j;
        j.at("compositions").get_to(m_compositions);
    }
    catch (const std::exception & e) {
        S.m_logger.exception( std::format("ActionComposer | failed to load JSON file: caught std::exception while loading file.  TRACEBACK: \"{}\".", e.what()) );
        return false;
    }

    _load_actions_from_comp(0);
    return true;
}







// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                      OTHER UTILITY FUNCTIONS...
// *************************************************************************** //

//  "_draw_settings_menu"
//
void ActionComposer::_draw_settings_menu(void)
//  void ActionComposer::_draw_settings_menu([[maybe_unused]] popup::Context & ctx)
{
    //  1.  ACTIONS...
    ImGui::SeparatorText("Actions...");
    {
        //  1A.     RESET STATE.
        this->label("Reset State:", this->ms_SETTINGS_LABEL_WIDTH, this->ms_SETTINGS_WIDGET_WIDTH);
        if ( ImGui::Button("State", ms_SETTINGS_BUTTON_SIZE) )      { this->reset_state(); }
        
        //  1B.     RESET DATA.
        this->label("Reset State:", this->ms_SETTINGS_LABEL_WIDTH, this->ms_SETTINGS_WIDGET_WIDTH);
        if ( ImGui::Button("Data", ms_SETTINGS_BUTTON_SIZE) )       { this->reset_data(); }
        
        //  1C.     RESET ALL.
        this->label("Reset All:", this->ms_SETTINGS_LABEL_WIDTH, this->ms_SETTINGS_WIDGET_WIDTH);
        if ( ImGui::Button("All", ms_SETTINGS_BUTTON_SIZE) )        { this->reset_all(); }
    }
    
    

    //  2.  APP SETTINGS...
    ImGui::NewLine();
    ImGui::SeparatorText("Settings...");
    {
        //  1.  [TOGGLE]    SHOW OVERLAY...
        this->label("Overlay Window:",              this->ms_SETTINGS_LABEL_WIDTH,      this->ms_SETTINGS_WIDGET_WIDTH);
        ImGui::SetNextItemWidth( ms_SETTINGS_BUTTON_SIZE.x );
        ImGui::Checkbox("##ActionComposer_OverlayToggle",           &m_show_overlay);

        //  2.  [TOGGLE]    RENDER VISUALS...
        this->label("Render Helper Visuals:",       this->ms_SETTINGS_LABEL_WIDTH,      this->ms_SETTINGS_WIDGET_WIDTH);
        //
        ImGui::SetNextItemWidth( ms_SETTINGS_BUTTON_SIZE.x );
        ImGui::Checkbox("##ActionComposer_RenderVisualsToggle",     &m_render_visuals);

        //  3.  [TOGGLE]    INPUT BLOCKER...
        this->label("Enable Input-Blocker:",        this->ms_SETTINGS_LABEL_WIDTH,      this->ms_SETTINGS_WIDGET_WIDTH);
        //
        ImGui::SetNextItemWidth( ms_SETTINGS_BUTTON_SIZE.x );
        ImGui::Checkbox("##ActionComposer_EnableInputBlocker",      &m_allow_input_blocker);

        //  4.  [TOGGLE]    SHOW COMPOSITION BROWSER...
        this->label("Show Composition Browser:",    this->ms_SETTINGS_LABEL_WIDTH,      this->ms_SETTINGS_WIDGET_WIDTH);
        //
        ImGui::SetNextItemWidth( ms_SETTINGS_BUTTON_SIZE.x );
        ImGui::Checkbox("##ActionComposer_ShowCompositionBrowser",  &m_show_composition_browser);
    }



    //  3.  I/O OPERATIONS...
    ImGui::NewLine();
    ImGui::SeparatorText("Serialization...");
    {
        const bool      has_file            = this->has_file();
        bool            force_save_as       = false;
    
    
        //      1.      CURRENT FILE...
        this->label("Current File:",                this->ms_SETTINGS_LABEL_WIDTH,      this->ms_SETTINGS_WIDGET_WIDTH);
        ImGui::TextDisabled( "%s", (has_file) ? this->m_filepath.string().c_str() : ms_NO_ASSIGNED_FILE_STRING );
    
    
        //      2.      SAVE DIALOGUE...
        this->label("Save:",                        this->ms_SETTINGS_LABEL_WIDTH,      this->ms_SETTINGS_WIDGET_WIDTH);
        if ( ImGui::Button("Save", ms_SETTINGS_BUTTON_SIZE) )    {
            
            if (has_file)       { this->save_to_file( this->m_filepath ); }
            else                { force_save_as = true; }

        }
        
        
        //      3.      "SAVE AS..." DIALOGUE...
        this->label("Save As...:",                  this->ms_SETTINGS_LABEL_WIDTH,      this->ms_SETTINGS_WIDGET_WIDTH);
        if ( force_save_as || ImGui::Button("Save As...", ms_SETTINGS_BUTTON_SIZE) )    {
            
            if ( !S.m_dialog_queued )
            {
                S.m_dialog_queued       = true;
                m_saving                = true;
                S.m_dialog_settings     = {
                    /* type               = */  cb::FileDialog::Type::Save,
                    /* window_name        = */  "Open Testing Suite",
                    /* default_filename   = */  "functional_test.json",
                    /* required_extension = */  ".json",
                    /* valid_extensions   = */  {  },
                    /* starting_dir       = */  std::filesystem::current_path()
                };
            }
        }


        //      4.      LOAD DIALOGUE...
        this->label("Load From File:",              this->ms_SETTINGS_LABEL_WIDTH,      this->ms_SETTINGS_WIDGET_WIDTH);
        if ( ImGui::Button("Load", ms_SETTINGS_BUTTON_SIZE) )    {
            
            if ( !S.m_dialog_queued )
            {
                S.m_dialog_queued       = true;
                m_loading               = true;
                S.m_dialog_settings     = {
                    /* type               = */  cb::FileDialog::Type::Open,
                    /* window_name        = */  "Save Testing Suite",
                    /* default_filename   = */  "",
                    /* required_extension = */  "",
                    /* valid_extensions   = */  {".json", ".cbjson", ".txt"},
                    /* starting_dir       = */  std::filesystem::current_path()
                };
            }
        }
    }
    
    
    
    return;
}


//  "_draw_input_blocker"
//
void ActionComposer::_draw_input_blocker(void)
{
    static constexpr ImGuiWindowFlags       FLAGS       = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav;// | ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGuiViewport *                         vp          = ImGui::GetMainViewport();
    
    ImGui::SetNextWindowPos(vp->Pos);
    ImGui::SetNextWindowSize(vp->Size);
    ImGui::SetNextWindowViewport(vp->ID);
    ImGui::SetNextWindowFocus();

    ImGui::PushStyleVar(    ImGuiStyleVar_WindowBorderSize, 0.0f        );
    ImGui::PushStyleVar(    ImGuiStyleVar_WindowRounding,   0.0f        );
    ImGui::PushStyleColor(  ImGuiCol_WindowBg,              0x00000000  );               // fully transparent
    //
    ImGui::Begin("##ActionComposer_InputBlocker", nullptr, FLAGS);

        ImGui::InvisibleButton("##blocker_btn", vp->Size);  //  One giant invisible button consumes all clicks...

    ImGui::End();
    //
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
    return;
}


//  "_refresh_monitor_cache"
//
//      Update m_active_monitor / m_monitor_bounds when the cursor enters a new
//      monitor.  Expects |global| = screen-space cursor coordinates.
//
void ActionComposer::_refresh_monitor_cache(ImVec2 global)
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

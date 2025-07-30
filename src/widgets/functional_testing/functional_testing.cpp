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
    app::WinInfo &      win_info    = *m_detview_window;
    
    if ( this->m_initialized )          { return; }
    
    
    win_info                        = {
        "Functional Testing",
        ImGuiWindowFlags_None | ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY,
        true,
        nullptr
    };
    
    this->m_initialized             = true;
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
    this->_update_mouse_capture();
    this->_update_key_capture();
    
    
    //  2.  DRAW THE "CONTROL-BAR" UI-INTERFACE...
    this->_draw_controlbar();
    
    
    //  3.  DRAW THE "BROWSER" UI-INTERFACE...
    this->draw_all();
    
    
    //  4.  DRIVE EXECUTION OF THE ACTION COMPOSITION...
    this->_drive_execution();
    this->_draw_overlay();
    this->_draw_renderer_visuals();
    
    
    //  5.  QUERY FILE DIALOG MENUS...
    if (busy) {
        this->_file_dialog_handler();
    }
    
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
        _save_actions_to_comp();                       // persist current comp
        m_compositions.emplace_back();                 // push default
        int new_idx = static_cast<int>(m_compositions.size()) - 1;
        _load_actions_from_comp(new_idx);              // select & reset state
    }
    
    ImGui::SameLine();
    
    ImGui::BeginDisabled(m_compositions.size() < 1);
        if ( ImGui::Button("- Delete") )
        {
            _save_actions_to_comp();                       // persist before erase
            m_compositions.erase(m_compositions.begin() + m_comp_sel);
            int next = std::clamp( m_comp_sel, 0, static_cast<int>(m_compositions.size()) - 1 );
            _load_actions_from_comp(next);
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
            _save_actions_to_comp();       // save previous selection
            _load_actions_from_comp(i);    // switch to new selection
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
    if (!m_render_visuals || S.m_glfw_window == nullptr)
        return;

    if (m_sel < 0 || m_sel >= static_cast<int>(m_actions->size()))
        return;

    const Action& act = (*m_actions)[m_sel];
    if (act.type != ActionType::CursorMove &&
        act.type != ActionType::MouseDrag)
        return;

    /* choose the two points we need to mark */
    ImVec2 local_a{}, local_b{};
    if (act.type == ActionType::CursorMove) {
        local_a = act.cursor.first;
        local_b = act.cursor.last;
    } else {                                // MouseDrag
        local_a = act.drag.from;
        local_b = act.drag.to;
    }

    /* convert to GLOBAL screen coords */
    int wx{}, wy{};
    glfwGetWindowPos(S.m_glfw_window, &wx, &wy);
    ImVec2 global_a = { local_a.x + wx, local_a.y + wy };
    ImVec2 global_b = { local_b.x + wx, local_b.y + wy };

    /* helper: find viewport that contains point */
    auto viewport_for = [](ImVec2 p) -> ImGuiViewport*
    {
        const ImGuiPlatformIO& pio = ImGui::GetPlatformIO();
        for (ImGuiViewport* vp : pio.Viewports)
            if (p.x >= vp->Pos.x && p.x < vp->Pos.x + vp->Size.x &&
                p.y >= vp->Pos.y && p.y < vp->Pos.y + vp->Size.y)
                return vp;
        return ImGui::GetMainViewport();     // fallback
    };

    ImGuiViewport* vp_a = viewport_for(global_a);
    ImGuiViewport* vp_b = viewport_for(global_b);

    auto draw_rect = [](ImDrawList* dl, ImVec2 center)
    {
        const float HALF = ms_VIS_RECT_HALF;
        ImVec2 p_min = { center.x - HALF, center.y - HALF };
        ImVec2 p_max = { center.x + HALF, center.y + HALF };
        dl->AddRect(p_min, p_max, ms_VIS_COLOR, 0.0f, 0, ms_VIS_THICK);
    };

    /* draw on each relevant viewport’s foreground list */
    ImDrawList* dl_a = ImGui::GetForegroundDrawList(vp_a);
    ImDrawList* dl_b = (vp_b == vp_a) ? dl_a
                                      : ImGui::GetForegroundDrawList(vp_b);

    /* connecting line (draw only once if both in same viewport) */
    dl_a->AddLine(global_a, global_b, ms_VIS_COLOR, ms_VIS_THICK);

    draw_rect(dl_a, global_a);
    if (dl_b != dl_a)   draw_rect(dl_b, global_b);
    else                draw_rect(dl_a, global_b);
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
bool ActionComposer::_begin_mouse_capture(Action & act, ImVec2 * destination)
{
    if (m_state == State::Run)  { return false; }
        
    /* detect current backend window under cursor ------------------------*/
    GLFWwindow *    hovered     = glfwGetCurrentContext();   // set by ImGui backend
    act.target                  = (hovered) ? hovered : S.m_glfw_window;
    m_m_capture_dest            = destination;
    m_state                     = State::MouseCapture;
    return true;
}


//  "_update_mouse_capture"
//
inline void ActionComposer::_update_mouse_capture(void)
{
    ImVec2          mpos        = ImVec2(-1.0f, -1.0f);
    ImGuiIO &       io          = ImGui::GetIO();
    
    if ( m_state != State::MouseCapture || m_m_capture_dest == nullptr )   { return; }


    //  ESC key ends capture...
    if ( ImGui::IsKeyPressed(ImGuiKey_Escape) )
    {
        m_state             = State::Idle;
        m_m_capture_dest    = nullptr;
        return;
    }

    // Query GLFW for window-relative cursor coordinates --------------------//
    mpos                        = this->get_cursor_pos();
    m_m_capture_dest->x         = static_cast<float>(mpos.x);
    m_m_capture_dest->y         = static_cast<float>(mpos.y);
    return;
}









// *************************************************************************** //
//      KEYBOARD INPUT CAPTURE...
// *************************************************************************** //

//  "_begin_key_capture"
//
bool ActionComposer::_begin_key_capture(HotkeyParams * dest)
{
    if (m_state == State::Run)  { return false; }
    
    m_key_capture      = {};          // reset all
    m_key_capture.active = true;
    m_key_capture.dest   = dest;
    m_key_capture.backup = *dest;      // keep previous binding
    ImGui::SetNextFrameWantCaptureKeyboard(true);
    return true;
}


//  "_update_key_capture"
//
inline void ActionComposer::_update_key_capture(void)
{
    if (!m_key_capture.active)
        return;

    ImGuiIO& io = ImGui::GetIO();

    // Detect first key press (ignore repeats)
    for (ImGuiKey k = ImGuiKey_NamedKey_BEGIN; k < ImGuiKey_NamedKey_END; k = (ImGuiKey)(k + 1))
        if (ImGui::IsKeyPressed(k, false))
        {
            m_key_capture.key_current = k;
            m_key_capture.ctrl  = io.KeyCtrl;
            m_key_capture.shift = io.KeyShift;
            m_key_capture.alt   = io.KeyAlt;
            m_key_capture.super = io.KeySuper;
        }

    // Accept when that key goes UP or user hits Enter
    if ((m_key_capture.key_current != ImGuiKey_None &&
         !ImGui::IsKeyDown(m_key_capture.key_current)) ||
        ImGui::IsKeyPressed(ImGuiKey_Enter))
    {
        _accept_key_capture();
    }
    // Cancel on Esc
    else if (ImGui::IsKeyPressed(ImGuiKey_Escape))
    {
        _cancel_key_capture();
    }
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
    m_key_capture.active = false;
}


//  "_cancel_key_capture"
//
inline void ActionComposer::_cancel_key_capture(void)
{
    if (m_key_capture.dest)
        *m_key_capture.dest = m_key_capture.backup;
    m_key_capture.active = false;
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
static_assert( utl::has_from_json<ImVec2>::value,               "ImVec2 \"from_json\" NOT visible."                 );
static_assert( utl::has_to_json<ImVec2>::value,                 "ImVec2 \"to_json\" NOT visible."                   );
//
//      2.      CursorMoveParams.
static_assert( utl::has_from_json<CursorMoveParams>::value,     "CursorMoveParams \"from_json\" NOT visible."       );
static_assert( utl::has_to_json<CursorMoveParams>::value,       "CursorMoveParams \"to_json\" NOT visible."         );
//
//      3.      Action.
static_assert( utl::has_from_json<Action>::value,               "Action \"from_json\" NOT visible."                 );
static_assert( utl::has_to_json<Action>::value,                 "Action \"to_json\" NOT visible."                   );
//
//      4.      Composition_t.
static_assert( utl::has_from_json<Composition_t>::value,        "Composition_t \"from_json\" NOT visible."          );
static_assert( utl::has_to_json<Composition_t>::value,          "Composition_t \"to_json\" NOT visible."            );
               
               
               
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
{

    //  1.  SAVE DIALOGUE...
    if ( ImGui::Button("Save") )    {
        
        if ( !S.m_dialog_queued )
        {
            S.m_dialog_queued       = true;
            m_saving                = true;
            S.m_dialog_settings     = {
                /* type               = */  cb::FileDialog::Type::Save,
                /* window_name        = */  "Open Testing Suite",
                /* default_filename   = */  "functional_test",
                /* required_extension = */  ".json",
                /* valid_extensions   = */  {  },
                /* starting_dir       = */  std::filesystem::current_path()
            };
        }
    }


    ImGui::SameLine(0, 20);
    
    
    //  2.  LOAD DIALOGUE...
    if ( ImGui::Button("Load") )    {
        
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

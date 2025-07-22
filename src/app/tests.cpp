/***********************************************************************************
*
*       *********************************************************************
*       ****              T E S T S . C P P  ____  F I L E               ****
*       *********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      July 20, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "app/app.h"



namespace cb {  //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



namespace ft {  //     BEGINNING NAMESPACE "ft"...
// *************************************************************************** //
// *************************************************************************** //



// ──────────────────────────────────────────────────────────────
//  FunctionalTestRunner
//      • owns its own state
//      • called once per frame from the testing window
// ──────────────────────────────────────────────────────────────
struct FunctionalTestRunner {
    enum class Phase { Idle, Move, Down, Up };
    Phase       phase      = Phase::Idle;
    ImVec2      start_pos  {}, target_pos {};
    float       duration   = 0.0f, elapsed = 0.0f;
    GLFWwindow* window     = nullptr;

    // start a new script -------------------------------------------------------
    void start(GLFWwindow* win, ImVec2 target, float seconds)
    {
        window     = win;
        target_pos = target;
        duration   = std::max(0.001f, seconds);
        elapsed    = 0.0f;

        // **real HW position**
        double cx, cy;
        glfwGetCursorPos(window, &cx, &cy);
        start_pos = ImVec2(static_cast<float>(cx), static_cast<float>(cy));

        // sync ImGui so both views agree
        ImGui::GetIO().AddMousePosEvent(start_pos.x, start_pos.y);

        phase = Phase::Move;
    }

    // abort immediately --------------------------------------------------------
    void abort()
    {
        phase   = Phase::Idle;
        elapsed = 0.0f;
        // also resync ImGui with wherever the OS cursor sits now
        if (window) {
            double cx, cy;  glfwGetCursorPos(window, &cx, &cy);
            ImGui::GetIO().AddMousePosEvent(static_cast<float>(cx),
                                            static_cast<float>(cy));
        }
    }

    // one-frame update ---------------------------------------------------------
    void update()
    {
        if (phase == Phase::Idle || !window) return;

        ImGuiIO& io = ImGui::GetIO();
        elapsed += io.DeltaTime;

        ImVec2 desired = io.MousePos;        // default

        switch (phase) {
            case Phase::Move: {
                float t = std::clamp(elapsed / duration, 0.0f, 1.0f);
                desired = ImLerp(start_pos, target_pos, t);
                if (t >= 1.0f) { phase = Phase::Down; elapsed = 0.0f; }
                break;
            }
            case Phase::Down: io.AddMouseButtonEvent(ImGuiMouseButton_Left, true);
                              phase = Phase::Up;
                              break;
            case Phase::Up:   io.AddMouseButtonEvent(ImGuiMouseButton_Left, false);
                              phase = Phase::Idle;
                              break;
            default: break;
        }

        // Override hardware + ImGui each frame
        glfwSetCursorPos(window, desired.x, desired.y);
        io.AddMousePosEvent(desired.x, desired.y);
    }

    bool running() const { return phase != Phase::Idle; }
};





    
// *************************************************************************** //
//
//
//      0.      "ActionComposer" TYPES...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//      ACTION TYPE LAYER...
// *************************************************************************** //

//  "ComposerState"
//
enum class ComposerState : uint8_t {
    None = 0,
    Run,
    Capture,
    COUNT
};

static constexpr std::array<const char *, static_cast<size_t>(ComposerState::COUNT)>
DEF_COMPOSER_STATE_NAMES = {
    "None", "Running", "Capturing"
};



// *************************************************************************** //
//      ACTION TYPE LAYER...
// *************************************************************************** //

//  "ActionType"
//
enum class ActionType {
    CursorMove,
    Hotkey,
//
    COUNT
};


//  "DEF_ACTION_TYPE_NAMES"
static constexpr std::array<const char *, static_cast<size_t>( ActionType::COUNT )>
DEF_ACTION_TYPE_NAMES = {
    "CursorMove", "Hotkey"
};



// *************************************************************************** //
//      ACTION DATA LAYER...
// *************************************************************************** //

//  "CursorMoveParams"
//
struct CursorMoveParams {
    ImVec2              first           {};
    ImVec2              last            {};
    float               duration        {1.f};
};


//  "HotkeyParams"
//
struct HotkeyParams {
    ImGuiKey            key             {ImGuiKey_C};
    bool                ctrl            {true};
    bool                shift           {false};
    bool                alt             {false};
};


//  "Action"
//
struct Action {
    std::string         name            = "new action";
    std::string         descr           = "description...";
    ActionType          type            = ActionType::CursorMove;
//
    CursorMoveParams    cursor;
    HotkeyParams        hotkey;
//
    bool                enabled         = true;
};



// *************************************************************************** //
//      ACTION EXECUTION LAYER...
// *************************************************************************** //
    
//  "ExecutorState"
//
enum class ExecutionState : uint8_t {
    None = 0,
    Move,
    ButtonDown,
    ButtonUp,
    COUNT
};

static constexpr std::array<const char *, static_cast<size_t>(ExecutionState::COUNT)>
DEF_EXEC_STATE_NAMES = {
    "None", "Move", "Button Down", "Button Up"
};
    
    
//  "ActionExecutor"
//      Runs one primitive at a time.
//
struct ActionExecutor
{
    using                       State                   = ExecutionState;

    /*--------------------------------------------------------------------*/
    /*  data members                                                      */
    /*--------------------------------------------------------------------*/
    State                       m_state                 { State::None };
    ImVec2                      m_first_pos             {  };                /* starting mouse-cursor position   */
    ImVec2                      m_last_pos              {  };                /* destination mouse-cursor pos.    */
    float                       m_duration_s            { 0.0f };          /* total time for Move              */
    float                       m_elapsed_s             { 0.0f };          /* accumulated time                 */
    GLFWwindow *                m_window                { nullptr };       /* target OS window                 */

    static constexpr float      ms_MIN_DURATION_S       = 0.001f;

    /*--------------------------------------------------------------------*/
    /*  public API                                                        */
    /*--------------------------------------------------------------------*/

    //  "start_cursor_move"
    //
    void start_cursor_move(GLFWwindow* window,
                                           ImVec2      first,
                                           ImVec2      last,
                                           float       duration_s)
    {
        m_window      = window;
        m_first_pos   = first;
        m_last_pos    = last;
        m_duration_s  = std::max(duration_s, ms_MIN_DURATION_S);
        m_elapsed_s   = 0.0f;
        m_state       = State::Move;
    }


    //  "start_button_action"
    //
    void start_button_action(GLFWwindow* window,
                                             ImGuiKey   key,
                                             bool       with_ctrl,
                                             bool       with_shift,
                                             bool       with_alt)
    {
        m_window = window;

        ImGuiIO& io = ImGui::GetIO();
        if (with_ctrl)  io.AddKeyEvent(ImGuiKey_LeftCtrl,  true);
        if (with_shift) io.AddKeyEvent(ImGuiKey_LeftShift, true);
        if (with_alt)   io.AddKeyEvent(ImGuiKey_LeftAlt,   true);

        io.AddKeyEvent(key, true);
        io.AddKeyEvent(key, false);

        if (with_ctrl)  io.AddKeyEvent(ImGuiKey_LeftCtrl,  false);
        if (with_shift) io.AddKeyEvent(ImGuiKey_LeftShift, false);
        if (with_alt)   io.AddKeyEvent(ImGuiKey_LeftAlt,   false);

        m_state = State::None;            /* completes immediately                */
    }


    //  "abort"
    //
    void abort()
    {
        m_state = State::None;
    }


    //  "busy"
    //
    bool busy() const
    {
        return m_state != State::None;
    }


    //  "update"
    //
    void update()
    {
        if (m_state == State::None || m_window == nullptr)
            return;

        ImGuiIO& io = ImGui::GetIO();

        switch (m_state)
        {
            case State::Move:
            {
                m_elapsed_s += io.DeltaTime;
                float t = std::clamp(m_elapsed_s / m_duration_s, 0.0f, 1.0f);
                ImVec2 pos = ImLerp(m_first_pos, m_last_pos, t);

                glfwSetCursorPos(m_window, pos.x, pos.y);
                io.AddMousePosEvent(pos.x, pos.y);

                if (t >= 1.0f)
                    m_state = State::ButtonDown;
            } break;

            case State::ButtonDown:
                io.AddMouseButtonEvent(ImGuiMouseButton_Left, true);
                m_state = State::ButtonUp;
                break;

            case State::ButtonUp:
                io.AddMouseButtonEvent(ImGuiMouseButton_Left, false);
                m_state = State::None;
                break;

            default:  /* fall-through to satisfy compiler */
                break;
        }
    }



};




// *************************************************************************** //
//                                  COMPOSITION TYPES...
// *************************************************************************** //
    
//  "Composition_t"
//
struct Composition_t {
//
    std::string             name        { "New Composition" };
    std::vector<Action>     actions;
};






// ─────────────────────────────────────────────────────────────
//  ActionComposer  –  minimal two-pane “browser” for test steps
// ─────────────────────────────────────────────────────────────

class ActionComposer
{
public:
    // *************************************************************************** //
    //  0.1.                            TYPENAME ALIASES...
    // *************************************************************************** //
    using                               State                               = ComposerState;
    using                               Composition                         = Composition_t;
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //  0.2.                            CONSTANTS...
    // *************************************************************************** //
    //
    //                              MISC. CONSTANTS:
    static constexpr ImVec2             ms_OVERLAY_OFFSET                   = ImVec2( 20.0f, 20.0f );
    //
    //                              WIDGET CONSTANTS:
    static constexpr size_t             ms_ACTION_NAME_LIMIT                = 64ULL;
    static constexpr size_t             ms_ACTION_DESCRIPTION_LIMIT         = 512ULL;
    //
    //                              INDIVIDUAL WIDGET DIMENSIONS:
    static constexpr float              ms_DESCRIPTION_FIELD_HEIGHT         = 60.0f;
    static constexpr ImVec2             ms_DELETE_BUTTON_DIMS               = ImVec2( 18.0f, 0.0f );
    //
    //                              UI BROWSER DIMENSIONS:
    static constexpr float              ms_SETTINGS_LABEL_WIDTH             = 150.0f;
    static constexpr float              ms_SETTINGS_WIDGET_WIDTH            = 300.0f;
    //
    static constexpr float              ms_COMPOSITION_COLUMN_WIDTH         = 340.0f;
    static constexpr float              ms_SELECTOR_COLUMN_WIDTH            = 340.0f;
    static constexpr const char *       ms_DRAG_DROP_HANDLE                 = "=";
    static constexpr const char *       ms_DELETE_BUTTON_HANDLE             = "-";
    //
    static constexpr float              ms_CONTROLBAR_SELECTABLE_SEP        = 16.0f;    //  controlbar offset.
    static constexpr float              ms_TOOLBAR_SELECTABLE_SEP           = 16.0f;    //  sep for  "+ Add",  "Prev",  etc...
    static constexpr float              ms_BROWSER_SELECTABLE_SEP           = 16.0f;    //  offset for buttons ontop of each selectable
    //
    //                              ARRAYS:
    static constexpr auto &             ms_COMPOSER_STATE_NAMES             = DEF_COMPOSER_STATE_NAMES;
    static constexpr auto &             ms_ACTION_TYPE_NAMES                = DEF_ACTION_TYPE_NAMES;
    static constexpr auto &             ms_EXEC_STATE_NAMES                 = DEF_EXEC_STATE_NAMES;
  
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //  0.3.                            DATA MEMBERS...
    // *************************************************************************** //
protected:
    //                              IMPORTANT DATA:
    GLFWwindow *                        m_glfw_window;
    ActionExecutor                      m_executor                          {  };
    std::vector<Composition>            m_compositions                      { 1 };
    std::vector<Action>                 m_actions;
    //
    int                                 m_comp_sel                          = 0;            // current composition selection
    int                                 m_sel                               = -1;           // current selection
    int                                 m_play_index                        = -1;           // current action being executed, -1 = idle
    //
    //
    //                              STATE:
    State                               m_state                             = State::None;
    bool                                m_is_running                        = false;
    bool                                m_step_req                          = false;
    bool                                m_show_overlay                      = true;
    bool                                m_capture_is_active                 = false;        //  < true while “Auto” sampling.
    //
    //
    //                              UTILITY:
    ImGuiTextFilter                     m_filter;
    ImVec2 *                            m_capture_dest                      = nullptr;      //  < pointer to coord being written
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END DATA MEMBERS.




    
// *************************************************************************** //
//
//
//  1.              MAIN PUBLIC API FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
public:
    
    //  Default Constructor.
    //
    ActionComposer(GLFWwindow * window) : m_glfw_window(window) {  }
    
    
    //  "Begin"
    //
    void Begin(void)
    {
        //  1.  DRAW THE "CONTROL-BAR" UI-INTERFACE...
        this->_update_capture();
        this->_draw_controlbar();
        
        //  2.  DRAW THE "BROWSER" UI-INTERFACE...
        this->draw_all();
        
        //  3.  DRIVE EXECUTION OF THE ACTION COMPOSITION...
        this->_drive_execution();
        this->_draw_overlay();
        
        return;
    }
    
    
    //  "draw_all"
    //
    void draw_all(void)
    {
        //  1.  DRAW THE TOP-MOST "COMPOSITION" BAR...
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1);
        ImGui::BeginChild("##ActionComposed_TestSelector", {ms_COMPOSITION_COLUMN_WIDTH, 0.0f}, ImGuiChildFlags_Borders);
            this->_draw_test_selector();
        ImGui::EndChild();
        ImGui::PopStyleVar();


        ImGui::SameLine();
        
        
        //  2.  DRAW THE "BROWSER" UI-INTERFACE...
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1);
        //
        //
            ImGui::BeginChild("##ActionComposer_Action_Selector",  {ms_SELECTOR_COLUMN_WIDTH, 0}, ImGuiChildFlags_Borders);
                this->_draw_action_selector();
            ImGui::EndChild();
            
            ImGui::SameLine();
            
            ImGui::BeginChild("##ActionComposer_Action_Inspector", {0,0},     ImGuiChildFlags_Borders);
                this->_draw_action_inspector();
            ImGui::EndChild();
        //
        //
        ImGui::PopStyleVar();
    
    
    
        return;
    }
    
    
    
//
//
// *************************************************************************** //
// *************************************************************************** //   END PUBLIC API.

    
    
// *************************************************************************** //
//
//
//  3.              PROTECTED MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
    // *************************************************************************** //
    //                                  MAIN UI FUNCTIONS...
    // *************************************************************************** //

    //  "_load_actions_from_comp"
    //
    void _load_actions_from_comp(void)
    {
        return;
    }


    //  "_save_actions_to_comp"
    //
    void _save_actions_to_comp(void)
    {
        return;
    }


    //  "_draw_test_selector"
    //
    void _draw_test_selector(void)
    {
        // add / remove
        if ( ImGui::Button("+ Add Composition") ) {
            m_compositions.emplace_back();
            m_comp_sel = (int)m_compositions.size() - 1;
            _load_actions_from_comp();
        }
        ImGui::SameLine();
        ImGui::BeginDisabled(m_compositions.size() <= 1);
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
                /* store previous comp, then load new */
                _save_actions_to_comp();
                m_comp_sel = i;
                _load_actions_from_comp();
            }
            ImGui::PopID();
        }

        // rename current
        ImGui::InputText("Name", &m_compositions[m_comp_sel].name);
        return;
    }
    
    
    //  "_draw_action_selector"
    //
    void _draw_action_selector(void)
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
            m_actions.emplace_back();
            m_sel = static_cast<int>(m_actions.size()) - 1;
        }
        
        ImGui::SameLine();
        
        ImGui::BeginDisabled(m_sel <= 0);
            if ( ImGui::Button("Prev") )        { m_sel = std::max(m_sel - 1, 0); }
        ImGui::EndDisabled();
        
        ImGui::SameLine(0.0f, ms_TOOLBAR_SELECTABLE_SEP);
        
        ImGui::BeginDisabled( m_sel < 0 || m_sel >= (int)m_actions.size() - 1 );
            if ( ImGui::Button("Next") )        { m_sel = std::min(m_sel + 1, (int)m_actions.size() - 1); }
        ImGui::EndDisabled();

        ImGui::Separator();
        ImGui::Separator();
        
        
        this->_draw_selector_table();
        
        
        return;
    }
    
    
    //  "_draw_selector_table"
    //
    inline void _draw_selector_table(void)
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

            clip.Begin(static_cast<int>(m_actions.size()));
            while ( clip.Step() )
            {
                for (int i = clip.DisplayStart; i < clip.DisplayEnd; ++i)
                {
                    const bool  selected    = (m_sel == i);
                    
                    if ( !m_filter.PassFilter(m_actions[i].name.c_str()) )      { continue; }


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
                    if ( ImGui::Selectable(m_actions[i].name.c_str(), selected, SELECTABLE_FLAGS) )     { m_sel = i; }


                    //      3.3.    DELETE BUTTON...
                    ImGui::TableSetColumnIndex(2);
                    if ( utl::SmallCButton(this->ms_DELETE_BUTTON_HANDLE) ) //  if ( ImGui::SmallButton("X") )
                    {
                        m_actions.erase(m_actions.begin() + i);
                        if ( m_sel >= i )       { m_sel = std::max(m_sel - 1, (int)m_actions.size() - 1); }
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
    void _draw_action_inspector(void)
    {
        if ( m_sel < 0 || m_sel >= m_actions.size() )         { ImGui::TextDisabled("No Selection..."); return; }
        
        Action &        a           = m_actions[m_sel];
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
            if ( ImGui::InputTextMultiline("##Action_Description", &a.descr, { -FLT_MIN, ms_DESCRIPTION_FIELD_HEIGHT }) )
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
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //                                  SECONDARY UI FUNCTIONS...
    // *************************************************************************** //
    
    //  "_draw_controlbar"
    //
    void _draw_controlbar(void)
    {
        static constexpr const char *   uuid            = "##Editor_Controls_Columns";
        static constexpr int            NC              = 8;
        static ImGuiOldColumnFlags      COLUMN_FLAGS    = ImGuiOldColumnFlags_None;
        static ImVec2                   WIDGET_SIZE     = ImVec2( -1,  32 );
        static ImVec2                   BUTTON_SIZE     = ImVec2( 32,   WIDGET_SIZE.y );
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
            ImGui::BeginDisabled( m_actions.empty() );
                if ( !this->is_running() )
                {
                    if ( ImGui::Button("Run", WIDGET_SIZE) ) {
                        m_play_index        = (m_sel >= 0           ? m_sel             : 0);
                        m_is_running        = !m_actions.empty();
                        m_state             = (m_actions.empty())   ? State::None       : State::Run;
                    }
                }
                else
                {
                    if ( utl::CButton("Stop", 0xFF453AFF, WIDGET_SIZE) ) {
                        this->reset_all();
                    }
                }
            ImGui::EndDisabled();
        
        
        
            //  2.  STEP BUTTON...
            ImGui::NextColumn();        ImGui::NewLine();
            ImGui::BeginDisabled( m_is_running || (m_state == State::Run) || m_actions.size() < 2 );
                if ( ImGui::Button("Step", WIDGET_SIZE) )
                {
                    m_play_index    = (m_play_index + 1) % (int)m_actions.size();
                    m_sel           = m_play_index;          // highlight row that will run next
                }
            ImGui::EndDisabled();



            //  3.  OVERVIEW...
            ImGui::NextColumn();
            ImGui::TextDisabled("Overlay:");
            //
            ImGui::SetNextItemWidth( BUTTON_SIZE.x );
            ImGui::Checkbox("##ActionComposer_OverlayToggle",           &m_show_overlay);



            //  4.  INFO...
            ImGui::NextColumn();
            ImGui::TextDisabled("Info:");
            //
            if ( this->is_running() )       { ImGui::Text("running: %d / %zu", m_play_index + 1, m_actions.size()); }
            else                            { ImGui::Text("idle"); }
            
            



            
            //  5.  EMPTY SPACES FOR LATER...
            for (int i = ImGui::GetColumnIndex(); i < NC - 1; ++i) {
                ImGui::Dummy( ImVec2(0,0) );    ImGui::NextColumn();
            }


            //  X.  MOUSE COORDINATES...
            ImGui::TextDisabled("Mouse Position:");
            ImVec2 mpos = ImGui::GetMousePos();
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
    void _draw_toolbar(void)
    {


        return;
    }
    
    
    //  "_draw_overlay"
    //
    void _draw_overlay(void)
    {
        static constexpr ImGuiWindowFlags       flags           = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
        ImVec2                                  cursor_pos      = ImGui::GetMousePos();
        ImVec2                                  win_pos         = cursor_pos + ms_OVERLAY_OFFSET;
    
    
        if ( !m_show_overlay )                                          { return; }
        if ( glfwGetWindowAttrib(m_glfw_window, GLFW_FOCUSED) == 0 )    { return; }


        ImGui::SetNextWindowBgAlpha(0.5f);   // semi-transparent
        ImGui::SetNextWindowPos(win_pos, ImGuiCond_Always);
            if ( ImGui::Begin("##ac_overlay", nullptr, flags) )
            {
                this->_dispatch_overlay_content();
            }
        ImGui::End();
        return;
    }
    
    
    //  "_dispatch_overlay_content"
    inline void _dispatch_overlay_content(void)
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
    inline void _overlay_ui_none(void)
    {
        static constexpr const char *   FMT_STRING              = "State: %s. \t Mouse: (%.0f, %.0f)";
        //
        static const char *             state_str               = nullptr;
        static State                    state_cache             = static_cast<State>( static_cast<int>(this->m_state) - 1 );
        const ImVec2                    mpos                    = ImGui::GetMousePos();
                
        if ( state_cache != this->m_state ) {
            state_cache     = static_cast<State>( this->m_state );
            state_str       = ms_COMPOSER_STATE_NAMES[ static_cast<size_t>(this->m_state) ];
        }
        
        ImGui::Text(FMT_STRING, state_str, mpos.x, mpos.y);
        ImGui::Separator();
        
        return;
    }
    
    //  "_overlay_ui_run"
    inline void _overlay_ui_run(void)
    {
        ImGui::TextUnformatted("Running test... (press ESC to CANCEL)");
        return;
    }
    
    //  "_overlay_ui_capture"
    inline void _overlay_ui_capture(void)
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
    void _drive_execution(void)
    {
        /* advance current primitive */
        m_executor.update();

        /* ready for next action? */
        if ( !m_executor.busy() && this->is_running() )
        {
            if ( m_play_index < 0 || m_play_index >= static_cast<int>(m_actions.size()) ) {
                m_state         = State::None;
                m_is_running    = false;
                m_step_req      = false;
                m_play_index    = -1;
                return;
            }

            Action& act = m_actions[m_play_index];
            this->_dispatch_execution(act);


            /* prepare for next index */
            ++m_play_index;
            if ( this->is_running() && m_play_index >= static_cast<int>(m_actions.size()) ) {
                m_state         = State::None;
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
    inline void _dispatch_execution(Action & act)
    {
        
        switch (act.type)
        {
            //  1.  CURSOR MOVEMENT...
            case ActionType::CursorMove: {
                double cx, cy;
                glfwGetCursorPos(m_glfw_window, &cx, &cy);

                m_executor.start_cursor_move(
                    m_glfw_window,
                    ImVec2(static_cast<float>(cx), static_cast<float>(cy)),
                    act.cursor.last,
                    act.cursor.duration
                );
                break;
            }

            //  2.  HOTKEY PRESS...
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

            //  3.  DEFAULT...
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
    //                                  ACTION-UI FUNCTIONS...
    // *************************************************************************** //

    //  "_dispatch_action_ui"
    inline void _dispatch_action_ui(Action & a)
    {
        
        switch(a.type)
        {
            case ActionType::CursorMove:        { this->_ui_cursor_move(a); break;       }
            case ActionType::Hotkey:            { this->_ui_cursor_move(a); break;       }
            default:                            { break; }
        }
        
        return;
    }
        
        
    //  "_ui_cursor_move"
    inline void _ui_cursor_move(Action & a)
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
    
    
    //  "_ui_hotkey"
    inline void _ui_hotkey(Action & a)
    {
        ImGui::Text(        "Key: %d",      a.hotkey.key);      //  Quick placeholder
        //
        ImGui::Checkbox(    "Ctrl",         &a.hotkey.ctrl);    ImGui::SameLine();
        ImGui::Checkbox(    "Shift",        &a.hotkey.shift);   ImGui::SameLine();
        ImGui::Checkbox(    "Alt",          &a.hotkey.alt);
        
        return;
    }
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END PROTECTED FUNCTIONS.




    
// *************************************************************************** //
//
//
//  4.              UTILITY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    //  "label"
    inline void                 label                               (const char * text)
    { utl::LeftLabel(text, this->ms_SETTINGS_LABEL_WIDTH, this->ms_SETTINGS_WIDGET_WIDTH); ImGui::SameLine(); };
    
    
    //  "is_running"
    inline bool                 is_running                          (void) const    { return ( this->m_is_running  &&  (this->m_state == State::Run) ); }
    
    
    //  "reset_state"
    inline void                 reset_state                         (void)          {
        m_is_running            = false;
        m_capture_is_active     = false;
        m_state                 = State::None;
        return;
    }
    
    //  "reset_data"
    inline void                 reset_data                         (void)          {
        m_sel                   = -1;
        m_play_index            = -1;
        m_capture_dest          = nullptr;
        m_executor.abort();
        return;
    }
    
    
    //  "reset_all"
    inline void                 reset_all                         (void)
    { this->reset_state();    this->reset_data(); }
    
    
    //  "_reorder"
    void                        _reorder                            (int from, int to)
    {
        if( from == to )    { return; }
        
        Action      tmp         = std::move(m_actions[from]);
        m_actions.erase( m_actions.begin() + from );
        m_actions.insert( m_actions.begin() + to, std::move(tmp) );
        m_sel                   = to;
        
        return;
    }
    
    
    //  "_begin_cursor_capture"
    inline bool                 _begin_cursor_capture               (ImVec2 * destination)
    {
        if (m_state == State::Run)  { return false; }
        
        m_state             = State::Capture;
        m_capture_dest      = destination;
        return true;
    }
    
    
    //  "_update_capture"
    inline void                 _update_capture                     (void)
    {
        ImGuiIO &   io      = ImGui::GetIO();
        
        if ( m_state != State::Capture || m_capture_dest == nullptr )   { return; }


        //  ESC key ends capture...
        if ( ImGui::IsKeyPressed(ImGuiKey_Escape) )
        {
            m_state             = State::None;
            m_capture_dest      = nullptr;
            return;
        }

        //  live-update destination with current cursor position */
        *m_capture_dest     = io.MousePos;
        return;
    }


    
//
//
// *************************************************************************** //
// *************************************************************************** //   END UTILITY FUNCTIONS...



// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
};//    END "ActionComposer" CLASS DEFINITION.









// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "ft" NAMESPACE.



























// *************************************************************************** //
//
//
//
//      1.      MAIN FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "BeginFunctionalTesting"
//
void App::BeginFunctionalTesting([[maybe_unused]] const char * uuid, [[maybe_unused]] bool * p_open, [[maybe_unused]] ImGuiWindowFlags flags)
{
    using namespace                     ft;
    static ActionComposer               composer            (S.m_glfw_window);
    
    
    //  MAIN WINDOW FOR FUNCTIONAL TESTING OPERATIONS...
    //
    ImGui::Begin(uuid, p_open, flags);
    //
    //
        composer.Begin();          // draws the new browser
    //
    //
    ImGui::End();



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

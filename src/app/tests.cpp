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




// ─────────────────────────────────────────────────────────────
//  ActionComposer  –  minimal two-pane “browser” for test steps
// ─────────────────────────────────────────────────────────────

struct ActionComposer
{
public:
    // *************************************************************************** //
    //  0.1.                            TYPENAME ALIASES...
    // *************************************************************************** //
        //using                           Clipboard                       = EditorIMPL::Clipboard                 ;
        
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //  0.2.                            CONSTANTS...
    // *************************************************************************** //
    //                              INDIVIDUAL WIDGET CONSTANTS:
    static constexpr size_t             ms_ACTION_NAME_LIMIT                = 64ULL;
    static constexpr size_t             ms_ACTION_DESCRIPTION_LIMIT         = 512ULL;
    //
    //                              INDIVIDUAL WIDGET DIMENSIONS:
    static constexpr float              ms_SETTINGS_LABEL_WIDTH             = 196.0f;
    static constexpr float              ms_SETTINGS_WIDGET_WIDTH            = 256.0f;
    static constexpr float              ms_DESCRIPTION_FIELD_HEIGHT         = 60.0f;
    //
    //                              UI BROWSER DIMENSIONS:
    static constexpr float              ms_SELECTOR_COLUMN_WIDTH            = 340.0f;
    static constexpr float              ms_BROWSER_SELECTABLE_SEP           = 16.0f;
    static constexpr float              ms_TOOLBAR_SELECTABLE_SEP           = 16.0f;
    //
    //                              ARRAYS:
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
    ActionExecutor                      m_executor                      {};
    std::vector<Action>                 m_actions;
    int                                 m_sel                           = -1;           // current selection
    int                                 m_play_index                    = -1;           // current action being executed, -1 = idle
    //
    //                              STATE:
    bool                                m_is_running                    = false;
    bool                                m_step_req                      = false;
    //
    //                              UTILITY:
    ImGuiTextFilter                     m_filter;
    
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
        this->_draw_controlbar();
        
        
        //  2.  DRAW THE "BROWSER" UI-INTERFACE...
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1);
        //
        //
            ImGui::BeginChild("##SeqLeft",  {ms_SELECTOR_COLUMN_WIDTH, 0}, ImGuiChildFlags_Borders);
                _draw_selector();
            ImGui::EndChild();
            
            ImGui::SameLine();
            
            ImGui::BeginChild("##SeqRight", {0,0},     ImGuiChildFlags_Borders);
                draw_inspector();
            ImGui::EndChild();
        //
        //
        ImGui::PopStyleVar();
        
        
        //  3.  DRIVE EXECUTION OF THE ACTION COMPOSITION...
        this->_drive_execution();
        
        
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

    //  "_draw_selector"
    //
    void _draw_selector(void)
    {
        ImGuiListClipper clip;
    
    
        //  1.  FILTER SEARCH BOX...
        //
        ImGui::SetNextItemWidth(-FLT_MIN);
        if ( ImGui::InputTextWithHint("##flt", "filter", m_filter.InputBuf, IM_ARRAYSIZE(m_filter.InputBuf)) )
            { m_filter.Build(); }
            
            
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
        
        ImGui::SameLine();
        
        ImGui::BeginDisabled( m_sel < 0 || m_sel >= (int)m_actions.size() - 1 );
            if ( ImGui::Button("Next") )        { m_sel = std::min(m_sel + 1, (int)m_actions.size() - 1); }
        ImGui::EndDisabled();


        //  ImGui::SameLine(0.0f, ms_BROWSER_SELECTABLE_SEP);
        //  ImGui::SameLine(0.0f, ms_BROWSER_SELECTABLE_SEP);
        ImGui::Separator();
        ImGui::Separator(); 
        
    
        //  3.  ARRAY OF EACH ACTION...
        //
        clip.Begin( static_cast<int>(m_actions.size()) );
        while ( clip.Step() )
        {
            for (int i = clip.DisplayStart; i < clip.DisplayEnd; ++i)
            {
                if ( !m_filter.PassFilter(m_actions[i].name.c_str()) )      { continue; }


                const bool      selected        = (m_sel == i);
                ImGui::PushID(i);



                //      3.1.    Handle for DRAG/DROP.
                ImGui::TextUnformatted("[=]");
                if ( ImGui::IsItemActive() && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoPreviewTooltip) )
                {
                    ImGui::SetDragDropPayload("ACTION_ROW", &i, sizeof i);
                    ImGui::EndDragDropSource();
                }
                
                if ( ImGui::BeginDragDropTarget() )
                {
                    if ( const ImGuiPayload * p = ImGui::AcceptDragDropPayload("ACTION_ROW") )      { _reorder(*(int*)p->Data, i); }
                    ImGui::EndDragDropTarget();
                }
                ImGui::SameLine();

                //      3.2.    Selectible Row.
                if ( ImGui::Selectable(m_actions[i].name.c_str(), selected, ImGuiSelectableFlags_SpanAllColumns) )  { m_sel = i; }
                ImGui::SameLine(0.0f, ms_BROWSER_SELECTABLE_SEP);

                //      3.3.    "Delete" Button.
                if ( ImGui::SmallButton("X") )
                {
                    m_actions.erase(m_actions.begin() + i);
                    if ( m_sel >= i )   { m_sel = std::max(m_sel - 1, (int)m_actions.size() - 1); }
                    ImGui::PopID(); break;                              // vector mutated
                }
                ImGui::PopID();
                
            }// END SELECTABLE ROW.
            
        }
        clip.End();
        
        
        return;
    }
    
    
    //  "draw_inspector"
    //
    void draw_inspector(void)
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
        //  1.  PLAY / PAUSE...
        ImGui::BeginDisabled( m_actions.empty() );
            if (!m_is_running)
            {
                if ( ImGui::Button("Run") ) {
                    m_play_index        = (m_sel >= 0 ? m_sel : 0);
                    m_is_running        = !m_actions.empty();
                }
            }
            else
            {
                if ( ImGui::Button("Stop") ) {
                    m_is_running        = false;
                    m_play_index        = -1;
                    m_executor.abort();
                }
            }
        ImGui::EndDisabled();
        
        
        ImGui::SameLine(0.0f, ms_TOOLBAR_SELECTABLE_SEP);


        //  2.  STEP BUTTON...
        ImGui::BeginDisabled(m_is_running || m_actions.empty());
            if ( ImGui::Button("Step") )
            {
                m_play_index    = (m_play_index + 1) % (int)m_actions.size();
                m_sel           = m_play_index;          // highlight row that will run next
            }
        ImGui::EndDisabled();


        //  3.  INFO...
        ImGui::SameLine(0.0f, ms_TOOLBAR_SELECTABLE_SEP);
        if ( m_is_running )     { ImGui::TextDisabled("running: %d / %zu", m_play_index + 1, m_actions.size()); }
        else                    { ImGui::TextDisabled("idle"); }
    
    
        ImGui::SameLine(0.0f, ms_TOOLBAR_SELECTABLE_SEP);
        
        
        //  4.  MOUSE COORDINATES...
        ImVec2 mpos = ImGui::GetMousePos();
        ImGui::Text("MousePos: (%.1f , %.1f)", mpos.x, mpos.y);     //  Live cursor read-out in the same units we feed to glfwSetCursorPos...


        return;
    }
    
    
    //  "_draw_toolbar"
    //
    void _draw_toolbar(void)
    {


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
        if (!m_executor.busy() && (m_is_running || m_step_req))
        {
            if (m_play_index < 0 || m_play_index >= static_cast<int>(m_actions.size())) {
                m_is_running = false;
                m_step_req   = false;
                m_play_index = -1;
                return;
            }

            Action& act = m_actions[m_play_index];
            this->_dispatch_execution(act);


            /* prepare for next index */
            ++m_play_index;
            if (m_is_running && m_play_index >= static_cast<int>(m_actions.size())) {
                m_is_running = false;
                m_play_index = -1;
            }
            m_step_req = false;
            m_sel      = m_play_index;
        }
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
    inline void _dispatch_action_ui(Action & a) {
        
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
        this->label("Init:");
        ImGui::DragFloat2   ("##init",          (float*)&a.cursor.first,        1,          0,          FLT_MAX,        "%.f");
        
        this->label("Final:");
        ImGui::DragFloat2   ("##final",         (float*)&a.cursor.last,         1,          0,          FLT_MAX,        "%.f");
        
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
    
    
    //  "_reorder"
    void _reorder(int from, int to)
    {
        if( from == to )    { return; }
        
        Action      tmp         = std::move(m_actions[from]);
        m_actions.erase(m_actions.begin()+from);
        m_actions.insert(m_actions.begin()+to,std::move(tmp));
        m_sel                   = to;
        
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

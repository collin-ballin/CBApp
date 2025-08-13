/***********************************************************************************
*
*       **************************************************************************
*       ****                A C T I O N . C P P  ____  F I L E                ****
*       **************************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      July 23, 2025.
*
**************************************************************************************
**************************************************************************************/
#include "widgets/functional_testing/functional_testing.h"
#include "app/state/state.h"
#include "app/state/_types.h"


namespace cb { namespace ui { //     BEGINNING NAMESPACE "cb::ui"...
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
        
        return;
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
        
        return;
    }

    // one-frame update ---------------------------------------------------------
    void update()
    {
        if ( phase == Phase::Idle || !window )      { return; }

        ImGuiIO &   io          = ImGui::GetIO();
        elapsed                += io.DeltaTime;
        ImVec2      desired     = io.MousePos;        // default


        switch (phase)
        {
            case Phase::Move: {
                float t = std::clamp(elapsed / duration, 0.0f, 1.0f);
                desired = ImLerp(start_pos, target_pos, t);
                if (t >= 1.0f) { phase = Phase::Down; elapsed = 0.0f; }
                break;
            }
            
            case Phase::Down: {
                io.AddMouseButtonEvent(ImGuiMouseButton_Left, true);
                phase = Phase::Up;
                break;
            }
            
            case Phase::Up: {
                io.AddMouseButtonEvent(ImGuiMouseButton_Left, false);
                phase = Phase::Idle;
                break;
            }
            
            default: { break; }
        }

        // Override hardware + ImGui each frame
        glfwSetCursorPos(window, desired.x, desired.y);
        io.AddMousePosEvent(desired.x, desired.y);
        return;
    }

    bool running() const { return phase != Phase::Idle; }
};












// *************************************************************************** //
//
//
//
//      1.      "ActionExecutor" IMPLEMENTATION...
// *************************************************************************** //
// *************************************************************************** //

//  "start_cursor_move"
//
void ActionExecutor::start_cursor_move(GLFWwindow* window, ImVec2 first, ImVec2 last, float duration_s)
{
    m_window        = window;
    m_first_pos     = first;
    m_last_pos      = last;
    m_duration_s    = std::max( static_cast<float>(1.0f / m_playback_speed.value) * duration_s, ms_MIN_DURATION_S);
    m_elapsed_s     = 0.0f;
    m_is_drag       = false;                    // ← pure move

    /* warp OS cursor (LOCAL) */
    glfwSetCursorPos(m_window, m_first_pos.x, m_first_pos.y);

    /* echo GLOBAL position to ImGui */
    ImVec2 global_start = _local_to_global(m_window, m_first_pos);
    ImGui::GetIO().AddMousePosEvent(global_start.x, global_start.y);

    m_state = State::Move;
}


//  "start_mouse_click"
//
void ActionExecutor::start_mouse_click(GLFWwindow * window, bool left_button)
{
    m_window        = window;
    ImGuiIO & io    = ImGui::GetIO();
    io.AddMouseButtonEvent(left_button ? 0 : 1, true);
    io.AddMouseButtonEvent(left_button ? 0 : 1, false);
    m_state         = State::None;
    return;
}


//  "start_mouse_press"
//
void ActionExecutor::start_mouse_press(GLFWwindow * window, bool left_button)
{
    m_window    = window;
    ImGui::GetIO().AddMouseButtonEvent(left_button ? 0 : 1, true);
    m_state     = State::None;
    return;
}


//  "start_mouse_release"
//
void ActionExecutor::start_mouse_release(GLFWwindow * window, bool left_button)
{
    m_window    = window;
    ImGui::GetIO().AddMouseButtonEvent(left_button ? 0 : 1, false);
    m_state     = State::None;
    return;
}


//  "start_mouse_drag"
//
void ActionExecutor::start_mouse_drag(GLFWwindow * window, ImVec2 from, ImVec2 to, float  dur_s, bool left_button)
{
    /* helper: inject a mouse-button edge even if ImGui already decided
       it isn’t accepting events this frame                                         */


    /* 0️⃣   House-keeping -------------------------------------------------- */
    m_window            = window;
    m_drag_button_left  = left_button;
    m_is_drag           = true;            // so update() knows to release later


    /* 1️⃣   Warp cursor to the **start** position (local coords) ---------- */
    glfwSetCursorPos(m_window, from.x, from.y);

    /*      Also send a global echo so Dear ImGui back-end is in sync       */
    {
        ImVec2 global_start = _local_to_global(m_window, from);
        ImGui::GetIO().AddMousePosEvent(global_start.x, global_start.y);
    }

    /* 2️⃣   Press & hold the requested button ----------------------------- */
    queue_mouse_button(left_button ? 0 : 1, /*down=*/true);

    /* 3️⃣   Prime the interpolation variables ----------------------------- */
    m_first_pos   = from;
    m_last_pos    = to;
    m_duration_s  = std::max(static_cast<float>(1.0f / m_playback_speed.value) * dur_s,
                              ms_MIN_DURATION_S);
    m_elapsed_s   = 0.0f;

    /*      The executor’s update() now runs in State::Move until finished  */
    m_state = State::Move;
}
   
   
   
   
//  "start_key_press"
//
void ActionExecutor::start_key_press([[maybe_unused]] GLFWwindow * win, ImGuiKey key, bool ctrl, bool shift, bool alt, bool super)
{
    m_window            = win;
    //  ImGuiIO &   io      = ImGui::GetIO();


    //  if ( key == ImGuiKey_None )                                         { return; }

    if ( ctrl   && !m_key_manager.IsActive(ImGuiKey_LeftCtrl)    )      { m_key_manager.Push(ImGuiKey_LeftCtrl);    }
    if ( shift  && !m_key_manager.IsActive(ImGuiKey_LeftShift)   )      { m_key_manager.Push(ImGuiKey_LeftShift);   }
    if ( alt    && !m_key_manager.IsActive(ImGuiKey_LeftAlt)     )      { m_key_manager.Push(ImGuiKey_LeftAlt);     }
    if ( super  && !m_key_manager.IsActive(ImGuiKey_LeftSuper)   )      { m_key_manager.Push(ImGuiKey_LeftSuper);   }
    //
    if ( !m_key_manager.IsActive(key) )                                 { m_key_manager.Push(key); }         // queue 1-frame edge → key is now held

    m_state = State::None;   // completes immediately
    return;
}
        
        
//  "start_key_release"
//
void ActionExecutor::start_key_release(GLFWwindow * win, ImGuiKey key, bool ctrl, bool shift, bool alt, bool super)
{
    m_window            = win;
    //  ImGuiIO &   io      = ImGui::GetIO();
    
    
    if ( m_key_manager.IsActive(key) )                                  { m_key_manager.Pop(key); }         // queue 1-frame edge → key is now held
    //
    if ( ctrl   &&  m_key_manager.IsActive(ImGuiKey_LeftCtrl)    )      { m_key_manager.Pop(ImGuiKey_LeftCtrl);     }
    if ( shift  &&  m_key_manager.IsActive(ImGuiKey_LeftShift)   )      { m_key_manager.Pop(ImGuiKey_LeftShift);    }
    if ( alt    &&  m_key_manager.IsActive(ImGuiKey_LeftAlt)     )      { m_key_manager.Pop(ImGuiKey_LeftAlt);      }
    if ( super  &&  m_key_manager.IsActive(ImGuiKey_LeftSuper)   )      { m_key_manager.Pop(ImGuiKey_LeftSuper);    }
    //
    if ( m_key_manager.IsActive(key) )                                  { m_key_manager.Pop(key); }         // queue 1-frame edge → key is now held

    m_state             = State::None;   // completes immediately
    return;
}


//  "start_button_action"
//
void ActionExecutor::start_button_action(GLFWwindow * window, ImGuiKey key, bool with_ctrl, bool with_shift, bool with_alt, bool with_super)
{
    m_window = window;
    ImGuiIO& io = ImGui::GetIO();

    if (with_ctrl)   { io.AddKeyEvent(ImGuiKey_LeftCtrl,  true);        io.AddKeyEvent(ImGuiKey_ReservedForModCtrl,  true);     }
    if (with_shift)  { io.AddKeyEvent(ImGuiKey_LeftShift, true);        io.AddKeyEvent(ImGuiKey_ReservedForModShift, true);     }
    if (with_alt)    { io.AddKeyEvent(ImGuiKey_LeftAlt,   true);        io.AddKeyEvent(ImGuiKey_ReservedForModAlt,   true);     }
    if (with_super)  { io.AddKeyEvent(ImGuiKey_LeftSuper, true);        io.AddKeyEvent(ImGuiKey_ReservedForModSuper, true);     }

    io.AddKeyEvent(key, true);
    io.AddKeyEvent(key, false);

    if (with_ctrl)   { io.AddKeyEvent(ImGuiKey_LeftCtrl,  false);       io.AddKeyEvent(ImGuiKey_ReservedForModCtrl,  false);    }
    if (with_shift)  { io.AddKeyEvent(ImGuiKey_LeftShift, false);       io.AddKeyEvent(ImGuiKey_ReservedForModShift, false);    }
    if (with_alt)    { io.AddKeyEvent(ImGuiKey_LeftAlt,   false);       io.AddKeyEvent(ImGuiKey_ReservedForModAlt,   false);    }
    if (with_super)  { io.AddKeyEvent(ImGuiKey_LeftSuper, false);       io.AddKeyEvent(ImGuiKey_ReservedForModSuper, false);    }

    m_state = State::None;   // completes immediately
}


//  "abort"
//
void ActionExecutor::abort(void)
{
    /* release a button that may still be held during a drag */
    if (m_state != State::None && m_is_drag)
    {
        ImGui::GetIO().AddMouseButtonEvent(m_drag_button_left ? 0 : 1, false);
    }

    /* reset local state */
    m_state     = State::None;
    m_is_drag   = false;
    m_window    = nullptr;
    this->reset();
    
    return;
}


//  "busy"
//
bool ActionExecutor::busy(void) const
{
    return m_state != State::None;
}


//  "update"
//
void ActionExecutor::update(void)
{
    if ( m_state == State::None || m_window == nullptr )    { return; }

    ImGuiIO &   io      = ImGui::GetIO();

    switch (m_state)
    {
        case State::Move: {
            m_elapsed_s += io.DeltaTime;
            float  t     = std::clamp( m_elapsed_s / m_duration_s, 0.0f, 1.0f);
            ImVec2 pos   = ImLerp(m_first_pos, m_last_pos, t);

            glfwSetCursorPos(m_window, pos.x, pos.y);                 // local
            ImVec2 global = _local_to_global(m_window, pos);
            io.AddMousePosEvent(global.x, global.y);                  // global echo

            if (t >= 1.0f)
            {
                if (m_is_drag)  m_state = State::ButtonUp;  // release only
                else            m_state = State::None;      // pure move done
            }
            break;
        }

        case State::ButtonDown: {
            io.AddMouseButtonEvent(ImGuiMouseButton_Left, true);
            m_state = State::ButtonUp;
            break;
        }
        
        case State::ButtonUp: {
            queue_mouse_button(m_drag_button_left ? 0 : 1, false);
            // io.AddMouseButtonEvent(m_drag_button_left ? 0 : 1, false);
            m_state = State::None;
            break;
        }

        default:  {/* fall-through to satisfy compiler */
            break;
        }
    }
    
    return;
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
//                                  GENERAL FUNCTIONS...
// *************************************************************************** //

//  "_drive_execution"
//
void ActionComposer::_drive_execution(void)
{
    /* === 0. global abort shortcut ===================================== */
    if ( m_state == State::Run && ImGui::IsKeyPressed(ImGuiKey_Escape) )
    {
        abort_test();
        //m_executor.abort();
        //reset_all();                    // idle, flags cleared
        return;                         // skip further processing this frame
    }

    /* === 1. advance current action ==================================== */
    m_executor.update();

    /* === 2. should we dispatch the next? ============================== */
    const bool  ready_to_start      = !m_executor.busy();
    const bool  want_action         = is_running() || need_step();
    if ( !ready_to_start || !want_action )      { return; }

    /* === 3. index bounds check ======================================== */
    if ( m_play_index < 0 || m_play_index >= static_cast<int>(m_actions->size()) )
    {
        this->exit_test();
        return;
    }

    /* === 4. run the indexed action ==================================== */
    Action &    act     = (*m_actions)[m_play_index];
    m_sel               = m_play_index;               // visuals stay in sync
    _dispatch_execution(act);


    /* === 5. advance or finish ========================================= */
    if ( need_step() )                           // Run‑Once
    {
        this->exit_test();
        //  m_step_req   = false;
        //  reset_state();
        //  m_play_index = -1;
    }
    else                                       // Run‑All
    {
        ++m_play_index;
        if (m_play_index >= static_cast<int>(m_actions->size()))
        {
            reset_state();
            m_play_index = -1;
        }
    }
}


//  "_dispatch_execution"
//
inline void ActionComposer::_dispatch_execution(Action & act)
{
    
    switch (act.type)
    {
        //  A1.     CURSOR MOVEMENT...
        case ActionType::CursorMove: {
            m_executor.start_cursor_move(
                act.target ? act.target : S.m_glfw_window,
                act.cursor.first,
                act.cursor.last,
                act.cursor.duration);
            break;
        }
        //
        //  A2.     SINGLE MOUSE CLICK...
        case ActionType::MouseClick: {
            m_executor.start_mouse_click(
                act.target ? act.target : S.m_glfw_window,
                act.click.left_button);
            break;
        }
        //
        //  A3.     MOUSE PRESS...
        case ActionType::MousePress: {
            m_executor.start_mouse_press(
                act.target ? act.target : S.m_glfw_window,
                act.press.left_button);
            break;
        }
        //
        //  A4.     MOUSE RELEASE...
        case ActionType::MouseRelease: {
            m_executor.start_mouse_release(
                act.target ? act.target : S.m_glfw_window,
                act.release.left_button);
            break;
        }
        //
        //  A5.     MOUSE DRAG...
        case ActionType::MouseDrag: {
            m_executor.start_mouse_drag(
                act.target ? act.target : S.m_glfw_window,
                act.cursor.first,
                act.cursor.last,
                act.cursor.duration,
                act.press.left_button);
            break;
        }
        //
        //
        //
        //  B1.     HOTKEY...
        case ActionType::Hotkey: {
            m_executor.start_button_action(
                act.target ? act.target : S.m_glfw_window,
                act.hotkey.key,
                act.hotkey.ctrl,
                act.hotkey.shift,
                act.hotkey.alt,
                act.hotkey.super);
            break;
        }
        //
        //  B2.     KEY PRESS...
        case ActionType::KeyPress: {
            m_executor.start_key_press(
                act.target ? act.target : S.m_glfw_window,
                act.hotkey.key, act.hotkey.ctrl, act.hotkey.shift,
                act.hotkey.alt, act.hotkey.super
            );
            break;
        }
        //
        //  B3.     KEY RELEASE...
        case ActionType::KeyRelease: {
            m_executor.start_key_release(
                act.target ? act.target : S.m_glfw_window,
                act.hotkey.key, act.hotkey.ctrl, act.hotkey.shift,
                act.hotkey.alt, act.hotkey.super
            );
            break;
        }
        //
        //
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
void ActionComposer::_dispatch_action_ui(Action & a)
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
        case ActionType::KeyPress:
        case ActionType::KeyRelease:
        case ActionType::Hotkey:            { this->_ui_hotkey(a);              break;       }
        default:                            { break; }
    }
    
    
    
    
    return;
}


//  "_ui_cursor_move"
//
inline void ActionComposer::_ui_cursor_move(Action & a)
{
    //  this->_ui_movement_widgets(a);
    this->_ui_movement_widgets_OLD(a);
    this->_ui_duration_widgets(a);
    
    return;
}


//  "_ui_mouse_click"
//
inline void ActionComposer::_ui_mouse_click(Action & a)
{
    label("Button:");
    int b = a.click.left_button ? 0 : 1;
    if ( ImGui::Combo("##btn", &b, ms_CLICK_PARAM_NAMES.data(), ms_CLICK_PARAM_NAMES.size()) )
        { a.click.left_button = (b == 0); }

    return;
}


//  "_ui_mouse_press"
//
inline void ActionComposer::_ui_mouse_press(Action & a)
{
    label("Button:");
    int b = a.press.left_button ? 0 : 1;
    if ( ImGui::Combo("##press_btn", &b, ms_CLICK_PARAM_NAMES.data(), ms_CLICK_PARAM_NAMES.size()) )
        { a.press.left_button = (b==0); }
}


//  "_ui_mouse_release"
//
inline void ActionComposer::_ui_mouse_release(Action & a)
{
    label("Button:");
    int b = a.release.left_button ? 0 : 1;
    if ( ImGui::Combo("##release_btn", &b, ms_CLICK_PARAM_NAMES.data(), ms_CLICK_PARAM_NAMES.size()) )
        { a.release.left_button = (b==0); }
}


//  "_ui_mouse_drag"
//
inline void ActionComposer::_ui_mouse_drag(Action & a)
{

    //      1.      Begin / End coordinate widgets (reuse CursorMove block)
    _ui_movement_widgets(a);


    _ui_duration_widgets(a);
    

    //      4.      Button chooser (Left / Right)
    label("Button:");
    int b = a.press.left_button ? 0 : 1;
    if ( ImGui::Combo("##press_btn", &b, ms_CLICK_PARAM_NAMES.data(), ms_CLICK_PARAM_NAMES.size()) )
        { a.press.left_button = (b==0); }
        
}


//  "_ui_hotkey"
//
inline void ActionComposer::_ui_hotkey(Action & a)
{
    static constexpr ImVec4     ENABLE_COLOR        = ImVec4(   0.196f,     0.843f,     0.294f,     1.000f      );
    static constexpr ImVec4     DISABLE_COLOR       = ImVec4(   0.596f,     0.596f,     0.616f,     1.000f      );
    auto                        check_button        = [&](const char * label, bool & value) -> void {
        if (value)      { if ( utl::SmallCButton(label, ENABLE_COLOR ) )       { value = false; }          }
        else            { if ( utl::SmallCButton(label, DISABLE_COLOR) )       { value = true;  }          }
    };
    //
    const bool                  recording           = ( m_key_capture.active && m_key_capture.dest == &a.hotkey );
    HotkeyParams                view                = ( recording )// show current / live key name
            ? HotkeyParams{ m_key_capture.key_current,  m_key_capture.ctrl,         m_key_capture.shift,
                            m_key_capture.alt,          m_key_capture.super }
            : a.hotkey;
    const char *                key_name            = ImGui::GetKeyName(view.key);



    if ( !key_name )    { key_name = this->ms_EMPTY_HOTKEY_NAME; }


    label("Key:");
    ImGui::Text("%s", key_name);
    ImGui::SameLine();
    ImGui::Text( "[%s%s%s%s]",
                  view.ctrl     ? "Ctrl "       : "",
                  view.shift    ? "Shift "      : "",
                  view.alt      ? "Alt "        : "",
                  view.super    ? "Super"       : ""
    );
    if ( !m_key_capture.active ) {
        ImGui::SameLine();
        if ( ImGui::SmallButton("Assign") )  {
            ImGui::SameLine();
            _begin_key_capture(&a.hotkey);
        }
    }


    label("Modifiers:");
    if ( recording ) {
        check_button            ("CTRL",        view.ctrl           );      ImGui::SameLine();
        check_button            ("shift",       view.shift          );      ImGui::SameLine();
        check_button            ("alt",         view.alt            );      ImGui::SameLine();
        check_button            ("super",       view.super          );
    }
    else {
        check_button            ("CTRL",        a.hotkey.ctrl       );      ImGui::SameLine();
        check_button            ("shift",       a.hotkey.shift      );      ImGui::SameLine();
        check_button            ("alt",         a.hotkey.alt        );      ImGui::SameLine();
        check_button            ("super",       a.hotkey.super      );
    }
    
        
    return;
}





// *************************************************************************** //
//
//
//      GENERIC UI FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "_ui_movement_widgets_OLD"
//
inline void ActionComposer::_ui_movement_widgets_OLD(Action & a)
{
    ImGui::PushStyleColor(  ImGuiCol_Text,                  ms_VIS_COLOR_A  );
    //
        ImGui::PushID("ActionComposed_CursorMove_Begin");
            this->label("Begin:");
            ImGui::DragFloat2   ("##init",          (float*)&a.cursor.first,        1,          0,          FLT_MAX,        "%.f");
        ImGui::PopStyleColor();
            ImGui::SameLine();
            if ( ImGui::SmallButton("assign") )         { _begin_mouse_capture(a,   &a.cursor.first_pos,    &a.cursor.first); }   // start capture for Begin
            ImGui::SameLine();
            if ( ImGui::SmallButton("swap") )           { a.swap(); }                                   // Swap the order of Begin and End...
    //
    ImGui::PopID();
    
    
    
    ImGui::PushStyleColor(  ImGuiCol_Text,                  ms_VIS_COLOR_B  );
    //
        ImGui::PushID("ActionComposed_CursorMove_End");
            this->label("End:");
            ImGui::DragFloat2   ("##final",         (float*)&a.cursor.last,         1,          0,          FLT_MAX,        "%.f");
        ImGui::PopStyleColor();
            ImGui::SameLine();
            if ( ImGui::SmallButton("assign") )         { _begin_mouse_capture(a,   &a.cursor.first_pos,    &a.cursor.last); }
    //
    ImGui::PopID();
    
  
    
    return;
}



//  "_ui_movement_widgets"
//
inline void ActionComposer::_ui_movement_widgets(Action & a)
{
    const ImGuiID       first_ID        = a.cursor.first_pos.widget.id;
    const ImGuiID       last_ID         = a.cursor.last_pos.widget.id;



    ImGui::PushStyleColor(  ImGuiCol_Text,                  ms_VIS_COLOR_A  );
    //
        ImGui::PushID("ActionComposed_CursorMove_NewBegin");
            this->label("Begin:");
            ImGui::DragFloat2   ("##init",          (float*)&a.cursor.first_pos.pos,        1,          0,          FLT_MAX,        "%.f");
        ImGui::PopStyleColor();
            ImGui::SameLine();
            if ( ImGui::SmallButton("assign") )         { _begin_mouse_capture(a,   &a.cursor.first_pos,    &a.cursor.first); }   // start capture for Begin
            ImGui::SameLine();
            if ( ImGui::SmallButton("swap") )           { a.swap(); }                                   // Swap the order of Begin and End...
    //
            this->label("UUID:");   ImGui::SameLine();
            ImGui::BeginDisabled( 0 > first_ID );
                ImGui::Text( "%d", first_ID );
            ImGui::EndDisabled();
    //
    ImGui::PopID();
    
    
    
    ImGui::PushStyleColor(  ImGuiCol_Text,                  ms_VIS_COLOR_B  );
    //
        ImGui::PushID("ActionComposed_CursorMove_NewEnd");
            this->label("End:");
            ImGui::DragFloat2   ("##final",         (float*)&a.cursor.last_pos.pos,         1,          0,          FLT_MAX,        "%.f");
        ImGui::PopStyleColor();
            ImGui::SameLine();
            if ( ImGui::SmallButton("assign") )         { _begin_mouse_capture(a,   &a.cursor.first_pos,    &a.cursor.last); }
    //
            this->label("UUID:");   ImGui::SameLine();
            ImGui::BeginDisabled( 0 > first_ID );
                ImGui::Text( "%d", last_ID );
            ImGui::EndDisabled();
    //
    ImGui::PopID();
    
    
    return;
}





//  "_ui_duration_widgets"
//
inline void ActionComposer::_ui_duration_widgets(Action & a)
{
    this->label("Duration:");
    ImGui::DragFloat    ("##ActionComposer_Duration",      &a.cursor.duration,             0.05f,      0.0f,       10,             "%.2f sec");
    
    return;
}







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

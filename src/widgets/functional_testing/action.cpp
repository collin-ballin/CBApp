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
    m_duration_s    = std::max(duration_s, ms_MIN_DURATION_S);
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
    start_cursor_move(window, from, to, dur_s);   // sets Move phase
    ImGui::GetIO().AddMouseButtonEvent(left_button ? 0 : 1, true);

    m_drag_button_left = left_button;
    m_is_drag          = true;                    // ← drag mode
    /* state already Move */
}
   
   
   
   
//  "start_key_press"
//
void ActionExecutor::start_key_press(GLFWwindow * win, ImGuiKey key, bool ctrl, bool shift, bool alt, bool super)
{
    m_window = win;
    ImGuiIO& io = ImGui::GetIO();

    if (ctrl)  { push_key_event(ImGuiKey_LeftCtrl,  true); io.KeyCtrl  = true; }
    if (shift) { push_key_event(ImGuiKey_LeftShift, true); io.KeyShift = true; }
    if (alt)   { push_key_event(ImGuiKey_LeftAlt,   true); io.KeyAlt   = true; }
    if (super) { push_key_event(ImGuiKey_LeftSuper, true); io.KeySuper = true; }

    push_key_event(key, true);            // key DOWN

    /* occupy the rest of this frame so ImGui sees the hold next frame */
    m_wait_one_frame = true;
    m_state = State::None;
}

   
//  "start_key_release"
//
void ActionExecutor::start_key_release(GLFWwindow * win, ImGuiKey key, bool ctrl, bool shift, bool alt, bool super)
{
    m_window = win;
    ImGuiIO& io = ImGui::GetIO();

    push_key_event(key, false);           // key UP

    if (ctrl)  { push_key_event(ImGuiKey_LeftCtrl,  false); io.KeyCtrl  = false; }
    if (shift) { push_key_event(ImGuiKey_LeftShift, false); io.KeyShift = false; }
    if (alt)   { push_key_event(ImGuiKey_LeftAlt,   false); io.KeyAlt   = false; }
    if (super) { push_key_event(ImGuiKey_LeftSuper, false); io.KeySuper = false; }

    m_wait_one_frame = true;                  // one frame so release is flushed
    m_state = State::None;
}


//  "start_button_action"
//
void ActionExecutor::start_button_action(GLFWwindow * window, ImGuiKey key, bool with_ctrl, bool with_shift, bool with_alt, bool with_super)
{
    m_window = window;
    ImGuiIO& io = ImGui::GetIO();

    if (with_ctrl)   io.AddKeyEvent(ImGuiKey_LeftCtrl,  true);
    if (with_shift)  io.AddKeyEvent(ImGuiKey_LeftShift, true);
    if (with_alt)    io.AddKeyEvent(ImGuiKey_LeftAlt,   true);
    if (with_super)  io.AddKeyEvent(ImGuiKey_LeftSuper, true);

    io.AddKeyEvent(key, true);
    io.AddKeyEvent(key, false);

    if (with_ctrl)   io.AddKeyEvent(ImGuiKey_LeftCtrl,  false);
    if (with_shift)  io.AddKeyEvent(ImGuiKey_LeftShift, false);
    if (with_alt)    io.AddKeyEvent(ImGuiKey_LeftAlt,   false);
    if (with_super)  io.AddKeyEvent(ImGuiKey_LeftSuper, false);

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
            float  t     = std::clamp(m_elapsed_s / m_duration_s, 0.0f, 1.0f);
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
            io.AddMouseButtonEvent(m_drag_button_left ? 0 : 1, false);
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
        m_executor.abort();
        reset_all();                    // idle, flags cleared
        return;                         // skip further processing this frame
    }

    /* === 1. advance current action ==================================== */
    m_executor.update();

    /* === 2. should we dispatch the next? ============================== */
    const bool  ready_to_start      = !m_executor.busy();
    const bool  want_action         = is_running() || need_step();
    if (!ready_to_start || !want_action)
        return;

    /* === 3. index bounds check ======================================== */
    if ( m_play_index < 0 || m_play_index >= static_cast<int>(m_actions->size()) )
    {
        reset_state();
        m_play_index = -1;
        return;
    }

    /* === 4. run the indexed action ==================================== */
    Action &    act     = (*m_actions)[m_play_index];
    m_sel               = m_play_index;               // visuals stay in sync
    _dispatch_execution(act);

    /* === 5. advance or finish ========================================= */
    if ( need_step() )                           // Run‑Once
    {
        m_step_req   = false;
        reset_state();
        m_play_index = -1;
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
                act.drag.from,
                act.drag.to,
                act.drag.duration,
                act.drag.left_button);
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
        }
        //
        //  B3.     KEY RELEASE...
        case ActionType::KeyRelease: {
            m_executor.start_key_release(
                act.target ? act.target : S.m_glfw_window,
                act.hotkey.key, act.hotkey.ctrl, act.hotkey.shift,
                act.hotkey.alt, act.hotkey.super
            );
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

    this->label("Begin:");
    ImGui::PushID("ActionComposed_CursorMove_Begin");
        ImGui::DragFloat2   ("##init",          (float*)&a.cursor.first,        1,          0,          FLT_MAX,        "%.f");
        ImGui::SameLine();
        if ( ImGui::SmallButton("auto") )           { _begin_mouse_capture(a, &a.cursor.first); }   // start capture for Begin
        ImGui::SameLine();
        if ( ImGui::SmallButton("swap") )           { a.swap(); }                                   // Swap the order of Begin and End...
    ImGui::PopID();
    
    
    this->label("End:");
    ImGui::PushID("ActionComposed_CursorMove_End");
        ImGui::DragFloat2   ("##final",         (float*)&a.cursor.last,         1,          0,          FLT_MAX,        "%.f");
        ImGui::SameLine();
        if ( ImGui::SmallButton("auto") )           { _begin_mouse_capture(a, &a.cursor.last); }        // start capture for End...
        ImGui::SameLine();
        if ( ImGui::SmallButton("swap") )           { a.swap(); }                                   // Swap the order of Begin and End...
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
    static constexpr int    NUM_CLICKS      = ms_CLICK_PARAM_NAMES.size();
    
    label("Begin:");
    ImGui::DragFloat2("##drag_from", (float*)&a.drag.from, 1.f, 0.f, FLT_MAX, "%.0f");
    ImGui::SameLine();
    if ( ImGui::SmallButton("Auto##drag_from") )        { _begin_mouse_capture(a, &a.drag.from); }


    label("End:");
    ImGui::DragFloat2("##drag_to", (float*)&a.drag.to, 1.f, 0.f, FLT_MAX, "%.0f");
    ImGui::SameLine();
    if ( ImGui::SmallButton("Auto##drag_to") )          { _begin_mouse_capture(a, &a.drag.to); }


    label("Duration:");
    ImGui::DragFloat("##drag_dur", &a.drag.duration, 0.05f, 0.f, 10.f, "%.2f s");


    label("Button:");
    int             b           = (a.drag.left_button) ? 0 : 1;
    if ( ImGui::Combo("##drag_btn", &b, ms_CLICK_PARAM_NAMES.data(), NUM_CLICKS) )      { a.drag.left_button = (b == 0); }

    return;
}


//  "_ui_hotkey"
//
inline void ActionComposer::_ui_hotkey(Action & a)
{
    int         mode_idx        = 0;
    
    this->label("Mode:");
    switch (a.type)
    {
        case ActionType::Hotkey :           { mode_idx = 0;     break; }
        case ActionType::KeyPress :         { mode_idx = 1;     break; }
        case ActionType::KeyRelease :       { mode_idx = 2;     break; }
        default :                           { break; }
    }
    
    if ( ImGui::Combo("##ActionComposer_Hotkey_Mode", &mode_idx, ms_CLICK_TYPE_NAMES.data(), ms_CLICK_TYPE_NAMES.size()) ) {
        a.type = (mode_idx == 0) ? ActionType::Hotkey
               : (mode_idx == 1) ? ActionType::KeyPress
                                 : ActionType::KeyRelease;
    }



    // show current / live key name
    const HotkeyParams      view        = ( m_key_capture.active && m_key_capture.dest == &a.hotkey )
            ? HotkeyParams{ m_key_capture.key_current,
                            m_key_capture.ctrl,         m_key_capture.shift,
                            m_key_capture.alt,          m_key_capture.super }
            : a.hotkey;
    const char *            key_name    = ImGui::GetKeyName(view.key);



    if ( !key_name )    { key_name = this->ms_EMPTY_HOTKEY_NAME; }


    label("Key:");
    ImGui::Text("%s", key_name);
    if ( !m_key_capture.active )
    {
        ImGui::SameLine();
        if ( ImGui::SmallButton("Assign") )  {
            ImGui::SameLine();
            _begin_key_capture(&a.hotkey);
        }
    }


    label("Modifiers:");
    ImGui::Text( "[%s%s%s%s]",
                 view.ctrl   ? "Ctrl "       : "",
                 view.shift  ? "Shift "      : "",
                 view.alt    ? "Alt "        : "",
                 view.alt    ? "Super"       : "" );

    if ( m_key_capture.active && m_key_capture.dest == &a.hotkey )
        { ImGui::TextColored(ImVec4(1,1,0,1), "Press key, Enter=accept, Esc=cancel"); }
        
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

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
    m_window      = window;
    m_first_pos   = first;
    m_last_pos    = last;
    m_duration_s  = std::max(duration_s, ms_MIN_DURATION_S);
    m_elapsed_s   = 0.0f;

    /* warp OS cursor (LOCAL coords) */
    glfwSetCursorPos(m_window, m_first_pos.x, m_first_pos.y);

    /* feed ImGui GLOBAL coords so hover works */
    ImVec2 global_start = _local_to_global(m_window, m_first_pos);
    ImGui::GetIO().AddMousePosEvent(global_start.x, global_start.y);

    m_state = State::Move;
}


//  "start_mouse_press"
//
void ActionExecutor::start_mouse_press(bool left_button)
{
    ImGui::GetIO().AddMouseButtonEvent( left_button ? ImGuiMouseButton_Left : ImGuiMouseButton_Right, true );
    m_state = State::None;   // completes instantly
    return;
}


//  "start_mouse_release"
//
void ActionExecutor::start_mouse_release(bool left_button)
{
    ImGui::GetIO().AddMouseButtonEvent( left_button ? ImGuiMouseButton_Left : ImGuiMouseButton_Right, false );
    m_state = State::None;
    return;
}

    
    

//  "start_button_action"
//
void ActionExecutor::start_button_action(GLFWwindow* window,
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
void ActionExecutor::abort()
{
    m_state = State::None;
}


//  "busy"
//
bool ActionExecutor::busy() const
{
    return m_state != State::None;
}


//  "update"
//
void ActionExecutor::update()
{
    if ( m_state == State::None || m_window == nullptr )    { return; }

    ImGuiIO &   io      = ImGui::GetIO();

    switch (m_state)
    {
        case State::Move:
        {
            m_elapsed_s                += io.DeltaTime;
            float       t               = std::clamp(m_elapsed_s / m_duration_s, 0.0f, 1.0f);
            ImVec2      pos             = ImLerp(m_first_pos, m_last_pos, t);

            /* move OS cursor (LOCAL) */
            glfwSetCursorPos(m_window, pos.x, pos.y);

            /* echo GLOBAL position to ImGui */
            ImVec2      global_pos      = _local_to_global(m_window, pos);
            io.AddMousePosEvent(global_pos.x, global_pos.y);

            if (t >= 1.0f)      { m_state = State::ButtonDown; }
            break;
        }

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

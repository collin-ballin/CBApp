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
    ImVec2              p0              {};
    ImVec2              pf              {};
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
    
//
//
// *************************************************************************** //
// *************************************************************************** //   END ALIASES AND CONSTANTS.




    
// *************************************************************************** //
//
//
//  1.              MAIN PUBLIC API FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
public:
    
    //  "Begin"
    //
    void Begin(void)
    {
        this->_draw_controlbar();
        
        
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
        
        
        return;
    }
    
//
//
// *************************************************************************** //
// *************************************************************************** //   END PUBLIC API.




    
    
// *************************************************************************** //
//
//
//  2.              PROTECTED DATA MEMBERS...
// *************************************************************************** //
// *************************************************************************** //
protected:
    //                              IMPORTANT DATA:
    std::vector<Action>                 m_actions;
    int                                 m_sel                           = -1;           // current selection
    //
    //                              STATE:
    int                                 m_play_idx                      = -1;           // current action being executed, -1 = idle
    bool                                m_is_running                    = false;
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


        ImGui::SameLine(0.0f, ms_BROWSER_SELECTABLE_SEP);
        ImGui::SameLine(0.0f, ms_BROWSER_SELECTABLE_SEP);

        
    
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

        
        this->label("Name:");
        ImGui::InputText("##name", &a.name);
        
        this->label("Description:");
        ImGui::InputTextMultiline("##description", &a.descr, { -FLT_MIN, ms_DESCRIPTION_FIELD_HEIGHT });
        
        this->label("Type:");
        if ( ImGui::Combo("##type", &type_int, ms_ACTION_TYPE_NAMES.data(), static_cast<int>( ActionType::COUNT )) )
            { a.type    = static_cast<ActionType>(type_int); }


        ImGui::Separator();
        
        
        switch(a.type)
        {
            case ActionType::CursorMove:        { this->_ui_cursor_move(a); break;       }
            case ActionType::Hotkey:            { this->_ui_cursor_move(a); break;       }
            default:                            { break; }
        }
        
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
        if (!m_is_running)
        {
            if ( ImGui::Button("Run") )
            {
                m_play_idx      = (m_sel >= 0 ? m_sel : 0);   // start at selection or first
                m_is_running    = !m_actions.empty();
            }
        }
        else
        {
            if ( ImGui::Button("Stop") ) {
                m_is_running = false;
                m_play_idx   = -1;
            }
        }
        
        ImGui::SameLine(0.0f, ms_TOOLBAR_SELECTABLE_SEP);


        //  2.  STEP BUTTON...
        ImGui::BeginDisabled(m_is_running || m_actions.empty());
            if ( ImGui::Button("Step →") )
            {
                m_play_idx = (m_play_idx + 1) % (int)m_actions.size();
                m_sel      = m_play_idx;          // highlight row that will run next
            }
        ImGui::EndDisabled();


        //  3.  INFO...
        ImGui::SameLine(0.0f, ms_TOOLBAR_SELECTABLE_SEP);
        if ( m_is_running )     { ImGui::TextDisabled("running: %d / %zu", m_play_idx + 1, m_actions.size()); }
        else                    { ImGui::TextDisabled("idle"); }
    
    
        ImGui::SameLine(0.0f, ms_TOOLBAR_SELECTABLE_SEP);
        
        
        //  4.  MOUSE COORDINATES...
        ImVec2 mpos = ImGui::GetMousePos();
        ImGui::Text("MousePos: (%.1f , %.1f)", mpos.x, mpos.y);     //  Live cursor read-out in the same units we feed to glfwSetCursorPos...


        return;
    }
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //                                  ACTION-UI FUNCTIONS...
    // *************************************************************************** //
    
    //  "_ui_cursor_move"
    //
    void _ui_cursor_move(Action & a)
    {
        this->label("Init:");
        ImGui::DragFloat2   ("##init",          (float*)&a.cursor.p0,       1,          0,          FLT_MAX,        "%.f");
        
        this->label("Final:");
        ImGui::DragFloat2   ("##final",         (float*)&a.cursor.pf,       1,          0,          FLT_MAX,        "%.f");
        
        this->label("Duration:");
        ImGui::DragFloat    ("##duration",      &a.cursor.duration,         0.05f,      0.0f,       10,             "%.2f s");
        
        return;
    }
    
    
    //  "_ui_hotkey"
    //
    void _ui_hotkey(Action & a)
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
    static float                        duration_s          = 2.5f;
    static ActionComposer               composer;
    static FunctionalTestRunner         runner;          // from earlier
    
    
    
    ImGui::Begin(uuid, p_open, flags);
        
                
        //  ImGui::InputFloat("Move duration (s)", &duration_s, 0.1f, 1.0f, "%.2f");

        //  if (ImGui::Button("Run test: move && click"))
        //      runner.start(S.m_glfw_window, ImVec2(400.0f, 300.0f), duration_s);

        //  if (ImGui::IsKeyPressed(ImGuiKey_Escape))
        //      runner.abort();


        ImGui::TextUnformatted(runner.running() ? "Running… (Esc aborts)" : "Idle");
        
        
        composer.Begin();          // draws the new browser
        
    ImGui::End();


    /* drive the script */
    runner.update();
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
